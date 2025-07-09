#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"

#include "image.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

Image* createImage(int width, int height, int channels) {
    
    Image *image_ptr = (Image *)malloc(sizeof(Image));
    image_ptr->width    = width;
    image_ptr->height   = height;
    image_ptr->channels = channels;
    image_ptr->size     = width * height * channels;
    image_ptr->data     = (uint8_t *)malloc(sizeof(uint8_t) * image_ptr->size);

    if(channels >= 3) 
        image_ptr->is_gray = false;
    else
        image_ptr->is_gray = true;

    return image_ptr;
}

Image* loadImage(const char *image_path) {

    int width, height, channels;
    uint8_t *data = stbi_load(image_path, &width, &height, &channels, 0); 
    if(data == NULL) {
        exit(1);
    }
    
    Image *image_ptr = (Image *)malloc(sizeof(Image));
    image_ptr->width    = width;
    image_ptr->height   = height;
    image_ptr->channels = channels;
    image_ptr->data     = data;
    image_ptr->size     = width * height * channels;
    
    if(channels >= 3) 
        image_ptr->is_gray = false;
    else
        image_ptr->is_gray = true;

    return image_ptr;
}

Image* toGray(Image *image_ptr) {

    int gray_channels = 0;
    
    if(image_ptr->is_gray == false)
        gray_channels = (image_ptr->channels == 4) ? (2): (1);
    else
        gray_channels = image_ptr->channels;
    
    Image *gray_image = createImage(image_ptr->width, 
                                    image_ptr->height, 
                                    gray_channels);

    for(int p1_idx = 0, p2_idx = 0; p1_idx < image_ptr->size; p1_idx += image_ptr->channels, 
                                                              p2_idx += gray_image->channels){
        
        if(image_ptr->is_gray == false && image_ptr->channels == 4) {
            // R, G, B
            gray_image->data[p2_idx] = image_ptr->data[p1_idx] * 0.299 + \
                                       image_ptr->data[p1_idx + 1] * 0.587 + \
                                       image_ptr->data[p1_idx + 2] * 0.114;
            
            gray_image->data[p2_idx + 1] = image_ptr->data[p1_idx + 3];
        }
        else if(image_ptr->is_gray == false && image_ptr->channels == 3) {
            gray_image->data[p2_idx] = image_ptr->data[p1_idx] * 0.299 + \
                                       image_ptr->data[p1_idx + 1] * 0.587 + \
                                       image_ptr->data[p1_idx + 2] * 0.114;
        }
        else if(image_ptr->is_gray == true && image_ptr->channels == 2) {
            gray_image->data[p2_idx]     = image_ptr->data[p1_idx];
            gray_image->data[p2_idx + 1] = image_ptr->data[p1_idx + 1];
        }
        else {
            gray_image->data[p2_idx]     = image_ptr->data[p1_idx];
        }
    }

    return gray_image;
}

void saveImage(const Image *image_ptr, const char *output_image_path) {

    // Check whether the image extension is valid. (.jpg / .JPG / .jpeg / .JPEG / .png / .PNG)
    char *valid_exts[] = {
        ".jpg", ".JPG", ".jpeg", ".JPEG", ".png", ".PNG"
    };

    for(int i = 0; i < 4; ++i) {
        if(checkExtension(output_image_path, valid_exts[i])) {
            stbi_write_jpg( output_image_path,    image_ptr->width, image_ptr->height, 
                            image_ptr->channels, image_ptr->data,  100);
            return;
        }
    }

    for(int i = 4; i < 6; ++i) {
        if(checkExtension(output_image_path, valid_exts[i])) {
            stbi_write_jpg( output_image_path,    image_ptr->width, image_ptr->height, 
                            image_ptr->channels, image_ptr->data,  image_ptr->width * image_ptr->channels);
            return;
        }
    }
    CHECK_ERROR(false, "Invalid file extension");
}

void freeImage(Image *image_ptr) {
    
    if(image_ptr == NULL) return;

    if(image_ptr->data != NULL) {
        stbi_image_free(image_ptr->data);
    }

    image_ptr->width = 0;
    image_ptr->height = 0;
    image_ptr->data = NULL;
    image_ptr->channels = 0;
}