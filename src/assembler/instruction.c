#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "assembler/instruction.h"
#include "assembler/label.h"
#include "assembler/utils.h"

Instruction* create_instruction(char* name, uint8_t opcode, InstrFormat format){
	Instruction* instruction = (Instruction*) malloc(sizeof(Instruction));

	if (instruction == NULL) {
		// Print error message and exit if memory allocation fails
		fprintf(stderr, "Error: failed to allocate memory for Instruction\n");
		exit(1);
	}
	
	instruction->name = name;
	instruction->opcode = opcode;
	instruction->format = format;

	return instruction;
}

void print_instruction(void* ptr){
	Instruction* instruction = (Instruction*) ptr;
	printf("Name: %s, Opcode: 0x%x, Format: %d\n", instruction->name, instruction->opcode, instruction->format);
}

void destroy_instruction(void* ptr){
	Instruction* instruction = (Instruction*) ptr;
	free(instruction);
}

int process_instruction(FILE* out, HashMap* lhm, HashMap* ihm, char* line){
	// Continue if the tabbed line is data
	if(is_data(line)){
		return 0;
	}

	char instrType[10];
	if(sscanf(line, "\t %9s", instrType) != 1){
		fprintf(stderr, "Error: no instruction type\n");
		return -1;
	}

	// Check if instruction is a valid instruction type 
	Instruction* instr = (Instruction*) hashmap_get(ihm, instrType);
	if(instr == NULL){
		fprintf(stderr, "Error: invalid instruction type %s\n", instrType);
		return -1;
	}

	// Process instruction based on operands
	switch(instr->format){
		case FORMAT_RRR:
			return process_RRR_instr(out, ihm, line);
		case FORMAT_RR:
			return process_RR_instr(out, ihm, line);
		case FORMAT_R:
			return process_R_instr(out, ihm, line);
		case FORMAT_RL:
			return process_RL_instr(out, lhm, ihm, line);
		case FORMAT_RRRL:
			return process_RRRL_instr(out, lhm, ihm, line);
		case FORMAT_BRR:
			return process_BRR_instr(out, lhm, ihm, line);
		case FORMAT_MOV:
			return process_MOV_instr(out, lhm, ihm, line);
		case FORMAT_NONE:
			return process_NONE_instr(out, line);
		default:
			fprintf(stderr, "Error: invalid instruction format for %s instruction\n", instrType);
			return -1;
	}
}

int process_RRR_instr(FILE* out, HashMap* ihm, char* line){
	char instrType[10], rd[256], rs[256], rt[256], extra[256];

	// Check 3 register instruction format
	if(sscanf(line, "\t %9[^ ,] r%255[^ ,] , r%255[^ ,] , r%255[^ ,\n] , %255[^ ,\n]", instrType, rd, rs, rt, extra) == 4){
		if(!is_valid_register(rd) || !is_valid_register(rs) || !is_valid_register(rt)){
			fprintf(stderr, "Error: invalid registers %s, %s, %s\n", rd, rs, rt);
			return -1;
		}

		// Encode instruction into 32-bit integer
		uint8_t opcode = ((Instruction*) hashmap_get(ihm, instrType))->opcode;
		uint8_t d = strtoul(rd, NULL, 10), s = strtoul(rs, NULL, 10), t = strtoul(rt, NULL, 10);
		uint32_t instr = encode_instruction(opcode, d, s, t, 0);
		
		fwrite(&instr, sizeof(uint32_t), 1, out);
		return 0;
	}

	return -1;
}

int process_RR_instr(FILE* out, HashMap* ihm, char* line){
	char instrType[10], rd[256], rs[256], extra[256];

	// Check 2 register instruction format
	if(sscanf(line, "\t %9[^ ,] r%255[^ ,] , r%255[^ ,\n] , %255[^ ,\n]", instrType, rd, rs, extra) == 3){
		if(!is_valid_register(rd) || !is_valid_register(rs)){
			fprintf(stderr, "Error: invalid registers %s, %s\n", rd, rs);
			return -1;
		}

		// Check if RR instruction is a macro
		if(strcmp(instrType, "in") == 0){
			uint8_t d = strtoul(rd, NULL, 10), s = strtoul(rs, NULL, 10);
			uint32_t instr = encode_instruction(0xf, d, s, 0, 0x3);
			fwrite(&instr, sizeof(uint32_t), 1, out);
		}
		else if(strcmp(instrType, "out") == 0){
			uint8_t d = strtoul(rd, NULL, 10), s = strtoul(rs, NULL, 10);
			uint32_t instr = encode_instruction(0xf, d, s, 0, 0x4);
			fwrite(&instr, sizeof(uint32_t), 1, out);
		}
		else{
			// Encode instruction into 32-bit integer
			uint8_t opcode = ((Instruction*) hashmap_get(ihm, instrType))->opcode;
			uint8_t d = strtoul(rd, NULL, 10), s = strtoul(rs, NULL, 10);
			uint32_t instr = encode_instruction(opcode, d, s, 0, 0);
			fwrite(&instr, sizeof(uint32_t), 1, out);
		}
		
		return 0;
	}

	return -1;
};

int process_R_instr(FILE* out, HashMap* ihm, char* line){
	char instrType[10], rd[256], extra[256];

	// Check 1 register instruction format
	if(sscanf(line, "\t %9[^\t ,] r%255[^ ,\n] , %255[^ ,\n]", instrType, rd, extra) == 2){
		if(!is_valid_register(rd)){
			fprintf(stderr, "Error: invalid register %s\n", rd);
			return -1;
		}

		// Check if instruction is a macro
		if(strcmp(instrType, "clr") == 0){
			uint8_t d = strtoul(rd, NULL, 10);
			uint32_t instr = encode_instruction(0x2, d, d, d, 0);
			fwrite(&instr, sizeof(uint32_t), 1, out);
		}
		else if(strcmp(instrType, "push") == 0){
			uint8_t d = strtoul(rd, NULL, 10);
			uint32_t mov = encode_instruction(0x13, 31, d, 0, -8);
			uint32_t subi = encode_instruction(0x1b, 31, 0, 0, 8);
			fwrite(&mov, sizeof(uint32_t), 1, out);
			fwrite(&subi, sizeof(uint32_t), 1, out);
		}
		else if(strcmp(instrType, "pop") == 0){
			uint8_t d = strtoul(rd, NULL, 10);
			uint32_t mov = encode_instruction(0x10, d, 31, 0, 0);
			uint32_t addi = encode_instruction(0x19, 31, 0, 0, 8);
			fwrite(&mov, sizeof(uint32_t), 1, out);
			fwrite(&addi, sizeof(uint32_t), 1, out);
		}
		else{
			// Encode instruction into 32-bit integer
			uint8_t opcode = ((Instruction*) hashmap_get(ihm, instrType))->opcode;
			uint8_t d = strtoul(rd, NULL, 10);
			uint32_t instr = encode_instruction(opcode, d, 0, 0, 0);
			fwrite(&instr, sizeof(uint32_t), 1, out);
		}
		
		return 0;
	}

	return -1;
}

int process_RL_instr(FILE* out, HashMap* lhm, HashMap* ihm, char* line){
	char instrType[10], rd[256], L[256], extra[256];

	// Check register and literal instruction format
	if(sscanf(line, "\t %9[^ ,] r%255[^, ] , %255[^, \n] , %255[^ ,\n]", instrType, rd, L, extra) == 3){
		if(!is_valid_register(rd) || !is_valid_literal(lhm, instrType, L)){
			fprintf(stderr, "Error: invalid RL instruction argument format\n");
			return -1;
		}

		// Check if instruction is a macro
		if(strcmp(instrType, "ld") == 0){
			uint8_t d = strtoul(rd, NULL, 10);

			uint64_t val;
			// Make sure label is valid
			if(L[0] == ':'){
				if(hashmap_get(lhm, L) == NULL){
					fprintf(stderr, "Error: invalid ld label\n");
					return -1;
				}

				val = ((Label*) hashmap_get(lhm, L))->address;
			}
			else{
				val = strtoull(L, NULL, 10);
			}
			
			uint32_t instr = encode_instruction(0x2, d, d, d, 0);
			fwrite(&instr, sizeof(uint32_t), 1, out);
			instr = encode_instruction(0x19, d, 0, 0, (val >> 52));
			fwrite(&instr, sizeof(uint32_t), 1, out);
			instr = encode_instruction(0x7, d, 0, 0, 12);
			fwrite(&instr, sizeof(uint32_t), 1, out);
			instr = encode_instruction(0x19, d, 0, 0, ((val >> 40) & 0xFFF));
			fwrite(&instr, sizeof(uint32_t), 1, out);
			instr = encode_instruction(0x7, d, 0, 0, 12);
			fwrite(&instr, sizeof(uint32_t), 1, out);
			instr = encode_instruction(0x19, d, 0, 0, ((val >> 28) & 0xFFF));
			fwrite(&instr, sizeof(uint32_t), 1, out);
			instr = encode_instruction(0x7, d, 0, 0, 12);
			fwrite(&instr, sizeof(uint32_t), 1, out);
			instr = encode_instruction(0x19, d, 0, 0, ((val >> 16) & 0xFFF));
			fwrite(&instr, sizeof(uint32_t), 1, out);
			instr = encode_instruction(0x7, d, 0, 0, 12);
			fwrite(&instr, sizeof(uint32_t), 1, out);
			instr = encode_instruction(0x19, d, 0, 0, ((val >> 4) & 0xFFF));
			fwrite(&instr, sizeof(uint32_t), 1, out);
			instr = encode_instruction(0x7, d, 0, 0, 4);
			fwrite(&instr, sizeof(uint32_t), 1, out);
			instr = encode_instruction(0x19, d, 0, 0, (val & 0xF));
			fwrite(&instr, sizeof(uint32_t), 1, out);
		}
		else{
			// Encode instruction into 32-bit integer
			uint8_t opcode = ((Instruction*) hashmap_get(ihm, instrType))->opcode;
			uint8_t d = strtoul(rd, NULL, 10);
			int16_t val = strtoul(L, NULL, 10);
			uint32_t instr = encode_instruction(opcode, d, 0, 0, val);
			fwrite(&instr, sizeof(uint32_t), 1, out);
		}
		return 0;
	}

	return -1;
}

int process_RRRL_instr(FILE* out, HashMap* lhm, HashMap* ihm, char* line){
	char instrType[10], rd[256], rs[256], rt[256], L[256];

	// Check 3 register and literal instruction format
	if(sscanf(line, "\t %9[^ ,] r%255[^, ] , r%255[^, ] , r%255[^, ] , %255[^, \n]", instrType, rd, rs, rt, L) == 5){
		if(!is_valid_register(rd) || !is_valid_register(rs) || !is_valid_register(rt) || !is_valid_literal(lhm, instrType, L)){
			fprintf(stderr, "Error: invalid RRRL instruction argument format\n");
			return -1;
		}

		// Encode instruction into 32-bit integer
		uint8_t opcode = ((Instruction*) hashmap_get(ihm, instrType))->opcode;
		uint8_t d = strtoul(rd, NULL, 10), s = strtoul(rs, NULL, 10), t = strtoul(rt, NULL, 10);
		int16_t val = strtoul(L, NULL, 10);
		uint32_t instr = encode_instruction(opcode, d, s, t, val);
		fwrite(&instr, sizeof(uint32_t), 1, out);
		return 0;
	}

	return -1;
};

int process_BRR_instr(FILE* out, HashMap* lhm, HashMap* ihm, char* line){
	char instrType[10], rd[256], L[256], extra[256];

	// Check brr register instruction format
	if(sscanf(line, "\t %9[^ ,] r%255[^ ,\n] , %255[^ ,\n]", instrType, rd, extra) == 2){
		if(!is_valid_register(rd)){
			fprintf(stderr, "Error: invalid brr register %s\n", rd);
			return -1;
		}

		// Encode instruction into 32-bit integer
		uint8_t d = strtoul(rd, NULL, 10);
		uint32_t instr = encode_instruction(0x9, d, 0, 0, 0);
		fwrite(&instr, sizeof(uint32_t), 1, out);
		return 0;
	}

	// Check brr literal instruction format
	if(sscanf(line, "\t %9[^ ,] %255[^ ,\n] , %255[^ ,\n]", instrType, L, extra) == 2){
		if(!is_valid_literal(lhm, instrType, L)){
			fprintf(stderr, "Error: invalid brr literal %s\n", L);
			return -1;
		}

		// Encode instruction into 32-bit integer
		int16_t val = strtoul(L, NULL, 10);
		uint32_t instr = encode_instruction(0xa, 0, 0, 0, val);
		fwrite(&instr, sizeof(uint32_t), 1, out);
		return 0;
	}

	return -1;
}

int process_MOV_instr(FILE* out, HashMap* lhm, HashMap* ihm, char* line){
	char instrType[10], rd[256], rs[256], L[256], extra[256];

	// Check mov memory to register instruction format
	if(sscanf(line, "\t %9[^ ,] r%255[^ ,] , ( r%255[^ )] ) ( %255[^ )] )%255[^ \n]", instrType, rd, rs, L, extra)	== 4){
		printf("here");
		if(!is_valid_register(rd) || !is_valid_register(rs) || !is_valid_literal(lhm, "movmem", L)){
			fprintf(stderr, "Error: invalid mov registers %s, %s or literal %s\n", rd, rs, L);
			return -1;
		}

		// Encode instruction into 32-bit integer
		uint8_t d = strtoul(rd, NULL, 10), s = strtoul(rs, NULL, 10);
		int16_t val = strtoul(L, NULL, 10);
		uint32_t instr = encode_instruction(0x10, d, s, 0, val);
		fwrite(&instr, sizeof(uint32_t), 1, out);
		return 0;
	}

	// Check mov register to memory instruction format
	if(sscanf(line, "\t %9[^ (] ( r%255[^ )] ) ( %255[^ )] ) , r%255[^ ,\n]  , %255[^ ,\n]", instrType, rd, L, rs, extra) == 4){
		if(!is_valid_register(rd) || !is_valid_register(rs) || !is_valid_literal(lhm, "movmem", L)){
			fprintf(stderr, "Error: invalid mov registers %s, %s or literal %s\n", rd, rs, L);
			return -1;
		}

		// Encode instruction into 32-bit integer
		uint8_t d = strtoul(rd, NULL, 10), s = strtoul(rs, NULL, 10);
		int16_t val = strtoul(L, NULL, 10);
		uint32_t instr = encode_instruction(0x13, d, s, 0, val);
		fwrite(&instr, sizeof(uint32_t), 1, out);
		return 0;
	}

	// Check mov two register instruction format
	if(sscanf(line, "\t %9[^ ,] r%255[^ ,] , r%255[^ ,\n] , %255[^ ,\n]", instrType, rd, rs, extra) == 3){
		if(!is_valid_register(rd) || !is_valid_register(rs)){
			fprintf(stderr, "Error: invalid mov registers %s, %s\n", rd, rs);
			return -1;
		}

		// Encode instruction into 32-bit integer
		uint8_t d = strtoul(rd, NULL, 10), s = strtoul(rs, NULL, 10);
		uint32_t instr = encode_instruction(0x11, d, s, 0, 0);
		fwrite(&instr, sizeof(uint32_t), 1, out);
		return 0;
	}

	// Check mov register and literal instruction format
	if(sscanf(line, "\t %9[^ ,] r%255[^ ,] , %255[^ ,\n] , %255[^ ,\n]", instrType, rd, L, extra) == 3){
		if(!is_valid_register(rd) || !is_valid_literal(lhm, instrType, L)){
			fprintf(stderr, "Error: invalid mov register %s or literal %s\n", rd, L);
			return -1;
		}

		// Encode instruction into 32-bit integer
		uint8_t d = strtoul(rd, NULL, 10);
		int16_t val = strtoul(L, NULL, 10);
		uint32_t instr = encode_instruction(0x12, d, 0, 0, val);
		fwrite(&instr, sizeof(uint32_t), 1, out);
		return 0;
	}

	return -1;
}

int process_NONE_instr(FILE* out, char* line){
	char instrType[10], extra[10];

	// Check no operand instruction formats
	if(sscanf(line, "\t %9[^ ,\n]  %9[^ ,\n]", instrType, extra) == 1){
		if(strcmp(instrType, "return") == 0){
			uint32_t instr = encode_instruction(0xd, 0, 0, 0, 0);
			fwrite(&instr, sizeof(uint32_t), 1, out);
			return 0;
		}
		
		// Check if instruction is a macro
		if(strcmp(instrType, "halt") == 0){
			uint32_t instr = encode_instruction(0xf, 0, 0, 0, 0);
			fwrite(&instr, sizeof(uint32_t), 1, out);
			return 0;
		}
	}

	return -1;
}

char process_directive(char* line){
	char directive[10];
	
	// Extract the directive type
	if(sscanf(line, ".%9s", directive) != 1){
		return 'N';
	}

	// Check if the directive is code or data
	if(strcmp(directive, "code") == 0){
		return 'C';
	}
	else if(strcmp(directive, "data") == 0){
		return 'D';
	}
	else{
		return 'N';
	}
}

uint32_t encode_instruction(uint8_t opcode, uint8_t rd, uint8_t rs, uint8_t rt, int16_t L){
	return ((opcode & 0x1F) << 27) | ((rd & 0x1F) << 22) | ((rs & 0x1F) << 17) | ((rt & 0x1F) << 12) | (L & 0xFFF);
}

int8_t change_in_address(char* line){
	// Check if the line is data
	if(is_data(line)){
		return 8;
	}

	char instructionType[10];
	// Extract the instruction type
	if(sscanf(line, "\t %9s", instructionType) != 1){
		return -1;
	}

	// Determine the address change based on the instruction type
	if(strcmp(instructionType, "ld") == 0){
		return 48;
	}
	else if(strcmp(instructionType, "push") == 0 || strcmp(instructionType, "pop") == 0){
		return 8;
	}
	
	return 4;
}

HashMap* create_instr_hashmap(){
	HashMap* ihm = create_hashmap();

	hashmap_insert(ihm, "add", create_instruction("add", 0x18, FORMAT_RRR));
	hashmap_insert(ihm, "addi", create_instruction("addi", 0x19, FORMAT_RL));
	hashmap_insert(ihm, "sub", create_instruction("sub", 0x1a, FORMAT_RRR));
	hashmap_insert(ihm, "subi", create_instruction("subi", 0x1b, FORMAT_RL));
	hashmap_insert(ihm, "mul", create_instruction("mul", 0x1c, FORMAT_RRR));
	hashmap_insert(ihm, "div", create_instruction("div", 0x1d, FORMAT_RRR));
	hashmap_insert(ihm, "and", create_instruction("and", 0x0, FORMAT_RRR));
	hashmap_insert(ihm, "or", create_instruction("or", 0x1, FORMAT_RRR));
	hashmap_insert(ihm, "xor", create_instruction("xor", 0x2, FORMAT_RRR));
	hashmap_insert(ihm, "not", create_instruction("not", 0x3, FORMAT_RR));
	hashmap_insert(ihm, "shftr", create_instruction("shftr", 0x4, FORMAT_RRR));
	hashmap_insert(ihm, "shftri", create_instruction("shftri", 0x5, FORMAT_RL));
	hashmap_insert(ihm, "shftl", create_instruction("shftl", 0x6, FORMAT_RRR));
	hashmap_insert(ihm, "shftli", create_instruction("shftli", 0x7, FORMAT_RL));
	hashmap_insert(ihm, "br", create_instruction("br", 0x8, FORMAT_R));
	hashmap_insert(ihm, "brr", create_instruction("brr", 0x20, FORMAT_BRR));
	hashmap_insert(ihm, "brnz", create_instruction("brnz", 0xb, FORMAT_RR));
	hashmap_insert(ihm, "call", create_instruction("call", 0xc, FORMAT_R));
	hashmap_insert(ihm, "return", create_instruction("return", 0xd, FORMAT_NONE));
	hashmap_insert(ihm, "brgt", create_instruction("brgt", 0xe, FORMAT_RRR));
	hashmap_insert(ihm, "priv", create_instruction("priv", 0xf, FORMAT_RRRL));
	hashmap_insert(ihm, "mov", create_instruction("mov", 0x20, FORMAT_MOV));
	hashmap_insert(ihm, "addf", create_instruction("addf", 0x14, FORMAT_RRR));
	hashmap_insert(ihm, "subf", create_instruction("subf", 0x15, FORMAT_RRR));
	hashmap_insert(ihm, "mulf", create_instruction("mulf", 0x16, FORMAT_RRR));
	hashmap_insert(ihm, "divf", create_instruction("divf", 0x17, FORMAT_RRR));
	hashmap_insert(ihm, "in", create_instruction("in", 0x20, FORMAT_RR));
	hashmap_insert(ihm, "out", create_instruction("out", 0x20, FORMAT_RR));
	hashmap_insert(ihm, "clr", create_instruction("clr", 0x20, FORMAT_R));
	hashmap_insert(ihm, "ld", create_instruction("ld", 0x20, FORMAT_RL));
	hashmap_insert(ihm, "push", create_instruction("push", 0x20, FORMAT_R));
	hashmap_insert(ihm, "pop", create_instruction("pop", 0x20, FORMAT_R));
	hashmap_insert(ihm, "halt", create_instruction("halt", 0x20, FORMAT_NONE));

	return ihm;
}