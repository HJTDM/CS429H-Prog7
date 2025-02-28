#include <stdio.h>
#include <stdlib.h>

#include "simulator/simulator.h"

int main(int argc, char* argv[]){
	// Check that there is one input file
	if(argc != 2){
		fprintf(stderr, "Invalid tinker filepath\n");
		exit(1);
	}

	simulate_program(argv[1]);

	return 0;
}