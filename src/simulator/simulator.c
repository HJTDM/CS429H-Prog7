#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "simulator/simulator.h"
#include "simulator/utils.h"

#define FILE_TYPE 0
#define INIT_CODE_ADDR 0x2000
#define INIT_DATA_ADDR 0x10000

TinkerFileHeader* create_tinker_file_header(){
	TinkerFileHeader* tfh = (TinkerFileHeader*) malloc(sizeof(TinkerFileHeader));

	if (tfh == NULL) {
		// Print error message and exit if memory allocation fails
		fprintf(stderr, "Failed to allocate memory for TinkerFileHeader\n");
		exit(1);
	}
	
	tfh->fileType = FILE_TYPE;
	tfh->codeBegin = INIT_CODE_ADDR;
	tfh->dataBegin = INIT_DATA_ADDR;

	return tfh;
}

Processor* create_processor(){
	Processor* processor = (Processor*) malloc(sizeof(Processor));

	if(processor == NULL){
		fprintf(stderr, "Error: failed to allocate memory for Processor");
		exit(1);
	}

	// Initialize the program counter to the starting address
	processor->pc = INIT_CODE_ADDR;
	// Initialize the registers and memory
	memset(processor->registers, 0, sizeof(processor->registers));
	memset(processor->memory, 255, sizeof(processor->memory));
	// Set the stack pointer register to the memory size
	processor->registers[31] = MEM_SIZE;
	processor->mode = USER_MODE;

	populate_instructions(processor);
	return processor;
}

void simulate_program(const char* filename){
	Processor* processor = create_processor();
	if(load_memory(filename, processor) == -1){
		fprintf(stderr, "Simulation error: failed to load memory\n");
		destroy_processor(processor);
		exit(1);
	}

	int status = 0;
	// Process instructions until an error or halt
	while((status = process_instruction(processor)) == 0){
		processor->pc += 4;

		// Check for program counter out of bounds
		if(processor->pc < INIT_CODE_ADDR || processor->pc >= INIT_DATA_ADDR){
			fprintf(stderr, "Simulation error: program counter out of bounds\n");
			destroy_processor(processor);
			exit(1);
		}
	}

	if(status == -1){
		fprintf(stderr, "Simulation error: invalid instruction\n");
		destroy_processor(processor);
		exit(1);
	}

	destroy_processor(processor);
	return;
}

int load_memory(const char* filename, Processor* processor){
	FILE *fp = fopen(filename, "rb");

	// Check if the input file was opened successfully
	if(fp == NULL){
		fprintf(stderr, "Invalid tinker filepath\n");
		return -1;
	}

	// Get file header data
	TinkerFileHeader* tfh = create_tinker_file_header();
	fread(tfh, sizeof(TinkerFileHeader), 1, fp);

	if(tfh->codeSize > INIT_DATA_ADDR - INIT_CODE_ADDR){
		fprintf(stderr, "Code segment is too large\n");
		return -1;
	}

	fread(&processor->memory[tfh->codeBegin], tfh->codeSize, 1, fp);
	fread(&processor->memory[tfh->dataBegin], tfh->dataSize, 1, fp);

	fclose(fp);
	return 0;
}

int process_instruction(Processor* processor){
	uint32_t instr;
	// Fetch the instruction from memory
	memcpy(&instr, &processor->memory[processor->pc], sizeof(instr));
	
	// Decode the instruction
	uint8_t opcode = (instr >> 27) & 0x1F;
	uint8_t rd = (instr >> 22) & 0x1F;
	uint8_t rs = (instr >> 17) & 0x1F;
	uint8_t rt = (instr >> 12) & 0x1F;
	int16_t L = instr & 0xFFFF;

	// Check for invalid opcode
	if(opcode < 0 || opcode > 29){
		fprintf(stderr, "Simulation error: invalid opcode\n");
		return -1;
	}

	return processor->instructions[opcode](processor, rd, rs, rt, L);
}

void populate_instructions(Processor* processor){
	Instruction instructions[] = {
		and, or, xor, not, shftr, shftri, shftl, shftli, br, brr, brrL, brnz, call, ret, brgt, priv, 
		movRRL, movRR, movRL, movRLR, addf, subf, mulf, divf, add, addi, sub, subi, mul, divi
	};

	memcpy(processor->instructions, instructions, sizeof(processor->instructions));
}

int add(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	int64_t d, s, t;
	memcpy(&s, &processor->registers[rs], sizeof(int64_t));
	memcpy(&t, &processor->registers[rt], sizeof(int64_t));

	d = s + t;
	memcpy(&processor->registers[rd], &d, sizeof(uint64_t));
	return 0;
}

int addi(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	processor->registers[rd] += (L & 0xFFF);
	return 0;
}

int sub(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	int64_t d, s, t;
	memcpy(&s, &processor->registers[rs], sizeof(int64_t));
	memcpy(&t, &processor->registers[rt], sizeof(int64_t));

	d = s - t;
	memcpy(&processor->registers[rd], &d, sizeof(uint64_t));
	return 0;
}

int subi(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	processor->registers[rd] -= (L & 0xFFF);
	return 0;
}

int mul(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	int64_t d, s, t;
	memcpy(&s, &processor->registers[rs], sizeof(int64_t));
	memcpy(&t, &processor->registers[rt], sizeof(int64_t));

	d = s * t;
	memcpy(&processor->registers[rd], &d, sizeof(uint64_t));
	return 0;
}

int divi(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	int64_t d, s, t;
	memcpy(&s, &processor->registers[rs], sizeof(int64_t));
	memcpy(&t, &processor->registers[rt], sizeof(int64_t));

	// Check for division by zero
	if(t == 0){
		return -1;
	}

	d = s / t;
	memcpy(&processor->registers[rd], &d, sizeof(uint64_t));
	return 0;
}

int and(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	processor->registers[rd] = processor->registers[rs] & processor->registers[rt];
	return 0;
}

int or(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	processor->registers[rd] = processor->registers[rs] | processor->registers[rt];
	return 0;
}

int xor(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	processor->registers[rd] = processor->registers[rs] ^ processor->registers[rt];
	return 0;
}

int not(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	processor->registers[rd] = ~processor->registers[rs];
	return 0;
}

int shftr(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	processor->registers[rd] = processor->registers[rs] >> processor->registers[rt];
	return 0;
}

int shftri(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	processor->registers[rd] >>= (L & 0xFFF);
	return 0;
}

int shftl(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	processor->registers[rd] = processor->registers[rs] << processor->registers[rt];
	return 0;
}

int shftli(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	processor->registers[rd] <<= (L & 0xFFF);
	return 0;
}

int br(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	processor->pc = processor->registers[rd] - 4;
	return 0;
}

int brr(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	processor->pc += processor->registers[rd] - 4;
	return 0;
}

int brrL(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	// Sign-extend the immediate value
	L = (L & 0x800) ? (L | 0xF000) : (L & 0xFFF);
	processor->pc += (L - 4);
	return 0;
}

int brnz(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	if(processor->registers[rs] != 0){
		processor->pc = processor->registers[rd] - 4;
	}

	return 0;
}

int call(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	processor->pc += 4;
	// Save return address on stack
	memcpy(&processor->memory[processor->registers[31] - 8], &(processor->pc), sizeof(processor->pc));
	// Branch to subroutine address
	processor->pc = processor->registers[rd] - 4;
	return 0;
}

int ret(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	// Restore return address from stack
	memcpy(&(processor->pc), &processor->memory[processor->registers[31] - 8], sizeof(processor->pc));
	processor->pc -= 4;
	return 0;
}

int brgt(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	if(processor->registers[rs] > processor->registers[rt]){
		processor->pc = processor->registers[rd] - 4;
	}

	return 0;
}

int priv(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	switch(L){
		case 0:
			// Halt the processor
			return 1;
		case 1:
			processor->mode = SUPERVISOR_MODE;
			break;
		case 2:
			processor->mode = USER_MODE;
			break;
		case 3:
			if(processor->registers[rs] == 0){
				char buffer[50];
				fgets(buffer, sizeof(buffer), stdin);
				size_t len = strcspn(buffer, "\n");
				buffer[len] = '\0';

				// Check if input is a valid unsigned 64-bit integer
				if(is_uint64(buffer)){
					uint64_t val = strtoull(buffer, NULL, 10);
					processor->registers[rd] = val;
				}
				else{
					return -1;
				}
			}

			break;
		case 4:
			if(processor->registers[rd] == 1){
				printf("%lu\n", processor->registers[rs]);
			}
			else if(processor->registers[rd] == 3){
				char character = processor->registers[rs] & 0xFF;
				printf("%c", character);
			}
			break;
		default:
			return -1;
	}

	return 0;
}

int movRRL(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	// Sign-extend the immediate value
	L = (L & 0x800) ? (L | 0xF000) : (L & 0xFFF);
	uint64_t index = processor->registers[rs] + L;

	// Check for memory index out of bounds
	if(index < 0 || index > MEM_SIZE - 8){
		return -1;
	}

	// Load value from memory into register
	memcpy(&(processor->registers[rd]), &(processor->memory[index]), sizeof(processor->registers[rd]));
	return 0;
}

int movRR(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	processor->registers[rd] = processor->registers[rs];
	return 0;
}

int movRL(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	// Move immediate value to lower 12 bits of destination register
	processor->registers[rd] &= ~0xFFFULL;
	processor->registers[rd] |= (uint64_t)(L & 0xFFF);
	return 0;
}

int movRLR(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	// Sign-extend the immediate value
	L = (L & 0x800) ? (L | 0xF000) : (L & 0xFFF);
	uint64_t index = processor->registers[rd] + L;

	// Check for memory index out of bounds
	if(index < 0 || index > MEM_SIZE - 8){
		return -1;
	}

	// Store value from register to memory
	memcpy(&(processor->memory[index]), &(processor->registers[rs]), sizeof(processor->registers[rs]));
	return 0;
}

int addf(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	double d, s, t;
	memcpy(&s, &processor->registers[rs], sizeof(double));
	memcpy(&t, &processor->registers[rt], sizeof(double));

	d = s + t;
	memcpy(&processor->registers[rd], &d, sizeof(uint64_t));
	return 0;
}

int subf(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	double d, s, t;
	memcpy(&s, &processor->registers[rs], sizeof(double));
	memcpy(&t, &processor->registers[rt], sizeof(double));

	d = s - t;
	memcpy(&processor->registers[rd], &d, sizeof(uint64_t));
	return 0;
}

int mulf(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	double d, s, t;
	memcpy(&s, &processor->registers[rs], sizeof(double));
	memcpy(&t, &processor->registers[rt], sizeof(double));

	d = s * t;
	memcpy(&processor->registers[rd], &d, sizeof(uint64_t));
	return 0;
}

int divf(Processor* processor, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	double d, s, t;
	memcpy(&s, &processor->registers[rs], sizeof(double));
	memcpy(&t, &processor->registers[rt], sizeof(double));

	if(t == 0){
		return -1;
	}

	d = s / t;
	memcpy(&processor->registers[rd], &d, sizeof(uint64_t));
	return 0;
}

void destroy_processor(Processor* processor){
	free(processor);
}