/**
 * @file        :   merge_sort_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit tests for the Recursive Merge Sort implementation.
 * @details     :   Verifies the correctness of the merge sort algorithm using 
 * the Unity Test Framework. Covers various scenarios including 
 * edge cases (single elements, duplicates), performance ranges 
 * (INT_MIN/MAX), and sub-segment sorting logic.
 * Compile:
 * clang ./tests/darwin/sorting/merge_sort_unit_tests.c ./src/darwin/sorting/merge-sort/merge_sort.c ./src/unity.c -Iinclude -o ./bin/merge_sort_unit_test
 * @version     :   1.0
 * @date        :   2026-04-13
 */
#include <limits.h>

#include "unity.h"
#include "darwin/sorting/merge_sort.h"

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
 * @brief Test: Standard unsorted array.
 */
void test_sort_standard_unsorted_sequence(void)
{
    // Arrange
    int array[] = {64, 34, 25, 12, 22, 11, 90};
    int expected[] = {11, 12, 22, 25, 34, 64, 90};

    size_t size = sizeof(array) / sizeof(array[0]);

    // Act
    merge_sort(array, 0, size - 1);

    // Assert
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Test: Array that is already sorted.
 */
void test_sort_already_ordered_data(void)
{
    // Arrange
    int array[] = {1, 2, 3, 4, 5};
    int expected[] = {1, 2, 3, 4, 5};

    size_t size = 5;

    // Act
    merge_sort(array, 0, size - 1);

    // Assert
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Test: Array in exact reverse order.
 */
void test_sort_reverse_chronological_data(void)
{
    // Arrange
    int array[] = {10, 8, 6, 4, 2};
    int expected[] = {2, 4, 6, 8, 10};

    size_t size = 5;

    // Act
    merge_sort(array, 0, size - 1);

    // Assert
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Test: Handling duplicate values.
 */
void test_sort_redundant_value_collection(void)
{
    // Arrange
    int array[] = {5, 1, 5, 2, 1};
    int expected[] = {1, 1, 2, 5, 5};

    size_t size = 5;

    // Act
    merge_sort(array, 0, size - 1);

    // Assert
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Test: Single element array (base case).
 */
void test_sort_singular_element_array(void)
{
    // Arrange
    int array[] = {42};
    int expected[] = {42};

    size_t size = 1;

    // Act
    merge_sort(array, 0, size - 1);

    // Assert
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Test: Two elements in wrong order.
 */
void test_sort_binary_unordered_pair(void)
{
    // Arrange
    int array[] = {2, 1};
    int expected[] = {1, 2};

    size_t size = 2;

    // Act
    merge_sort(array, 0, size - 1);

    // Assert
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Test: Array with all identical values.
 */
void test_sort_homogenous_value_stream(void)
{
    // Arrange
    int array[] = {7, 7, 7, 7};
    int expected[] = {7, 7, 7, 7};

    size_t size = 4;

    // Act
    merge_sort(array, 0, size - 1);

    // Assert
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Test: Large negative and positive values.
 */
void test_sort_extreme_integer_range(void)
{
    // Arrange
    int array[] = {INT_MAX, 0, INT_MIN, -1, 1};
    int expected[] = {INT_MIN, -1, 0, 1, INT_MAX};

    size_t size = 5;

    // Act
    merge_sort(array, 0, size - 1);

    // Assert
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Test: Array with prime numbers in random order.
 */
void test_sort_prime_number_distribution(void)
{
    // Arrange
    int array[] = {13, 2, 11, 3, 7, 5};
    int expected[] = {2, 3, 5, 7, 11, 13};

    size_t size = 6;

    // Act
    merge_sort(array, 0, size - 1);

    // Assert
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Test: Only negative integers.
 */
void test_sort_strictly_negative_integers(void)
{
    // Arrange
    int array[] = {-5, -10, -1, -3};
    int expected[] = {-10, -5, -3, -1};

    size_t size = 4;

    // Act
    merge_sort(array, 0, size - 1);

    // Assert
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Test: Array where only the last element is out of place.
 */
void test_sort_nearly_ordered_trailing_outlier(void)
{
    // Arrange
    int array[] = {10, 20, 30, 40, 5};
    int expected[] = {5, 10, 20, 30, 40};

    size_t size = 5;

    // Act
    merge_sort(array, 0, size - 1);

    // Assert
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Test: Array where only the first element is out of place.
 */
void test_sort_nearly_ordered_leading_outlier(void)
{
    // Arrange
    int array[] = {50, 1, 2, 3, 4};
    int expected[] = {1, 2, 3, 4, 50};

    size_t size = 5;

    // Act
    merge_sort(array, 0, size - 1);

    // Assert
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Test: Large sequence of zero values.
 */
void test_sort_null_value_sequence(void)
{
    // Arrange
    int array[] = {0, 0, 0, 1, 0};
    int expected[] = {0, 0, 0, 0, 1};

    size_t size = 5;

    // Act
    merge_sort(array, 0, size - 1);

    // Assert
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Test: Array with alternating high and low values.
 */
void test_sort_oscillating_value_pattern(void)
{
    // Arrange
    int array[] = {100, 1, 99, 2, 98, 3};
    int expected[] = {1, 2, 3, 98, 99, 100};

    size_t size = 6;

    // Act
    merge_sort(array, 0, size - 1);

    // Assert
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Test: Sorting a sub-section of an array only.
 */
void test_sort_isolated_subsegment_logic(void)
{
    // Arrange
    int array[] = {10, 5, 2, 8, 0};
    int expected[] = {10, 2, 5, 8, 0};

    // Act
    merge_sort(array, 1, 3);

    // Assert
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, 5);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_sort_standard_unsorted_sequence);
    RUN_TEST(test_sort_already_ordered_data);
    RUN_TEST(test_sort_reverse_chronological_data);
    RUN_TEST(test_sort_redundant_value_collection);
    RUN_TEST(test_sort_singular_element_array);
    RUN_TEST(test_sort_binary_unordered_pair);
    RUN_TEST(test_sort_homogenous_value_stream);
    RUN_TEST(test_sort_extreme_integer_range);
    RUN_TEST(test_sort_prime_number_distribution);
    RUN_TEST(test_sort_strictly_negative_integers);
    RUN_TEST(test_sort_nearly_ordered_trailing_outlier);
    RUN_TEST(test_sort_nearly_ordered_leading_outlier);
    RUN_TEST(test_sort_null_value_sequence);
    RUN_TEST(test_sort_oscillating_value_pattern);
    RUN_TEST(test_sort_isolated_subsegment_logic);

    // Destroy the session.
    return UNITY_END();
}