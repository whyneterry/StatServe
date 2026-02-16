#ifndef _LCTHW_QUEUE_H_
#define _LCTHW_QUEUE_H_

#define MAX_SIZE 10

#define Queue_count(A) A->count

typedef struct Queue {
	void **elements;
	int count;
	int popped;
} Queue;

Queue *Queue_create();

int Queue_destroy(Queue *queue);

int Queue_send(Queue *queue, void *element);

void *Queue_recv(Queue *queue);

void *Queue_peek(Queue *peek);

void Queue_print(Queue *queue);

#endif
