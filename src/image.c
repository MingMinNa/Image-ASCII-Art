#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"

#include "font.h"
#include "utils.h"
#include "image.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

Image* createImage(int32_t width, int32_t height, int32_t channels, uint8_t *data) {
    
    Image *image_ptr    = (Image *)malloc(sizeof(Image));
    image_ptr->width    = width;
    image_ptr->height   = height;
    image_ptr->channels = channels;
    image_ptr->size     = width * height * channels;
    
    if(data == NULL)
        image_ptr->data = (uint8_t *)malloc(sizeof(uint8_t) * image_ptr->size);
    else
        image_ptr->data = data;

    if(channels >= 3) 
        image_ptr->is_gray = false;
    else
        image_ptr->is_gray = true;

    return image_ptr;
}

Image* loadImage(const char *image_path) {

    int32_t width, height, channels;
    uint8_t *data = stbi_load(image_path, &width, &height, &channels, 0); 

    CHECK_ERROR(data == NULL, "Fail to load the image \"%s\"", image_path);

    Image *image_ptr = createImage(width, height, channels, data);

    return image_ptr;
}

Image* toGray(Image *image_ptr) {

    int32_t gray_channels = 0;
    
    if(image_ptr->is_gray == false)
        gray_channels = (image_ptr->channels == 4) ? (2): (1);
    else
        gray_channels = image_ptr->channels;
    
    Image *gray_image = createImage(
        image_ptr->width, 
        image_ptr->height, 
        gray_channels, 
        NULL 
    );

    int32_t p1_idx = 0, p1_step = image_ptr->channels;
    int32_t p2_idx = 0, p2_step = gray_image->channels;

    // formula: grayscale = (0.299 * red) + (0.587 * green) + (0.114 * blue)
    // OpenCV reference: https://docs.opencv.org/3.4/de/d25/imgproc_color_conversions.html 
    for(; p1_idx < image_ptr->size; p1_idx += p1_step, p2_idx += p2_step){
        
        // [0]: red, [1]: green, [2]: blue, [3]: alpha
        if( image_ptr->is_gray  == false && 
            image_ptr->channels == 4) {

            gray_image->data[p2_idx] = image_ptr->data[p1_idx] * 0.299 + \
                                       image_ptr->data[p1_idx + 1] * 0.587 + \
                                       image_ptr->data[p1_idx + 2] * 0.114;
            
            gray_image->data[p2_idx + 1] = image_ptr->data[p1_idx + 3];
        }
        // [0]: red, [1]: green, [2]: blue
        else if(
            image_ptr->is_gray  == false && 
            image_ptr->channels == 3) {

            gray_image->data[p2_idx] = image_ptr->data[p1_idx] * 0.299 + \
                                       image_ptr->data[p1_idx + 1] * 0.587 + \
                                       image_ptr->data[p1_idx + 2] * 0.114;
        }
        // [0]: grayscale, [1]: alpha
        else if(
            image_ptr->is_gray  == true && 
            image_ptr->channels == 2) {
                
            gray_image->data[p2_idx]     = image_ptr->data[p1_idx];
            gray_image->data[p2_idx + 1] = image_ptr->data[p1_idx + 1];
        }
        // [0]: grayscale
        else if(
            image_ptr->is_gray  == true && 
            image_ptr->channels == 1) {
                
            gray_image->data[p2_idx]     = image_ptr->data[p1_idx];
        }
    }

    return gray_image;
}

void renderChar(Image *out_image, int32_t x_pos , int32_t y_pos, char ch,  
                Font *font_ptr  , Color fg_color, Color bg_color) {

    const int32_t out_w = out_image->width;
    const int32_t out_h = out_image->height;
    const int32_t channels = out_image->channels;
    
    int w, h, xoff, yoff;

    uint8_t *bitmap = stbtt_GetCodepointBitmap(
        &font_ptr->fontinfo, 0, font_ptr->scale,
        ch, &w, &h, &xoff, &yoff);

    for (int y = 0; y < h; ++y) {
        int out_y = y_pos + y + yoff;
        if (out_y < 0 || out_y >= out_h) continue;

        for (int x = 0; x < w; ++x) {
            int out_x = x_pos + x + xoff;
            if (out_x < 0 || out_x >= out_w) continue;

            int out_idx = (out_y * out_w + out_x) * channels;

            uint8_t alpha = bitmap[y * w + x];
            if(channels == 1) {
                uint8_t blended = (alpha * fg_color.r + (255 - alpha) * bg_color.r) / 255;
                out_image->data[out_idx] = blended;
            }
            else if(channels == 3) {
                out_image->data[out_idx + 0] = (alpha * fg_color.r + (255 - alpha) * bg_color.r) / 255;
                out_image->data[out_idx + 1] = (alpha * fg_color.g + (255 - alpha) * bg_color.g) / 255;
                out_image->data[out_idx + 2] = (alpha * fg_color.b + (255 - alpha) * bg_color.b) / 255;
            }
            
        }
    }

    stbtt_FreeBitmap(bitmap, NULL);
}

Image* cropImage(Image *out_image, BBox box) {

    const int32_t out_w = out_image->width;
    const int32_t channels = out_image->channels;

    int32_t cropped_w = box.right - box.left + 1;
    int32_t cropped_h = box.bottom - box.top + 1;

    uint8_t *cropped = malloc(cropped_w * cropped_h * channels);

    for (int y = 0; y < cropped_h; ++y) {
        for (int x = 0; x < cropped_w; ++x) {
            for(int c = 0; c < channels; ++c) {
                cropped[(y * cropped_w + x) * channels + c] 
                    = out_image->data[((box.top + y) * out_w + (box.left + x)) * channels + c];
            }
        }
    }

    Image *cropped_image = createImage(cropped_w, cropped_h, channels, cropped);

    return cropped_image;
}

void saveImage(const Image *image_ptr, const char *output_image_path) {

    if(isPNGFile(output_image_path)) {
        
        stbi_write_png( output_image_path  , image_ptr->width, image_ptr->height, 
                        image_ptr->channels, image_ptr->data , image_ptr->width * image_ptr->channels);
        return;
    }
    else if(isJPGFile(output_image_path)) {

        stbi_write_jpg( output_image_path  , image_ptr->width, image_ptr->height, 
                        image_ptr->channels, image_ptr->data , 100);
        return;
    }

    CHECK_ERROR(true, "Invalid output image format (%s)", output_image_path);
}

void freeImage(Image *image_ptr) {
    
    if(image_ptr == NULL) return;

    if(image_ptr->data != NULL) {
        stbi_image_free(image_ptr->data);
    }

    image_ptr->width = 0;
    image_ptr->height = 0;
    image_ptr->data = NULL;
    image_ptr->channels = 0;
    free(image_ptr);
}