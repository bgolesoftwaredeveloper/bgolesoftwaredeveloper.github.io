/**
 * @file        :   bi_directional_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit test suite for the bi-directional binary search tree.
 * @details     :   Provides comprehensive validation coverage for node construction, 
 * tree insertion logic, element lookup, value updates, complex 
 * element deletions, and memory management routines using Unity.dz
 * Compile:
 * clang ./tests/darwin/graph/bi_directional_unit_tests.c ./src/darwin/graph/bi-directional/bi_directional.c ./src/unity.c -Iinclude -o ./bin/bi_directional_unit_test
 * @version     :   1.0
 * @date        :   2026-05-24
 */
#include <stdio.h>
#include <string.h>

#include "unity.h"
#include "darwin/graph/bi_directional.h"

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
 * @brief Verifies that construct initializes a standalone node correctly.
 */
void test_construct_should_initialize_node_with_given_data_and_null_children(void)
{
    // Arrange.
    int data = 42;

    // Act.
    Node* result = construct(data, NULL);

    // Assert.
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_INT(data, result->data);
    TEST_ASSERT_NULL(result->left);
    TEST_ASSERT_NULL(result->right);
    TEST_ASSERT_NULL(result->parent);

    clean(result);
}

/**
 * @brief Verifies that construct sets the parent pointer correctly.
 */
void test_construct_should_assign_specified_parent_pointer(void)
{
    // Arrange.
    Node* parent = construct(10, NULL);

    int data = 5;

    // Act.
    Node* child = construct(data, parent);

    // Assert.
    TEST_ASSERT_NOT_NULL(child);
    TEST_ASSERT_EQUAL_PTR(parent, child->parent);

    clean(child);
    clean(parent);
}

/**
 * @brief Verifies that insert creates a root node if the tree is empty.
 */
void test_insert_should_create_root_node_when_tree_is_empty(void)
{
    // Arrange.
    Node* root = NULL;

    int data = 50;

    // Act.
    root = insert(root, data);

    // Assert.
    TEST_ASSERT_NOT_NULL(root);
    TEST_ASSERT_EQUAL_INT(data, root->data);
    TEST_ASSERT_NULL(root->parent);

    clean(root);
}

/**
 * @brief Verifies that smaller values are inserted into the left subtree with parent linked.
 */
void test_insert_should_place_smaller_value_in_left_subtree_and_link_parent(void)
{
    // Arrange.
    Node* root = NULL;

    root = insert(root, 50);

    // Act.
    root = insert(root, 30);

    // Assert.
    TEST_ASSERT_NOT_NULL(root->left);
    TEST_ASSERT_EQUAL_INT(30, root->left->data);
    TEST_ASSERT_EQUAL_PTR(root, root->left->parent);

    clean(root);
}

/**
 * @brief Verifies that larger values are inserted into the right subtree with parent linked.
 */
void test_insert_should_place_larger_value_in_right_subtree_and_link_parent(void)
{
    // Arrange.
    Node* root = NULL;

    root = insert(root, 50);

    // Act.
    root = insert(root, 70);

    // Assert.
    TEST_ASSERT_NOT_NULL(root->right);
    TEST_ASSERT_EQUAL_INT(70, root->right->data);
    TEST_ASSERT_EQUAL_PTR(root, root->right->parent);

    clean(root);
}

/**
 * @brief Verifies that inserting a duplicate item does not change the tree structure.
 */
void test_insert_should_ignore_duplicate_values(void)
{
    // Arrange.
    Node* root = NULL;

    root = insert(root, 50);
    root = insert(root, 30);

    // Act.
    root = insert(root, 30);

    // Assert.
    TEST_ASSERT_NULL(root->left->left);
    TEST_ASSERT_NULL(root->left->right);

    clean(root);
}

/**
 * @brief Verifies search handles an empty tree configuration cleanly.
 */
void test_search_should_return_null_when_tree_is_empty(void)
{
    // Arrange.
    Node* root = NULL;

    // Act.
    Node* found_node = search(root, 42);

    // Assert.
    TEST_ASSERT_NULL(found_node);
}

/**
 * @brief Verifies search extracts the matching node when located in the tree.
 */
void test_search_should_find_existing_node_in_tree(void)
{
    // Arrange.
    Node* root = NULL;

    root = insert(root, 50);
    root = insert(root, 30);
    root = insert(root, 70);

    // Act.
    Node* found_node = search(root, 30);

    // Assert.
    TEST_ASSERT_NOT_NULL(found_node);
    TEST_ASSERT_EQUAL_INT(30, found_node->data);
    TEST_ASSERT_EQUAL_PTR(root, found_node->parent);

    clean(root);
}

/**
 * @brief Verifies search returns null if a requested key does not exist.
 */
void test_search_should_return_null_when_key_does_not_exist(void)
{
    // Arrange.
    Node* root = NULL;

    root = insert(root, 50);

    // Act.
    Node* found_node = search(root, 99);

    // Assert.
    TEST_ASSERT_NULL(found_node);

    clean(root);
}

/**
 * @brief Verifies update changes target value structure safely.
 */
void test_update_should_modify_value_by_reinserting_node(void)
{
    // Arrange.
    Node* root = NULL;

    root = insert(root, 50);
    root = insert(root, 30);

    // Act.
    root = update(root, 30, 35);

    // Assert.
    TEST_ASSERT_NULL(search(root, 30));

    Node* updated_node = search(root, 35);

    TEST_ASSERT_NOT_NULL(updated_node);
    TEST_ASSERT_EQUAL_PTR(root, updated_node->parent);

    clean(root);
}

/**
 * @brief Verifies update returns the original layout unchanged if old value is missing.
 */
void test_update_should_do_nothing_if_target_value_is_missing(void)
{
    // Arrange.
    Node* root = NULL;

    root = insert(root, 50);

    // Act.
    root = update(root, 11, 22);

    // Assert.
    TEST_ASSERT_NOT_NULL(search(root, 50));
    TEST_ASSERT_NULL(search(root, 22));

    clean(root);
}

/**
 * @brief Verifies erase handles empty tree parameters properly.
 */
void test_erase_should_return_null_when_tree_is_empty(void)
{
    // Arrange.
    Node* root = NULL;

    // Act.
    root = erase(root, 50);

    // Assert.
    TEST_ASSERT_NULL(root);
}

/**
 * @brief Verifies erase safely removes a single leaf node element.
 */
void test_erase_should_remove_leaf_node_correctly(void)
{
    // Arrange.
    Node* root = NULL;

    root = insert(root, 50);
    root = insert(root, 30);

    // Act.
    root = erase(root, 30);

    // Assert.
    TEST_ASSERT_NULL(root->left);
    TEST_ASSERT_NULL(search(root, 30));

    clean(root);
}

/**
 * @brief Verifies erase handles single child node deletion and child promotion from right.
 */
void test_erase_should_promote_right_child_when_left_child_is_null(void)
{
    // Arrange.
    Node* root = NULL;

    root = insert(root, 50);
    root = insert(root, 30);
    root = insert(root, 35);

    // Act.
    root = erase(root, 30);

    // Assert.
    TEST_ASSERT_EQUAL_INT(35, root->left->data);
    TEST_ASSERT_EQUAL_PTR(root, root->left->parent);

    clean(root);
}

/**
 * @brief Verifies erase handles single child node deletion and child promotion from left.
 */
void test_erase_should_promote_left_child_when_right_child_is_null(void)
{
    // Arrange.
    Node* root = NULL;

    root = insert(root, 50);
    root = insert(root, 30);
    root = insert(root, 20);

    // Act.
    root = erase(root, 30);

    // Assert.
    TEST_ASSERT_EQUAL_INT(20, root->left->data);
    TEST_ASSERT_EQUAL_PTR(root, root->left->parent);

    clean(root);
}

/**
 * @brief Verifies erase resolves two-child node deletions via successor replacement.
 */
void test_erase_should_use_in_order_successor_to_replace_deleted_node_with_two_children(void)
{
    // Arrange.
    Node* root = NULL;

    root = insert(root, 50);
    root = insert(root, 30);
    root = insert(root, 20);
    root = insert(root, 40);

    // Act.
    root = erase(root, 30);

    // Assert.
    TEST_ASSERT_EQUAL_INT(40, root->left->data);
    TEST_ASSERT_EQUAL_INT(20, root->left->left->data);
    TEST_ASSERT_EQUAL_PTR(root->left, root->left->left->parent);

    clean(root);
}

/**
 * @brief Verifies erase changes the main root container node when it has only one child.
 */
void test_erase_should_update_root_pointer_when_root_is_deleted(void)
{
    // Arrange.
    Node* root = NULL;

    root = insert(root, 50);
    root = insert(root, 60);

    // Act.
    root = erase(root, 50);

    // Assert.
    TEST_ASSERT_NOT_NULL(root);
    TEST_ASSERT_EQUAL_INT(60, root->data);
    TEST_ASSERT_NULL(root->parent);

    clean(root);
}

/**
 * @brief Verifies nested parent values update properly when processing multiple elements.
 */
void test_tree_traversal_should_maintain_correct_parent_linkages_deeply(void)
{
    // Arrange.
    Node* root = NULL;

    // Act.
    root = insert(root, 50);
    root = insert(root, 30);
    root = insert(root, 20);
    root = insert(root, 10);

    // Assert.
    Node* deepest_left_node = root->left->left->left;

    TEST_ASSERT_NOT_NULL(deepest_left_node);
    TEST_ASSERT_EQUAL_INT(10, deepest_left_node->data);
    TEST_ASSERT_EQUAL_PTR(root->left->left, deepest_left_node->parent);

    clean(root);
}

/**
 * @brief Verifies erase does nothing if looking for a non-existent element in a filled tree.
 */
void test_erase_should_leave_tree_intact_if_key_is_not_found(void)
{
    // Arrange.
    Node* root = NULL;

    root = insert(root, 50);
    root = insert(root, 25);

    // Act.
    root = erase(root, 99);

    // Assert.
    TEST_ASSERT_NOT_NULL(search(root, 50));
    TEST_ASSERT_NOT_NULL(search(root, 25));

    clean(root);
}

/**
 * @brief Verifies update handles modifying the absolute root item of the tree safely.
 */
void test_update_should_successfully_change_root_node_value(void)
{
    // Arrange.
    Node* root = NULL;

    root = insert(root, 50);
    root = insert(root, 70);

    // Act.
    root = update(root, 50, 80);

    // Assert.
    TEST_ASSERT_EQUAL_INT(70, root->data);
    TEST_ASSERT_NOT_NULL(root->right);
    TEST_ASSERT_EQUAL_INT(80, root->right->data);
    TEST_ASSERT_EQUAL_PTR(root, root->right->parent);

    clean(root);
}

/**
 * @brief Entry point for running the test harness suite.
 */
int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_construct_should_initialize_node_with_given_data_and_null_children);
    RUN_TEST(test_construct_should_assign_specified_parent_pointer);
    RUN_TEST(test_insert_should_create_root_node_when_tree_is_empty);
    RUN_TEST(test_insert_should_place_smaller_value_in_left_subtree_and_link_parent);
    RUN_TEST(test_insert_should_place_larger_value_in_right_subtree_and_link_parent);
    RUN_TEST(test_insert_should_ignore_duplicate_values);
    RUN_TEST(test_search_should_return_null_when_tree_is_empty);
    RUN_TEST(test_search_should_find_existing_node_in_tree);
    RUN_TEST(test_search_should_return_null_when_key_does_not_exist);
    RUN_TEST(test_update_should_modify_value_by_reinserting_node);
    RUN_TEST(test_update_should_do_nothing_if_target_value_is_missing);
    RUN_TEST(test_erase_should_return_null_when_tree_is_empty);
    RUN_TEST(test_erase_should_remove_leaf_node_correctly);
    RUN_TEST(test_erase_should_promote_right_child_when_left_child_is_null);
    RUN_TEST(test_erase_should_promote_left_child_when_right_child_is_null);
    RUN_TEST(test_erase_should_use_in_order_successor_to_replace_deleted_node_with_two_children);
    RUN_TEST(test_erase_should_update_root_pointer_when_root_is_deleted);
    RUN_TEST(test_tree_traversal_should_maintain_correct_parent_linkages_deeply);
    RUN_TEST(test_erase_should_leave_tree_intact_if_key_is_not_found);
    RUN_TEST(test_update_should_successfully_change_root_node_value);

    // Destroy the session.
    return UNITY_END();
}