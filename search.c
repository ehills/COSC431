/*Author: Edward Hills
 * Date: 30/04/2012
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
#include <unistd.h>
#include "search.h"

#define NUM_WORDS 900000
#define MAX_DOCUMENTS 173252
#define DOCID_LENGTH 15
#define AVE_WORD_LENGTH 50

/* stores postings */
struct posting_rec {

    int posting_count;
    int posting_docid;
    double rank;

};

/* is the grunt of the search engine. Takes in a list of terms and searches
 * the index for the terms provided 
 */
int do_search(char** terms, int term_length, int verbosity) {
    size_t ave_word_length = AVE_WORD_LENGTH;
    char docid_buffer[DOCID_LENGTH]; 
    int i, j;
    double density = 0.0;
    int words_entered = 0;
    char *temp1;
    
    /* Set up file pointers */
    FILE *indexFile = NULL;
    FILE *postings_file = NULL;
    FILE *word_count_file = NULL;
    char *temp_word = emalloc(AVE_WORD_LENGTH + 1);
    
    /* dictionary to store all terms */
    char **dictionary = emalloc(NUM_WORDS * sizeof(dictionary[0]));
    
    /* store location and length of postings on disk */
    int *posting_pos = emalloc(NUM_WORDS * sizeof(posting_pos[0]));
    int *posting_length = emalloc(NUM_WORDS * sizeof(posting_length[0]));
    int *docs_containing_term = emalloc(NUM_WORDS * sizeof(int));
    int *bad_term;
    int post_count;
    int merged_count, new_count, word_pos, word_count = 0;
    double result;

    /* store postings read in */
    posting **postings;

    /* store the postings once they have been merged */
    posting **merged_postings = emalloc(2 * sizeof(merged_postings[0]));
    
    /* store the number of words per document */
    posting *doc_num_words = emalloc(MAX_DOCUMENTS * sizeof(postings[0]));
    int *num_post_per_term = emalloc(15 * sizeof(int));
    
    merged_postings[0] = NULL;
    merged_postings[1] = NULL;

    bad_term = emalloc(term_length * sizeof(int));
    postings = emalloc(term_length * sizeof(postings[0]));

    /* open file */
    indexFile = fopen("wsj-index", "r");
    if (indexFile == NULL) {
        fprintf(stderr, "File failed to open or cannot find file: 'wsj-index'\n");
        return EXIT_FAILURE;
    }

    /* initial loadup */
    while (getline(&temp_word, &ave_word_length, indexFile) != EOF) {
        dictionary[words_entered] = emalloc((strlen(temp_word) + 1) * sizeof(char));
        sscanf(temp_word, "%s %d %d %d", dictionary[words_entered], &(posting_pos[words_entered]), &(posting_length[words_entered]), &(docs_containing_term[words_entered]));
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
    for (j = 0; j < term_length; j++) { 

        word_pos = search(terms[j], dictionary, 0, words_entered-1);

        post_count = 0;
        if (word_pos != -1) {

            postings[j] = emalloc(sizeof(posting));
    
            /* read from disk */
            fseek(postings_file, posting_pos[word_pos], SEEK_SET);
            temp_word = emalloc(posting_length[word_pos] + 1);
            fread(temp_word, posting_length[word_pos], sizeof(char), postings_file);
            temp_word[posting_length[word_pos]] = '\0';

            /* load posting stuff into memory */
            
            /* need to do this just the once */
            temp1 = strtok(temp_word, " ");
            postings[j][post_count].posting_docid = atoi(strtok(NULL, "\t"));
            word_count = get_word_count_id_search(postings[j][post_count].posting_docid, doc_num_words, 0, MAX_DOCUMENTS);
            /* Calculate density of term in doc */

            density = atof(temp1) / word_count;
            density *= (docs_containing_term[word_pos] / (MAX_DOCUMENTS / 10000)); 

            postings[j][post_count].posting_count = atoi(temp1);
            postings[j][post_count].rank = density;
            post_count++;

            while((temp1 = strtok(NULL, " ")) != NULL) {
                postings[j] = erealloc(postings[j], (post_count + 1) * sizeof(posting));

                postings[j][post_count].posting_docid = atoi(strtok(NULL, "\t"));
                word_count = get_word_count_id_search(postings[j][post_count].posting_docid, doc_num_words, 0, MAX_DOCUMENTS);
                /* Calculate density of term in doc */
                
                density = atof(temp1) / word_count;
                density *= (docs_containing_term[word_pos] / (MAX_DOCUMENTS / 10000)); 

                postings[j][post_count].posting_count = atoi(temp1);
                postings[j][post_count].rank = density;
                post_count++;
            }

            bad_term[j] = -1;
            num_post_per_term[j] = post_count;

        } else {
            fprintf(stderr, "Sorry your term '%s' found no results.\n", terms[j]);
            bad_term[j] = 0;
            continue;
        }

        /* Merge results from previous term and this term */
          if (merged_postings[0] != NULL) {

            merged_postings[1] = emalloc(merged_count * sizeof(posting));
            new_count = 0;
            for (i = 0; i < post_count; i++) {

                if ((result = id_search(postings[j][i].posting_docid, merged_postings[0], 0, merged_count)) != -1) {
                    merged_postings[1][new_count].posting_docid = postings[j][i].posting_docid;
                    merged_postings[1][new_count].posting_count = postings[j][i].posting_count;
                    merged_postings[1][new_count].rank = (postings[j][i].rank * result);
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
                merged_postings[0][i] = postings[j][i];
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
        for (j = 0; j < term_length; j++) {
            if (bad_term[j] == -1) {
                qsort(postings[j], num_post_per_term[j], sizeof(posting), compare_count);
                fprintf(stderr, "Top 10 Documents containing '%s'\n", terms[j]);
                fprintf(stderr, "\nDocid\t\tRelevance Rank\n");
                for (i=0; i < ((10 < num_post_per_term[j]) ? 10 : num_post_per_term[j]); i++) {
                    fprintf(stdout, "%s\t%f\n", decompress(docid_buffer,postings[j][i].posting_docid), postings[j][i].rank);
                }
                fprintf(stdout, "\n");
            }
        }
    }

    /* displays the postings for the merged list */
    if (merged_postings[0] != NULL) {
        qsort(merged_postings[0], merged_count, sizeof(posting), compare_count);
        fprintf(stderr, "Top 10 Documents containing ");
        for (j=0; j < term_length; j++) {
            if (bad_term[j] == -1) { 
                fprintf(stderr, "'%s' ", terms[j]);
            }
        }
        fprintf(stderr, "\n\nDocid\t\tRelevance Rank\n");
        for (i=0; i < ((10 < merged_count) ? 10 : merged_count); i++) {
            fprintf(stdout, "%s\t%f\n", decompress(docid_buffer,merged_postings[0][i].posting_docid),
                                            merged_postings[0][i].rank);
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
    double count1 = ix->rank;
    double count2 = iy->rank;


    if (count1 < count2) {
        return 1;
    } else if(count1 > count2) {
        return -1;
    } else {
        return 0;
    }
}

/* binary searches the docid list and returns the posting count */
int get_word_count_id_search(int docid, posting *postings, int start, int finish){
    int m = (finish + start) / 2;

    if(finish < start){
        return -1; 
    }   
    else if((postings[m].posting_docid > docid)){ 
        return get_word_count_id_search(docid, postings, start, m - 1); 
    } else if(postings[m].posting_docid < docid){ 
        return get_word_count_id_search(docid, postings, m + 1, finish);
    }else {
        return postings[m].posting_count;
    }   
}

/* binary searches the docid list and returns the posting count */
double id_search(int docid, posting *postings, int start, int finish){
    int m =(finish + start) / 2;

    if(finish < start){
        return -1; 
    }   
    else if((postings[m].posting_docid > docid)){ 
        return id_search(docid, postings, start, m - 1); 
    } else if(postings[m].posting_docid < docid){ 
        return id_search(docid, postings, m + 1, finish);
    }else {
        return postings[m].rank;
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

/* end search.c */
