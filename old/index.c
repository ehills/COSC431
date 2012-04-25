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
#define NUM_WORDS 900000 // ~750,000 unique words

FILE *fp;
char const *index_file_name = "wsj-index";
int docno_incoming = 0;
htable dict;
unsigned int count = 0;
long curr_docno;

/* 
* This method will begin indexing and set up all things that it needs.
*/
void begin_indexing(void) {
    fp = fopen(index_file_name, "w");
    if (fp == 0) {
        perror(index_file_name);
        exit(EXIT_FAILURE);
    }

    dict = htable_new(NUM_WORDS, DOUBLE_H);

}

/* 
* This method will end indexing and free all things that it used.
*/
void end_indexing(void) {

    htable_save_to_disk(dict, fp);
    htable_delete(dict);

    if (fclose(fp) < 0) {
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

    if (docno_incoming) {
        curr_docno = atol(get_doc_no(spelling));
    }

    htable_insert(dict, spelling, curr_docno);
}

char *get_doc_no(const char *docid) {
    
    char* doc = emalloc(sizeof(docid));
    int i;
    int place = 0;
    
    for (i = 3; docid[i] != '\0'; i++) {
        if (docid[i] != '-') {
            doc[place++] = docid[i];
        }
    }

    return doc;
}

// end index.c
