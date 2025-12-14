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
#include <cstdint>
#include <cmath>

bool BMPImage::load(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
        return false;

    bmpHeader.resize(BMP_HEADER_SIZE);
    file.read(reinterpret_cast<char*>(bmpHeader.data()), BMP_HEADER_SIZE);
    if (*reinterpret_cast<uint16_t*>(bmpHeader.data()) != BMP_SIGNATURE)
        throw std::runtime_error("Only uncompressed 24bpp BMP supported");

    dibHeader.resize(DIB_HEADER_SIZE);
    file.read(reinterpret_cast<char*>(dibHeader.data()), DIB_HEADER_SIZE);

    width = *reinterpret_cast<int32_t*>(&dibHeader[DIB_WIDTH_OFFSET]);
    height = std::abs(*reinterpret_cast<int32_t*>(&dibHeader[DIB_HEIGHT_OFFSET]));
    uint16_t bpp = *reinterpret_cast<uint16_t*>(&dibHeader[DIB_BPP_OFFSET]);
    uint32_t compression = *reinterpret_cast<uint32_t*>(&dibHeader[16]);

    if (bpp != BITS_PER_PIXEL_24 || compression != 0)
        throw std::runtime_error("Only uncompressed 24bpp BMP supported");

    int rowSize = calculateRowSize(width);
    pixelData.resize(rowSize * height);

    uint32_t dataOffset = *reinterpret_cast<uint32_t*>(&bmpHeader[BMP_DATA_OFFSET]);
    file.seekg(dataOffset, std::ios::beg);
    file.read(reinterpret_cast<char*>(pixelData.data()), pixelData.size());
    file.close();

    return true;
}

bool BMPImage::save(const std::string& filename)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file) return false;

    file.write(reinterpret_cast<char*>(bmpHeader.data()), bmpHeader.size());
    file.write(reinterpret_cast<char*>(dibHeader.data()), dibHeader.size());
    file.write(reinterpret_cast<char*>(pixelData.data()), pixelData.size());
    file.close();
    return true;
}

void BMPImage::rotate90clockwise()
{
    int oldStride = calculateRowSize(width);
    int newW = height;
    int newH = width;
    int newStride = calculateRowSize(newW);

    std::vector<uint8_t> out(newStride * newH, 0);

    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x) {
            int src = y * oldStride + x * BYTES_PER_PIXEL_24;
            int dst = x * newStride + (newW - 1 - y) * BYTES_PER_PIXEL_24;
            std::copy_n(&pixelData[src], BYTES_PER_PIXEL_24, &out[dst]);
        }

    width = newW;
    height = newH;
    pixelData = std::move(out);

    *reinterpret_cast<int32_t*>(&dibHeader[DIB_WIDTH_OFFSET]) = width;
    *reinterpret_cast<int32_t*>(&dibHeader[DIB_HEIGHT_OFFSET]) = height;
}

void BMPImage::rotate90counter()
{
    int oldStride = calculateRowSize(width);
    int newW = height;
    int newH = width;
    int newStride = calculateRowSize(newW);

    std::vector<uint8_t> out(newStride * newH, 0);

    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x) {
            int src = y * oldStride + x * BYTES_PER_PIXEL_24;
            int dst = (newH - 1 - x) * newStride + y * BYTES_PER_PIXEL_24;
            std::copy_n(&pixelData[src], BYTES_PER_PIXEL_24, &out[dst]);
        }

    width = newW;
    height = newH;
    pixelData = std::move(out);

    *reinterpret_cast<int32_t*>(&dibHeader[DIB_WIDTH_OFFSET]) = width;
    *reinterpret_cast<int32_t*>(&dibHeader[DIB_HEIGHT_OFFSET]) = height;
}

void BMPImage::applyGaussian3x3()
{
    static const int kernel[3][3] = {{1,2,1},{2,4,2},{1,2,1}};
    int rowSize = calculateRowSize(width);
    std::vector<uint8_t> out = pixelData;

    for (int y = 1; y < height - 1; ++y)
        for (int x = 1; x < width - 1; ++x) {
            int sumB = 0, sumG = 0, sumR = 0;
            for (int ky = -1; ky <= 1; ++ky)
                for (int kx = -1; kx <= 1; ++kx) {
                    int idx = (y + ky) * rowSize + (x + kx) * BYTES_PER_PIXEL_24;
                    sumB += pixelData[idx] * kernel[ky + 1][kx + 1];
                    sumG += pixelData[idx + 1] * kernel[ky + 1][kx + 1];
                    sumR += pixelData[idx + 2] * kernel[ky + 1][kx + 1];
                }
            int dstIdx = y * rowSize + x * BYTES_PER_PIXEL_24;
            out[dstIdx] = sumB / GAUSS_KERNEL_SUM;
            out[dstIdx + 1] = sumG / GAUSS_KERNEL_SUM;
            out[dstIdx + 2] = sumR / GAUSS_KERNEL_SUM;
        }

    pixelData = std::move(out);
}

