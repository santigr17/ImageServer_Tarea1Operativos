#ifndef HEADER_IMAGE_PROCESSOR_FILE
#define HEADER_IMAGE_PROCESSOR_FILE
#include <wand/MagickWand.h>

/**
 * Holds and image wand pointer and its dimensions 
 */
struct ImageInfo {
    MagickWand *magick_wand;
    unsigned long int _width, _height;
}; 
struct PixelRGB {
    float R;
    float G;
    float B;
};
/**
 * Reads an image and then return a Image info struct
 */
struct ImageInfo readImage(char *path);
/**
 * Save an image
 */
void saveImage (MagickWand *magick_wand, char *path, char *name );

/*
 * Prints and RGB matrix
 */
void printMagicImage (MagickWand *magick_wand);
/**
 * Gets a pixel region iterator
 */
PixelIterator *getRegionMakickWand(MagickWand *magick_wand, const size_t row, const size_t col, const size_t width, const size_t height);

/**
 * Print iterator
 */
void printPixelIteratorRegion(PixelIterator *iterator, const size_t width, const size_t height);

/**
Transform a region
 * */
struct PixelRGB transformRegion (PixelIterator *iterator, unsigned long int width, unsigned long int height );

/*
 * set the color of a region
 */
void setColorToRegion (MagickWand *magick_wand, unsigned long int row, unsigned long int col, unsigned long int width, unsigned long int height, struct PixelRGB pixelrgb); 
/**
 * Applies AVG filter.
 */ 
void applyFilterAVG(MagickWand *magick_wand);
#endif