/**
 * @file        :   interpolation_search_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit tests for the interpolation search algorithm.
 * @details     :   This suite utilizes the Unity Test Framework to validate the 
 * robustness, edge cases, and performance of the interpolation 
 * search implementation.
 * Compile:
 * clang ./tests/darwin/searching/interpolation_search_unit_tests.c ./src/darwin/searching/interpolation-search/interpolation_search.c ./src/unity.c -Iinclude -o ./bin/interpolation_search_unit_test
 * @version     :   1.0
 * @date        :   2026-04-16
 */

#include "unity.h"
#include "darwin/searching/interpolation_search.h"

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
 * @brief Validates that the search successfully locates an element in a perfectly uniform array.
 */
void test_search_finds_element_in_uniform_array(void)
{
    // Arrange.
    int array[] = {10, 20, 30, 40, 50};

    size_t size = 5;
    size_t index = 0;

    int key = 40;

    // Act.
    int result = interpolation_search(array, size, key, &index);

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_INT(3, index);
}

/**
 * @brief Ensures the function returns the correct error code when a key is not present in the data.
 */
void test_search_returns_not_found_for_missing_element(void)
{
    // Arrange.
    int array[] = {10, 20, 30, 40, 50};

    size_t size = 5;
    size_t index = 0;

    int key = 25;

    // Act.
    int result = interpolation_search(array, size, key, &index);

    // Assert.
    TEST_ASSERT_EQUAL_INT(-2, result);
}

/**
 * @brief Verifies that the function handles a NULL array pointer without crashing.
 */
void test_search_handles_null_array_pointer(void)
{
    // Arrange.
    size_t index = 0;

    int key = 10;

    // Act.
    int result = interpolation_search(NULL, 10, key, &index);

    // Assert.
    TEST_ASSERT_EQUAL_INT(-1, result);
}

/**
 * @brief Checks that searching an array with a size of zero is handled as a "not found" state.
 */
void test_search_handles_empty_array(void)
{
    // Arrange.
    int array[1] = {0};

    size_t index = 0;

    int key = 10;

    // Act.
    int result = interpolation_search(array, 0, key, &index);

    // Assert.
    TEST_ASSERT_EQUAL_INT(-2, result);
}

/**
 * @brief Confirms that the search can correctly identify the element at the very beginning of the array.
 */
void test_search_finds_first_element(void)
{
    // Arrange.
    int array[] = {1, 2, 3, 4, 5};
    size_t index = 0;

    // Act.
    int result = interpolation_search(array, 5, 1, &index);

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_INT(0, index);
}

/**
 * @brief Confirms that the search can correctly identify the element at the very end of the array.
 */
void test_search_finds_last_element(void)
{
    // Arrange.
    int array[] = {1, 2, 3, 4, 5};
    size_t index = 0;

    // Act.
    int result = interpolation_search(array, 5, 5, &index);

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_INT(4, index);
}

/**
 * @brief Tests the algorithm's behavior when searching a single-element array for a matching value.
 */
void test_search_handles_single_element_array_match(void)
{
    // Arrange.
    int array[] = {42};
    size_t index = 0;

    // Act.
    int result = interpolation_search(array, 1, 42, &index);

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_INT(0, index);
}

/**
 * @brief Tests the algorithm's behavior when searching a single-element array for a non-matching value.
 */
void test_search_handles_single_element_array_mismatch(void)
{
    // Arrange.
    int array[] = {42};
    size_t index = 0;

    // Act.
    int result = interpolation_search(array, 1, 7, &index);

    // Assert.
    TEST_ASSERT_EQUAL_INT(-2, result);
}

/**
 * @brief Validates that the search correctly handles ranges where all elements are identical and match the key.
 */
void test_search_handles_all_identical_elements_match(void)
{
    // Arrange.
    int array[] = {10, 10, 10, 10, 10};
    size_t index = 0;

    // Act.
    int result = interpolation_search(array, 5, 10, &index);

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_INT(0, index);
}

/**
 * @brief Validates that the search correctly handles ranges where all elements are identical but do not match the key.
 */
void test_search_handles_all_identical_elements_mismatch(void)
{
    // Arrange.
    int array[] = {10, 10, 10, 10, 10};
    size_t index = 0;

    // Act.
    int result = interpolation_search(array, 5, 20, &index);

    // Assert.
    TEST_ASSERT_EQUAL_INT(-2, result);
}

/**
 * @brief Ensures the search terminates correctly when the target key is lower than the smallest value in the array.
 */
void test_search_key_below_minimum_range(void)
{
    // Arrange.
    int array[] = {100, 200, 300};
    size_t index = 0;

    // Act.
    int result = interpolation_search(array, 3, 50, &index);

    // Assert.
    TEST_ASSERT_EQUAL_INT(-2, result);
}

/**
 * @brief Ensures the search terminates correctly when the target key is higher than the largest value in the array.
 */
void test_search_key_above_maximum_range(void)
{
    // Arrange.
    int array[] = {100, 200, 300};
    size_t index = 0;

    // Act.
    int result = interpolation_search(array, 3, 400, &index);

    // Assert.
    TEST_ASSERT_EQUAL_INT(-2, result);
}

/**
 * @brief Tests the precision and scaling of the interpolation formula over a larger dataset.
 */
void test_search_works_with_large_uniform_range(void)
{
    // Arrange.
    int array[100];

    for(int index = 0; index < 100; index++)
    {
        array[index] = index * 2;
    }

    size_t index = 0;

    // Act.
    int result = interpolation_search(array, 100, 150, &index);

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_INT(75, index);
}

/**
 * @brief Verifies that the function operates correctly even when the user does not provide a pointer for the output index.
 */
void test_search_handles_null_output_index_gracefully(void)
{
    // Arrange.
    int array[] = {1, 2, 3};

    // Act.
    int result = interpolation_search(array, 3, 2, NULL);

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, result);
}

/**
 * @brief Confirms the algorithm still functions correctly on sorted data that is not perfectly uniform.
 */
void test_search_handles_non_uniform_distribution(void)
{
    // Arrange.
    int array[] = {1, 2, 10, 100, 1000};
    size_t index = 0;

    // Act.
    int result = interpolation_search(array, 5, 100, &index);

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_INT(3, index);
}

/**
 * @brief Main execution point for the unit test suite.
 */
int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_search_finds_element_in_uniform_array);
    RUN_TEST(test_search_returns_not_found_for_missing_element);
    RUN_TEST(test_search_handles_null_array_pointer);
    RUN_TEST(test_search_handles_empty_array);
    RUN_TEST(test_search_finds_first_element);
    RUN_TEST(test_search_finds_last_element);
    RUN_TEST(test_search_handles_single_element_array_match);
    RUN_TEST(test_search_handles_single_element_array_mismatch);
    RUN_TEST(test_search_handles_all_identical_elements_match);
    RUN_TEST(test_search_handles_all_identical_elements_mismatch);
    RUN_TEST(test_search_key_below_minimum_range);
    RUN_TEST(test_search_key_above_maximum_range);
    RUN_TEST(test_search_works_with_large_uniform_range);
    RUN_TEST(test_search_handles_null_output_index_gracefully);
    RUN_TEST(test_search_handles_non_uniform_distribution);

    // Destroy the session.
    return UNITY_END();
}