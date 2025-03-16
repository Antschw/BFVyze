#include "screenshot/ScreenshotProcessor.h"
#include <spdlog/spdlog.h>
#include "core/Constants.h"
#include "screenshot/ScreenshotSaver.h"

// Define cropping constants (adjust these values to match your red rectangle)
static constexpr int CROP_X      = 0; // X-coordinate of the top-left corner
static constexpr int CROP_Y      = 0; // Y-coordinate of the top-left corner
static constexpr int CROP_WIDTH  = 1400; // Width of the crop area
static constexpr int CROP_HEIGHT = 500; // Height of the crop area

namespace screenshot {

// Helper function to crop a bitmap to a specified rectangle.
static HBITMAP cropBitmap(HBITMAP hSource, const int cropX, const int cropY, const int cropWidth, const int cropHeight) {
    // Get a device context for the screen.
    HDC hScreenDC = GetDC(nullptr);
    // Create a device context compatible with the screen.
    HDC hSourceDC = CreateCompatibleDC(hScreenDC);
    SelectObject(hSourceDC, hSource);

    // Create a new bitmap for the cropped area.
    HBITMAP hCropped = CreateCompatibleBitmap(hScreenDC, cropWidth, cropHeight);
    if (!hCropped) {
        spdlog::error("Failed to create compatible bitmap for cropping.");
        DeleteDC(hSourceDC);
        ReleaseDC(nullptr, hScreenDC);
        return nullptr;
    }

    // Create a destination DC for the cropped bitmap.
    HDC hDestDC = CreateCompatibleDC(hScreenDC);
    SelectObject(hDestDC, hCropped);

    // Copy the specified region from the source bitmap.
    if (!BitBlt(hDestDC, 0, 0, cropWidth, cropHeight, hSourceDC, cropX, cropY, SRCCOPY)) {
        spdlog::error("BitBlt failed during cropping. Error: {}", GetLastError());
        DeleteObject(hCropped);
        DeleteDC(hDestDC);
        DeleteDC(hSourceDC);
        ReleaseDC(nullptr, hScreenDC);
        return nullptr;
    }

    DeleteDC(hDestDC);
    DeleteDC(hSourceDC);
    ReleaseDC(nullptr, hScreenDC);
    return hCropped;
}

// Helper function to convert a cropped bitmap to grayscale.
// Assumes hCropped is already cropped to the desired region.
static std::vector<BYTE> convertCroppedBitmapToGrayscale(HBITMAP hCropped, int width, int height,
                                                         ScreenshotProcessor::BITMAPINFO256 &bmpInfo256) {
    // Prepare BITMAPINFO256 for an 8-bit grayscale image.
    bmpInfo256.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
    bmpInfo256.bmiHeader.biWidth         = width;
    bmpInfo256.bmiHeader.biHeight        = -height; // negative for top-down DIB
    bmpInfo256.bmiHeader.biPlanes        = 1;
    bmpInfo256.bmiHeader.biBitCount      = GRAYSCALE_BIT_DEPTH;
    bmpInfo256.bmiHeader.biCompression   = BI_RGB;
    bmpInfo256.bmiHeader.biSizeImage     = 0;
    bmpInfo256.bmiHeader.biXPelsPerMeter = 0;
    bmpInfo256.bmiHeader.biYPelsPerMeter = 0;
    bmpInfo256.bmiHeader.biClrUsed       = 256;
    bmpInfo256.bmiHeader.biClrImportant  = 256;

    for (int i = 0; i < 256; ++i) {
        bmpInfo256.bmiColors[i].rgbBlue     = i;
        bmpInfo256.bmiColors[i].rgbGreen    = i;
        bmpInfo256.bmiColors[i].rgbRed      = i;
        bmpInfo256.bmiColors[i].rgbReserved = 0;
    }

    std::vector<BYTE> pixelData(width * height * (GRAYSCALE_BIT_DEPTH / 8));
    spdlog::info("Retrieving grayscale data using GetDIBits for cropped image...");

    HDC hDC = GetDC(nullptr);
    if (!hDC) {
        spdlog::error("Failed to get DC for GetDIBits");
        return {};
    }

    const int result = GetDIBits(hDC, hCropped, 0, height, pixelData.data(),
                                 reinterpret_cast<BITMAPINFO *>(&bmpInfo256), DIB_RGB_COLORS);
    ReleaseDC(nullptr, hDC);

    if (result == 0) {
        spdlog::error("GetDIBits failed with error: {}", GetLastError());
        return {};
    }

    spdlog::info("Grayscale conversion executed successfully. Pixels extracted: {} (expected: {})",
                 pixelData.size(), width * height * (GRAYSCALE_BIT_DEPTH / 8));

    return pixelData;
}

std::vector<BYTE> ScreenshotProcessor::convertToGrayscale(HBITMAP hBitmap, int width, int height,
                                                          BITMAPINFO256 &bmpInfo256) {
    spdlog::info("Starting grayscale conversion for an image of size {}x{}", width, height);

    // Ensure the cropping rectangle is within the bounds of the original image.
    if (CROP_X + CROP_WIDTH > width || CROP_Y + CROP_HEIGHT > height) {
        spdlog::error("Cropping dimensions exceed image bounds ({}x{} vs crop at ({}, {}) {}x{}).",
                      width, height, CROP_X, CROP_Y, CROP_WIDTH, CROP_HEIGHT);
        return {};
    }

    // Crop the bitmap to the predefined region.
    HBITMAP hCropped = cropBitmap(hBitmap, CROP_X, CROP_Y, CROP_WIDTH, CROP_HEIGHT);
    if (!hCropped) {
        spdlog::error("Cropping failed.");
        return {};
    }

    // --- DEBUG: Save the cropped bitmap to a file to inspect the crop region ---
    {
        BITMAPINFO256 debugBmpInfo = {};
        // Note: La conversion en grayscale pour le debug. Vous pouvez utiliser convertCroppedBitmapToGrayscale directement.
        std::vector<BYTE> debugPixels = convertCroppedBitmapToGrayscale(hCropped, CROP_WIDTH, CROP_HEIGHT, debugBmpInfo);
        if (!debugPixels.empty()) {
            // Assurez-vous d'avoir une fonction de sauvegarde qui fonctionne et sauvegarde un BMP.
            screenshot::ScreenshotSaver::saveBitmapToFile("debug_cropped.bmp", debugBmpInfo, debugPixels);
            spdlog::info("Cropped debug image saved as debug_cropped.bmp");
        }
    }
    // --- End DEBUG

    // Use the cropped dimensions.
    int croppedWidth  = CROP_WIDTH;
    int croppedHeight = CROP_HEIGHT;

    // Convert the cropped bitmap to grayscale.
    std::vector<BYTE> pixelData = convertCroppedBitmapToGrayscale(hCropped, croppedWidth, croppedHeight, bmpInfo256);

    // Clean up the cropped bitmap.
    DeleteObject(hCropped);

    return pixelData;
}

std::vector<BYTE> ScreenshotProcessor::convertToBlackAndWhite(HBITMAP hBitmap, int width, int height,
                                                              BITMAPINFO256 &bmpInfo256, const BYTE threshold) {
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

} // namespace screenshot
