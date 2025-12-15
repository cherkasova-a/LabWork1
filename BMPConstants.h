/*
*Anastasia Cherkasova
*st140594@student.spbu.ru
*LabWork1
*/
#pragma once
#include <cstddef>
#include <cstdint>

constexpr std::size_t BMP_HEADER_SIZE = 14;
constexpr std::size_t DIB_HEADER_MIN_SIZE = 40;

constexpr int BITS_PER_PIXEL = 24;
constexpr int BYTES_PER_PIXEL = 3;
constexpr int ROW_ALIGNMENT = 4;

constexpr uint32_t BMP_FILESIZE_OFFSET = 2;
constexpr uint32_t DIB_IMAGE_SIZE_OFFSET = 20;

constexpr uint32_t BMP_DATA_OFFSET = 10;

constexpr uint32_t DIB_WIDTH_OFFSET        = 4;
constexpr uint32_t DIB_HEIGHT_OFFSET       = 8;
constexpr uint32_t DIB_PLANES_OFFSET       = 12;
constexpr uint32_t DIB_BPP_OFFSET          = 14;
constexpr uint32_t DIB_COMPRESSION_OFFSET  = 16;

constexpr int GAUSS_KERNEL_SUM = 16;

inline int calculateRowSize(int width)
{
    return ((width * BYTES_PER_PIXEL + ROW_ALIGNMENT - 1) / ROW_ALIGNMENT) * ROW_ALIGNMENT;
}

