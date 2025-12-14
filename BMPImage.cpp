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
        throw std::runtime_error("Only BMP files supported");

    uint32_t dibSize = 0;
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
    pixelData.resize(calculateRowSize(width) * height);
    file.seekg(pixelOffset, std::ios::beg);
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
    int oldRowSize = calculateRowSize(width);
    int newRowSize = calculateRowSize(newW);

    std::vector<uint8_t> out(newRowSize * newH, 0);

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
}

void BMPImage::rotate90counter()
{
    int newW = height;
    int newH = width;
    int oldRowSize = calculateRowSize(width);
    int newRowSize = calculateRowSize(newW);

    std::vector<uint8_t> out(newRowSize * newH, 0);

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
