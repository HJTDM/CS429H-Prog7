# CS429H-Prog7

## Student Information

- Name: Hantao Jin
- EID: hj8639

## Compiling and Running the Program

### Using the Shell Script

To compile and run the program, clone the repository and run the following commands:
```bash
./build.sh
./hw5 [inputFile] [outputFile]  # Replace [inputFile] and [outputFile] with the path to the input and output file
```

### Using the Makefile

Alternatively, use the makefile to compile (with debug flags) and run the program with the following commands:
```bash
make main
make runmain IN=[inputFile] OUT=[outputFile]  # Replace [inputFile] and [outputFile] with the path to the input and output file
```

## Compiling and Running Tests

### Using the Makefile

To compile and run the tests, clone the repository and run the following command:
```bash
make tests
```

The unit tests involve black-box testing on the HashMap, instruction, and utility methods. A [custom testing framework](include/test_framework.h) is used to allow for assertions (true/false, equals/not equals, etc.).# CS429H-Prog7
