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
#include "index.h"
#include "flexarray.h"
#include <string.h>

#define NUM_WORDS 900000
#define MAX_DOCUMENTS 200000

int compare_count(const void *x, const void *y);
typedef struct posting_rec {

    int posting_count;
    long posting_docid;

} posting;

int main(int argc, char **argv) {

    size_t ave_word_length = 80;
    size_t bytes_read = 0;
    int i;
    int words_entered = 0;
    char temp1[ave_word_length];
    char temp2[ave_word_length];
    FILE *file = NULL;
    FILE *indexFile = NULL;
    FILE *postings_file = NULL;
    char *temp_word = emalloc(ave_word_length + 1);
    char **dictionary = emalloc(NUM_WORDS * sizeof(dictionary[0]));
    int *posting_pos = emalloc(NUM_WORDS * sizeof(posting_pos[0]));
    int *posting_length = emalloc(NUM_WORDS * sizeof(posting_length[0]));

    posting **postings = emalloc((argc -2) * sizeof(postings[0]));

//    int **posting_count = emalloc((argc -2) * sizeof(posting_count[0]));
//    long **posting_docid = emalloc((argc -2) * sizeof(posting_docid[0]));

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

            /* need to decide to store them now or later or what */

        }

        /* open file */
        indexFile = fopen("wsj-index", "r");
        if (indexFile == NULL) {
            fprintf(stderr, "File failed to open or cannot find file: 'wsj-index'\n");
            return EXIT_FAILURE;
        }

        /* initial loadup */
        while (getline(&temp_word, &ave_word_length, indexFile) != EOF) {
            dictionary[words_entered] = emalloc((strlen(temp_word) + 1) * sizeof(char));
            sscanf(temp_word, "%s %d %d", dictionary[words_entered], &(posting_pos[words_entered]), &(posting_length[words_entered]));
            words_entered++;

        }

        /* now dict is in memory search for postings */

        postings_file = fopen("wsj-postings", "r");
        if (postings_file == NULL) {
            fprintf(stderr, "File failed to open or cannot find file: 'wsj-postings'\n");
            return EXIT_FAILURE;
        }

        // find term from soon to be sorted array then look it up with (see below)

        int j;
        for (j = 2; j < argc; j++) { 

            i = search(argv[j], dictionary, 0, words_entered-1);

            if (i != -1) {

                postings[j-2] = emalloc(MAX_DOCUMENTS * sizeof(posting));

                fseek(postings_file, posting_pos[i], SEEK_SET);
                temp_word = emalloc(posting_length[i]);
                fgets(temp_word, posting_length[i], postings_file);

                // load posting stuff into memory
                i = 0;
                bytes_read = 0;
                while (sscanf(temp_word + bytes_read, "%s %s", temp1, temp2) == 2) {

                    postings[j-2][i].posting_count = atoi(temp1);
                    postings[j-2][i].posting_docid = atol(temp2);
                    bytes_read += (strlen(temp1) + strlen(temp2) + 2);
                    i++;
                }
            } else {
                fprintf(stderr, "Sorry your search term did not return any results\n");
            }

            // merge here
            //quick sort to get top hits
            qsort(postings[j-2], i, sizeof(posting), compare_count);
            // return hits

        }

        for (j =2; j < argc; j++) {
            fprintf(stdout, "\nTop 10 Postings for %s\n", argv[j]);
            for (i=0; i < 10; i++) {
                if (postings[j-2][i].posting_count) {
                fprintf(stdout, "Docid: %ld count: %d\n", postings[j-2][i].posting_docid, postings[j-2][i].posting_count);
                }
            }
        }

        /* close postings file */
        if (fclose(postings_file) != 0) {
            fprintf(stderr, "Postings file failed to close\n");
            return EXIT_FAILURE;
        }

        /* close index file */
        if (fclose(indexFile) != 0) {
            fprintf(stderr, "Index file failed to close\n");
            return EXIT_FAILURE;
        }

    }

    return EXIT_SUCCESS;

}

int compare_count(const void *x, const void *y) {
    
    posting *ix = (posting *) x;
    posting *iy = (posting *) y;
    int count1 = ix->posting_count;
    int count2 = iy->posting_count;

    if (count1 > count2) {
        return -1;
    } else if(count1 < count2) {
        return 1;
    } else {
        return 0;
    }
}

