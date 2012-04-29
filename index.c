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
#define NUM_WORDS 900000 /* ~750,000 unique words */

FILE *fp;
FILE *word_count_fp;
char const *index_file_name = "wsj-index";
char const *word_count_filename = "wsj-doc_word_count";
int docno_incoming = 0;
htable dict;
unsigned int count = 0;
int curr_docno;
int word_count = 0;

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

    dict = htable_new(NUM_WORDS, DOUBLE_H);

}

/* 
* This method will end indexing and free all things that it used.
*/
void end_indexing(void) {

    htable_save_to_disk(dict, fp);
/*    htable_delete(dict); */

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

    word_count++;
    if (!docno_incoming) {
        htable_insert(dict, spelling, curr_docno);
    } else {
        save_word_count(curr_docno, word_count);
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
void save_word_count(int docid, int count) {
    fprintf(word_count_fp, "%d %d\n", docid, count);
}

/* end index.c */
