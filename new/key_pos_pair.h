#ifndef KEY_POS_PAIR_H_
#define KEY_POS_PAIR_H_

typedef struct key_pos_pair_rec *key_pos_pair;
extern int kpp_insert(key_pos_pair, const char *, long);
extern void kpp_print(key_pos_pair);
extern int kpp_save_to_disk(key_pos_pair, FILE*, FILE*);

#endif
