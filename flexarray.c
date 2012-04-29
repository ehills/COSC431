#include <stdio.h>
#include <stdlib.h>
#include "mylib.h"
#include "flexarray.h"

/* flexarray struct to store my dynamically size array */
/* TODO separate out functionality of that of flexarray and postings */
struct flexarrayrec {
    int capacity;
    int no_of_documents;
    int *docid;
    int *times_found;
};

/* Initialises new flexarray */
flexarray flexarray_new(){
    flexarray result = emalloc(sizeof(result));
    result->capacity = 2;
    result->no_of_documents = 0;
    result->docid = emalloc((result->capacity) * sizeof(result->docid[0]));
    result->times_found = emalloc((result->capacity) * sizeof(result->times_found[0]));
    return result;
}

/* Writes all documents in this array to disk */
unsigned int flexarray_save_to_disk(flexarray f, FILE* fp) {
    int i;
    unsigned int length = 0;

    qsort(f->docid, f->no_of_documents, sizeof(int), flex_compare_docid);
    for (i = 0; i < f->no_of_documents; i++) {
        length += fprintf(fp, "%d %d\t", f->times_found[i], f->docid[i]);
    }
    return length;
}

/* Updates count of term in document */
void flexarray_updatecount(flexarray f) {

    f->times_found[f->no_of_documents -1]++;

}

/* Returns the last written docid for this term */
long flexarray_get_last_id(flexarray f) {

    return f->docid[f->no_of_documents - 1];

}

/* Adds a new docid for this term */
void flexarray_append(flexarray f, int doc){
    if (f->no_of_documents == f-> capacity){
        f->capacity *= 2;
        f->docid = erealloc(f->docid, (f->capacity) * sizeof(f->docid[0]));
        f->times_found = erealloc(f->times_found, (f->capacity) * sizeof(f->times_found[0]));
    }
    f->times_found[f->no_of_documents] = 1;
    f->docid[f->no_of_documents++] = doc;
}

/* Prints this flexarray */
void flexarray_print(flexarray f) {
    int i;

    for (i = 0; i < f->no_of_documents; i++) {
        printf("%d %d\t", f->times_found[i], f->docid[i]);
    }
}

/* Live free or die hard */
void flexarray_delete(flexarray f){
    free(f->times_found);
    free(f->docid);
    free(f);
}

/* Compare docids. Used in qsort */
int flex_compare_docid(const void *x, const void *y) {
    int *ix = (int *)x;
    int *iy = (int *)y;
    int docid1 = *ix;
    int docid2 = *iy;

    if (docid1 > docid2) {
        return 1;
    } else if (docid1 < docid2) {
        return -1;
    } else {
        return 0;
    }
}
