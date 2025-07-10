#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct Font Font;
typedef struct Color Color;
typedef struct BBox BBox;

typedef struct Image{
    int32_t channels;
    int32_t width;     
    int32_t height;
    int64_t size;
    uint8_t* data;
    bool is_gray;
} Image;

Image* createImage(int32_t width, int32_t height, int32_t channels, uint8_t *data);
Image* loadImage(const char *image_path);
Image* toGray(Image *image_ptr);
void renderChar(Image *out_image, int32_t x_pos , int32_t y_pos, char ch,  
                Font *font_ptr  , Color fg_color, Color bg_color);
Image* cropImage(Image *out_image, BBox box);
void saveImage(const Image *image_ptr, const char *output_image_path);
void freeImage(Image *image_ptr);

#endif