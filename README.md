# CS429H-Prog7

## Student Information

- Name: Hantao Jin
- EID: hj8639

## Compiling and Running the Program

### Using the Shell Script

To compile and run the program, clone the repository and run the following commands:
```bash
./build.sh
./hw7-asm [inputFile] [outputFile]  # Replace [inputFile] and [outputFile] with the path to the input and output file
./hw7-sim [inputFile] # Replace [inputFile] with the path to the input file
```

### Using the Makefile

Alternatively, use the makefile to compile (with debug flags) and run the program with the following commands:
```bash
make asm
make runasm IN=[inputFile] OUT=[outputFile]  # Replace [inputFile] and [outputFile] with the path to the input and output file
make sim
make runsim IN=[inputFile]  # Replace [inputFile] with the path to the input file
```

## Compiling and Running Tests

### Using the Makefile

To compile and run the tests, clone the repository and run the following command:
```bash
make asmtests
make simtests
```

A [custom testing framework](include/test_framework.h) is used to allow for assertions (true/false, equals/not equals, etc.).# CS429H-Prog7
