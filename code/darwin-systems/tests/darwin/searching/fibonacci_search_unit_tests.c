/**
 * @file        :   fibonacci_search_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit tests for the Fibonacci search algorithm.
 * @details     :   This suite utilizes the Unity Test Framework to validate the 
 * Fibonacci search implementation, ensuring correct boolean 
 * returns and index assignments across various edge cases.
 * Compile:
 * clang ./tests/darwin/searching/fibonacci_search_unit_tests.c ./src/darwin/searching/fibonacci-search/fibonacci_search.c ./src/unity.c -Iinclude -o ./bin/fibonacci_search_unit_test
 * @version     :   1.0
 * @date        :   2026-04-19
 */
#include <stdio.h>

#include "unity.h"
#include "darwin/searching/fibonacci_search.h"

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
void test_fibonacci_search_middle_element(void)
{
    // Arrange.
    int array[] = {1, 3, 5, 7, 9};
    
    size_t size = 5;
    size_t index = 0;
    
    // Act.
    bool found = fibonacci_search(array, size, 5, &index);
    
    // Assert.
    TEST_ASSERT_TRUE(found);
    TEST_ASSERT_EQUAL_UINT(2, index);
}

/**
 * @brief Test searching for a target at the very first index.
 */
void test_fibonacci_search_start_element(void)
{
    // Arrange.
    int array[] = {10, 20, 30, 40};

    size_t size = 4;
    size_t index = 0;
    
    // Act.
    bool found = fibonacci_search(array, size, 10, &index);
    
    // Assert.
    TEST_ASSERT_TRUE(found);
    TEST_ASSERT_EQUAL_UINT(0, index);
}

/**
 * @brief Test searching for a target at the very last index.
 */
void test_fibonacci_search_end_element(void)
{
    // Arrange.
    int array[] = {10, 20, 30, 40, 50};

    size_t size = 5;
    size_t index = 0;
    
    // Act.
    bool found = fibonacci_search(array, size, 50, &index);
    
    // Assert.
    TEST_ASSERT_TRUE(found);
    TEST_ASSERT_EQUAL_UINT(4, index);
}

/**
 * @brief Test searching in an array where the size is exactly a Fibonacci number.
 */
void test_fibonacci_search_size_is_fib_number(void)
{
    // Arrange.
    int array[] = {1, 2, 3, 4, 5, 6, 7, 8};

    size_t size = 8;
    size_t index = 0;
    
    // Act.
    bool found = fibonacci_search(array, size, 7, &index);
    
    // Assert.
    TEST_ASSERT_TRUE(found);
    TEST_ASSERT_EQUAL_UINT(6, index);
}

/**
 * @brief Test searching in an array where the size is a Fibonacci number plus one.
 */
void test_fibonacci_search_size_is_fib_plus_one(void)
{
    // Arrange.
    int array[] = {10, 20, 30, 40, 50, 60};

    size_t size = 6;
    size_t index = 0;
    
    // Act.
    bool found = fibonacci_search(array, size, 40, &index);
    
    // Assert.
    TEST_ASSERT_TRUE(found);
    TEST_ASSERT_EQUAL_UINT(3, index);
}

/**
 * @brief Test behavior when the target value is missing from the array.
 */
void test_fibonacci_search_missing_element(void)
{
    // Arrange.
    int array[] = {10, 20, 30, 40};

    size_t size = 4;
    size_t index = 0;
    
    // Act.
    bool found = fibonacci_search(array, size, 25, &index);
    
    // Assert.
    TEST_ASSERT_FALSE(found);
}

/**
 * @brief Test searching in a single-element array (element present).
 */
void test_fibonacci_search_single_element_found(void)
{
    // Arrange.
    int array[] = {99};

    size_t size = 1;
    size_t index = 0;
    
    // Act.
    bool found = fibonacci_search(array, size, 99, &index);
    
    // Assert.
    TEST_ASSERT_TRUE(found);
    TEST_ASSERT_EQUAL_UINT(0, index);
}

/**
 * @brief Test searching in a single-element array (element absent).
 */
void test_fibonacci_search_single_element_not_found(void)
{
    // Arrange.
    int array[] = {99};

    size_t size = 1;
    size_t index = 0;
    
    // Act.
    bool found = fibonacci_search(array, size, 100, &index);
    
    // Assert.
    TEST_ASSERT_FALSE(found);
}

/**
 * @brief Test defensive check for a NULL array pointer.
 */
void test_fibonacci_search_null_pointer(void)
{
    // Arrange.
    int* array = NULL;

    size_t index = 0;
    
    // Act.
    bool found = fibonacci_search(array, 10, 5, &index);
    
    // Assert.
    TEST_ASSERT_FALSE(found);
}

/**
 * @brief Test defensive check for size 0 array.
 */
void test_fibonacci_search_empty_array(void)
{
    // Arrange.
    int array[] = {1};

    size_t size = 0;
    size_t index = 0;
    
    // Act.
    bool found = fibonacci_search(array, size, 1, &index);
    
    // Assert.
    TEST_ASSERT_FALSE(found);
}

/**
 * @brief Test search when the key is smaller than the minimum value.
 */
void test_fibonacci_search_key_below_min(void)
{
    // Arrange.
    int array[] = {10, 20, 30};

    size_t index = 0;
    
    // Act.
    bool found = fibonacci_search(array, 3, 5, &index);
    
    // Assert.
    TEST_ASSERT_FALSE(found);
}

/**
 * @brief Test search when the key is larger than the maximum value.
 */
void test_fibonacci_search_key_above_max(void)
{
    // Arrange.
    int array[] = {10, 20, 30};

    size_t index = 0;
    
    // Act.
    bool found = fibonacci_search(array, 3, 45, &index);
    
    // Assert.
    TEST_ASSERT_FALSE(found);
}

/**
 * @brief Test searching an array containing negative integers.
 */
void test_fibonacci_search_negative_values(void)
{
    // Arrange.
    int array[] = {-50, -40, -30, -20, -10};

    size_t size = 5;
    size_t index = 0;
    
    // Act.
    bool found = fibonacci_search(array, size, -40, &index);
    
    // Assert.
    TEST_ASSERT_TRUE(found);
    TEST_ASSERT_EQUAL_UINT(1, index);
}

/**
 * @brief Test searching in a large array to verify Fibonacci step logic.
 */
void test_fibonacci_search_large_array(void)
{
    // Arrange.
    int array[200];

    for (int index = 0; index < 200; index++)
    {
        array[index] = index * 2;
    }
    
    size_t size = 200;
    size_t index = 0;

    int key = 300;
    
    // Act.
    bool found = fibonacci_search(array, size, key, &index);
    
    // Assert.
    TEST_ASSERT_TRUE(found);
    TEST_ASSERT_EQUAL_UINT(150, index);
}

/**
 * @brief Test search behavior when all elements in the array are identical.
 */
void test_fibonacci_search_identical_elements(void)
{
    // Arrange.
    int array[] = {5, 5, 5, 5, 5, 5};

    size_t size = 6;
    size_t index = 0;
    
    // Act.
    bool found = fibonacci_search(array, size, 5, &index);
    
    // Assert.
    TEST_ASSERT_TRUE(found);
    TEST_ASSERT_TRUE(index < size);
}

/**
 * @brief Main entry point for the Fibonacci search unit test suite.
 */
int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_fibonacci_search_middle_element);
    RUN_TEST(test_fibonacci_search_start_element);
    RUN_TEST(test_fibonacci_search_end_element);
    RUN_TEST(test_fibonacci_search_size_is_fib_number);
    RUN_TEST(test_fibonacci_search_size_is_fib_plus_one);
    RUN_TEST(test_fibonacci_search_missing_element);
    RUN_TEST(test_fibonacci_search_single_element_found);
    RUN_TEST(test_fibonacci_search_single_element_not_found);
    RUN_TEST(test_fibonacci_search_null_pointer);
    RUN_TEST(test_fibonacci_search_empty_array);
    RUN_TEST(test_fibonacci_search_key_below_min);
    RUN_TEST(test_fibonacci_search_key_above_max);
    RUN_TEST(test_fibonacci_search_negative_values);
    RUN_TEST(test_fibonacci_search_large_array);
    RUN_TEST(test_fibonacci_search_identical_elements);

    // Destroy the session.
    return UNITY_END();
}