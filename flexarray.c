#include <stdio.h>
#include <stdlib.h>
#include "mylib.h"
#include "flexarray.h"

/* flexarray struct to store my dynamically size array */
/* TODO separate out functionality of that of flexarray and postings */
struct flexarrayrec {
    int capacity;
    int no_of_documents;
    flex_posting* postings;
};

struct flex_posting_rec {

    int posting_count;
    int posting_docid;

};

/* Initialises new flexarray */
flexarray flexarray_new(){
    flexarray result = emalloc(sizeof(result));
    result->capacity = 2;
    result->no_of_documents = 0;
    result->postings = emalloc((result->capacity) * sizeof(flex_posting));
    return result;
}

/* Writes all documents in this array to disk */
unsigned int flexarray_save_to_disk(flexarray f, FILE* fp) {
    int i;
    unsigned int length = 0;

    qsort(f->postings, f->no_of_documents, sizeof(flex_posting), flex_compare_docid);
    for (i = 0; i < f->no_of_documents; i++) {
        length += fprintf(fp, "%d %d\t", f->postings[i].posting_count, f->postings[i].posting_docid);
    }
    return length;
}

/* Updates count of term in document */
void flexarray_updatecount(flexarray f) {

    (f->postings[f->no_of_documents -1].posting_count)++;

}

/* Returns the last written docid for this term */
long flexarray_get_last_id(flexarray f) {

    return f->postings[f->no_of_documents - 1].posting_docid;

}

/* Adds a new docid for this term */
void flexarray_append(flexarray f, int doc){
    if (f->no_of_documents == f-> capacity){
        f->capacity *= 2;
        f->postings = erealloc(f->postings, (f->capacity) * sizeof(flex_posting));
    }
    f->postings[f->no_of_documents].posting_count = 1;
    f->postings[f->no_of_documents++].posting_docid = doc;
}

/* Prints this flexarray */
void flexarray_print(flexarray f) {
    int i;

    for (i = 0; i < f->no_of_documents; i++) {
        printf("%d %d\t", f->postings[i].posting_count, f->postings[i].posting_docid);
    }
}

/* Live free or die hard */
void flexarray_delete(flexarray f){
    free(f->postings);
    free(f);
}

/* Compare docids. Used in qsort */
int flex_compare_docid(const void *x, const void *y) {
    flex_posting *ix = (flex_posting *)x;
    flex_posting *iy = (flex_posting *)y;
    int docid1 = ix->posting_docid;
    int docid2 = iy->posting_docid;

    if (docid1 > docid2) {
        return 1;
    } else if (docid1 < docid2) {
        return -1;
    } else {
        return 0;
    }
}
