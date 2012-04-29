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
#include "posting.c"
#include <string.h>
#include <unistd.h>

#define NUM_WORDS 900000
#define MAX_DOCUMENTS 173252
#define DOCID_LENGTH 15
#define AVE_WORD_LENGTH 50

/* function declarations */
int compare_count(const void *, const void *);
char *decompress(char *,int);
int id_search(int, posting *, int, int);
int do_index(char *);
int do_search(char **, int, int);
void usage(void);


/* Will either search the index or index the file given to it */
int main(int argc, char **argv) {

    int c;
    extern char *optarg;
    int verbosity = 0;

    c = getopt(argc, argv, "i:v:s:h");
    switch (c) {
        case 'i':
            do_index(optarg);
            break;
        case 's':
            do_search(argv, argc, verbosity);
            break;
        case 'v':
            verbosity = 1;
            do_search(argv, argc, verbosity);
            break;
        case 'h':
            usage();
            return EXIT_FAILURE;
            break;
        case '?':
            usage();
            return EXIT_FAILURE;
            break;
        default:
            usage();
            return EXIT_FAILURE;
            break;
    }

    return EXIT_SUCCESS;

}

void usage(void) {

    fprintf(stderr, "\nPlease provide valid commands.\n");
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "-i\tfile_to_index\tThis will index the file you have passed to it.\n");
    fprintf(stderr, "-s\tsearch_term(s)\tThis will search for term(s) you have provided.\n");
    fprintf(stderr, "-v\tsearch_term(s)\tThis will search for term(s) you have provided but will\n");
    fprintf(stderr, "\t\t\toutput the individual results as well as combined results.\n");
    fprintf(stderr, "-h\t\tPrints this usage.\n");

}

int do_index(char *filename) {
    FILE *file = NULL;

    file = fopen(filename, "r");
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

    return EXIT_SUCCESS;
}

int do_search(char **argv, int argc, int verbosity) {
    size_t ave_word_length = AVE_WORD_LENGTH;
    char docid_buffer[DOCID_LENGTH]; 
    size_t bytes_read = 0;
    int i, j;
    unsigned int density = 0;
    int words_entered = 0;
    char temp1[AVE_WORD_LENGTH], temp2[AVE_WORD_LENGTH];
    FILE *indexFile = NULL;
    FILE *postings_file = NULL;
    FILE *word_count_file = NULL;
    char *temp_word = emalloc(AVE_WORD_LENGTH + 1);
    char **dictionary = emalloc(NUM_WORDS * sizeof(dictionary[0]));
    int *posting_pos = emalloc(NUM_WORDS * sizeof(posting_pos[0]));
    int *posting_length = emalloc(NUM_WORDS * sizeof(posting_length[0]));
    int *bad_term;
    int post_count;
    int merged_count = 0;
    int result = 0;
    int new_count = 0;
    int word_count = 0;
    posting **merged_postings = emalloc(2 * sizeof(merged_postings[0]));
    posting **postings = emalloc((argc -2) * sizeof(postings[0]));
    posting *doc_num_words = emalloc(MAX_DOCUMENTS * sizeof(postings[0]));
    int *num_post_per_term = emalloc(15 * sizeof(int));
    merged_postings[0] = NULL;
    merged_postings[1] = NULL;
    bad_term = emalloc((argc-2) * sizeof(int));

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

    /* Close index file */
    if (fclose(indexFile) < 0) {
        perror("Closing file.");
        exit(EXIT_FAILURE);
    }

    /* initial loadup - load word_count for each doc */
    word_count_file = fopen("wsj-doc_word_count", "r");
    if (word_count_file == NULL) {
        fprintf(stderr, "File failed to open or cannot find file: 'wsj-doc_word_count'\n");
        return EXIT_FAILURE;
    }

    for (i = 0; i < MAX_DOCUMENTS; i++) {
        fscanf(word_count_file, "%d %d", &(doc_num_words[i].posting_docid), &(doc_num_words[i].posting_count));
    }

    if (fclose(word_count_file) < 0) {
        perror("Closing file.");
        exit(EXIT_FAILURE);
    }

    postings_file = fopen("wsj-postings", "r");
    if (postings_file == NULL) {
        fprintf(stderr, "File failed to open or cannot find file: 'wsj-postings'\n");
        return EXIT_FAILURE;
    }

    /* Get all docids related to this term then sort by docid for further searching */
    for (j = 2; j < argc; j++) { 

        i = search(argv[j], dictionary, 0, words_entered-1);

        if (i != -1) {

            postings[j-2] = emalloc(sizeof(posting));

            fseek(postings_file, posting_pos[i], SEEK_SET);
            temp_word = emalloc(posting_length[i]);
            fgets(temp_word, posting_length[i], postings_file);

            /* load posting stuff into memory */
            post_count = 0;
            bytes_read = 0;
            while (sscanf(temp_word + bytes_read, "%s %s", temp1, temp2) == 2) {

                if (post_count != 0) { 
                    postings[j-2] = erealloc(postings[j-2], (post_count + 1) * sizeof(posting));
                }
                
                postings[j-2][post_count].posting_docid = atoi(temp2);
                word_count = id_search(postings[j-2][post_count].posting_docid, doc_num_words, 0, MAX_DOCUMENTS);
                
                if (word_count < 0) {
                    fprintf(stderr, "uh oh word count: %d\n", word_count);
                    fprintf(stderr, "Was looking for docid %d but didnt find it\n", postings[j-2][post_count].posting_docid);
                }

                /* Calculate density of term in doc */
                density = word_count / atoi(temp1);
                if (density < 0) {
                    fprintf(stderr, "uh oh density: %d\n", density);
                }

                postings[j-2][post_count].posting_count = density;

                bytes_read += (strlen(temp1) + strlen(temp2) + 2);
                post_count++;
            }
            bad_term[j-2] = -1;
            num_post_per_term[j-2] = post_count;

        } else {
            fprintf(stderr, "Sorry your term %s found no results.\n", argv[j]);
            bad_term[j-2] = 0;
            continue;
        }


        /* Merge results from previous term and this term */
        if (j >= 3) {

            merged_postings[1] = emalloc(merged_count * sizeof(posting));
            new_count = 0;
            for (i = 0; i < post_count; i++) {

                if ((result = id_search(postings[j-2][i].posting_docid, merged_postings[0], 0, merged_count)) != -1) {
                    density = result * postings[j-2][i].posting_count; 
                    if (density < 0) {
                        fprintf(stderr, "really uh oh density: %d probs blew size limit\n", density);
                    }
                    merged_postings[1][new_count].posting_docid = postings[j-2][i].posting_docid;
                    merged_postings[1][new_count].posting_count = density ;
                    new_count++;
                }
            }
            if (new_count == 0) {
                free(merged_postings[1]);
                free(merged_postings[0]);
                merged_postings[0] = NULL;
                break;
            }
            merged_count = new_count;
            merged_postings[0] = merged_postings[1];

        } else {
            merged_postings[0] = emalloc(post_count * sizeof(posting));
            for (i = 0; i < post_count; i++) {
                merged_postings[0][i] = postings[j-2][i];
            }
            merged_count = post_count;
        }

    } /* end proccessing */

    /* close postings file */
    if (fclose(postings_file) != 0) {
        fprintf(stderr, "Postings file failed to close\n");
        return EXIT_FAILURE;
    }

    /* displays the postings for each term */
    if (verbosity) {
        for (j = 2; j < argc; j++) {
            if (bad_term[j-2] == -1) {
                /* Sort on count TODO change to sort on relevance */
                qsort(postings[j-2], num_post_per_term[j-2], sizeof(posting), compare_count);
                fprintf(stdout, "Top 10 Documents containing '%s'\n", argv[j]);
                fprintf(stdout, "\nDocid\t\tRelevance Rank\n");
                for (i=0; i < ((10 < num_post_per_term[j-2]) ? 10 : num_post_per_term[j-2]); i++) {
                    fprintf(stdout, "%s\t%d\n", decompress(docid_buffer,postings[j-2][i].posting_docid), i);
                }
                fprintf(stdout, "\n");
            }
        }
    }

    /* displays the postings for the merged list */
    if (merged_postings[0] != NULL) {
        qsort(merged_postings[0], merged_count, sizeof(posting), compare_count);
        fprintf(stdout, "Top 10 Documents containing ");
        for (j=2; j < argc; j++) {
            if (bad_term[j-2] == -1) { 
                fprintf(stdout, "'%s' ", argv[j]);
            }
        }
        fprintf(stdout, "\n\nDocid\t\tRelevance Rank\n");
        for (i=0; i < ((10 < merged_count) ? 10 : merged_count); i++) {
            fprintf(stdout, "%s\t%d\n", decompress(docid_buffer,merged_postings[0][i].posting_docid), i);
        }
    } else {
        fprintf(stderr, "Sorry no documents were found containing all the terms in your query. Please search for something else.\n");
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
        return 1;
    } else if(count1 < count2) {
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

/* Creates docid to be displayed to the user */
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
