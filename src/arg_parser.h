#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include <stdint.h>

enum FuncType {
    TEXT_ASCII, 
    IMG_ASCII
};

enum ModeType {
    SIMPLE, 
    COMPLEX
};

typedef struct {
    uint32_t argc;
    uint32_t num_options;
    int *option_index;
    char **argv;
} ArgParser;

typedef struct {
    char *input_path;
    char *output_path;
    uint32_t num_cols;
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
