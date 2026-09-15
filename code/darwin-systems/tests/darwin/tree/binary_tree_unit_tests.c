/**
 * @file        :   binary_search_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit testing suite for the custom Binary Search Tree (BST) API.
 * @details     :   Utilizes the Unity Test Framework to validate 20 distinct 
 * functional behaviors across insertion, search, erasure, 
 * traversals, and memory cleaning operations.
 * Compile:
 * clang ./tests/darwin/tree/binary_tree_unit_tests.c ./src/darwin/tree/binary-tree/binary_tree.c ./src/unity.c -Iinclude -o ./bin/binary_tree_unit_test
 * @version     :   1.0
 * @date        :   2026-04-19
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Include header files.
#include "unity.h"
#include "darwin/tree/binary_tree.h"

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
 * @brief Verifies insertion into an uninitialized tree pointer.
 */
void test_insert_payload_into_null_tree_establishes_new_root(void)
{
    // Arrange.
    Node* root = NULL;

    size_t value = 50;

    // Act.
    root = insert(root, value);

    // Assert.
    TEST_ASSERT_NOT_NULL(root);
    TEST_ASSERT_EQUAL_UINT(value, root->data);
    TEST_ASSERT_NULL(root->left);
    TEST_ASSERT_NULL(root->right);

    clean(root);
}

/**
 * @brief Verifies smaller values route to the left subtree.
 */
void test_insert_smaller_payload_routes_to_left_child(void)
{
    // Arrange.
    Node* root = NULL;

    size_t parent_value = 100;
    size_t left_value = 45;

    // Act.
    root = insert(root, parent_value);

    insert(root, left_value);

    // Assert.
    TEST_ASSERT_NOT_NULL(root->left);
    TEST_ASSERT_EQUAL_UINT(left_value, root->left->data);

    clean(root);
}

/**
 * @brief Verifies larger values route to the right subtree.
 */
void test_insert_larger_payload_routes_to_right_child(void)
{
    // Arrange.
    Node* root = NULL;

    size_t parent_value = 200;
    size_t right_value = 350;

    // Act.
    root = insert(root, parent_value);

    insert(root, right_value);

    // Assert.
    TEST_ASSERT_NOT_NULL(root->right);
    TEST_ASSERT_EQUAL_UINT(right_value, root->right->data);

    clean(root);
}

/**
 * @brief Verifies duplicate keys handle placement gracefully.
 */
void test_insert_duplicate_payload_appends_to_right_subtree(void)
{
    // Arrange.
    Node* root = NULL;

    size_t value1 = 75;
    size_t value2 = 75;

    // Act.
    root = insert(root, value1);

    insert(root, value2);

    // Assert.
    TEST_ASSERT_NOT_NULL(root->right);
    TEST_ASSERT_EQUAL_UINT(value2, root->right->data);

    clean(root);
}

/**
 * @brief Verifies searching on an empty tree.
 */
void test_search_on_empty_tree_returns_false(void)
{
    // Arrange.
    const Node* root = NULL;

    size_t target = 88;

    // Act.
    bool found = search(root, target);

    // Assert.
    TEST_ASSERT_FALSE(found);
}

/**
 * @brief Verifies locating data stored at the root position.
 */
void test_search_for_existing_root_payload_returns_true(void)
{
    // Arrange.
    Node* root = NULL;

    size_t value = 1000;

    // Act.
    root = insert(root, value);
    
    bool found = search(root, value);

    // Assert.
    TEST_ASSERT_TRUE(found);

    clean(root);
}

/**
 * @brief Verifies a successful left subtree search traversal.
 */
void test_search_for_deep_left_leaf_payload_returns_true(void)
{
    // Arrange.
    Node* root = NULL;

    size_t five_hundred = 500;
    size_t two_hundred_fifty = 250;
    size_t one_hundred_twenty_five = 125;

    bool found = false;

    // Act.
    root = insert(root, five_hundred);
    
    insert(root, two_hundred_fifty);
    insert(root, one_hundred_twenty_five);

    found = search(root, one_hundred_twenty_five);

    // Assert.
    TEST_ASSERT_TRUE(found);

    clean(root);
}

/**
 * @brief Verifies a successful right subtree search traversal.
 */
void test_search_for_deep_right_leaf_payload_returns_true(void)
{
    // Arrange.
    Node* root = NULL;

    size_t ten = 10;
    size_t twenty = 20;
    size_t thirty = 30;

    bool found = false;

    // Act.
    root = insert(root, ten);

    insert(root, twenty);
    insert(root, thirty);

    found = search(root, thirty);

    // Assert.
    TEST_ASSERT_TRUE(found);

    clean(root);
}

/**
 * @brief Verifies search failures when key is absent.
 */
void test_search_for_non_existent_payload_returns_false(void)
{
    // Arrange.
    Node* root = NULL;

    size_t target = 999;

    bool found = true;

    // Act.
    root = insert(root, 100);

    insert(root, 50);
    insert(root, 150);

    found = search(root, target);

    // Assert.
    TEST_ASSERT_FALSE(found);

    clean(root);
}

/**
 * @brief Verifies erasing an item from an unallocated tree.
 */
void test_erase_on_empty_tree_returns_null(void)
{
    // Arrange.
    Node* root = NULL;

    size_t target = 40;

    // Act.
    root = erase(root, target);

    // Assert.
    TEST_ASSERT_NULL(root);
}

/**
 * @brief Verifies deletion of a terminal leaf node.
 */
void test_erase_leaf_node_removes_node_and_nullifies_parent_link(void)
{
    // Arrange.
    Node* root = NULL;

    size_t parent_value = 50;
    size_t left_value = 25;

    // Act.
    root = insert(root, parent_value);

    insert(root, left_value);

    root = erase(root, left_value);

    // Assert.
    TEST_ASSERT_NULL(root->left);
    TEST_ASSERT_FALSE(search(root, left_value));

    clean(root);
}

/**
 * @brief Verifies deletion of a node possessing only a left child.
 */
void test_erase_node_with_only_left_child_promotes_left_child(void)
{
    // Arrange.
    Node* root = NULL;

    size_t one_hundred = 100;
    size_t fifty = 50;
    size_t twenty_five = 25;

    // Act.
    root = insert(root, one_hundred);

    insert(root, fifty);
    insert(root, twenty_five);

    root = erase(root, fifty);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(root->left->data, twenty_five);
    TEST_ASSERT_FALSE(search(root, fifty));

    clean(root);
}

/**
 * @brief Verifies deletion of a node possessing only a right child.
 */
void test_erase_node_with_only_right_child_promotes_right_child(void)
{
    // Arrange.
    Node* root = NULL;

    size_t one_hundred = 100;
    size_t one_hundred_fifty = 150;
    size_t two_hundred = 200;

    // Act.
    root = insert(root, one_hundred);

    insert(root, one_hundred_fifty);
    insert(root, two_hundred);

    root = erase(root, one_hundred_fifty);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(root->right->data, two_hundred);
    TEST_ASSERT_FALSE(search(root, one_hundred_fifty));

    clean(root);
}

/**
 * @brief Verifies deletion of a complex node with two children.
 */
void test_erase_node_with_two_children_replaces_with_in_order_successor(void)
{
    // Arrange.
    Node* root = NULL;

    size_t root_value = 50;
    size_t left_value = 30;
    size_t right_value = 70;
    size_t successor_value = 60;

    // Act.
    root = insert(root, root_value);

    insert(root, left_value);
    insert(root, right_value);
    insert(root, successor_value);

    root = erase(root, root_value);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(successor_value, root->data);
    TEST_ASSERT_TRUE(search(root, right_value));
    TEST_ASSERT_TRUE(search(root, left_value));

    clean(root);
}

/**
 * @brief Verifies erase operations ignore missing nodes.
 */
void test_erase_non_existent_payload_leaves_tree_unmodified(void)
{
    // Arrange.
    Node* root = NULL;

    size_t value1 = 80;
    size_t value2 = 40;
    size_t target = 13;

    // Act.
    root = insert(root, value1);

    insert(root, value2);

    root = erase(root, target);

    // Assert.
    TEST_ASSERT_TRUE(search(root, value1));
    TEST_ASSERT_TRUE(search(root, value2));

    clean(root);
}

/**
 * @brief Verifies pre-order traversal execution safety.
 */
void test_pre_order_traversal_handles_null_root_safely(void)
{
    // Arrange.
    const Node* root = NULL;

    // Act.
    pre_order(root);

    // Assert.
    TEST_ASSERT_NULL(root);
}

/**
 * @brief Verifies in-order traversal execution safety.
 */
void test_in_order_traversal_handles_null_root_safely(void)
{
    // Arrange.
    const Node* root = NULL;

    // Act.
    in_order(root);

    // Assert.
    TEST_ASSERT_NULL(root);
}

/**
 * @brief Verifies post-order traversal execution safety.
 */
void test_post_order_traversal_handles_null_root_safely(void)
{
    // Arrange.
    const Node* root = NULL;

    // Act.
    post_order(root);

    // Assert.
    TEST_ASSERT_NULL(root);
}

/**
 * @brief Verifies memory cleaning execution safety on NULL references.
 */
void test_clean_handles_null_root_safely(void)
{
    // Arrange.
    Node* root = NULL;

    // Act.
    clean(root);

    // Assert.
    TEST_ASSERT_NULL(root);
}

/**
 * @brief Verifies comprehensive multi-node deallocation.
 */
void test_clean_deallocates_multi_level_tree_successfully(void)
{
    // Arrange.
    Node* root = NULL;

    // Act.
    root = insert(root, 100);

    insert(root, 50);
    insert(root, 150);
    insert(root, 25);

    clean(root);

    root = NULL;

    // Assert.
    TEST_ASSERT_NULL(root);
}

int main(void)
{
    // Initiate the session
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_insert_payload_into_null_tree_establishes_new_root);
    RUN_TEST(test_insert_smaller_payload_routes_to_left_child);
    RUN_TEST(test_insert_larger_payload_routes_to_right_child);
    RUN_TEST(test_insert_duplicate_payload_appends_to_right_subtree);
    RUN_TEST(test_search_on_empty_tree_returns_false);
    RUN_TEST(test_search_for_existing_root_payload_returns_true);
    RUN_TEST(test_search_for_deep_left_leaf_payload_returns_true);
    RUN_TEST(test_search_for_deep_right_leaf_payload_returns_true);
    RUN_TEST(test_search_for_non_existent_payload_returns_false);
    RUN_TEST(test_erase_on_empty_tree_returns_null);
    RUN_TEST(test_erase_leaf_node_removes_node_and_nullifies_parent_link);
    RUN_TEST(test_erase_node_with_only_left_child_promotes_left_child);
    RUN_TEST(test_erase_node_with_only_right_child_promotes_right_child);
    RUN_TEST(test_erase_node_with_two_children_replaces_with_in_order_successor);
    RUN_TEST(test_erase_non_existent_payload_leaves_tree_unmodified);
    RUN_TEST(test_pre_order_traversal_handles_null_root_safely);
    RUN_TEST(test_in_order_traversal_handles_null_root_safely);
    RUN_TEST(test_post_order_traversal_handles_null_root_safely);
    RUN_TEST(test_clean_handles_null_root_safely);
    RUN_TEST(test_clean_deallocates_multi_level_tree_successfully);

    // Destroy the session.
    return UNITY_END();
}