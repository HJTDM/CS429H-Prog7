#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler/label.h"

Label* create_label(char* name, uint64_t address) {
	Label* label = (Label*) malloc(sizeof(Label));

	if (label == NULL) {
		// Print error message and exit if memory allocation fails
		fprintf(stderr, "Error: failed to allocate memory for Label\n");
		exit(1);
	}
	
	label->name = name;
	label->address = address;

	return label;
}

void print_label(void* ptr){
	Label* label = (Label*) ptr;
	printf("Name: %s, Address: 0x%lx\n", label->name, label->address);
}

void destroy_label(void* ptr){
	Label* label = (Label*) ptr;
	free(label->name);
	free(label);
}