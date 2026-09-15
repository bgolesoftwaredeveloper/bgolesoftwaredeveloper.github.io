/**
 * @file        :   exponential_search_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit tests for the exponential search algorithm.
 * @details     :   This suite utilizes the Unity Test Framework to validate the 
 * exponential search implementation across various edge cases and standard scenarios.
 * Compile:
 * clang ./tests/darwin/searching/exponential_search_unit_tests.c ./src/darwin/searching/exponential-search/exponential_search.c ./src/unity.c -Iinclude -o ./bin/exponential_search_unit_test
 * @version     :   1.0
 * @date        :   2026-04-16
 */

#include "unity.h"
#include "darwin/searching/exponential_search.h"

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
 * @brief Test searching for a target in the exact middle of an odd-sized array.
 */
void test_search_target_in_middle(void)
{
    // Arrange.
    int array[] = {10, 20, 30, 40, 50};
    
    size_t size = 5;
    int target = 30;

    // Act.
    int result = exponential_search(array, size, target);

    // Assert.
    TEST_ASSERT_EQUAL_INT(2, result);
}

/**
 * @brief Test searching for a target at the very beginning of the array.
 */
void test_search_target_at_start(void)
{
    // Arrange.
    int array[] = {10, 20, 30, 40, 50};

    size_t size = 5;
    int target = 10;

    // Act.
    int result = exponential_search(array, size, target);

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, result);
}

/**
 * @brief Test searching for a target at the very end of the array.
 */
void test_search_target_at_end(void)
{
    // Arrange.
    int array[] = {10, 20, 30, 40, 50};

    size_t size = 5;
    int target = 50;

    // Act.
    int result = exponential_search(array, size, target);

    // Assert.
    TEST_ASSERT_EQUAL_INT(4, result);
}

/**
 * @brief Test behavior when the target value is not present in the array.
 */
void test_search_target_not_found(void)
{
    // Arrange.
    int array[] = {10, 20, 30, 40, 50};

    size_t size = 5;
    int target = 25;

    // Act.
    int result = exponential_search(array, size, target);

    // Assert.
    TEST_ASSERT_EQUAL_INT(-1, result);
}

/**
 * @brief Test searching in an array with only one element (target present).
 */
void test_single_element_array_found(void)
{
    // Arrange.
    int array[] = {100};

    size_t size = 1;
    int target = 100;

    // Act.
    int result = exponential_search(array, size, target);

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, result);
}

/**
 * @brief Test searching in an array with only one element (target absent).
 */
void test_single_element_array_not_found(void)
{
    // Arrange.
    int array[] = {100};

    size_t size = 1;
    int target = 50;

    // Act.
    int result = exponential_search(array, size, target);

    // Assert.
    TEST_ASSERT_EQUAL_INT(-1, result);
}

/**
 * @brief Test defensive check for a NULL array pointer.
 */
void test_null_array_pointer(void)
{
    // Arrange.
    int* array = NULL;

    size_t size = 10;
    int target = 5;

    // Act.
    int result = exponential_search(array, size, target);

    // Assert.
    TEST_ASSERT_EQUAL_INT(-1, result);
}

/**
 * @brief Test defensive check for an empty array (size 0).
 */
void test_empty_array_size(void)
{
    // Arrange.
    int array[] = {1, 2, 3};

    size_t size = 0;
    int target = 2;

    // Act.
    int result = exponential_search(array, size, target);

    // Assert.
    TEST_ASSERT_EQUAL_INT(-1, result);
}

/**
 * @brief Test searching in an even-sized array (left-leaning middle).
 */
void test_even_sized_array_search(void)
{
    // Arrange.
    int array[] = {10, 20, 30, 40};

    size_t size = 4;
    int target = 20;

    // Act.
    int result = exponential_search(array, size, target);

    // Assert.
    TEST_ASSERT_EQUAL_INT(1, result);
}

/**
 * @brief Test searching for the smallest value in a large range.
 */
void test_large_array_min_boundary(void)
{
    // Arrange.
    int array[100];

    for (int index = 0; index < 100; index++) 
    {
        array[index] = index * 2;
    }

    size_t size = 100;
    int target = 0;

    // Act.
    int result = exponential_search(array, size, target);

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, result);
}

/**
 * @brief Test searching for the largest value in a large range.
 */
void test_large_array_max_boundary(void)
{
    // Arrange.
    int array[100];

    for (int index = 0; index < 100; index++) 
    {
        array[index] = index * 2;
    }

    size_t size = 100;
    int target = 198;

    // Act.
    int result = exponential_search(array, size, target);

    // Assert.
    TEST_ASSERT_EQUAL_INT(99, result);
}

/**
 * @brief Test search when target is smaller than the minimum element.
 */
void test_target_smaller_than_minimum(void)
{
    // Arrange.
    int array[] = {10, 20, 30};

    size_t size = 3;
    int target = 5;

    // Act.
    int result = exponential_search(array, size, target);

    // Assert.
    TEST_ASSERT_EQUAL_INT(-1, result);
}

/**
 * @brief Test search when target is larger than the maximum element.
 */
void test_target_larger_than_maximum(void)
{
    // Arrange.
    int array[] = {10, 20, 30};

    size_t size = 3;
    int target = 40;

    // Act.
    int result = exponential_search(array, size, target);

    // Assert.
    TEST_ASSERT_EQUAL_INT(-1, result);
}

/**
 * @brief Test searching an array containing negative numbers.
 */
void test_array_with_negative_numbers(void)
{
    // Arrange.
    int array[] = {-50, -20, -10, 0, 5, 15};

    size_t size = 6;
    int target = -20;

    // Act.
    int result = exponential_search(array, size, target);

    // Assert.
    TEST_ASSERT_EQUAL_INT(1, result);
}

/**
 * @brief Test search in an array where all elements are identical.
 */
void test_all_identical_elements(void)
{
    // Arrange.
    int array[] = {7, 7, 7, 7, 7};

    size_t size = 5;
    int target = 7;

    // Act.
    int result = exponential_search(array, size, target);

    // Assert.
    TEST_ASSERT_MESSAGE(result >= 0 && result < 5, "Failed to find target in identical array");
}

/**
 * @brief Main entry point for the unit test suite.
 */
int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_search_target_in_middle);
    RUN_TEST(test_search_target_at_start);
    RUN_TEST(test_search_target_at_end);
    RUN_TEST(test_search_target_not_found);
    RUN_TEST(test_single_element_array_found);
    RUN_TEST(test_single_element_array_not_found);
    RUN_TEST(test_null_array_pointer);
    RUN_TEST(test_empty_array_size);
    RUN_TEST(test_even_sized_array_search);
    RUN_TEST(test_large_array_min_boundary);
    RUN_TEST(test_large_array_max_boundary);
    RUN_TEST(test_target_smaller_than_minimum);
    RUN_TEST(test_target_larger_than_maximum);
    RUN_TEST(test_array_with_negative_numbers);
    RUN_TEST(test_all_identical_elements);

    // Destroy the session.
    return UNITY_END();
}