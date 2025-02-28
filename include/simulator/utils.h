#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

/**
 * @brief Checks if the string is an unsigned 64-bit integer.
 * 
 * @param str the string to check
 * @return True if the string is an unsigned 64-bit integer, false otherwise
 */
bool is_uint64(const char* str);

#endif