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

    uint32_t outer_loop_bound[] = {
        (uint32_t)(i * cell_height), 
        min((uint32_t)((i + 1) * cell_height), height)
    };

    uint32_t inner_loop_bound[] = {
        (uint32_t)(j * cell_width),
        min((uint32_t)((j + 1) * cell_width), width)
    };

    int block_h = outer_loop_bound[1] - outer_loop_bound[0];
    int block_w = inner_loop_bound[1] - inner_loop_bound[0];
    
    uint64_t sum = 0;
    int count = block_w * block_h;

    for(uint32_t k = outer_loop_bound[0]; k < outer_loop_bound[1]; ++k) {
        for(uint32_t l = inner_loop_bound[0]; l < inner_loop_bound[1]; ++l) {
            sum += image_ptr->data[width * channels * k + channels * l];
        }
    }

    if(count == 0) return 0.0;
    return sum * 1.0 / count;
}

Color calcBlockColor(
    Image *image_ptr, 
    int i, 
    int j, 
    double cell_height, 
    double cell_width, 
    Color bg) {

    uint32_t channels = image_ptr->channels;
    uint32_t height   = image_ptr->height;
    uint32_t width    = image_ptr->width;

    uint32_t outer_loop_bound[] = {
        (uint32_t)(i * cell_height), 
        min((uint32_t)((i + 1) * cell_height), height)
    };

    uint32_t inner_loop_bound[] = {
        (uint32_t)(j * cell_width),
        min((uint32_t)((j + 1) * cell_width), width)
    };

    if(outer_loop_bound[1] <= outer_loop_bound[0] || 
       inner_loop_bound[1] <= inner_loop_bound[0])
        return bg;
    
    int block_h = outer_loop_bound[1] - outer_loop_bound[0];
    int block_w = inner_loop_bound[1] - inner_loop_bound[0];

    uint32_t r_sum = 0;
    uint32_t g_sum = 0;
    uint32_t b_sum = 0;

    for(int i = outer_loop_bound[0]; i < outer_loop_bound[1]; ++i) {
        for(int j = inner_loop_bound[0]; j < inner_loop_bound[1]; ++j) {
            r_sum += image_ptr->data[(i * width + j) * channels + 0];
            g_sum += image_ptr->data[(i * width + j) * channels + 1];
            b_sum += image_ptr->data[(i * width + j) * channels + 2];
        }
    }

    Color ret_color = { 
        .r = (uint8_t)(r_sum * 1.0 / (block_h * block_w)),
        .g = (uint8_t)(g_sum * 1.0 / (block_h * block_w)),
        .b = (uint8_t)(b_sum * 1.0 / (block_h * block_w)),
    };
    return ret_color;
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
    size_t num_chars = strlen(font_ptr->char_list);

    const char *input_image_path = args->input_path;
    const char *output_image_path = args->output_path;
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
    int out_channels = 1;

    uint8_t *out_image = (uint8_t *)malloc(out_width * out_height * out_channels);
    memset(out_image, args->bg_code, out_width * out_height * out_channels);

    Color bg_color = {
        .r = args->bg_code,
        .g = args->bg_code,
        .b = args->bg_code,
    };
    Color fg_color = {
        .r = 255 - bg_color.r,
        .b = 255 - bg_color.b,
        .g = 255 - bg_color.g,
    };

    for(int i = 0; i < num_rows; ++i) {
        for(int j = 0; j < num_cols; ++j) {
            double mean = calcBlockMean(gray_image, i, j, cell_height, cell_width);
            uint32_t char_idx = (uint32_t)(mean * num_chars / 255.0);
            char ch = font_ptr->char_list[min(char_idx, num_chars - 1)];

            int x_pos = j * char_width;
            int y_pos = i * char_height;

            renderChar(out_image, out_channels, out_width, out_height, 
                       ch,        x_pos,        y_pos,     font_ptr, 
                       fg_color,  bg_color);
        }
    }

    BBox bbox = findBoundingBox(out_image, out_channels, out_width, out_height, args->bg_code);
    uint8_t *cropped_image = cropImage(out_image, out_channels, out_width, out_height, bbox, &out_width, &out_height);

    if(isPNGFile(output_image_path)) {
        stbi_write_png(output_image_path, out_width, out_height, out_channels, cropped_image, out_width * out_channels);
    }
    else if(isJPGFile(output_image_path)) {
        stbi_write_jpg(output_image_path, out_width, out_height, out_channels, cropped_image, out_width * out_channels);
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

void ImageColorAscii(Arguments *args) {

    Font *font_ptr = getFont(args->mode);
    size_t num_chars = strlen(font_ptr->char_list);

    const char *input_image_path = args->input_path;
    const char *output_image_path = args->output_path;
    uint32_t num_cols = args->num_cols;

    Image *raw_image = loadImage(input_image_path);
    
    int h_scale = font_ptr->h_scale;
    double cell_width  = raw_image->width * 1.0 / num_cols;
    double cell_height = h_scale * cell_width;

    uint32_t num_rows = (uint32_t)(raw_image->height / cell_height);

    if( num_cols > raw_image->width ||
        num_rows > raw_image->height){
        cell_width = 6.0;
        cell_height = 12.0;
        num_cols = (uint32_t)(raw_image->width  / cell_width); 
        num_rows = (uint32_t)(raw_image->height / cell_height);
    }

    int x0, y0, x1, y1;
    stbtt_GetCodepointBitmapBox(&font_ptr->fontinfo, font_ptr->sample_character, font_ptr->scale, font_ptr->scale, &x0, &y0, &x1, &y1);

    int pad_x = 1;
    int pad_y = 2;

    int char_width  = x1 - x0 + pad_x;
    int char_height = y1 - y0 + pad_y;

    int out_width  = char_width * num_cols;
    int out_height = h_scale * char_height * num_rows;
    int out_channels = 3;

    uint8_t *out_image = (uint8_t *)malloc(out_width * out_height * out_channels);
    memset(out_image, args->bg_code, out_width * out_height * out_channels);

    Color bg_color = {
        .r = args->bg_code,
        .g = args->bg_code,
        .b = args->bg_code,
    };

    for(int i = 0; i < num_rows; ++i) {
        for(int j = 0; j < num_cols; ++j) {
            Color bg = {
                .r = args->bg_code,
                .g = args->bg_code,
                .b = args->bg_code
            };
            Color block_avg_color = calcBlockColor(raw_image, i, j, cell_height, cell_width, bg);
            double color_mean = (block_avg_color.r + block_avg_color.g + block_avg_color.b) / 3.0;
            uint32_t char_idx = (uint32_t)(color_mean * num_chars / 255.0);
            char ch = font_ptr->char_list[min(char_idx, num_chars - 1)];

            int x_pos = j * char_width;
            int y_pos = i * char_height;
            
            renderChar(out_image, out_channels, out_width, out_height, 
                       ch,        x_pos,        y_pos,     font_ptr, 
                       block_avg_color,  bg_color);
        }
    }

    BBox bbox = findBoundingBox(out_image, out_channels, out_width, out_height, args->bg_code);
    uint8_t *cropped_image = cropImage(out_image, out_channels, out_width, out_height, bbox, &out_width, &out_height);

    if(isPNGFile(output_image_path)) {
        stbi_write_png(output_image_path, out_width, out_height, out_channels, cropped_image, out_width * out_channels);
    }
    else if(isJPGFile(output_image_path)) {
        stbi_write_jpg(output_image_path, out_width, out_height, out_channels, cropped_image, out_width * out_channels);
    }
    else {
        CHECK_ERROR(true, "Invalid output image format (%s)", output_image_path);
    }

    free(out_image);
    free(cropped_image);
    freeFont(font_ptr);
    freeImage(raw_image);
    return;
}
