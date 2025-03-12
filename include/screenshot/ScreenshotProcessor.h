#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include <windows.h>
#include <vector>

/**
 * @class ScreenshotProcessor
 * @brief Manages image processing to prepare the capture for OCR analysis.
 */
namespace screenshot {
    class ScreenshotProcessor {
    public:
        /**
         * @brief Structure containing a BITMAPINFOHEADER and a 256-color palette for an 8-bit image.
         */
        struct BITMAPINFO256 {
            BITMAPINFOHEADER bmiHeader;
            RGBQUAD bmiColors[256];
        };

        /**
        * @brief Converts a bitmap to an 8-bit grayscale image using a 256-color palette.
        * @param hBitmap Handle to the source bitmap.
        * @param width Width of the image.
        * @param height Height of the image.
        * @param bmpInfo256 BITMAPINFO256 structure that will be filled.
        * @return A vector containing the 8-bit pixel data.
        */
        static std::vector<BYTE> convertToGrayscale(HBITMAP hBitmap, int width, int height, BITMAPINFO256 &bmpInfo256);

        /**
        * @brief Converts a bitmap to a black and white image (binarization) to facilitate OCR analysis.
        *        Uses the original grayscale conversion and then applies a threshold.
        * @param hBitmap Handle to the source bitmap.
        * @param width Width of the image.
        * @param height Height of the image.
        * @param bmpInfo256 BITMAPINFO256 structure that will be filled during the grayscale conversion.
        * @param threshold Binarization threshold (default is 128).
        * @return A vector containing binary data (0 for black, 255 for white) for each pixel.
        */
        static std::vector<BYTE> convertToBlackAndWhite(HBITMAP hBitmap, int width, int height, BITMAPINFO256 &bmpInfo256, BYTE threshold = 128);
    };
}

#endif // IMAGE_PROCESSOR_H
