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

bool BMPImage::load(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
        return false;

    bmpHeader.resize(BMP_HEADER_SIZE);
    file.read(reinterpret_cast<char*>(bmpHeader.data()), BMP_HEADER_SIZE);

    if (bmpHeader[0] != 'B' || bmpHeader[1] != 'M')
        throw std::runtime_error("Not a BMP file");

    uint32_t dibSize = 0;
    file.read(reinterpret_cast<char*>(&dibSize), sizeof(uint32_t));
    file.seekg(BMP_HEADER_SIZE, std::ios::beg);

    dibHeader.resize(dibSize);
    file.read(reinterpret_cast<char*>(dibHeader.data()), dibSize);

    uint16_t planes = *reinterpret_cast<uint16_t*>(&dibHeader[12]);
    uint16_t bpp = *reinterpret_cast<uint16_t*>(&dibHeader[14]);
    uint32_t compression = *reinterpret_cast<uint32_t*>(&dibHeader[16]);

    if (planes != 1 || bpp != BYTES_PER_PIXEL_24 * 8 || compression != 0)
        throw std::runtime_error("Only uncompressed 24bpp BMP supported");

    width = *reinterpret_cast<int32_t*>(&dibHeader[4]);
    height = std::abs(*reinterpret_cast<int32_t*>(&dibHeader[8]));
    uint32_t pixelOffset = *reinterpret_cast<uint32_t*>(&bmpHeader[10]);

    int rowSize = calculateRowSize(width);
    pixelData.resize(rowSize * height);

    file.seekg(pixelOffset, std::ios::beg);
    file.read(reinterpret_cast<char*>(pixelData.data()), pixelData.size());

    file.close();
    return true;
}

bool BMPImage::save(const std::string& filename)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file)
        return false;

    file.write(reinterpret_cast<const char*>(bmpHeader.data()), bmpHeader.size());
    file.write(reinterpret_cast<const char*>(dibHeader.data()), dibHeader.size());
    file.write(reinterpret_cast<const char*>(pixelData.data()), pixelData.size());

    file.close();
    return true;
}

void BMPImage::rotate90clockwise()
{
    int oldRowSize = calculateRowSize(width);
    int newWidth = height;
    int newHeight = width;
    int newRowSize = calculateRowSize(newWidth);

    std::vector<uint8_t> newPixels(newRowSize * newHeight, 0);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int srcIndex = y * oldRowSize + x * BYTES_PER_PIXEL_24;
            int destIndex = x * newRowSize + (newWidth - 1 - y) * BYTES_PER_PIXEL_24;
            std::copy_n(&pixelData[srcIndex], BYTES_PER_PIXEL_24, &newPixels[destIndex]);
        }
    }

    width = newWidth;
    height = newHeight;
    pixelData = std::move(newPixels);

    *reinterpret_cast<int32_t*>(&dibHeader[4]) = width;
    *reinterpret_cast<int32_t*>(&dibHeader[8]) = height;
}

void BMPImage::rotate90counter()
{
    int oldRowSize = calculateRowSize(width);
    int newWidth = height;
    int newHeight = width;
    int newRowSize = calculateRowSize(newWidth);

    std::vector<uint8_t> newPixels(newRowSize * newHeight, 0);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int srcIndex = y * oldRowSize + x * BYTES_PER_PIXEL_24;
            int destIndex = (newHeight - 1 - x) * newRowSize + y * BYTES_PER_PIXEL_24;
            std::copy_n(&pixelData[srcIndex], BYTES_PER_PIXEL_24, &newPixels[destIndex]);
        }
    }

    width = newWidth;
    height = newHeight;
    pixelData = std::move(newPixels);

    *reinterpret_cast<int32_t*>(&dibHeader[4]) = width;
    *reinterpret_cast<int32_t*>(&dibHeader[8]) = height;
}

void BMPImage::applyGaussian3x3()
{
    static const int kernel[3][3] = {{1,2,1},{2,4,2},{1,2,1}};
    constexpr int kernelSum = GAUSS_KERNEL_SUM;

    int rowSize = calculateRowSize(width);
    std::vector<uint8_t> newPixels = pixelData;

    for (int y = 1; y < height - 1; ++y)
    {
        for (int x = 1; x < width - 1; ++x)
        {
            int sum[3] = {0,0,0};
            for (int ky = -1; ky <= 1; ++ky)
            {
                for (int kx = -1; kx <= 1; ++kx)
                {
                    int idx = (y + ky) * rowSize + (x + kx) * BYTES_PER_PIXEL_24;
                    for (int c = 0; c < 3; ++c)
                        sum[c] += pixelData[idx + c] * kernel[ky + 1][kx + 1];
                }
            }
            int dstIdx = y * rowSize + x * BYTES_PER_PIXEL_24;
            for (int c = 0; c < 3; ++c)
                newPixels[dstIdx + c] = sum[c] / kernelSum;
        }
    }

    pixelData = std::move(newPixels);
}

