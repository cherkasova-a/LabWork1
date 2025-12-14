/*
*Anastasia Cherkasova
*st140594@student.spbu.ru
*LabWork1
*/
#include "BMPImage.h"
#include "BMPConstants.h"

#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <cstring>

bool BMPImage::load(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
        return false;

    bmpHeader.resize(BMP_HEADER_SIZE);
    file.read(reinterpret_cast<char*>(bmpHeader.data()), BMP_HEADER_SIZE);

    uint16_t signature;
    std::memcpy(&signature, bmpHeader.data(), sizeof(signature));
    if (signature != BMP_SIGNATURE)
        throw std::runtime_error("Not a BMP file");

    dibHeader.resize(DIB_HEADER_SIZE);
    file.read(reinterpret_cast<char*>(dibHeader.data()), DIB_HEADER_SIZE);

    std::memcpy(&width,  &dibHeader[DIB_WIDTH_OFFSET],  sizeof(int32_t));
    std::memcpy(&height, &dibHeader[DIB_HEIGHT_OFFSET], sizeof(int32_t));

    uint16_t bpp;
    std::memcpy(&bpp, &dibHeader[DIB_BPP_OFFSET], sizeof(uint16_t));
    if (bpp != BITS_PER_PIXEL_24)
        throw std::runtime_error("Only 24bpp supported");

    uint16_t planes;
    std::memcpy(&planes, &dibHeader[DIB_PLANES_OFFSET], sizeof(uint16_t));
    if (planes != 1)
        throw std::runtime_error("Invalid BMP planes");

    height = std::abs(height);

    uint32_t dataOffset;
    std::memcpy(&dataOffset, &bmpHeader[BMP_DATA_OFFSET], sizeof(uint32_t));

    int rowSize = calculateRowSize(width);
    pixelData.resize(rowSize * height);

    file.seekg(dataOffset, std::ios::beg);
    file.read(reinterpret_cast<char*>(pixelData.data()), pixelData.size());

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

    return true;
}

void BMPImage::rotate90clockwise()
{
    int oldStride = calculateRowSize(width);
    int newWidth = height;
    int newHeight = width;
    int newStride = calculateRowSize(newWidth);

    std::vector<uint8_t> out(newStride * newHeight, 0);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int src = y * oldStride + x * BYTES_PER_PIXEL_24;
            int dst = x * newStride + (newWidth - 1 - y) * BYTES_PER_PIXEL_24;
            std::copy_n(&pixelData[src], BYTES_PER_PIXEL_24, &out[dst]);
        }
    }

    width = newWidth;
    height = newHeight;
    pixelData = std::move(out);

    std::memcpy(&dibHeader[DIB_WIDTH_OFFSET],  &width,  sizeof(int32_t));
    std::memcpy(&dibHeader[DIB_HEIGHT_OFFSET], &height, sizeof(int32_t));
}

void BMPImage::rotate90counter()
{
    int oldStride = calculateRowSize(width);
    int newWidth = height;
    int newHeight = width;
    int newStride = calculateRowSize(newWidth);

    std::vector<uint8_t> out(newStride * newHeight, 0);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int src = y * oldStride + x * BYTES_PER_PIXEL_24;
            int dst = (newHeight - 1 - x) * newStride + y * BYTES_PER_PIXEL_24;
            std::copy_n(&pixelData[src], BYTES_PER_PIXEL_24, &out[dst]);
        }
    }

    width = newWidth;
    height = newHeight;
    pixelData = std::move(out);

    std::memcpy(&dibHeader[DIB_WIDTH_OFFSET],  &width,  sizeof(int32_t));
    std::memcpy(&dibHeader[DIB_HEIGHT_OFFSET], &height, sizeof(int32_t));
}

void BMPImage::applyGaussian3x3()
{
    static const int kernel[3][3] =
    {
        {1, 2, 1},
        {2, 4, 2},
        {1, 2, 1}
    };

    int stride = calculateRowSize(width);
    std::vector<uint8_t> out = pixelData;

    for (int y = 1; y < height - 1; ++y)
    {
        for (int x = 1; x < width - 1; ++x)
        {
            int sumB = 0, sumG = 0, sumR = 0;

            for (int ky = -1; ky <= 1; ++ky)
            {
                for (int kx = -1; kx <= 1; ++kx)
                {
                    int idx = (y + ky) * stride + (x + kx) * BYTES_PER_PIXEL_24;
                    int k = kernel[ky + 1][kx + 1];

                    sumB += pixelData[idx]     * k;
                    sumG += pixelData[idx + 1] * k;
                    sumR += pixelData[idx + 2] * k;
                }
            }

            int dst = y * stride + x * BYTES_PER_PIXEL_24;
            out[dst]     = sumB / GAUSS_KERNEL_SUM;
            out[dst + 1] = sumG / GAUSS_KERNEL_SUM;
            out[dst + 2] = sumR / GAUSS_KERNEL_SUM;
        }
    }

    pixelData = std::move(out);
}

