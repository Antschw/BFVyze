#ifndef SCREENSHOT_MANAGER_H
#define SCREENSHOT_MANAGER_H

#include <string>

/**
 * @class ScreenshotOrchestrator
 * @brief Manages the full screenshot process from capture to saving.
 */
namespace screenshot {
    class ScreenshotOrchestrator {
    public:
        /**
         * @brief Captures and saves a screenshot to the specified file.
         * If any step fails (capture, processing, or saving), the function logs the error and returns false.
         * @param filename The name of the file where the screenshot will be saved.
         * @return True if the screenshot was captured and saved successfully, false otherwise.
         */
        static bool captureAndSaveScreenshot(const std::string& filename);
    };
}

#endif // SCREENSHOT_MANAGER_H
