/*
* File: index.c
* Author: Edward Hills
* Date: 23/03/2012
* Description: This program will index the words that are passed to it
*              from parse.c. 
*/

#include <stdio.h>
#include "parse.h"
#include "index.h"
#include "mylib.h"
#include "htable.h"
#include <string.h>
#include "flexarray.h"
#define NUM_WORDS 900000 /* ~750,000 unique words */
#define MAX_DOCS 173252 /* ~750,000 unique words */

FILE *fp;
FILE *word_count_fp;
char const *index_file_name = "wsj-index";
char const *word_count_filename = "wsj-doc_word_count";
int docno_incoming = 0;
htable dict;
unsigned int count = 0;
int docs_entered = 0;
int curr_docno =0;
int word_count = 0;
index_posting *postings;

struct index_posting_rec {

    int posting_count;
    int posting_docid;

};

/* 
* This method will begin indexing and set up all things that it needs.
*/
void begin_indexing(void) {
    fp = fopen(index_file_name, "w");
    if (fp == 0) {
        perror(index_file_name);
        exit(EXIT_FAILURE);
    }

    word_count_fp = fopen(word_count_filename, "w");
    if (word_count_fp == 0) {
        perror(word_count_filename);
        exit(EXIT_FAILURE);
    }

    postings = emalloc(MAX_DOCS * sizeof(postings[0]));
    dict = htable_new(NUM_WORDS, DOUBLE_H);

}

/* 
* This method will end indexing and free all things that it used.
*/
void end_indexing(void) {

    htable_save_to_disk(dict, fp);

    postings[docs_entered].posting_docid = curr_docno;
    postings[docs_entered].posting_count = word_count;
    docs_entered++;

    qsort(postings, docs_entered, sizeof(index_posting), compare_docid);
    save_word_count();

    free(dict);

    if (fclose(fp) < 0) {
        perror("Closing file.");
        exit(EXIT_FAILURE);
    }
    
    if (fclose(word_count_fp) < 0) {
        perror("Closing file.");
        exit(EXIT_FAILURE);
    }
}

/* 
* This method will take in the start_tag and index it.
*/
void start_tag(char const *name) {

    if (!strcmp(name, "<DOCNO>")) {
        docno_incoming = 1;
    }
}

/* 
* This method will take in the end_tag and index it.
*/
void end_tag(char const *name) {
    if (!strcmp(name, "</DOCNO>")) {
        docno_incoming = 0;
    }
}

/* 
* This method will take in the word and index it.
*/
void word(char const *spelling) {

    if (!docno_incoming) {
        word_count++;
        htable_insert(dict, spelling, curr_docno);
    } else {
        if (curr_docno != 0) { 
            postings[docs_entered].posting_docid = curr_docno;
            postings[docs_entered].posting_count = word_count;
            docs_entered++;
        }
        curr_docno = atoi(get_doc_no(spelling));
        word_count =1;
        htable_insert(dict, spelling, curr_docno);
    }
    
}

/* Turns the docno into a integer */
char *get_doc_no(const char *docid) {
    
    char *doc = emalloc((strlen(docid) -3) * sizeof(docid[0]));
    int i;
    int place = 0;
    
    for (i = 3; i < strlen(docid); i++) {
        
        if (docid[i] != '-') {
            doc[place++] = docid[i];
        } else {
            i++;
        }
    }
    doc[place] = '\0';

    return doc;
}

/* Saves the word count to disk */
void save_word_count(void) {
    int i;
    for (i =0; i < MAX_DOCS; i++) {
        fprintf(word_count_fp, "%d %d\n", postings[i].posting_docid, 
                                            postings[i].posting_count);
    }
}

/* Compare docids. Used in qsort */
int compare_docid(const void *x, const void *y) {
    index_posting *ix = (index_posting *)x;
    index_posting *iy = (index_posting *)y;
    int docid1 = ix->posting_docid;
    int docid2 = iy->posting_docid;

    if (docid1 > docid2) {
        return 1;
    } else if (docid1 < docid2) {
        return -1; 
    } else {
        return 0;
    }   
}


/* end index.c */
