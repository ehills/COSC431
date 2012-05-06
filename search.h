#ifndef SEARCH_H_
#define SEARCH_H_

typedef struct posting_rec posting;
int compare_rank(const void *, const void *); 
char *decompress(char *,int);
double get_rank(int, posting *, int, int);
int get_word_count(int, posting *, int, int);
int do_search(char **,int,int);

#endif
