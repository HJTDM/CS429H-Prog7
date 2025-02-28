#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>

extern int tests_run;
extern int tests_failed;

/**
 * @brief Defines a test case function
 *
 * @param name the name of the test case function
 */
#define TEST_CASE(name) int name()

/**
 * @brief Runs a test case function
 *
 * @param name the name of the test case function to run
 */
#define RUN_TEST(name) do { \
	printf("    Running %s...", #name); \
	tests_run++; \
	if (name() == 0) { \
		printf(" Passed!\n"); \
	} \
} while (0)

/**
 * @brief Asserts that two values are equal
 *
 * If the values are not equal, prints an error message and increments the
 * tests_failed counter
 *
 * @param actual the actual value
 * @param expected the expected value
 * @return 1 if the values are equal, 0 otherwise
 */
#define ASSERT_EQUALS(actual, expected) do { \
	if ((actual) != (expected)) { \
		printf("\n        Test failed: %s != %s (line %d)\n", #actual, #expected, __LINE__); \
		tests_failed++; \
		return -1; \
	} \
} while (0)

/**
 * @brief Asserts that two values are not equal
 *
 * If the values are equal, prints an error message and increments the
 * tests_failed counter
 *
 * @param actual the actual value
 * @param expected the value that should not be equal to the actual value
 */
#define ASSERT_NOT_EQUALS(actual, expected) do { \
	if ((actual) == (expected)) { \
		printf("\n        Test failed: %s == %s (line %d)\n", #actual, #expected, __LINE__); \
		tests_failed++; \
		return -1; \
	} \
} while (0)

/**
 * @brief Asserts that a condition is true
 *
 * If the condition is false, prints an error message and increments the
 * tests_failed counter
 *
 * @param condition the condition to check
 */
#define ASSERT_TRUE(condition) do { \
	if (!(condition)) { \
		printf("\n        Test failed: %s is false (line %d)\n", #condition, __LINE__); \
		tests_failed++; \
		return -1; \
	} \
} while (0)

/**
 * @brief Asserts that a condition is false
 *
 * If the condition is true, prints an error message and increments the
 * tests_failed counter
 *
 * @param condition the condition to check
 */
#define ASSERT_FALSE(condition) do { \
	if (condition) { \
		printf("\n        Test failed: %s is true (line %d)\n", #condition, __LINE__); \
		tests_failed++; \
		return -1; \
	} \
} while (0)

/**
 * @brief Asserts that a value is null
 *
 * If the value is not null, prints an error message and increments the
 * tests_failed counter
 *
 * @param value the value to check
 */
#define ASSERT_NULL(value) do { \
	if ((value) != NULL) { \
		printf("\n        Test failed: %s is not null (line %d)\n", #value, __LINE__); \
		tests_failed++; \
		return -1; \
	} \
} while (0)

/**
 * @brief Asserts that a value is not null
 *
 * If the value is null, prints an error message and increments the
 * tests_failed counter
 *
 * @param value the value to check
 */
#define ASSERT_NOT_NULL(value) do { \
	if ((value) == NULL) { \
		printf("\n        Test failed: %s is null (line %d)\n", #value, __LINE__); \
		tests_failed++; \
		return -1; \
	} \
} while (0)

#endif