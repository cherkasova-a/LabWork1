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

constexpr int BYTES_PER_PIXEL_24 = 3;

inline int calculateRowSize(int width)
{
    constexpr int ALIGNMENT = 4;
    return ((width * BYTES_PER_PIXEL_24 + ALIGNMENT - 1) / ALIGNMENT) * ALIGNMENT;
}

#endif

