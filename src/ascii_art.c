#include "../include/stb_image_write.h"
#include "utils.h"
#include "font.h"
#include "image.h"
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

    const int32_t channels = image_ptr->channels;
    const int32_t height   = image_ptr->height;
    const int32_t width    = image_ptr->width;

    int32_t outer_loop_bound[] = {
        (int32_t)(i * cell_height), 
        min((int32_t)((i + 1) * cell_height), height)
    };

    int32_t inner_loop_bound[] = {
        (int32_t)(j * cell_width),
        min((int32_t)((j + 1) * cell_width), width)
    };

    if(outer_loop_bound[1] <= outer_loop_bound[0] || 
       inner_loop_bound[1] <= inner_loop_bound[0])
        return 0.0;

    int32_t block_h = outer_loop_bound[1] - outer_loop_bound[0];
    int32_t block_w = inner_loop_bound[1] - inner_loop_bound[0];
    
    uint64_t sum = 0;
    int32_t count = block_w * block_h;

    for(uint32_t k = outer_loop_bound[0]; k < outer_loop_bound[1]; ++k) {
        for(uint32_t l = inner_loop_bound[0]; l < inner_loop_bound[1]; ++l) {
            sum += image_ptr->data[width * channels * k + channels * l];
        }
    }

    return sum * 1.0 / count;
}

Color calcBlockColor(
    Image *image_ptr, 
    int i, 
    int j, 
    double cell_height, 
    double cell_width, 
    Color bg) {

    const int32_t channels = image_ptr->channels;
    const int32_t height   = image_ptr->height;
    const int32_t width    = image_ptr->width;

    int32_t outer_loop_bound[] = {
        (int32_t)(i * cell_height), 
        min((int32_t)((i + 1) * cell_height), height)
    };

    int32_t inner_loop_bound[] = {
        (int32_t)(j * cell_width),
        min((int32_t)((j + 1) * cell_width), width)
    };

    if(outer_loop_bound[1] <= outer_loop_bound[0] || 
       inner_loop_bound[1] <= inner_loop_bound[0])
        return bg;
    
    int32_t block_h = outer_loop_bound[1] - outer_loop_bound[0];
    int32_t block_w = inner_loop_bound[1] - inner_loop_bound[0];

    uint32_t r_sum = 0;
    uint32_t g_sum = 0;
    uint32_t b_sum = 0;
    int32_t count = block_w * block_h;

    for(int32_t i = outer_loop_bound[0]; i < outer_loop_bound[1]; ++i) {
        for(int32_t j = inner_loop_bound[0]; j < inner_loop_bound[1]; ++j) {
            r_sum += image_ptr->data[(i * width + j) * channels + 0];
            g_sum += image_ptr->data[(i * width + j) * channels + 1];
            b_sum += image_ptr->data[(i * width + j) * channels + 2];
        }
    }

    Color ret_color = { 
        .r = (uint8_t)(r_sum * 1.0 / count),
        .g = (uint8_t)(g_sum * 1.0 / count),
        .b = (uint8_t)(b_sum * 1.0 / count),
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
        cell_width  = 6.0;
        cell_height = 12.0;
        num_cols    = (uint32_t)(gray_image->width  / cell_width); 
        num_rows    = (uint32_t)(gray_image->height / cell_height);
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

void GrayImageAscii(Arguments *args) {

    Font  *font_ptr  = getFont(args->mode);
    size_t num_chars = strlen(font_ptr->char_list);

    const char *input_image_path  = args->input_path;
    const char *output_image_path = args->output_path;
    uint32_t num_cols = args->num_cols;

    Image *raw_image = loadImage(input_image_path);
    Image *gray_image = toGray(raw_image);

    int32_t h_scale = font_ptr->h_scale;
    double cell_width  = gray_image->width * 1.0 / num_cols;
    double cell_height = h_scale * cell_width;

    uint32_t num_rows = (uint32_t)(gray_image->height / cell_height);

    if( num_cols > gray_image->width ||
        num_rows > gray_image->height){
        cell_width  = 6.0;
        cell_height = 12.0;
        num_cols    = (uint32_t)(gray_image->width  / cell_width); 
        num_rows    = (uint32_t)(gray_image->height / cell_height);
    }

    int32_t x0, y0, x1, y1;
    stbtt_GetCodepointBitmapBox(
        &font_ptr->fontinfo, 
        font_ptr->sample_character, 
        font_ptr->scale, 
        font_ptr->scale, 
        &x0, &y0, &x1, &y1
    );

    int32_t char_width  = x1 - x0 + font_ptr->pad_x;
    int32_t char_height = y1 - y0 + font_ptr->pad_y;

    int32_t out_width    = char_width * num_cols;
    int32_t out_height   = h_scale * char_height * num_rows;
    int32_t out_channels = 1;

    Image *out_image = createImage(
        out_width, 
        out_height, 
        out_channels, 
        NULL
    );
    memset(out_image->data, args->bg_code, out_width * out_height * out_channels);

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

    for(uint32_t i = 0; i < num_rows; ++i) {
        for(uint32_t j = 0; j < num_cols; ++j) {

            double mean = calcBlockMean(gray_image, i, j, cell_height, cell_width);
            uint32_t char_idx = (uint32_t)(mean * num_chars / 255.0);
            char ch = font_ptr->char_list[min(char_idx, num_chars - 1)];

            int32_t x_pos = j * char_width;
            int32_t y_pos = i * char_height;

            renderChar( 
                out_image, x_pos   , y_pos   , ch,  
                font_ptr , fg_color, bg_color
            );
        }
    }

    BBox bbox = findBoundingBox(out_image, args->bg_code);
    Image *cropped_image = cropImage(out_image, bbox);

    saveImage(cropped_image, output_image_path);

    freeFont(font_ptr);
    freeImage(raw_image);
    freeImage(gray_image);
    freeImage(out_image);
    freeImage(cropped_image);
    return;
}

void ColorImageAscii(Arguments *args) {

    Font  *font_ptr  = getFont(args->mode);
    size_t num_chars = strlen(font_ptr->char_list);

    const char *input_image_path  = args->input_path;
    const char *output_image_path = args->output_path;
    uint32_t num_cols = args->num_cols;

    Image *raw_image = loadImage(input_image_path);
    
    int32_t h_scale = font_ptr->h_scale;
    double cell_width  = raw_image->width * 1.0 / num_cols;
    double cell_height = h_scale * cell_width;

    uint32_t num_rows = (uint32_t)(raw_image->height / cell_height);

    if( num_cols > raw_image->width ||
        num_rows > raw_image->height){
        cell_width  = 6.0;
        cell_height = 12.0;
        num_cols    = (uint32_t)(raw_image->width  / cell_width); 
        num_rows    = (uint32_t)(raw_image->height / cell_height);
    }

    int32_t x0, y0, x1, y1;
    stbtt_GetCodepointBitmapBox(
        &font_ptr->fontinfo, 
        font_ptr->sample_character, 
        font_ptr->scale, 
        font_ptr->scale, 
        &x0, &y0, &x1, &y1
    );

    int32_t char_width  = x1 - x0 + font_ptr->pad_x;
    int32_t char_height = y1 - y0 + font_ptr->pad_y;

    int32_t out_width  = char_width * num_cols;
    int32_t out_height = h_scale * char_height * num_rows;
    int32_t out_channels = 3;

    Image *out_image = createImage(
        out_width, 
        out_height, 
        out_channels, 
        NULL
    );
    memset(out_image->data, args->bg_code, out_width * out_height * out_channels);

    Color bg_color = {
        .r = args->bg_code,
        .g = args->bg_code,
        .b = args->bg_code,
    };

    for(uint32_t i = 0; i < num_rows; ++i) {
        for(uint32_t j = 0; j < num_cols; ++j) {

            Color block_avg_color = calcBlockColor(raw_image, i, j, cell_height, cell_width, bg_color);
            double color_mean = (block_avg_color.r + block_avg_color.g + block_avg_color.b) / 3.0;
            uint32_t char_idx = (uint32_t)(color_mean * num_chars / 255.0);
            char ch = font_ptr->char_list[min(char_idx, num_chars - 1)];

            int32_t x_pos = j * char_width;
            int32_t y_pos = i * char_height;
            
            renderChar( 
                out_image, x_pos          , y_pos   , ch,  
                font_ptr , block_avg_color, bg_color
            );
        }
    }

    BBox bbox = findBoundingBox(out_image, args->bg_code);
    Image *cropped_image = cropImage(out_image, bbox);

    saveImage(cropped_image, output_image_path);

    freeFont(font_ptr);
    freeImage(raw_image);
    freeImage(out_image);
    freeImage(cropped_image);
    return;
}
