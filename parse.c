/*
* File: parse.c
* Author: Edward Hills
* Date: 23/03/2012
* Description: This program will parse the xml collection of the wall street
*              journal collection and parse it to the indexer to be indexed.*/

#include "parse.h"

char line[80];

/*
* This method will parse the input file and send its data to the indexer.
*/
void parse(FILE *stream) {
    while (fgets(line, 80, stream) != NULL) {
        printf("%s",line);
    }
}

// end parse.c
