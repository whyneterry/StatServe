#ifndef LCTHW_LIST_ALGOS_H
#define LCTHW_LIST_ALGOS_H

#include <lcthw/list.h>

void bubbleSort(List *List, char *datatype);
ListNode *mergeSortSplit(ListNode *head);
ListNode *mergeSortMerge(ListNode *first, ListNode *second, char *datatype);
ListNode *mergeSortStart(ListNode *head, char *datatype);
#endif
