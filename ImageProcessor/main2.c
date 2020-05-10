#include <stdio.h>
#include <stdlib.h>
#include <wand/MagickWand.h>

#define ThrowWandException(wand)                                             \
  {                                                                          \
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

void applyFilter(MagickWand *magick_wand_source, char *name)
{
  unsigned long int _width, _height;
  PixelWand **pixels = NULL;
  PixelIterator *iterator = NULL;

  // Get a 9-pixel region at coordinate 0,
  _width = MagickGetImageWidth(magick_wand_source);
  _height = MagickGetImageHeight(magick_wand_source);

  //Variables for the destination image
  MagickWand *magick_wand_destination = NULL;
  PixelWand **pixels_destination = NULL;
  PixelIterator *iterator_destination;
  PixelWand *p_wand_destination = NULL;
  magick_wand_destination = NewMagickWand();
  p_wand_destination = NewPixelWand();
  MagickNewImage(magick_wand_destination, _width, _height, p_wand_destination);
  char hex[128];

  for (int row_section = 0; row_section < _height; row_section += 3)
  {
    for (int col_section = 0; col_section < _width; col_section += 3)
    {
      //  printf(" row : %d col %d\n",
      //    row_section,
      //    col_section);

      iterator = NewPixelRegionIterator(magick_wand_source, row_section, col_section, 3, 3);
      float accumulated_red = 0;
      float accumulated_green = 0;
      float accumulated_blue = 0;
      for (int y = 0; y < 3; y++)
      {
        pixels = PixelGetNextIteratorRow(iterator, &_width);
        for (int x = 0; x < 3; x++)
        {
          float red = PixelGetRed(pixels[x]) * 255;
          float green = PixelGetGreen(pixels[x]) * 255;
          float blue = PixelGetBlue(pixels[x]) * 255;
          accumulated_red += red;
          accumulated_green += green;
          accumulated_blue += blue;
          printf(" R: %f G: %f  B %f  X: %d  Y: %d\n",
                 red,
                 green,
                 blue,
                 x, y);
        }
      }
      accumulated_red = accumulated_red / 9;
      accumulated_green = accumulated_green / 9;
      accumulated_blue = accumulated_blue / 9;
      printf(" Avg R: %f Avg G: %f  Avg B %f  \n",
             accumulated_red,
             accumulated_green,
             accumulated_blue);
      iterator_destination = NewPixelRegionIterator(magick_wand_destination, row_section, col_section, 3, 3);

      for (int y = 0; y < 3; y++)
      {
        pixels_destination = PixelGetNextIteratorRow(iterator_destination, &_width);
        for (int x = 0; x < 3; x++)
        {
          sprintf(hex, "#%02x%02x%02x", accumulated_red, accumulated_green, accumulated_blue);
          PixelSetColor(pixels_destination[x], hex);
        }
      }
    }
  }
  MagickBooleanType status = MagickWriteImages(magick_wand_destination, name, MagickTrue);
  if (status == MagickFalse)
    ThrowWandException(magick_wand_destination);
}

int main(int argc, char **argv)
{

  //Amount columns and rows
  unsigned long int _width, _height;
  MagickWand *magick_wand = NULL;
  PixelWand **pixels = NULL;
  PixelWand *p_wand = NULL;
  PixelIterator *iterator;
  MagickBooleanType status;
  char hex[128];

  //Variables for new variable media
  MagickWand *new_magick_wand = NULL;
  PixelWand **new_pixels = NULL;
  PixelIterator *new_iterator;
  PixelWand *new_p_wand = NULL;

  //Variables for new variable mediana
  MagickWand *new_magick_wand2 = NULL;
  PixelWand **new_pixels2 = NULL;
  PixelIterator *new_iterator2;
  PixelWand *new_p_wand2 = NULL;

  if (argc != 5)
  {
    (void)fprintf(stdout, "Usage: %s image thumbnail\n", argv[0]);
    exit(0);
  }
  MagickWandGenesis();
  magick_wand = NewMagickWand();
  new_magick_wand = NewMagickWand();
  new_magick_wand2 = NewMagickWand();
  /*new_p_wand
    Read an image.
  */
  status = MagickReadImage(magick_wand, argv[1]);

  if (status == MagickFalse)
  {
    ThrowWandException(magick_wand);
  }
  // Get the image's width and heightfor (int y = 0; y < _height; y++)printf(" R: %f G: %f  B %f  X: %d  Y: %d\n",
  //  red,
  //  green,
  //  blue,
  //  x, y);
  _width = MagickGetImageWidth(magick_wand);
  _height = MagickGetImageHeight(magick_wand);
  applyFilter(magick_wand, argv[4]);
  //printf(" W: %d H: %d\n", _width, _height);

  /**
   * Create Image
   */
  new_p_wand = NewPixelWand();
  MagickNewImage(new_magick_wand, _width, _height, new_p_wand);
  new_p_wand2 = NewPixelWand();
  MagickNewImage(new_magick_wand2, _width, _height, new_p_wand2);

  iterator = NewPixelIterator(magick_wand);
  new_iterator = NewPixelIterator(new_magick_wand);
  new_iterator2 = NewPixelIterator(new_magick_wand2);

  for (int y = 0; y < _height; y++)
  {
    pixels = PixelGetNextIteratorRow(iterator, &_width);
    new_pixels = PixelGetNextIteratorRow(new_iterator, &_width);
    new_pixels2 = PixelGetNextIteratorRow(new_iterator2, &_width);
    for (int x = 0; x < _width; x++)
    {
      float red = PixelGetRed(pixels[x]) * 255;
      float green = PixelGetGreen(pixels[x]) * 255;
      float blue = PixelGetBlue(pixels[x]) * 255;
      int gray = (red + green + blue) / 3;
      int mediana_gray = 0;
      if ((red <= green && green <= blue) || (blue <= green && green <= red))
      {
        mediana_gray = green;
      }
      if ((green <= red && red <= blue) || (blue <= red && red <= green))
      {
        mediana_gray = red;
      }
      if ((green <= blue && blue <= red) || (red <= blue && blue <= green))
      {
        mediana_gray = blue;
      }
      // printf(" R: %f G: %f  B %f  X: %d  Y: %d\n",
      //        red,
      //        green,
      //        blue,
      //        x, y);
      sprintf(hex, "#%02x%02x%02x", gray, gray, gray);
      PixelSetColor(new_pixels[x], hex);
      sprintf(hex, "#%02x%02x%02x", mediana_gray, mediana_gray, mediana_gray);
      PixelSetColor(new_pixels2[x], hex);
    }
    PixelSyncIterator(iterator);
    PixelSyncIterator(new_iterator);
    PixelSyncIterator(new_iterator2);
  }

  /*
    Write the image then destroy it.
  */
  status = MagickWriteImages(new_magick_wand, argv[2], MagickTrue);
  if (status == MagickFalse)
    ThrowWandException(new_magick_wand);

  status = MagickWriteImages(new_magick_wand2, argv[3], MagickTrue);
  if (status == MagickFalse)
    ThrowWandException(new_magick_wand2);

  magick_wand = DestroyMagickWand(magick_wand);
  new_magick_wand = DestroyMagickWand(new_magick_wand);
  new_magick_wand2 = DestroyMagickWand(new_magick_wand2);
  MagickWandTerminus();
  return (0);
}