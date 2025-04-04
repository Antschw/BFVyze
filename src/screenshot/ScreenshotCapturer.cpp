#include "screenshot/ScreenshotCapturer.h"
#include <spdlog/spdlog.h>

namespace screenshot {
ScreenshotCapturer::HBitmapPtr ScreenshotCapturer::captureScreen() {
    int screenWidth  = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    spdlog::info("Capturing screen: {}x{}", screenWidth, screenHeight);

    // ReSharper disable once CppLocalVariableMayBeConst
    HDC hScreenDC = GetDC(nullptr);
    if (!hScreenDC) {
        spdlog::error("Failed to get screen DC.");
        return {nullptr, DeleteObject};
    }

    // ReSharper disable once CppLocalVariableMayBeConst
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
    if (!hMemoryDC) {
        spdlog::error("Failed to create memory DC.");
        ReleaseDC(nullptr, hScreenDC);
        return {nullptr, DeleteObject};
    }

    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, screenWidth, screenHeight);
    if (!hBitmap) {
        spdlog::error("Failed to create compatible bitmap.");
        DeleteDC(hMemoryDC);
        ReleaseDC(nullptr, hScreenDC);
        return {nullptr, DeleteObject};
    }

    SelectObject(hMemoryDC, hBitmap);
    spdlog::info("Bitmap and memory DC created successfully.");

    if (!BitBlt(hMemoryDC, 0, 0, screenWidth, screenHeight, hScreenDC, 0, 0, SRCCOPY)) {
        spdlog::error("BitBlt failed with error: {}", GetLastError());
        DeleteObject(hBitmap);
        DeleteDC(hMemoryDC);
        ReleaseDC(nullptr, hScreenDC);
        return {nullptr, DeleteObject};
    }

    spdlog::info("Screen captured successfully.");

    DeleteDC(hMemoryDC);
    ReleaseDC(nullptr, hScreenDC);

    return {hBitmap, DeleteObject};
}
} // namespace screenshot
