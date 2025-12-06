/*
*Anastasia Cherkasova
*st140594@student.spbu.ru
*LabWork1
*/
#include "BMPImage.h"
#include <iostream>

int main() {
    BMPImage img;

    if (!img.load("input.bmp")) {
        std::cerr << "Ошибка: не удалось загрузить input.bmp" << std::endl;
        return 1;
    }

    img.rotate90clockwise();

    img.applyGaussian3x3();

    if (!img.save("output.bmp")) {
        std::cerr << "Ошибка: не удалось сохранить output.bmp" << std::endl;
        return 1;
    }

    std::cout << "Готово!" << std::endl;
    return 0;
}

