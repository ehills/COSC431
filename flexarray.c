#include <stdio.h>
#include <stdlib.h>
#include "mylib.h"
#include "flexarray.h"

struct flexarrayrec {
    int capacity;
    int no_of_documents;
    long *docid;
    int *times_found;
};

flexarray flexarray_new(){
    flexarray result = emalloc(sizeof(result));
    result->capacity = 2;
    result->no_of_documents = 0;
    result->docid = emalloc((result->capacity) * sizeof(result->docid[0]));
    result->times_found = emalloc((result->capacity) * sizeof(result->times_found[0]));
    return result;
}

unsigned int flexarray_get_posting_length(flexarray f, FILE* fp) {
    int i;
    unsigned int length = 0;
    int chars= 0;
    char line[20];
    for (i = 0; i < f->no_of_documents; i++) {
        chars += sprintf(line, "%d %ld\t", f->times_found[i], f->docid[i]);
    }
    length = chars * sizeof(char);
    return length;
}

void flexarray_updatecount(flexarray f) {

    f->times_found[f->no_of_documents -1]++;

}

long flexarray_get_last_id(flexarray f) {

    return f->docid[f->no_of_documents - 1];

}

void flexarray_append(flexarray f, long doc){
    if (f->no_of_documents == f-> capacity){
        f->capacity *= 2;
        f->docid = erealloc(f->docid, (f->capacity) * sizeof(f->docid[0]));
        f->times_found = erealloc(f->times_found, (f->capacity) * sizeof(f->times_found[0]));
    }
    f->times_found[f->no_of_documents] = 1;
    f->docid[f->no_of_documents++] = doc;
}

void flexarray_print(flexarray f) {
    int i;

    for (i = 0; i < f->no_of_documents; i++) {
        printf("%d %ld\t", f->times_found[i], f->docid[i]);
    }
}

void flexarray_delete(flexarray f){
    free(f->times_found);
    free(f->docid);
    free(f);
}
