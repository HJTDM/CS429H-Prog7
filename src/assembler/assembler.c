#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler/assembler.h"
#include "assembler/instruction.h"
#include "assembler/label.h"
#include "assembler/stack.h"
#include "assembler/utils.h"

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

void generate_object_file(const char* inputFile, const char* outputFile){
	HashMap* lhm = create_hashmap();
	HashMap* ihm = create_instr_hashmap();
	TinkerFileHeader* tfh = create_tinker_file_header();

	FILE *in = NULL, *out = NULL;
	check_files(inputFile, outputFile, &in, &out);

	// Populate labels from the input file
	if(populate_labels(in, lhm, tfh) != 0){
		fprintf(stderr, "Error: failed to populate LabelHashMap\n");
		destroy_hashmap(lhm, destroy_label);
		destroy_hashmap(ihm, destroy_instruction);
		free(tfh);
		fclose(in);
		fclose(out);
		remove(outputFile);
		exit(1);
	}

	rewind(in);
	// Resolve the program by writing the object file
	if(resolve_program(in, out, lhm, ihm, tfh) != 0){
		fprintf(stderr, "Error: failed to create object file\n");
		destroy_hashmap(lhm, destroy_label);
		destroy_hashmap(ihm, destroy_instruction);
		free(tfh);
		fclose(in);
		fclose(out);
		remove(outputFile);
		exit(1);
	}

	destroy_hashmap(lhm, destroy_label);
	destroy_hashmap(ihm, destroy_instruction);
	free(tfh);
	fclose(in);
	fclose(out);
}

void check_files(const char* inputFile, const char* outputFile, FILE** in, FILE** out){
	*in = fopen(inputFile, "r");

	// Check if the input file was opened successfully
	if(*in == NULL){
		fprintf(stderr, "Error: could not open the file %s for reading\n", inputFile);
		exit(1);
	}

	*out = fopen(outputFile, "wb");

	// Check if the output file was opened or created successfully
	if(*out == NULL){
		fprintf(stderr, "Error: could not open or create the file %s for writing\n", outputFile);
		exit(1);
	}
}

int populate_labels(FILE* in, HashMap* lhm, TinkerFileHeader* tfh){
	char line[256];
	uint64_t codeAddress = INIT_CODE_ADDR;
	uint64_t dataAddress = INIT_DATA_ADDR;
	char currentDirective = 'N';

	Stack* labelStack = create_stack();

	while(fgets(line, sizeof(line), in)){
		// Skip comments and empty lines
		if(line[0] == ';' || is_empty(line)){
			continue;
		}
		// Process directives
		else if(line[0] == '.'){
			if((currentDirective = process_directive(line)) == 'N'){
				fprintf(stderr, "Error: invalid directive format");
				return -1;
			}
		}
		// Process label lines
		else if(line[0] == ':'){
			trim(line);
			char* label = strdup(line);

			if(!is_valid_label(label)){
				fprintf(stderr, "Error: invalid label format\n");
				return -1;
			}

			// Add label to label stack
			stack_push(labelStack, label);
		}
		// Process instruction lines
		else if(line[0] == '\t'){
			// Pop all labels from stack and add to hashmap
			while(!stack_is_empty(labelStack)){
				if(currentDirective == 'C'){
					char* label = stack_pop(labelStack);
					hashmap_insert(lhm, label, create_label(label, codeAddress));
				}
				else if(currentDirective == 'D') {
					char* label = stack_pop(labelStack);
					hashmap_insert(lhm, label, create_label(label, dataAddress));
				}
			}

			// Calculate new address based on directive type
			int8_t change = change_in_address(line);
			if(change == -1){
				fprintf(stderr, "Error: invalid instruction format\n");
				return -1;
			}

			if(currentDirective == 'C'){
				codeAddress += change;
			}
			else if(currentDirective == 'D') {
				dataAddress += change;
			}
		}
		// Handle invalid line format
		else{
			fprintf(stderr, "Error: invalid line format\n");
			return -1;
		}
	}

	// Pop the rest of the labels from the stack and add to hashmap
	while(!stack_is_empty(labelStack)){
		if(currentDirective == 'C'){
			char* label = stack_pop(labelStack);
			hashmap_insert(lhm, label, create_label(label, codeAddress));
		}
		else if(currentDirective == 'D') {
			char* label = stack_pop(labelStack);
			hashmap_insert(lhm, label, create_label(label, dataAddress));
		}
	}
	
	// Calculate the size of the code and data segments
	tfh->codeSize = codeAddress - INIT_CODE_ADDR;
	tfh->dataSize = dataAddress - INIT_DATA_ADDR;
	destroy_stack(labelStack);
	return 0;
}

int resolve_program(FILE* in, FILE* out, HashMap* lhm, HashMap* ihm, TinkerFileHeader* tfh){
	char currentDirective = 'N';
	bool hasCodeDirective = false;

	// Create data array to consolidate data separately from code
	uint64_t* dataBinary = (uint64_t*) malloc(sizeof(uint64_t) * (tfh->dataSize / 8));
	uint64_t dataCount = 0;

	// Write file header to output file
	fwrite(tfh, sizeof(TinkerFileHeader), 1, out);
	uint64_t currentLine = 1;

	char line[256];
	while(fgets(line, sizeof(line), in)){
		// Skip labels, comments, and empty lines
		if(line[0] == ':' || line[0] == ';' || is_empty(line)){
			continue;
		}
		// Process directive lines
		else if(line[0] == '.'){
			currentDirective = process_directive(line);
			hasCodeDirective = currentDirective == 'C' ? true : hasCodeDirective;
		}
		// Process data and instruction lines
		else if(line[0] == '\t'){
			if(is_data(line)){
				if(currentDirective != 'D'){
					fprintf(stderr, "Error: data must be under a .data directive\n");
					return -1;
				}

				trim(line);
				dataBinary[dataCount] = strtoull(line, NULL, 10);
				dataCount++;
			}
			else{
				if(currentDirective != 'C'){
					fprintf(stderr, "Error: instructions must be under a .code directive\n");
					return -1;
				}

				if(process_instruction(out, lhm, ihm, line) != 0){
					fprintf(stderr, "Error: failed to process instruction at line %lu\n", currentLine);
					return -1;
				}
			}
			
			currentLine++;
		}
	}

	// Ensure there is at least one .code directive
	if(!hasCodeDirective){
		fprintf(stderr, "Error: the program must have at least one .code directive\n");
		return -1;
	}

	// Write all data last to output file
	fwrite(dataBinary, sizeof(uint64_t), (tfh->dataSize / 8), out);
	free(dataBinary);
	return 0;
}