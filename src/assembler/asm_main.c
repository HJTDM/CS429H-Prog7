#include <stdio.h>
#include <stdlib.h>

#include "assembler/assembler.h"

int main(int argc, char* argv[]){
	if(argc != 3){
		fprintf(stderr, "Please include an input file and output file as arguments\n");
		exit(1);
	}

	generate_object_file(argv[1], argv[2]);
}