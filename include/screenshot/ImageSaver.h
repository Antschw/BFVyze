#ifndef IMAGE_SAVER_H
#define IMAGE_SAVER_H

#include <string>
#include <vector>
#include <windows.h>
#include <screenshot/ImageProcessor.h>

namespace screenshot {
    class ImageSaver {
    public:
        /**
         * @brief Saves grayscale bitmap data to a BMP file, including the 256-color palette.
         * @param filename The output file path.
         * @param bmpInfo256 The complete bitmap info structure (header + 256-color palette).
         * @param pixelData The raw grayscale pixel data.
         * @return True if the file was saved successfully, false otherwise.
         */
        static bool saveBitmapToFile(const std::string& filename, const ImageProcessor::BITMAPINFO256 &bmpInfo256, const std::vector<BYTE>& pixelData);
    };
}

#endif // IMAGE_SAVER_H
