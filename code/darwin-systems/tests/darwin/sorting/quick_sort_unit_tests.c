/**
 * @file        :   quick_sort_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   This is a quick sort.
 * @details     :   Quick Sort is a divide-and-conquer algorithm that partitions 
 * arrays around a pivot. These tests verify partitioning 
 * logic, recursive depth safety, and edge case handling.
 * Compile:
 * clang ./tests/darwin/sorting/quick_sort_unit_tests.c ./src/darwin/sorting/quick-sort/quick_sort.c ./src/unity.c -Iinclude -o ./bin/quick_sort_unit_test
 * @version     :   1.0
 * @date        :   2026-04-13
 */
#include <stdio.h>
#include <limits.h>

#include "unity.h"
#include "darwin/sorting/quick_sort.h"

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
 * @brief Tests sorting of a standard unsorted array.
 */
void test_quick_sort_should_sort_standard_unsorted_array(void)
{
    // Arrange.
    int unsorted_numbers[] = {10, 2, 8, 6, 7, 3};
    int expected[] = {2, 3, 6, 7, 8, 10};

    size_t size = sizeof(unsorted_numbers) / sizeof(unsorted_numbers[0]);

    // Act.
    quick_sort(unsorted_numbers, 0, size - 1);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, unsorted_numbers, size);
}

/**
 * @brief Tests an array that is already sorted.
 */
void test_quick_sort_should_maintain_already_sorted_array(void)
{
    // Arrange.
    int sorted_numbers[] = {1, 2, 3, 4, 5};
    int expected[] = {1, 2, 3, 4, 5};

    size_t size = 5;

    // Act.
    quick_sort(sorted_numbers, 0, size - 1);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, sorted_numbers, size);
}

/**
 * @brief Tests an array sorted in reverse order.
 */
void test_quick_sort_should_sort_reverse_sorted_array(void)
{
    // Arrange.
    int reverse_numbers[] = {9, 7, 5, 3, 1};
    int expected[] = {1, 3, 5, 7, 9};

    size_t size = 5;

    // Act.
    quick_sort(reverse_numbers, 0, size - 1);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, reverse_numbers, size);
}

/**
 * @brief Tests an array containing multiple duplicate values.
 */
void test_quick_sort_should_handle_multiple_duplicates(void)
{
    // Arrange.
    int duplicate_numbers[] = {4, 1, 4, 2, 1, 4};
    int expected[] = {1, 1, 2, 4, 4, 4};
    
    size_t size = 6;

    // Act.
    quick_sort(duplicate_numbers, 0, size - 1);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, duplicate_numbers, size);
}

/**
 * @brief Tests an array where all elements are identical.
 */
void test_quick_sort_should_handle_all_identical_elements(void)
{
    // Arrange.
    int identical_numbers[] = {5, 5, 5, 5};
    int expected[] = {5, 5, 5, 5};

    size_t size = 4;

    // Act.
    quick_sort(identical_numbers, 0, size - 1);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, identical_numbers, size);
}

/**
 * @brief Tests handling of a NULL pointer safely.
 */
void test_quick_sort_should_fail_gracefully_on_null_pointer(void)
{
    // Arrange.
    int* null_array = NULL;

    // Act.
    quick_sort(null_array, 0, 10);

    // Assert.
    TEST_ASSERT_NULL(null_array);
}

/**
 * @brief Tests handling of an empty array (size 0).
 */
void test_quick_sort_should_handle_empty_array(void)
{
    // Arrange.
    int empty_array[] = {0};

    size_t size = 0;

    // Act & Assert.
    quick_sort(empty_array, 0, 0);

    // Success is implicitly measured by lack of segmentation fault.
}

/**
 * @brief Tests sorting an array with a single element.
 */
void test_quick_sort_should_handle_single_element_array(void)
{
    // Arrange.
    int single_element[] = {42};
    int expected[] = {42};

    size_t size = 1;

    // Act.
    quick_sort(single_element, 0, size - 1);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, single_element, size);
}

/**
 * @brief Tests sorting an array containing negative integers.
 */
void test_quick_sort_should_sort_negative_integers(void)
{
    // Arrange.
    int mixed_numbers[] = {-3, 10, -1, 0, -15};
    int expected[] = {-15, -3, -1, 0, 10};

    size_t size = 5;

    // Act.
    quick_sort(mixed_numbers, 0, size - 1);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, mixed_numbers, size);
}

/**
 * @brief Tests sorting with extreme values (INT_MIN/INT_MAX).
 */
void test_quick_sort_should_handle_extreme_integer_limits(void)
{
    // Arrange.
    int extreme_numbers[] = {INT_MAX, 0, INT_MIN, -1, 1};
    int expected[] = {INT_MIN, -1, 0, 1, INT_MAX};

    size_t size = 5;

    // Act.
    quick_sort(extreme_numbers, 0, size - 1);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, extreme_numbers, size);
}

/**
 * @brief Tests sorting of a large randomized sequence.
 */
void test_quick_sort_should_sort_large_random_sequence(void)
{
    // Arrange.
    int random_numbers[] = {50, 10, 40, 20, 30, 0, 5, 15, 25, 35};
    int expected[] = {0, 5, 10, 15, 20, 25, 30, 40, 50, 35};
    int actual_expected[] = {0, 5, 10, 15, 20, 25, 30, 35, 40, 50};

    size_t size = 10;

    // Act.
    quick_sort(random_numbers, 0, size - 1);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(actual_expected, random_numbers, size);
}

/**
 * @brief Tests stability/correctness with two-element array.
 */
void test_quick_sort_should_sort_two_element_unsorted_array(void)
{
    // Arrange.
    int pair[] = {2, 1};
    int expected[] = {1, 2};

    // Act.
    quick_sort(pair, 0, 1);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, pair, 2);
}

/**
 * @brief Tests array where pivot is already in the correct position.
 */
void test_quick_sort_should_sort_when_pivot_is_initially_correct(void)
{
    // Arrange.
    int pivot_centered[] = {1, 2, 5, 4, 3};
    int expected[] = {1, 2, 3, 4, 5};

    // Act.
    quick_sort(pivot_centered, 0, 4);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, pivot_centered, 5);
}

/**
 * @brief Tests an array with a "Sawtooth" pattern.
 */
void test_quick_sort_should_sort_sawtooth_pattern(void)
{
    // Arrange.
    int sawtooth[] = {10, 1, 10, 1, 10};
    int expected[] = {1, 1, 10, 10, 10};

    // Act.
    quick_sort(sawtooth, 0, 4);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, sawtooth, 5);
}

/**
 * @brief Tests nearly sorted array (only first and last swapped).
 */
void test_quick_sort_should_sort_nearly_sorted_with_ends_swapped(void)
{
    // Arrange.
    int nearly_sorted[] = {5, 2, 3, 4, 1};
    int expected[] = {1, 2, 3, 4, 5};

    // Act.
    quick_sort(nearly_sorted, 0, 4);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, nearly_sorted, 5);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();
    
    // Register the tests to run.
    RUN_TEST(test_quick_sort_should_sort_standard_unsorted_array);
    RUN_TEST(test_quick_sort_should_maintain_already_sorted_array);
    RUN_TEST(test_quick_sort_should_sort_reverse_sorted_array);
    RUN_TEST(test_quick_sort_should_handle_multiple_duplicates);
    RUN_TEST(test_quick_sort_should_handle_all_identical_elements);
    RUN_TEST(test_quick_sort_should_fail_gracefully_on_null_pointer);
    RUN_TEST(test_quick_sort_should_handle_empty_array);
    RUN_TEST(test_quick_sort_should_handle_single_element_array);
    RUN_TEST(test_quick_sort_should_sort_negative_integers);
    RUN_TEST(test_quick_sort_should_handle_extreme_integer_limits);
    RUN_TEST(test_quick_sort_should_sort_large_random_sequence);
    RUN_TEST(test_quick_sort_should_sort_two_element_unsorted_array);
    RUN_TEST(test_quick_sort_should_sort_when_pivot_is_initially_correct);
    RUN_TEST(test_quick_sort_should_sort_sawtooth_pattern);
    RUN_TEST(test_quick_sort_should_sort_nearly_sorted_with_ends_swapped);

    // Destroy the session.
    return UNITY_END();
}