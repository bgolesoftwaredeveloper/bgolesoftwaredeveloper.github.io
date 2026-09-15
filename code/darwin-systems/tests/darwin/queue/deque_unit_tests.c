/**
 * @file        :   deque_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Comprehensive suite of 20 unique unit tests for the dynamic circular Deque.
 * @details     :   Validates boundary conditions, dynamic resizing behavior, null-pointer safeguards,
 * circular indexing transitions, and explicit pass-by-pointer data extraction via 
 * the Unity Test Framework.
 * Compile:
 * clang ./tests/darwin/queue/deque_unit_tests.c ./src/darwin/queue/deque/deque.c ./src/unity.c -Iinclude -o ./bin/deque_unit_test
 * @version     :   1.0
 * @date        :   2026-05-20
 */
#include <stdio.h>

#include "unity.h"
#include "darwin/queue/deque.h"

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
 * @brief Test 1: Verifies that initialization with valid capacity allocates memory and resets attributes.
 */
void test_initialize_with_valid_capacity_should_allocate_successfully(void)
{
    // Arrange.
    size_t target_capacity = 4;

    // Act.
    Deque* target_queue = initialize(target_capacity);

    // Assert.
    TEST_ASSERT_NOT_NULL(target_queue);
    TEST_ASSERT_EQUAL_INT(0, target_queue->size);
    TEST_ASSERT_EQUAL_UINT(target_capacity, target_queue->capacity);

    clean(target_queue);
}

/**
 * @brief Test 2: Verifies that initialization with zero capacity yields a null pointer.
 */
void test_initialize_with_zero_capacity_should_return_null(void)
{
    // Arrange.
    size_t invalid_capacity = 0;

    // Act.
    Deque* target_queue = initialize(invalid_capacity);

    // Assert.
    TEST_ASSERT_NULL(target_queue);
}

/**
 * @brief Test 3: Verifies that inserting into the front of an empty deque sets state indexes to zero.
 */
void test_insert_front_on_empty_deque_should_set_indices_to_zero(void)
{
    // Arrange.
    Deque* target_queue = initialize(4);

    int input_payload = 42;

    // Act.
    bool execution_result = insert_front(target_queue, input_payload);

    // Assert.
    TEST_ASSERT_TRUE(execution_result);
    TEST_ASSERT_EQUAL_UINT(0, target_queue->front);
    TEST_ASSERT_EQUAL_UINT(0, target_queue->rear);
    TEST_ASSERT_EQUAL_INT(1, target_queue->size);

    clean(target_queue);
}

/**
 * @brief Test 4: Verifies that inserting into the rear of an empty deque sets state indexes to zero.
 */
void test_insert_rear_on_empty_deque_should_set_indices_to_zero(void)
{
    // Arrange.
    Deque* target_queue = initialize(4);

    int input_payload = 84;

    // Act.
    bool execution_result = insert_rear(target_queue, input_payload);

    // Assert.
    TEST_ASSERT_TRUE(execution_result);
    TEST_ASSERT_EQUAL_UINT(0, target_queue->front);
    TEST_ASSERT_EQUAL_UINT(0, target_queue->rear);
    TEST_ASSERT_EQUAL_INT(1, target_queue->size);

    clean(target_queue);
}

/**
 * @brief Test 5: Verifies that sequential insertions at the front shift the front index backwards circularly.
 */
void test_insert_front_multiple_items_should_wrap_index_circularly(void)
{
    // Arrange.
    Deque* target_queue = initialize(4);

    int primary_value = 10;
    int secondary_value = 20;

    // Act.
    insert_front(target_queue, primary_value);

    bool execution_result = insert_front(target_queue, secondary_value);

    // Assert.
    TEST_ASSERT_TRUE(execution_result);
    TEST_ASSERT_EQUAL_UINT(target_queue->capacity - 1, target_queue->front);
    TEST_ASSERT_EQUAL_INT(2, target_queue->size);

    clean(target_queue);
}

/**
 * @brief Test 6: Verifies that sequential insertions at the rear increment the rear index cleanly.
 */
void test_insert_rear_multiple_items_should_increment_index_sequentially(void)
{
    // Arrange.
    Deque* target_queue = initialize(4);

    int primary_value = 100;
    int secondary_value = 200;

    // Act.
    insert_rear(target_queue, primary_value);

    bool execution_result = insert_rear(target_queue, secondary_value);

    // Assert.
    TEST_ASSERT_TRUE(execution_result);
    TEST_ASSERT_EQUAL_UINT(1, target_queue->rear);
    TEST_ASSERT_EQUAL_INT(2, target_queue->size);

    clean(target_queue);
}

/**
 * @brief Test 7: Verifies that exceeding the initial capacity triggers an internal expansion.
 */
void test_insert_rear_beyond_capacity_should_trigger_dynamic_resize(void)
{
    // Arrange.
    Deque* target_queue = initialize(2);

    size_t initial_limit = target_queue->capacity;

    // Act.
    insert_rear(target_queue, 11);
    insert_rear(target_queue, 22);

    bool boundary_insertion = insert_rear(target_queue, 33);

    // Assert.
    TEST_ASSERT_TRUE(boundary_insertion);
    TEST_ASSERT_EQUAL_UINT(initial_limit * 2, target_queue->capacity);
    TEST_ASSERT_EQUAL_INT(3, target_queue->size);

    clean(target_queue);
}

/**
 * @brief Test 8: Verifies that delete_front removes items in FIFO order and updates data.
 */
void test_delete_front_should_extract_head_element_and_advance_index(void)
{
    // Arrange.
    Deque* target_queue = initialize(4);

    insert_rear(target_queue, 50);
    insert_rear(target_queue, 60);

    int destination_buffer = 0;

    // Act.
    bool execution_result = delete_front(target_queue, &destination_buffer);

    // Assert.
    TEST_ASSERT_TRUE(execution_result);
    TEST_ASSERT_EQUAL_INT(50, destination_buffer);
    TEST_ASSERT_EQUAL_INT(1, target_queue->size);
    TEST_ASSERT_EQUAL_UINT(1, target_queue->front);

    clean(target_queue);
}

/**
 * @brief Test 9: Verifies that delete_rear removes items in LIFO order relative to rear insertion.
 */
void test_delete_rear_should_extract_tail_element_and_regress_index(void)
{
    // Arrange.
    Deque* target_queue = initialize(4);

    insert_rear(target_queue, 70);
    insert_rear(target_queue, 80);

    int destination_buffer = 0;

    // Act.
    bool execution_result = delete_rear(target_queue, &destination_buffer);

    // Assert.
    TEST_ASSERT_TRUE(execution_result);
    TEST_ASSERT_EQUAL_INT(80, destination_buffer);
    TEST_ASSERT_EQUAL_INT(1, target_queue->size);
    TEST_ASSERT_EQUAL_UINT(0, target_queue->rear);

    clean(target_queue);
}

/**
 * @brief Test 10: Verifies that deleting the final item resets tracking markers back to structural zero.
 */
void test_delete_front_of_last_remaining_element_should_reset_indices(void)
{
    // Arrange.
    Deque* target_queue = initialize(4);

    insert_front(target_queue, 99);

    int destination_buffer = 0;

    // Act.
    bool execution_result = delete_front(target_queue, &destination_buffer);

    // Assert.
    TEST_ASSERT_TRUE(execution_result);
    TEST_ASSERT_EQUAL_UINT(0, target_queue->front);
    TEST_ASSERT_EQUAL_UINT(0, target_queue->rear);
    TEST_ASSERT_EQUAL_INT(0, target_queue->size);

    clean(target_queue);
}

/**
 * @brief Test 11: Verifies that deleting the final item via rear resets indices back to structural zero.
 */
void test_delete_rear_of_last_remaining_element_should_reset_indices(void)
{
    // Arrange.
    Deque* target_queue = initialize(4);

    insert_front(target_queue, 88);

    int destination_buffer = 0;

    // Act.
    bool execution_result = delete_rear(target_queue, &destination_buffer);

    // Assert.
    TEST_ASSERT_TRUE(execution_result);
    TEST_ASSERT_EQUAL_UINT(0, target_queue->front);
    TEST_ASSERT_EQUAL_UINT(0, target_queue->rear);
    TEST_ASSERT_EQUAL_INT(0, target_queue->size);

    clean(target_queue);
}

/**
 * @brief Test 12: Verifies that deletion calls on empty deques fail gracefully without mutations.
 */
void test_delete_front_on_empty_deque_should_fail_gracefully(void)
{
    // Arrange.
    Deque* target_queue = initialize(4);

    int destination_buffer = -999;

    // Act.
    bool execution_result = delete_front(target_queue, &destination_buffer);

    // Assert.
    TEST_ASSERT_FALSE(execution_result);
    TEST_ASSERT_EQUAL_INT(-999, destination_buffer);

    clean(target_queue);
}

/**
 * @brief Test 13: Verifies that rear deletion calls on empty deques fail gracefully without mutations.
 */
void test_delete_rear_on_empty_deque_should_fail_gracefully(void)
{
    // Arrange.
    Deque* target_queue = initialize(4);

    int destination_buffer = -999;

    // Act.
    bool execution_result = delete_rear(target_queue, &destination_buffer);

    // Assert.
    TEST_ASSERT_FALSE(execution_result);
    TEST_ASSERT_EQUAL_INT(-999, destination_buffer);

    clean(target_queue);
}

/**
 * @brief Test 14: Verifies null-pointer tracking defense parameters within input data functions.
 */
void test_insert_front_with_null_pointer_queue_should_return_false(void)
{
    // Arrange.
    Deque* unallocated_queue = NULL;

    int input_payload = 15;

    // Act.
    bool execution_result = insert_front(unallocated_queue, input_payload);

    // Assert.
    TEST_ASSERT_FALSE(execution_result);
}

/**
 * @brief Test 15: Verifies null-pointer tracking defense parameters within insertion properties.
 */
void test_insert_rear_with_null_pointer_queue_should_return_false(void)
{
    // Arrange.
    Deque* unallocated_queue = NULL;

    int input_payload = 25;

    // Act.
    bool execution_result = insert_rear(unallocated_queue, input_payload);

    // Assert.
    TEST_ASSERT_FALSE(execution_result);
}

/**
 * @brief Test 16: Verifies front deletion handles null destination buffers without critical crash exceptions.
 */
void test_delete_front_with_null_destination_buffer_should_succeed_mutating_size(void)
{
    // Arrange.
    Deque* target_queue = initialize(4);

    insert_rear(target_queue, 777);

    // Act.
    bool execution_result = delete_front(target_queue, NULL);

    // Assert.
    TEST_ASSERT_TRUE(execution_result);
    TEST_ASSERT_EQUAL_INT(0, target_queue->size);

    clean(target_queue);
}

/**
 * @brief Test 17: Verifies rear deletion handles null destination buffers without critical crash exceptions.
 */
void test_delete_rear_with_null_destination_buffer_should_succeed_mutating_size(void)
{
    // Arrange.
    Deque* target_queue = initialize(4);

    insert_rear(target_queue, 888);

    // Act.
    bool execution_result = delete_rear(target_queue, NULL);

    // Assert.
    TEST_ASSERT_TRUE(execution_result);
    TEST_ASSERT_EQUAL_INT(0, target_queue->size);

    clean(target_queue);
}

/**
 * @brief Test 18: Verifies interleaved operations wrap around limits correctly without fragmentation leaks.
 */
void test_interleaved_insert_and_delete_operations_should_wrap_safely(void)
{
    // Arrange.
    Deque* target_queue = initialize(3);

    int extraction_container = 0;

    // Act.
    insert_rear(target_queue, 1);
    insert_rear(target_queue, 2);

    delete_front(target_queue, &extraction_container);

    insert_rear(target_queue, 3);
    insert_rear(target_queue, 4);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(3, target_queue->capacity);
    TEST_ASSERT_EQUAL_INT(3, target_queue->size);

    bool terminal_push = insert_rear(target_queue, 5);

    TEST_ASSERT_TRUE(terminal_push);

    TEST_ASSERT_EQUAL_UINT(6, target_queue->capacity);
    TEST_ASSERT_EQUAL_INT(4, target_queue->size);
    
    delete_front(target_queue, &extraction_container);

    TEST_ASSERT_EQUAL_INT(2, extraction_container);
    
    clean(target_queue);
}

/**
 * @brief Test 19: Verifies data sequencing order consistency throughout front-loaded inputs.
 */
void test_front_insertion_sequence_ordering_integrity(void)
{
    // Arrange.
    Deque* target_queue = initialize(4);

    int output_verification = 0;

    // Act.
    insert_front(target_queue, 1000);
    insert_front(target_queue, 2000);
    insert_front(target_queue, 3000);

    // Assert.
    delete_front(target_queue, &output_verification);
    TEST_ASSERT_EQUAL_INT(3000, output_verification);
    delete_front(target_queue, &output_verification);
    TEST_ASSERT_EQUAL_INT(2000, output_verification);
    delete_front(target_queue, &output_verification);
    TEST_ASSERT_EQUAL_INT(1000, output_verification);
    
    clean(target_queue);
}

/**
 * @brief Test 20: Verifies that cleaning a null reference structural object does not generate runtime exceptions.
 */
void test_clean_with_null_pointer_should_return_without_exception(void)
{
    // Arrange.
    Deque* empty_null_reference = NULL;

    // Act & Assert.
    clean(empty_null_reference);
    
    // Explicit pass confirmation if line executed cleanly.
    TEST_PASS();
}

/**
 * @brief Application execution driver entry point mapping test runner sequences.
 */
int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_initialize_with_valid_capacity_should_allocate_successfully);
    RUN_TEST(test_initialize_with_zero_capacity_should_return_null);
    RUN_TEST(test_insert_front_on_empty_deque_should_set_indices_to_zero);
    RUN_TEST(test_insert_rear_on_empty_deque_should_set_indices_to_zero);
    RUN_TEST(test_insert_front_multiple_items_should_wrap_index_circularly);
    RUN_TEST(test_insert_rear_multiple_items_should_increment_index_sequentially);
    RUN_TEST(test_insert_rear_beyond_capacity_should_trigger_dynamic_resize);
    RUN_TEST(test_delete_front_should_extract_head_element_and_advance_index);
    RUN_TEST(test_delete_rear_should_extract_tail_element_and_regress_index);
    RUN_TEST(test_delete_front_of_last_remaining_element_should_reset_indices);
    RUN_TEST(test_delete_rear_of_last_remaining_element_should_reset_indices);
    RUN_TEST(test_delete_front_on_empty_deque_should_fail_gracefully);
    RUN_TEST(test_delete_rear_on_empty_deque_should_fail_gracefully);
    RUN_TEST(test_insert_front_with_null_pointer_queue_should_return_false);
    RUN_TEST(test_insert_rear_with_null_pointer_queue_should_return_false);
    RUN_TEST(test_delete_front_with_null_destination_buffer_should_succeed_mutating_size);
    RUN_TEST(test_delete_rear_with_null_destination_buffer_should_succeed_mutating_size);
    RUN_TEST(test_interleaved_insert_and_delete_operations_should_wrap_safely);
    RUN_TEST(test_front_insertion_sequence_ordering_integrity);
    RUN_TEST(test_clean_with_null_pointer_should_return_without_exception);

    // Destroy the session.
    return UNITY_END();
}