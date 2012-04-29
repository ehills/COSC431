/*
* File: parse.c
* Author: Edward Hills
* Date: 23/03/2012
* Description: This program will parse the xml collection given to it 
*              using the scanner created by flex below, and then pass
*              the terms onto the indexer.
*/

%option noyywrap
%option nounput
%option noinput
%{
#include <stdio.h>
#include "mylib.h"
#include "index.h"

%}

%%
[a-zA-Z]+ { 

    toLower(yytext);
    word(yytext); 
    
} /* eg fred */

[a-zA-Z]+[\'][a-zA-Z]{2,} {

    toLower(yytext);
    word(yytext);

}

[\'][a-z] { /* eat it up */ }

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

