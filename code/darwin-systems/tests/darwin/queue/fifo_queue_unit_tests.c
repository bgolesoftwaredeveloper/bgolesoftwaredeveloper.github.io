/**
 * @file        :   fifo_queue_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit test suite for the dynamic circular FIFO queue.
 * @details     :   This suite utilizes the Unity Test Framework to verify the 
 * robustness of the Darwin FIFO queue implementation. It 
 * comprehensively covers memory allocation safety, geometric 
 * resizing logic (buffer unrolling), circular index wrapping, 
 * and First-In-First-Out data integrity.
 * Compile:
 * clang ./tests/darwin/queue/fifo_queue_unit_tests.c ./src/darwin/queue/fifo-queue/fifo_queue.c ./src/unity.c -Iinclude -o ./bin/fifo_queue_unit_test
 * @version     :   1.0
 * @date        :   2026-04-14
 */
#include <stdio.h>

#include "unity.h"
#include "darwin/queue/fifo_queue.h"

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
 * @brief Ensures the prepare function returns a non-null pointer.
 */
void test_prepare_allocates_valid_memory_structure(void)
{
    // Arrange.
    size_t requested_initial_volume;

    Queue* allocated_fifo_pointer;

    requested_initial_volume = 10;

    // Act.
    allocated_fifo_pointer = prepare(requested_initial_volume);

    // Assert.
    TEST_ASSERT_NOT_NULL(allocated_fifo_pointer);

    destroy(allocated_fifo_pointer);
}

/**
 * @brief Verifies that zero capacity initialization returns NULL.
 */
void test_prepare_returns_null_on_zero_capacity_request(void)
{
    // Arrange.
    size_t invalid_zero_capacity;

    Queue* null_result_indicator;

    invalid_zero_capacity = 0;

    // Act.
    null_result_indicator = prepare(invalid_zero_capacity);

    // Assert.
    TEST_ASSERT_NULL(null_result_indicator);
}

/**
 * @brief Tests if size increments correctly after a single push.
 */
void test_enqueue_increments_total_element_count(void)
{
    // Arrange.
    Queue* primary_test_container;

    int primary_integer_payload;

    primary_test_container = prepare(5);
    primary_integer_payload = 100;

    // Act.
    enqueue(primary_test_container, primary_integer_payload);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(1, primary_test_container->size);

    destroy(primary_test_container);
}

/**
 * @brief Confirms that the first item added is the first item retrieved.
 */
void test_fifo_principle_maintenance_during_standard_ops(void)
{
    // Arrange.
    Queue* sequence_validation_queue;
    bool operational_success_toggle;

    int first_extracted_value;

    sequence_validation_queue = prepare(5);

    // Act.
    enqueue(sequence_validation_queue, 500);
    enqueue(sequence_validation_queue, 600);

    first_extracted_value = dequeue(sequence_validation_queue, &operational_success_toggle);

    // Assert.
    TEST_ASSERT_EQUAL_INT(500, first_extracted_value);

    destroy(sequence_validation_queue);
}

/**
 * @brief Verifies capacity doubling upon buffer saturation.
 */
void test_automatic_geometric_resize_on_buffer_saturation(void)
{
    // Arrange.
    Queue* expansion_target_buffer;

    expansion_target_buffer = prepare(2);

    // Act.
    enqueue(expansion_target_buffer, 1);
    enqueue(expansion_target_buffer, 2);
    enqueue(expansion_target_buffer, 3);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(4, expansion_target_buffer->capacity);

    destroy(expansion_target_buffer);
}

/**
 * @brief Ensures dequeue signals failure when the buffer is empty.
 */
void test_dequeue_returns_failure_signal_on_exhausted_queue(void)
{
    // Arrange.
    Queue* empty_state_test_queue;

    bool error_detection_flag;

    empty_state_test_queue = prepare(5);
    error_detection_flag = true;

    // Act.
    dequeue(empty_state_test_queue, &error_detection_flag);

    // Assert.
    TEST_ASSERT_FALSE(error_detection_flag);

    destroy(empty_state_test_queue);
}

/**
 * @brief Verifies flag success on valid data extraction.
 */
void test_dequeue_confirms_success_on_valid_data_retrieval(void)
{
    // Arrange.
    Queue* active_data_buffer;

    bool validation_confirmation_bit;

    active_data_buffer = prepare(5);
    validation_confirmation_bit = false;

    enqueue(active_data_buffer, 999);

    // Act.
    dequeue(active_data_buffer, &validation_confirmation_bit);

    // Assert.
    TEST_ASSERT_TRUE(validation_confirmation_bit);

    destroy(active_data_buffer);
}

/**
 * @brief Checks circular index wrapping at the array boundary.
 */
void test_circular_index_wrapping_at_memory_boundary(void)
{
    // Arrange.
    Queue* circular_logic_evaluator;

    bool extraction_status_bool;

    circular_logic_evaluator = prepare(2);

    // Act.
    enqueue(circular_logic_evaluator, 11);
    enqueue(circular_logic_evaluator, 22);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(0, circular_logic_evaluator->rear);

    destroy(circular_logic_evaluator);
}

/**
 * @brief Validates that data survives the resizing unroll process.
 */
void test_data_integrity_preservation_during_resize_event(void)
{
    // Arrange.
    Queue* migratory_data_structure;

    bool integrity_check_flag;
    int post_migration_value;

    migratory_data_structure = prepare(2);

    // Act.
    enqueue(migratory_data_structure, 77);
    enqueue(migratory_data_structure, 88);
    enqueue(migratory_data_structure, 99);

    post_migration_value = dequeue(migratory_data_structure, &integrity_check_flag);

    // Assert.
    TEST_ASSERT_EQUAL_INT(77, post_migration_value);

    destroy(migratory_data_structure);
}

/**
 * @brief Ensures size is zero after removing all items.
 */
void test_size_attribute_reflects_empty_state_after_drain(void)
{
    // Arrange.
    Queue* drainage_evaluation_object;

    bool pop_status_signal;

    drainage_evaluation_object = prepare(10);

    // Act.
    enqueue(drainage_evaluation_object, 42);
    dequeue(drainage_evaluation_object, &pop_status_signal);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(0, drainage_evaluation_object->size);

    destroy(drainage_evaluation_object);
}

/**
 * @brief Checks for null pointer safety when the flag is omitted.
 */
void test_dequeue_operates_safely_with_null_flag_pointer(void)
{
    // Arrange.
    Queue* pointer_safety_evaluation_fifo;

    int standalone_result_value;

    pointer_safety_evaluation_fifo = prepare(5);

    enqueue(pointer_safety_evaluation_fifo, 25);

    // Act.
    standalone_result_value = dequeue(pointer_safety_evaluation_fifo, NULL);

    // Assert.
    TEST_ASSERT_EQUAL_INT(25, standalone_result_value);

    destroy(pointer_safety_evaluation_fifo);
}

/**
 * @brief Tests sequential growth across multiple resize triggers.
 */
void test_sequential_memory_expansion_for_high_volume_data(void)
{
    // Arrange.
    Queue* multi_stage_growth_container;

    multi_stage_growth_container = prepare(1);

    // Act.
    enqueue(multi_stage_growth_container, 10);
    enqueue(multi_stage_growth_container, 20);
    enqueue(multi_stage_growth_container, 30);
    enqueue(multi_stage_growth_container, 40);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(4, multi_stage_growth_container->capacity);

    destroy(multi_stage_growth_container);
}

/**
 * @brief Ensures the front index moves forward correctly.
 */
void test_front_pointer_progression_during_element_extraction(void)
{
    // Arrange.
    Queue* index_progression_monitored_fifo;

    bool progression_check_toggle;

    index_progression_monitored_fifo = prepare(5);

    // Act.
    enqueue(index_progression_monitored_fifo, 1);
    enqueue(index_progression_monitored_fifo, 2);

    dequeue(index_progression_monitored_fifo, &progression_check_toggle);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(1, index_progression_monitored_fifo->front);

    destroy(index_progression_monitored_fifo);
}

/**
 * @brief Tests circular logic with a minimal buffer size of one.
 */
void test_minimalist_buffer_circular_interleaving_logic(void)
{
    // Arrange.
    Queue* singular_cell_storage_unit;

    bool cell_operation_flag;

    singular_cell_storage_unit = prepare(1);

    // Act.
    enqueue(singular_cell_storage_unit, 8);

    dequeue(singular_cell_storage_unit, &cell_operation_flag);
    enqueue(singular_cell_storage_unit, 9);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(1, singular_cell_storage_unit->size);

    destroy(singular_cell_storage_unit);
}

/**
 * @brief Verifies that destroy handles NULL input without crashing.
 */
void test_destroy_mechanism_handles_null_reference_safely(void)
{
    // Arrange.
    Queue* non_existent_heap_pointer;

    non_existent_heap_pointer = NULL;

    // Act.
    destroy(non_existent_heap_pointer);

    // Assert.
    TEST_PASS();
}

/**
 * @brief Checks if queue can be refilled after being emptied.
 */
void test_reinitialization_capacity_after_reaching_exhaustion(void)
{
    // Arrange.
    Queue* cyclic_reset_test_environment;

    bool reset_success_indicator;

    cyclic_reset_test_environment = prepare(5);

    // Act.
    enqueue(cyclic_reset_test_environment, 123);

    dequeue(cyclic_reset_test_environment, &reset_success_indicator);
    enqueue(cyclic_reset_test_environment, 456);

    // Assert.
    TEST_ASSERT_EQUAL_INT(456, cyclic_reset_test_environment->array[cyclic_reset_test_environment->front]);

    destroy(cyclic_reset_test_environment);
}

/**
 * @brief Ensures large integer boundaries are handled.
 */
void test_queue_integrity_with_maximum_integer_boundaries(void)
{
    // Arrange.
    Queue* boundary_value_verification_queue;

    int extreme_integer_constant;
    int validated_output_value;

    bool boundary_retrieval_status;

    boundary_value_verification_queue = prepare(5);
    extreme_integer_constant = 2147483647;

    // Act.
    enqueue(boundary_value_verification_queue, extreme_integer_constant);

    validated_output_value = dequeue(boundary_value_verification_queue, &boundary_retrieval_status);

    // Assert.
    TEST_ASSERT_EQUAL_INT(extreme_integer_constant, validated_output_value);

    destroy(boundary_value_verification_queue);
}

/**
 * @brief Checks for failure when passing NULL to enqueue.
 */
void test_enqueue_safely_rejects_null_queue_pointer(void)
{
    // Arrange.
    Queue* unallocated_memory_reference;

    bool insertion_failure_outcome;

    unallocated_memory_reference = NULL;

    // Act.
    insertion_failure_outcome = enqueue(unallocated_memory_reference, 10);

    // Assert.
    TEST_ASSERT_FALSE(insertion_failure_outcome);
}

/**
 * @brief Ensures size doesn't underflow on repeated empty dequeues.
 */
void test_size_attribute_stability_on_redundant_pop_requests(void)
{
    // Arrange.
    Queue* zero_state_stability_structure;

    bool redundant_fail_flag;

    zero_state_stability_structure = prepare(5);

    // Act.
    dequeue(zero_state_stability_structure, &redundant_fail_flag);
    dequeue(zero_state_stability_structure, &redundant_fail_flag);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(0, zero_state_stability_structure->size);

    destroy(zero_state_stability_structure);
}

/**
 * @brief Full lifecycle test from allocation to destruction.
 */
void test_comprehensive_lifecycle_fill_resize_and_drain_cycle(void)
{
    // Arrange.
    Queue* lifecycle_comprehensive;

    bool cycle_validation_status;

    lifecycle_comprehensive = prepare(2);

    // Act.
    enqueue(lifecycle_comprehensive, 1);
    enqueue(lifecycle_comprehensive, 2);
    enqueue(lifecycle_comprehensive, 3);

    dequeue(lifecycle_comprehensive, &cycle_validation_status);
    dequeue(lifecycle_comprehensive, &cycle_validation_status);
    dequeue(lifecycle_comprehensive, &cycle_validation_status);
    enqueue(lifecycle_comprehensive, 4);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(1, lifecycle_comprehensive->size);

    destroy(lifecycle_comprehensive);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_prepare_allocates_valid_memory_structure);
    RUN_TEST(test_prepare_returns_null_on_zero_capacity_request);
    RUN_TEST(test_destroy_mechanism_handles_null_reference_safely);
    RUN_TEST(test_enqueue_increments_total_element_count);
    RUN_TEST(test_enqueue_safely_rejects_null_queue_pointer);
    RUN_TEST(test_queue_integrity_with_maximum_integer_boundaries);
    RUN_TEST(test_fifo_principle_maintenance_during_standard_ops);
    RUN_TEST(test_dequeue_returns_failure_signal_on_exhausted_queue);
    RUN_TEST(test_dequeue_confirms_success_on_valid_data_retrieval);
    RUN_TEST(test_dequeue_operates_safely_with_null_flag_pointer);
    RUN_TEST(test_automatic_geometric_resize_on_buffer_saturation);
    RUN_TEST(test_data_integrity_preservation_during_resize_event);
    RUN_TEST(test_sequential_memory_expansion_for_high_volume_data);
    RUN_TEST(test_circular_index_wrapping_at_memory_boundary);
    RUN_TEST(test_front_pointer_progression_during_element_extraction);
    RUN_TEST(test_minimalist_buffer_circular_interleaving_logic);
    RUN_TEST(test_size_attribute_reflects_empty_state_after_drain);
    RUN_TEST(test_reinitialization_capacity_after_reaching_exhaustion);
    RUN_TEST(test_size_attribute_stability_on_redundant_pop_requests);
    RUN_TEST(test_comprehensive_lifecycle_fill_resize_and_drain_cycle);

    // Destroy the session.
    return UNITY_END();
}