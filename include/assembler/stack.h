#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdbool.h>

typedef struct StackNode {
	char* value;
	struct StackNode* next;
} StackNode;

StackNode* create_node(char* value);

StackNode* create_node_with_next(char* value, StackNode* next);

typedef struct Stack {
	int size;
	struct StackNode* head;
	struct StackNode* tail;
} Stack;

Stack* create_stack();

void stack_push(Stack* stack, char* value);

char* stack_pop(Stack* stack);

char* stack_peek(Stack* stack);

void destroy_stack(Stack* stack);

int stack_get_size(Stack* stack);

bool stack_is_empty(Stack* stack);

void stack_clear(Stack* stack);

#endif