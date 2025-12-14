/*
*Anastasia Cherkasova
*st140594@student.spbu.ru
*LabWork1
*/
#ifndef BMP_CONSTANTS_H
#define BMP_CONSTANTS_H

#include <cstddef>
#include <cstdint>

constexpr std::size_t BMP_HEADER_SIZE = 14;
constexpr std::size_t DIB_HEADER_SIZE = 40;

constexpr uint16_t BMP_SIGNATURE = 0x4D42;

constexpr uint16_t BITS_PER_PIXEL_24 = 24;

constexpr uint32_t DIB_WIDTH_OFFSET = 4;
constexpr uint32_t DIB_HEIGHT_OFFSET = 8;
constexpr uint32_t DIB_PLANES_OFFSET = 12;
constexpr uint32_t DIB_BPP_OFFSET = 14;
constexpr uint32_t DIB_IMAGE_SIZE_OFFSET = 20;

constexpr uint32_t BMP_DATA_OFFSET = 10;

constexpr int BYTES_PER_PIXEL_24 = 3;

constexpr int GAUSS_KERNEL_SIZE = 3;
constexpr int GAUSS_KERNEL_SUM = 16;

inline int calculateRowSize(int width)
{
    return ((width * BYTES_PER_PIXEL_24 + 3) / 4) * 4;
}

#endif

