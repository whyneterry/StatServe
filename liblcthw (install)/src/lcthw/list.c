#include <lcthw/list.h>
#include <lcthw/dbg.h>

List *List_create() {

	return calloc(1, sizeof(List));
}

List *List_copy(List *List_from) {
	
	printf("List_copy\n");
	List *List_to = calloc(1, sizeof(List));
	//List_to->first = List_from->first;
	//List_to->last = List_from->last;
	if(List_to != NULL) {
		printf("Memory allocated\n");
	}

	LIST_FOREACH(List_from, first, next, cur, _node) {
		
		printf("Copying value: %d\n", *((int *)cur->value));		
		List_push(List_to, cur->value);
	}
	
	return List_to;
}

List *List_join(List *List1, List *List2) {
	
	printf("List_join\n");
	List *List_joined = calloc(1, sizeof(List));
	if(List_joined != NULL) {
		printf("Memory allocated\n");
	}	
	LIST_FOREACH(List1, first, next, cur, _node) {
		printf("Joining value: %s\n", (char *)cur->value);
		List_push(List_joined, cur->value);
	}
	
	LIST_FOREACH(List2, first, next, cur2, _node2) {
		printf("Joining valueAAA: %s\n", (char *)cur2->value);
		List_push(List_joined, cur2->value);
	}	
	
	LIST_FOREACH(List_joined, first, next, cur3, _node3) {
		printf("Value in joined List: %s\n", (char *)cur3->value);
	}

	return List_joined;
}

void List_print(List *List, char *datatype) {

	printf("Values: ");

	if( strcmp(datatype, "int") == 0 ) {
		LIST_FOREACH(List, first, next, cur, _node) {
			
			printf("%d ", *((int *)cur->value));
		}
	}
	else if ( strcmp(datatype, "str") == 0 ) {
		LIST_FOREACH(List, first, next, cur, _node) {
		
			printf("%s ", (char *)cur->value);
		}
	}

	printf("\n");
}

void List_destroy(List *List) {

	LIST_FOREACH(List, first, next, cur, _node) {
		if(cur->prev) {
			free(cur->prev);
		}
	}

	free(List->last);
	free(List);
}

void List_clear(List *List) {

	LIST_FOREACH(List, first, next, cur, _node) {
		free(cur->value);
	}
}

void List_clear_destroy(List *List) {

	List_clear(List);
	List_destroy(List);
}

void List_push(List *list, void *value) {

	ListNode *node = calloc(1, sizeof(List));
	check_mem(node);

	node->value = value;

	if(list->last == NULL) {
		list->first = node;
		list->last = node;
	}
	else {
		list->last->next = node;
		node->prev = list->last;
		list->last = node;
	}

	list->count++;

error:
	return;
}

void *List_pop(List *List) {

	ListNode *node = List->last;
	return node != NULL ? List_remove(List, node) : NULL;
}

void List_unshift(List *list, void *value) {

	ListNode *node = calloc(1, sizeof(ListNode));
	check_mem(node);
	
	node->value = value;

	if(list->first == NULL) {
		list->first = node;
		list->last = node;
	}
	else {
		node->next = list->first;
		list->first->prev = node;
		list->first = node;
	}

	list->count++;

error:
	return;
}

void *List_shift(List *List) {

	ListNode *node = List->first;
	return node != NULL ? List_remove(List, node) : NULL;
}

void *List_remove(List *list, ListNode *node) {

	void *result = NULL;

	check(list->first && list->last, "List is epmty.");
	check(node, "node can't be NULL");

	if (node == list->first && node == list->last) {
		list->first = NULL;
		list->last = NULL;
	}
	else if(node == list->first) {
		list->first = node->next;
		check(list->first != NULL, "Invalid list, somehow got a first that is NULL.");
		list->first->prev = NULL;
	}
	else if(node == list->last) {
		list->last = node->prev;
		check(list->last != NULL, "Invaliv list, somehow got a last that is NULL.");
		list->last->next = NULL;
	}
	else {
		ListNode *after = node->next;
		ListNode *before = node->prev;
		after->prev = before;
		before->next = after;
	}

	list->count--;
	result = node->value;
	free(node);

error:
	return result;
}
