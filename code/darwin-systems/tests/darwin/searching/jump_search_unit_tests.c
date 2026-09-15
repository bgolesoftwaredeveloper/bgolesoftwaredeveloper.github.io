/**
 * @file        :   jump_search_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit tests for the jump search algorithm.
 * @details     :   This suite utilizes the Unity Test Framework to validate the 
 * jump search implementation across various edge cases and standard scenarios.
 * Compile:
 * clang ./tests/darwin/searching/jump_search_unit_tests.c ./src/darwin/searching/jump-search/jump_search.c ./src/unity.c -Iinclude -o ./bin/jump_search_unit_test
 * @version     :   1.0
 * @date        :   2026-04-14
 */

#include "unity.h"
#include "darwin/searching/jump_search.h"

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
 * @brief Validates finding a value in the middle of a standard array.
 */
void test_find_middle_element(void)
{
    // Arrange.
    int array[] = {10, 20, 30, 40, 50};

    // Act & Assert.
    TEST_ASSERT_EQUAL_INT(2, jump_search(array, 5, 30));
}

/**
 * @brief Validates finding the first element of an array.
 */
void test_find_first_element(void)
{
    // Arrange.
    int array[] = {10, 20, 30, 40, 50};

    // Act & Assert.
    TEST_ASSERT_EQUAL_INT(0, jump_search(array, 5, 10));
}

/**
 * @brief Validates finding the last element of an array.
 */
void test_find_last_element(void)
{
    // Arrange.
    int array[] = {10, 20, 30, 40, 50};

    // Act & Assert.
    TEST_ASSERT_EQUAL_INT(4, jump_search(array, 5, 50));
}

/**
 * @brief Validates searching for a value that does not exist in the array.
 */
void test_element_not_found(void)
{
    // Arrange.
    int array[] = {10, 20, 30, 40, 50};

    // Act & Assert.
    TEST_ASSERT_EQUAL_INT(-1, jump_search(array, 5, 25));
}

/**
 * @brief Validates behavior when searching an empty array.
 */
void test_empty_array(void)
{
    // Arrange.
    int* array = NULL;

    // Act & Assert.
    TEST_ASSERT_EQUAL_INT(-1, jump_search(array, 0, 10));
}

/**
 * @brief Validates finding a single element in a one-element array.
 */
void test_single_element_array_found(void)
{
    // Arrange.
    int array[] = {100};

    // Act & Assert.
    TEST_ASSERT_EQUAL_INT(0, jump_search(array, 1, 100));
}

/**
 * @brief Validates searching for a missing element in a one-element array.
 */
void test_single_element_array_not_found(void)
{
    // Arrange.
    int array[] = {100};

    // Act & Assert.
    TEST_ASSERT_EQUAL_INT(-1, jump_search(array, 1, 50));
}

/**
 * @brief Validates behavior when the target is smaller than the smallest element.
 */
void test_target_less_than_minimum(void)
{
    // Arrange.
    int array[] = {10, 20, 30};

    // Act & Assert.
    TEST_ASSERT_EQUAL_INT(-1, jump_search(array, 3, 5));
}

/**
 * @brief Validates behavior when the target is larger than the largest element.
 */
void test_target_greater_than_maximum(void)
{
    // Arrange.
    int array[] = {10, 20, 30};

    // Act & Assert.
    TEST_ASSERT_EQUAL_INT(-1, jump_search(array, 3, 40));
}

/**
 * @brief Validates finding an element in a large array.
 */
void test_large_array_search(void)
{
    // Arrange.
    int array[100];

    for (int index = 0; index < 100; index++) 
    {
        array[index] = index * 2;
    }

    // Act & Assert.
    TEST_ASSERT_EQUAL_INT(45, jump_search(array, 100, 90));
}

/**
 * @brief Validates search accuracy in an array with duplicate values.
 */
void test_array_with_duplicates(void)
{
    // Arrange.
    int array[] = {10, 20, 20, 20, 30};

    // Act.
    int result = jump_search(array, 5, 20);

    // Assert.
    TEST_ASSERT_TRUE(result >= 1 && result <= 3);
}

/**
 * @brief Validates search accuracy in an array with negative values.
 */
void test_negative_values_search(void)
{
    // Arrange.
    int array[] = {-50, -25, 0, 25, 50};

    // Act & Assert.
    TEST_ASSERT_EQUAL_INT(1, jump_search(array, 5, -25));
}

/**
 * @brief Validates search when the target is exactly on a jump boundary.
 */
void test_target_on_jump_boundary(void)
{
    // Arrange.
    int array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    // Act & Assert.
    TEST_ASSERT_EQUAL_INT(2, jump_search(array, 9, 3));
}

/**
 * @brief Validates search just before a jump boundary.
 */
void test_target_before_jump_boundary(void)
{
    // Arrange.
    int array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    // Act & Assert.
    TEST_ASSERT_EQUAL_INT(1, jump_search(array, 9, 2));
}

/**
 * @brief Validates search just after a jump boundary.
 */
void test_target_after_jump_boundary(void)
{
    // Arrange.
    int array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    // Act & Assert.
    TEST_ASSERT_EQUAL_INT(3, jump_search(array, 9, 4));
}

/**
 * @brief Main entry point for the unit test execution.
 */
int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_find_middle_element);
    RUN_TEST(test_find_first_element);
    RUN_TEST(test_find_last_element);
    RUN_TEST(test_element_not_found);
    RUN_TEST(test_empty_array);
    RUN_TEST(test_single_element_array_found);
    RUN_TEST(test_single_element_array_not_found);
    RUN_TEST(test_target_less_than_minimum);
    RUN_TEST(test_target_greater_than_maximum);
    RUN_TEST(test_large_array_search);
    RUN_TEST(test_array_with_duplicates);
    RUN_TEST(test_negative_values_search);
    RUN_TEST(test_target_on_jump_boundary);
    RUN_TEST(test_target_before_jump_boundary);
    RUN_TEST(test_target_after_jump_boundary);

    // Destroy the session.
    return UNITY_END();
}