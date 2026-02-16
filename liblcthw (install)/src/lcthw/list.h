#ifndef lcthw_List_h
#define lcthw_List_h

#include <stdlib.h>

struct ListNode;
typedef struct ListNode {
	struct ListNode *next;
	struct ListNode *prev;
	void *value;
} ListNode;

typedef struct List {
	int count;
	ListNode *first;
	ListNode *last;
} List;

List *List_create();
void List_destroy(List *List);
void List_clear(List *List);
void List_clear_destroy(List *List);
void List_print(List *List, char *datatype);
List *List_join(List *List1, List *List2);

#define List_count(A) ((A)->count)
#define List_first(A) ((A)->first != NULL ? (A)->first->value : NULL)
#define List_last(A) ((A)->last != NULL ? (A)->last->value : NULL)

void List_push(List *List, void *value);
void *List_pop(List *List);

void List_unshift(List *List, void *value);
void *List_shift(List *List);

void *List_remove(List *List, ListNode *Node);

List *List_copy(List *List_from);

#define LIST_FOREACH(L, S, M, V, N)\
ListNode *N = NULL;\
ListNode *V = NULL;\
for(V = N = L->S; N != NULL; V = N = N->M)

#endif
