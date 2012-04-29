/**
 * @file htable.h
 * @author Ed Hills
 * @date April 2012
 *
 * This header file defines the methods used in the htable.c file.
 */

#ifndef HTABLE_H_
#define HTABLE_H_

typedef struct htablerec *htable;
typedef struct key_value_rec key_value;
typedef enum hashing_e {LINEAR_P, DOUBLE_H} hashing_t;

extern htable htable_new(int capacity, hashing_t hash_type);
extern int find_prime(int size);
extern int is_prime(int candidate);
extern void htable_delete(htable h);
extern int htable_insert(htable h, const char *s, int docid);
extern int htable_search(htable h, const char *s);
extern void htable_print(htable h);
extern int htable_save_to_disk(htable h, FILE*);
extern int compare(const void *, const void *);

#endif
