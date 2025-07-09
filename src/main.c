#include "utils.h"
#include "arg_parser.h"
#include "ascii_art.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char *argv[]){

    ArgParser *parser = initParser(argc, argv);
    Arguments *args = parse(parser);
    
    switch (args->func_type){

        case TEXT_ASCII:
            TextAscii(args);
            break;
        case IMG_ASCII:
            ImageAscii(args);
            break;
        default:
            CHECK_ERROR(true, "Invalid func type");
            break;
    }
    freeArgs(args);
    freeParser(parser);
    return 0;
}