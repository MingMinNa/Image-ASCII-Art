#include "../include/Image-ASCII-Art/utils.h"
#include "../include/Image-ASCII-Art/font.h"
#include "../include/Image-ASCII-Art/arg_parser.h"
#include "../include/Image-ASCII-Art/ascii_art.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char *argv[])
{
    arg_parser *parser = init_parser(argc, argv);
    arguments *args = parse(parser);
    
    switch (args->func_type){

        case TEXT_ASCII:
            text_ascii(args);
            break;
        case GRAY_IMG:
            gray_image_ascii(args);
            break;
        case COLOR_IMG:
            color_image_ascii(args);
            break;
        default:
            CHECK_ERROR(true, "Invalid func type");
            break;
    }
    
    free_args(args);
    free_parser(parser);
    return 0;
}