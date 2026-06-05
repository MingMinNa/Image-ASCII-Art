#ifndef FONT_H
#define FONT_H

#include "../stb/stb_truetype.h"
#include <stdint.h>

typedef struct image image;

enum mode_type {
    SIMPLE, 
    COMPLEX
};

typedef struct color{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} color;

typedef struct font{
    const char *char_list;
    char sample_character;
    stbtt_fontinfo fontinfo;
    uint8_t *ttf_buffer;
    int32_t h_scale;
    float scale;
    uint8_t pad_x;
    uint8_t pad_y;
} font;

typedef struct bbox{
    uint32_t left;
    uint32_t right;
    uint32_t top; 
    uint32_t bottom;
} bbox;

font* get_font(enum mode_type mode);
bbox find_bounding_box(image *out_image, uint8_t bg_code);
void free_font(font *font_ptr);

extern const char *simple_alphabet;
extern const char *complex_alphabet;

#endif