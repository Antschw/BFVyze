#include <windows.h>
#include <thread>
#include "input/HotkeyManager.h"
#include "screenshot/ScreenshotCapturer.h"
#include "ipc/IPCManager.h"
#include <spdlog/spdlog.h>

// Callback triggered when the hotkey is pressed (ex: F9)
void captureAndSendScreenshot(ipc::IPCManager &ipcManager) {
    // Capture the screen only when hotkey is pressed
    auto hBitmap = screenshot::ScreenshotCapturer::captureScreen();
    if (!hBitmap) {
        spdlog::error("Screen capture failed.");
        return;
    }

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Send the processed (black and white) image via ZeroMQ
    if (ipcManager.sendBlackAndWhiteImage(&*hBitmap, screenWidth, screenHeight)) {
        spdlog::info("Screenshot sent successfully.");
    } else {
        spdlog::error("Screenshot sending failed.");
    }
}

int main() {
    // ZeroMQ endpoint to which the C++ sender connects (PUSH socket)
    const std::string zmqEndpoint = "tcp://localhost:5555";

    // Create IPCManager which handles sending the image via ZeroMQ
    ipc::IPCManager ipcManager(zmqEndpoint);

    // Create HotkeyManager with the hotkey (e.g., F9)
    input::HotkeyManager hotkeyManager(VK_ADD);

    // Start hotkey polling with a callback that triggers the capture and send process
    hotkeyManager.start([&ipcManager]() {
        spdlog::info("+ pressed: capturing screenshot...");
        captureAndSendScreenshot(ipcManager);
    });

    spdlog::info("Press F9 to capture and send screenshot. Press ESC to exit.");

    // Boucle principale qui attend la touche ESC pour quitter
    while (true) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            spdlog::info("ESC pressed. Exiting.");
            hotkeyManager.stop();
            break;
        }
        Sleep(100);
    }

    return 0;
}
