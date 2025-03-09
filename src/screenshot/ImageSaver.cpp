#include "screenshot/ImageSaver.h"
#include "screenshot/ImageProcessor.h"
#include "core/Constants.h"
#include <spdlog/spdlog.h>
#include <fstream>

namespace screenshot {
    bool ImageSaver::saveBitmapToFile(const std::string& filename, const ImageProcessor::BITMAPINFO256 &bmpInfo256, const std::vector<BYTE>& pixelData) {
        spdlog::info("Saving BMP file: {}", filename);

        if (filename.empty()) {
            spdlog::error("Filename is empty!");
            return false;
        }

        BITMAPFILEHEADER fileHeader = {0};
        fileHeader.bfType = BMP_HEADER_TYPE;
        fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);
        fileHeader.bfSize = fileHeader.bfOffBits + pixelData.size();

        spdlog::info("Expected file size: {} bytes (Header: {} + Pixel Data: {})",
             fileHeader.bfSize,
             sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER),
             pixelData.size());

        std::ofstream file(filename, std::ios::out | std::ios::binary);
        if (!file) {
            spdlog::error("Failed to open file: {}", filename);
            return false;
        }

        spdlog::info("File opened successfully. Writing headers...");

        file.write(reinterpret_cast<const char*>(&fileHeader), sizeof(BITMAPFILEHEADER));
        file.write(reinterpret_cast<const char*>(&bmpInfo256.bmiHeader), sizeof(BITMAPINFOHEADER));
        file.write(reinterpret_cast<const char*>(bmpInfo256.bmiColors), 256 * sizeof(RGBQUAD));

        spdlog::info("Headers written successfully. Writing pixel data...");

        file.write(reinterpret_cast<const char*>(pixelData.data()),
                   static_cast<std::streamsize>(pixelData.size()));

        if (!file) {
            spdlog::error("File write failed!");
            return false;
        }

        file.close();
        spdlog::info("File saved successfully.");
        return true;
    }
}
