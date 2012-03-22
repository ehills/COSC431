/*
 * File: main.c
 * Author: Edward Hills
 * Date: 23/03/2012
 * Description: This program will open the files necessary from the wall
 street journal xml collection and pass it to the parser.
 */

#include "parse.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    FILE *file = NULL;

    if (argc < 2) {
        fprintf(stderr, "Please provide a filename.\n");
        return EXIT_FAILURE;
    }

    file = fopen(argv[1], "r");
    if (file == NULL) {
        fprintf(stderr, "File failed to open\n");
        return EXIT_FAILURE;
    }

    begin_indexing(); // just a stub atm
 
    parse(file); // parse.c will do the checking for eof

    end_indexing(); // just a stub atm
    if (fclose(file) != 0) {
        fprintf(stderr, "File failed to close\n");
        return EXIT_FAILURE;
    }

    fprintf(stdout, "File %s successfully parsed and indexed.\n", argv[1]);
    return EXIT_SUCCESS;
} // end main.c
