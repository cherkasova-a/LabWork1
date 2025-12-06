
/*
*Anastasia Cherkasova
*st140594@student.spbu.ru
*LabWork1
*/
#ifndef BMP_IMAGE_H
#define BMP_IMAGE_H

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

private:
    int width = 0;
    int height = 0;

    std::vector<uint8_t> bmpHeader;  
    std::vector<uint8_t> dibHeader;  
    std::vector<uint8_t> pixelData;  
};

#endif
