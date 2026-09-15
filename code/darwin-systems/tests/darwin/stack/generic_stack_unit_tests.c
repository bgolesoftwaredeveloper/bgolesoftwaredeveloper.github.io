/**
 * @file        :   generic_stack_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Comprehensive unit test suite for the Generic Stack.
 * @details     :   This module uses the Unity Test Framework to validate
 * dynamic resizing, type-agnostic storage, memory
 * integrity, and edge cases for the Generic Stack.
 * Compile:
 * clang ./tests/darwin/stack/generic_stack_unit_tests.c ./src/darwin/stack/generic-stack/generic_stack.c ./src/unity.c -Iinclude -o ./bin/generic_stack_unit_test
 * @version     :   1.0
 * @date        :   2026-04-19
 */
#include <stdio.h>
#include <limits.h>

// Include header files.
#include "unity.h"
#include "darwin/stack/generic_stack.h"

/**
 * @brief Sets up resources before each test.
 */
void setUp(void)
{
    ;
    ;
}

/**
 * @brief Cleans up resources after each test.
 */
void tearDown(void)
{
    ;
    ;
}

/**
 * @brief Tests basic initialization of the stack.
 */
void test_stack_initialization(void)
{
    // Arrange.
    GenericStack stack;

    // Act.
    bool result = initialize(&stack, sizeof(int));

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(0, retrieve_stack_size(&stack));
    TEST_ASSERT_TRUE(is_empty(&stack));

    clear(&stack);
}

/**
 * @brief Tests pushing and popping a single integer.
 */
void test_stack_push_pop_integer(void)
{
    // Arrange.
    GenericStack stack;

    initialize(&stack, sizeof(int));

    int value = 42;
    int result = 0;

    // Act.
    push(&stack, &value);
    bool pop_result = pop(&stack, &result);

    // Assert.
    TEST_ASSERT_TRUE(pop_result);
    TEST_ASSERT_EQUAL_INT(value, result);
    TEST_ASSERT_TRUE(is_empty(&stack));

    clear(&stack);
}

/**
 * @brief Tests the LIFO (Last-In-First-Out) behavior.
 */
void test_stack_lifo_behavior(void)
{
    // Arrange.
    GenericStack stack;

    initialize(&stack, sizeof(int));

    int values[] = {10, 20, 30};
    int output;

    // Act & Assert.
    for (int index = 0; index < 3; index++)
    {
        push(&stack, &values[index]);
    }

    pop(&stack, &output);

    TEST_ASSERT_EQUAL_INT(30, output);

    pop(&stack, &output);

    TEST_ASSERT_EQUAL_INT(20, output);

    pop(&stack, &output);

    TEST_ASSERT_EQUAL_INT(10, output);

    clear(&stack);
}

/**
 * @brief Tests automatic dynamic resizing (pushing beyond initial capacity).
 */
void test_stack_dynamic_resizing(void)
{
    // Arrange.
    GenericStack stack;

    initialize(&stack, sizeof(int));

    int push_count = 20;

    // Act.
    for (int index = 0; index < push_count; index++)
    {
        push(&stack, &index);
    }

    // Assert.
    TEST_ASSERT_EQUAL_INT(push_count, retrieve_stack_size(&stack));

    int top;

    peek(&stack, &top);

    TEST_ASSERT_EQUAL_INT(19, top);

    clear(&stack);
}

/**
 * @brief Tests peeking at the top element without removing it.
 */
void test_stack_peek(void)
{
    // Arrange.
    GenericStack stack;

    initialize(&stack, sizeof(int));

    int value = 99;
    int peeked = 0;

    // Act.
    push(&stack, &value);
    bool result = peek(&stack, &peeked);

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(value, peeked);
    TEST_ASSERT_EQUAL_INT(1, retrieve_stack_size(&stack));

    clear(&stack);
}

/**
 * @brief Tests popping from an empty stack.
 */
void test_stack_pop_empty(void)
{
    // Arrange.
    GenericStack stack;

    initialize(&stack, sizeof(int));

    int target;

    // Act.
    bool result = pop(&stack, &target);

    // Assert.
    TEST_ASSERT_FALSE(result);

    clear(&stack);
}

/**
 * @brief Tests stack behavior with a custom struct (Genericity).
 */
void test_stack_with_custom_struct(void)
{
    // Arrange.
    typedef struct
    {
        char id;
        float value;
    } Node;

    GenericStack stack;

    initialize(&stack, sizeof(Node));

    Node node = {'A', 1.5f};
    Node result;

    // Act.
    push(&stack, &node);
    pop(&stack, &result);

    // Assert.
    TEST_ASSERT_EQUAL_CHAR('A', result.id);
    TEST_ASSERT_EQUAL_FLOAT(1.5f, result.value);

    clear(&stack);
}

/**
 * @brief Tests the reserve function to pre-allocate capacity.
 */
void test_stack_reserve_capacity(void)
{
    // Arrange.
    GenericStack stack;

    initialize(&stack, sizeof(int));

    // Act.
    bool result = reserve(&stack, 50);

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_TRUE(stack.capacity >= 50);

    clear(&stack);
}

/**
 * @brief Tests the shrink function to reduce memory footprint.
 */
void test_stack_shrink_to_fit(void)
{
    // Arrange.
    GenericStack stack;

    initialize(&stack, sizeof(int));

    int value = 5;

    reserve(&stack, 100);

    // Act.
    push(&stack, &value);
    bool result = shrink(&stack);

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(1, stack.capacity);
    TEST_ASSERT_EQUAL_INT(1, stack.size);

    clear(&stack);
}

/**
 * @brief Tests clearing the stack.
 */
void test_stack_clear(void)
{
    // Arrange.
    GenericStack stack;

    initialize(&stack, sizeof(int));

    int value = 1;

    push(&stack, &value);

    // Act.
    clear(&stack);

    // Assert.
    TEST_ASSERT_NULL(stack.data);
    TEST_ASSERT_EQUAL_INT(0, stack.size);
    TEST_ASSERT_EQUAL_INT(0, stack.capacity);
}

/**
 * @brief Tests behavior with large data types (Strings).
 */
void test_stack_large_data_type(void)
{
    // Arrange.
    char *message = "Darwin Systems Stack Test";

    GenericStack stack;

    initialize(&stack, sizeof(char *));

    char *result;

    // Act.
    push(&stack, &message);
    pop(&stack, &result);

    // Assert.
    TEST_ASSERT_EQUAL_STRING(message, result);

    clear(&stack);
}

/**
 * @brief Tests initializing with invalid item size.
 */
void test_stack_init_invalid_size(void)
{
    // Arrange.
    GenericStack stack;

    // Act.
    bool result = initialize(&stack, 0);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief Tests pushing NULL into the stack.
 */
void test_stack_push_null_item(void)
{
    // Arrange.
    GenericStack stack;

    initialize(&stack, sizeof(int));

    // Act.
    bool result = push(&stack, NULL);

    // Assert.
    TEST_ASSERT_FALSE(result);

    clear(&stack);
}

/**
 * @brief Tests pop without a target pointer (Silent Pop).
 */
void test_stack_silent_pop(void)
{
    // Arrange.
    GenericStack stack;

    initialize(&stack, sizeof(int));

    int value = 10;

    push(&stack, &value);

    // Act.
    bool result = pop(&stack, NULL);

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(0, retrieve_stack_size(&stack));

    clear(&stack);
}

/**
 * @brief Tests stack behavior under mixed large pushes and pops.
 */
void test_stack_stress_mixed_ops(void)
{
    // Arrange.
    GenericStack stack;

    initialize(&stack, sizeof(int));

    int count = 100;

    // Act.
    for (int index = 0; index < count; index++)
    {
        push(&stack, &index);
    }

    for (int index = 0; index < count / 2; index++)
    {
        pop(&stack, NULL);
    }

    // Assert.
    TEST_ASSERT_EQUAL_INT(50, retrieve_stack_size(&stack));

    int top;

    peek(&stack, &top);

    TEST_ASSERT_EQUAL_INT(49, top);

    clear(&stack);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_stack_initialization);
    RUN_TEST(test_stack_push_pop_integer);
    RUN_TEST(test_stack_lifo_behavior);
    RUN_TEST(test_stack_dynamic_resizing);
    RUN_TEST(test_stack_peek);
    RUN_TEST(test_stack_pop_empty);
    RUN_TEST(test_stack_with_custom_struct);
    RUN_TEST(test_stack_reserve_capacity);
    RUN_TEST(test_stack_shrink_to_fit);
    RUN_TEST(test_stack_clear);
    RUN_TEST(test_stack_large_data_type);
    RUN_TEST(test_stack_init_invalid_size);
    RUN_TEST(test_stack_push_null_item);
    RUN_TEST(test_stack_silent_pop);
    RUN_TEST(test_stack_stress_mixed_ops);

    // Destroy the session.
    return UNITY_END();
}