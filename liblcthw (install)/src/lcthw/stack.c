#include "lcthw/stack.h"

#include <stdlib.h>
#include <stdio.h>

Stack *Stack_create()
{
	Stack *stack = calloc(1, sizeof(Stack));
	stack->elements = calloc(MAX_SIZE, sizeof(void *));
	return stack;
}

int Stack_destroy(Stack *stack)
{
	if (stack) {
		free(stack);
		return 0;
	}
	
	return -1;
}

int Stack_push(Stack *stack, void *element)
{
	if (stack->top == MAX_SIZE) {
		return -1;
	}
	stack->elements[stack->top] = element;
	stack->top++;

	return 0;
}

void *Stack_pop(Stack *stack)
{
	if (stack->top == 0) {
		return NULL;
	}

	stack->top--;
	return stack->elements[stack->top];
}

void *Stack_peek(Stack *stack)
{
	if (stack->top == 0) {
		return NULL;
	}

	return stack->elements[stack->top - 1];
}

void Stack_print(Stack *stack)
{
	int i = 0;
	for (i = 0; i < stack->top; i++) {
		printf("%s | ", (char *)stack->elements[i]);
	}
	printf("\n");
}
