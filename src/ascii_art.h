#ifndef ASCII_ART_H
#define ASCII_ART_H

#include "arg_parser.h"
#include "font.h"
#include "image.h"

double calcBlockMean(Image *image_ptr, int i, int j, double cell_height, double cell_width);
Color calcBlockColor(Image *image_ptr, int i, int j, double cell_height, double cell_width, Color bg);
void TextAscii(Arguments *args);
void ImageAscii(Arguments *args);
void ImageColorAscii(Arguments *args);

#endif