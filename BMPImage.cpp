/*
*Anastasia Cherkasova
*st140594@student.spbu.ru
*LabWork1
*/
#include "BMPImage.h"
#include <fstream>
#include <iostream>

bool BMPImage::load(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) return false;

    bmpHeader.resize(14);
    file.read(reinterpret_cast<char*>(bmpHeader.data()), 14);

    dibHeader.resize(40);
    file.read(reinterpret_cast<char*>(dibHeader.data()), 40);

    width = *reinterpret_cast<int*>(&dibHeader[4]);
    height = *reinterpret_cast<int*>(&dibHeader[8]);

    int rowSize = ((24 * width + 31) / 32) * 4;
    pixelData.resize(rowSize * height);
    file.read(reinterpret_cast<char*>(pixelData.data()), pixelData.size());

    file.close();
    return true;
}

bool BMPImage::save(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) return false;

    file.write(reinterpret_cast<char*>(bmpHeader.data()), bmpHeader.size());
    file.write(reinterpret_cast<char*>(dibHeader.data()), dibHeader.size());
    file.write(reinterpret_cast<char*>(pixelData.data()), pixelData.size());

    file.close();
    return true;
}
void BMPImage::rotate90clockwise() {
    int rowSize = ((24 * width + 31) / 32) * 4;
    int newWidth = height;
    int newHeight = width;
    std::vector<uint8_t> newPixels(newHeight * ((24*newWidth+31)/32*4));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int srcIndex = y * rowSize + x * 3;
            int destRowSize = ((24 * newWidth + 31) / 32) * 4;
            int destIndex = x * destRowSize + (newWidth - 1 - y) * 3;
            newPixels[destIndex] = pixelData[srcIndex];
            newPixels[destIndex+1] = pixelData[srcIndex+1];
            newPixels[destIndex+2] = pixelData[srcIndex+2];
        }
    }

    std::swap(width, height);
    pixelData = std::move(newPixels);
}

void BMPImage::rotate90counter() {
    int rowSize = ((24 * width + 31) / 32) * 4;
    int newWidth = height;
    int newHeight = width;
    std::vector<uint8_t> newPixels(newHeight * ((24*newWidth+31)/32*4));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int srcIndex = y * rowSize + x * 3;
            int destRowSize = ((24 * newWidth + 31) / 32) * 4;
            int destIndex = (newHeight - 1 - x) * destRowSize + y * 3;
            newPixels[destIndex] = pixelData[srcIndex];
            newPixels[destIndex+1] = pixelData[srcIndex+1];
            newPixels[destIndex+2] = pixelData[srcIndex+2];
        }
    }

    std::swap(width, height);
    pixelData = std::move(newPixels);
}

void BMPImage::applyGaussian3x3() {
    int rowSize = ((24 * width + 31) / 32) * 4;
    std::vector<uint8_t> newPixels = pixelData;

    int kernel[3][3] = {{1,2,1},{2,4,2},{1,2,1}};

    for (int y = 1; y < height-1; ++y) {
        for (int x = 1; x < width-1; ++x) {
            int sumR=0, sumG=0, sumB=0;

            for (int ky=-1; ky<=1; ++ky) {
                for (int kx=-1; kx<=1; ++kx) {
                    int px = x+kx;
                    int py = y+ky;
                    int idx = py*rowSize + px*3;
                    sumB += pixelData[idx] * kernel[ky+1][kx+1];
                    sumG += pixelData[idx+1] * kernel[ky+1][kx+1];
                    sumR += pixelData[idx+2] * kernel[ky+1][kx+1];
                }
            }

            int dstIdx = y*rowSize + x*3;
            newPixels[dstIdx]   = sumB / 16;
            newPixels[dstIdx+1] = sumG / 16;
            newPixels[dstIdx+2] = sumR / 16;
        }
    }

    pixelData = std::move(newPixels);
}
