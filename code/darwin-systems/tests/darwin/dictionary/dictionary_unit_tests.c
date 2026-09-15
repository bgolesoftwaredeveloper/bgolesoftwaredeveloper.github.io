/**
 * @file        :   dictionary_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit test suite for the dynamic Dictionary (Hash Map) implementation.
 * @details     :   This suite utilizes the Unity Test Framework to rigorously validate 
 * the dictionary's core functionality. Key areas of verification include:
 * - Proper initialization and teardown of the Dictionary structure.
 * - Growth and contraction logic triggered by load factor thresholds.
 * - Collision resolution via linked-list chaining.
 * - Key-value persistence and memory isolation using string duplication.
 * - Robustness against edge cases like NULL inputs and empty keys.
 * Compile:
 * clang -DUNITY_INCLUDE_DOUBLE ./tests/darwin/dictionary/dictionary_unit_tests.c ./src/darwin/dictionary/dictionary.c ./src/unity.c -Iinclude -o ./bin/dictionary_unit_test
 * @version     :   1.0
 * @date        :   2026-05-12
 */
#include <stdio.h>
#include <string.h>

#include "unity.h"
#include "darwin/dictionary/dictionary.h"

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
 * @brief : Tests that initialization sets up correct default values.
 */
void test_dictionary_initial_state_compliance(void)
{
    // Arrange.
    Dictionary* instance_under_test = NULL;

    // Act.
    instance_under_test = initialize();

    // Assert.
    TEST_ASSERT_NOT_NULL(instance_under_test);
    TEST_ASSERT_EQUAL_UINT(INITIAL_SIZE, instance_under_test->size);
    TEST_ASSERT_EQUAL_UINT(0, instance_under_test->count);
    
    uninitialize(instance_under_test);
}

/**
 * @brief : Tests basic insertion and retrieval functionality.
 */
void test_dictionary_single_entry_persistence(void)
{
    // Arrange.
    Dictionary* target_hashtable = initialize();

    const char* unique_identifier = "node_01";

    int hex_payload = 0xDEADBEEF;

    // Act.
    insert(target_hashtable, unique_identifier, &hex_payload);

    int* retrieved_payload = (int*)retrieve(target_hashtable, unique_identifier);

    // Assert.
    TEST_ASSERT_NOT_NULL(retrieved_payload);
    TEST_ASSERT_EQUAL_INT(hex_payload, *retrieved_payload);

    uninitialize(target_hashtable);
}

/**
 * @brief : Tests updating an existing key with a new value.
 */
void test_dictionary_value_overwrite_behavior(void)
{
    // Arrange.
    Dictionary* dynamic_store = initialize();

    const char* static_key = "configuration_bit";

    int initial_state = 0;
    int updated_state = 1;

    // Act.
    insert(dynamic_store, static_key, &initial_state);
    insert(dynamic_store, static_key, &updated_state);

    int* current_state = (int*)retrieve(dynamic_store, static_key);

    // Assert.
    TEST_ASSERT_EQUAL_INT(1, dynamic_store->count);
    TEST_ASSERT_EQUAL_INT(updated_state, *current_state);

    uninitialize(dynamic_store);
}

/**
 * @brief : Tests that retrieving a non-existent key returns NULL.
 */
void test_dictionary_missing_key_null_guarantee(void)
{
    // Arrange.
    Dictionary* safe_registry = initialize();

    // Act.
    void* result_pointer = retrieve(safe_registry, "absent_key_token");

    // Assert.
    TEST_ASSERT_NULL(result_pointer);

    uninitialize(safe_registry);
}

/**
 * @brief : Tests that the dictionary handles NULL inputs gracefully.
 */
void test_dictionary_null_input_safety_protocol(void)
{
    // Arrange.
    Dictionary* protected_dictionary = initialize();

    // Act & Assert.
    insert(NULL, "fail", NULL);
    insert(protected_dictionary, NULL, NULL);

    void* retrieved_data = retrieve(NULL, "fail");

    TEST_ASSERT_NULL(retrieved_data);

    uninitialize(protected_dictionary);
}

/**
 * @brief : Tests behavior when multiple keys hash to the same bucket.
 */
void test_dictionary_collision_chaining_resolution(void)
{
    // Arrange.
    Dictionary* collision_tester = initialize();

    const char* first_colliding_key = "alpha_stream";
    const char* second_colliding_key = "beta_stream";

    double numeric_data_x = 1.1;
    double numeric_data_y = 2.2;

    // Act.
    insert(collision_tester, first_colliding_key, &numeric_data_x);
    insert(collision_tester, second_colliding_key, &numeric_data_y);

    // Assert.
    TEST_ASSERT_EQUAL_DOUBLE(numeric_data_x, *(double*)retrieve(collision_tester, first_colliding_key));
    TEST_ASSERT_EQUAL_DOUBLE(numeric_data_y, *(double*)retrieve(collision_tester, second_colliding_key));

    uninitialize(collision_tester);
}

/**
 * @brief : Tests automatic resizing when GROWTH_THRESHOLD is met.
 */
void test_dictionary_dynamic_expansion_trigger(void)
{
    // Arrange.
    Dictionary* elastic_buffer = initialize();
    size_t original_capacity = elastic_buffer->size;

    char key_buffer[16];
    int generic_value = 100;

    // Act.
    for (int index = 0; index < 7; index++)
    {
        sprintf(key_buffer, "key_%d", index);
        insert(elastic_buffer, key_buffer, &generic_value);
    }

    // Assert.
    TEST_ASSERT_TRUE(elastic_buffer->size > original_capacity);

    uninitialize(elastic_buffer);
}

/**
 * @brief : Tests removal of a leaf node in the chain.
 */
void test_dictionary_targeted_element_removal(void)
{
    // Arrange.
    Dictionary* cleanup_instance = initialize();

    const char* transient_key = "volatile_data";
    long long system_timestamp = 123456789;

    // Act.
    insert(cleanup_instance, transient_key, &system_timestamp);
    erase(cleanup_instance, transient_key);

    // Assert.
    TEST_ASSERT_NULL(retrieve(cleanup_instance, transient_key));
    TEST_ASSERT_EQUAL_UINT(0, cleanup_instance->count);

    uninitialize(cleanup_instance);
}

/**
 * @brief : Tests removal of the first element in a bucket list.
 */
void test_dictionary_bucket_head_removal_logic(void)
{
    // Arrange.
    Dictionary* sequence_map = initialize();

    const char* primary_key = "head_node";
    const char* secondary_key = "tail_node";

    int integer_constant = 42;

    // Act.
    insert(sequence_map, primary_key, &integer_constant);
    insert(sequence_map, secondary_key, &integer_constant);
    erase(sequence_map, primary_key);

    // Assert.
    TEST_ASSERT_NULL(retrieve(sequence_map, primary_key));
    TEST_ASSERT_NOT_NULL(retrieve(sequence_map, secondary_key));

    uninitialize(sequence_map);
}

/**
 * @brief : Tests that shrinking occurs when below SHRINK_THRESHOLD.
 */
void test_dictionary_load_factor_contraction_verification(void)
{
    // Arrange.
    Dictionary* volatile_cache = initialize();
    
    char identification_buffer[24];
    int placeholder_data = 100;

    // Act.
    for (int index = 0; index < 40; index++) 
    {
        snprintf(identification_buffer, sizeof(identification_buffer), "batch_key_%d", index);
        insert(volatile_cache, identification_buffer, &placeholder_data);
    }

    size_t peak_capacity = volatile_cache->size;
    
    for (int index = 0; index < 38; index++)
    {
        snprintf(identification_buffer, sizeof(identification_buffer), "batch_key_%d", index);
        erase(volatile_cache, identification_buffer);
    }

    // Assert.
    TEST_ASSERT_TRUE(volatile_cache->size < peak_capacity);

    uninitialize(volatile_cache);
}

/**
 * @brief : Tests removal of a key that does not exist in the map.
 */
void test_dictionary_non_existent_key_removal_safety(void)
{
    // Arrange.
    Dictionary* robust_map = initialize();

    int sample_data = 99;

    // Act.
    insert(robust_map, "valid_key", &sample_data);
    erase(robust_map, "ghost_key");

    // Assert.
    TEST_ASSERT_EQUAL_UINT(1, robust_map->count);

    uninitialize(robust_map);
}

/**
 * @brief : Tests retrieval after an internal resize event occurs.
 */
void test_dictionary_post_resize_retrieval_integrity(void)
{
    // Arrange.
    Dictionary* scale_test_map = initialize();

    const char* permanent_anchor = "anchor_point";
    int essential_value = 888;

    // Act.
    insert(scale_test_map, permanent_anchor, &essential_value);

    char keys[20];

    for (int index = 0; index < 20; index++)
    {
        insert(scale_test_map, keys, &essential_value);
    }

    // Assert.
    int* anchor_data = (int*)retrieve(scale_test_map, permanent_anchor);

    TEST_ASSERT_NOT_NULL(anchor_data);
    TEST_ASSERT_EQUAL_INT(essential_value, *anchor_data);

    uninitialize(scale_test_map);
}

/**
 * @brief : Tests inserting an empty string as a valid key.
 */
void test_dictionary_empty_string_key_support(void)
{
    // Arrange.
    Dictionary* string_edge_map = initialize();

    const char* null_string_key = "";
    float precision_metric = 0.001f;

    // Act.
    insert(string_edge_map, null_string_key, &precision_metric);
    float* retrieved_metric = (float*)retrieve(string_edge_map, null_string_key);

    // Assert.
    TEST_ASSERT_NOT_NULL(retrieved_metric);
    TEST_ASSERT_EQUAL_FLOAT(precision_metric, *retrieved_metric);

    uninitialize(string_edge_map);
}

/**
 * @brief : Tests that very long strings do not break the hash function.
 */
void test_dictionary_voluminous_key_string_processing(void)
{
    // Arrange.
    Dictionary* long_key_map = initialize();

    char massive_key[1024];

    memset(massive_key, 'A', 1023);

    massive_key[1023] = '\0';

    int buffer_id = 77;

    // Act.
    insert(long_key_map, massive_key, &buffer_id);

    // Assert.
    TEST_ASSERT_EQUAL_INT(buffer_id, *(int*)retrieve(long_key_map, massive_key));

    uninitialize(long_key_map);
}

/**
 * @brief : Tests removal from a completely empty dictionary.
 */
void test_dictionary_empty_container_removal_resilience(void)
{
    // Arrange.
    Dictionary* vacuum_dictionary = initialize();

    // Act.
    erase(vacuum_dictionary, "non_existent_key");

    // Assert.
    TEST_ASSERT_EQUAL_UINT(0, vacuum_dictionary->count);

    uninitialize(vacuum_dictionary);
}

/**
 * @brief : Tests if keys remain independent of the original input buffer.
 */
void test_dictionary_key_memory_independence_isolation(void)
{
    // Arrange.
    Dictionary* independent_vault = initialize();

    char stack_buffer[] = "original_buffer";
    int secure_val = 55;

    // Act.
    insert(independent_vault, stack_buffer, &secure_val);

    stack_buffer[0] = 'X';

    // Assert.
    TEST_ASSERT_NOT_NULL(retrieve(independent_vault, "original_buffer"));
    TEST_ASSERT_NULL(retrieve(independent_vault, "Xriginal_buffer"));

    uninitialize(independent_vault);
}

/**
 * @brief : Tests performance stability with a high volume of entries.
 */
void test_dictionary_high_volume_stress_saturation(void)
{
    // Arrange.
    Dictionary* stress_subject = initialize();

    int iteration_limit = 500;
    int payload_constant = 13;

    // Act.
    for (int index = 0; index < iteration_limit; index++)
    {
        char uuid_label[32];

        sprintf(uuid_label, "session_id_%d", index);

        insert(stress_subject, uuid_label, &payload_constant);
    }

    // Assert.
    TEST_ASSERT_EQUAL_UINT(iteration_limit, stress_subject->count);

    uninitialize(stress_subject);
}

/**
 * @brief : Tests retrieval consistency after removing neighboring nodes.
 */
void test_dictionary_neighboring_node_integrity_stability(void)
{
    // Arrange.
    Dictionary* cluster_map = initialize();

    int one = 1;
    int two = 2; 
    int three = 3;

    // Act.
    insert(cluster_map, "node_a", &one);
    insert(cluster_map, "node_b", &two);
    insert(cluster_map, "node_c", &three);
    erase(cluster_map, "node_b");

    // Assert.
    TEST_ASSERT_NOT_NULL(retrieve(cluster_map, "node_a"));
    TEST_ASSERT_NOT_NULL(retrieve(cluster_map, "node_c"));

    uninitialize(cluster_map);
}

/**
 * @brief : Tests removing an item and re-inserting it immediately.
 */
void test_dictionary_deletion_reinsertion_cycle_reliability(void)
{
    // Arrange.
    Dictionary* recycling_bin = initialize();

    const char* recycled_key = "resource_handle";

    int initial_resource = 10;
    int new_resource = 20;

    // Act.
    insert(recycling_bin, recycled_key, &initial_resource);
    erase(recycling_bin, recycled_key);
    insert(recycling_bin, recycled_key, &new_resource);

    // Assert.
    TEST_ASSERT_EQUAL_INT(new_resource, *(int*)retrieve(recycling_bin, recycled_key));

    uninitialize(recycling_bin);
}

/**
 * @brief : Tests that uninitialize handles a null dictionary pointer.
 */
void test_dictionary_deallocation_null_safety_check(void)
{
    // Arrange.
    Dictionary* null_ptr_dictionary = NULL;

    // Act & Assert.
    uninitialize(null_ptr_dictionary);

    TEST_ASSERT_NULL(null_ptr_dictionary);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_dictionary_initial_state_compliance);
    RUN_TEST(test_dictionary_single_entry_persistence);
    RUN_TEST(test_dictionary_value_overwrite_behavior);
    RUN_TEST(test_dictionary_missing_key_null_guarantee);
    RUN_TEST(test_dictionary_null_input_safety_protocol);
    RUN_TEST(test_dictionary_collision_chaining_resolution);
    RUN_TEST(test_dictionary_dynamic_expansion_trigger);
    RUN_TEST(test_dictionary_targeted_element_removal);
    RUN_TEST(test_dictionary_bucket_head_removal_logic);
    RUN_TEST(test_dictionary_load_factor_contraction_verification);
    RUN_TEST(test_dictionary_non_existent_key_removal_safety);
    RUN_TEST(test_dictionary_post_resize_retrieval_integrity);
    RUN_TEST(test_dictionary_empty_string_key_support);
    RUN_TEST(test_dictionary_voluminous_key_string_processing);
    RUN_TEST(test_dictionary_empty_container_removal_resilience);
    RUN_TEST(test_dictionary_key_memory_independence_isolation);
    RUN_TEST(test_dictionary_high_volume_stress_saturation);
    RUN_TEST(test_dictionary_neighboring_node_integrity_stability);
    RUN_TEST(test_dictionary_deletion_reinsertion_cycle_reliability);
    RUN_TEST(test_dictionary_deallocation_null_safety_check);

    // Destory the session.
    return UNITY_END();
}