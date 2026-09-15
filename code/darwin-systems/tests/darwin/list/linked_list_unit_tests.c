/**
 * @file        :   linked_list_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit tests for a singly linked list implementation.
 * @details     :   Utilizes the Unity Test Framework to validate initialization,
 * appending, mapping, updating, erasing, and memory safety.
 * Compile:
 * clang ./tests/darwin/list/linked_list_unit_tests.c ./src/darwin/list/singly-linked-list/linked_list.c ./src/unity.c -Iinclude -o ./bin/linked_list_unit_test
 * @version     :   1.0
 * @date        :   2026-05-13
 */
#include <stdio.h>
#include <string.h>

#include "unity.h"
#include "darwin/list/linked_list.h"

/**
 * @brief       :   Helper function to square values for map tests.
 * @param       :   value: Pointer to the integer to modify.
 */
static void square_helper(int* value)
{
    *value *= *value;
}

static List* list = NULL;

/**
 * @brief Sets up resources before each test.
 */
void setUp(void)
{
    list = initialize();
}

/**
 * @brief Cleans up resources after each test.
 */
void tearDown(void)
{
    uninitialize(list);
}

/**
 * @brief : Tests that initialization sets up correct default values.
 */
void test_list_initial_state_compliance(void)
{
    // Assert.
    TEST_ASSERT_NOT_NULL(list);
    TEST_ASSERT_EQUAL_UINT(0, list->size);
    TEST_ASSERT_NULL(list->head);
}

/**
 * @brief : Tests basic appending of a single node.
 */
void test_list_single_element_append_persistence(void)
{
    // Arrange.
    int payload = 10;

    // Act.
    bool result = append(list, payload);

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(1, list->size);
    TEST_ASSERT_EQUAL_INT(payload, list->head->data);
}

/**
 * @brief : Tests sequential tail-insertion order.
 */
void test_list_multiple_element_sequencing(void)
{
    // Arrange.
    append(list, 100);
    append(list, 200);

    // Act.
    append(list, 300);

    // Assert.
    TEST_ASSERT_EQUAL_INT(3, list->size);
    TEST_ASSERT_EQUAL_INT(300, list->head->next->next->data);
}

/**
 * @brief : Tests robustness when operating on a NULL list reference.
 */
void test_list_null_input_safety_protocol(void)
{
    // Act.
    bool append_result = append(NULL, 10);
    bool update_result = update(NULL, 10, 20);

    // Assert.
    TEST_ASSERT_FALSE(append_result);
    TEST_ASSERT_FALSE(update_result);
}

/**
 * @brief : Tests target-based data update for an existing node.
 */
void test_list_value_overwrite_behavior(void)
{
    // Arrange.
    append(list, 55);

    // Act.
    bool result = update(list, 55, 99);

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(99, list->head->data);
}

/**
 * @brief : Tests that update fails gracefully when key is missing.
 */
void test_list_missing_value_update_failure(void)
{
    // Arrange.
    append(list, 10);

    // Act.
    bool result = update(list, 999, 0);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief : Verifies that only the first match is updated in multi-node lists.
 */
void test_list_update_first_occurrence_isolation(void)
{
    // Arrange.
    append(list, 7);
    append(list, 7);

    // Act.
    update(list, 7, 8);

    // Assert.
    TEST_ASSERT_EQUAL_INT(8, list->head->data);
    TEST_ASSERT_EQUAL_INT(7, list->head->next->data);
}

/**
 * @brief : Tests update behavior on an empty list container.
 */
void test_list_empty_container_update_resilience(void)
{
    // Act.
    bool result = update(list, 1, 2);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief : Tests removal of the entry point (head) node.
 */
void test_list_head_node_erase_logic(void)
{
    // Arrange.
    append(list, 1);
    append(list, 2);

    // Act.
    bool result = erase(list, 1);

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(2, list->head->data);
    TEST_ASSERT_EQUAL_INT(1, list->size);
}

/**
 * @brief : Tests removal of a node located between two other nodes.
 */
void test_list_mid_chain_node_removal(void)
{
    // Arrange.
    append(list, 10);
    append(list, 20);
    append(list, 30);

    // Act.
    bool result = erase(list, 20);

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(30, list->head->next->data);
    TEST_ASSERT_EQUAL_INT(2, list->size);
}

/**
 * @brief : Tests removal of the terminal (tail) node.
 */
void test_list_terminal_node_removal_safety(void)
{
    // Arrange.
    append(list, 5);
    append(list, 10);

    // Act.
    bool result = erase(list, 10);

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_NULL(list->head->next);
}

/**
 * @brief : Tests erase behavior when the target value does not exist.
 */
void test_list_non_existent_element_removal_safety(void)
{
    // Arrange.
    append(list, 1);

    // Act.
    bool result = erase(list, 404);

    // Assert.
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_EQUAL_UINT(1, list->size);
}

/**
 * @brief : Tests removal resilience on a completely empty list.
 */
void test_list_empty_container_erase_resilience(void)
{
    // Act.
    bool result = erase(list, 5);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief : Tests higher-order data transformation via map callback.
 */
void test_list_functional_mapping_transformation(void)
{
    // Arrange.
    append(list, 2);
    append(list, 4);

    // Act.
    map(list, square_helper);

    // Assert.
    TEST_ASSERT_EQUAL_INT(4, list->head->data);
    TEST_ASSERT_EQUAL_INT(16, list->head->next->data);
}

/**
 * @brief : Tests that map handles a NULL function pointer without a crash.
 */
void test_list_map_null_callback_protection(void)
{
    // Arrange.
    append(list, 100);

    // Act.
    map(list, NULL); 

    // Assert.
    TEST_ASSERT_EQUAL_INT(100, list->head->data);
}

/**
 * @brief : Tests that deallocation handles a NULL container reference.
 */
void test_list_deallocation_null_safety_check(void)
{
    // Act & Assert.
    uninitialize(NULL); 
}

/**
 * @brief : Tests map functionality on an unpopulated list.
 */
void test_list_map_empty_chain_stability(void)
{
    // Act.
    map(list, square_helper); 

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, list->size);
}

/**
 * @brief : Tests pointer integrity and loop bounds with a high node count.
 */
void test_list_high_volume_stress_saturation(void)
{
    // Act.
    for(int index = 0; index < 500; index++)
    {
        append(list, index);
    }

    // Assert.
    TEST_ASSERT_EQUAL_UINT(500, list->size);
}

/**
 * @brief : Verifies size variable synchronization after node removal.
 */
void test_list_size_counter_integrity_post_erase(void)
{
    // Arrange.
    append(list, 1);
    append(list, 1);

    // Act.
    erase(list, 1);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(1, list->size);
}

/**
 * @brief : Tests that nodes can be re-appended immediately after deletion.
 */
void test_list_deletion_reinsertion_cycle_reliability(void)
{
    // Arrange.
    append(list, 42);
    erase(list, 42);

    // Act.
    bool result = append(list, 42);

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(42, list->head->data);
    TEST_ASSERT_EQUAL_UINT(1, list->size);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_list_initial_state_compliance);
    RUN_TEST(test_list_single_element_append_persistence);
    RUN_TEST(test_list_multiple_element_sequencing);
    RUN_TEST(test_list_null_input_safety_protocol);
    RUN_TEST(test_list_value_overwrite_behavior);
    RUN_TEST(test_list_missing_value_update_failure);
    RUN_TEST(test_list_update_first_occurrence_isolation);
    RUN_TEST(test_list_empty_container_update_resilience);
    RUN_TEST(test_list_head_node_erase_logic);
    RUN_TEST(test_list_mid_chain_node_removal);
    RUN_TEST(test_list_terminal_node_removal_safety);
    RUN_TEST(test_list_non_existent_element_removal_safety);
    RUN_TEST(test_list_empty_container_erase_resilience);
    RUN_TEST(test_list_functional_mapping_transformation);
    RUN_TEST(test_list_map_null_callback_protection);
    RUN_TEST(test_list_deallocation_null_safety_check);
    RUN_TEST(test_list_map_empty_chain_stability);
    RUN_TEST(test_list_high_volume_stress_saturation);
    RUN_TEST(test_list_size_counter_integrity_post_erase);
    RUN_TEST(test_list_deletion_reinsertion_cycle_reliability);

    // Destroy the session.
    return UNITY_END();
}