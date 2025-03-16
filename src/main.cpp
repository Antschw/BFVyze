#define WIN32_LEAN_AND_MEAN
#ifndef WIN32_WINNT
#define WIN32_WINNT 0x0601
#endif

#include <windows.h>
#include <shellapi.h>

#include <atomic>
#include <chrono>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <thread>
#include <zmq.hpp>
#include "core/CheaterCountManager.h"
#include "input/HotkeyManager.h"
#include "ipc/IPCManager.h"
#include "overlay/OverlayController.h"
#include "screenshot/ScreenshotCapturer.h"

#include "core/Constants.h"
#include "global/GlobalState.h"

#include "ipc/PythonBackendController.h"

std::atomic g_waitingForResponse{false};

/**
 * @brief Capture et envoie une capture d'écran via IPC.
 * @param ipcManager Instance d'IPCManager utilisée pour envoyer l'image.
 */
void captureAndSendScreenshot(ipc::IPCManager &ipcManager) {
    spdlog::info("Hotkey pressed: capturing screenshot...");
    g_waitingForResponse.store(true);
    const auto hBitmap = screenshot::ScreenshotCapturer::captureScreen();
    if (!hBitmap) {
        spdlog::error("Screen capture failed.");
        return;
    }
    const int screenWidth  = GetSystemMetrics(SM_CXSCREEN);
    if (const int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        ipcManager.sendBlackAndWhiteImage(&*hBitmap, screenWidth, screenHeight)) {
        spdlog::info("Screenshot sent successfully.");
    } else {
        spdlog::error("Screenshot sending failed.");
        g_waitingForResponse.store(false);
    }
}

/**
 * @brief Fonction qui exécute l'écoute ZeroMQ pour le nombre de cheaters.
 * @param cheaterManager Pointeur partagé vers le gestionnaire de cheater count.
 * @param running Référence à la variable de contrôle de l'exécution.
 */
void runCheaterListener(const std::shared_ptr<core::CheaterCountManager> &cheaterManager,
                        const std::atomic<bool>                          &running) {
    zmq::context_t context(1);
    zmq::socket_t  subscriber(context, zmq::socket_type::pull);

    // Set a longer timeout (e.g., 5000 milliseconds)
    int timeoutMs = 50000;
    subscriber.set(zmq::sockopt::rcvtimeo, timeoutMs);
    subscriber.connect("tcp://localhost:5556");
    spdlog::info("ZeroMQ listener started on port 5556 with timeout {} ms.", timeoutMs);

    while (running.load()) {
        zmq::message_t message;
        // If no message is received within the timeout, set an error and continue.
        if (!subscriber.recv(message, zmq::recv_flags::none)) {
            if (g_waitingForResponse.load()) {
                spdlog::error("Timeout waiting for message from Python backend.");
                if (cheaterManager) {
                    cheaterManager->setError("Timeout waiting for backend response.");
                }
                g_waitingForResponse.store(false);
            }
            continue;
        }
        // Process the received message…
        g_waitingForResponse.store(false);
        try {
            auto json_msg = nlohmann::json::parse(message.to_string());

            // Stocker l'OCR s'il existe
            if (cheaterManager && json_msg.contains("ocr_result")) {
                cheaterManager->setOCR(json_msg["ocr_result"].get<std::string>());
            }

            // Stocker l'erreur s'il y en a une
            if (json_msg.contains("error")) {
                auto err = json_msg["error"].get<std::string>();
                if (cheaterManager) {
                    cheaterManager->setError(err);
                }
                spdlog::error("Received error from Python: {}", err);
            }

            // Stocker le cheater_count s'il y en a un
            if (json_msg.contains("cheater_count")) {
                int count = json_msg["cheater_count"].get<int>();
                if (cheaterManager) {
                    cheaterManager->setCount(count);
                }
                spdlog::info("Received cheater count: {}", count);
            }
        } catch (const std::exception &e) {
            if (cheaterManager) {
                cheaterManager->setError(e.what());
            }
            spdlog::error("Error parsing JSON: {}", e.what());
        }
    }
}

/**
 * @brief Fonction qui exécute le pipeline de capture d'écran dans un thread.
 * @param running Référence à la variable de contrôle de l'exécution.
 */
void runScreenshotPipeline(const std::atomic<bool> &running) {
    const std::string zmqEndpoint = "tcp://localhost:5555";
    ipc::IPCManager   ipcManager(zmqEndpoint);
    // On utilise la touche VK_ADD pour déclencher la capture d'écran
    input::HotkeyManager hotkeyManager(VK_ADD);
    hotkeyManager.start([&ipcManager] {
        GlobalState::pipelineActive.store(true);
        GlobalState::scanInitiated.store(true); // Indicate that a scan has been started
        GlobalState::errorMessage = ""; // Reset error message
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
    // Create the Python backend controller with appropriate paths.
    ipc::PythonBackendController backendController(PYTHON_EXECUTABLE_PATH, PYTHON_SCRIPT_PATH);
    backendController.launchBackend();

    // Créer le gestionnaire partagé pour le nombre de cheaters
    auto cheaterManager = std::make_shared<core::CheaterCountManager>();

    // Variable atomique de contrôle pour signaler l'arrêt aux threads
    std::atomic appRunning{true};

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

    backendController.shutdownBackend();
}

int main() {
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
    spdlog::info("Starting BFVyze overlay application...");

    runApplication();

    return 0;
}
