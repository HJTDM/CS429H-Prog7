#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

#include "hashmap.h"

/**
 * @brief Checks if the line contains data.
 * 
 * @param line the line to check
 * @return True if the line contains data, false otherwise
 */
bool is_data(const char* line);

/**
 * @brief Checks if the line is empty.
 * 
 * @param line the line to check
 * @return True if the line is empty, false otherwise
 */
bool is_empty(const char* line);

/**
 * @brief Trims whitespace from the string.
 * 
 * @param str the string to trim
 */
void trim(char* str);

/**
 * @brief Checks if the string is an unsigned 64-bit integer.
 * 
 * @param str the string to check
 * @return True if the string is an unsigned 64-bit integer, false otherwise
 */
bool is_uint64(const char* str);

/**
 * @brief Checks if the register is valid.
 * 
 * @param reg the register to check
 * @return True if the register is valid, false otherwise
 */
bool is_valid_register(const char* reg);

/**
 * @brief Checks if the literal is valid.
 * 
 * @param lhm the label hashmap
 * @param instruction the instruction to check
 * @param L the literal to check
 * @return True if the literal is valid, false otherwise
 */
bool is_valid_literal(HashMap* lhm, char* instruction, char* L);

/**
 * @brief Checks if the label is valid.
 * 
 * @param label the label to check
 * @return True if the label is valid, false otherwise
 */
bool is_valid_label(char* label);

#endif