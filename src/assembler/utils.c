#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <errno.h>
#include <limits.h>

#include "assembler/hashmap.h"
#include "assembler/utils.h"
#include "assembler/label.h"

bool is_data(const char* line){
	// Check if the line is empty or does not start with a tab
	if (line == NULL || *line != '\t'){
		return false;
	}

	char* temp = strdup(line);
	trim(temp);

	if(!is_uint64(temp)){
		free(temp);
		return false;
	}
	
	free(temp);
	return true;
}

bool is_empty(const char *str) {
	// Check if the string contains only whitespace
	while (*str != '\0') {
		if (!isspace((unsigned char)*str))
			return false;
		str++;
	}

	return true;
}

void trim(char* str) {
	// Check if the string is NULL or empty
	if (str == NULL || *str == '\0') {
		return;
	}

	// Trim leading whitespace
	char* start = str;
	while (isspace((unsigned char)*start)) {
		start++;
	}

	// Trim trailing whitespace
	char* end = start + strlen(start) - 1;
	while (end > start && isspace((unsigned char)*end)) {
		*end = '\0';
		end--;
	}

	// Move the trimmed string to the original position
	if (start != str) {
		memmove(str, start, strlen(start) + 1);
	}
}

bool is_uint64(const char* str) {
	// Check if the string is NULL or empty
	if (str == NULL || *str == '\0') {
		return false;
	}

	// Check if the string contains only digits
	const char* ptr = str;
	while (*ptr) {
		if (!isdigit((unsigned char)*ptr)) {
			return false;
		}
		ptr++;
	}

	errno = 0;
	// Convert the string to an unsigned long long and check if it fits in 64 bits
	unsigned long long val = strtoull(str, NULL, 10);
	if (errno == ERANGE || val > UINT64_MAX || errno == EINVAL) {
		return false;
	}

	return true;
}

bool is_valid_register(const char* reg){
	if(!is_uint64(reg)){
		return false;
	}

	unsigned long long val = strtoull(reg, NULL, 10);
	return (val >= 0 && val <= 31);
}

bool is_valid_literal(HashMap* lhm, char* instruction, char* L){
	// Check if the literal is a label
	if (L[0] == ':') {
		if(hashmap_get(lhm, L) == NULL){
			return false;
		}

		uint64_t address = ((Label*) hashmap_get(lhm, L))->address;
		
		// Check if the label address is invalid
		if (address == -1) {
			return false;
		}
		// Check if the address is within range for specific instructions
		else if (strcmp(instruction, "brr") == 0 || strcmp(instruction, "mov") == 0) {
			if(address > 2047){
				return false;
			}
		}
		// Check if the address is within range for other instructions
		else if (strcmp(instruction, "ld") != 0 && address > 4095) {
			return false;
		}
	} 
	else {
		char *endptr, *endptr2;
		errno = 0;

		// Check if the literal is within range for specific instructions
		if (strcmp(instruction, "brr") == 0 || strcmp(instruction, "movmem") == 0) {
			int64_t val = strtoll(L, &endptr, 10);

			// Validate the conversion and range
			if (errno == ERANGE || endptr == L || *endptr != '\0') {
				return false;
			}
			else if (val < -2048 || val > 2047) {
				return false;
			}
		} 
		else {
			uint64_t val = strtoull(L, &endptr, 10);
			int64_t val2 = strtoll(L, &endptr2, 10);

			// Validate the conversion and range
			if (errno == ERANGE || endptr == L || *endptr != '\0' ||
				endptr2 == L || *endptr2 != '\0') {
				return false;
			}
			// Check if the value is negative for ld instruction
			else if (strcmp(instruction, "ld") == 0 && val2 < 0) {
				return false;
			}
			// Check if the value is within range for other instructions
			else if (strcmp(instruction, "ld") != 0 && (val < 0 || val > 4095)) {
				return false;
			}
		}
	}

	return true;
}

bool is_valid_label(char* label){
	// Check if the string is NULL or empty
	if (label == NULL || *label == '\0') {
		return false;
	}

	// Check if the string doesnt contain spaces
	const char* ptr = label;
	while (*ptr) {
		if (isspace((unsigned char)*ptr)) {
			return false;
		}
		ptr++;
	}

	return true;
}