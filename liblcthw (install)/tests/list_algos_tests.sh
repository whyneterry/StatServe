#include "minunit.h"
#include <lcthw/list_algos.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define COUNT 6

bool isInArray(int arr[], int size, int value) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == value) return true;
    }
    return false;
}

void createRandomArray(int arr[], int size) {
    // Initialize array with -1 to indicate empty slots
    for (int i = 0; i < size; i++) {
        arr[i] = -1;
    }
    
    srand(time(NULL));
    int count = 0;
    
    while (count < size) {
        int num = rand() % 1000;  // Random number 0-999
        
        // Ensure the number is unique
        if (!isInArray(arr, count, num)) {
            arr[count] = num;
            count++;
        }
    }
}

int numbers[1000];

char *strings[] = { "dayn", "down", "up", "1234", "vasiliy", "krista" };

List *create_nums() {
	
	createRandomArray(numbers, 1000);
	printf("First 20 random numbers:\n");
	for (int i = 0; i < 20; i++) {
		printf("%d ", numbers[i]);
	}
	printf("\n\n");

	List *nums = List_create();
	int i = 0;
	for(i = 0; i < 1000; i++) {
		List_push(nums, &numbers[i]);
	}

	return nums;
}

List *create_strs() {

	List *strs = List_create();
	int i = 0;
	for(i = 0; i < COUNT; i++) {
		List_push(strs, strings[i]);
	}

	return strs;
}

char *test_bubble_sort() {
	
	List *nums = create_nums();
	//List_print(nums, "int");
	
	clock_t t;
	t = clock(); 
	bubbleSort(nums, "int");
	t = clock() - t;
	double time = ((double)t)/CLOCKS_PER_SEC;
	printf("bubbleSort with 1000 ints took %f secs\n\n", time);

	//List_print(nums, "int");

	List *strs = create_strs();
	//List_print(strs, "str");

	t = clock();
	bubbleSort(nums, "str");
	t = clock() - t;
	time = ((double)t)/CLOCKS_PER_SEC;
	printf("bubbleSort with 6 strings took %f secs\n\n", time);

	//List_print(strs, "str");

	return NULL;
}

char *test_merge_sort() {

	List *nums = create_nums();
	//List_print(nums, "int");

	clock_t t;
	t = clock(); 
	ListNode *sorted = mergeSortStart(nums->first, "int");
	t = clock() - t;
	double time = ((double)t)/CLOCKS_PER_SEC;
	printf("mergeSort with 1000 ints took %f secs\n\n", time);

	nums->first = sorted;
	//List_print(nums, "int");
	while(sorted) {
		if(sorted->next == NULL) {
			nums->last = sorted;
		}
		sorted = sorted->next;
	}
	//List_print(nums, "int");
	
	List *strs = create_strs();
	//List_print(strs, "str");

	t = clock(); 
	ListNode *sortedstr = mergeSortStart(strs->first, "str");
	t = clock() - t;
	time = ((double)t)/CLOCKS_PER_SEC;
	printf("mergeSort with 6 strings took %f secs\n\n", time);

	strs->first = sortedstr;
	//List_print(nums, "int");
	while(sortedstr) {
		if(sortedstr->next == NULL) {
			strs->last = sortedstr;
		}
		sortedstr = sortedstr->next;
	}
	List_print(strs, "str");
	
	return NULL;
}

char *all_tests() {

	mu_suite_start();
	mu_run_test(test_merge_sort);
	mu_run_test(test_bubble_sort);

	return NULL;
}

RUN_TESTS(all_tests);
