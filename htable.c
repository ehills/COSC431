/**
 * @file htable.c
 * @author Ed Hills
 * @date April 2012
 *
 * It creates a hash-table which can use linear-probing or double-hashing
 * as a collision resolution strategy. Its size is either a default value given
 * or the user can pass it and it will be made closeset to prime. It will store
 * stats about the hashtable created.
 */

#include <stdio.h>
#include <stdlib.h>
#include "htable.h"
#include "mylib.h"
#include "flexarray.h"
#include <string.h>


/**
 * Sets the attributes that the htable will have, it will store the capacity,
 * number of items entered, the values as well as how frequent they appear,
 * stats and the hashing type.
 */
struct htablerec{
    hashing_t hashing_type;
    int num_keys;
    int capacity;
    int *count;
    key_value *keys;
};

struct key_value_rec {
    char *key;
    flexarray postings;
};

/**
 * Initialises a new htable.
 *
 * @param capacity the size of the hash-table.
 * @param hashing_type the type of hashing.
 *
 * @return htable the new hash-table.
 */
htable htable_new(int capacity, hashing_t hashing_type){
    int i;
    htable result = emalloc(sizeof(*result));

    capacity = find_prime(capacity);
    result->capacity = capacity;
    result->num_keys = 0;
    result->keys = emalloc(capacity * sizeof(result->keys[0]));
    result->count = emalloc(capacity * sizeof(int));
    result->hashing_type = hashing_type;
    for (i = 0; i < capacity; i++){
        result->count[i] = 0;
    }
    return result;
}

/**
 * Finds a prime number that is >= to the size that is given.
 *
 * @param size the size of the htable to be converted to a prime.
 * @return size the new size of the htable.
 */
int find_prime(int size){
    while (!is_prime(size)){
        size++;
    }
    return size;
}

/**
 * Decides whether that value passed to it is a prime or not.
 *
 * @param candidate the possible candidate to be prime.
 * @return whether it is a prime or not.
 */
int is_prime(int candidate) {
    int i = 2;

    for (i = 2; i < candidate; i++){
        if (candidate % i == 0){
            return 0;
        }
    }
    return 1;
}

/**
 * Frees all of the memory that is allocated for the htable.
 *
 * @param h the htable to be freed.
 */
void htable_delete(htable h){
    int i;

    //for (i = 0; i < h->capacity; i++){
     //   free(h->keys[i]);
  //  }
    free(h->count);
    free(h->keys);
    free(h);
}

/**
 * Converts the word into a integer value.
 *
 * @param word the word to be converted.
 * @return result the result of the converted word.
 */
static unsigned int htable_word_to_int(const char *word) {
    unsigned int result = 0;

    while (*word != '\0') {
        result = (*word++ + 31 * result);
    }
    return result;
}

/**
 * Re-hashes the position of the value to be placed in the
 * htable if the home position is taken.
 *
 * @param h the htable to be hashed with.
 * @param i_key the value to be re hashed.
 * @return the new position.
 */
static unsigned int htable_step(htable h, unsigned int i_key) {
    if (h->hashing_type == DOUBLE_H){
        return 1 + (i_key % (h->capacity -1));
    } else {
        return 1;
    }
}

/**
 * Inserts a new item into the hash table using double
 * hashing or linear probing.
 *
 * @param h the htable to be hashed into.
 * @param s the value to be inserted into the hash table.
 * @return returns whether the value was inserted successfully or not.
 */
int htable_insert(htable h, const char *s, long docid){
    int collisions;
    int position = htable_word_to_int(s) % h-> capacity;
    int step = htable_step(h, htable_word_to_int(s));

    for (collisions = 0; collisions <= h->capacity; collisions++){
        if ((h->keys[position]).key == NULL) {
            (h->keys[position]).key = emalloc((strlen(s) + 1) * sizeof(s[0]));
            strcpy((h->keys[position]).key, s);
            (h->keys[position]).postings = flexarray_new();
            flexarray_append((h->keys[position]).postings, docid);
            h->num_keys++;
            h->count[position]++;
            return 1;
        } else if (!strcmp((h->keys[position]).key, s)){

            h->count[position]++;

            if (flexarray_get_last_id((h->keys[position]).postings) != docid) {
                flexarray_append((h->keys[position]).postings, docid);
            } else {
                flexarray_updatecount((h->keys[position]).postings);
            }
            return 1;
        } else {
            position = (position + step) % h->capacity;
        }
    }
    return 0;
}

/**
 * Searches the hash table for the given value.
 *
 * @param h the htable to be searched.
 * @param s the value to search the hash table with.
 * @return whether the value was found or not.
 */
int htable_search(htable h, const char *s){
    int i;
    int position = htable_word_to_int(s) % h-> capacity;
    int step = htable_step(h, htable_word_to_int(s));

    for (i = 0; i <= h->capacity; i++){
        if (h->keys[position].key == NULL){
            return 0;
        } else if(strcmp(h->keys[position].key, s) == 0){
            return 1;
        } else {
            position = (position + step) % h->capacity;
        }
    }
    return 0;
}

int htable_save_to_disk(htable h, FILE* fp) {
    int i;
    FILE *postings_fp = NULL;
    unsigned int pos = 0;
    unsigned int length = 0;

    qsort(h->keys, h->capacity, sizeof(key_value), compare);
 
    postings_fp = fopen("wsj-postings", "w");
    if (postings_fp == NULL) {
        fprintf(stderr, "'wsj-postings' failed to open\n");
        return EXIT_FAILURE;
    }

    for (i = 0; i < h->num_keys; i++) {
        length = flexarray_save_to_disk((h->keys[i]).postings, postings_fp);
        fprintf(fp, "%s %d %d\n", (h->keys[i]).key, pos, length);
        pos += length;
    }

    if (fclose(postings_fp) != 0) {
        fprintf(stderr, "'wsj-postings' failed to close\n"); 
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

}

/**
 * Prints out all the words stored in the hashtable and the frequency of
 * each of them.
 *
 * @param h the htable to get the words from.
 * @param stream the output stream to write to.
 */
void htable_print(htable h){
    int i;

    for (i = 0; i < h->num_keys; i++){
        if ((h->keys[i]).key != NULL) {
            printf("%s\t", (h->keys[i]).key);
            flexarray_print((h->keys[i]).postings);
            printf("\n");
        }
    }
    printf("Number of words entered: %d", h->num_keys);
}

int compare(const void *x, const void *y) {

    key_value *ix = (key_value *) x;
    key_value *iy = (key_value *) y;
    char *key1 = ix->key;
    char *key2 = iy->key;
    
    if (key1 == NULL && key2 == NULL) {
        return 0;
    }
    if (key1 == NULL) {
        return 1;
    }
    if (key2 == NULL) {
        return -1;
    }
    
    return strcmp(key1, key2);
}

