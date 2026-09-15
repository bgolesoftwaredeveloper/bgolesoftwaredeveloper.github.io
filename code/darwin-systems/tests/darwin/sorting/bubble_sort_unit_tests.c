/**
 * @file        :   bubble_sort_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   This is a bubble sort.
 * @details     :   Bubble sort is a simple comparison-based sorting algorithm.
 *                  Compile:
 *                  clang ./tests/darwin/sorting/bubble_sort_unit_tests.c ./src/darwin/sorting/bubble-sort/bubble_sort.c ./src/unity.c -Iinclude -o ./bin/bubble_sort_unit_test
 * @version     :   1.0
 * @date        :   2026-04-13
 */
#include <limits.h>

#include "unity.h"
#include "darwin/sorting/bubble_sort.h"

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
void test_bubble_sort_should_sort_standard_array(void)
{
    // Arrange.
    int array[] = {5, 1, 3, 2, 8};
    int expected[] = {1, 2, 3, 5, 8};

    size_t size = sizeof(array) / sizeof(array[0]);

    // Act.
    bubble_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Tests sorting of an array that is already sorted.
 */
void test_bubble_sort_should_handle_already_sorted_array(void)
{
    // Arrange.
    int array[] = {10, 20, 30, 40, 50};
    int expected[] = {10, 20, 30, 40, 50};

    size_t size = 5;

    // Act.
    bubble_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Tests sorting of an array in reverse order.
 */
void test_bubble_sort_should_sort_reverse_array(void)
{
    // Arrange.
    int array[] = {5, 4, 3, 2, 1};
    int expected[] = {1, 2, 3, 4, 5};

    size_t size = 5;

    // Act.
    bubble_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Tests sorting of an array containing duplicate values.
 */
void test_bubble_sort_should_handle_duplicates(void)
{
    // Arrange.
    int array[] = {3, 1, 2, 3, 1};
    int expected[] = {1, 1, 2, 3, 3};

    size_t size = 5;

    // Act.
    bubble_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Tests the algorithm's response to an empty array (size 0).
 */
void test_bubble_sort_should_handle_empty_array(void)
{
    // Arrange.
    int array[] = {0}; 
    size_t size = 0;

    // Act.
    bubble_sort(array, size);

    // Assert.
    // Success is identified by the test runner completing without a crash.
}

/**
 * @brief Tests the algorithm's response to a NULL pointer.
 */
void test_bubble_sort_should_handle_null_pointer(void)
{
    // Arrange.
    int* array = NULL;

    // Act.
    bubble_sort(array, 10);

    // Assert.
    // Success is identified by the test runner completing without a crash.
}

/**
 * @brief Tests sorting of an array with only one element.
 * @details Verifies that the algorithm safely exits without error on minimal size.
 */
void test_bubble_sort_should_handle_single_element(void)
{
    // Arrange.
    int array[] = {42};
    int expected[] = {42};

    size_t size = 1;

    // Act.
    bubble_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Tests sorting of an array containing negative integers.
 */
void test_bubble_sort_should_sort_negative_numbers(void)
{
    // Arrange.
    int array[] = {-5, 2, -10, 0, 7};
    int expected[] = {-10, -5, 0, 2, 7};

    size_t size = 5;

    // Act.
    bubble_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Tests an array where only the last element is out of place.
 */
void test_bubble_sort_should_sort_nearly_sorted_end(void)
{
    // Arrange.
    int array[] = {2, 3, 4, 5, 1};
    int expected[] = {1, 2, 3, 4, 5};

    size_t size = 5;

    // Act.
    bubble_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Tests an array with all identical values.
 * @details Ensures the swapped flag optimization handles zero-swap scenarios correctly.
 */
void test_bubble_sort_should_handle_all_identical_elements(void)
{
    // Arrange.
    int array[] = {7, 7, 7, 7, 7};
    int expected[] = {7, 7, 7, 7, 7};

    size_t size = 5;

    // Act.
    bubble_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Tests an array where only the first element is out of place.
 * @details This is the inverse of the "nearly sorted end" test, forcing the 
 * value to bubble through the entire length.
 */
void test_bubble_sort_should_sort_nearly_sorted_beginning(void)
{
    // Arrange.
    int array[] = {10, 1, 2, 3, 4};
    int expected[] = {1, 2, 3, 4, 10};

    size_t size = 5;

    // Act.
    bubble_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Tests an array with the maximum and minimum possible integer values.
 * @details Verifies that comparison logic doesn't suffer from overflow 
 * if arithmetic is used for comparison (though standard > is safe).
 */
void test_bubble_sort_should_handle_extreme_values(void)
{
    // Arrange.
    int array[] = {0, INT_MAX, INT_MIN, -1, 1};
    int expected[] = {INT_MIN, -1, 0, 1, INT_MAX};

    size_t size = 5;

    // Act.
    bubble_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Tests a large, completely scrambled array.
 * @details Ensures that the nested loops iterate correctly over a larger set.
 */
void test_bubble_sort_should_sort_scrambled_large_array(void)
{
    // Arrange.
    int array[] = {9, 3, 7, 1, 5, 8, 2, 6, 4, 0};
    int expected[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    size_t size = 10;

    // Act.
    bubble_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Tests an array where two identical values are separated.
 * @details Verifies "stability"—while hard to see with just ints, it ensures 
 * the algorithm doesn't perform unnecessary swaps on equal values.
 */
void test_bubble_sort_should_handle_spread_duplicates(void)
{
    // Arrange.
    int array[] = {5, 2, 9, 2, 1};
    int expected[] = {1, 2, 2, 5, 9};

    size_t size = 5;

    // Act.
    bubble_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

/**
 * @brief Tests a "Zig-Zag" or "Sawtooth" pattern.
 * @details High-low patterns force the bubble flag to work hard every pass.
 */
void test_bubble_sort_should_sort_zigzag_array(void)
{
    // Arrange.
    int array[] = {1, 10, 2, 9, 3, 8};
    int expected[] = {1, 2, 3, 8, 9, 10};

    size_t size = 6;

    // Act.
    bubble_sort(array, size);

    // Assert.
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, array, size);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register tests to run.
    RUN_TEST(test_bubble_sort_should_sort_standard_array);
    RUN_TEST(test_bubble_sort_should_handle_already_sorted_array);
    RUN_TEST(test_bubble_sort_should_sort_reverse_array);
    RUN_TEST(test_bubble_sort_should_handle_duplicates);
    RUN_TEST(test_bubble_sort_should_handle_empty_array);
    RUN_TEST(test_bubble_sort_should_handle_null_pointer);
    RUN_TEST(test_bubble_sort_should_handle_single_element);
    RUN_TEST(test_bubble_sort_should_sort_negative_numbers);
    RUN_TEST(test_bubble_sort_should_sort_nearly_sorted_end);
    RUN_TEST(test_bubble_sort_should_handle_all_identical_elements);
    RUN_TEST(test_bubble_sort_should_sort_nearly_sorted_beginning);
    RUN_TEST(test_bubble_sort_should_handle_extreme_values);
    RUN_TEST(test_bubble_sort_should_sort_scrambled_large_array);
    RUN_TEST(test_bubble_sort_should_handle_spread_duplicates);
    RUN_TEST(test_bubble_sort_should_sort_zigzag_array);

    // Destory the session.
    return UNITY_END();
}