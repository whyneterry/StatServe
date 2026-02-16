#include <stdio.h>
#include <stdlib.h>

#include "lcthw/queue.h"

Queue *Queue_create()
{
	Queue *queue = calloc(1, sizeof(Queue));
	queue->elements = calloc(MAX_SIZE, sizeof(void *));

	return queue;
}

int Queue_destroy(Queue *queue)
{
	if (queue != NULL) {
		if (queue->elements != NULL) {
			free(queue->elements);
		}
		free(queue);
		return 0;
	}
	
	return -1;
}

int Queue_send(Queue *queue, void *element)
{
	if (queue != NULL && element != NULL) {
		if (queue->count != MAX_SIZE) {	
			queue->elements[MAX_SIZE - 1 - queue->count] = element;
			queue->count++;
			
			return 0;
		}
	}
	
	return -1;
}

void *Queue_recv(Queue *queue)
{
	if (queue != NULL) {
		if (queue->count != 0 && queue->elements != NULL) {
			queue->popped++;
			queue->count--;
			return queue->elements[MAX_SIZE - queue->popped];
		}
	}
	
	return NULL;
}

void *Queue_peek(Queue *queue)
{
	if (queue != NULL) {
		if (queue->count != 0 && queue->elements != NULL) {
			return queue->elements[MAX_SIZE - 1 - queue->popped];
		}
	}
	
	return NULL;
}

void Queue_print(Queue *queue)
{
	if (queue == NULL) {
		return;
	}
	int i = MAX_SIZE - 1;
	for (i = MAX_SIZE - 1; i > MAX_SIZE - queue->count - 1; i--) {
		printf("%s | ", (char *)queue->elements[9]);
	}
	printf("\n");
}
