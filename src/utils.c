#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


bool checkFileExist(const char *file_path) {
    
    if(file_path == NULL) return false;

    FILE *fp = fopen(file_path, "r");

    bool is_valid = false;
    if (fp) {
        is_valid = true;
        fclose(fp);
    }
    return is_valid;
}

bool checkExtension(const char *file_name, const char *ext_name) {
    
    CHECK_ERROR(file_name == NULL, "The file name is NULL");
    CHECK_ERROR(ext_name == NULL, "The extension is NULL");
    size_t file_name_len = strlen(file_name);
    size_t ext_name_len  = strlen(ext_name);

    if(ext_name_len > file_name_len) return false;

    for(int i = 0; i < ext_name_len; ++i) {
        int file_idx = file_name_len - ext_name_len + i, ext_idx = i;
        if(file_name[file_idx] != ext_name[ext_idx]) 
            return false;
    }
    return true;
}


bool isPNGFile(const char *file_name) {

    if(file_name == NULL) return false;
    
    return checkExtension(file_name, ".png") || checkExtension(file_name, ".PNG");
}

bool isJPGFile(const char *file_name) {

    if(file_name == NULL) return false;

    return checkExtension(file_name, ".jpg") || checkExtension(file_name, ".jpeg") ||
           checkExtension(file_name, ".JPG") || checkExtension(file_name, ".JPEG");
}