#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "test_framework.h"
#include "instruction.h"
#include "label.h"
#include "hashmap.h"
#include "utils.h"

int tests_run = 0;
int tests_failed = 0;

// Test that label hashmap methods work as intended
TEST_CASE(test_label_hashmap){
	HashMap* lhm = create_hashmap();

	hashmap_insert(lhm, ":L1", create_label(":L1", 0x1010));

	Label* l1 = (Label*) hashmap_get(lhm, ":L1");
	ASSERT_TRUE(strcmp(l1->name, ":L1") == 0);
	ASSERT_EQUALS(l1->address, 0x1010);

	ASSERT_NULL(hashmap_get(lhm, ":L2"));

	destroy_hashmap(lhm, destroy_label);
}

// Test that instruction hashmap methods work as intended
TEST_CASE(test_instr_hashmap){
	HashMap* ihm = create_hashmap();

	hashmap_insert(ihm, "addi", create_instruction("addi", 0x19, FORMAT_RL));

	Instruction* instr = (Instruction*) hashmap_get(ihm, "addi");
	ASSERT_TRUE(strcmp(instr->name, "addi") == 0);
	ASSERT_EQUALS(instr->opcode, 0x19);
	ASSERT_EQUALS(instr->format, FORMAT_RL);

	ASSERT_NULL(hashmap_get(ihm, "mov"));

	destroy_hashmap(ihm, destroy_instruction);
}

// Test that process directive returns the correct character based on the directive
TEST_CASE(test_process_directive){
	char str1[] = ".code";
	ASSERT_EQUALS(process_directive(str1), 'C');

	char str2[] = ".data";
	ASSERT_EQUALS(process_directive(str2), 'D');

	char str3[] = "code";
	ASSERT_EQUALS(process_directive(str3), 'N');

	char str4[] = ".invalid";
	ASSERT_EQUALS(process_directive(str4), 'N');
}

// Test that encode_instruction correctly encodes into 32-bit intgers
TEST_CASE(test_encode_instruction){
	uint32_t add = encode_instruction(0x18, 5, 7, 9, 0);
	ASSERT_EQUALS(add, 0xc14e9000);

	uint32_t addi = encode_instruction(0x19, 25, 0, 0, 568);
	ASSERT_EQUALS(addi, 0xce400238);

	uint32_t not = encode_instruction(0x3, 27, 31, 0, 0);
	ASSERT_EQUALS(not, 0x1efe0000);

	uint32_t br = encode_instruction(0x8, 13, 0, 0, 0);
	ASSERT_EQUALS(br, 0x43400000);

	uint32_t brr = encode_instruction(0xa, 0, 0, 0, -1035);
	ASSERT_EQUALS(brr, 0x50000bf5);

	uint32_t ret = encode_instruction(0xd, 0, 0, 0, 0);
	ASSERT_EQUALS(ret, 0x68000000);

	uint32_t priv = encode_instruction(0xf, 26, 4, 0, 3);
	ASSERT_EQUALS(priv, 0x7e880003);

	uint32_t mov = encode_instruction(0x10, 6, 24, 0, -231);
	ASSERT_EQUALS(mov, 0x81b00f19);
}

// Test that change_in_address returns the correct value for instructions/macros/data
TEST_CASE(test_change_in_address){
	char str1[] = "\t56562326\n";
	ASSERT_EQUALS(change_in_address(str1), 8);

	char str2[] = "\tadd r0, r1, r2\n";
	ASSERT_EQUALS(change_in_address(str2), 4);

	char str3[] = "\tld r0, 100\n";
	ASSERT_EQUALS(change_in_address(str3), 48);

	char str4[] = "\tpush r0\n";
	ASSERT_EQUALS(change_in_address(str4), 8);
	
	char str5[] = "\tpop r0\n";
	ASSERT_EQUALS(change_in_address(str5), 8);

	char str6[] = "";
	ASSERT_EQUALS(change_in_address(str6), -1);
}

// Test is_data can identify valid numerical strings
TEST_CASE(test_is_data){
	char str1[] = "\t563234234\n";
	ASSERT_TRUE(is_data(str1));

	char str2[] = "\t223423423   ";
	ASSERT_TRUE(is_data(str2));

	char str3[] = "adfasdfaasdf ";
	ASSERT_FALSE(is_data(str3));

	char str4[] = "\t2234  23423   ";
	ASSERT_FALSE(is_data(str4));

	char str5[] = "\t18446744073709551616";
	ASSERT_FALSE(is_data(str5));

	char str6[] = "\t-123456";
	ASSERT_FALSE(is_data(str6));
}

// Test is_empty can identify empty/whitespace strings
TEST_CASE(test_is_empty){
    char str1[] = "                  ";
	ASSERT_TRUE(is_empty(str1));

	char str2[] = " \t\t\n  ";
	ASSERT_TRUE(is_empty(str2));

	char str3[] = "  \t  334   ";
	ASSERT_FALSE(is_empty(str3));
}

// Test trim works as expected on leading and trailing whitespace
TEST_CASE(test_trim){
	char str1[] = "   2    ";
	trim(str1);
	ASSERT_TRUE(strcmp(str1, "2") == 0);

	char str2[] = "1    ";
	trim(str2);
	ASSERT_TRUE(strcmp(str2, "1") == 0);

	char str3[] = "   3";
	trim(str3);
	ASSERT_TRUE(strcmp(str3, "3") == 0);

	// All whitespace should result in empty string
	char str4[] = "       ";
	trim(str4);
	ASSERT_TRUE(strcmp(str4, "") == 0);
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
}

// Test is_valid_register can identify valid registers
TEST_CASE(test_is_valid_register){
    char str1[] = "0";
    ASSERT_TRUE(is_valid_register(str1));

    char str2[] = "12";
    ASSERT_TRUE(is_valid_register(str2));

    char str3[] = "-1";
    ASSERT_FALSE(is_valid_register(str3));

    char str4[] = "32";
    ASSERT_FALSE(is_valid_register(str4));
}

// Test is_valid_literal can identify valid literals
TEST_CASE(test_is_valid_literal){
	HashMap* lhm = create_hashmap();
	hashmap_insert(lhm, ":L1", create_label(":L1", 0x1000));
	hashmap_insert(lhm, ":L2", create_label(":L2", 0x1004));
	hashmap_insert(lhm, ":L3", create_label(":L3", 0x1008));

	char str1[] = "563";
	ASSERT_TRUE(is_valid_literal(lhm, "add", str1));

	char str2[] = "-563";
	ASSERT_TRUE(is_valid_literal(lhm, "brr", str2));
	
	char str3[] = "4096";
	ASSERT_FALSE(is_valid_literal(lhm, "add", str3));

	char str4[] = ":L1";
	ASSERT_TRUE(is_valid_literal(lhm, "ld", str4));

	char str5[] = ":L4";
	ASSERT_FALSE(is_valid_literal(lhm, "ld", str5));
}

// Test is_valid_labels can identify valid labels
TEST_CASE(test_is_valid_label){
	char str1[] = ":L1";
	ASSERT_TRUE(is_valid_label(str1));

	char str2[] = ":JVGKHVJHKBHK";
	ASSERT_TRUE(is_valid_label(str2));

	char str3[] = ":L1  L2";
	ASSERT_FALSE(is_valid_label(str3));
}

int main() {
	printf("HashMap tests:\n");
	RUN_TEST(test_label_hashmap);
	RUN_TEST(test_instr_hashmap);
	printf("\n");

	printf("Instruction tests:\n");
	RUN_TEST(test_process_directive);
	RUN_TEST(test_encode_instruction);
	RUN_TEST(test_change_in_address);
	printf("\n");

	printf("Utils tests:\n");
	RUN_TEST(test_is_data);
	RUN_TEST(test_is_empty);
	RUN_TEST(test_trim);
	RUN_TEST(test_is_uint64);
	RUN_TEST(test_is_valid_register);
	RUN_TEST(test_is_valid_literal);
	RUN_TEST(test_is_valid_label);
	printf("\n");

    printf("Tests run: %d, Passed: %d, Failed: %d\n", tests_run, (tests_run - tests_failed), tests_failed);
    return tests_failed;
}