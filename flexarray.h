#ifndef FLEXARRAY_H_
#define FLEXARRAY_H_

typedef struct flexarrayrec *flexarray;
typedef struct flex_posting_rec flex_posting;

extern flexarray flexarray_new();
extern void flexarray_append(flexarray, int);
extern void flexarray_print(flexarray);
extern void flexarray_delete(flexarray);
extern long flexarray_get_last_id(flexarray);
extern unsigned int flexarray_save_to_disk(flexarray, FILE*);
extern void flexarray_updatecount(flexarray);
extern int flex_compare_docid(const void *, const void *);

#endif
