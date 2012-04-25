
#include <stdio.h>
#include <stdlib.h>
#include "mylib.h"
#include "key_pos_pair.h"
#include "flexarray.h"
#include <string.h>

struct key_pos_pair_rec {
    flexarray postings;
    char *key;
};

key_pos_pair kpp_new() {

    key_pos_pair result = emalloc(sizeof(*result));
    
    result->postings = flexarray_new();

    return result;
    
}

void kpp_add(key_pos_pair kpp, const char *s, long docid) {
    if (kpp->key == NULL) {
        kpp->key = emalloc((strlen(s) + 1) * sizeof(char));
        flexarray_append(kpp->posting, docid);
    } else if (flexarray_get_last_id(kpp->postings) != docid) {
        flexarray_append(kpp->posting, docid);
    } else {
        flexarray_updatecount(kpp->postings);
    }
}

char *kpp_get_key(key_pos_pair kpp) {
    
    return kpp->key;

}

unsigned int kpp_save_to_disk(key_pos_pair kpp, unsigned int pos, FILE *postings_fp, FILE *fp) {
    
    int length = flexarray_save_to_disk(kpp->postings, postings_fp);
    fprintf(fp, "%s %d %d\n", kpp->key, pos, length);

    return length;

}

void kpp_print_postings(key_pos_pair kpp) {
    flexarray_print(kpp->postings);
}

