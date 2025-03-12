#include "screenshot/ScreenshotOrchestrator.h"
#include "screenshot/ScreenshotProcessor.h"
#include "screenshot/ScreenshotSaver.h"
#include "screenshot/ScreenshotCapturer.h"
#include <spdlog/spdlog.h>
#include <filesystem>

namespace screenshot {
    bool ScreenshotOrchestrator::captureAndSaveScreenshot(const std::string& filename) {
        spdlog::info("Starting full screenshot process...");

        if (filename.empty()) {
            spdlog::error("Filename is empty. Aborting screenshot.");
            return false;
        }

        // Capture the screen
        spdlog::info("Filename received: {}", filename);
        auto hBitmap = ScreenshotCapturer::captureScreen();
        if (!hBitmap) {
            spdlog::error("Screen capture failed.");
            return false;
        }

        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        // Convert to grayscale
        ScreenshotProcessor::BITMAPINFO256 bmpInfo256 = {};
        std::vector<BYTE> grayData = ScreenshotProcessor::convertToGrayscale(
            &*hBitmap, screenWidth, screenHeight, bmpInfo256);

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
        std::filesystem::path absolutePath = std::filesystem::absolute(filename);
        spdlog::info("Screenshot successfully saved to {}", absolutePath.string());

        return true;
    }
}
