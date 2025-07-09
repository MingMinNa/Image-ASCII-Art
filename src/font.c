#define STB_TRUETYPE_IMPLEMENTATION
// #include "../include/stb_truetype.h"

#include "font.h"
#include "utils.h"
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
    font_ptr->scale = stbtt_ScaleForPixelHeight(&font_ptr->fontinfo, pixel_height);
    font_ptr->h_scale = 2;
    
    return font_ptr;
}

void renderChar(uint8_t *out_image, int out_width, int out_height,
                char ch, int x_pos, int y_pos, Font *font_ptr, uint8_t bg_code) {
                    
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

            uint8_t alpha   = bitmap[y * w + x];
            uint8_t blended = (alpha * (255 - bg_code) + (255 - alpha) * bg_code) / 255;

            out_image[out_y * out_width + out_x] = blended;
        }
    }

    stbtt_FreeBitmap(bitmap, NULL);
}

BBox findBoundingBox(uint8_t *image, int width, int height, uint8_t bg) {
    int left = width, right = -1, top = height, bottom = -1;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            uint8_t pixel = image[y * width + x];

            if (pixel != bg) {
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

uint8_t *cropImage(uint8_t *image, int width, int height, BBox box, int *new_w, int *new_h) {
    int out_w = box.right - box.left + 1;
    int out_h = box.bottom - box.top + 1;

    uint8_t *cropped = malloc(out_w * out_h);

    for (int y = 0; y < out_h; ++y) {
        for (int x = 0; x < out_w; ++x) {
            cropped[y * out_w + x] = image[(box.top + y) * width + (box.left + x)];
        }
    }

    *new_w = out_w;
    *new_h = out_h;
    return cropped;
}

void freeFont(Font *font_ptr) {

    if(font_ptr == NULL) return;
    if (font_ptr->ttf_buffer) free(font_ptr->ttf_buffer);
    free(font_ptr);
}