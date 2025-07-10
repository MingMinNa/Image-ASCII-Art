#ifndef ASCII_ART_H
#define ASCII_ART_H

typedef struct Image Image;
typedef struct Color Color;
typedef struct Arguments Arguments;

double calcBlockMean(Image *image_ptr, int i, int j, double cell_height, double cell_width);
Color calcBlockColor(Image *image_ptr, int i, int j, double cell_height, double cell_width, Color bg);
void TextAscii(Arguments *args);
void GrayImageAscii(Arguments *args);
void ColorImageAscii(Arguments *args);

#endif