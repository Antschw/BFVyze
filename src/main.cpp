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
#include <zmq.hpp>
#include <nlohmann/json.hpp> // Pour parser le JSON

/**
 * @brief Lance le serveur backend Python dans une nouvelle fenêtre.
 */
void launchPythonServer() {
    spdlog::info("Launching Python backend server...");
    int ret = std::system("start \"\" python.exe ../python/main.py");
    if (ret != 0) {
        spdlog::error("Failed to launch Python server (system call returned {}).", ret);
    }
}

/**
 * @brief Capture et envoie une capture d'écran via IPC.
 * @param ipcManager Instance d'IPCManager utilisée pour envoyer l'image.
 */
void captureAndSendScreenshot(ipc::IPCManager &ipcManager) {
    spdlog::info("Hotkey pressed: capturing screenshot...");
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
    }
}

/**
 * @brief Fonction qui exécute l'écoute ZeroMQ pour le nombre de cheaters.
 * @param manager Pointeur partagé vers le gestionnaire de cheater count.
 * @param running Référence à la variable de contrôle de l'exécution.
 */
void runCheaterListener(std::shared_ptr<core::CheaterCountManager> manager,
                          std::atomic<bool>& running) {
    zmq::context_t context(1);
    zmq::socket_t subscriber(context, zmq::socket_type::pull);
    subscriber.connect("tcp://localhost:5556");
    spdlog::info("ZeroMQ listener started on port 5556.");

    while (running.load()) {
        zmq::message_t message;
        // Bloquant avec timeout ou attente active selon vos besoins
        if (subscriber.recv(message, zmq::recv_flags::none)) {
            try {
                auto json_msg = nlohmann::json::parse(message.to_string());
                int count = json_msg["cheater_count"].get<int>();
                manager->setCount(count);
                spdlog::info("Received cheater count: {}", count);
            } catch (const std::exception& e) {
                spdlog::error("Error parsing cheater count JSON: {}", e.what());
            }
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
        captureAndSendScreenshot(ipcManager);
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