/*
 * File: main.c
 * Author: Edward Hills
 * Date: 23/03/2012
 * Description: This program will open the files necessary from the wall
 *              street journal xml collection and pass it to the parser.
 */

#include "parse.h"
#include <stdio.h>
#include <stdlib.h>
#include "mylib.h"
#include "flexarray.h"
#include <string.h>

#define NUM_WORDS 1000000

int main(int argc, char **argv) {

    int i;
    FILE *file = NULL;
    FILE *indexFile = NULL;
    FILE *postings_file = NULL;
    char *word = NULL;
    size_t ave_word_length = 100;
    int words_entered = 0;

    char *temp_word;
    temp_word = (char *)emalloc(ave_word_length + 1);
    char **dictionary = emalloc(NUM_WORDS * sizeof(dictionary[0]));
    int *posting_pos = emalloc(NUM_WORDS * sizeof(posting_pos[0]));
    int *posting_length = emalloc(NUM_WORDS * sizeof(posting_pos[0]));

    if (argc < 2) {
        fprintf(stderr, "Please provide a command\n");
        return EXIT_FAILURE;
    }
    
    if (*argv[1] == 'i') {

        if (argc < 3) {
            fprintf(stderr, "Please provide a filename to index.\n");
            return EXIT_FAILURE;
        }

        file = fopen(argv[2], "r");
        if (file == NULL) {
            fprintf(stderr, "File failed to open\n");
            return EXIT_FAILURE;
        }

        begin_indexing();  
        parse(file);
        end_indexing(); 

        if (fclose(file) != 0) {
            fprintf(stderr, "File failed to close\n");
            return EXIT_FAILURE;
        }


    } else if (*argv[1] == 's') {
        
        if (argc < 3) {
            fprintf(stderr, "Please enter at least one search query\n");
            return EXIT_FAILURE;
        }

        for (i = 2; i < argc; i++) {

            // need to decide to store them now or later or what

        }

        // open file
        indexFile = fopen("wsj-index", "r");
        if (indexFile == NULL) {
            fprintf(stderr, "File failed to open or cannot find file: 'wsj-index'\n");
            return EXIT_FAILURE;
        }

        // load word list into memory
        while (getline(&temp_word, &ave_word_length, indexFile) != EOF) {
            dictionary[words_entered] = emalloc((strlen(temp_word) + 1) * sizeof(char));
            sscanf(temp_word, "%s %d %d", dictionary[words_entered], &(posting_pos[words_entered]), &(posting_length[words_entered]));
            words_entered++;

        }

        postings_file = fopen("wsj-postings", "r");
        if (postings_file == NULL) {
            fprintf(stderr, "File failed to open or cannot find file: 'wsj-postings'\n");
            return EXIT_FAILURE;
        }

        // close postings file
        if (fclose(postings_file) != 0) {
            fprintf(stderr, "Postings file failed to close\n");
            return EXIT_FAILURE;
        }

        // close index file
        if (fclose(indexFile) != 0) {
            fprintf(stderr, "Index file failed to close\n");
            return EXIT_FAILURE;
        }


    }

    return EXIT_SUCCESS;




}

