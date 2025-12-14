/*
*Anastasia Cherkasova
*st140594@student.spbu.ru
*LabWork1
*/
#include "BMPImage.h"
#include "BMPConstants.h"
#include <fstream>
#include <stdexcept>
#include <vector>
#include <algorithm>

bool BMPImage::load(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) return false;

    uint8_t fileHeader[BMP_HEADER_SIZE];
    uint8_t infoHeader[DIB_HEADER_SIZE];

    file.read(reinterpret_cast<char*>(fileHeader), BMP_HEADER_SIZE);
    file.read(reinterpret_cast<char*>(infoHeader), DIB_HEADER_SIZE);

    if (*reinterpret_cast<uint16_t*>(fileHeader) != BMP_SIGNATURE)
        throw std::runtime_error("Not a BMP file");

    width  = *reinterpret_cast<int32_t*>(&infoHeader[DIB_WIDTH_OFFSET]);
    height = std::abs(*reinterpret_cast<int32_t*>(&infoHeader[DIB_HEIGHT_OFFSET]));
    uint16_t bpp = *reinterpret_cast<uint16_t*>(&infoHeader[DIB_BPP_OFFSET]);

    if (bpp != BITS_PER_PIXEL_24)
        throw std::runtime_error("Only 24bpp supported");

    bmpHeader.assign(fileHeader, fileHeader + BMP_HEADER_SIZE);
    dibHeader.assign(infoHeader, infoHeader + DIB_HEADER_SIZE);

    int rowSize = calculateRowSize(width);
    pixelData.resize(rowSize * height);

    uint32_t pixelOffset = *reinterpret_cast<uint32_t*>(&fileHeader[BMP_DATA_OFFSET]);
    file.seekg(pixelOffset, std::ios::beg);
    file.read(reinterpret_cast<char*>(pixelData.data()), pixelData.size());
    file.close();

    return true;
}

bool BMPImage::save(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) return false;

    file.write(reinterpret_cast<char*>(bmpHeader.data()), bmpHeader.size());
    file.write(reinterpret_cast<char*>(dibHeader.data()), dibHeader.size());
    file.write(reinterpret_cast<char*>(pixelData.data()), pixelData.size());

    file.close();
    return true;
}

void BMPImage::rotate90clockwise() {
    int oldRowSize = calculateRowSize(width);
    int newWidth = height;
    int newHeight = width;
    int newRowSize = calculateRowSize(newWidth);

    std::vector<uint8_t> newPixels(newRowSize * newHeight, 0);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int srcIdx = y * oldRowSize + x * BYTES_PER_PIXEL_24;
            int dstIdx = x * newRowSize + (newWidth - 1 - y) * BYTES_PER_PIXEL_24;
            for (int i = 0; i < BYTES_PER_PIXEL_24; ++i)
                newPixels[dstIdx + i] = pixelData[srcIdx + i];
        }
    }

    width = newWidth;
    height = newHeight;
    pixelData = std::move(newPixels);

    *reinterpret_cast<int32_t*>(&dibHeader[DIB_WIDTH_OFFSET])  = width;
    *reinterpret_cast<int32_t*>(&dibHeader[DIB_HEIGHT_OFFSET]) = height;
}

void BMPImage::rotate90counter() {
    int oldRowSize = calculateRowSize(width);
    int newWidth = height;
    int newHeight = width;
    int newRowSize = calculateRowSize(newWidth);

    std::vector<uint8_t> newPixels(newRowSize * newHeight, 0);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int srcIdx = y * oldRowSize + x * BYTES_PER_PIXEL_24;
            int dstIdx = (newHeight - 1 - x) * newRowSize + y * BYTES_PER_PIXEL_24;
            for (int i = 0; i < BYTES_PER_PIXEL_24; ++i)
                newPixels[dstIdx + i] = pixelData[srcIdx + i];
        }
    }

    width = newWidth;
    height = newHeight;
    pixelData = std::move(newPixels);

    *reinterpret_cast<int32_t*>(&dibHeader[DIB_WIDTH_OFFSET])  = width;
    *reinterpret_cast<int32_t*>(&dibHeader[DIB_HEIGHT_OFFSET]) = height;
}

void BMPImage::applyGaussian3x3() {
    static const int kernel[GAUSS_KERNEL_SIZE][GAUSS_KERNEL_SIZE] = {{1,2,1},{2,4,2},{1,2,1}};
    int rowSize = calculateRowSize(width);
    std::vector<uint8_t> newPixels = pixelData;

    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            int sumB = 0, sumG = 0, sumR = 0;
            for (int ky = -1; ky <= 1; ++ky) {
                for (int kx = -1; kx <= 1; ++kx) {
                    int idx = (y + ky) * rowSize + (x + kx) * BYTES_PER_PIXEL_24;
                    sumB += pixelData[idx]     * kernel[ky+1][kx+1];
                    sumG += pixelData[idx + 1] * kernel[ky+1][kx+1];
                    sumR += pixelData[idx + 2] * kernel[ky+1][kx+1];
                }
            }
            int dstIdx = y * rowSize + x * BYTES_PER_PIXEL_24;
            newPixels[dstIdx]     = sumB / GAUSS_KERNEL_SUM;
            newPixels[dstIdx + 1] = sumG / GAUSS_KERNEL_SUM;
            newPixels[dstIdx + 2] = sumR / GAUSS_KERNEL_SUM;
        }
    }

    pixelData = std::move(newPixels);
}

