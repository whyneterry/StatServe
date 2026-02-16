#include <lcthw/darray.h>
#include <lcthw/hashmap.h>
#include <lcthw/bstrlib.h>
#include <stdio.h>

DArray *DArray_create(size_t element_size, size_t initial_max) {

	size_t darrsize = sizeof(DArray);
	DArray *DArray = malloc(darrsize);
	check_mem(DArray);
	check(initial_max > 0 && initial_max < SIZE_MAX, "Invalid initial_max value.")
	DArray->contents = malloc(initial_max * sizeof(void *));
	check_mem(DArray->contents);
	DArray->max = initial_max;
	DArray->end = 0;
	DArray->element_size = element_size;
	DArray->expand_rate = DEFAULT_EXPAND_RATE;

	return DArray;

error:
	if(DArray) {
		free(DArray);
	}
	if(DArray->contents) {
		free(DArray->contents);
	}
	return NULL;
}

void DArray_destroy(DArray *array) {

	check(array != NULL, "Can't destroy nothing.")
	if(array->contents) {
		free(array->contents);
	}
	free(array);

error:
	return;
}

void DArray_print(DArray *array) {
	int i = 0;
	HashmapNode *el = NULL;
	bstring data;
	for (i = 0; i < array->end; i++) {
		el = DArray_get(array, i);
		data = el->key;
		printf("%s | ", bdata(data));
	}
	printf("\n");
}

void DArray_clear(DArray *array) {

	check(array != NULL, "Can't clear nothing.");
	check(array->contents != NULL, "Array is already clear.");
	check(array->max && array->end && array->element_size > 0, "Invalid array.");	

	int i = 0;
	for (i = 0; i < array->max; i++) {
		if ( (array->contents)[i] != NULL ) {
			printf("Before: %p\nValue: %d\n", array->contents[i], *(int *)array->contents[i]);
			free( (array->contents)[i] );
			printf("After: %p\nValue: %d\n", array->contents[i], *(int *)array->contents[i]);
		}
	}

error:
	return;
}

static inline int DArray_resize(DArray *array, size_t newsize) {

	check(array != NULL, "Can't resize nothing.")
	check(newsize > 0, "Can't resize to zero or negative.")
	check(array->contents, "There's nothing to resize.");
	array->max = newsize;
	void *temp = realloc(array->contents, array->max * sizeof(void *));
	check_mem(temp);
	array->contents = temp;

	return 0;

error:
	return -1;
}

int DArray_expand(DArray *array) {

	check(array != NULL, "Can't expand nothing.");
	check(array->max && array->expand_rate, "Invalid array.");
	size_t old_max = array->max;
	size_t newsize = array->max + array->expand_rate;
	
	int rc = DArray_resize(array, newsize);
	check(rc == 0, "Expand array failed. Size: %d.", array->max+(int)array->expand_rate);
	
	int memsetsize = array->max - (array->end + 1); 
	memset(array->contents + old_max, 0, array->expand_rate + 1);
	return 0;

error:
	return -1;
}

int DArray_contract(DArray *array) {

	check(array != NULL, "Can't contract nothing.");
	check(array->end && array->expand_rate, "Invalid array.");
	int new_size = array->end < (int)array->expand_rate ? (int)array->expand_rate : array->end;
	return DArray_resize(array, new_size + 1);

error:
	return -1;
}

void DArray_clear_destroy(DArray *array) {

	check(array != NULL, "Can't clear+destroy nothing.");
	check(array->contents, "Array is already cleared. Can't clear+destroy.");

	DArray_clear(array);
	DArray_destroy(array);

error:
	return;
}

int DArray_push(DArray *array, void *el) {

	check(array, "Can't push to nothing.");
	check(array->max && array->contents, "Invalid array.");
	check(el != NULL, "Can't push NULLPTR.")	
	
	int rc = 0;
	if (DArray_end(array) + 1 >= DArray_max(array)) {
		rc = DArray_expand(array);
	}
	
	check(rc == 0, "Expanding array failed in DArray_push.");
	array->contents[array->end] = el;
	array->end++;

	return 0;

error:
	return -1;
}

void *DArray_pop(DArray *array) {

	check(array->end - 1 >= 0, "Array is empty.");
	void *el = DArray_remove(array, array->end - 1);
	array->end--;

	if (DArray_end(array) > (int)array->expand_rate && DArray_end(array) % array->expand_rate) {
		DArray_contract(array);
	}

	return el;

error:
	return NULL;
}
