#ifndef IMAGE_H
#define IMAGE_H

#include "font.h"

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t channels;
    uint32_t width;     
    uint32_t height;
    uint8_t* data;
    uint64_t size;
    bool is_gray;
} Image;

Image* createImage(int width, int height, int channels);
Image* loadImage(const char *image_path);
Image* toGray(Image *image_ptr);
void renderChar(uint8_t *out_image, int channels, int out_width,  int out_height,     
                char ch,            int x_pos,    int y_pos,      Font *font_ptr, 
                Color fg_color, Color bg_color);
uint8_t *cropImage(uint8_t *image, int channels, int width,  int height, 
                   BBox box,       int *new_w,   int *new_h);
void saveImage(const Image *image_ptr, const char *output_image_path);
void freeImage(Image *image_ptr);

#endif