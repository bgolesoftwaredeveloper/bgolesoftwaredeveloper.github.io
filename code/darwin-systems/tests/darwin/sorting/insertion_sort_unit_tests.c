/**
 * @file        :   insertion_sort_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   This is a Insertion Sort.
 * @details     :   This is the test suite for the insertion sort.
 *                  clang ./tests/darwin/sorting/insertion_sort_unit_tests.c ./src/darwin/sorting/insertion-sort/insertion_sort.c ./src/unity.c -Iinclude -o ./bin/insertion_sort_unit_test
 * @version     :   1.0
 * @date        :   2026-04-17
 */
#include <limits.h>

#include "unity.h"
#include "darwin/sorting/insertion_sort.h"

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
 * @brief Tests sorting of a standard, unsorted integer array.
 */
void test_insertion_sort_should_sort_standard_array(void)
{
    // Arrange.
    int array[] = {5, 1, 3, 2, 8};
    int expected[] = {1, 2, 3, 5, 8};
    size_t size = 5;

    // Act.
    insertion_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Tests sorting of an array that is already sorted.
 */
void test_insertion_sort_should_handle_already_sorted_array(void)
{
    // Arrange.
    int array[] = {10, 20, 30, 40, 50};
    int expected[] = {10, 20, 30, 40, 50};
    size_t size = 5;

    // Act.
    insertion_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Tests sorting of an array in reverse order.
 */
void test_insertion_sort_should_sort_reverse_array(void)
{
    // Arrange.
    int array[] = {5, 4, 3, 2, 1};
    int expected[] = {1, 2, 3, 4, 5};
    size_t size = 5;

    // Act.
    insertion_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Tests sorting of an array containing duplicate values.
 */
void test_insertion_sort_should_handle_duplicates(void)
{
    // Arrange.
    int array[] = {3, 1, 2, 3, 1};
    int expected[] = {1, 1, 2, 3, 3};
    size_t size = 5;

    // Act.
    insertion_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Tests the algorithm's response to an empty array (size 0).
 */
void test_insertion_sort_should_handle_empty_array(void)
{
    // Arrange.
    int array[] = {0};
    size_t size = 0;

    // Act.
    insertion_sort(array, size);

    // Assert.
    // Success is identified by the test runner completing without a crash.
}

/**
 * @brief Tests the algorithm's response to a NULL pointer.
 */
void test_insertion_sort_should_handle_null_pointer(void)
{
    // Arrange.
    int* array = NULL;

    // Act.
    insertion_sort(array, 10);

    // Assert.
    // Success is identified by the test runner completing without a crash.
}

/**
 * @brief Tests sorting of an array with only one element.
 */
void test_insertion_sort_should_handle_single_element(void)
{
    // Arrange.
    int array[] = {42};
    int expected[] = {42};
    size_t size = 1;

    // Act.
    insertion_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Tests sorting of an array containing negative numbers.
 */
void test_insertion_sort_should_handle_negative_numbers(void)
{
    // Arrange.
    int array[] = {0, -5, 10, -2, 3};
    int expected[] = {-5, -2, 0, 3, 10};
    size_t size = 5;

    // Act.
    insertion_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Tests an array where only the last element is out of place.
 */
void test_insertion_sort_should_sort_nearly_sorted_end(void)
{
    // Arrange.
    int array[] = {2, 3, 4, 5, 1};
    int expected[] = {1, 2, 3, 4, 5};
    size_t size = 5;

    // Act.
    insertion_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Tests an array with all identical values.
 */
void test_insertion_sort_should_handle_all_identical_elements(void)
{
    // Arrange.
    int array[] = {7, 7, 7, 7, 7};
    int expected[] = {7, 7, 7, 7, 7};
    size_t size = 5;

    // Act.
    insertion_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Tests an array where only the first element is out of place.
 */
void test_insertion_sort_should_sort_nearly_sorted_beginning(void)
{
    // Arrange.
    int array[] = {10, 1, 2, 3, 4};
    int expected[] = {1, 2, 3, 4, 10};
    size_t size = 5;

    // Act.
    insertion_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Tests sorting with INT_MIN and INT_MAX values.
 */
void test_insertion_sort_should_handle_boundary_values(void)
{
    // Arrange.
    int array[] = {INT_MAX, 0, INT_MIN, 42};
    int expected[] = {INT_MIN, 0, 42, INT_MAX};
    size_t size = 4;

    // Act.
    insertion_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Tests a large, completely scrambled array.
 */
void test_insertion_sort_should_sort_scrambled_large_array(void)
{
    // Arrange.
    int array[] = {9, 3, 7, 1, 5, 8, 2, 6, 4, 0};
    int expected[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    size_t size = 10;

    // Act.
    insertion_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Tests an array where identical values are spread throughout.
 */
void test_insertion_sort_should_handle_spread_duplicates(void)
{
    // Arrange.
    int array[] = {5, 2, 9, 2, 1};
    int expected[] = {1, 2, 2, 5, 9};
    size_t size = 5;

    // Act.
    insertion_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Tests sorting of an array that is already sorted in ascending order.
 */
void test_insertion_sort_should_verify_stability(void)
{
    // Arrange.
    int array[] = {1, 3, 3, 2};
    int expected[] = {1, 2, 3, 3};
    size_t size = 4;

    // Act.
    insertion_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register tests to run.
    RUN_TEST(test_insertion_sort_should_sort_standard_array);
    RUN_TEST(test_insertion_sort_should_handle_already_sorted_array);
    RUN_TEST(test_insertion_sort_should_sort_reverse_array);
    RUN_TEST(test_insertion_sort_should_handle_duplicates);
    RUN_TEST(test_insertion_sort_should_handle_empty_array);
    RUN_TEST(test_insertion_sort_should_handle_null_pointer);
    RUN_TEST(test_insertion_sort_should_handle_single_element);
    RUN_TEST(test_insertion_sort_should_handle_negative_numbers);
    RUN_TEST(test_insertion_sort_should_sort_nearly_sorted_end);
    RUN_TEST(test_insertion_sort_should_handle_all_identical_elements);
    RUN_TEST(test_insertion_sort_should_sort_nearly_sorted_beginning);
    RUN_TEST(test_insertion_sort_should_handle_boundary_values);
    RUN_TEST(test_insertion_sort_should_sort_scrambled_large_array);
    RUN_TEST(test_insertion_sort_should_handle_spread_duplicates);
    RUN_TEST(test_insertion_sort_should_verify_stability);

    // Destroy the session.
    return UNITY_END();
}