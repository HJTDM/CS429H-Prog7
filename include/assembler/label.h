#ifndef LABEL_H
#define LABEL_H

#include <stdint.h>

/**
 * @brief Represents a label with a name and an address.
 */
typedef struct Label{
	char* name; /**< The name of the label */
	uint64_t address; /**< The address of the label */
} Label;

/**
 * @brief Creates a new label.
 * 
 * @param name the name of the label
 * @param address the address of the label
 * @return A pointer to the newly created label
 */
Label* create_label(char* name, uint64_t address);

/**
 * @brief Prints the label information.
 * 
 * @param ptr a pointer to the label to be printed
 */
void print_label(void* ptr);

/**
 * @brief Destroys the label and frees associated memory.
 * 
 * @param ptr a pointer to the label to be destroyed
 */
void destroy_label(void* ptr);

#endif