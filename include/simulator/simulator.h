#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stdint.h>

#define NUM_REGS 32
#define MEM_SIZE 512 * 1024
#define NUM_INSTR 30

typedef struct TinkerFileHeader {
	uint64_t fileType; // Currently, 0
	uint64_t codeBegin; // Address into which the code is to be loaded in memory
	uint64_t codeSize; // Size of the code segment
	uint64_t dataBegin; // Address into which the data is to be loaded in memory
	uint64_t dataSize; // Size of the data segment (could be 0)
} TinkerFileHeader;

TinkerFileHeader* create_tinker_file_header();

/// @brief Structure representing a processor.
typedef struct Processor Processor;

/**
 * @brief Function pointer type for instructions.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
typedef int (*Instruction)(Processor*, uint8_t, uint8_t, uint8_t, int16_t);

/// @brief Enumeration for operation modes.
typedef enum {
	USER_MODE,
	SUPERVISOR_MODE
} OpMode;

struct Processor {
	uint64_t pc;                     /**< Program counter */
	uint64_t registers[NUM_REGS];    /**< General purpose registers */
	uint8_t memory[MEM_SIZE];        /**< Memory */
	Instruction instructions[NUM_INSTR]; /**< Instruction set */
	OpMode mode;                     /**< Current operation mode */
};

/**
 * @brief Creates a new processor.
 * 
 * @return Pointer to the newly created processor
 */
Processor* create_processor();

/**
 * @brief Simulates a program from a file.
 * 
 * @param filename path to the program file
 */
void simulate_program(const char* filename);

/**
 * @brief Loads memory from a file into the processor.
 * 
 * @param filename path to the memory file
 * @param processor pointer to the processor
 */
int load_memory(const char* filename, Processor* processor);

/**
 * @brief Processes a single instruction.
 * 
 * @param processor pointer to the processor
 * @return 0 if successful, non-zero otherwise
 */
int process_instruction(Processor* processor);

/**
 * @brief Populates the processor's instruction set.
 * 
 * @param processor pointer to the processor
 */
void populate_instructions(Processor* processor);

/**
 * @brief Adds two registers and stores the result in a destination register.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int add(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Adds a register and a literal and stores the result in a destination register.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int addi(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Subtracts one register from another and stores the result in a destination register.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int sub(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Subtracts a literal from a register and stores the result in a destination register.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int subi(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Multiplies two registers and stores the result in a destination register.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int mul(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Divides one register by another and stores the result in a destination register.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int divi(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Performs a bitwise AND on two registers and stores the result in a destination register.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int and(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Performs a bitwise OR on two registers and stores the result in a destination register.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int or(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Performs a bitwise XOR on two registers and stores the result in a destination register.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int xor(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Performs a bitwise NOT on a register and stores the result in a destination register.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int not(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Shifts a register right and stores the result in a destination register.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int shftr(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Shifts a register right by a literal and stores the result in a destination register.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int shftri(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Shifts a register left and stores the result in a destination register.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int shftl(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Shifts a register left by a literal and stores the result in a destination register.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int shftli(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Branches to the address in the destination register.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int br(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Branches to the address of the program counter offset by the value in the destination register.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int brr(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Branches to an address with a literal offset.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int brrL(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Branches if the value in a register is not zero.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int brnz(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Calls a function at the address in the register.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int call(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Returns from a function.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int ret(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Branches if the value in a register is greater than another register.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int brgt(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Executes a privileged instruction.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int priv(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Moves a value from memory to a register.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int movRRL(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Moves a value from one register to another.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int movRR(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Moves a literal value to the last 12 bits of a register.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int movRL(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Moves a value from a register to memory.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int movRLR(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Adds two floating-point registers and stores the result in a destination register.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int addf(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Subtracts one floating-point register from another and stores the result in a destination register.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int subf(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Multiplies two floating-point registers and stores the result in a destination register.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int mulf(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Divides one floating-point register by another and stores the result in a destination register.
 * 
 * @param processor pointer to the processor
 * @param rd destination register
 * @param rs source register
 * @param rt source register
 * @param L 12 bit literal
 * @return 0 if successful, non-zero otherwise
 */
int divf(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L);

/**
 * @brief Destroys a processor and frees its memory.
 * 
 * @param processor pointer to the processor
 */
void destroy_processor(Processor* processor);

#endif