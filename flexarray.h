#ifndef FLEXARRAY_H_
#define FLEXARRAY_H_

typedef struct flexarrayrec *flexarray;

extern flexarray flexarray_new();
extern void flexarray_append(flexarray, long);
extern void flexarray_print(flexarray);
extern void flexarray_delete(flexarray);
extern long flexarray_get_last_id(flexarray);

#endif
