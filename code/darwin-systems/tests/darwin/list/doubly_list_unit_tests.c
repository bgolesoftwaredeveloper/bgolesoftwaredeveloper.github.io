/**
 * @file        :   doubly_list_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit test suite for the Darwin generic Doubly Linked List.
 * @details     :   This suite utilizes the Unity Test Framework to validate 20 distinct 
 * behaviors of the doubly linked list. It specifically targets:
 * - Memory safety (callbacks and cleanup).
 * - Structural integrity (head/tail/node pointer consistency).
 * - Edge case handling (empty lists, NULL inputs).
 * - Core API functionality (push, pop, find, and traversal).
 * Compile:
 * clang ./tests/darwin/list/doubly_list_unit_tests.c ./src/darwin/list/doubly-linked-list/doubly_linked_list.c ./src/unity.c -Iinclude -o ./bin/doubly_list_unit_test
 * @version     :   1.0
 * @date        :   2026-05-13
 */
#include <stdio.h>

#include "unity.h"
#include "darwin/list/doubly_linked_list.h"

static int count = 0;

/**
 * @brief Mock callback to verify traversal.
 */
void count_visits(void* data) 
{ 
    count++; 
}

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
 * @brief Verifies that initialize correctly sets up the list manager.
 */
void test_initialize_should_set_correct_initial_state(void)
{
    // Arrange.
    List* list = NULL;

    // Act.
    list = initialize(destroy);

    // Assert.
    TEST_ASSERT_NOT_NULL(list);
    TEST_ASSERT_EQUAL_INT(0, list->size);
    TEST_ASSERT_NULL(list->head);
    TEST_ASSERT_NULL(list->tail);

    clear(list);
}

/**
 * @brief Verifies initialize returns NULL if no cleanup function is provided.
 */
void test_initialize_should_fail_if_callback_is_null(void)
{
    // Arrange.
    List* list = NULL;

    // Act.
    list = initialize(NULL);

    // Assert.
    TEST_ASSERT_NULL(list);
}

/**
 * @brief Verifies that push_back correctly handles the first element.
 */
void test_push_back_should_work_on_empty_list(void)
{
    // Arrange.
    List* list = initialize(destroy);

    char* data = make_text("First");

    // Act.
    bool result = push_back(list, data);

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(1, list->size);
    TEST_ASSERT_EQUAL_PTR(data, list->head->data);

    clear(list);
}

/**
 * @brief Verifies that push_front correctly handles the first element.
 */
void test_push_front_should_work_on_empty_list(void)
{
    // Arrange.
    List* list = initialize(destroy);

    char* data = make_text("Front");

    // Act.
    bool result = push_front(list, data);

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_PTR(list->head, list->tail);

    clear(list);
}

/**
 * @brief Verifies push_back order when multiple items are added.
 */
void test_push_back_should_maintain_correct_order(void)
{
    // Arrange.
    List* list = initialize(destroy);

    push_back(list, make_text("A"));

    // Act.
    push_back(list, make_text("B"));

    // Assert.
    TEST_ASSERT_EQUAL_STRING("A", (char*)list->head->data);
    TEST_ASSERT_EQUAL_STRING("B", (char*)list->tail->data);

    clear(list);
}

/**
 * @brief Verifies push_front order when multiple items are added.
 */
void test_push_front_should_maintain_correct_order(void)
{
    // Arrange.
    List* list = initialize(destroy);

    push_front(list, make_text("A"));

    // Act.
    push_front(list, make_text("B"));

    // Assert.
    TEST_ASSERT_EQUAL_STRING("B", (char*)list->head->data);
    TEST_ASSERT_EQUAL_STRING("A", (char*)list->tail->data);

    clear(list);
}

/**
 * @brief Verifies pop_front removes and returns the correct data.
 */
void test_pop_front_should_return_data_and_decrement_size(void)
{
    // Arrange.
    List* list = initialize(destroy);

    push_back(list, make_text("Poppable"));

    // Act.
    char* data = (char*)pop_front(list);

    // Assert.
    TEST_ASSERT_EQUAL_STRING("Poppable", data);
    TEST_ASSERT_EQUAL_INT(0, list->size);

    destroy(data);
    clear(list);
}

/**
 * @brief Verifies pop_back removes and returns the correct data.
 */
void test_pop_back_should_return_data_and_decrement_size(void)
{
    // Arrange.
    List* list = initialize(destroy);

    push_back(list, make_text("TailData"));

    // Act.
    char* data = (char*)pop_back(list);

    // Assert.
    TEST_ASSERT_EQUAL_STRING("TailData", data);
    TEST_ASSERT_NULL(list->tail);

    destroy(data);
    clear(list);
}

/**
 * @brief Verifies pop functions return NULL for empty lists.
 */
void test_pops_should_return_null_on_empty_list(void)
{
    // Arrange.
    List* list = initialize(destroy);

    // Act.
    void* front_data = pop_front(list);
    void* rear_data = pop_back(list);

    // Assert.
    TEST_ASSERT_NULL(front_data);
    TEST_ASSERT_NULL(rear_data);
    
    clear(list);
}

/**
 * @brief Verifies find locates an existing string.
 */
void test_find_should_return_pointer_to_existing_data(void)
{
    // Arrange.
    List* list = initialize(destroy);

    push_back(list, make_text("Target"));

    // Act.
    void* result = find(list, is_match, "Target");

    // Assert.
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("Target", (char*)result);

    clear(list);
}

/**
 * @brief Verifies find returns NULL when data is missing.
 */
void test_find_should_return_null_for_missing_data(void)
{
    // Arrange.
    List* list = initialize(destroy);

    push_back(list, make_text("Existing"));

    // Act.
    void* result = find(list, is_match, "Missing");

    // Assert.
    TEST_ASSERT_NULL(result);

    clear(list);
}

/**
 * @brief Verifies forEach traverses every node in a list.
 */
void test_foreach_should_execute_for_every_element(void)
{
    // Arrange.
    List* list = initialize(destroy);

    push_back(list, make_text("1"));
    push_back(list, make_text("2"));
    push_back(list, make_text("3"));

    // Act.
    forEach(list, count_visits);

    // Assert.
    TEST_ASSERT_EQUAL_INT(3, count);

    clear(list);
}

/**
 * @brief Verifies pointer integrity between two adjacent nodes.
 */
void test_node_linkage_should_be_bi_directional(void)
{
    // Arrange.
    List* list = initialize(destroy);

    push_back(list, make_text("NodeA"));
    push_back(list, make_text("NodeB"));

    // Act.
    Node* head = list->head;
    Node* tail = list->tail;

    // Assert.
    TEST_ASSERT_EQUAL_PTR(tail, head->next);
    TEST_ASSERT_EQUAL_PTR(head, tail->previous);

    clear(list);
}

/**
 * @brief Verifies that head and tail are the same when size is 1.
 */
void test_head_and_tail_equivalence_single_element(void)
{
    // Arrange.
    List* list = initialize(destroy);

    // Act.
    push_back(list, make_text("OnlyOne"));

    // Assert.
    TEST_ASSERT_EQUAL_PTR(list->head, list->tail);

    clear(list);
}

/**
 * @brief Verifies list state after popping the only element.
 */
void test_list_state_after_popping_to_empty(void)
{
    // Arrange.
    List* list = initialize(destroy);

    push_back(list, make_text("Temp"));

    // Act.
    destroy(pop_front(list));

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, list->size);
    TEST_ASSERT_NULL(list->head);
    TEST_ASSERT_NULL(list->tail);

    clear(list);
}

/**
 * @brief Verifies push functions reject NULL data.
 */
void test_push_functions_should_reject_null_data(void)
{
    // Arrange.
    List* list = initialize(destroy);

    // Act.
    bool res1 = push_back(list, NULL);
    bool res2 = push_front(list, NULL);

    // Assert.
    TEST_ASSERT_FALSE(res1);
    TEST_ASSERT_FALSE(res2);
    TEST_ASSERT_EQUAL_INT(0, list->size);

    clear(list);
}

/**
 * @brief Verifies integrity of head/tail after popping front of a 2-item list.
 */
void test_pop_front_on_two_items_updates_pointers(void)
{
    // Arrange.
    List* list = initialize(destroy);

    push_back(list, make_text("A"));
    push_back(list, make_text("B"));

    // Act.
    destroy(pop_front(list));

    // Assert.
    TEST_ASSERT_EQUAL_PTR(list->head, list->tail);
    TEST_ASSERT_EQUAL_STRING("B", (char*)list->head->data);

    clear(list);
}

/**
 * @brief Verifies find handles a NULL predicate safely.
 */
void test_find_should_handle_null_predicate_safely(void)
{
    // Arrange.
    List* list = initialize(destroy);

    push_back(list, make_text("X"));

    // Act.
    void* result = find(list, NULL, "X");

    // Assert.
    TEST_ASSERT_NULL(result);

    clear(list);
}

/**
 * @brief Verifies forEach handles a NULL callback safely.
 */
void test_foreach_should_handle_null_callback_safely(void)
{
    // Arrange.
    List* list = initialize(destroy);

    push_back(list, make_text("Y"));

    // Act.
    forEach(list, NULL);

    // Assert.
    TEST_PASS();

    clear(list);
}

/**
 * @brief Verifies clear execution on a populated list.
 */
void test_clear_should_execute_without_error(void)
{
    // Arrange.
    List* list = initialize(destroy);

    push_back(list, make_text("CleanupTarget"));

    // Act.
    clear(list);

    // Assert.
    TEST_PASS();
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_initialize_should_set_correct_initial_state);
    RUN_TEST(test_initialize_should_fail_if_callback_is_null);
    RUN_TEST(test_push_back_should_work_on_empty_list);
    RUN_TEST(test_push_front_should_work_on_empty_list);
    RUN_TEST(test_push_back_should_maintain_correct_order);
    RUN_TEST(test_push_front_should_maintain_correct_order);
    RUN_TEST(test_pop_front_should_return_data_and_decrement_size);
    RUN_TEST(test_pop_back_should_return_data_and_decrement_size);
    RUN_TEST(test_pops_should_return_null_on_empty_list);
    RUN_TEST(test_find_should_return_pointer_to_existing_data);
    RUN_TEST(test_find_should_return_null_for_missing_data);
    RUN_TEST(test_foreach_should_execute_for_every_element);
    RUN_TEST(test_node_linkage_should_be_bi_directional);
    RUN_TEST(test_head_and_tail_equivalence_single_element);
    RUN_TEST(test_list_state_after_popping_to_empty);
    RUN_TEST(test_push_functions_should_reject_null_data);
    RUN_TEST(test_pop_front_on_two_items_updates_pointers);
    RUN_TEST(test_find_should_handle_null_predicate_safely);
    RUN_TEST(test_foreach_should_handle_null_callback_safely);
    RUN_TEST(test_clear_should_execute_without_error);
    
    // Destroy the session.
    return UNITY_END();
}