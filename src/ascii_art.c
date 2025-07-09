#include "../include/stb_image_write.h"

#include "image.h"
#include "utils.h"
#include "ascii_art.h"
#include "arg_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double calcBlockMean(
    Image *image_ptr,
    int i, 
    int j, 
    double cell_height, 
    double cell_width) {

    uint32_t channels = image_ptr->channels;
    uint32_t height   = image_ptr->height;
    uint32_t width    = image_ptr->width;

    uint64_t sum = 0;
    uint32_t count = 0;
    
    uint32_t outer_loop_bound[] = {
        (uint32_t)(i * cell_height), 
        min((uint32_t)((i + 1) * cell_height), height)
    };

    uint32_t inner_loop_bound[] = {
        (uint32_t)(j * cell_width),
        min((uint32_t)((j + 1) * cell_width), width)
    };

    for(uint32_t k = outer_loop_bound[0]; k < outer_loop_bound[1]; ++k) {
        for(uint32_t l = inner_loop_bound[0]; l < inner_loop_bound[1]; ++l) {
            sum += image_ptr->data[width * channels * k + channels * l];
            ++ count;
        }
    }
    if(count == 0) return 0.0;
    return sum * 1.0 / count;
}

void TextAscii(Arguments *args) {

    const char *input_image_path = args->input_path;
    const char *output_text_path = args->output_path;
    uint32_t num_cols = args->num_cols;

    const char *alphabet;
    if(args->mode == SIMPLE) alphabet = simple_alphabet;
    else                     alphabet = complex_alphabet;

    Image *raw_image = loadImage(input_image_path);
    Image *gray_image = toGray(raw_image);

    double cell_width  = gray_image->width * 1.0 / num_cols;
    double cell_height = 2.0 * cell_width;

    uint32_t num_rows = (uint32_t)(gray_image->height / cell_height);

    if( num_cols > gray_image->width ||
        num_rows > gray_image->height){
        cell_width = 6.0;
        cell_height = 12.0;
        num_cols = (uint32_t)(gray_image->width  / cell_width); 
        num_rows = (uint32_t)(gray_image->height / cell_height);
    }

    size_t num_chars = strlen(alphabet);
    FILE *output_fp = fopen(output_text_path, "w");
    CHECK_ERROR(output_fp == NULL, "Fail to open the file \"%s\"", output_text_path);

    for(uint32_t i = 0; i < num_rows; ++i) {
        for(uint32_t j = 0; j < num_cols; ++j) {
            double mean = calcBlockMean(gray_image, i, j, cell_height, cell_width);
            uint32_t char_idx = (uint32_t)(mean * num_chars / 255.0);
            fwrite(&alphabet[min(char_idx, num_chars - 1)], sizeof(char), 1, output_fp);
        }
        char newline = '\n';
        fwrite(&newline, sizeof(char), 1, output_fp);
    }
    fclose(output_fp);
    freeImage(raw_image);
    freeImage(gray_image);
    return;
}

void ImageAscii(Arguments *args) {

    Font *font_ptr = getFont(args->mode);

    const char *input_image_path = args->input_path;
    const char *output_image_path = args->output_path;
    size_t num_chars = strlen(font_ptr->char_list);
    uint32_t num_cols = args->num_cols;

    Image *raw_image = loadImage(input_image_path);
    Image *gray_image = toGray(raw_image);

    int h_scale = font_ptr->h_scale;
    double cell_width  = gray_image->width * 1.0 / num_cols;
    double cell_height = h_scale * cell_width;

    uint32_t num_rows = (uint32_t)(gray_image->height / cell_height);

    if( num_cols > gray_image->width ||
        num_rows > gray_image->height){
        cell_width = 6.0;
        cell_height = 12.0;
        num_cols = (uint32_t)(gray_image->width  / cell_width); 
        num_rows = (uint32_t)(gray_image->height / cell_height);
    }

    int x0, y0, x1, y1;
    stbtt_GetCodepointBitmapBox(&font_ptr->fontinfo, font_ptr->sample_character, font_ptr->scale, font_ptr->scale, &x0, &y0, &x1, &y1);

    int pad_x = 1;
    int pad_y = 2;

    int char_width  = x1 - x0 + pad_x;
    int char_height = y1 - y0 + pad_y;

    int out_width  = char_width * num_cols;
    int out_height = h_scale * char_height * num_rows;

    uint8_t *out_image = (uint8_t *)malloc(out_width * out_height);
    memset(out_image, args->bg_code, out_width * out_height);

    for(int i = 0; i < num_rows; ++i) {
        for(int j = 0; j < num_cols; ++j) {
            double mean = calcBlockMean(gray_image, i, j, cell_height, cell_width);
            uint32_t char_idx = (uint32_t)(mean * num_chars / 255.0);
            char ch = font_ptr->char_list[min(char_idx, num_chars - 1)];

            int x_pos = j * char_width;
            int y_pos = i * char_height;

            renderChar(out_image, out_width, out_height, ch, x_pos, y_pos, font_ptr, args->bg_code);
        }
    }

    BBox bbox = findBoundingBox(out_image, out_width, out_height, args->bg_code);
    uint8_t *cropped_image = cropImage(out_image, out_width, out_height, bbox, &out_width, &out_height);

    if(isPNGFile(output_image_path)) {
        stbi_write_png(output_image_path, out_width, out_height, 1, cropped_image, out_width);
    }
    else if(isJPGFile(output_image_path)) {
        stbi_write_jpg(output_image_path, out_width, out_height, 1, cropped_image, out_width);
    }
    else {
        CHECK_ERROR(true, "Invalid output image format (%s)", output_image_path);
    }

    free(out_image);
    free(cropped_image);
    freeFont(font_ptr);
    freeImage(raw_image);
    freeImage(gray_image);
    return;
}

