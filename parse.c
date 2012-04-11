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
%}

%%
[a-zA-Z]+([\']?[-]?[a-zA-Z])* { word(yytext); } /* eg fred\'s */
^"<"[a-zA-Z][a-zA-Z0-9]*">"   { start_tag(yytext); }
"</"[a-zA-Z][a-zA-Z0-9]*">"   { end_tag(yytext); }
([\$%]?[\.]?[0-9]+([\.,-][0-9]+)*[\$%]?)  { word(yytext); } /* eg. $24.08 */
([a-zA-Z][\.])+               { word(yytext); }
.                               /* eat it up */
%%

#include "parse.h"

void parse(FILE *stream) {
    
    yyin = stream;
    yylex();

}

/** DECISIONS
*   Chose not to include "quoted sentences as one word"
*   Chose not to include words thats end with a single quote
*   Chose to include words that have ampersand as one word
*   and separately
*   Chose to index the dates as numbers separetly and with slashes
*/
