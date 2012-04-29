#ifndef SEARCH_H_
#define SEARCH_H_

typedef struct posting_rec posting;
int compare_count(const void *, const void *); 
char *decompress(char *,int);
double id_search(int, posting *, int, int);
int get_word_count_id_search(int, posting *, int, int);
int do_search(char **,int,int);

#endif
