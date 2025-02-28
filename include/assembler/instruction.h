#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdio.h>

#include "hashmap.h"

/**
 * @brief Instruction formats enum.
 */
typedef enum {
	FORMAT_RRR,
	FORMAT_RR,
	FORMAT_R,
	FORMAT_RL,
	FORMAT_RRRL,
	FORMAT_BRR,
	FORMAT_MOV,
	FORMAT_NONE
} InstrFormat;

/**
 * @brief Represents an instruction.
 */
typedef struct Instruction {
	char* name;
	uint8_t opcode;
	InstrFormat format;
} Instruction;

/**
 * @brief Creates a new instruction.
 * 
 * @param name the name of the instruction
 * @param opcode the opcode of the instruction
 * @param format the format of the instruction
 * @return a pointer to the created instruction
 */
Instruction* create_instruction(char* name, uint8_t opcode, InstrFormat format);

/**
 * @brief Prints an instruction.
 * 
 * @param ptr a pointer to the instruction to print
 */
void print_instruction(void* ptr);

/**
 * @brief Destroys an instruction.
 * 
 * @param ptr a pointer to the instruction to destroy
 */
void destroy_instruction(void* ptr);

/**
 * @brief Processes an instruction line.
 * 
 * @param out the output file
 * @param lhm the label hashmap
 * @param ihm the instruction hashmap
 * @param line the instruction line to process
 * @return 0 on success, non-zero on failure
 */
int process_instruction(FILE* out, HashMap* lhm, HashMap* ihm, char* line);

/**
 * @brief Processes an RRR format instruction.
 * 
 * @param out the output file
 * @param ihm the instruction hashmap
 * @param line the instruction line to process
 * @return 0 on success, non-zero on failure
 */
int process_RRR_instr(FILE* out, HashMap* ihm, char* line);

/**
 * @brief Processes an RR format instruction.
 * 
 * @param out the output file
 * @param ihm the instruction hashmap
 * @param line the instruction line to process
 * @return 0 on success, non-zero on failure
 */
int process_RR_instr(FILE* out, HashMap* ihm, char* line);

/**
 * @brief Processes an R format instruction.
 * 
 * @param out the output file
 * @param ihm the instruction hashmap
 * @param line the instruction line to process
 * @return 0 on success, non-zero on failure
 */
int process_R_instr(FILE* out, HashMap* ihm, char* line);

/**
 * @brief Processes an RL format instruction.
 * 
 * @param out the output file
 * @param lhm the label hashmap
 * @param ihm the instruction hashmap
 * @param line the instruction line to process
 * @return 0 on success, non-zero on failure
 */
int process_RL_instr(FILE* out, HashMap* lhm, HashMap* ihm, char* line);

/**
 * @brief Processes an RRRL format instruction.
 * 
 * @param out the output file
 * @param lhm the label hashmap
 * @param ihm the instruction hashmap
 * @param line the instruction line to process
 * @return 0 on success, non-zero on failure
 */
int process_RRRL_instr(FILE* out, HashMap* lhm, HashMap* ihm, char* line);

/**
 * @brief Processes a BRR format instruction.
 * 
 * @param out the output file
 * @param lhm the label hashmap
 * @param ihm the instruction hashmap
 * @param line the instruction line to process
 * @return 0 on success, non-zero on failure
 */
int process_BRR_instr(FILE* out, HashMap* lhm, HashMap* ihm, char* line);

/**
 * @brief Processes a MOV format instruction.
 * 
 * @param out the output file
 * @param lhm the label hashmap
 * @param ihm the instruction hashmap
 * @param line the instruction line to process
 * @return 0 on success, non-zero on failure
 */
int process_MOV_instr(FILE* out, HashMap* lhm, HashMap* ihm, char* line);

/**
 * @brief Processes a NONE format instruction.
 * 
 * @param out the output file
 * @param line the instruction line to process
 * @return 0 on success, non-zero on failure
 */
int process_NONE_instr(FILE* out, char* line);

/**
 * @brief Processes a directive line.
 * 
 * @param line the directive line to process
 * @return the directive character
 */
char process_directive(char* line);

/**
 * @brief Encodes an instruction.
 * 
 * @param opcode the opcode of the instruction
 * @param rd the destination register
 * @param rs the source register
 * @param rt the target register
 * @param L the literal value
 * @return the encoded instruction
 */
uint32_t encode_instruction(uint8_t opcode, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Change in the current address from a line of code.
 * 
 * @param line the line to process
 * @return the change in address
 */
int8_t change_in_address(char* line);

/**
 * @brief Creates a hashmap for instructions.
 * 
 * @return a pointer to the created hashmap
 */
HashMap* create_instr_hashmap();

#endif