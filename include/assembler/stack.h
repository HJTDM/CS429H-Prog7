#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdbool.h>

/// @brief Represents a node in the stack
typedef struct StackNode {
	char* value;
	struct StackNode* next;
} StackNode;

/**
 * @brief Creates a new stack node with the given value
 * 
 * @param value the value to be stored in the node
 * @return A pointer to the newly created stack node
 */
StackNode* create_node(char* value);

/**
 * @brief Creates a new stack node with the given value and next node
 * 
 * @param value the value to be stored in the node
 * @param next the next node in the stack
 * @return A pointer to the newly created stack node
 */
StackNode* create_node_with_next(char* value, StackNode* next);

/// @brief Represents a stack data structure
typedef struct Stack {
	int size;
	struct StackNode* head;
	struct StackNode* tail;
} Stack;

/**
 * @brief Creates a new stack
 * 
 * @return A pointer to the newly created stack
 */
Stack* create_stack();

/**
 * @brief Pushes a value onto the stack
 * 
 * @param stack the stack to push the value onto
 * @param value the value to be pushed onto the stack
 */
void stack_push(Stack* stack, char* value);

/**
 * @brief Pops a value from the stack
 * 
 * @param stack the stack to pop the value from
 * @return The value popped from the stack
 */
char* stack_pop(Stack* stack);

/**
 * @brief Peeks at the top value of the stack without removing it
 * 
 * @param stack the stack to peek at
 * @return The value at the top of the stack
 */
char* stack_peek(Stack* stack);

/**
 * @brief Destroys the stack and frees all associated memory
 * 
 * @param stack the stack to be destroyed
 */
void destroy_stack(Stack* stack);

/**
 * @brief Gets the size of the stack
 * 
 * @param stack the stack to get the size of
 * @return The size of the stack
 */
int stack_get_size(Stack* stack);

/**
 * @brief Checks if the stack is empty
 * 
 * @param stack the stack to check
 * @return True if the stack is empty, false otherwise
 */
bool stack_is_empty(Stack* stack);

/**
 * @brief Clears all elements from the stack
 * 
 * @param stack the stack to be cleared
 */
void stack_clear(Stack* stack);

#endif