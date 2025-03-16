#include "screenshot/ScreenshotOrchestrator.h"
#include <filesystem>
#include <spdlog/spdlog.h>
#include "screenshot/ScreenshotCapturer.h"
#include "screenshot/ScreenshotProcessor.h"
#include "screenshot/ScreenshotSaver.h"

namespace screenshot {
bool ScreenshotOrchestrator::captureAndSaveScreenshot(const std::string &filename) {
    spdlog::info("Starting full screenshot process...");

    if (filename.empty()) {
        spdlog::error("Filename is empty. Aborting screenshot.");
        return false;
    }

    // Capture the screen
    spdlog::info("Filename received: {}", filename);
    const auto hBitmap = ScreenshotCapturer::captureScreen();
    if (!hBitmap) {
        spdlog::error("Screen capture failed.");
        return false;
    }

    const int screenWidth  = GetSystemMetrics(SM_CXSCREEN);
    const int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Convert to grayscale
    ScreenshotProcessor::BITMAPINFO256 bmpInfo256 = {};
    const std::vector<BYTE>            grayData =
            ScreenshotProcessor::convertToGrayscale(&*hBitmap, screenWidth, screenHeight, bmpInfo256);

    if (grayData.empty()) {
        spdlog::error("Grayscale conversion failed.");
        return false;
    }

    // Save the file
    if (!ScreenshotSaver::saveBitmapToFile(filename, bmpInfo256, grayData)) {
        spdlog::error("Saving screenshot to file failed.");
        return false;
    }

    // Convert relative path to absolute path
    const std::filesystem::path absolutePath = std::filesystem::absolute(filename);
    spdlog::info("Screenshot successfully saved to {}", absolutePath.string());

    return true;
}
} // namespace screenshot
