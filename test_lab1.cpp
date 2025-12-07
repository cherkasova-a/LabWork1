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
    void SetUp() override { img.load("test.bmp"); }
};

TEST_F(BMPImageTest, RotateClockwise) {
    int oldWidth = img.getWidth();
    int oldHeight = img.getHeight();
    img.rotate90clockwise();
    EXPECT_EQ(img.getWidth(), oldHeight);
    EXPECT_EQ(img.getHeight(), oldWidth);
}

TEST_F(BMPImageTest, RotateCounterClockwise) {
    int oldWidth = img.getWidth();
    int oldHeight = img.getHeight();
    img.rotate90counter();
    EXPECT_EQ(img.getWidth(), oldHeight);
    EXPECT_EQ(img.getHeight(), oldWidth);
}

TEST_F(BMPImageTest, ApplyGaussianFilter) {
    auto oldPixels = img.getPixelData();
    img.applyGaussian3x3();
    auto newPixels = img.getPixelData();
    bool changed = false;
    for (size_t i = 0; i < oldPixels.size(); ++i)
        if (oldPixels[i] != newPixels[i]) { changed = true; break; }
    EXPECT_TRUE(changed);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

