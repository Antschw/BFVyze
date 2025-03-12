#ifndef SCREEN_CAPTURER_H
#define SCREEN_CAPTURER_H

#include <windows.h>
#include <memory>

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
}

#endif // SCREEN_CAPTURER_H
