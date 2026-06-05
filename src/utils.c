#include "../include/Image-ASCII-Art/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool check_file_exist(const char *file_path) 
{    
    if(file_path == NULL) 
        return false;

    // attempt to open the file to check if it exists
    FILE *fp = fopen(file_path, "r");

    bool is_valid = false;
    if (fp) {
        is_valid = true;
        fclose(fp);
    }
    return is_valid;
}

bool check_extension(const char *file_name, const char *ext_name) 
{
    CHECK_ERROR(file_name == NULL, "The file name is NULL");
    CHECK_ERROR(ext_name == NULL, "The extension is NULL");
    size_t file_name_len = strlen(file_name);
    size_t ext_name_len  = strlen(ext_name);

    if(ext_name_len > file_name_len) 
        return false;

    for(size_t i = 0; i < ext_name_len; ++i) {
        size_t file_idx = file_name_len - ext_name_len + i, ext_idx = i;
        if(file_name[file_idx] != ext_name[ext_idx]) 
            return false;
    }
    return true;
}

bool is_png_file(const char *file_name) 
{
    if(file_name == NULL) 
        return false;
    
    return check_extension(file_name, ".png") || 
           check_extension(file_name, ".PNG");
}

bool is_jpg_file(const char *file_name) 
{
    if(file_name == NULL) 
        return false;

    return check_extension(file_name, ".jpg") || 
           check_extension(file_name, ".JPG") || 
           check_extension(file_name, ".jpeg") ||
           check_extension(file_name, ".JPEG");
}