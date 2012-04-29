/*
 * File: main.c
 * Author: Edward Hills
 * Date: 30/04/2012
 * Description: This program will parse the terms sent in by the user
 *              in the same way that the indexer did.
 */

/* parse input same as parse did to index */
%option noyywrap
%option nounput
%option noinput
%{
#include <stdio.h>
#include "mylib.h"
#include "search.h"

void add_term(const char *);

%}

%%
[a-zA-Z]+ { 

        toLower(yytext);
        add_term(yytext); 
                
}

[a-zA-Z]+[\'][a-zA-Z]{2,} {

        toLower(yytext);
        add_term(yytext);

}

[\'][a-z]                       /* eat it up */ 

[a-zA-Z]*[\$]?[\.]?[-\+]?[0-9]+([/]?[\.,-][0-9]+)*[\%]?  {   
            
                toLower(yytext);
                add_term(yytext); 
                            
}
([a-zA-Z][\.])+ { 
            
                toLower(yytext);
                add_term(yytext); 
                            
}

[\&][a-z\;]+                  /* eat it up */ 
["\n"]                        /* eat it up */ 
.                             /* eat it up */
%%
/* end parse */

#include <stdio.h>
#include <stdlib.h>
#include "index.h"
#include <string.h>
#include <unistd.h>
#include "search.h"
#include "mylib.h"
#include "parse.h"

/* function declarations */
int do_index(char *);
void usage(void);
void get_input();

int term_count = 0;
char **term_input;

void get_input() {

    yylex();

}
/* Will either search the index or index the file given to it */
int main(int argc, char **argv) {

    int c;
    extern char *optarg;
    int verbosity = 0;

    term_input = emalloc(sizeof(char *) * 15);

    c = getopt(argc, argv, "i:vsh");
    switch (c) {
        case 'i':
            do_index(optarg);
            break;
        case 's':
            get_input();
            if (!term_count) {
                usage();
                return EXIT_FAILURE;
            }
            do_search(term_input, term_count, verbosity);
            break;
        case 'v':
            verbosity = 1;
            get_input();
            if (!term_count) {
                usage();
                return EXIT_FAILURE;
            }
            do_search(term_input, term_count, verbosity);
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

/* displays usage to the user */
void usage(void) {

    fprintf(stderr, "\nPlease provide valid commands.\n");
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "-i\twsj-file\tThis will index the file you have passed to it.\n");
    fprintf(stderr, "-s\tsearch_term(s)\tThis will search for term(s) you have provided.\n");
    fprintf(stderr, "-v\tsearch_term(s)\tThis will search for term(s) you have provided but will\n");
    fprintf(stderr, "\t\t\toutput the individual results as well as combined results.\n");
    fprintf(stderr, "-h\t\t\tPrints this usage.\n");

}

/* indexes the file given to it */
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

/* adds the term to a list and will be passed to search later on */
void add_term(const char *term) {

    term_input[term_count] = emalloc(sizeof(char*) * (strlen(term) + 1));
    strcpy(term_input[term_count], term); 
    term_count++;

}
