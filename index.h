/* 
* File: index.h
* Author: Edward Hills
* Date: 23/03/2012
* Description: Header file for index.c. Defines all method signatures.
*/

#ifndef INDEX_H_
#define INDEX_H_

#include <stdlib.h>

typedef struct index_posting_rec index_posting;
extern void begin_indexing(void);
extern void end_indexing(void);
extern void start_tag(char const *);
extern void end_tag(char const *);
extern void word(char const *);
extern char *get_doc_no(char const *);
extern void save_word_count(void);
int compare_docid(const void *, const void *);

#endif
