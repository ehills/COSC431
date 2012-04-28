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
#define DOCID_LENGTH 15

int compare_docid(const void *x, const void *y);
int compare_count(const void *x, const void *y);
char *decompress(char *,int);

typedef struct posting_rec {

    int posting_count;
    int posting_docid;

} posting;

int main(int argc, char **argv) {

    size_t ave_word_length = 60;
    char docid_buffer[DOCID_LENGTH]; 
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
    int bad_term[argc -2];
    int post_count;
    int merged_count = 0;
    int result = 0;
    int new_count = 0;
    posting **merged_postings = emalloc(2 * sizeof(merged_postings[0]));
    posting **postings = emalloc((argc -2) * sizeof(postings[0]));
    int *num_post_per_term = emalloc(15 * sizeof(int));
    merged_postings[0] = NULL;
    merged_postings[1] = NULL;

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

        // Get all docids related to this term then sort by docid for further searching
        int j;
        for (j = 2; j < argc; j++) { 

            i = search(argv[j], dictionary, 0, words_entered-1);

            if (i != -1) {

                postings[j-2] = emalloc(sizeof(posting));

                fseek(postings_file, posting_pos[i], SEEK_SET);
                temp_word = emalloc(posting_length[i]);
                fgets(temp_word, posting_length[i], postings_file);

                // load posting stuff into memory
                post_count = 0;
                bytes_read = 0;
                while (sscanf(temp_word + bytes_read, "%s %s", temp1, temp2) == 2) {

                    if (post_count != 0) { 
                        postings[j-2] = erealloc(postings[j-2], (post_count + 1) * sizeof(posting));
                    }
                    postings[j-2][post_count].posting_count = atoi(temp1);
                    postings[j-2][post_count].posting_docid = atoi(temp2);
                    bytes_read += (strlen(temp1) + strlen(temp2) + 2);
                    post_count++;
                }
                bad_term[j-2] = -1;
                num_post_per_term[j-2] = post_count;

                if (argc > 3) {
                    qsort(postings[j-2], post_count, sizeof(posting), compare_docid);
                } else {
                    break;
                }
            } else {
                bad_term[j-2] = 0;
                fprintf(stderr, "Sorry your search for '%s' did not return any results.\n", argv[j]);
                continue; // move onto next term
            }

            if (j >= 3) {

                merged_postings[1] = erealloc(merged_postings[1], post_count * sizeof(posting));
                new_count = 0;
                for (i = 0; i < post_count; i++) {

              //      if (&(postings[j-2][i]) == NULL) {
              //          continue;
              //      }
                    if ((result = id_search(postings[j-2][i].posting_docid, merged_postings[0], 0, merged_count)) != -1) {
                        merged_postings[1][new_count].posting_docid = postings[j-2][i].posting_docid;
                        merged_postings[1][new_count].posting_count = postings[j-2][i].posting_count + result;
                        new_count++;
                    }
                }
                if (new_count == 0) {
                    continue;
                }
                merged_count = new_count;
                merged_postings[0] = erealloc(merged_postings[0], post_count * sizeof(posting));
                merged_postings[0] = merged_postings[1];

            } else {
                merged_postings[0] = emalloc(post_count * sizeof(posting));
                for (i = 0; i < post_count; i++) {
                    merged_postings[0][i] = postings[j-2][i];
                }
                merged_postings[1] = emalloc(sizeof(posting));
                merged_count = post_count;
            }

        }


        /* displays the postings for each term */
        for (j = 2; j < argc; j++) {
        if (bad_term[j-2] == -1) {
            qsort(postings[j-2], num_post_per_term[j-2], sizeof(posting), compare_count);
            fprintf(stdout, "Top 10 Postings for '%s'\n", argv[j]);
            fprintf(stdout, "Docid\t\tTimes term found\n");
            for (i=0; i < 10; i++) {
                if (postings[j-2][i].posting_count) {
                    fprintf(stdout, "%s\t%d\n", decompress(docid_buffer,postings[j-2][i].posting_docid), postings[j-2][i].posting_count);
                }
            }
        }
        }
        /* displays the postings for the merged list */
        if (merged_postings[0] != NULL) {
            qsort(merged_postings[0], merged_count, sizeof(posting), compare_count);
            fprintf(stdout, "\nTop 10 Merged Postings for ");
            for (j=2; j < argc; j++) {
                if (bad_term[j-2] == -1) { 
                    fprintf(stdout, "'%s' ", argv[j]);
                }
            }
            fprintf(stdout, "\nDocid\t\tTimes terms found (word frequency combined)\n");
            for (i=0; i < 10; i++) {
                fprintf(stdout, "%s\t%d\n", decompress(docid_buffer,merged_postings[0][i].posting_docid), merged_postings[0][i].posting_count);
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

/* compares the two counts */
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

int compare_docid(const void *x, const void *y) {

    posting *ix = (posting *) x;
    posting *iy = (posting *) y;
    int docid1 = ix->posting_docid;
    int docid2 = iy->posting_docid;

    if (docid1 > docid2) {
        return 1;
    } else if(docid1 < docid2) {
        return -1;
    } else {
        return 0;
    }
}

/* binary searches the docid list and returns the posting count */
int id_search(int docid, posting *postings, int start, int finish){
    int m =(finish + start) / 2;

    if(finish < start){
        return -1; 
    }   
    else if((postings[m].posting_docid > docid)){ 
        return id_search(docid, postings, start, m - 1); 
    } else if(postings[m].posting_docid < docid){ 
        return id_search(docid, postings, m + 1, finish);
    }else {
        return postings[m].posting_count;
    }   
}

/* Creates 'string' and decompresses docid back to original form */
char *decompress(char *decompressed_docid, int docid) {
    int length = 0;

    length = sprintf(decompressed_docid, "WSJ%d", docid);

    decompressed_docid[DOCID_LENGTH-1] = '\0';
    decompressed_docid[DOCID_LENGTH-2] = decompressed_docid[length -1];
    decompressed_docid[DOCID_LENGTH-3] = decompressed_docid[length -2];
    decompressed_docid[DOCID_LENGTH-4] = decompressed_docid[length -3];
    decompressed_docid[DOCID_LENGTH-5] = '0';
    decompressed_docid[DOCID_LENGTH-6] = '-';

    return decompressed_docid;
}
