#include "image.h"
#include "utils.h"
#include "ascii_art.h"
#include "arg_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *simple_alphabet  = "@%#*+=-:. ";
const char *complex_alphabet = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";

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
    uint32_t txt_num_cols = args->num_cols;
    
    CHECK_ERROR(!checkFileExist(input_image_path), "\"%s\" doesn't exist", input_image_path);

    const char *alphabet;
    if(args->mode == SIMPLE) alphabet = simple_alphabet;
    else                     alphabet = complex_alphabet;

    Image *raw_image = loadImage(input_image_path);
    Image *gray_image = toGray(raw_image);

    double cell_width  = gray_image->width * 1.0 / txt_num_cols;
    double cell_height = 2.0 * cell_width;

    uint32_t txt_num_rows = (uint32_t)(gray_image->height / cell_height);

    if( txt_num_cols > gray_image->width ||
        txt_num_rows > gray_image->height){
        cell_width = 6.0;
        cell_height = 12.0;
        txt_num_cols = (uint32_t)(gray_image->width  / cell_width); 
        txt_num_rows = (uint32_t)(gray_image->height / cell_height);
    }

    size_t num_chars = strlen(alphabet);
    FILE *output_fp = fopen(output_text_path, "w");
    CHECK_ERROR(output_fp == NULL, "Fail to open the file \"%s\"", output_text_path);

    for(uint32_t i = 0; i < txt_num_rows; ++i) {
        for(uint32_t j = 0; j < txt_num_cols; ++j) {
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
    // TODO
}