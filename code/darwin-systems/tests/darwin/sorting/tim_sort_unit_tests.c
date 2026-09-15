/**
 * @file        :   tim_sort_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Comprehensive unit test suite for the Timsort algorithm.
 * @details     :   This module uses the Unity Test Framework to validate various 
 * edge cases, logic correctness, and standard sorting scenarios
 * specific to the hybrid Timsort algorithm.
 * Compile:
 * clang ./tests/darwin/sorting/tim_sort_unit_tests.c ./src/darwin/sorting/tim-sort/tim_sort.c ./src/unity.c -Iinclude -o ./bin/tim_sort_unit_test
 * @version     :   1.0
 * @date        :   2026-04-18
 */
#include <stdio.h>

// Include header files.
#include "unity.h"
#include "darwin/sorting/tim_sort.h"

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
void test_tim_sort_standard_positive(void)
{
    // Arrange.
    int array[] = {64, 25, 12, 22, 11};
    int expected[] = {11, 12, 22, 25, 64};
    size_t size = 5;

    // Act.
    tim_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting of an array that is already sorted.
 */
void test_tim_sort_already_sorted(void)
{
    // Arrange.
    int array[] = {1, 2, 3, 4, 5};
    int expected[] = {1, 2, 3, 4, 5};
    size_t size = 5;

    // Act.
    tim_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting of an array sorted in descending order.
 */
void test_tim_sort_reverse_sorted(void)
{
    // Arrange.
    int array[] = {5, 4, 3, 2, 1};
    int expected[] = {1, 2, 3, 4, 5};
    size_t size = 5;

    // Act.
    tim_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting of an array where all elements are identical.
 */
void test_tim_sort_identical_elements(void)
{
    // Arrange.
    int array[] = {7, 7, 7, 7, 7};
    int expected[] = {7, 7, 7, 7, 7};
    size_t size = 5;

    // Act.
    tim_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting of an array with a single element.
 */
void test_tim_sort_single_element(void)
{
    // Arrange.
    int array[] = {42};
    int expected[] = {42};
    size_t size = 1;

    // Act.
    tim_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests the behavior when a NULL pointer is provided.
 */
void test_tim_sort_null_pointer(void)
{
    // Arrange.
    int* array = NULL;
    size_t size = 10;

    // Act.
    tim_sort(array, size);
    
    // Assert.
    TEST_ASSERT_NULL(array);
}

/**
 * @brief Tests sorting of an array containing only two elements.
 */
void test_tim_sort_two_elements(void)
{
    // Arrange.
    int array[] = {10, -5};
    int expected[] = {-5, 10};
    size_t size = 2;

    // Act.
    tim_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting when the maximum and minimum integer values are present.
 */
void test_tim_sort_extreme_values(void)
{
    // Arrange.
    int array[] = {INT_MAX, 0, INT_MIN};
    int expected[] = {INT_MIN, 0, INT_MAX};
    size_t size = 3;

    // Act.
    tim_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting of an array with many duplicate values.
 */
void test_tim_sort_large_duplicates(void)
{
    // Arrange.
    int array[] = {3, 1, 2, 1, 3, 2, 1};
    int expected[] = {1, 1, 1, 2, 2, 3, 3};
    size_t size = 7;

    // Act.
    tim_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting of an array with both positive and negative integers.
 */
void test_tim_sort_mixed_signs(void)
{
    // Arrange.
    int array[] = {-10, 5, -2, 0, 8};
    int expected[] = {-10, -2, 0, 5, 8};
    size_t size = 5;

    // Act.
    tim_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting an array larger than the defined RUN size (32).
 */
void test_tim_sort_larger_than_run(void)
{
    // Arrange.
    int array[40];
    int expected[40];

    for (int index = 0; index < 40; index++)
    {
        array[index] = 40 - index;
        expected[index] = index + 1;
    }

    size_t size = 40;

    // Act.
    tim_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting when the array size is an exact multiple of RUN.
 */
void test_tim_sort_exact_run_multiple(void)
{
    // Arrange.
    int array[64];

    for (int index = 0; index < 64; index++) 
    {
        array[index] = (index % 2 == 0) ? 100 - index : index;
    }
    
    size_t size = 64;

    // Act.
    tim_sort(array, size);
    
    // Assert.
    for (size_t index = 0; index < size - 1; index++) {
        TEST_ASSERT_TRUE(array[index] <= array[index+1]);
    }
}

/**
 * @brief Tests sorting of an array where the minimum is at the very end.
 */
void test_tim_sort_min_at_end(void)
{
    // Arrange.
    int array[] = {50, 40, 30, 20, 10};
    int expected[] = {10, 20, 30, 40, 50};
    size_t size = 5;

    // Act.
    tim_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests sorting when the maximum is at the very beginning.
 */
void test_tim_sort_max_at_start(void)
{
    // Arrange.
    int array[] = {100, 10, 20, 30, 40};
    int expected[] = {10, 20, 30, 40, 100};
    size_t size = 5;

    // Act.
    tim_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

/**
 * @brief Tests stability integrity by ensuring equal elements remain sorted.
 */
void test_tim_sort_stability_integrity(void)
{
    // Arrange.
    int array[] = {5, 2, 5, 1, 2};
    int expected[] = {1, 2, 2, 5, 5};
    size_t size = 5;

    // Act.
    tim_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, (int)size);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_tim_sort_standard_positive);
    RUN_TEST(test_tim_sort_already_sorted);
    RUN_TEST(test_tim_sort_reverse_sorted);
    RUN_TEST(test_tim_sort_identical_elements);
    RUN_TEST(test_tim_sort_single_element);
    RUN_TEST(test_tim_sort_null_pointer);
    RUN_TEST(test_tim_sort_two_elements);
    RUN_TEST(test_tim_sort_extreme_values);
    RUN_TEST(test_tim_sort_large_duplicates);
    RUN_TEST(test_tim_sort_mixed_signs);
    RUN_TEST(test_tim_sort_larger_than_run);
    RUN_TEST(test_tim_sort_exact_run_multiple);
    RUN_TEST(test_tim_sort_min_at_end);
    RUN_TEST(test_tim_sort_max_at_start);
    RUN_TEST(test_tim_sort_stability_integrity);

    // Destroy the session.
    return UNITY_END();
}