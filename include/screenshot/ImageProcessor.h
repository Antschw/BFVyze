#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include <windows.h>
#include <vector>

/**
 * @class ImageProcessor
 * @brief Handles image processing operations such as grayscale conversion.
 */
namespace screenshot {
    class ImageProcessor {
    public:
        /**
         * @brief Structure to be filled by BITMAPINFOHEADER and 256 colors for 8 bits image.
         */
        struct BITMAPINFO256 {
            BITMAPINFOHEADER bmiHeader;
            RGBQUAD bmiColors[256];
        };


        /**
         * @brief Converts a given bitmap to grayscale.
         * @param hBitmap Handle to the original bitmap.
         * @param width Image width.
         * @param height Image height.
         * @param bmpInfo256 Reference to a BITMAPINFO256 structure to be filled.
         * @return A vector containing grayscale pixel data.
         */
        static std::vector<BYTE> convertToGrayscale(HBITMAP hBitmap, int width, int height, BITMAPINFO256 &bmpInfo256);
    };
}

#endif // IMAGE_PROCESSOR_H
