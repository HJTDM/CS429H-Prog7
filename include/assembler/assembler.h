#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "hashmap.h"

/// @brief A struct representing the file header for a tinker program
typedef struct TinkerFileHeader {
	uint64_t fileType; // Currently, 0
	uint64_t codeBegin; // Address into which the code is to be loaded in memory
	uint64_t codeSize; // Size of the code segment
	uint64_t dataBegin; // Address into which the data is to be loaded in memory
	uint64_t dataSize; // Size of the data segment (could be 0)
} TinkerFileHeader;

/**
 * @brief Creates a pointer to a new tinker file header
 * 
 * @return The pointer to the new tinker file header
 */
TinkerFileHeader* create_tinker_file_header();

/**
 * @brief Generates the object file from the input file
 * 
 * @param inputFile path to the input file
 * @param outputFile path to the output file
 */
void generate_object_file(const char* inputFile, const char* outputFile);

/**
 * @brief Checks and opens the input and output files
 * 
 * @param inputFile path to the input file
 * @param outputFile path to the output file
 * @param in pointer to the input file pointer
 * @param out pointer to the output file pointer
 */
void check_files(const char* inputFile, const char* outputFile, FILE** in, FILE** out);

/**
 * @brief Populates the label hash map from the input file
 * 
 * @param in pointer to the input file
 * @param lhm pointer to the label hash map
 * @param tfh pointer to the tinker file header
 * @return 0 if successful, non-zero otherwise
 */
int populate_labels(FILE* in, HashMap* lhm, TinkerFileHeader* tfh);

/**
 * @brief Resolves the program and writes the object file
 * 
 * @param in pointer to the input file
 * @param out pointer to the output file
 * @param lhm pointer to the label hash map
 * @param ihm pointer to the instruction hash map
 * @param tfh pointer to the tinker file header
 * @return 0 if successful, non-zero otherwise
 */
int resolve_program(FILE* in, FILE* out, HashMap* lhm, HashMap* ihm, TinkerFileHeader* tfh);

#endif