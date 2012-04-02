/*
* File: parse.c
* Author: Edward Hills
* Date: 23/03/2012
* Description: This program will parse the xml collection of the wall street
*              journal collection and parse it to the indexer to be indexed.*/

#include "parse.h"

char line[2];

/*
* This method will parse the input file and send its data to the indexer.
*/
void parse(FILE *stream) {
    while (fgets(line, 2, stream) != NULL) {

        // case 1:
        // if its a <l its opening tag
        
        // case 2: 
        // if its a ll its a word unless after opening/closing tag
        // in which case its the name of the tag

        // case 3:
        // if its a </ its a closing tag

        // case 4:
        // if its a > its end of either opening or closing tag

        printf("%s\n",line);
    }
}

// end parse.c
