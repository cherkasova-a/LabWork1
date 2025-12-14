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

#pragma pack(push, 1)
struct BMPFileHeader {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
};

struct BMPInfoHeader {
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};
#pragma pack(pop)

bool BMPImage::load(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
        return false;

    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    file.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

    if (fileHeader.bfType != BMP_SIGNATURE)
        throw std::runtime_error("Not a BMP file");

    if (infoHeader.biBitCount != 24 || infoHeader.biCompression != 0)
        throw std::runtime_error("Only uncompressed 24bpp BMP supported");

    width = infoHeader.biWidth;
    height = std::abs(infoHeader.biHeight);

    bmpHeader.resize(BMP_HEADER_SIZE);
    dibHeader.resize(DIB_HEADER_SIZE);
    std::copy_n(reinterpret_cast<uint8_t*>(&fileHeader), BMP_HEADER_SIZE, bmpHeader.begin());
    std::copy_n(reinterpret_cast<uint8_t*>(&infoHeader), DIB_HEADER_SIZE, dibHeader.begin());

    int rowSize = calculateRowSize(width);
    pixelData.resize(rowSize * height);

    file.seekg(fileHeader.bfOffBits, std::ios::beg);
    file.read(reinterpret_cast<char*>(pixelData.data()), pixelData.size());
    file.close();

    return true;
}

bool BMPImage::save(const std::string& filename)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file)
        return false;

    file.write(reinterpret_cast<char*>(bmpHeader.data()), bmpHeader.size());
    file.write(reinterpret_cast<char*>(dibHeader.data()), dibHeader.size());
    file.write(reinterpret_cast<char*>(pixelData.data()), pixelData.size());

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
        for (int x = 0; x < width; ++x)
            for (int i = 0; i < BYTES_PER_PIXEL_24; ++i)
                newPixels[x * newRowSize + (newWidth - 1 - y) * BYTES_PER_PIXEL_24 + i] =
                    pixelData[y * oldRowSize + x * BYTES_PER_PIXEL_24 + i];

    width = newWidth;
    height = newHeight;
    pixelData = std::move(newPixels);
    *reinterpret_cast<int*>(&dibHeader[4]) = width;
    *reinterpret_cast<int*>(&dibHeader[8]) = height;
}

void BMPImage::rotate90counter()
{
    int oldRowSize = calculateRowSize(width);
    int newWidth = height;
    int newHeight = width;
    int newRowSize = calculateRowSize(newWidth);

    std::vector<uint8_t> newPixels(newRowSize * newHeight, 0);

    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            for (int i = 0; i < BYTES_PER_PIXEL_24; ++i)
                newPixels[(newHeight - 1 - x) * newRowSize + y * BYTES_PER_PIXEL_24 + i] =
                    pixelData[y * oldRowSize + x * BYTES_PER_PIXEL_24 + i];

    width = newWidth;
    height = newHeight;
    pixelData = std::move(newPixels);
    *reinterpret_cast<int*>(&dibHeader[4]) = width;
    *reinterpret_cast<int*>(&dibHeader[8]) = height;
}

void BMPImage::applyGaussian3x3()
{
    static const int kernel[3][3] = {{1,2,1},{2,4,2},{1,2,1}};
    constexpr int kernelSum = 16;

    int rowSize = calculateRowSize(width);
    std::vector<uint8_t> newPixels = pixelData;

    for (int y = 1; y < height - 1; ++y)
        for (int x = 1; x < width - 1; ++x)
        {
            int sumB = 0, sumG = 0, sumR = 0;
            for (int ky = -1; ky <= 1; ++ky)
                for (int kx = -1; kx <= 1; ++kx)
                {
                    int idx = (y + ky) * rowSize + (x + kx) * BYTES_PER_PIXEL_24;
                    sumB += pixelData[idx] * kernel[ky + 1][kx + 1];
                    sumG += pixelData[idx + 1] * kernel[ky + 1][kx + 1];
                    sumR += pixelData[idx + 2] * kernel[ky + 1][kx + 1];
                }
            int dstIdx = y * rowSize + x * BYTES_PER_PIXEL_24;
            newPixels[dstIdx] = sumB / kernelSum;
            newPixels[dstIdx + 1] = sumG / kernelSum;
            newPixels[dstIdx + 2] = sumR / kernelSum;
        }

    pixelData = std::move(newPixels);
}

