#include "screenshot/ScreenCapturer.h"
#include <spdlog/spdlog.h>

namespace screenshot {
    ScreenCapturer::HBitmapPtr ScreenCapturer::captureScreen() {
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        spdlog::info("Capturing screen: {}x{}", screenWidth, screenHeight);

        HDC hScreenDC = GetDC(nullptr);
        if (!hScreenDC) {
            spdlog::error("Failed to get screen DC.");
            return {nullptr, DeleteObject};
        }

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

        return HBitmapPtr(hBitmap, DeleteObject);
    }
}


