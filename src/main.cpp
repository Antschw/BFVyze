#include "overlay/OverlayController.h"
#include "input/HotkeyManager.h"
#include "screenshot/ScreenshotCapturer.h"
#include "ipc/IPCManager.h"
#include "core/CheaterCountManager.h"
#include <thread>
#include <atomic>
#include <cstdlib>
#include <chrono>
#include <spdlog/spdlog.h>
#include <windows.h>
#include <shellapi.h>
#include <zmq.hpp>
#include <string>
#include <nlohmann/json.hpp>
#include "global/GlobalState.h"
#include <filesystem>

std::atomic<bool> g_waitingForResponse{false};


/**
 * @brief Lance le serveur backend Python dans une nouvelle fenêtre.
 */
void launchPythonServer() {
    std::string pythonExe = "python-3.13.2-embed-amd64\\pythonw.exe";
    std::string script    = "python-3.13.2-embed-amd64\\scripts\\main.py";

    // Vérif: est-ce que pythonExe existe ?
    if (!std::filesystem::exists(pythonExe)) {
        spdlog::error("Python executable not found at: {}", pythonExe);
        return;
    }
    if (!std::filesystem::exists(script)) {
        spdlog::error("Python script not found at: {}", script);
        return;
    }

    spdlog::info("Launching Python backend: {} {}", pythonExe, script);
    HINSTANCE hRes = ShellExecuteA(NULL, "open", pythonExe.c_str(), script.c_str(), NULL, SW_HIDE);
    intptr_t code = reinterpret_cast<intptr_t>(hRes);
    if (code <= 32) {
        spdlog::error("ShellExecute failed with code {}", code);
    } else {
        spdlog::info("ShellExecute success, Python backend should be starting.");
    }
}



/**
 * @brief Capture et envoie une capture d'écran via IPC.
 * @param ipcManager Instance d'IPCManager utilisée pour envoyer l'image.
 */
void captureAndSendScreenshot(ipc::IPCManager &ipcManager) {
    spdlog::info("Hotkey pressed: capturing screenshot...");
    g_waitingForResponse.store(true);
    auto hBitmap = screenshot::ScreenshotCapturer::captureScreen();
    if (!hBitmap) {
        spdlog::error("Screen capture failed.");
        return;
    }
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    if (ipcManager.sendBlackAndWhiteImage(&*hBitmap, screenWidth, screenHeight)) {
        spdlog::info("Screenshot sent successfully.");
    } else {
        spdlog::error("Screenshot sending failed.");
        g_waitingForResponse.store(false);
    }
}

/**
 * @brief Fonction qui exécute l'écoute ZeroMQ pour le nombre de cheaters.
 * @param manager Pointeur partagé vers le gestionnaire de cheater count.
 * @param running Référence à la variable de contrôle de l'exécution.
 */
void runCheaterListener(std::shared_ptr<core::CheaterCountManager> manager,
                        std::atomic<bool>& running)
{
    zmq::context_t context(1);
    zmq::socket_t subscriber(context, zmq::socket_type::pull);

    // On peut mettre un petit timeout pour pas bloquer trop longtemps
    int shortTimeoutMs = 200;
    subscriber.setsockopt(ZMQ_RCVTIMEO, &shortTimeoutMs, sizeof(shortTimeoutMs));
    subscriber.connect("tcp://localhost:5556");
    spdlog::info("ZeroMQ listener started on port 5556.");

    while (running.load()) {
        zmq::message_t message;
        if (!subscriber.recv(message, zmq::recv_flags::none)) {
            // On n'a rien reçu dans le délai shortTimeoutMs
            // => check si on attendait une réponse
            if (g_waitingForResponse.load()) {
                // On peut incrémenter un compteur de "combien de fois on n'a rien reçu"
                // et au bout d'un certain temps, on loggue l'erreur
                static int noResponseCounter = 0;
                noResponseCounter++;
                if (noResponseCounter > 10) { // par ex. 10 x 200ms = 2 secondes
                    spdlog::warn("Timeout waiting for message from Python backend (2s).");
                    g_waitingForResponse.store(false);
                    noResponseCounter = 0;
                }
            }
            continue;
        }
        // On a reçu quelque chose
        g_waitingForResponse.store(false);
        try {
            auto json_msg = nlohmann::json::parse(message.to_string());
            if (json_msg.contains("error")) {
                std::string err = json_msg["error"].get<std::string>();
                manager->setError(err);
                spdlog::error("Received error from Python: {}", err);
            } else if (json_msg.contains("cheater_count")) {
                int count = json_msg["cheater_count"].get<int>();
                manager->setCount(count);
                spdlog::info("Received cheater count: {}", count);
            }
        } catch (const std::exception& e) {
            manager->setError(e.what());
            spdlog::error("Error parsing cheater count JSON: {}", e.what());
        }
    }
}


/**
 * @brief Fonction qui exécute le pipeline de capture d'écran dans un thread.
 * @param running Référence à la variable de contrôle de l'exécution.
 */
void runScreenshotPipeline(std::atomic<bool>& running) {
    const std::string zmqEndpoint = "tcp://localhost:5555";
    ipc::IPCManager ipcManager(zmqEndpoint);
    // On utilise la touche VK_ADD pour déclencher la capture d'écran
    input::HotkeyManager hotkeyManager(VK_ADD);
    hotkeyManager.start([&ipcManager]() {
        GlobalState::pipelineActive.store(true);
        captureAndSendScreenshot(ipcManager);
        GlobalState::pipelineActive.store(false);
    });
    spdlog::info("Screenshot capture pipeline started. Press ESC to stop this pipeline.");
    while (running.load()) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            spdlog::info("ESC pressed. Stopping screenshot capture pipeline.");
            hotkeyManager.stop();
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

/**
 * @brief Fonction principale de l'application qui lance les threads et l'overlay.
 */
void runApplication() {
    // Lancer le serveur Python en début d'exécution
    launchPythonServer();

    // Créer le gestionnaire partagé pour le nombre de cheaters
    auto cheaterManager = std::make_shared<core::CheaterCountManager>();

    // Variable atomique de contrôle pour signaler l'arrêt aux threads
    std::atomic<bool> appRunning{true};

    // Lancer le thread pour l'écoute ZeroMQ
    std::thread zmqListener(runCheaterListener, cheaterManager, std::ref(appRunning));

    // Lancer le thread du pipeline de capture d'écran
    std::thread screenshotThread(runScreenshotPipeline, std::ref(appRunning));

    // Lancer l'overlay (cette fonction est bloquante)
    overlay::OverlayController overlayController(cheaterManager);
    overlayController.run();

    // Lorsque l'overlay se ferme, signaler aux threads de s'arrêter
    appRunning.store(false);

    if (screenshotThread.joinable())
        screenshotThread.join();
    if (zmqListener.joinable())
        zmqListener.join();
}

int main() {
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
    spdlog::info("Starting BFVyze overlay application...");

    runApplication();

    return 0;
}