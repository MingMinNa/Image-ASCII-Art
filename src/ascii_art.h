#ifndef ASCII_ART_H
#define ASCII_ART_H

#include "arg_parser.h"
#include "image.h"

extern const char *simple_alphabet;
extern const char *complex_alphabet;

double calcBlockMean(Image *image_ptr, int i, int j, double cell_height, double cell_width);
void TextAscii(Arguments *args);
void ImageAscii(Arguments *args);

#endif