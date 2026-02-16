#include <lcthw/list.h>
#include <lcthw/list_algos.h>

#include <string.h>
#include <stdio.h>

void bubbleSort(List *List, char *datatype) {
	
	int swapped = 1;
	ListNode *node = NULL;
	
	while(swapped) {
		swapped = 0;
		for(node = List->first; node != List->last; node = node->next) {
			if( strcmp(datatype, "int") == 0 ) {		
				
				if( *(int *)(node->value) > *(int *)(node->next->value) ) {
				
					void *temp_value = node->value;
					node->value = node->next->value;
					node->next->value = temp_value;
					swapped = 1;
				}
			}

			if( strcmp(datatype, "str") == 0 ) {
				
				if( strcmp( (char *)(node->value), (char *)(node->next->value) ) > 0 ) {
			
					void *temp_value = node->value;
					node->value = node->next->value;
					node->next->value = temp_value;
					swapped = 1;
				}
			}
		}
	}
}

ListNode *mergeSortSplit(ListNode *head) {

	ListNode *slow = head;
	ListNode *fast = head;

	while(fast->next && fast->next->next) {
		
		slow = slow->next;
		fast = fast->next->next;
	}

	ListNode *temp = slow->next;
	temp->prev->next = NULL;
	if(temp != NULL) {
		temp->prev = NULL;
	}
	/*
	printf("1st Half: ");
	ListNode *temphead = head;
	while(temphead) {
		printf("%d ", *((int *)temphead->value));
		temphead = temphead->next;
	}
	printf("\n");

	printf("2d Half: ");
	ListNode *temptemp = temp;
	while(temptemp) {
		printf("%d ", *((int *)temptemp->value));
		temptemp = temptemp->next;
	}
	printf("\n");
	*/
	return temp;
}

ListNode *mergeSortMerge(ListNode *first, ListNode *second, char *datatype) {

	if(first == NULL) {
		//printf("First is NULL so we return Second\n");
		return second;
	}
	if(second == NULL) {
		//printf("Second is NULL so we return First\n");
		return first;
	}

	if( strcmp(datatype, "int") == 0 ) {		
		int firstValue = *((int *)first->value);	
		int secondValue = *((int *)second->value);

		//printf("We compare %d ? %d\n", firstValue, secondValue);

		if(firstValue < secondValue) {
			//printf("Case FV < SV\n");
			first->next = mergeSortMerge(first->next, second, datatype);
			if(first->next != NULL) {
				//printf("Recursive first->next: %d\n", *((int *)first->next->value));
				first->next->prev = first;
			}
			first->prev = NULL;
			//printf("Returning first: %d\n", *((int *)first->value));
			return first;
		}
		else {
			//printf("Case FV > SV\n");
			second->next = mergeSortMerge(first, second->next, datatype);
			if(second->next != NULL) {
				//printf("Recursive second->next: %d\n", *((int *)second->next->value));
				second->next->prev = second;
			}
			second->prev = NULL;
			//printf("Returning second: %d\n", *((int *)second->value));
			return second;
		}
	}
	else if( strcmp(datatype, "str") == 0 ) {
		char *firstValue = ((char *)first->value);	
		char *secondValue = ((char *)second->value);

		if( strcmp(firstValue, secondValue) < 0 ) {
			first->next = mergeSortMerge(first->next, second, datatype);
			if(first->next != NULL) {
				first->next->prev = first;
			}
			first->prev = NULL;
			return first;
		}
		else {
			second->next = mergeSortMerge(first, second->next, datatype);
			if(second->next != NULL) {
				second->next->prev = second;
			}
			second->prev = NULL;
			return second;
		}	
	}
	return NULL;
}

ListNode *mergeSortStart(ListNode *head, char *datatype) {

	if(head == NULL || head->next == NULL) {
		return head;
	}

	ListNode *second = mergeSortSplit(head);
	
	head = mergeSortStart(head, datatype);
	second = mergeSortStart(second, datatype);

	return mergeSortMerge(head, second, datatype);
}
