#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include <stdint.h>

enum ModeType;
typedef struct Image Image;

enum FuncType {
    TEXT_ASCII, 
    GRAY_IMG, 
    COLOR_IMG,
};

typedef struct ArgParser{
    int32_t *option_index;
    uint32_t num_options;
    uint32_t argc;
    char **argv;
} ArgParser;

typedef struct Arguments{
    uint32_t num_cols;
    uint8_t bg_code;
    char *input_path;
    char *output_path;
    enum FuncType func_type;
    enum ModeType mode;
} Arguments;

ArgParser* initParser(int argc, char *argv[]);
Arguments* parse(ArgParser *parser);
void showParser(ArgParser *parser);
void freeParser(ArgParser *parser);
void showArgs(Arguments *args);
void freeArgs(Arguments *args);

extern char *options[][2];

#endif
