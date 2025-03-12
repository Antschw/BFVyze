#include "screenshot/ScreenshotProcessor.h"
#include "core/Constants.h"  // Contient la définition de GRAYSCALE_BIT_DEPTH (généralement 8)
#include <spdlog/spdlog.h>

namespace screenshot {

    std::vector<BYTE> ScreenshotProcessor::convertToGrayscale(HBITMAP hBitmap, int width, int height, BITMAPINFO256 &bmpInfo256) {
        spdlog::info("Starting grayscale conversion for an image of size {}x{}", width, height);

        bmpInfo256.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmpInfo256.bmiHeader.biWidth = width;
        bmpInfo256.bmiHeader.biHeight = -height;
        bmpInfo256.bmiHeader.biPlanes = 1;
        bmpInfo256.bmiHeader.biBitCount = GRAYSCALE_BIT_DEPTH;
        bmpInfo256.bmiHeader.biCompression = BI_RGB;
        bmpInfo256.bmiHeader.biSizeImage = 0;
        bmpInfo256.bmiHeader.biXPelsPerMeter = 0;
        bmpInfo256.bmiHeader.biYPelsPerMeter = 0;
        bmpInfo256.bmiHeader.biClrUsed = 256;
        bmpInfo256.bmiHeader.biClrImportant = 256;

        for (int i = 0; i < 256; ++i) {
            bmpInfo256.bmiColors[i].rgbBlue = i;
            bmpInfo256.bmiColors[i].rgbGreen = i;
            bmpInfo256.bmiColors[i].rgbRed = i;
            bmpInfo256.bmiColors[i].rgbReserved = 0;
        }

        std::vector<BYTE> pixelData(width * height * (GRAYSCALE_BIT_DEPTH / 8));

        spdlog::info("Retrieving grayscale data using GetDIBits...");

        HDC hDC = GetDC(nullptr);
        if (!hDC) {
            spdlog::error("Failed to get DC for GetDIBits");
            return {};
        }

        int result = GetDIBits(hDC, hBitmap, 0, height, pixelData.data(), reinterpret_cast<BITMAPINFO*>(&bmpInfo256), DIB_RGB_COLORS);
        ReleaseDC(nullptr, hDC);

        if (result == 0) {
            spdlog::error("GetDIBits failed with error: {}", GetLastError());
            return {};
        }

        spdlog::info("Grayscale conversion executed successfully. Pixels extracted: {} (expected: {})",
                     pixelData.size(), width * height * (GRAYSCALE_BIT_DEPTH / 8));

        return pixelData;
    }

    std::vector<BYTE> ScreenshotProcessor::convertToBlackAndWhite(HBITMAP hBitmap, int width, int height, BITMAPINFO256 &bmpInfo256, BYTE threshold) {
        spdlog::info("Starting black and white conversion for an image of size {}x{}", width, height);
        std::vector<BYTE> grayData = convertToGrayscale(hBitmap, width, height, bmpInfo256);

        if (grayData.empty()) {
            spdlog::error("Failed to retrieve grayscale data for black and white conversion.");
            return {};
        }

        for (auto &pixel : grayData) {
            pixel = (pixel >= threshold) ? 255 : 0;
        }

        spdlog::info("Black and white conversion completed. Total pixels processed: {}", grayData.size());
        return grayData;
    }
}
