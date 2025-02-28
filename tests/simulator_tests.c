#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "test_framework.h"
#include "simulator/simulator.h"
#include "simulator/utils.h"

int tests_run = 0;
int tests_failed = 0;

TEST_CASE(test_add){
	Processor* processor = create_processor();
	processor->registers[0] = 3;
	processor->registers[1] = 4;

	add(processor, 2, 0, 1, 0);
	ASSERT_EQUALS(processor->registers[2], 7);
	return 0;
}

TEST_CASE(test_addi){
	Processor* processor = create_processor();
	processor->registers[0] = 3;

	addi(processor, 0, 0, 0, 56);
	ASSERT_EQUALS(processor->registers[0], 59);
	return 0;
}

TEST_CASE(test_sub){
	Processor* processor = create_processor();
	processor->registers[0] = 3;
	processor->registers[1] = 4;

	sub(processor, 2, 0, 1, 0);
	ASSERT_EQUALS(processor->registers[2], -1);
	return 0;
}

TEST_CASE(test_subi){
	Processor* processor = create_processor();
	processor->registers[0] = 100;

	subi(processor, 0, 0, 0, 75);
	ASSERT_EQUALS(processor->registers[0], 25);
	return 0;
}

TEST_CASE(test_mul){
	Processor* processor = create_processor();
	processor->registers[0] = 25;
	processor->registers[1] = 4;

	mul(processor, 2, 0, 1, 0);
	ASSERT_EQUALS(processor->registers[2], 100);
	return 0;
}

TEST_CASE(test_div){
	Processor* processor = create_processor();
	processor->registers[0] = 24;
	processor->registers[1] = 4;

	divi(processor, 2, 0, 1, 0);
	ASSERT_EQUALS(processor->registers[2], 6);
	
	// Division by zero
	ASSERT_EQUALS(divi(processor, 2, 0, 3, 0), -1);
	return 0;
}

TEST_CASE(test_and){
	Processor* processor = create_processor();
	processor->registers[0] = 2;
	processor->registers[1] = 4;

	and(processor, 2, 0, 1, 0);
	ASSERT_EQUALS(processor->registers[2], 0);
	return 0;
}

TEST_CASE(test_or){
	Processor* processor = create_processor();
	processor->registers[0] = 5;
	processor->registers[1] = 3;

	or(processor, 2, 0, 1, 0);
	ASSERT_EQUALS(processor->registers[2], 7);
	return 0;
}

TEST_CASE(test_xor){
	Processor* processor = create_processor();
	processor->registers[0] = 2;
	processor->registers[1] = 4;

	xor(processor, 2, 0, 1, 0);
	ASSERT_EQUALS(processor->registers[2], 6);
	return 0;
}

TEST_CASE(test_not){
	Processor* processor = create_processor();
	processor->registers[0] = 25;

	not(processor, 0, 0, 0, 0);
	ASSERT_EQUALS(processor->registers[0], 18446744073709551590ULL);
	return 0;
}

TEST_CASE(test_shftr){
	Processor* processor = create_processor();
	processor->registers[0] = 24;
	processor->registers[1] = 2;

	shftr(processor, 2, 0, 1, 0);
	ASSERT_EQUALS(processor->registers[2], 6);
	return 0;
}

TEST_CASE(test_shftri){
	Processor* processor = create_processor();
	processor->registers[0] = 48;

	shftri(processor, 0, 0, 0, 3);
	ASSERT_EQUALS(processor->registers[0], 6);
	return 0;
}

TEST_CASE(test_shftl){
	Processor* processor = create_processor();
	processor->registers[0] = 24;
	processor->registers[1] = 2;

	shftl(processor, 2, 0, 1, 0);
	ASSERT_EQUALS(processor->registers[2], 96);
	return 0;
}

TEST_CASE(test_shftli){
	Processor* processor = create_processor();
	processor->registers[0] = 48;

	shftli(processor, 0, 0, 0, 3);
	ASSERT_EQUALS(processor->registers[0], 384);
	return 0;
}

TEST_CASE(test_br){
	Processor* processor = create_processor();
	processor->registers[0] = 0x1500;

	br(processor, 0, 0, 0, 0);
	processor->pc += 4;
	ASSERT_EQUALS(processor->pc, 0x1500);
	return 0;
}

TEST_CASE(test_brr){
	Processor* processor = create_processor();
	processor->registers[0] = 0x1500;

	brr(processor, 0, 0, 0, 0);
	processor->pc += 4;
	ASSERT_EQUALS(processor->pc, 0x3500);
	return 0;
}

TEST_CASE(test_brrL){
	Processor* processor = create_processor();

	// Positive literal
	brrL(processor, 0, 0, 0, 0x100);
	processor->pc += 4;
	ASSERT_EQUALS(processor->pc, 0x2100);

	// Negative literal
	brrL(processor, 0, 0, 0, 0x09A0);
	processor->pc += 4;
	ASSERT_EQUALS(processor->pc, 0x1AA0);
	return 0;
}

TEST_CASE(test_brnz){
	Processor* processor = create_processor();
	processor->registers[0] = 0x1589;

	// Source register is 0
	brnz(processor, 0, 1, 0, 0);
	processor->pc += 4;
	ASSERT_EQUALS(processor->pc, 0x2004);

	// Source register is not 0
	brnz(processor, 0, 0, 0, 0);
	processor->pc += 4;
	ASSERT_EQUALS(processor->pc, 0x1589);
	return 0;
}

TEST_CASE(test_call){
	Processor* processor = create_processor();
	processor->registers[0] = 0x1589;

	call(processor, 0, 0, 0, 0);

	// Check address on stack
	uint64_t address;
	memcpy(&address, &(processor->memory[processor->registers[31] - 8]), sizeof(address));
	ASSERT_EQUALS(address, 0x2004);

	// Check current program counter
	processor->pc += 4;
	ASSERT_EQUALS(processor->pc, 0x1589);
	return 0;
}

TEST_CASE(test_return){
	Processor* processor = create_processor();
	processor->registers[0] = 0x1589;

	call(processor, 0, 0, 0, 0);
	ret(processor, 0, 0, 0, 0);

	// Check current program counter
	processor->pc += 4;
	ASSERT_EQUALS(processor->pc, 0x2004);
	return 0;
}

TEST_CASE(test_brgt){
	Processor* processor = create_processor();
	processor->registers[0] = 2;
	processor->registers[1] = 3;
	processor->registers[2] = 0x8956;

	// Not greater
	brgt(processor, 2, 0, 1, 0);
	processor->pc += 4;
	ASSERT_EQUALS(processor->pc, 0x2004);

	// Is greater
	brgt(processor, 2, 1, 0, 0);
	processor->pc += 4;
	ASSERT_EQUALS(processor->pc, 0x8956);
	return 0;
}

TEST_CASE(test_priv){
	Processor* processor = create_processor();

	ASSERT_EQUALS(priv(processor, 0, 0, 0, 0), 1);

	priv(processor, 0, 0, 0, 1);
	ASSERT_EQUALS(processor->mode, SUPERVISOR_MODE);

	priv(processor, 0, 0, 0, 2);
	ASSERT_EQUALS(processor->mode, USER_MODE);

	return 0;
}

TEST_CASE(test_movRRL){
	Processor* processor = create_processor();
	processor->registers[1] = 0x1562;

	uint64_t data = 0x156789456;
	memcpy(&(processor->memory[processor->registers[1] + 0x15]), &data, sizeof(data));

	movRRL(processor, 0, 1, 0, 0x15);
	ASSERT_EQUALS(processor->registers[0], 0x156789456);

	uint64_t data2 = 0x45abc56df;
	memcpy(&(processor->memory[processor->registers[1] - 0x15]), &data2, sizeof(data2));

	movRRL(processor, 0, 1, 0, -0x15);
	ASSERT_EQUALS(processor->registers[0], 0x45abc56df);
	return 0;
}

TEST_CASE(test_movRR){
	Processor* processor = create_processor();
	processor->registers[1] = 0xabcdef;

	movRR(processor, 0, 1, 0, 0);
	ASSERT_EQUALS(processor->registers[0], 0xabcdef);
	return 0;
}

TEST_CASE(test_movRL){
	Processor* processor = create_processor();

	movRL(processor, 0, 0, 0, 0x9AC);
	ASSERT_EQUALS(processor->registers[0], 0x9AC);
	return 0;
}

TEST_CASE(test_movRLR){
	Processor* processor = create_processor();
	processor->registers[0] = 0x5000;
	processor->registers[1] = 0x156245678;

	movRLR(processor, 0, 1, 0, 0x15);

	uint64_t data;
	memcpy(&data, &(processor->memory[processor->registers[0] + 0x15]), sizeof(data));
	ASSERT_EQUALS(data, 0x156245678);

	processor->registers[1] = 0x4578965;
	movRLR(processor, 0, 1, 0, -0x15);

	uint64_t data2;
	memcpy(&data2, &(processor->memory[processor->registers[0] - 0x15]), sizeof(data2));
	ASSERT_EQUALS(data2, 0x4578965);
	return 0;
}

TEST_CASE(test_addf){
	Processor* processor = create_processor();
	processor->registers[0] = 0x400C000000000000; // 3.50 in IEEE 754 double precision
	processor->registers[1] = 0x4013D70A3D70A3D7; // 4.96 in IEEE 754 double precision

	addf(processor, 2, 0, 1, 0);

	double d;
	memcpy(&d, &processor->registers[2], sizeof(d));
	ASSERT_EQUALS(d, 8.46);
	return 0;
}

TEST_CASE(test_subf){
	Processor* processor = create_processor();
	processor->registers[0] = 0x400C000000000000; // 3.50 in IEEE 754 double precision
	processor->registers[1] = 0x4013D70A3D70A3D7; // 4.96 in IEEE 754 double precision

	subf(processor, 2, 0, 1, 0);

	double d;
	memcpy(&d, &processor->registers[2], sizeof(d));
	ASSERT_EQUALS(d, -1.46);
	return 0;
}

TEST_CASE(test_mulf){
	Processor* processor = create_processor();
	processor->registers[0] = 0x400C000000000000; // 3.50 in IEEE 754 double precision
	processor->registers[1] = 0x4013D70A3D70A3D7; // 4.96 in IEEE 754 double precision

	mulf(processor, 2, 0, 1, 0);

	double d;
	memcpy(&d, &processor->registers[2], sizeof(d));
	ASSERT_EQUALS(d, 17.36);
	return 0;
}

TEST_CASE(test_divf){
	Processor* processor = create_processor();
	processor->registers[0] = 0x4014000000000000; // 5 in IEEE 754 double precision
	processor->registers[1] = 0x4000000000000000; // 2 in IEEE 754 double precision

	divf(processor, 2, 0, 1, 0);

	double d;
	memcpy(&d, &processor->registers[2], sizeof(d));
	ASSERT_EQUALS(d, 2.5);
	return 0;
}

// Test is_uint64 checks that a string is an unsigned 64-bit integer
TEST_CASE(test_is_uint64){
    char str1[] = "0";
    ASSERT_TRUE(is_uint64(str1));

    char str2[] = "123456789";
    ASSERT_TRUE(is_uint64(str2));

    char str3[] = "18446744073709551615";
    ASSERT_TRUE(is_uint64(str3));

    char str4[] = "18446744073709551616";
    ASSERT_FALSE(is_uint64(str4));

    char str5[] = "-1";
    ASSERT_FALSE(is_uint64(str5));

	char str6[] = "asdfasdfasdfasdf";
    ASSERT_FALSE(is_uint64(str6));
	return 0;
}

int main() {
	printf("Simulator tests:\n");
	RUN_TEST(test_add);
	RUN_TEST(test_addi);
	RUN_TEST(test_sub);
	RUN_TEST(test_subi);
	RUN_TEST(test_mul);
	RUN_TEST(test_div);
	RUN_TEST(test_and);
	RUN_TEST(test_or);
	RUN_TEST(test_xor);
	RUN_TEST(test_not);
	RUN_TEST(test_shftr);
	RUN_TEST(test_shftri);
	RUN_TEST(test_shftl);
	RUN_TEST(test_shftli);
	RUN_TEST(test_br);
	RUN_TEST(test_brr);
	RUN_TEST(test_brrL);
	RUN_TEST(test_brnz);
	RUN_TEST(test_call);
	RUN_TEST(test_return);
	RUN_TEST(test_brgt);
	RUN_TEST(test_priv);
	RUN_TEST(test_movRRL);
	RUN_TEST(test_movRR);
	RUN_TEST(test_movRL);
	RUN_TEST(test_movRLR);
	RUN_TEST(test_addf);
	RUN_TEST(test_subf);
	RUN_TEST(test_mulf);
	RUN_TEST(test_divf);
	printf("\n");
	
	printf("Utils tests:\n");
	RUN_TEST(test_is_uint64);
	printf("\n");

    printf("Tests run: %d, Passed: %d, Failed: %d\n", tests_run, (tests_run - tests_failed), tests_failed);
    return tests_failed;
}