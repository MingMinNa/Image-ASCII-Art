#ifndef ASCII_ART_H
#define ASCII_ART_H

typedef struct image image;
typedef struct color color;
typedef struct arguments arguments;

double calc_block_mean(image *image_ptr, int i, int j, double cell_height, double cell_width);
color calc_block_color(image *image_ptr, int i, int j, double cell_height, double cell_width, color bg);
void text_ascii(arguments *args);
void gray_image_ascii(arguments *args);
void color_image_ascii(arguments *args);

#endif