#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct font font;
typedef struct color color;
typedef struct bbox bbox;

typedef struct image{
    int32_t channels;
    int32_t width;     
    int32_t height;
    int64_t size;
    uint8_t* data;
    bool is_gray;
} image;

image* create_image(int32_t width, int32_t height, int32_t channels, uint8_t *data);
image* load_image(const char *image_path);
image* to_gray(image *image_ptr);
void render_char(
    image *out_image, int32_t x_pos , int32_t y_pos, char ch,  
    font *font_ptr  , color fg_color, color bg_color
);
image* crop_image(image *out_image, bbox box);
void save_image(const image *image_ptr, const char *output_image_path);
void free_image(image *image_ptr);

#endif