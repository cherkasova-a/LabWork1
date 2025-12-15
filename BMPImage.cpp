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
#include <cstring>

bool BMPImage::load(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
        return false;

    bmpHeader.resize(BMP_HEADER_SIZE);
    file.read(reinterpret_cast<char*>(bmpHeader.data()), BMP_HEADER_SIZE);
    if (bmpHeader[0] != 'B' || bmpHeader[1] != 'M')
        return false;

    uint32_t dibSize;
    file.read(reinterpret_cast<char*>(&dibSize), sizeof(dibSize));
    file.seekg(BMP_HEADER_SIZE, std::ios::beg);

    dibHeader.resize(dibSize);
    file.read(reinterpret_cast<char*>(dibHeader.data()), dibSize);

    width = read_s32_le(dibHeader.data() + DIB_WIDTH_OFFSET);
    int32_t rawHeight = read_s32_le(dibHeader.data() + DIB_HEIGHT_OFFSET);

    originalTopDown = rawHeight < 0;
    height = std::abs(rawHeight);

    uint16_t planes = read_u16_le(dibHeader.data() + DIB_PLANES_OFFSET);
    uint16_t bpp = read_u16_le(dibHeader.data() + DIB_BPP_OFFSET);
    uint32_t compression = read_u32_le(dibHeader.data() + DIB_COMPRESSION_OFFSET);

    if (planes != 1 || bpp != BITS_PER_PIXEL || compression != 0)
        throw std::runtime_error("Only uncompressed 24bpp BMP supported");

    uint32_t pixelOffset = read_u32_le(bmpHeader.data() + BMP_DATA_OFFSET);
    int rowSize = calculateRowSize(width);

    pixelData.assign(rowSize * height, 0);
    file.seekg(pixelOffset, std::ios::beg);

    for (int y = 0; y < height; ++y)
    {
        int row = originalTopDown ? y : (height - 1 - y);
        file.read(reinterpret_cast<char*>(&pixelData[row * rowSize]), rowSize);
    }

    return true;
}

bool BMPImage::save(const std::string& filename)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file)
        return false;

    int rowSize = calculateRowSize(width);
    uint32_t imageSize = rowSize * height;
    uint32_t fileSize = bmpHeader.size() + dibHeader.size() + imageSize;

    write_u32_le(bmpHeader.data() + BMP_FILESIZE_OFFSET, fileSize);
    write_s32_le(dibHeader.data() + DIB_WIDTH_OFFSET, width);
    write_s32_le(
        dibHeader.data() + DIB_HEIGHT_OFFSET,
        originalTopDown ? -height : height
    );
    write_u32_le(dibHeader.data() + DIB_IMAGE_SIZE_OFFSET, imageSize);

    file.write(reinterpret_cast<char*>(bmpHeader.data()), bmpHeader.size());
    file.write(reinterpret_cast<char*>(dibHeader.data()), dibHeader.size());

    for (int y = 0; y < height; ++y)
    {
        int row = originalTopDown ? y : (height - 1 - y);
        file.write(reinterpret_cast<char*>(&pixelData[row * rowSize]), rowSize);
    }

    return true;
}



void BMPImage::rotate90clockwise()
{
    int oldRowSize = calculateRowSize(width);
    int newW = height;
    int newH = width;
    int newRowSize = calculateRowSize(newW);

    std::vector<uint8_t> out(newRowSize * newH, 0);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            for (int c = 0; c < BYTES_PER_PIXEL; ++c)
            {
                int srcIdx = y * oldRowSize + x * BYTES_PER_PIXEL + c;
                int dstX = height - 1 - y;
                int dstY = x;
                int dstIdx = dstY * newRowSize + dstX * BYTES_PER_PIXEL + c;
                out[dstIdx] = pixelData[srcIdx];
            }
        }
    }

    pixelData = std::move(out);
    width = newW;
    height = newH;

}

void BMPImage::rotate90counter()
{
    int oldRowSize = calculateRowSize(width);
    int newW = height;
    int newH = width;
    int newRowSize = calculateRowSize(newW);

    std::vector<uint8_t> out(newRowSize * newH, 0);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            for (int c = 0; c < BYTES_PER_PIXEL; ++c)
            {
                int srcIdx = y * oldRowSize + x * BYTES_PER_PIXEL + c;
                int dstX = y;
                int dstY = width - 1 - x;
                int dstIdx = dstY * newRowSize + dstX * BYTES_PER_PIXEL + c;
                out[dstIdx] = pixelData[srcIdx];
            }
        }
    }

    pixelData = std::move(out);
    width = newW;
    height = newH;

}



void BMPImage::applyGaussian3x3()
{
    int rowSize = calculateRowSize(width);
    std::vector<uint8_t> newPixels(pixelData.size(), 0);

    constexpr int kernel[3][3] = {
        {1, 2, 1},
        {2, 4, 2},
        {1, 2, 1}
    };

    for (int y = 1; y < height - 1; ++y)
    {
        for (int x = 1; x < width - 1; ++x)
        {
            for (int c = 0; c < BYTES_PER_PIXEL; ++c)
            {
                int sum = 0;
                for (int ky = -1; ky <= 1; ++ky)
                {
                    for (int kx = -1; kx <= 1; ++kx)
                    {
                        int idx = (y + ky) * rowSize + (x + kx) * BYTES_PER_PIXEL + c;
                        sum += pixelData[idx] * kernel[ky + 1][kx + 1];
                    }
                }
                newPixels[y * rowSize + x * BYTES_PER_PIXEL + c] = sum / GAUSS_KERNEL_SUM;
            }
        }
    }

    pixelData = std::move(newPixels);
}
