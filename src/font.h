#ifndef FONT_H
#define FONT_H

#include "../include/stb_truetype.h"
#include <stdint.h>

typedef struct Image Image;

enum ModeType {
    SIMPLE, 
    COMPLEX
};

typedef struct Color{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

typedef struct Font{
    const char *char_list;
    char sample_character;
    stbtt_fontinfo fontinfo;
    uint8_t *ttf_buffer;
    int32_t h_scale;
    float scale;
    uint8_t pad_x;
    uint8_t pad_y;
} Font;

typedef struct BBox{
    uint32_t left;
    uint32_t right;
    uint32_t top; 
    uint32_t bottom;
} BBox;

Font* getFont(enum ModeType mode);
BBox findBoundingBox(Image *out_image, uint8_t bg_code);
void freeFont(Font *font_ptr);

extern const char *simple_alphabet;
extern const char *complex_alphabet;

#endif