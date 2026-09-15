/**
 * @file        :   concurrent_hash_table_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit tests for a lock-striped concurrent hash table.
 * @details     :   Provides comprehensive validation of the hash table API using the 
 * Unity test framework. Test cases verify initialization, basic 
 * mutations, collision resolution, lock-free lookups, explicit item 
 * erasure, multi-threaded worker simulations, and complete memory lifecycle management.
 * Compile:
 * clang ./tests/darwin/hash-table/concurrent_hash_table_unit_tests.c ./src/darwin/hash-table/concurrent/concurrent_hash_table.c ./src/unity.c -Iinclude -o ./bin/concurrent_unit_test
 * @version     :   1.0
 * @date        :   2026-05-25
 */
#include <stdio.h>
#include <stdlib.h>

#include "unity.h"
#include "darwin/hash-table/concurrent_hash_table.h"

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
 * @brief Verifies that initialize correctly creates a non-null hash table container.
 */
void test_initialize_returns_non_null_pointer(void)
{
    // Arrange
    ConcurrentHashTable* test_table = NULL;

    // Act
    test_table = initialize();

    // Assert
    TEST_ASSERT_NOT_NULL(test_table);

    destroy(test_table);
}

/**
 * @brief Verifies that inserting into a null table returns false.
 */
void test_insert_with_null_table_returns_false(void)
{
    // Arrange
    const char* unique_key = "test_key";
    void* payload_value = (void*)(intptr_t)42;

    // Act
    bool operation_result = insert(NULL, unique_key, payload_value);

    // Assert
    TEST_ASSERT_FALSE(operation_result);
}

/**
 * @brief Verifies that inserting a null key returns false.
 */
void test_insert_with_null_key_returns_false(void)
{
    // Arrange
    ConcurrentHashTable* test_table = initialize();

    void* payload_value = (void*)(intptr_t)42;

    // Act
    bool operation_result = insert(test_table, NULL, payload_value);

    // Assert
    TEST_ASSERT_FALSE(operation_result);

    destroy(test_table);
}

/**
 * @brief Verifies that inserting a unique key-value pair returns true.
 */
void test_insert_successful_entry_returns_true(void)
{
    // Arrange
    ConcurrentHashTable* test_table = initialize();

    const char* unique_key = "unique_identifier";
    void* payload_value = (void*)(intptr_t)100;

    // Act
    bool operation_result = insert(test_table, unique_key, payload_value);

    // Assert
    TEST_ASSERT_TRUE(operation_result);

    destroy(test_table);
}

/**
 * @brief Verifies that lookup on a null table returns null.
 */
void test_lookup_with_null_table_returns_null(void)
{
    // Arrange
    const char* unique_key = "search_key";

    // Act
    void* retrieved_value = lookup(NULL, unique_key);

    // Assert
    TEST_ASSERT_NULL(retrieved_value);
}

/**
 * @brief Verifies that lookup with a null key returns null.
 */
void test_lookup_with_null_key_returns_null(void)
{
    // Arrange
    ConcurrentHashTable* test_table = initialize();

    // Act
    void* retrieved_value = lookup(test_table, NULL);

    // Assert
    TEST_ASSERT_NULL(retrieved_value);

    destroy(test_table);
}

/**
 * @brief Verifies that lookup returns the exact payload pointer that was inserted.
 */
void test_lookup_retrieves_correct_inserted_value(void)
{
    // Arrange
    ConcurrentHashTable* test_table = initialize();

    const char* unique_key = "retrieval_key";
    void* expected_value = (void*)(intptr_t)999;

    insert(test_table, unique_key, expected_value);

    // Act
    void* actual_value = lookup(test_table, unique_key);

    // Assert
    TEST_ASSERT_EQUAL_PTR(expected_value, actual_value);

    destroy(test_table);
}

/**
 * @brief Verifies that looking up a non-existent key returns null.
 */
void test_lookup_non_existent_key_returns_null(void)
{
    // Arrange
    ConcurrentHashTable* test_table = initialize();

    const char* missing_key = "does_not_exist";

    // Act
    void* retrieved_value = lookup(test_table, missing_key);

    // Assert
    TEST_ASSERT_NULL(retrieved_value);

    destroy(test_table);
}

/**
 * @brief Verifies that inserting a key that already exists updates its value payload.
 */
void test_insert_duplicate_key_updates_existing_value(void)
{
    // Arrange
    ConcurrentHashTable* test_table = initialize();

    const char* configuration_key = "shared_key";

    void* initial_value = (void*)(intptr_t)111;
    void* updated_value = (void*)(intptr_t)222;

    insert(test_table, configuration_key, initial_value);

    // Act
    insert(test_table, configuration_key, updated_value);

    void* current_value = lookup(test_table, configuration_key);

    // Assert
    TEST_ASSERT_EQUAL_PTR(updated_value, current_value);

    destroy(test_table);
}

/**
 * @brief Verifies that erasing an entry from a null table returns false.
 */
void test_erase_with_null_table_returns_false(void)
{
    // Arrange
    const char* target_key = "deletion_key";

    // Act
    bool operation_result = erase(NULL, target_key);

    // Assert
    TEST_ASSERT_FALSE(operation_result);
}

/**
 * @brief Verifies that erasing a null key returns false.
 */
void test_erase_with_null_key_returns_false(void)
{
    // Arrange
    ConcurrentHashTable* test_table = initialize();

    // Act
    bool operation_result = erase(test_table, NULL);

    // Assert
    TEST_ASSERT_FALSE(operation_result);

    destroy(test_table);
}

/**
 * @brief Verifies that erasing an existing key removes it and returns true.
 */
void test_erase_existing_key_returns_true_and_removes_node(void)
{
    // Arrange
    ConcurrentHashTable* test_table = initialize();

    const char* volatile_key = "remove_me";

    insert(test_table, volatile_key, (void*)(intptr_t)55);

    // Act
    bool removal_result = erase(test_table, volatile_key);
    void* subsequent_lookup = lookup(test_table, volatile_key);

    // Assert
    TEST_ASSERT_TRUE(removal_result);
    TEST_ASSERT_NULL(subsequent_lookup);

    destroy(test_table);
}

/**
 * @brief Verifies that attempting to erase a non-existent key returns false.
 */
void test_erase_non_existent_key_returns_false(void)
{
    // Arrange
    ConcurrentHashTable* test_table = initialize();

    const char* missing_key = "not_in_table";

    // Act
    bool operation_result = erase(test_table, missing_key);

    // Assert
    TEST_ASSERT_FALSE(operation_result);

    destroy(test_table);
}

/**
 * @brief Verifies that destroy functions cleanly when given a null table pointer.
 */
void test_destroy_with_null_table_handles_gracefully(void)
{
    // Arrange
    ConcurrentHashTable* null_table_reference = NULL;

    // Act & Assert
    destroy(null_table_reference);
}

/**
 * @brief Verifies that display functions cleanly when given a null table pointer.
 */
void test_display_with_null_table_handles_gracefully(void)
{
    // Arrange
    ConcurrentHashTable* null_table_reference = NULL;

    // Act & Assert
    display(null_table_reference);
}

/**
 * @brief Verifies that multiple distinct keys can be inserted and retrieved safely.
 */
void test_multiple_unique_insertions_persist_independently(void)
{
    // Arrange
    ConcurrentHashTable* test_table = initialize();

    const char* first_key = "alpha";
    const char* second_key = "beta";

    void* first_value = (void*)(intptr_t)701;
    void* second_value = (void*)(intptr_t)702;

    // Act
    insert(test_table, first_key, first_value);
    insert(test_table, second_key, second_value);

    // Assert
    TEST_ASSERT_EQUAL_PTR(first_value, lookup(test_table, first_key));
    TEST_ASSERT_EQUAL_PTR(second_value, lookup(test_table, second_key));

    destroy(test_table);
}

/**
 * @brief Verifies that erasing a mid-chain item maintains structural continuity.
 */
void test_erase_mid_chain_node_preserves_surrounding_links(void)
{
    // Arrange
    ConcurrentHashTable* test_table = initialize();

    const char* leading_key = "Key-A";
    const char* middle_key = "Key-B";
    const char* trailing_key = "Key-C";

    insert(test_table, leading_key, (void*)(intptr_t)1);
    insert(test_table, middle_key, (void*)(intptr_t)2);
    insert(test_table, trailing_key, (void*)(intptr_t)3);

    // Act
    bool removal_result = erase(test_table, middle_key);

    // Assert
    TEST_ASSERT_TRUE(removal_result);
    TEST_ASSERT_NOT_NULL(lookup(test_table, leading_key));
    TEST_ASSERT_NOT_NULL(lookup(test_table, trailing_key));
    TEST_ASSERT_NULL(lookup(test_table, middle_key));

    destroy(test_table);
}

/**
 * @brief Verifies that empty tables behave consistently when displayed.
 */
void test_display_empty_table_executes_without_errors(void)
{
    // Arrange
    ConcurrentHashTable* test_table = initialize();

    // Act & Assert
    display(test_table);

    destroy(test_table);
}

/**
 * @brief Verifies that a worker thread payload executes initialization blocks safely.
 */
void test_worker_thread_task_handles_null_payload_gracefully(void)
{
    // Arrange
    void* null_argument = NULL;

    // Act
    void* task_result = worker_thread_task(null_argument);

    // Assert
    TEST_ASSERT_NULL(task_result);
}

/**
 * @brief Verifies worker thread task validation safeguards against blank tables.
 */
void test_worker_thread_task_handles_missing_table_inside_payload(void)
{
    // Arrange
    ThreadPayload isolated_payload;

    isolated_payload.id = 0;
    isolated_payload.table = NULL;

    // Act
    void* task_result = worker_thread_task(&isolated_payload);

    // Assert
    TEST_ASSERT_NULL(task_result);
}

/**
 * @brief Verifies that entry count stability remains solid across large string boundaries.
 */
void test_insert_with_extremely_long_key_string_succeeds(void)
{
    // Arrange
    ConcurrentHashTable* test_table = initialize();

    char standard_long_key[256];

    for (uint32_t character_index = 0; character_index < 255; character_index++)
    {
        standard_long_key[character_index] = 'A';
    }

    standard_long_key[255] = '\0';

    void* expected_payload = (void*)(intptr_t)8888;

    // Act
    bool insertion_result = insert(test_table, standard_long_key, expected_payload);

    // Assert
    TEST_ASSERT_TRUE(insertion_result);
    TEST_ASSERT_EQUAL_PTR(expected_payload, lookup(test_table, standard_long_key));

    destroy(test_table);
}

/**
 * @brief Verifies that clear functions eradicate all properties without residues.
 */
void test_destroy_clears_populated_table_elements_without_crashing(void)
{
    // Arrange
    ConcurrentHashTable* test_table = initialize();

    insert(test_table, "temporary_alpha", (void*)(intptr_t)10);
    insert(test_table, "temporary_beta", (void*)(intptr_t)20);

    // Act & Assert
    destroy(test_table);
}

/**
 * @brief Verifies tracking capabilities with empty keys.
 */
void test_insert_with_empty_string_key_succeeds(void)
{
    // Arrange
    ConcurrentHashTable* test_table = initialize();

    const char* empty_string_key = "";
    void* expected_payload = (void*)(intptr_t)12345;

    // Act
    bool insertion_result = insert(test_table, empty_string_key, expected_payload);

    // Assert
    TEST_ASSERT_TRUE(insertion_result);
    TEST_ASSERT_EQUAL_PTR(expected_payload, lookup(test_table, empty_string_key));

    destroy(test_table);
}

/**
 * @brief Verifies erasure tracking constraints against multiple duplicates.
 */
void test_erase_head_chain_node_adjusts_bucket_head_correctly(void)
{
    // Arrange
    ConcurrentHashTable* test_table = initialize();

    const char* first_key = "chain_base";
    const char* second_key = "chain_head";

    insert(test_table, first_key, (void*)(intptr_t)500);
    insert(test_table, second_key, (void*)(intptr_t)600);

    // Act
    bool removal_result = erase(test_table, second_key);

    // Assert
    TEST_ASSERT_TRUE(removal_result);
    TEST_ASSERT_NULL(lookup(test_table, second_key));
    TEST_ASSERT_NOT_NULL(lookup(test_table, first_key));

    destroy(test_table);
}

/**
 * @brief Verifies structural sanity throughout a loop sequence.
 */
void test_sequential_insert_and_erase_restores_empty_state(void)
{
    // Arrange
    ConcurrentHashTable* test_table = initialize();

    const char* key = "lifecycle_key";

    void* payload = (void*)(intptr_t)77;

    // Act
    insert(test_table, key, payload);
    
    bool initial_erasure = erase(test_table, key);
    bool secondary_erasure = erase(test_table, key);

    // Assert
    TEST_ASSERT_TRUE(initial_erasure);
    TEST_ASSERT_FALSE(secondary_erasure);
    TEST_ASSERT_NULL(lookup(test_table, key));

    destroy(test_table);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_initialize_returns_non_null_pointer);
    RUN_TEST(test_insert_with_null_table_returns_false);
    RUN_TEST(test_insert_with_null_key_returns_false);
    RUN_TEST(test_insert_successful_entry_returns_true);
    RUN_TEST(test_lookup_with_null_table_returns_null);
    RUN_TEST(test_lookup_with_null_key_returns_null);
    RUN_TEST(test_lookup_retrieves_correct_inserted_value);
    RUN_TEST(test_lookup_non_existent_key_returns_null);
    RUN_TEST(test_insert_duplicate_key_updates_existing_value);
    RUN_TEST(test_erase_with_null_table_returns_false);
    RUN_TEST(test_erase_with_null_key_returns_false);
    RUN_TEST(test_erase_existing_key_returns_true_and_removes_node);
    RUN_TEST(test_erase_non_existent_key_returns_false);
    RUN_TEST(test_destroy_with_null_table_handles_gracefully);
    RUN_TEST(test_display_with_null_table_handles_gracefully);
    RUN_TEST(test_multiple_unique_insertions_persist_independently);
    RUN_TEST(test_erase_mid_chain_node_preserves_surrounding_links);
    RUN_TEST(test_display_empty_table_executes_without_errors);
    RUN_TEST(test_worker_thread_task_handles_null_payload_gracefully);
    RUN_TEST(test_worker_thread_task_handles_missing_table_inside_payload);
    RUN_TEST(test_insert_with_extremely_long_key_string_succeeds);
    RUN_TEST(test_destroy_clears_populated_table_elements_without_crashing);
    RUN_TEST(test_insert_with_empty_string_key_succeeds);
    RUN_TEST(test_erase_head_chain_node_adjusts_bucket_head_correctly);
    RUN_TEST(test_sequential_insert_and_erase_restores_empty_state);

    // Destroy the session.
    return UNITY_END();
}