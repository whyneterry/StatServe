#ifndef _LCTHW_STACK_H_
#define _LCTHW_STACK_H_

#define MAX_SIZE 10

#define Stack_count(A) A->top

typedef struct Stack {
	void **elements;
	int top;
} Stack;

Stack *Stack_create();

int Stack_destroy(Stack *stack);

int Stack_push(Stack *stack, void *element);

void *Stack_pop(Stack *stack);

void *Stack_peek(Stack *stack);

void Stack_print(Stack *stack);

#endif
