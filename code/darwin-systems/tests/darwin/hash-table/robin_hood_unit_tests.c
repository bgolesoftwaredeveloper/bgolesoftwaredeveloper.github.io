/**
 * @file        :   robin_hood_unit_test.c
 * @author      :   Braiden Gole
 * @brief       :   Unit tests for a single-threaded Robin Hood hash table.
 * @details     :   Provides comprehensive validation of the hash table API using the 
 * Unity test framework. Test cases verify initialization, dynamic resizing, 
 * probe sequence length stabilization, backwards-shift element erasure, 
 * and complete memory lifecycle management.
 * Compile:
 * clang ./tests/darwin/hash-table/robin_hood_unit_tests.c ./src/darwin/hash-table/robin-hood/robin_hood.c ./src/unity.c -Iinclude -o ./bin/robin_hood_unit_test
 * @version     :   1.0
 * @date        :   2026-05-26
 */
#include <stdio.h>

#include "unity.h"
#include "darwin/hash-table/robin_hood.h"

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
 * @brief Verifies that initialize_map correctly creates a non-null hash table container.
 */
void test_initialize_map_returns_non_null_pointer(void)
{
    // Arrange
    RobinHood* test_map = NULL;

    // Act
    test_map = initialize_map();

    // Assert
    TEST_ASSERT_NOT_NULL(test_map);
    TEST_ASSERT_EQUAL_INT(0, test_map->size);
    TEST_ASSERT_EQUAL_INT(INITIAL_CAPACITY, test_map->capacity);

    clean(test_map);
}

/**
 * @brief Verifies that putting an item into a null map pointer returns false.
 */
void test_put_with_null_map_returns_false(void)
{
    // Arrange
    const char* unique_key = "test_key";

    int value_payload = 42;

    // Act
    bool operation_result = put(NULL, unique_key, value_payload);

    // Assert
    TEST_ASSERT_FALSE(operation_result);
}

/**
 * @brief Verifies that putting an item with a null key string returns false.
 */
void test_put_with_null_key_returns_false(void)
{
    // Arrange
    RobinHood* test_map = initialize_map();

    int value_payload = 42;

    // Act
    bool operation_result = put(test_map, NULL, value_payload);

    // Assert
    TEST_ASSERT_FALSE(operation_result);

    clean(test_map);
}

/**
 * @brief Verifies that putting a unique key-value pair returns true and increases size.
 */
void test_put_successful_entry_returns_true(void)
{
    // Arrange
    RobinHood* test_map = initialize_map();

    const char* unique_key = "unique_identifier";
    int value_payload = 100;

    // Act
    bool operation_result = put(test_map, unique_key, value_payload);

    // Assert
    TEST_ASSERT_TRUE(operation_result);
    TEST_ASSERT_EQUAL_INT(1, test_map->size);

    clean(test_map);
}

/**
 * @brief Verifies that get on a null map pointer returns false.
 */
void test_get_with_null_map_returns_false(void)
{
    // Arrange
    const char* unique_key = "search_key";
    int output_buffer = 0;

    // Act
    bool operation_result = get(NULL, unique_key, &output_buffer);

    // Assert
    TEST_ASSERT_FALSE(operation_result);
}

/**
 * @brief Verifies that get with a null key string returns false.
 */
void test_get_with_null_key_returns_false(void)
{
    // Arrange
    RobinHood* test_map = initialize_map();

    int output_buffer = 0;

    // Act
    bool operation_result = get(test_map, NULL, &output_buffer);

    // Assert
    TEST_ASSERT_FALSE(operation_result);

    clean(test_map);
}

/**
 * @brief Verifies that get with a null destination output pointer returns false.
 */
void test_get_with_null_output_value_pointer_returns_false(void)
{
    // Arrange
    RobinHood* test_map = initialize_map();

    const char* unique_key = "valid_key";

    put(test_map, unique_key, 500);

    // Act
    bool operation_result = get(test_map, unique_key, NULL);

    // Assert
    TEST_ASSERT_FALSE(operation_result);

    clean(test_map);
}

/**
 * @brief Verifies that get retrieves the exact integer value that was inserted.
 */
void test_get_retrieves_correct_inserted_value(void)
{
    // Arrange
    RobinHood* test_map = initialize_map();

    const char* unique_key = "retrieval_key";
    int expected_value = 999;

    put(test_map, unique_key, expected_value);

    int actual_value = 0;

    // Act
    bool lookup_result = get(test_map, unique_key, &actual_value);

    // Assert
    TEST_ASSERT_TRUE(lookup_result);
    TEST_ASSERT_EQUAL_INT(expected_value, actual_value);

    clean(test_map);
}

/**
 * @brief Verifies that looking up a non-existent key returns false.
 */
void test_get_non_existent_key_returns_false(void)
{
    // Arrange
    RobinHood* test_map = initialize_map();

    const char* missing_key = "does_not_exist";
    int output_buffer = -1;

    // Act
    bool lookup_result = get(test_map, missing_key, &output_buffer);

    // Assert
    TEST_ASSERT_FALSE(lookup_result);
    TEST_ASSERT_EQUAL_INT(-1, output_buffer);

    clean(test_map);
}

/**
 * @brief Verifies that putting a key that already exists updates its value without scaling size.
 */
void test_put_duplicate_key_updates_existing_value(void)
{
    // Arrange
    RobinHood* test_map = initialize_map();

    const char* configuration_key = "shared_key";

    int initial_value = 111;
    int updated_value = 222;

    put(test_map, configuration_key, initial_value);

    // Act
    bool update_result = put(test_map, configuration_key, updated_value);
    
    int current_value = 0;

    get(test_map, configuration_key, &current_value);

    // Assert
    TEST_ASSERT_TRUE(update_result);
    TEST_ASSERT_EQUAL_INT(updated_value, current_value);
    TEST_ASSERT_EQUAL_INT(1, test_map->size);

    clean(test_map);
}

/**
 * @brief Verifies that erasing an entry from a null map pointer returns false.
 */
void test_erase_with_null_map_returns_false(void)
{
    // Arrange
    const char* target_key = "deletion_key";

    // Act
    bool operation_result = erase(NULL, target_key);

    // Assert
    TEST_ASSERT_FALSE(operation_result);
}

/**
 * @brief Verifies that erasing a null key reference string returns false.
 */
void test_erase_with_null_key_returns_false(void)
{
    // Arrange
    RobinHood* test_map = initialize_map();

    // Act
    bool operation_result = erase(test_map, NULL);

    // Assert
    TEST_ASSERT_FALSE(operation_result);

    clean(test_map);
}

/**
 * @brief Verifies that erasing an existing key removes it, shifts appropriately, and reduces size.
 */
void test_erase_existing_key_returns_true_and_removes_node(void)
{
    // Arrange
    RobinHood* test_map = initialize_map();

    const char* volatile_key = "remove_me";

    put(test_map, volatile_key, 55);

    int output_buffer = 0;

    // Act
    bool removal_result = erase(test_map, volatile_key);
    bool subsequent_lookup = get(test_map, volatile_key, &output_buffer);

    // Assert
    TEST_ASSERT_TRUE(removal_result);
    TEST_ASSERT_FALSE(subsequent_lookup);
    TEST_ASSERT_EQUAL_INT(0, test_map->size);

    clean(test_map);
}

/**
 * @brief Verifies that attempting to erase a non-existent key returns false.
 */
void test_erase_non_existent_key_returns_false(void)
{
    // Arrange
    RobinHood* test_map = initialize_map();

    const char* missing_key = "not_in_table";

    // Act
    bool operation_result = erase(test_map, missing_key);

    // Assert
    TEST_ASSERT_FALSE(operation_result);

    clean(test_map);
}

/**
 * @brief Verifies that clean functions safely when handed a null map pointer reference.
 */
void test_clean_with_null_map_handles_gracefully(void)
{
    // Arrange
    RobinHood* null_map_reference = NULL;

    // Act & Assert
    clean(null_map_reference);
}

/**
 * @brief Verifies that display functions safely when handed a null map pointer reference.
 */
void test_display_with_null_map_handles_gracefully(void)
{
    // Arrange
    RobinHood* null_map_reference = NULL;

    // Act & Assert
    display(null_map_reference);
}

/**
 * @brief Verifies that multiple distinct keys can be inserted and retrieved safely.
 */
void test_multiple_unique_insertions_persist_independently(void)
{
    // Arrange
    RobinHood* test_map = initialize_map();

    const char* first_key = "alpha";
    const char* second_key = "beta";

    int first_value = 701;
    int second_value = 702;

    // Act
    put(test_map, first_key, first_value);
    put(test_map, second_key, second_value);

    int output_alpha = 0;
    int output_beta = 0;

    get(test_map, first_key, &output_alpha);
    get(test_map, second_key, &output_beta);

    // Assert
    TEST_ASSERT_EQUAL_INT(first_value, output_alpha);
    TEST_ASSERT_EQUAL_INT(second_value, output_beta);
    TEST_ASSERT_EQUAL_INT(2, test_map->size);

    clean(test_map);
}

/**
 * @brief Verifies that display handles an empty table without hitting operational limits.
 */
void test_display_empty_table_executes_without_errors(void)
{
    // Arrange
    RobinHood* test_map = initialize_map();

    // Act & Assert
    display(test_map);

    clean(test_map);
}

/**
 * @brief Verifies that entry count stability remains solid when passing large tracking string keys.
 */
void test_put_with_extremely_long_key_string_succeeds(void)
{
    // Arrange
    RobinHood* test_map = initialize_map();

    char standard_long_key[256];

    for (size_t character_index = 0; character_index < 255; character_index++)
    {
        standard_long_key[character_index] = 'A';
    }

    standard_long_key[255] = '\0';

    int expected_payload = 8888;

    // Act
    bool insertion_result = put(test_map, standard_long_key, expected_payload);
    
    int actual_value = 0;

    get(test_map, standard_long_key, &actual_value);

    // Assert
    TEST_ASSERT_TRUE(insertion_result);
    TEST_ASSERT_EQUAL_INT(expected_payload, actual_value);

    clean(test_map);
}

/**
 * @brief Verifies that continuous operations on empty string keys compute valid bounds.
 */
void test_put_with_empty_string_key_succeeds(void)
{
    // Arrange
    RobinHood* test_map = initialize_map();

    const char* empty_string_key = "";
    int expected_payload = 12345;

    // Act
    bool insertion_result = put(test_map, empty_string_key, expected_payload);

    int actual_value = 0;

    get(test_map, empty_string_key, &actual_value);

    // Assert
    TEST_ASSERT_TRUE(insertion_result);
    TEST_ASSERT_EQUAL_INT(expected_payload, actual_value);

    clean(test_map);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_initialize_map_returns_non_null_pointer);
    RUN_TEST(test_put_with_null_map_returns_false);
    RUN_TEST(test_put_with_null_key_returns_false);
    RUN_TEST(test_put_successful_entry_returns_true);
    RUN_TEST(test_get_with_null_map_returns_false);
    RUN_TEST(test_get_with_null_key_returns_false);
    RUN_TEST(test_get_with_null_output_value_pointer_returns_false);
    RUN_TEST(test_get_retrieves_correct_inserted_value);
    RUN_TEST(test_get_non_existent_key_returns_false);
    RUN_TEST(test_put_duplicate_key_updates_existing_value);
    RUN_TEST(test_erase_with_null_map_returns_false);
    RUN_TEST(test_erase_with_null_key_returns_false);
    RUN_TEST(test_erase_existing_key_returns_true_and_removes_node);
    RUN_TEST(test_erase_non_existent_key_returns_false);
    RUN_TEST(test_clean_with_null_map_handles_gracefully);
    RUN_TEST(test_display_with_null_map_handles_gracefully);
    RUN_TEST(test_multiple_unique_insertions_persist_independently);
    RUN_TEST(test_display_empty_table_executes_without_errors);
    RUN_TEST(test_put_with_extremely_long_key_string_succeeds);
    RUN_TEST(test_put_with_empty_string_key_succeeds);

    // Destroy the session.
    return UNITY_END();
}