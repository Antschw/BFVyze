#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include <vector>
#include <windows.h>

/**
 * @class ScreenshotProcessor
 * @brief Manages image processing for OCR analysis.
 *
 * This class provides methods to convert a captured bitmap into an 8-bit grayscale image or a binary (black and white) image.
 * The processing always starts by cropping the source image to a predefined region before further conversion.
 */
namespace screenshot {

class ScreenshotProcessor {
public:
    /**
     * @brief Structure containing a BITMAPINFOHEADER and a 256-color palette for an 8-bit image.
     */
    struct BITMAPINFO256 {
        BITMAPINFOHEADER bmiHeader;
        RGBQUAD          bmiColors[256];
    };

    /**
     * @brief Converts a bitmap to an 8-bit grayscale image using a 256-color palette.
     *
     * The source bitmap is first cropped to a predefined region before conversion.
     * If the cropping rectangle exceeds the image bounds, an empty vector is returned.
     *
     * @param hBitmap Handle to the source bitmap.
     * @param width Original width of the source image.
     * @param height Original height of the source image.
     * @param bmpInfo256 BITMAPINFO256 structure that will be filled for the cropped image.
     * @return A vector containing the 8-bit pixel data of the cropped grayscale image.
     */
    static std::vector<BYTE> convertToGrayscale(HBITMAP hBitmap, int width, int height, BITMAPINFO256 &bmpInfo256);

    /**
     * @brief Converts a bitmap to a binary (black and white) image to facilitate OCR analysis.
     *
     * This method first converts the source bitmap (cropped to a predefined region) to grayscale,
     * then applies a threshold to produce a binary image.
     *
     * @param hBitmap Handle to the source bitmap.
     * @param width Original width of the source image.
     * @param height Original height of the source image.
     * @param bmpInfo256 BITMAPINFO256 structure that will be filled during the grayscale conversion.
     * @param threshold Binarization threshold (default is 128).
     * @return A vector containing binary data (0 for black, 255 for white) for each pixel of the cropped image.
     */
    static std::vector<BYTE> convertToBlackAndWhite(HBITMAP hBitmap, int width, int height, BITMAPINFO256 &bmpInfo256,
                                                    BYTE threshold = 128);
};

} // namespace screenshot

#endif // IMAGE_PROCESSOR_H
