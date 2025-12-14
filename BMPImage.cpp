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
        throw std::runtime_error("Not a BMP file");

    uint32_t dibSize = 0;
    file.read(reinterpret_cast<char*>(&dibSize), sizeof(dibSize));

    if (dibSize < DIB_HEADER_MIN_SIZE)
        throw std::runtime_error("Unsupported DIB header size");

    dibHeader.resize(dibSize);
    file.seekg(BMP_HEADER_SIZE, std::ios::beg);
    file.read(reinterpret_cast<char*>(dibHeader.data()), dibSize);

    width = *reinterpret_cast<int32_t*>(&dibHeader[DIB_WIDTH_OFFSET]);
    int32_t rawHeight = *reinterpret_cast<int32_t*>(&dibHeader[DIB_HEIGHT_OFFSET]);
    height = std::abs(rawHeight);
    originalTopDown = rawHeight < 0;

    uint16_t planes = *reinterpret_cast<uint16_t*>(&dibHeader[DIB_PLANES_OFFSET]);
    uint16_t bpp = *reinterpret_cast<uint16_t*>(&dibHeader[DIB_BPP_OFFSET]);
    uint32_t compression = *reinterpret_cast<uint32_t*>(&dibHeader[DIB_COMPRESSION_OFFSET]);

    if (planes != 1 || bpp != BITS_PER_PIXEL || compression != 0)
        throw std::runtime_error("Only uncompressed 24bpp BMP supported");

    uint32_t pixelOffset = *reinterpret_cast<uint32_t*>(&bmpHeader[BMP_DATA_OFFSET]);
    file.seekg(pixelOffset, std::ios::beg);

    int rowSize = calculateRowSize(width);
    pixelData.resize(rowSize * height);
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
    int newW = height;
    int newH = width;
    int newRowSize = calculateRowSize(newW);
    std::vector<uint8_t> out(newRowSize * newH, 0);

    int oldRowSize = calculateRowSize(width);
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int src = y * oldRowSize + x * BYTES_PER_PIXEL;
            int dst = x * newRowSize + (newW - 1 - y) * BYTES_PER_PIXEL;
            std::copy_n(&pixelData[src], BYTES_PER_PIXEL, &out[dst]);
        }
    }

    pixelData = std::move(out);
    width = newW;
    height = newH;
    write_s32_le(dibHeader.data() + DIB_WIDTH_OFFSET, width);
    write_s32_le(dibHeader.data() + DIB_HEIGHT_OFFSET, height);
}

void BMPImage::rotate90counter()
{
    int newW = height;
    int newH = width;
    int newRowSize = calculateRowSize(newW);
    std::vector<uint8_t> out(newRowSize * newH, 0);

    int oldRowSize = calculateRowSize(width);
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int src = y * oldRowSize + x * BYTES_PER_PIXEL;
            int dst = (newH - 1 - x) * newRowSize + y * BYTES_PER_PIXEL;
            std::copy_n(&pixelData[src], BYTES_PER_PIXEL, &out[dst]);
        }
    }

    pixelData = std::move(out);
    width = newW;
    height = newH;
    write_s32_le(dibHeader.data() + DIB_WIDTH_OFFSET, width);
    write_s32_le(dibHeader.data() + DIB_HEIGHT_OFFSET, height);
}

void BMPImage::applyGaussian3x3()
{
    const int kernel[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
    int rowSize = calculateRowSize(width);
    std::vector<uint8_t> newPixels(pixelData.size(), 0);

    for (int y = 1; y < height - 1; ++y)
    {
        for (int x = 1; x < width - 1; ++x)
        {
            for (int c = 0; c < 3; ++c)
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
