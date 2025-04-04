#ifndef IMAGE_SAVER_H
#define IMAGE_SAVER_H

#include <screenshot/ScreenshotProcessor.h>
#include <string>
#include <vector>
#include <windows.h>

namespace screenshot {
class ScreenshotSaver {
public:
    /**
     * @brief Saves BMP image data in grayscale (256-color palette) to a file.
     * @param filename Full path of the output file.
     * @param bmpInfo256 BITMAPINFO256 structure containing the header and palette.
     * @param pixelData Raw pixel data.
     * @return True if the operation succeeded, false otherwise.
     */
    static bool saveBitmapToFile(const std::string &filename, const ScreenshotProcessor::BITMAPINFO256 &bmpInfo256,
                                 const std::vector<BYTE> &pixelData);

    /**
     * @brief Creates an in-memory buffer containing a complete BMP file.
     *        This buffer can be directly transmitted in RAM (ideal for OCR via Python).
     * @param bmpInfo256 BITMAPINFO256 structure containing the header and palette.
     * @param pixelData Raw pixel data.
     * @return A vector of bytes representing the BMP file.
     */
    static std::vector<BYTE> createBitmapBuffer(const ScreenshotProcessor::BITMAPINFO256 &bmpInfo256,
                                                const std::vector<BYTE>                  &pixelData);
};
} // namespace screenshot

#endif // IMAGE_SAVER_H
