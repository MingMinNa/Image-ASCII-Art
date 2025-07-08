#ifndef IMAGE_H
#define IMAGE_H

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
void saveImage(const Image *image_ptr, const char *output_image_path);
void freeImage(Image *image_ptr);

#endif