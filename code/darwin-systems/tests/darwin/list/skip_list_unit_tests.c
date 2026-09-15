/**
 * @file        :   skip_list_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit tests for the probabilistic Skip List data structure.
 * @details     :   Validates boundary limits, state updates, lookups, initialization 
 * state configurations, and clean up operations using the Unity Test Framework.
 * Compile:
 * clang ./tests/darwin/list/skip_list_unit_tests.c ./src/darwin/list/skip-list/skip_list.c ./src/unity.c -Iinclude -o ./bin/skip_list_unit_test
 * @version     :   1.0
 * @date        :   2026-05-13
 */
#include <stdio.h>

#include "unity.h"
#include "darwin/list/skip_list.h"

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
 * @brief Verifies that initialize creates a valid pointer with zeroed initial level state.
 */
void test_initialize_creates_non_null_list_with_zero_level(void)
{
    // Arrange.

    // Act.
    SkipList* list = initialize();

    // Assert.
    TEST_ASSERT_NOT_NULL(list);
    TEST_ASSERT_EQUAL_INT(0, list->level);

    clean(list);
}

/**
 * @brief Verifies that insert handles a single normal entry insertion successfully.
 */
void test_insert_single_node_returns_true(void)
{
    // Arrange.
    SkipList* list = initialize();

    // Act.
    bool result = insert(list, 10, 100);

    // Assert.
    TEST_ASSERT_TRUE(result);

    clean(list);
}

/**
 * @brief Verifies that insert on a null pointer handled gracefully and returns false.
 */
void test_insert_null_list_returns_false(void)
{
    // Arrange.

    // Act.
    bool result = insert(NULL, 10, 100);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief Verifies that searching for an existing key successfully extracts its value.
 */
void test_search_existing_key_returns_true_and_extracts_value(void)
{
    // Arrange.
    SkipList* list = initialize();

    insert(list, 42, 420);

    int extracted_value = 0;

    // Act.
    bool found = search(list, 42, &extracted_value);

    // Assert.
    TEST_ASSERT_TRUE(found);
    TEST_ASSERT_EQUAL_INT(420, extracted_value);

    clean(list);
}

/**
 * @brief Verifies that searching for a non-existent key returns false and leaves value untouched.
 */
void test_search_non_existent_key_returns_false(void)
{
    // Arrange.
    SkipList* list = initialize();

    insert(list, 10, 100);

    int extracted_value = -999;

    // Act.
    bool found = search(list, 99, &extracted_value);

    // Assert.
    TEST_ASSERT_FALSE(found);
    TEST_ASSERT_EQUAL_INT(-999, extracted_value);

    clean(list);
}

/**
 * @brief Verifies that searching with a null output pointer argument returns false gracefully.
 */
void test_search_null_value_pointer_returns_false(void)
{
    // Arrange.
    SkipList* list = initialize();

    insert(list, 10, 100);

    // Act.
    bool found = search(list, 10, NULL);

    // Assert.
    TEST_ASSERT_FALSE(found);

    clean(list);
}

/**
 * @brief Verifies that updating an existing key updates its payload value rather than duplicating.
 */
void test_insert_existing_key_updates_payload_value(void)
{
    // Arrange.
    SkipList* list = initialize();

    insert(list, 15, 150);

    int extracted_value = 0;

    // Act.
    bool update_status = insert(list, 15, 999);
    bool search_status = search(list, 15, &extracted_value);

    // Assert.
    TEST_ASSERT_TRUE(update_status);
    TEST_ASSERT_TRUE(search_status);
    TEST_ASSERT_EQUAL_INT(999, extracted_value);

    clean(list);
}

/**
 * @brief Verifies that continuous ordered insertions maintain lookups across all items.
 */
void test_insert_multiple_ordered_keys_retains_all_values(void)
{
    // Arrange.
    SkipList* list = initialize();

    int extracted_value_first = 0;
    int extracted_value_second = 0;
    int extracted_value_third = 0;

    // Act.
    insert(list, 1, 10);
    insert(list, 2, 20);
    insert(list, 3, 30);

    // Assert.
    TEST_ASSERT_TRUE(search(list, 1, &extracted_value_first));
    TEST_ASSERT_TRUE(search(list, 2, &extracted_value_second));
    TEST_ASSERT_TRUE(search(list, 3, &extracted_value_third));
    TEST_ASSERT_EQUAL_INT(10, extracted_value_first);
    TEST_ASSERT_EQUAL_INT(20, extracted_value_second);
    TEST_ASSERT_EQUAL_INT(30, extracted_value_third);

    clean(list);
}

/**
 * @brief Verifies that reverse ordered insertions maintain accurate lookups and structures.
 */
void test_insert_multiple_reverse_ordered_keys_retains_all_values(void)
{
    // Arrange.
    SkipList* list = initialize();

    int extracted_value_first = 0;
    int extracted_value_second = 0;

    // Act.
    insert(list, 50, 500);
    insert(list, 40, 400);

    // Assert.
    TEST_ASSERT_TRUE(search(list, 50, &extracted_value_first));
    TEST_ASSERT_TRUE(search(list, 40, &extracted_value_second));
    TEST_ASSERT_EQUAL_INT(500, extracted_value_first);
    TEST_ASSERT_EQUAL_INT(400, extracted_value_second);

    clean(list);
}

/**
 * @brief Verifies that negative integer keys are inserted and queried correctly.
 */
void test_insert_negative_keys_handles_correctly(void)
{
    // Arrange.
    SkipList* list = initialize();

    int extracted_value = 0;

    // Act.
    bool insert_status = insert(list, -100, 88);
    bool search_status = search(list, -100, &extracted_value);

    // Assert.
    TEST_ASSERT_TRUE(insert_status);
    TEST_ASSERT_TRUE(search_status);
    TEST_ASSERT_EQUAL_INT(88, extracted_value);

    clean(list);
}

/**
 * @brief Verifies that zero can be leveraged as a standard functional key identifier.
 */
void test_insert_zero_key_handles_correctly(void)
{
    // Arrange.
    SkipList* list = initialize();

    int extracted_value = 0;

    // Act.
    bool insert_status = insert(list, 0, 77);
    bool search_status = search(list, 0, &extracted_value);

    // Assert.
    TEST_ASSERT_TRUE(insert_status);
    TEST_ASSERT_TRUE(search_status);
    TEST_ASSERT_EQUAL_INT(77, extracted_value);

    clean(list);
}

/**
 * @brief Verifies that keys near extreme ceiling boundaries are managed securely.
 */
void test_insert_maximum_integer_key_handles_correctly(void)
{
    // Arrange.
    SkipList* list = initialize();

    int extreme_key = INT_MAX - 1;
    int extracted_value = 0;

    // Act.
    bool insert_status = insert(list, extreme_key, 66);
    bool search_status = search(list, extreme_key, &extracted_value);

    // Assert.
    TEST_ASSERT_TRUE(insert_status);
    TEST_ASSERT_TRUE(search_status);
    TEST_ASSERT_EQUAL_INT(66, extracted_value);

    clean(list);
}

/**
 * @brief Verifies that keys near extreme floor boundaries are managed securely.
 */
void test_insert_minimum_integer_key_handles_correctly(void)
{
    // Arrange.
    SkipList* list = initialize();

    int extreme_key = INT_MIN + 1;
    int extracted_value = 0;

    // Act.
    bool insert_status = insert(list, extreme_key, 55);
    bool search_status = search(list, extreme_key, &extracted_value);

    // Assert.
    TEST_ASSERT_TRUE(insert_status);
    TEST_ASSERT_TRUE(search_status);
    TEST_ASSERT_EQUAL_INT(55, extracted_value);

    clean(list);
}

/**
 * @brief Verifies that construct fails and returns null if maximum level limits are violated.
 */
void test_construct_exceeding_maximum_level_returns_null(void)
{
    // Arrange.

    // Act.
    Node* dynamic_node = construct(10, 100, MAXIMUM_SKIP_LIST_LEVEL + 1);

    // Assert.
    TEST_ASSERT_NULL(dynamic_node);
}

/**
 * @brief Verifies that display executes safely without crashing given an active populated list.
 */
void test_display_executes_safely_on_populated_list(void)
{
    // Arrange.
    SkipList* list = initialize();

    insert(list, 5, 50);

    // Act & Assert.
    display(list);

    clean(list);
}

/**
 * @brief Verifies that display returns early when given a null list context wrapper pointer.
 */
void test_display_handles_null_list_safely(void)
{
    // Arrange.

    // Act & Assert.
    display(NULL);
}

/**
 * @brief Verifies that clean can process a completely null list tracking argument pointer safely.
 */
void test_clean_handles_null_list_safely(void)
{
    // Arrange.

    // Act & Assert.
    clean(NULL);
}

/**
 * @brief Verifies that search on a completely empty list fails cleanly rather than dereferencing errors.
 */
void test_search_empty_list_returns_false(void)
{
    // Arrange.
    SkipList* list = initialize();

    int extracted_value = 0;

    // Act.
    bool found = search(list, 10, &extracted_value);

    // Assert.
    TEST_ASSERT_FALSE(found);

    clean(list);
}

/**
 * @brief Verifies that searching on a null list instance handle returns false.
 */
void test_search_null_list_returns_false(void)
{
    // Arrange.
    int extracted_value = 0;

    // Act.
    bool found = search(NULL, 10, &extracted_value);

    // Assert.
    TEST_ASSERT_FALSE(found);
}

/**
 * @brief Verifies large volume structural handling through continuous pseudo-random insertions.
 */
void test_large_volume_insertions_and_lookups(void)
{
    // Arrange.
    SkipList* list = initialize();

    int verification_value = 0;

    // Act.
    for (int increment_counter = 1; increment_counter <= 100; increment_counter++)
    {
        insert(list, increment_counter, increment_counter * 10);
    }

    // Assert.
    bool final_lookup_status = search(list, 50, &verification_value);

    TEST_ASSERT_TRUE(final_lookup_status);
    TEST_ASSERT_EQUAL_INT(500, verification_value);

    clean(list);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_initialize_creates_non_null_list_with_zero_level);
    RUN_TEST(test_insert_single_node_returns_true);
    RUN_TEST(test_insert_null_list_returns_false);
    RUN_TEST(test_search_existing_key_returns_true_and_extracts_value);
    RUN_TEST(test_search_non_existent_key_returns_false);
    RUN_TEST(test_search_null_value_pointer_returns_false);
    RUN_TEST(test_insert_existing_key_updates_payload_value);
    RUN_TEST(test_insert_multiple_ordered_keys_retains_all_values);
    RUN_TEST(test_insert_multiple_reverse_ordered_keys_retains_all_values);
    RUN_TEST(test_insert_negative_keys_handles_correctly);
    RUN_TEST(test_insert_zero_key_handles_correctly);
    RUN_TEST(test_insert_maximum_integer_key_handles_correctly);
    RUN_TEST(test_insert_minimum_integer_key_handles_correctly);
    RUN_TEST(test_construct_exceeding_maximum_level_returns_null);
    RUN_TEST(test_display_executes_safely_on_populated_list);
    RUN_TEST(test_display_handles_null_list_safely);
    RUN_TEST(test_clean_handles_null_list_safely);
    RUN_TEST(test_search_empty_list_returns_false);
    RUN_TEST(test_search_null_list_returns_false);
    RUN_TEST(test_large_volume_insertions_and_lookups);

    // Destroy the session.
    return UNITY_END();
}