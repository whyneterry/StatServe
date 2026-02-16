#ifndef LCTHW_DARRAY_ALGOS_H
#define LCTHW_DARRAY_ALGOS_H

#include <lcthw/darray.h>

typedef int (*DArray_compare) (const void *a, const void *b);

int DArray_qsort(DArray *array, DArray_compare cmp);

int DArray_sort_add(DArray *array, void *el, DArray_compare cmp);

int DArray_find(DArray *array, void *el);

int my_bsort(DArray *array);

#endif
