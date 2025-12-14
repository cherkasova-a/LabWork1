/*
*Anastasia Cherkasova
*st140594@student.spbu.ru
*LabWork1
*/
#pragma once
#include <string>
#include <vector>

class IImage {
public:
    virtual bool load(const std::string& filename) = 0;
    virtual bool save(const std::string& filename) = 0;

    virtual void rotate90clockwise() = 0;
    virtual void rotate90counter() = 0;

    virtual void applyGaussian3x3() = 0;

    virtual ~IImage() = default;
};

