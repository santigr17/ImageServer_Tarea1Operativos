#include <stdio.h>
#include <stdlib.h>
#include "ImageProcessor.h"

#define ThrowWandException(wand)                                                 \
    {                                                                            \
        char                                                                     \
            *description;                                                        \
                                                                                 \
        ExceptionType                                                            \
            severity;                                                            \
                                                                                 \
        description = MagickGetException(wand, &severity);                       \
        (void)fprintf(stderr, "%s %s %lu %s\n", GetMagickModule(), description); \
        description = (char *)MagickRelinquishMemory(description);               \
        exit(-1);                                                                \
    }

/**
 * Reads and image
 * 
 */
struct ImageInfo readImage(char *path)
{
    unsigned long int _width, _height;
    MagickWand *magick_wand = NULL;
    PixelWand *p_wand = NULL;
    MagickBooleanType status;
    magick_wand = NewMagickWand();
    status = MagickReadImage(magick_wand, path);
    if (status == MagickFalse)
    {
        ThrowWandException(magick_wand);
    }
    _width = MagickGetImageWidth(magick_wand);
    _height = MagickGetImageHeight(magick_wand);
    //printf(" Reading file: %s\n Size: width : %d pixels and height: %d pixels. \n", path, _width, _height);
    struct ImageInfo info =
        {
            magick_wand,
            _width,
            _height};
    return info;
}

/**
 * Saves the image for a given path and file name.
 */
void saveImage(MagickWand *magick_wand, char *path, char *name)
{
    char complete_path[128];
    sprintf(complete_path, "%s/%s", path, name);
    //printf(" Save File: %s\n", complete_path);
    MagickBooleanType status = MagickWriteImages(magick_wand, complete_path, MagickTrue);
    if (status == MagickFalse)
    {
        ThrowWandException(magick_wand);
    }
}

/**
 * Prinst a magic wand 
 */
void printMagicImage(MagickWand *magick_wand)
{
    PixelIterator *iterator = NULL;
    unsigned long int _width, _height;
    PixelWand **pixels = NULL;
    iterator = NewPixelIterator(magick_wand);
    _width = MagickGetImageWidth(magick_wand);
    _height = MagickGetImageHeight(magick_wand);
    for (int y = 0; y < _height; y++)
    {
        pixels = PixelGetNextIteratorRow(iterator, &_width);
        for (int x = 0; x < _width; x++)
        {
            float red = PixelGetRed(pixels[x]) * 255;
            float green = PixelGetGreen(pixels[x]) * 255;
            float blue = PixelGetBlue(pixels[x]) * 255;
            printf(" [%d,%d,%d] ",
                   (int)red,
                   (int)green,
                   (int)blue);
        }
        printf("\n");
    }
    //TODO: check if the iterator needs to be deleted.
}

/** Get a region of Magick wand
 * 
 */
PixelIterator *getRegionMakickWand(MagickWand *magick_wand, const size_t row, const size_t col, const size_t width, const size_t height)
{
    PixelIterator *iterator = NULL;
    iterator = NewPixelRegionIterator(magick_wand, row, col, width, height);
    return iterator;
}

/** Prints a pixelIteratorRegion **
 */
void printPixelIteratorRegion(PixelIterator *iterator, unsigned long int width, unsigned long int height)
{
    PixelWand **pixels = NULL;
    // TOOD: Destroy this iterator when finished.
    PixelIterator *temp = ClonePixelIterator(iterator);
    for (int y = 0; y < height; y++)
    {
        pixels = PixelGetNextIteratorRow(temp, &width);
        for (int x = 0; x < width; x++)
        {
            float red = PixelGetRed(pixels[x]) * 255;
            float green = PixelGetGreen(pixels[x]) * 255;
            float blue = PixelGetBlue(pixels[x]) * 255;
            printf(" [%d,%d,%d] ",
                   (int)red,
                   (int)green,
                   (int)blue);
        }
        printf("\n");
    }
}

/**
 * Gets a pixel that has the average value
 */
struct PixelRGB transformRegion(PixelIterator *iterator, unsigned long int width, unsigned long int height)
{
    float red_avg = 1;
    float green_avg = 1;
    float blue_avg = 1;

    PixelWand **pixels = NULL;
    // TODO: Destroy this iterator when finished.
    PixelIterator *temp = ClonePixelIterator(iterator);

    for (int y = 0; y < height; y++)
    {
        pixels = PixelGetNextIteratorRow(temp, &width);
        for (int x = 0; x < width; x++)
        {
            red_avg += PixelGetRed(pixels[x]) * 255;
            green_avg += PixelGetGreen(pixels[x]) * 255;
            blue_avg += PixelGetBlue(pixels[x]) * 255;
        }
    }
    red_avg = red_avg / (width * height);
    green_avg = green_avg / (width * height);
    blue_avg = blue_avg / (width * height);
    struct PixelRGB result = {red_avg, green_avg, blue_avg};
    return result;
}

/*
 * set the color of a region
 */
void setColorToRegion(MagickWand *magick_wand, unsigned long int row, unsigned long int col, unsigned long int width, unsigned long int height, struct PixelRGB pixelrgb)
{
    PixelIterator *temp = getRegionMakickWand(magick_wand, row, col, width, height);
    //printPixelIteratorRegion(temp, width, height);
    PixelWand **pixels = NULL;
    char hex[128];
    sprintf(hex, "#%02x%02x%02x", (int)pixelrgb.R, (int)pixelrgb.G, (int)pixelrgb.B);
    //printf("Color: %s \n", hex);
    for (int y = 0; y < height; y++)
    {
        pixels = PixelGetNextIteratorRow(temp, &width);
        for (int x = 0; x < width; x++)
        {
            /**
             * Return's 1 if correct 0 otherwise 
             */
            PixelSetColor(pixels[x], hex);
        }
        PixelSyncIterator(temp);
    }
}

/**
 * Apply Average filter
 */
void applyFilterAVG(MagickWand *magick_wand)
{
    //get dimenssions
    int _width, _height;
    _width = MagickGetImageWidth(magick_wand);
    _height = MagickGetImageHeight(magick_wand);
    int padding = 3;
    for (int y = 0; y < _height; y += padding)
    {
        for (int x = 0; x < _width; x += padding)
        {
            PixelIterator *it = getRegionMakickWand(magick_wand, x, y, padding, padding);
            struct PixelRGB pixelrgb = transformRegion(it, padding, padding);
            setColorToRegion(magick_wand, x, y, padding, padding, pixelrgb);
        }
    }
    saveImage(magick_wand, "./output", "image.png");
}