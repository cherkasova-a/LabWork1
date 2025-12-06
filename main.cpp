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
    if (!img.save("output_clockwise.bmp")) {
        std::cerr << "Ошибка: не удалось сохранить output_clockwise.bmp" << std::endl;
        return 1;
    }

    img.rotate90counter();
    if (!img.save("output_counterclockwise.bmp")) {
        std::cerr << "Ошибка: не удалось сохранить output_counterclockwise.bmp" << std::endl;
        return 1;
    }

    img.rotate90clockwise();
    img.applyGaussian3x3();
    if (!img.save("output_gaussian.bmp")) {
        std::cerr << "Ошибка: не удалось сохранить output_gaussian.bmp" << std::endl;
        return 1;
    }

    std::cout << "Готово! Все файлы сохранены:" << std::endl;

    return 0;
}

