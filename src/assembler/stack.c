#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler/stack.h"

StackNode* create_node(char* value) {
    StackNode* stackNode = malloc(sizeof(StackNode));

    if (stackNode == NULL) {
        printf("Failed to allocate memory\n");
        exit(-1);
    }
	else{
		stackNode->value = value;
		stackNode->next = NULL;
	}

    return stackNode;
}

StackNode* create_node_with_next(char* value, StackNode* next) {
    StackNode* stackNode = malloc(sizeof(StackNode));

    if (stackNode == NULL) {
        printf("Failed to allocate memory\n");
        exit(-1);
    }
	else{
		stackNode->value = value;
		stackNode->next = next;
	}

    return stackNode;
}

Stack* create_stack(){
	Stack* stack = malloc(sizeof(Stack));

    if (stack == NULL) {
        printf("Failed to allocate memory for Stack\n");
        exit(1);
    }
	else{
		stack->size = 0;
		stack->head = NULL;
		stack->tail = NULL;
	}

	return stack;
}

void stack_push(Stack* stack, char* value){
	if (stack == NULL) {
        return;
    }

	StackNode* newNode = create_node(value);

	if(stack->tail == NULL){
		stack->tail = newNode;
	}

	newNode->next = stack->head;
	stack->head = newNode;
	stack->size++;
}

char* stack_pop(Stack* stack){
	if (stack == NULL || stack->size == 0) {
        return NULL;
    }

	char* value = strdup(stack->head->value);
	StackNode* temp = stack->head;
	stack->head = stack->head->next;
	free(temp->value);
	free(temp);

	stack->size--;
	return value;
}

char* stack_peek(Stack* stack){
	if (stack == NULL) {
        return NULL;
    }

	return stack->head->value;
}

void destroy_stack(Stack* stack){
	if (stack == NULL) {
        return;
    }

    StackNode* currentNode = stack->head;
    while (currentNode != NULL) {
        StackNode* temp = currentNode;
        currentNode = currentNode->next;
        free(temp->value);
        free(temp);
    }

    free(stack);
}

int stack_get_size(Stack* stack){	
	return stack->size;
}

bool stack_is_empty(Stack* stack){
	return stack->size == 0;
}

void stack_clear(Stack* stack){
	if (stack == NULL) {
		return;
	}

	StackNode* currentNode = stack->head;
	while (currentNode != NULL) {
		StackNode* temp = currentNode;
		currentNode = currentNode->next;
		free(temp);
	}

	stack->size = 0;
	stack->head = NULL;
	stack->tail = NULL;
}