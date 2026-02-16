#include <lcthw/darray_algos.h>
#include <lcthw/hashmap.h>
#include <lcthw/bstrlib.h>
#include <lcthw/dbg.h>
#include <stdlib.h>

int my_bsort(DArray *array) {
    int n = array->end;  // array->end is the count, not index
    int i, j;
    int swapped = 1;
    HashmapNode *el1 = NULL;
    HashmapNode *el2 = NULL;
    
    for (i = 0; i < n - 1 && swapped; ++i) {
        swapped = 0;
        for (j = 0; j < n - i - 1; ++j) {
            el1 = DArray_get(array, j);
            el2 = DArray_get(array, j + 1);
            
            if (!el1 || !el2) {
                return -1;
            }
            
            // Compare keys, not data
            int res = bstrcmp(el1->data, el2->data);
            if (res > 0) {
                // Swap the entire nodes, not just data
                DArray_set(array, j, el2);
                DArray_set(array, j + 1, el1);
                swapped = 1;
            }
        }
    }
    
    return 0;
}

int qsort_part(DArray *array, int low, int high) {
	HashmapNode *p = DArray_get(array, low);
	HashmapNode *l = NULL;
	HashmapNode *h = NULL;

	int i = low - 1;
	int j = high + 1;

	while(1) {
		do {
			++i;
			l = DArray_get(array, i);
		} while ( bstrcmp(l->key, p->key) < 0 );

		do {
			--j;
			h = DArray_get(array, j);
		} while ( bstrcmp(h->key, p->key) > 0 );

		if (i >= j) {
			return j;
		}

		DArray_set(array, i, h);
		DArray_set(array, j, l);
	}
}

int my_qsort(DArray *array, int low, int high) {
	check(low >= 0 && high <= array->end && low < high, "Invalid setting for qsort");

	printf("Low: %d | High: %d\n", low, high);
	int pivot = qsort_part(array, low, high);
	int rc = my_qsort(array, low, pivot);
	check(rc == 0, "Error with my_qsort");
	rc = my_qsort(array, pivot + 1, high);
	check(rc == 0, "Error with my_qsort");
	
	return 0;	

error:
	return -1;
}

int DArray_qsort(DArray *array, DArray_compare cmp) {

	int rc = my_qsort(array, 0, array->end - 1);
	if (rc == -1) {
		return -1;
	}
	//qsort(array->contents, DArray_count(array), sizeof(void *), cmp);
	return 0;
}

int DArray_sort_add(DArray *array, void *el, DArray_compare cmp)
{
	check(array, "Can't push to nothing.");
	check(array->max && array->contents, "Invalid array.");
	check(el != NULL, "Can't push NULLPTR.")	
	
	int rc = 0;
	if (DArray_end(array) + 1 >= DArray_max(array)) {
		rc = DArray_expand(array);
	}
	
	check(rc == 0, "Expanding array failed in DArray_push.");
	printf("array->end: %d\n", array->end);
	array->contents[array->end] = el;
	array->end++;

	if (array->end > 1) {
		rc = DArray_qsort(array, cmp);
		check(rc == 0, "Error with sorting");
	}
	return 0;

error:
	return -1;
}

int DArray_find(DArray *array, void *el)
{
	check(array, "Can't find something in nothing");
	check(array->contents, "Can't find something in empty array");
	
	int low = 0;
	int high = array->end;
	int mid, res;
	char *temp;
	while(low <= high) {
		mid = low + (high - low) / 2;
		temp = array->contents[mid];
		res = strcmp(temp, el);
		if(res < 0) {
			low = mid + 1;
		}
		else if(res > 0) {
			high = mid - 1;
		}
		else {
			return mid;
		}
	}
	return -1;

error:
	return -1;
}
