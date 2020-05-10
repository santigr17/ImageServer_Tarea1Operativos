#include "ImageProcessor.h"

/** This program test an image reader */
int main(int argc, char **argv)
{
    struct ImageInfo info = readImage(argv[1]);
    printf(" Reading file: %s\n Size: width : %d pixels and height: %d pixels. \n", argv[1], info._width, info._height);
    // //printMagicImage(info.magick_wand);
    // PixelIterator *it = getRegionMakickWand(info.magick_wand, 10, 10, 3, 3);
    // printPixelIteratorRegion(it, 3, 3);
    // struct PixelRGB pixelrgb = transformRegion(it, 3, 3);
    // printf(" Avg: %f,%f,%f \n", pixelrgb.R, pixelrgb.G, pixelrgb.B);
    // struct PixelRGB color = { (float)255,(float)0,(float)0 };
    // setColorToRegion(info.magick_wand, 1, 1, 3, 3,pixelrgb);
    // saveImage(info.magick_wand, "./output", "image.png");
    applyFilterAVG(info.magick_wand);
}