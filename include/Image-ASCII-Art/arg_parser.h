#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include <stdint.h>

enum mode_type;
typedef struct image image;

enum func_type {
    TEXT_ASCII, 
    GRAY_IMG, 
    COLOR_IMG,
};

typedef struct arg_parser{
    int32_t *option_index;
    uint32_t num_options;
    uint32_t argc;
    char **argv;
} arg_parser;

typedef struct arguments{
    uint32_t num_cols;
    uint8_t bg_code;
    char *input_path;
    char *output_path;
    enum func_type func_type;
    enum mode_type mode;
} arguments;

arg_parser* init_parser(int argc, char *argv[]);
arguments* parse(arg_parser *parser);
void show_parser(arg_parser *parser);
void free_parser(arg_parser *parser);
void show_args(arguments *args);
void free_args(arguments *args);

extern char *options[][2];

#endif
