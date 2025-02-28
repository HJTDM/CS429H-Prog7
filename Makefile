CC = gcc

DEBUG_FLAGS = -Wall -Werror -O0 -g

ASM_SRC_DIR = src/assembler
ASM_SRC_FILES = $(wildcard $(ASM_SRC_DIR)/*.c)
ASM_SRC_FILES := $(filter-out $(ASM_SRC_DIR)/asm_main.c, $(ASM_SRC_FILES))

SIM_SRC_DIR = src/simulator
SIM_SRC_FILES = $(wildcard $(SIM_SRC_DIR)/*.c)
SIM_SRC_FILES := $(filter-out $(SIM_SRC_DIR)/sim_main.c, $(SIM_SRC_FILES))

INC_DIR = include
ASM_INC_FILES = $(wildcard $(ASM_INC_DIR)/assembler/*.h)
SIM_INC_FILES = $(wildcard $(ASM_INC_DIR)/simulator/*.h)

asm: $(ASM_SRC_FILES) $(ASM_INC_FILES)
	$(CC) $(DEBUG_FLAGS) -o hw7-asm src/assembler/asm_main.c $(ASM_SRC_FILES) -I $(INC_DIR)

sim: $(SIM_SRC_FILES) $(SIM_INC_FILES)
	$(CC) $(DEBUG_FLAGS) -o hw7-sim src/simulator/sim_main.c $(SIM_SRC_FILES) -I $(INC_DIR)

runasm: hw7-asm
	./hw7-asm $(IN) $(OUT)

runsim: hw7-sim
	./hw7-sim $(IN) $(OUT)

leakcheckasm: hw7-asm
	valgrind --leak-check=full ./hw7-asm $(IN) $(OUT)

leakchecksim: hw7-sim
	valgrind --leak-check=full ./hw7-sim $(IN) $(OUT)

asmtests: tests/assembler_tests.c $(ASM_SRC_FILES) $(ASM_INC_FILES)
	$(CC) $(DEBUG_FLAGS) -o assembler_tests tests/assembler_tests.c $(ASM_SRC_FILES) -I$(INC_DIR) && ./assembler_tests

simtests: tests/simulator_tests.c $(SIM_SRC_FILES) $(SIM_INC_FILES)
	$(CC) $(DEBUG_FLAGS) -o simulator_tests tests/simulator_tests.c $(SIM_SRC_FILES) -I$(INC_DIR) && ./simulator_tests

.PHONY: clean

clean:
	rm -f *.o hw7-asm hw7-sim