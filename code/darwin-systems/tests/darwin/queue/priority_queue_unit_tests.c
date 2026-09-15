/**
 * @file        :   priority_queue_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Comprehensive unit test suite for the dynamic priority queue.
 * @details     :   Verifies edge cases, capacity variations, min-heap ordering, 
 * defensive arguments, and structural stability using the Unity testing framework.
 * Compile:
 * clang ./tests/darwin/queue/priority_queue_unit_tests.c ./src/darwin/queue/priority-queue/priority_queue.c ./src/unity.c -Iinclude -o ./bin/priority_queue_unit_test
 * @version     :   1.0
 * @date        :   2026-04-15
 */
#include <stdio.h>

#include "unity.h"
#include "darwin/queue/priority_queue.h"

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
 * @brief Verifies that a valid queue structure is created with requested parameters.
 */
void test_initialize_creates_valid_queue_with_expected_capacity(void)
{
    // Arrange.
    size_t preliminary_allocation_limit = 15;

    // Act.
    Queue* active_heap_container = initialize(preliminary_allocation_limit);

    // Assert.
    TEST_ASSERT_NOT_NULL(active_heap_container);
    TEST_ASSERT_EQUAL_UINT(0, active_heap_container->size);
    TEST_ASSERT_EQUAL_UINT(preliminary_allocation_limit, active_heap_container->capacity);

    clear(active_heap_container);
}

/**
 * @brief Verifies that initialization handles zero capacity gracefully by returning NULL.
 */
void test_initialize_returns_null_when_given_zero_capacity(void)
{
    // Arrange.
    size_t non_viable_capacity_threshold = 0;

    // Act.
    Queue* invalid_heap_allocation = initialize(non_viable_capacity_threshold);

    // Assert.
    TEST_ASSERT_NULL(invalid_heap_allocation);
}

/**
 * @brief Verifies that inserting an item correctly tracks tracking state and sizes.
 */
void test_enqueue_saves_initial_element_correctly(void)
{
    // Arrange.
    Queue* operational_priority_queue = initialize(5);

    int assigned_priority_rank = 25;
    char* target_string_payload = "Initial Testing Token";

    // Act.
    bool target_insertion_status = enqueue(operational_priority_queue, assigned_priority_rank, target_string_payload);

    // Assert.
    TEST_ASSERT_TRUE(target_insertion_status);
    TEST_ASSERT_EQUAL_UINT(1, operational_priority_queue->size);
    TEST_ASSERT_EQUAL_INT(assigned_priority_rank, operational_priority_queue->nodes[0].priority);

    clear(operational_priority_queue);
}

/**
 * @brief Verifies defensive handling of invalid queue instances during entry additions.
 */
void test_enqueue_returns_false_when_given_null_queue_pointer(void)
{
    // Arrange.
    Queue* non_existent_queue_reference = NULL;

    int generic_priority_score = 10;
    char* isolated_payload_reference = "Orphaned Element Data";

    // Act.
    bool isolation_execution_outcome = enqueue(non_existent_queue_reference, generic_priority_score, isolated_payload_reference);

    // Assert.
    TEST_ASSERT_FALSE(isolation_execution_outcome);
}

/**
 * @brief Verifies that null data insertion attempts are actively intercepted and blocked.
 */
void test_enqueue_returns_false_when_given_null_data_pointer(void)
{
    // Arrange.
    Queue* protected_heap_structure = initialize(5);

    int functional_priority_level = 42;
    void* missing_payload_pointer = NULL;

    // Act.
    bool tracking_rejection_result = enqueue(protected_heap_structure, functional_priority_level, missing_payload_pointer);

    // Assert.
    TEST_ASSERT_FALSE(tracking_rejection_result);
    TEST_ASSERT_EQUAL_UINT(0, protected_heap_structure->size);

    clear(protected_heap_structure);
}

/**
 * @brief Verifies that expanding thresholds triggers internal geometrical resizing seamlessly.
 */
void test_enqueue_automatically_triggers_resize_when_capacity_exceeded(void)
{
    // Arrange.
    Queue* restrictive_capacity_queue = initialize(1);

    enqueue(restrictive_capacity_queue, 100, "Forced Element One");

    int secondary_priority_index = 50;
    char* consecutive_payload_entry = "Forced Element Two";

    // Act.
    bool dynamic_expansion_outcome = enqueue(restrictive_capacity_queue, secondary_priority_index, consecutive_payload_entry);

    // Assert.
    TEST_ASSERT_TRUE(dynamic_expansion_outcome);
    TEST_ASSERT_EQUAL_UINT(2, restrictive_capacity_queue->size);
    TEST_ASSERT_EQUAL_UINT(2, restrictive_capacity_queue->capacity);

    clear(restrictive_capacity_queue);
}

/**
 * @brief Verifies viewing capabilities do not distort queue bounds or structural tracking.
 */
void test_peek_returns_root_priority_without_modifying_queue_size(void)
{
    // Arrange.
    Queue* static_inspection_heap = initialize(10);

    enqueue(static_inspection_heap, 88, "Inspection Element");

    // Act.
    int evaluated_priority_metric = peek(static_inspection_heap);

    // Assert.
    TEST_ASSERT_EQUAL_INT(88, evaluated_priority_metric);
    TEST_ASSERT_EQUAL_UINT(1, static_inspection_heap->size);

    clear(static_inspection_heap);
}

/**
 * @brief Verifies empty queue peeks safely return structural error indicators.
 */
void test_peek_returns_negative_one_when_queue_is_empty(void)
{
    // Arrange.
    Queue* unpopulated_evaluation_queue = initialize(4);

    // Act.
    int fallback_priority_response = peek(unpopulated_evaluation_queue);

    // Assert.
    TEST_ASSERT_EQUAL_INT(-1, fallback_priority_response);

    clear(unpopulated_evaluation_queue);
}

/**
 * @brief Verifies non-initialized structural references respond safely to inspection checks.
 */
void test_peek_returns_negative_one_when_given_null_queue_pointer(void)
{
    // Arrange.
    Queue* unallocated_structural_pointer = NULL;

    // Act.
    int error_mitigation_response = peek(unallocated_structural_pointer);

    // Assert.
    TEST_ASSERT_EQUAL_INT(-1, error_mitigation_response);
}

/**
 * @brief Verifies processing extractions safely recover exact metadata targets.
 */
void test_dequeue_extracts_single_element_successfully(void)
{
    // Arrange.
    Queue* individual_element_queue = initialize(3);

    char* deterministic_payload = "Solitary Node Context";

    enqueue(individual_element_queue, 14, deterministic_payload);

    int extracted_priority_score = 0;
    void* retrieved_generic_payload = NULL;

    // Act.
    bool extraction_execution_success = dequeue(individual_element_queue, &extracted_priority_score, &retrieved_generic_payload);

    // Assert.
    TEST_ASSERT_TRUE(extraction_execution_success);
    TEST_ASSERT_EQUAL_INT(14, extracted_priority_score);
    TEST_ASSERT_EQUAL_PTR(deterministic_payload, retrieved_generic_payload);
    TEST_ASSERT_EQUAL_UINT(0, individual_element_queue->size);

    clear(individual_element_queue);
}

/**
 * @brief Verifies that extraction drops gracefully report errors when structures have zero entries.
 */
void test_dequeue_returns_false_when_queue_is_empty(void)
{
    // Arrange.
    Queue* vacant_processing_unit = initialize(10);

    int baseline_priority_buffer = 999;
    void* foundational_payload_buffer = NULL;

    // Act.
    bool empty_extraction_response = dequeue(vacant_processing_unit, &baseline_priority_buffer, &foundational_payload_buffer);

    // Assert.
    TEST_ASSERT_FALSE(empty_extraction_response);

    clear(vacant_processing_unit);
}

/**
 * @brief Verifies missing physical instances terminate execution runs with appropriate responses.
 */
void test_dequeue_returns_false_when_given_null_queue_pointer(void)
{
    // Arrange.
    Queue* vanished_heap_instance = NULL;

    int terminal_priority_tracker = 0;
    void* terminal_data_tracker = NULL;

    // Act.
    bool direct_failure_consequence = dequeue(vanished_heap_instance, &terminal_priority_tracker, &terminal_data_tracker);

    // Assert.
    TEST_ASSERT_FALSE(direct_failure_consequence);
}

/**
 * @brief Verifies underlying binary relationships scale sorting correctly for multiple nodes.
 */
void test_dequeue_enforces_min_heap_order_for_three_elements(void)
{
    // Arrange.
    Queue* sorting_evaluation_heap = initialize(5);

    enqueue(sorting_evaluation_heap, 30, "High Boundary");
    enqueue(sorting_evaluation_heap, 10, "Minimum Target");
    enqueue(sorting_evaluation_heap, 20, "Median Target");

    int first_extracted_priority = 0;
    int second_extracted_priority = 0;
    int third_extracted_priority = 0;

    // Act.
    dequeue(sorting_evaluation_heap, &first_extracted_priority, NULL);
    dequeue(sorting_evaluation_heap, &second_extracted_priority, NULL);
    dequeue(sorting_evaluation_heap, &third_extracted_priority, NULL);

    // Assert.
    TEST_ASSERT_EQUAL_INT(10, first_extracted_priority);
    TEST_ASSERT_EQUAL_INT(20, second_extracted_priority);
    TEST_ASSERT_EQUAL_INT(30, third_extracted_priority);

    clear(sorting_evaluation_heap);
}

/**
 * @brief Verifies extraction outputs partition reference locations isolated from variables.
 */
void test_dequeue_correctly_updates_out_parameters_independently(void)
{
    // Arrange.
    Queue* partitioned_output_queue = initialize(2);

    char* target_metadata_reference = "Isolated Storage Frame";

    enqueue(partitioned_output_queue, 7, target_metadata_reference);

    int specialized_priority_collector = -5;
    void* specialized_pointer_collector = NULL;

    // Act.
    dequeue(partitioned_output_queue, &specialized_priority_collector, &specialized_pointer_collector);

    // Assert.
    TEST_ASSERT_EQUAL_INT(7, specialized_priority_collector);
    TEST_ASSERT_EQUAL_PTR(target_metadata_reference, specialized_pointer_collector);

    clear(partitioned_output_queue);
}

/**
 * @brief Verifies that ignoring return values via null variables processes without crashes.
 */
void test_dequeue_allows_null_out_parameters_for_silent_extraction(void)
{
    // Arrange.
    Queue* silent_processing_heap = initialize(2);
    enqueue(silent_processing_heap, 55, "Discarded Context Block");

    // Act.
    bool blind_extraction_outcome = dequeue(silent_processing_heap, NULL, NULL);

    // Assert.
    TEST_ASSERT_TRUE(blind_extraction_outcome);
    TEST_ASSERT_EQUAL_UINT(0, silent_processing_heap->size);

    clear(silent_processing_heap);
}

/**
 * @brief Verifies up-shifting behavior by sorting heavy weights to descending positions.
 */
void test_enqueue_maintains_heap_property_with_descending_priority_inputs(void)
{
    // Arrange.
    Queue* inverted_insertion_heap = initialize(3);

    // Act.
    enqueue(inverted_insertion_heap, 50, "Third Processing Priority");
    enqueue(inverted_insertion_heap, 40, "Second Processing Priority");
    enqueue(inverted_insertion_heap, 30, "First Processing Priority");

    // Assert.
    TEST_ASSERT_EQUAL_INT(30, inverted_insertion_heap->nodes[0].priority);

    clear(inverted_insertion_heap);
}

/**
 * @brief Verifies sequence sorting updates remain stationary when properties match exactly.
 */
void test_enqueue_maintains_heap_property_with_ascending_priority_inputs(void)
{
    // Arrange.
    Queue* standardized_insertion_heap = initialize(3);

    // Act.
    enqueue(standardized_insertion_heap, 11, "Alpha Node");
    enqueue(standardized_insertion_heap, 22, "Beta Node");
    enqueue(standardized_insertion_heap, 33, "Gamma Node");

    // Assert.
    TEST_ASSERT_EQUAL_INT(11, standardized_insertion_heap->nodes[0].priority);

    clear(standardized_insertion_heap);
}

/**
 * @brief Verifies memory configurations remain robust during rapid large-scale growth.
 */
void test_resize_handles_large_scale_stress_insertions(void)
{
    // Arrange.
    Queue* high_volume_stress_queue = initialize(2);
    size_t iterative_stress_limit = 100;
    bool cluster_insertion_failure_flag = false;

    // Act.
    for (size_t current_count = 0; current_count < iterative_stress_limit; current_count++)
    {
        if (!enqueue(high_volume_stress_queue, (int)(1000 - current_count), "Stress Batch Data"))
        {
            cluster_insertion_failure_flag = true;
        }
    }

    // Assert.
    TEST_ASSERT_FALSE(cluster_insertion_failure_flag);
    TEST_ASSERT_EQUAL_UINT(iterative_stress_limit, high_volume_stress_queue->size);
    TEST_ASSERT_EQUAL_INT(901, peek(high_volume_stress_queue));

    clear(high_volume_stress_queue);
}

/**
 * @brief Verifies that clearing unallocated resources passes checks without faults.
 */
void test_clear_gracefully_ignores_null_queue_pointer(void)
{
    // Arrange.
    Queue* vacant_memory_address = NULL;

    // Act & Assert.
    clear(vacant_memory_address);
    
    // Pass verification confirming no crash occurred.
    TEST_PASS();
}

/**
 * @brief Verifies total processing cycles scale size metrics directly back to baseline zero.
 */
void test_dequeue_empties_queue_completely_returning_to_size_zero(void)
{
    // Arrange.
    Queue* cyclic_drainage_queue = initialize(4);

    enqueue(cyclic_drainage_queue, 1, "A");
    enqueue(cyclic_drainage_queue, 2, "B");

    // Act.
    dequeue(cyclic_drainage_queue, NULL, NULL);
    dequeue(cyclic_drainage_queue, NULL, NULL);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(0, cyclic_drainage_queue->size);
    TEST_ASSERT_FALSE(dequeue(cyclic_drainage_queue, NULL, NULL));

    clear(cyclic_drainage_queue);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_initialize_creates_valid_queue_with_expected_capacity);
    RUN_TEST(test_initialize_returns_null_when_given_zero_capacity);
    RUN_TEST(test_enqueue_saves_initial_element_correctly);
    RUN_TEST(test_enqueue_returns_false_when_given_null_queue_pointer);
    RUN_TEST(test_enqueue_returns_false_when_given_null_data_pointer);
    RUN_TEST(test_enqueue_automatically_triggers_resize_when_capacity_exceeded);
    RUN_TEST(test_peek_returns_root_priority_without_modifying_queue_size);
    RUN_TEST(test_peek_returns_negative_one_when_queue_is_empty);
    RUN_TEST(test_peek_returns_negative_one_when_given_null_queue_pointer);
    RUN_TEST(test_dequeue_extracts_single_element_successfully);
    RUN_TEST(test_dequeue_returns_false_when_queue_is_empty);
    RUN_TEST(test_dequeue_returns_false_when_given_null_queue_pointer);
    RUN_TEST(test_dequeue_enforces_min_heap_order_for_three_elements);
    RUN_TEST(test_dequeue_correctly_updates_out_parameters_independently);
    RUN_TEST(test_dequeue_allows_null_out_parameters_for_silent_extraction);
    RUN_TEST(test_enqueue_maintains_heap_property_with_descending_priority_inputs);
    RUN_TEST(test_enqueue_maintains_heap_property_with_ascending_priority_inputs);
    RUN_TEST(test_resize_handles_large_scale_stress_insertions);
    RUN_TEST(test_clear_gracefully_ignores_null_queue_pointer);
    RUN_TEST(test_dequeue_empties_queue_completely_returning_to_size_zero);

    // Destory the session.
    return UNITY_END();
}