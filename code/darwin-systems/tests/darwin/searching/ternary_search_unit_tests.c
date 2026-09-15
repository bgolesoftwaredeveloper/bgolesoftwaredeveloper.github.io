/**
 * @file        :   ternary_search_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit tests for the jump search algorithm.
 * @details     :   This suite utilizes the Unity Test Framework to validate the 
 * jump search implementation across various edge cases and standard scenarios.
 * Compile:
 * clang ./tests/darwin/searching/ternary_search_unit_tests.c ./src/darwin/searching/ternary-search/ternary_search.c ./src/unity.c -Iinclude -o ./bin/ternary_search_unit_test
 * @version     :   1.0
 * @date        :   2026-04-14
 */

#include "unity.h"
#include "darwin/searching/ternary_search.h"

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
 * @brief Validates finding a value at the very beginning of a sorted array.
 */
void test_finds_element_at_start(void)
{
    // Arrange.
    int array[] = {10, 20, 30, 40, 50, 60};
    size_t size = 6;
    int key = 10;

    // Act.
    int result = ternary_search(array, 0, size - 1, key);

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, result);
}

/**
 * @brief Validates finding a value at the very end of a sorted array.
 */
void test_finds_element_at_end(void)
{
    // Arrange.
    int array[] = {10, 20, 30, 40, 50, 60};
    size_t size = 6;
    int key = 60;

    // Act.
    int result = ternary_search(array, 0, size - 1, key);

    // Assert.
    TEST_ASSERT_EQUAL_INT(5, result);
}

/**
 * @brief Validates finding an element specifically located at the first division point.
 */
void test_finds_element_at_left_pivot(void)
{
    // Arrange.
    int array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    size_t size = 9;
    int key = 3;

    // Act.
    int result = ternary_search(array, 0, size - 1, key);

    // Assert.
    TEST_ASSERT_EQUAL_INT(2, result);
}

/**
 * @brief Validates finding an element specifically located at the second division point.
 */
void test_finds_element_at_right_pivot(void)
{
    // Arrange.
    int array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    size_t size = 9;
    int key = 7;

    // Act.
    int result = ternary_search(array, 0, size - 1, key);

    // Assert.
    TEST_ASSERT_EQUAL_INT(6, result);
}

/**
 * @brief Validates that the search works correctly on an array with only one element.
 */
void test_single_element_array_found(void)
{
    // Arrange.
    int array[] = {42};
    size_t size = 1;
    int key = 42;

    // Act.
    int result = ternary_search(array, 0, size - 1, key);

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, result);
}

/**
 * @brief Validates that a missing key is correctly handled in a single-element array.
 */
void test_single_element_array_not_found(void)
{
    // Arrange.
    int array[] = {42};
    size_t size = 1;
    int key = 7;

    // Act.
    int result = ternary_search(array, 0, size - 1, key);

    // Assert.
    TEST_ASSERT_EQUAL_INT(-1, result);
}

/**
 * @brief Validates handling when the target key is smaller than all array elements.
 */
void test_key_less_than_minimum(void)
{
    // Arrange.
    int array[] = {10, 20, 30};
    size_t size = 3;
    int key = 5;

    // Act.
    int result = ternary_search(array, 0, size - 1, key);

    // Assert.
    TEST_ASSERT_EQUAL_INT(-1, result);
}

/**
 * @brief Validates handling when the target key is larger than all array elements.
 */
void test_key_greater_than_maximum(void)
{
    // Arrange.
    int array[] = {10, 20, 30};
    size_t size = 3;
    int key = 100;

    // Act.
    int result = ternary_search(array, 0, size - 1, key);

    // Assert.
    TEST_ASSERT_EQUAL_INT(-1, result);
}

/**
 * @brief Validates that the algorithm functions correctly with an even number of elements.
 */
void test_even_sized_array(void)
{
    // Arrange.
    int array[] = {2, 4, 6, 8};
    size_t size = 4;
    int key = 6;

    // Act.
    int result = ternary_search(array, 0, size - 1, key);

    // Assert.
    TEST_ASSERT_EQUAL_INT(2, result);
}

/**
 * @brief Validates finding a value located specifically in the middle third of the array.
 */
void test_finds_element_in_middle_third(void)
{
    // Arrange.
    int array[] = {10, 20, 30, 40, 50, 60, 70, 80, 90};
    size_t size = 9;
    int key = 50;

    // Act.
    int result = ternary_search(array, 0, size - 1, key);

    // Assert.
    TEST_ASSERT_EQUAL_INT(4, result);
}

/**
 * @brief Validates safety by ensuring the search returns -1 for a NULL array pointer.
 */
void test_null_array_pointer(void)
{
    // Arrange.
    int* array = NULL;
    int key = 10;

    // Act.
    int result = ternary_search(array, 0, 0, key);

    // Assert.
    TEST_ASSERT_EQUAL_INT(-1, result);
}

/**
 * @brief Validates that missing values falling between array elements return -1.
 */
void test_missing_value_in_range(void)
{
    // Arrange.
    int array[] = {10, 20, 30, 40};
    size_t size = 4;
    int key = 25;

    // Act.
    int result = ternary_search(array, 0, size - 1, key);

    // Assert.
    TEST_ASSERT_EQUAL_INT(-1, result);
}

/**
 * @brief Validates that the search finds at least one instance in an array of duplicates.
 */
void test_all_identical_elements(void)
{
    // Arrange.
    int array[] = {5, 5, 5, 5, 5};
    size_t size = 5;
    int key = 5;

    // Act.
    int result = ternary_search(array, 0, size - 1, key);

    // Assert.
    TEST_ASSERT_TRUE(result >= 0 && result <= 4);
}

/**
 * @brief Validates search accuracy and recursion depth on a larger dataset.
 */
void test_large_sorted_array(void)
{
    // Arrange.
    int array[100];
    
    for (int index = 0; index < 100; index++) 
    {
        array[index] = index * 2;
    }

    size_t size = 100;
    int key = 150;

    // Act.
    int result = ternary_search(array, 0, size - 1, key);

    // Assert.
    TEST_ASSERT_EQUAL_INT(75, result);
}

/**
 * @brief Validates that an improper search range (low > high) returns -1 immediately.
 */
void test_invalid_range(void)
{
    // Arrange.
    int array[] = {10, 20, 30};
    int key = 20;

    // Act.
    int result = ternary_search(array, 5, 2, key);

    // Assert.
    TEST_ASSERT_EQUAL_INT(-1, result);
}

int main(void)
{
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_finds_element_at_start);
    RUN_TEST(test_finds_element_at_end);
    RUN_TEST(test_finds_element_at_left_pivot);
    RUN_TEST(test_finds_element_at_right_pivot);
    RUN_TEST(test_single_element_array_found);
    RUN_TEST(test_single_element_array_not_found);
    RUN_TEST(test_key_less_than_minimum);
    RUN_TEST(test_key_greater_than_maximum);
    RUN_TEST(test_even_sized_array);
    RUN_TEST(test_finds_element_in_middle_third);
    RUN_TEST(test_null_array_pointer);
    RUN_TEST(test_missing_value_in_range);
    RUN_TEST(test_all_identical_elements);
    RUN_TEST(test_large_sorted_array);
    RUN_TEST(test_invalid_range);

    // Destroy the session.
    return UNITY_END();
}