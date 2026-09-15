/**
 * @file        :   shell_sort_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Comprehensive unit test suite for the Shell Sort algorithm.
 * @details     :   This module uses the Unity Test Framework to validate various 
 * edge cases, logic correctness, and standard sorting scenarios
 * specific to the Shell Sort algorithm.
 * Compile:
 * clang ./tests/darwin/sorting/shell_sort_unit_tests.c ./src/darwin/sorting/shell-sort/shell_sort.c ./src/unity.c -Iinclude -o ./bin/shell_sort_unit_test
 * @version     :   1.0
 * @date        :   2026-04-18
 */
#include <stdio.h>
#include <limits.h>

// Include header files.
#include "unity.h"
#include "darwin/sorting/shell_sort.h"

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
    int array[] = {64, 25, 12, 22, 11};
    int expected[] = {11, 12, 22, 25, 64};
    size_t size = sizeof(array) / sizeof(array[0]);

    // Act.
    shell_sort(array, size);

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
    shell_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting of an array sorted in descending order.
 */
void test_sort_reverse_sorted_array(void)
{
    // Arrange.
    int array[] = {5, 4, 3, 2, 1};
    int expected[] = {1, 2, 3, 4, 5};
    size_t size = 5;

    // Act.
    shell_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting of an array where all elements are identical.
 */
void test_sort_identical_elements(void)
{
    // Arrange.
    int array[] = {7, 7, 7, 7};
    int expected[] = {7, 7, 7, 7};
    size_t size = 4;

    // Act.
    shell_sort(array, size);

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
    shell_sort(array, size);

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

    // Act & Assert.
    shell_sort(array, size);
    
    TEST_ASSERT_NULL(array);
}

/**
 * @brief Tests sorting of an array containing only two elements.
 */
void test_sort_two_elements(void)
{
    // Arrange.
    int array[] = {10, -5};
    int expected[] = {-5, 10};
    size_t size = 2;

    // Act.
    shell_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting when the maximum and minimum integer values are present.
 */
void test_sort_extreme_values(void)
{
    // Arrange.
    int array[] = {INT_MAX, 0, INT_MIN};
    int expected[] = {INT_MIN, 0, INT_MAX};
    size_t size = 3;

    // Act.
    shell_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting of an array with many duplicate values.
 */
void test_sort_large_duplicates(void)
{
    // Arrange.
    int array[] = {3, 1, 2, 1, 3, 2, 1};
    int expected[] = {1, 1, 1, 2, 2, 3, 3};
    size_t size = 7;

    // Act.
    shell_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting of an array with both positive and negative integers.
 */
void test_sort_mixed_signs(void)
{
    // Arrange.
    int array[] = {-10, 5, -2, 0, 8};
    int expected[] = {-10, -2, 0, 5, 8};
    size_t size = 5;

    // Act.
    shell_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting of an array containing large gaps between values.
 */
void test_sort_large_gaps(void)
{
    // Arrange.
    int array[] = {1000000, 1, -1000000, 50};
    int expected[] = {-1000000, 1, 50, 1000000};
    size_t size = 4;

    // Act.
    shell_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting when elements are already in a zig-zag pattern.
 */
void test_sort_zigzag_pattern(void)
{
    // Arrange.
    int array[] = {1, 10, 2, 9, 3, 8};
    int expected[] = {1, 2, 3, 8, 9, 10};
    size_t size = 6;

    // Act.
    shell_sort(array, size);

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

    // Act & Assert.
    shell_sort(array, size);

    TEST_ASSERT_NULL(array);
}

/**
 * @brief Tests sorting of an array where the minimum is at the very end.
 */
void test_sort_min_at_end(void)
{
    // Arrange.
    int array[] = {10, 20, 30, 40, 5};
    int expected[] = {5, 10, 20, 30, 40};
    size_t size = 5;

    // Act.
    shell_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting of an array where the maximum is at the very beginning.
 */
void test_sort_max_at_start(void)
{
    // Arrange.
    int array[] = {100, 10, 20, 30, 40};
    int expected[] = {10, 20, 30, 40, 100};
    size_t size = 5;

    // Act.
    shell_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
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
    RUN_TEST(test_sort_extreme_values);
    RUN_TEST(test_sort_large_duplicates);
    RUN_TEST(test_sort_mixed_signs);
    RUN_TEST(test_sort_large_gaps);
    RUN_TEST(test_sort_zigzag_pattern);
    RUN_TEST(test_sort_null_pointer);
    RUN_TEST(test_sort_min_at_end);
    RUN_TEST(test_sort_max_at_start);

    // Destroy the session.
    return UNITY_END();
}