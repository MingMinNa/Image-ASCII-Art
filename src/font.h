#ifndef FONT_H
#define FONT_H

#include "../include/stb_truetype.h"
#include <stdint.h>

extern const char *simple_alphabet;
extern const char *complex_alphabet;

enum ModeType {
    SIMPLE, 
    COMPLEX
};

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

typedef struct {
    const char *char_list;
    char sample_character;
    stbtt_fontinfo fontinfo;
    uint8_t *ttf_buffer;
    float scale;
    int h_scale;
} Font;

typedef struct {
    int left;
    int right;
    int top; 
    int bottom;
} BBox;

Font* getFont(enum ModeType mode);
BBox findBoundingBox(uint8_t *image, int channels, int width, int height, uint8_t bg);
void freeFont(Font *font_ptr);

#endif