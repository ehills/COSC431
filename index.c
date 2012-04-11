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

FILE *fp;
char const *index_file_name = "wsj-index";

/* 
* This method will begin indexing and set up all things that it needs.
*/
void begin_indexing(void) {
    fp = fopen(index_file_name, "w");
    if (fp == 0) {
        perror(index_file_name);
        exit(EXIT_FAILURE);
    }
}

/* 
* This method will end indexing and free all things that it used.
*/
void end_indexing(void) {
    if (fclose(fp) < 0) {
        perror("Closing file.");
        exit(EXIT_FAILURE);
    }
}

/* 
* This method will take in the start_tag and index it.
*/
void start_tag(char const *name) {
    printf("(%s\n", name);
}

/* 
* This method will take in the end_tag and index it.
*/
void end_tag(char const *name) {
    printf(")%s\n", name);
}

/* 
* This method will take in the word and index it.
*/
void word(char const *spelling) {
    printf("- %s\n", spelling);
}

// end index.c
