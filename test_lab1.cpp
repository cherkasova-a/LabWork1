/*
*Anastasia Cherkasova
*st140594@student.spbu.ru
*LabWork1
*/
#include "gtest/gtest.h"
#include "BMPImage.h"

class BMPImageTest : public ::testing::Test {
protected:
    BMPImage img;

    void SetUp() override {
        img.load("input.bmp");
    }
};

TEST_F(BMPImageTest, RotateClockwise) {
    img.rotate90clockwise();
    SUCCEED();
}

TEST_F(BMPImageTest, RotateCounterClockwise) {
    img.rotate90counter();
    SUCCEED();
}

TEST_F(BMPImageTest, ApplyGaussianFilter) {
    img.applyGaussian3x3();
    SUCCEED();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

