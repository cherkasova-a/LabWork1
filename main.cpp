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

    //turn clockwise by 90
    img.rotate90clockwise();
    if (!img.save("output_clockwise.bmp")) {
        std::cerr << "Ошибка: не удалось сохранить output_clockwise.bmp" << std::endl;
        return 1;
    }

    //turn counterclockwise by 90
    img.rotate90counter();
    img.rotate90counter();
    if (!img.save("output_counterclockwise.bmp")) {
        std::cerr << "Ошибка: не удалось сохранить output_counterclockwise.bmp" << std::endl;
        return 1;
    }

    //apply filter Gaus
    img.applyGaussian3x3();
    if (!img.save("output_gaussian.bmp")) {
        std::cerr << "Ошибка: не удалось сохранить output_gaussian.bmp" << std::endl;
        return 1;
    }

    std::cout << "Готово! Все файлы сохранены:" << std::endl;
    std::cout << "1) output_clockwise.bmp" << std::endl;
    std::cout << "2) output_counterclockwise.bmp" << std::endl;
    std::cout << "3) output_gaussian.bmp" << std::endl;

    return 0;
}

