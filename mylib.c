#include <stdio.h>
#include <stdlib.h>
#include "mylib.h"

void *emalloc(size_t s){
   void *result = malloc(s);
   if (NULL == result){
      fprintf(stderr, "memory allocation failed.\n");
      exit(EXIT_FAILURE);
   }
   return result;
}

void *erealloc(void *p, size_t s){
   void *result = realloc(p, s);
   if (NULL == result){
      fprintf(stderr, "memory allocation failed.\n");
      exit(EXIT_FAILURE);
   }
   return result;
}
