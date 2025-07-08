#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

#define min(a, b) (a < b)?(a):(b)

#define CHECK_ERROR(error_cond, fmt, ...) \
do { \
    if((error_cond)) { \
        fprintf(stderr, "[func: %s] ", __func__); \
        fprintf(stderr, fmt, ##__VA_ARGS__); \
        fprintf(stderr, " in file: %s, line: %d\n", __FILE__, __LINE__); \
        exit(EXIT_FAILURE); \
    } \
} while(0)

bool checkFileExist(const char *file_path);
bool checkExtension(const char *file_name, const char *ext_name);

#endif