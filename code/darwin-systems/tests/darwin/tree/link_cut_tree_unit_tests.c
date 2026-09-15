/**
 * @file        :   link_cut_tree_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of a Unity validation test suite for a Link-Cut Tree.
 * @details     :   This suite validates structural safety, amortized splay path balance, 
 * lazy propagation inversion, edge addition/removal mutations, path metric 
 * maximum aggregations, and component lifecycle clearing operations.
 * Compile:
 * clang ./tests/darwin/tree/link_cut_tree_unit_tests.c ./src/darwin/tree/link-cut-tree/link_cut_tree.c ./src/unity.c -Iinclude -o ./bin/link_cut_tree_unit_test
 * @version     :   1.0
 * @date        :   2026-05-22
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Include header files.
#include "unity.h"
#include "darwin/tree/link_cut_tree.h"

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
 * @brief Verifies node parameters initialize perfectly upon construction.
 */
void test_construct_should_initialize_node_properties_correctly(void)
{
    // Arrange.
    size_t id = 101;
    
    int value = 45;

    // Act.
    Node* node = construct(id, value);

    // Assert.
    TEST_ASSERT_NOT_NULL(node);
    TEST_ASSERT_EQUAL_UINT64(101, node->id);
    TEST_ASSERT_EQUAL_INT(45, node->value);
    TEST_ASSERT_EQUAL_INT(45, node->maximum);
    TEST_ASSERT_FALSE(node->is_reversed);
    TEST_ASSERT_NULL(node->parent);
    TEST_ASSERT_NULL(node->children[0]);
    TEST_ASSERT_NULL(node->children[1]);

    clean(node);
}

/**
 * @brief Verifies single-node find() targets itself as the absolute root.
 */
void test_find_on_isolated_node_should_return_itself(void)
{
    // Arrange.
    Node* node = construct(1, 10);

    // Act.
    Node* result = find(node);

    // Assert.
    TEST_ASSERT_EQUAL_PTR(node, result);

    clean(node);
}

/**
 * @brief Verifies linking two nodes successfully builds a relationship parent pointer.
 */
void test_link_should_connect_two_isolated_nodes(void)
{
    // Arrange.
    Node* source = construct(1, 10);
    Node* target = construct(2, 20);

    // Act.
    bool status = link(source, target);

    // Assert.
    TEST_ASSERT_TRUE(status);

    Node* root = find(source);

    TEST_ASSERT_EQUAL_PTR(target, root);

    clean(root);
}

/**
 * @brief Verifies self-linking is rejected cleanly to block internal cycle generation.
 */
void test_link_node_to_itself_should_fail(void)
{
    // Arrange.
    Node* node = construct(1, 10);

    // Act.
    bool status = link(node, node);

    // Assert.
    TEST_ASSERT_FALSE(status);

    clean(node);
}

/**
 * @brief Verifies linking components already connected in the same tree is blocked.
 */
void test_link_already_connected_nodes_should_fail(void)
{
    // Arrange.
    Node* ten = construct(1, 10);
    Node* twenty = construct(2, 20);

    link(ten, twenty);

    // Act.
    bool status = link(twenty, ten);

    // Assert.
    TEST_ASSERT_FALSE(status);

    clean(find(ten));
}

/**
 * @brief Verifies assign_root() updates the absolute root of a tree component.
 */
void test_assign_root_should_alter_component_absolute_root(void)
{
    // Arrange.
    Node* ten = construct(1, 10);
    Node* twenty = construct(2, 20);

    link(ten, twenty);

    TEST_ASSERT_EQUAL_PTR(twenty, find(ten));

    // Act.
    bool status = assign_root(ten);

    // Assert.
    TEST_ASSERT_TRUE(status);
    TEST_ASSERT_EQUAL_PTR(ten, find(twenty));

    clean(ten);
}

/**
 * @brief Verifies path maximum evaluates correctly on an isolated root node.
 */
void test_query_on_single_node_should_return_value(void)
{
    // Arrange.
    Node* node = construct(1, 99);

    bool flag = false;

    // Act.
    int maximum = query(node, node, &flag);

    // Assert.
    TEST_ASSERT_TRUE(flag);
    TEST_ASSERT_EQUAL_INT(99, maximum);

    clean(node);
}

/**
 * @brief Verifies path maximum updates correctly after linear tree extensions.
 */
void test_query_on_linear_path_should_return_correct_maximum(void)
{
    // Arrange.
    Node* fifteen = construct(1, 15);
    Node* fourty_five = construct(2, 45);
    Node* thirty = construct(3, 30);

    link(fifteen, fourty_five);
    link(fourty_five, thirty);
    
    bool flag = false;

    // Act.
    int maximum = query(fifteen, thirty, &flag);

    // Assert.
    TEST_ASSERT_TRUE(flag);
    TEST_ASSERT_EQUAL_INT(45, maximum);

    clean(find(fifteen));
}

/**
 * @brief Verifies query flags failure cleanly when tracking entirely isolated trees.
 */
void test_query_on_disconnected_nodes_should_fail_and_return_zero(void)
{
    // Arrange.
    Node* ten = construct(1, 10);
    Node* twenty = construct(2, 20);

    bool flag = true;

    // Act.
    int maximum = query(ten, twenty, &flag);

    // Assert.
    TEST_ASSERT_FALSE(flag);
    TEST_ASSERT_EQUAL_INT(0, maximum);

    clean(ten);
    clean(twenty);
}

/**
 * @brief Verifies cutting a valid active edge separates components cleanly.
 */
void test_cut_should_disconnect_valid_edge(void)
{
    // Arrange.
    Node* ten = construct(1, 10);
    Node* twenty = construct(2, 20);

    link(ten, twenty);

    // Act.
    bool status = cut(ten, twenty);

    // Assert.
    TEST_ASSERT_TRUE(status);
    TEST_ASSERT_EQUAL_PTR(ten, find(ten));
    TEST_ASSERT_EQUAL_PTR(twenty, find(twenty));

    clean(ten);
    clean(twenty);
}

/**
 * @brief Verifies cutting an unlinked/non-existent edge returns false.
 */
void test_cut_on_non_existent_edge_should_return_false(void)
{
    // Arrange.
    Node* ten = construct(1, 10);
    Node* twenty = construct(2, 20);

    // Act.
    bool status = cut(ten, twenty);

    // Assert.
    TEST_ASSERT_FALSE(status);

    clean(ten);
    clean(twenty);
}

/**
 * @brief Verifies erase() completely frees and detaches an isolated node structure.
 */
void test_erase_should_isolate_and_free_node(void)
{
    // Arrange.
    Node* ten = construct(1, 10);
    Node* twenty = construct(2, 20);

    link(ten, twenty);

    // Act.
    bool status = erase(ten);

    // Assert.
    TEST_ASSERT_TRUE(status);

    Node* remainder = find(twenty);

    TEST_ASSERT_EQUAL_PTR(twenty, remainder);

    clean(remainder);
}

/**
 * @brief Verifies null pointer protection guards inside structural entry points.
 */
void test_api_functions_should_handle_null_inputs_gracefully(void)
{
    // Arrange.
    bool flag = true;

    // Act & Assert.
    TEST_ASSERT_NULL(find(NULL));
    TEST_ASSERT_FALSE(link(NULL, NULL));
    TEST_ASSERT_FALSE(cut(NULL, NULL));
    TEST_ASSERT_FALSE(assign_root(NULL));
    TEST_ASSERT_FALSE(erase(NULL));
    TEST_ASSERT_EQUAL_INT(0, query(NULL, NULL, &flag));
    TEST_ASSERT_FALSE(flag);
}

/**
 * @brief Verifies large multi-tiered linear paths accurately track maximum aggregates.
 */
void test_long_path_maximum_query(void)
{
    // Arrange.
    Node* nodes[6];

    int values[6] = {0, 11, 77, 22, 99, 44};

    for (size_t index = 1; index <= 5; index++)
    {
        nodes[index] = construct(index, values[index]);
    }

    for (size_t index = 1; index < 5; index++)
    {
        link(nodes[index], nodes[index + 1]);
    }

    bool flag = false;

    // Act.
    int maximum = query(nodes[1], nodes[5], &flag);

    // Assert.
    TEST_ASSERT_TRUE(flag);
    TEST_ASSERT_EQUAL_INT(99, maximum);

    clean(find(nodes[1]));
}

/**
 * @brief Verifies tree structure changes preserve correct max metrics after a cut.
 */
void test_path_maximum_after_dynamic_cut_reconfiguration(void)
{
    // Arrange.
    Node* one_hundred = construct(1, 100);
    Node* twenty = construct(2, 20);
    Node* eighty = construct(3, 80);

    link(one_hundred, twenty);
    link(twenty, eighty);

    cut(one_hundred, twenty);

    bool flag = false;

    // Act.
    int maximum = query(twenty, eighty, &flag);

    // Assert.
    TEST_ASSERT_TRUE(flag);
    TEST_ASSERT_EQUAL_INT(80, maximum);

    clean(one_hundred);
    clean(find(twenty));
}

/**
 * @brief Verifies branch additions merge paths and adjust tracking metrics safely.
 */
void test_path_maximum_after_dynamic_link_reconfiguration(void)
{
    // Arrange.
    Node* ten = construct(1, 10);
    Node* two_hundred = construct(2, 200);
    Node* thirty = construct(3, 30);

    link(ten, two_hundred);

    bool flag = false;

    int maximum = query(ten, two_hundred, &flag);

    TEST_ASSERT_EQUAL_INT(200, maximum);

    // Act.
    link(two_hundred, thirty);

    maximum = query(ten, thirty, &flag);

    // Assert.
    TEST_ASSERT_EQUAL_INT(200, maximum);

    clean(find(ten));
}

/**
 * @brief Verifies tree shape adjustments do not damage tracking structures.
 */
void test_splay_ordering_invariance_under_multiple_splays(void)
{
    // Arrange.
    Node* five = construct(1, 5);
    Node* fifteen = construct(2, 15);
    Node* twenty_five = construct(3, 25);

    link(five, fifteen);
    link(fifteen, twenty_five);

    // Act.
    splay(five);
    splay(twenty_five);
    splay(fifteen);

    // Assert.
    TEST_ASSERT_EQUAL_PTR(find(twenty_five), find(five));

    clean(find(five));
}

/**
 * @brief Verifies parent link pointers shift correctly under structural splay operations.
 */
void test_access_updates_preferred_child_structures_correctly(void)
{
    // Arrange.
    Node* ten = construct(1, 10);
    Node* twenty = construct(2, 20);
    Node* thirty = construct(3, 30);

    link(ten, twenty);
    link(twenty, thirty);

    // Act.
    access(ten);
    splay(ten);

    // Assert.
    TEST_ASSERT_NULL(ten->children[1]);

    clean(find(ten));
}

/**
 * @brief Verifies lazy inversion flags cancel out correctly when applied twice.
 */
void test_double_assign_root_retains_original_root(void)
{
    // Arrange.
    Node* ten = construct(1, 10);
    Node* twenty = construct(2, 20);

    link(ten, twenty);

    // Act.
    assign_root(ten);
    assign_root(ten);

    // Assert.
    TEST_ASSERT_EQUAL_PTR(ten, find(ten));

    clean(find(ten));
}

/**
 * @brief Verifies fork trees resolve root identification lookups systematically.
 */
void test_forked_tree_find_root_validation(void)
{
    // Arrange.
    Node* root = construct(1, 50);
    Node* left_fork = construct(2, 20);
    Node* right_fork = construct(3, 30);

    link(left_fork, root);

    // Act.
    link(right_fork, root);

    // Assert.
    TEST_ASSERT_EQUAL_PTR(root, find(left_fork));
    TEST_ASSERT_EQUAL_PTR(root, find(right_fork));

    clean(root);
}

/**
 * @brief Verifies extreme metric scales function accurately across tree structures.
 */
void test_query_handles_negative_values_and_zero(void)
{
    // Arrange.
    Node* negative_fifty = construct(1, -50);
    Node* zero = construct(2, 0);
    Node* negative_ten = construct(3, -10);

    link(negative_fifty, zero);
    link(zero, negative_ten);
    bool flag = false;

    // Act.
    int maximum = query(negative_fifty, negative_ten, &flag);

    // Assert.
    TEST_ASSERT_TRUE(flag);
    TEST_ASSERT_EQUAL_INT(0, maximum);

    clean(find(negative_fifty));
}

/**
 * @brief Verifies node components retain original tracking ids through access loops.
 */
void test_node_ids_remain_immutable_throughout_lifecycles(void)
{
    // Arrange.
    Node* ten = construct(999, 10);
    Node* twenty = construct(888, 20);

    link(ten, twenty);

    // Act.
    access(ten);
    splay(ten);

    // Assert.
    TEST_ASSERT_EQUAL_UINT64(999, ten->id);
    TEST_ASSERT_EQUAL_UINT64(888, twenty->id);

    clean(find(ten));
}

/**
 * @brief Verifies flag remains false when query target is a null reference.
 */
void test_query_with_null_target_sets_flag_false(void)
{
    // Arrange.
    Node* ten = construct(1, 10);

    bool flag = true;

    // Act.
    int result = query(ten, NULL, &flag);

    // Assert.
    TEST_ASSERT_FALSE(flag);
    TEST_ASSERT_EQUAL_INT(0, result);

    clean(ten);
}

/**
 * @brief Verifies structural configurations stabilize safely during multi-stage linking loops.
 */
void test_sequential_linking_complex_chain_roots(void)
{
    // Arrange.
    Node* ten = construct(1, 10);
    Node* twenty = construct(2, 20);
    Node* thirty = construct(3, 30);
    Node* fourty = construct(4, 40);

    link(ten, twenty);
    link(thirty, fourty);

    // Act.
    link(twenty, thirty);

    // Assert.
    TEST_ASSERT_EQUAL_PTR(fourty, find(ten));

    clean(find(ten));
}

/**
 * @brief Verifies path maximum query functions correctly on reverse-ordered links.
 */
void test_query_path_maximum_under_reverse_order_linking(void)
{
    // Arrange.
    Node* fifty = construct(1, 50);
    Node* ten = construct(2, 10);
    Node* one_hundred = construct(3, 100);

    link(one_hundred, ten);
    link(ten, fifty);

    bool flag = false;

    // Act.
    int maximum = query(one_hundred, fifty, &flag);

    // Assert.
    TEST_ASSERT_TRUE(flag);
    TEST_ASSERT_EQUAL_INT(100, maximum);

    clean(find(one_hundred));
}

/**
 * @brief Application execution driver initialization for Unity testing framework.
 */
int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_construct_should_initialize_node_properties_correctly);
    RUN_TEST(test_find_on_isolated_node_should_return_itself);
    RUN_TEST(test_link_should_connect_two_isolated_nodes);
    RUN_TEST(test_link_node_to_itself_should_fail);
    RUN_TEST(test_link_already_connected_nodes_should_fail);
    RUN_TEST(test_assign_root_should_alter_component_absolute_root);
    RUN_TEST(test_query_on_single_node_should_return_value);
    RUN_TEST(test_query_on_linear_path_should_return_correct_maximum);
    RUN_TEST(test_query_on_disconnected_nodes_should_fail_and_return_zero);
    RUN_TEST(test_cut_should_disconnect_valid_edge);
    RUN_TEST(test_cut_on_non_existent_edge_should_return_false);
    RUN_TEST(test_erase_should_isolate_and_free_node);
    RUN_TEST(test_api_functions_should_handle_null_inputs_gracefully);
    RUN_TEST(test_long_path_maximum_query);
    RUN_TEST(test_path_maximum_after_dynamic_cut_reconfiguration);
    RUN_TEST(test_path_maximum_after_dynamic_link_reconfiguration);
    RUN_TEST(test_splay_ordering_invariance_under_multiple_splays);
    RUN_TEST(test_access_updates_preferred_child_structures_correctly);
    RUN_TEST(test_double_assign_root_retains_original_root);
    RUN_TEST(test_forked_tree_find_root_validation);
    RUN_TEST(test_query_handles_negative_values_and_zero);
    RUN_TEST(test_node_ids_remain_immutable_throughout_lifecycles);
    RUN_TEST(test_query_with_null_target_sets_flag_false);
    RUN_TEST(test_sequential_linking_complex_chain_roots);
    RUN_TEST(test_query_path_maximum_under_reverse_order_linking);

    // Destroy the session.
    return UNITY_END();
}