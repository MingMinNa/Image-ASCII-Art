#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"

#include "image.h"
#include "font.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

Image* createImage(int width, int height, int channels) {
    
    Image *image_ptr = (Image *)malloc(sizeof(Image));
    image_ptr->width    = width;
    image_ptr->height   = height;
    image_ptr->channels = channels;
    image_ptr->size     = width * height * channels;
    image_ptr->data     = (uint8_t *)malloc(sizeof(uint8_t) * image_ptr->size);

    if(channels >= 3) 
        image_ptr->is_gray = false;
    else
        image_ptr->is_gray = true;

    return image_ptr;
}

Image* loadImage(const char *image_path) {

    int width, height, channels;
    uint8_t *data = stbi_load(image_path, &width, &height, &channels, 0); 
    if(data == NULL) {
        exit(1);
    }
    
    Image *image_ptr = (Image *)malloc(sizeof(Image));
    image_ptr->width    = width;
    image_ptr->height   = height;
    image_ptr->channels = channels;
    image_ptr->data     = data;
    image_ptr->size     = width * height * channels;
    
    if(channels >= 3) 
        image_ptr->is_gray = false;
    else
        image_ptr->is_gray = true;

    return image_ptr;
}

Image* toGray(Image *image_ptr) {

    int gray_channels = 0;
    
    if(image_ptr->is_gray == false)
        gray_channels = (image_ptr->channels == 4) ? (2): (1);
    else
        gray_channels = image_ptr->channels;
    
    Image *gray_image = createImage(image_ptr->width, 
                                    image_ptr->height, 
                                    gray_channels);

    for(int p1_idx = 0, p2_idx = 0; p1_idx < image_ptr->size; p1_idx += image_ptr->channels, 
                                                              p2_idx += gray_image->channels){
        
        if(image_ptr->is_gray == false && image_ptr->channels == 4) {
            // R, G, B
            gray_image->data[p2_idx] = image_ptr->data[p1_idx] * 0.299 + \
                                       image_ptr->data[p1_idx + 1] * 0.587 + \
                                       image_ptr->data[p1_idx + 2] * 0.114;
            
            gray_image->data[p2_idx + 1] = image_ptr->data[p1_idx + 3];
        }
        else if(image_ptr->is_gray == false && image_ptr->channels == 3) {
            gray_image->data[p2_idx] = image_ptr->data[p1_idx] * 0.299 + \
                                       image_ptr->data[p1_idx + 1] * 0.587 + \
                                       image_ptr->data[p1_idx + 2] * 0.114;
        }
        else if(image_ptr->is_gray == true && image_ptr->channels == 2) {
            gray_image->data[p2_idx]     = image_ptr->data[p1_idx];
            gray_image->data[p2_idx + 1] = image_ptr->data[p1_idx + 1];
        }
        else {
            gray_image->data[p2_idx]     = image_ptr->data[p1_idx];
        }
    }

    return gray_image;
}

void renderChar(uint8_t *out_image, int channels, int out_width,  int out_height,     
                char ch,            int x_pos,    int y_pos,      Font *font_ptr, 
                Color fg_color, Color bg_color) {
                    
    int w, h, xoff, yoff;

    uint8_t *bitmap = stbtt_GetCodepointBitmap(
        &font_ptr->fontinfo, 0, font_ptr->scale,
        ch, &w, &h, &xoff, &yoff);


    for (int y = 0; y < h; ++y) {
        int out_y = y_pos + y + yoff;
        if (out_y < 0 || out_y >= out_height) continue;

        for (int x = 0; x < w; ++x) {
            int out_x = x_pos + x + xoff;
            if (out_x < 0 || out_x >= out_width) continue;

            int out_idx = (out_y * out_width + out_x) * channels;

            uint8_t alpha   = bitmap[y * w + x];
            if(channels == 1) {
                uint8_t blended = (alpha * fg_color.r + (255 - alpha) * bg_color.r) / 255;
                out_image[out_idx] = blended;
            }
            else if(channels == 3) {
                out_image[out_idx + 0] = (alpha * fg_color.r + (255 - alpha) * bg_color.r) / 255;
                out_image[out_idx + 1] = (alpha * fg_color.g + (255 - alpha) * bg_color.g) / 255;
                out_image[out_idx + 2] = (alpha * fg_color.b + (255 - alpha) * bg_color.b) / 255;
            }
            
        }
    }

    stbtt_FreeBitmap(bitmap, NULL);
}

uint8_t *cropImage(uint8_t *image, int channels, int width,  int height, 
                   BBox box,       int *new_w,   int *new_h) {

    int out_w = box.right - box.left + 1;
    int out_h = box.bottom - box.top + 1;

    uint8_t *cropped = malloc(out_w * out_h * channels);

    for (int y = 0; y < out_h; ++y) {
        for (int x = 0; x < out_w; ++x) {
            for(int c = 0; c < channels; ++c) {
                cropped[(y * out_w + x) * channels + c] = image[((box.top + y) * width + (box.left + x)) * channels + c];
            }
        }
    }

    *new_w = out_w;
    *new_h = out_h;
    return cropped;
}

void saveImage(const Image *image_ptr, const char *output_image_path) {

    // Check whether the image extension is valid. (.jpg / .JPG / .jpeg / .JPEG / .png / .PNG)
    char *valid_exts[] = {
        ".jpg", ".JPG", ".jpeg", ".JPEG", ".png", ".PNG"
    };

    for(int i = 0; i < 4; ++i) {
        if(checkExtension(output_image_path, valid_exts[i])) {
            stbi_write_jpg( output_image_path,    image_ptr->width, image_ptr->height, 
                            image_ptr->channels, image_ptr->data,  100);
            return;
        }
    }

    for(int i = 4; i < 6; ++i) {
        if(checkExtension(output_image_path, valid_exts[i])) {
            stbi_write_jpg( output_image_path,    image_ptr->width, image_ptr->height, 
                            image_ptr->channels, image_ptr->data,  image_ptr->width * image_ptr->channels);
            return;
        }
    }
    CHECK_ERROR(false, "Invalid file extension");
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
}