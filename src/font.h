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
void renderChar(uint8_t *out_image, int out_width, int out_height,
                char ch, int x_pos, int y_pos, Font *font_ptr, uint8_t bg_code);
BBox findBoundingBox(uint8_t *image, int width, int height, uint8_t bg);
uint8_t* cropImage(uint8_t *image, int width, int height, BBox box, int *new_w, int *new_h);
void freeFont(Font *font_ptr);

#endif