/*
*Anastasia Cherkasova
*st140594@student.spbu.ru
*LabWork1
*/
#pragma once
#include "BMPConstants.h"
#include "IImage.h"
#include <vector>
#include <string>
#include <cstdint>

class BMPImage : public IImage {
public:
    BMPImage() = default;

    bool load(const std::string& filename) override;
    bool save(const std::string& filename) override;

    void rotate90clockwise() override;
    void rotate90counter() override;
    void applyGaussian3x3() override;

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    const std::vector<uint8_t>& getPixelData() const { return pixelData; }

private:
    int width = 0;
    int height = 0;
    bool originalTopDown = false;

    std::vector<uint8_t> bmpHeader;
    std::vector<uint8_t> dibHeader;
    std::vector<uint8_t> pixelData;

    static constexpr int BMP_HEADER_SIZE = 14;
    static constexpr int MIN_DIB_HEADER_SIZE = 40;
    static constexpr int BITS_PER_PIXEL = 24;
    static constexpr int BYTES_PER_PIXEL = BITS_PER_PIXEL / 8;
    static constexpr int ROW_ALIGNMENT = 4;

    static int calculateRowSize(int w) {
        return ((w * BYTES_PER_PIXEL + ROW_ALIGNMENT - 1) / ROW_ALIGNMENT) * ROW_ALIGNMENT;
    }

    static uint16_t read_u16_le(const uint8_t* ptr) {
        return static_cast<uint16_t>(ptr[0]) | (static_cast<uint16_t>(ptr[1]) << 8);
    }
    static uint32_t read_u32_le(const uint8_t* ptr) {
        return static_cast<uint32_t>(ptr[0]) | (static_cast<uint32_t>(ptr[1]) << 8) |
               (static_cast<uint32_t>(ptr[2]) << 16) | (static_cast<uint32_t>(ptr[3]) << 24);
    }
    static int32_t read_s32_le(const uint8_t* ptr) {
        return static_cast<int32_t>(read_u32_le(ptr));
    }
    static void write_s32_le(uint8_t* ptr, int32_t v) {
        ptr[0] = static_cast<uint8_t>(v & 0xFF);
        ptr[1] = static_cast<uint8_t>((v >> 8) & 0xFF);
        ptr[2] = static_cast<uint8_t>((v >> 16) & 0xFF);
        ptr[3] = static_cast<uint8_t>((v >> 24) & 0xFF);
    }
    static void write_u32_le(uint8_t* ptr, uint32_t v) {
        ptr[0] = static_cast<uint8_t>(v & 0xFF);
        ptr[1] = static_cast<uint8_t>((v >> 8) & 0xFF);
        ptr[2] = static_cast<uint8_t>((v >> 16) & 0xFF);
        ptr[3] = static_cast<uint8_t>((v >> 24) & 0xFF);
    }
};
