#ifndef LCTHW_DARRAY_H
#define LCTHW_DARRAY_H
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <lcthw/dbg.h>

typedef struct DArray {
	int end;
	int max;
	size_t element_size;
	size_t expand_rate;
	void **contents;
} DArray;

DArray *DArray_create(size_t element_size, size_t initial_max);

void DArray_destroy(DArray *array);

void DArray_clear(DArray *array);

void DArray_print(DArray *array);

int DArray_expand(DArray *array);

int DArray_contract(DArray *array);

int DArray_push(DArray *array, void *el);

void *DArray_pop(DArray *array);

void DArray_clear_destroy(DArray *array);

#define DArray_last(A) ((A)->contents[(A)->end - 1])
#define DArray_first(A) ((A)->contents[0])
#define DArray_end(A) ((A)->end)
#define DArray_count(A) DArray_end(A)
#define DArray_max(A) ((A)->max)

#define DEFAULT_EXPAND_RATE 300

static inline void DArray_set(DArray *array, int i, void *el) {

	check(i < DArray_max(array), "DArray attempt to set past max.");
	if (i > DArray_end(array)) {
		array->end = i;
	}
	array->contents[i] = el;

error:
	return;
}

static inline void *DArray_remove(DArray *array, int i) {

	check(i <= array->end, "Index is out of DArray's range.")
	void *el = array->contents[i];
	array->contents[i] = NULL;
	return el;

error:
	return NULL;
}

static inline void *DArray_get(DArray *array, int i) {

	check(i <= array->end, "Index is out of DArray's range.")
	void *el = array->contents[i];
	return el;

error:
	return NULL;
}

static inline void *DArray_new(DArray *array) {

	check(array->element_size > 0, "Cant create zero-sized DArrays");
	return calloc(1, sizeof(array->element_size));

error:
	return NULL;
}

#define DArray_free(E) free((E))
#endif
