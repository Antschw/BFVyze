#ifndef SCREEN_CAPTURER_H
#define SCREEN_CAPTURER_H

#include <memory>
#include <windows.h>

/**
 * @class ScreenshotCapturer
 * @brief Handles screen capture and returns an HBITMAP.
 */
namespace screenshot {
class ScreenshotCapturer {
public:
    /**
     * @brief Captures the entire screen and returns a managed bitmap handle.
     * @return A unique_ptr to HBITMAP, automatically released using DeleteObject.
     */
    using HBitmapPtr = std::unique_ptr<std::remove_pointer_t<HBITMAP>, decltype(&DeleteObject)>;
    static HBitmapPtr captureScreen();
};
} // namespace screenshot

#endif // SCREEN_CAPTURER_H
