#include "ipc/IPCManager.h"
#include <spdlog/spdlog.h>

#include <utility>
#include "ipc/ZMQMessageFormatter.h"

#include "screenshot/ScreenshotProcessor.h"
#include "screenshot/ScreenshotSaver.h"

namespace ipc {

IPCManager::IPCManager(std::string endpoint) : endpoint_(std::move(endpoint)) {
    if (!zmqClient_.connect(endpoint_)) {
        spdlog::error("IPCManager: Failed to connect to endpoint: {}", endpoint_);
    }
}

// ReSharper disable once CppParameterMayBeConst
bool IPCManager::sendBlackAndWhiteImage(HBITMAP hBitmap, const int width, const int height) {
    screenshot::ScreenshotProcessor::BITMAPINFO256 bmpInfo256{};
    const std::vector<BYTE>                        bwData =
            screenshot::ScreenshotProcessor::convertToBlackAndWhite(hBitmap, width, height, bmpInfo256, 128);
    if (bwData.empty()) {
        spdlog::error("IPCManager: Black and white conversion failed.");
        return false;
    }

    const std::vector<BYTE> bmpBuffer = screenshot::ScreenshotSaver::createBitmapBuffer(bmpInfo256, bwData);
    spdlog::info("IPCManager: BMP buffer created ({} bytes).", bmpBuffer.size());

    if (zmq::message_t message = ZMQMessageFormatter::formatBMPMessage(bmpBuffer);
        !zmqClient_.sendMessage(std::move(message))) {
        spdlog::error("IPCManager: Failed to send BMP buffer via ZeroMQ.");
        return false;
    }

    spdlog::info("IPCManager: BMP buffer sent successfully.");
    return true;
}

} // namespace ipc
