#define STB_TRUETYPE_IMPLEMENTATION

#include "utils.h"
#include "font.h"
#include "image.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

const char *simple_alphabet  = "@%#*+=-:. ";
const char *complex_alphabet = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";

Font* getFont(enum ModeType mode) {
    
    Font *font_ptr = (Font *)malloc(sizeof(Font));

    if(mode == SIMPLE)  font_ptr->char_list = simple_alphabet;
    else                font_ptr->char_list = complex_alphabet;
    
    font_ptr->sample_character = 'A';
    font_ptr->pad_x = 1;
    font_ptr->pad_y = 2;

    FILE *fp = fopen("fonts/DejaVuSansMono-Bold.ttf", "rb");
    CHECK_ERROR(fp == NULL, "Fail to load font");

    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    rewind(fp);

    font_ptr->ttf_buffer = (uint8_t *)malloc(size);
    size_t _ = fread(font_ptr->ttf_buffer, 1, size, fp);
    fclose(fp);

    if (!stbtt_InitFont(&font_ptr->fontinfo, font_ptr->ttf_buffer, 0)) {
        free(font_ptr->ttf_buffer); 
        CHECK_ERROR(true, "Fail to initialize font");
    }

    float pixel_height = 20.0f;
    font_ptr->scale    = stbtt_ScaleForPixelHeight(&font_ptr->fontinfo, pixel_height);
    font_ptr->h_scale  = 2;
    
    return font_ptr;
}

BBox findBoundingBox(Image *out_image, uint8_t bg_code) {

    const int32_t height   = out_image->height, 
                  width    = out_image->width, 
                  channels = out_image->channels;

    int32_t left = width, right = -1, 
            top = height, bottom = -1;

    for (int32_t y = 0; y < height; ++y) {
        for (int32_t x = 0; x < width; ++x) {

            bool is_bg = true;

            for(int32_t c = 0; c < channels; ++ c) {
                uint8_t pixel = out_image->data[(y * width + x) * channels + c];
                if(pixel != bg_code) {
                    is_bg = false;
                    break;
                }
            }

            if (!is_bg) {
                if (x < left) left = x;
                if (x > right) right = x;
                if (y < top) top = y;
                if (y > bottom) bottom = y;
            }
        }
    }

    if (right == -1) {
        left = top = 0;
        right = width - 1;
        bottom = height - 1;
    }

    BBox box = {
        .left = left, 
        .right = right,
        .top = top,  
        .bottom = bottom
    };

    return box;
}

void freeFont(Font *font_ptr) {

    if(font_ptr == NULL) 
        return;

    if (font_ptr->ttf_buffer != NULL) 
        free(font_ptr->ttf_buffer);
    free(font_ptr);
}