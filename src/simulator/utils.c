#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <errno.h>

#include "simulator/utils.h"

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