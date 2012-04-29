/*
* File: parse.c
* Author: Edward Hills
* Date: 23/03/2012
* Description: This program will parse the xml collection given to it 
*              using the scanner created by flex below, and then pass
*              the terms onto the indexer.
*/

%option noyywrap
%{
#include <stdio.h>
#include "index.h"

void toLower(char *);

void toLower(char *word) {

    int i;
    for (i =0; word[i] != '\0'; i++) {
        if (word[i] >= 'A' && word[i] <= 'Z') {
            word[i] = word[i] - ('A' -'a');
        }
    }
}
%}

%%
[a-zA-Z]+([\']?[-]?[a-zA-Z])* { 

    toLower(yytext);
    word(yytext); 
    
} /* eg fred\'s */

"<"[a-zA-Z]+">"   { start_tag(yytext); }
"</"[a-zA-Z]+">"   { end_tag(yytext); }
[a-zA-Z]*[\$]?[\.]?[-\+]?[0-9]+([/]?[\.,-][0-9]+)*[\%]?  { 
    
    toLower(yytext);
    word(yytext); 
    
} /* eg. $24.08 */
([a-zA-Z][\.])+ { 
    
    toLower(yytext);
    word(yytext); 
    
}

[\&][a-z\;]+                  /* eat it up */ 
["\n"]                        /* eat it up */ 
.                             /* eat it up */
%%

#include "parse.h"

void parse(FILE *stream) {
    
    yyin = stream;
    yylex();

}


/** DECISIONS
*   Chose not to include "quoted sentences as one word"
*   Chose not to include words thats end with a single quote
*   Chose to remove the &amp; entities and treat surrouding words separately
*   Chose to index the dates as numbers without slashes
*/
