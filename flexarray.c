#include <stdio.h>
#include <stdlib.h>
#include "mylib.h"
#include "flexarray.h"

struct flexarrayrec {
   int capacity;
   int no_of_documents;
   long *docid;
};

flexarray flexarray_new(){
   flexarray result = emalloc(sizeof(result));
   result->capacity = 2;
   result->no_of_documents = 0;
   result->docid = emalloc((result->capacity) * sizeof(result->docid[0]));
   return result;
}

long flexarray_get_last_id(flexarray f) {

    return f->docid[f->no_of_documents-1];

}

void flexarray_append(flexarray f, long doc){
   if (f->no_of_documents == f-> capacity){
      f->capacity *= 2;
      f->docid = erealloc(f->docid, (f->capacity) * sizeof(f->docid[0]));
   }
   f->docid[f->no_of_documents++] = doc;
}

void flexarray_print(flexarray f) {
   int i;

   for (i = 0; i < f->no_of_documents; i++) {
      printf("%ld\t", f->docid[i]);
   }
}

void flexarray_delete(flexarray f){
   free(f->docid);
   free(f);
}
