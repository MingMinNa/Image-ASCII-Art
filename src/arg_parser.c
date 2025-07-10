#include "utils.h"
#include "font.h"
#include "image.h"
#include "arg_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *options[][2] = {
    {"--type",       "str"},    // required
    {"--input",      "str"},    // required
    {"--output",     "str"},    // required
    {"--mode",       "str"},    // default: COMPLEX
    {"--num_cols",   "int"},    // default: 300
    {"--background", "str"},    // default: black(0)
};

ArgParser* initParser(int argc, char *argv[]) {
    
    ArgParser *parser = (ArgParser *)malloc(sizeof(ArgParser));

    // copy argc and argv
    parser->argc = argc;
    parser->argv = (char **)malloc(sizeof(char *) * argc);

    for(int i = 0; i < argc; ++i) {
        parser->argv[i] = (char *)calloc(strlen(argv[i]) + 1, sizeof(char));
        snprintf(parser->argv[i], strlen(argv[i]) + 1, "%s", argv[i]);
    }
    
    uint32_t num_options = sizeof(options) / sizeof(options[0]);
    parser->num_options = num_options;
    parser->option_index = (int32_t *)calloc(num_options, sizeof(int));
    
    for(uint32_t i = 0; i < num_options; ++i) {
        parser->option_index[i] = -1;   
        for(int j = 0; j < argc; ++j) {
            if(!strcmp(argv[j], options[i][0])) {
                parser->option_index[i] = j;
                break;
            }
        }
    }

    return parser;
}

Arguments* parse(ArgParser *parser) {
    
    Arguments *args = (Arguments *)malloc(sizeof(Arguments));

    /* default values */
    args->func_type = TEXT_ASCII;
    args->input_path = NULL;
    args->output_path = NULL;
    args->mode = COMPLEX;
    args->num_cols = 300;
    args->bg_code = 0;      // black(0) / white(255)

    int32_t index = -1;

    /* --type [0] */
    index = parser->option_index[0];
    CHECK_ERROR(index == -1, "Option \"--type\" is required");
    CHECK_ERROR(index + 1 >= parser->argc, "Option \"--type\" requires a value, but none was provided");

    const char *func_type_strs[]         = { "text"    , "image"  , "image_color"};
    const enum FuncType func_type_vals[] = { TEXT_ASCII, GRAY_IMG , COLOR_IMG    };
    uint16_t num_types = sizeof(func_type_strs) / sizeof(func_type_strs[0]);
    
    bool is_valid_val = false;

    for(uint16_t i = 0; i < num_types; ++i) {
        if(!strcmp(parser->argv[index + 1], func_type_strs[i])) {
            args->func_type = func_type_vals[i];
            is_valid_val = true;
            break;
        }
    }
    CHECK_ERROR(!is_valid_val, "Option \"--type\" has invalid value (%s)", parser->argv[index + 1]);

    /* --input [1] */
    index = parser->option_index[1]; 
    CHECK_ERROR(index == -1, "Option \"--input\" is required");
    CHECK_ERROR(index + 1 >= parser->argc, "Option \"--input\" requires a value, but none was provided");
    CHECK_ERROR(!checkFileExist(parser->argv[index + 1]), "The input file doesn't exist");
    args->input_path = (char *)calloc(strlen(parser->argv[index + 1]) + 1, sizeof(char));
    snprintf(args->input_path, strlen(parser->argv[index + 1]) + 1, "%s", parser->argv[index + 1]);

    /* --output [2] */
    index = parser->option_index[2]; 
    CHECK_ERROR(index == -1, "Option \"--output\" is required");
    CHECK_ERROR(index + 1 >= parser->argc, "Option \"--output\" requires a value, but none was provided");
    args->output_path = (char *)calloc(strlen(parser->argv[index + 1]) + 1, sizeof(char));
    snprintf(args->output_path, strlen(parser->argv[index + 1]) + 1, "%s", parser->argv[index + 1]);

    /* --mode [3] */
    index = parser->option_index[3];
    if(index != -1) {
        CHECK_ERROR(index + 1 >= parser->argc, "Option \"--mode\" requires a value, but none was provided.");

        const char *model_type_strs[]         = { "simple", "complex"};
        const enum ModeType model_type_vals[] = { SIMPLE  , COMPLEX  };
        uint16_t num_types = sizeof(model_type_strs) / sizeof(model_type_strs[0]);
        
        bool is_valid_val = false;

        for(uint16_t i = 0; i < num_types; ++i) {
            if(!strcmp(parser->argv[index + 1], model_type_strs[i])) {
                args->mode = model_type_vals[i];
                is_valid_val = true;
                break;
            }
        }
        CHECK_ERROR(!is_valid_val, "Option \"--mode\" has invalid value (%s).", parser->argv[index + 1]);
    }

    /* --num_cols [4] */
    index = parser->option_index[4];
    if(index != -1) {
        CHECK_ERROR(index + 1 >= parser->argc, "Option \"--num_cols\" requires a value, but none was provided.");

        int num_cols = 0;
        bool is_valid_val = true;

        for(uint32_t i = 0, len = strlen(parser->argv[index + 1]); i < len; ++i) {
            char ch = parser->argv[index + 1][i];
            if(isdigit(ch))
                num_cols = num_cols * 10 + (ch - '0');
            else {
                is_valid_val = false;
                break;
            }
        }
        if(is_valid_val) args->num_cols = num_cols;
        CHECK_ERROR(!is_valid_val, "Option \"--num_cols\" has invalid value (%s).", parser->argv[index + 1]);
    }

    /* --background [5] */
    index = parser->option_index[5];
    if(index != -1) {
        CHECK_ERROR(index + 1 >= parser->argc, "Option \"--background\" requires a value, but none was provided.");

        const char *background_colors[]  = { "white", "black"};
        const uint8_t background_codes[] = {     255,       0};
        uint16_t num_types = sizeof(background_colors) / sizeof(background_colors[0]);
        
        bool is_valid_val = false;

        for(uint16_t i = 0; i < num_types; ++i) {
            if(!strcmp(parser->argv[index + 1], background_colors[i])) {
                args->bg_code = background_codes[i];
                is_valid_val = true;
                break;
            }
        }
        CHECK_ERROR(!is_valid_val, "Option \"--background\" has invalid value (%s).", parser->argv[index + 1]);
    }

    return args;
}

// For debug
void showParser(ArgParser *parser) {

    printf("argc: %u\n", parser->argc);
    for(uint32_t i = 0; i < parser->argc; ++i) {
        printf("argv[%u]: %s\n", i, parser->argv[i]);
    }

    printf("num_options: %u\n", parser->num_options);
    for(uint32_t i = 0; i < parser->num_options; ++i) {
        printf("option_index[%u]: %d\n", i, parser->option_index[i]);
    }
    return;
}

// For debug
void showArgs(Arguments *args) {
    
    if(args->input_path != NULL)  printf("input_path: %s\n", args->input_path);
    else                          printf("input_path: none\n");

    if(args->output_path != NULL) printf("output_path: %s\n", args->output_path);
    else                          printf("output_path: none\n");

    printf("num_cols: %u\n", args->num_cols);
    
    if     (args->func_type == TEXT_ASCII) printf("type: TEXT_ASCII\n");
    else if(args->func_type == GRAY_IMG)   printf("type: GRAY_IMG\n");
    else                                   printf("type: COLOR_IMG\n");

    if  (args->mode == SIMPLE) printf("mode: SIMPLE\n");
    else                       printf("mode: COMPLEX\n");

    if  (args->bg_code == 255) printf("background: white\n");
    else                       printf("background: black\n");

    return;
}

void freeParser(ArgParser *parser) {

    if(parser == NULL) 
        return;
    
    if(parser->option_index != NULL) 
        free(parser->option_index);

    if(parser->argv != NULL) {
        for(uint32_t i = 0; i < parser->argc; ++i) {
            if(parser->argv[i] == NULL) continue;
            free(parser->argv[i]);
        }
        free(parser->argv);
    } 
    free(parser);
}

void freeArgs(Arguments *args) {

    if(args == NULL) 
        return;

    if(args->input_path != NULL)
        free(args->input_path);
    if(args->output_path != NULL)
        free(args->output_path);
    free(args);
}