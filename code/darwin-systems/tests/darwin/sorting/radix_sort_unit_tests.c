/**
 * @file        :   radix_sort_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Comprehensive unit test suite for the Radix Sort algorithm.
 * @details     :   This module uses the Unity Test Framework to validate various 
 * edge cases, stability, and standard sorting scenarios.
 * Compile:
 * clang ./tests/darwin/sorting/radix_sort_unit_tests.c ./src/darwin/sorting/radix-sort/radix_sort.c ./src/unity.c -Iinclude -o ./bin/radix_sort_unit_test
 * @version     :   1.0
 * @date        :   2026-04-13
 */
#include <stdio.h>

#include "unity.h"
#include "darwin/sorting/radix_sort.h"

/**
 * @brief Sets up resources before each test.
 */
void setUp(void)
{
    ;;
}

/**
 * @brief Cleans up resources after each test.
 */
void tearDown(void)
{
    ;;
}

/**
 * @brief Tests sorting of a standard unsorted array of positive integers.
 */
void test_sort_standard_positive_array(void)
{
    // Arrange.
    int array[] = {170, 45, 75, 90, 802, 24, 2, 66};
    int expected[] = {2, 24, 45, 66, 75, 90, 170, 802};
    size_t size = sizeof(array) / sizeof(array[0]);

    // Act.
    radix_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting of an array that is already sorted.
 */
void test_sort_already_sorted_array(void)
{
    // Arrange.
    int array[] = {1, 2, 3, 4, 5};
    int expected[] = {1, 2, 3, 4, 5};
    size_t size = 5;

    // Act.
    radix_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting of an array sorted in descending order.
 */
void test_sort_reverse_sorted_array(void)
{
    // Arrange.
    int array[] = {500, 400, 300, 200, 100};
    int expected[] = {100, 200, 300, 400, 500};
    size_t size = 5;

    // Act.
    radix_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting of an array where all elements are identical.
 */
void test_sort_identical_elements(void)
{
    // Arrange.
    int array[] = {10, 10, 10, 10};
    int expected[] = {10, 10, 10, 10};
    size_t size = 4;

    // Act.
    radix_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting of an array with a single element.
 */
void test_sort_single_element(void)
{
    // Arrange.
    int array[] = {42};
    int expected[] = {42};
    size_t size = 1;

    // Act.
    radix_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests the behavior when an empty array (size 0) is provided.
 */
void test_sort_empty_array(void)
{
    // Arrange.
    int* array = NULL;
    size_t size = 0;

    // Act.
    radix_sort(array, size);

    // Assert.
    TEST_ASSERT_NULL(array);
}

/**
 * @brief Tests sorting of an array containing only two elements.
 */
void test_sort_two_elements(void)
{
    // Arrange.
    int array[] = {99, 11};
    int expected[] = {11, 99};
    size_t size = 2;

    // Act.
    radix_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting when the maximum integer value is present.
 */
void test_sort_maximum_int_value(void)
{
    // Arrange.
    int array[] = {0, INT_MAX, 500};
    int expected[] = {0, 500, INT_MAX};
    size_t size = 3;

    // Act.
    radix_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting of an array with a large number of duplicate values.
 */
void test_sort_large_duplicates(void)
{
    // Arrange.
    int array[] = {5, 1, 5, 2, 5, 3};
    int expected[] = {1, 2, 3, 5, 5, 5};
    size_t size = 6;

    // Act.
    radix_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting of values with varying numbers of digits.
 */
void test_sort_varying_digit_counts(void)
{
    // Arrange.
    int array[] = {1000, 1, 100, 10};
    int expected[] = {1, 10, 100, 1000};
    size_t size = 4;

    // Act.
    radix_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting of an array containing zero as an element.
 */
void test_sort_with_zero(void)
{
    // Arrange.
    int array[] = {10, 0, 5, 0};
    int expected[] = {0, 0, 5, 10};
    size_t size = 4;

    // Act.
    radix_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting performance with a larger sequence of digits.
 */
void test_sort_large_range(void)
{
    // Arrange.
    int array[] = {9999, 1111, 5555, 2222};
    int expected[] = {1111, 2222, 5555, 9999};
    size_t size = 4;

    // Act.
    radix_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests the algorithm's stability with pre-sorted sub-keys.
 */
void test_sort_stability_check(void)
{
    // Arrange.
    int array[] = {20, 10, 21, 11};
    int expected[] = {10, 11, 20, 21};
    size_t size = 4;

    // Act.
    radix_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting of an array where elements are powers of ten.
 */
void test_sort_powers_of_ten(void)
{
    // Arrange.
    int array[] = {100, 1, 1000, 10};
    int expected[] = {1, 10, 100, 1000};
    size_t size = 4;

    // Act.
    radix_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests handling of a null pointer as input.
 */
void test_sort_null_pointer(void)
{
    // Arrange.
    int* array = NULL;
    size_t size = 10;

    // Act.
    radix_sort(array, size);

    // Assert.
    TEST_ASSERT_NULL(array);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_sort_standard_positive_array);
    RUN_TEST(test_sort_already_sorted_array);
    RUN_TEST(test_sort_reverse_sorted_array);
    RUN_TEST(test_sort_identical_elements);
    RUN_TEST(test_sort_single_element);
    RUN_TEST(test_sort_empty_array);
    RUN_TEST(test_sort_two_elements);
    RUN_TEST(test_sort_maximum_int_value);
    RUN_TEST(test_sort_large_duplicates);
    RUN_TEST(test_sort_varying_digit_counts);
    RUN_TEST(test_sort_with_zero);
    RUN_TEST(test_sort_large_range);
    RUN_TEST(test_sort_stability_check);
    RUN_TEST(test_sort_powers_of_ten);
    RUN_TEST(test_sort_null_pointer);

    // Destroy the session.
    return UNITY_END();
}