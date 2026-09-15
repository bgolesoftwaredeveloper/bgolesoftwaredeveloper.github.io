/**
 * @file        :   avl_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Comprehensive unit test suite for the AVL Tree.
 * @details     :   Uses the Unity Test Framework to validate AVL properties,
 * including self-balancing (LL, RR, LR, RL rotations),
 * BST ordering, node updates, and memory cleanup.
 * Compile:
 * clang ./tests/darwin/tree/avl_unit_tests.c ./src/darwin/tree/avl/avl.c ./src/unity.c -Iinclude -o ./bin/avl_unit_test
 * @version     :   1.0
 * @date        :   2026-04-19
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Include header files.
#include "unity.h"
#include "darwin/tree/avl.h"

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
 * @brief Helper function to verify the AVL height property.
 */
static int get_height(Node* node)
{
    return (node == NULL) ? 0 : node->height;
}

/**
 * @brief Tests basic initialization of a node.
 */
void test_avl_initialization(void)
{
    // Arrange.
    int key = 50;

    // Act.
    Node* root = initialize(key);

    // Assert.
    TEST_ASSERT_NOT_NULL(root);

    TEST_ASSERT_EQUAL_INT(50, root->key);
    TEST_ASSERT_EQUAL_INT(1, root->height);
    
    TEST_ASSERT_NULL(root->left_child);
    TEST_ASSERT_NULL(root->right_child);

    clean_up(root);
}

/**
 * @brief Tests the Left-Left (LL) rotation case.
 */
void test_avl_insert_ll_rotation(void)
{
    // Arrange.
    Node* root = NULL;

    // Act.
    root = insert(root, 30);
    root = insert(root, 20);
    root = insert(root, 10);

    // Assert.
    TEST_ASSERT_EQUAL_INT(20, root->key);
    TEST_ASSERT_EQUAL_INT(10, root->left_child->key);
    TEST_ASSERT_EQUAL_INT(30, root->right_child->key);
    TEST_ASSERT_EQUAL_INT(2, root->height);

    clean_up(root);
}

/**
 * @brief Tests the Right-Right (RR) rotation case.
 */
void test_avl_insert_rr_rotation(void)
{
    // Arrange.
    Node* root = NULL;

    // Act.
    root = insert(root, 10);
    root = insert(root, 20);
    root = insert(root, 30);

    // Assert.
    TEST_ASSERT_EQUAL_INT(20, root->key);
    TEST_ASSERT_EQUAL_INT(10, root->left_child->key);
    TEST_ASSERT_EQUAL_INT(30, root->right_child->key);

    clean_up(root);
}

/**
 * @brief Tests the Left-Right (LR) rotation case.
 */
void test_avl_insert_lr_rotation(void)
{
    // Arrange.
    Node* root = NULL;

    // Act.
    root = insert(root, 30);
    root = insert(root, 10);
    root = insert(root, 20);

    // Assert.
    TEST_ASSERT_EQUAL_INT(20, root->key);
    TEST_ASSERT_EQUAL_INT(10, root->left_child->key);
    TEST_ASSERT_EQUAL_INT(30, root->right_child->key);

    clean_up(root);
}

/**
 * @brief Tests the Right-Left (RL) rotation case.
 */
void test_avl_insert_rl_rotation(void)
{
    // Arrange.
    Node* root = NULL;

    // Act.
    root = insert(root, 10);
    root = insert(root, 30);
    root = insert(root, 20);

    // Assert.
    TEST_ASSERT_EQUAL_INT(20, root->key);
    TEST_ASSERT_EQUAL_INT(10, root->left_child->key);
    TEST_ASSERT_EQUAL_INT(30, root->right_child->key);

    clean_up(root);
}

/**
 * @brief Tests insertion of duplicate keys (should remain unchanged).
 */
void test_avl_insert_duplicate(void)
{
    // Arrange.
    Node* root = initialize(10);

    // Act.
    root = insert(root, 10);

    // Assert.
    TEST_ASSERT_EQUAL_INT(1, root->height);
    TEST_ASSERT_NULL(root->left_child);
    TEST_ASSERT_NULL(root->right_child);

    clean_up(root);
}

/**
 * @brief Tests erasing a leaf node.
 */
void test_avl_erase_leaf(void)
{
    // Arrange.
    Node* root = NULL;

    root = insert(root, 20);
    root = insert(root, 10);
    
    // Act.
    root = erase(root, 10);

    // Assert.
    TEST_ASSERT_EQUAL_INT(20, root->key);
    TEST_ASSERT_NULL(root->left_child);
    TEST_ASSERT_EQUAL_INT(1, root->height);

    clean_up(root);
}

/**
 * @brief Tests erasing a node with two children.
 */
void test_avl_erase_two_children(void)
{
    // Arrange.
    Node* root = NULL;

    root = insert(root, 20);
    root = insert(root, 10);
    root = insert(root, 30);

    // Act.
    root = erase(root, 20);

    // Assert.
    TEST_ASSERT_EQUAL_INT(30, root->key);
    TEST_ASSERT_NOT_NULL(root->left_child);
    TEST_ASSERT_EQUAL_INT(10, root->left_child->key);

    clean_up(root);
}

/**
 * @brief Tests updating a key value.
 */
void test_avl_update_node(void)
{
    // Arrange.
    Node* root = NULL;

    root = insert(root, 10);
    root = insert(root, 20);

    // Act.
    bool success = false;

    root = update(root, 10, 30, &success);

    // Assert.
    TEST_ASSERT_EQUAL_INT(20, root->key);
    TEST_ASSERT_EQUAL_INT(30, root->right_child->key);
    TEST_ASSERT_TRUE(success);

    clean_up(root);
}

/**
 * @brief Tests erasing the root until the tree is empty.
 */
void test_avl_erase_to_empty(void)
{
    // Arrange.
    Node* root = initialize(50);

    // Act.
    root = erase(root, 50);

    // Assert.
    TEST_ASSERT_NULL(root);
}

/**
 * @brief Tests re-balancing after a deletion.
 */
void test_avl_balance_after_erase(void)
{
    // Arrange.
    Node* root = NULL;

    int keys[] = {40, 20, 50, 10, 30};

    for(int index = 0; index < 5; index++) 
    {
        root = insert(root, keys[index]);
    }

    // Act.
    // Deleting 50 should trigger a re-balance if necessary.
    root = erase(root, 50);

    // Assert.
    int balance = get_height(root->left_child) - get_height(root->right_child);

    TEST_ASSERT_TRUE(abs(balance) <= 1);

    clean_up(root);
}

/**
 * @brief Tests tree behavior with a larger set of keys.
 */
void test_avl_stress_insertions(void)
{
    // Arrange.
    Node* root = NULL;

    // Act.
    for (int index = 1; index <= 15; index++)
    {
        root = insert(root, index);
    }

    // Assert.
    // For 15 nodes, a perfectly balanced AVL tree has height 4.
    TEST_ASSERT_EQUAL_INT(4, root->height);

    clean_up(root);
}

/**
 * @brief Tests erasing a key that does not exist.
 */
void test_avl_erase_non_existent(void)
{
    // Arrange.
    Node* root = initialize(10);

    // Act.
    Node* result = erase(root, 99);

    // Assert.
    TEST_ASSERT_EQUAL_PTR(root, result);
    TEST_ASSERT_EQUAL_INT(10, root->key);

    clean_up(root);
}

/**
 * @brief Tests height consistency across multiple operations.
 */
void test_avl_height_consistency(void)
{
    // Arrange.
    Node* root = NULL;

    root = insert(root, 50);
    root = insert(root, 25);
    root = insert(root, 75);
    root = insert(root, 10);

    // Act & Assert.
    TEST_ASSERT_EQUAL_INT(3, root->height);
    
    root = erase(root, 10);

    TEST_ASSERT_EQUAL_INT(2, root->height);

    clean_up(root);
}

/**
 * @brief Tests updating a node to a value that already exists.
 */
void test_avl_update_to_existing(void)
{
    // Arrange.
    Node* root = NULL;

    root = insert(root, 10);
    root = insert(root, 20);

    // Act.
    bool success = true;

    root = update(root, 10, 20, &success);

    // Assert.
    TEST_ASSERT_EQUAL_INT(10, root->key);
    TEST_ASSERT_NULL(root->left_child);
    TEST_ASSERT_NOT_NULL(root->right_child);
    TEST_ASSERT_EQUAL_INT(20, root->right_child->key);
    TEST_ASSERT_FALSE(success);

    clean_up(root);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_avl_initialization);
    RUN_TEST(test_avl_insert_ll_rotation);
    RUN_TEST(test_avl_insert_rr_rotation);
    RUN_TEST(test_avl_insert_lr_rotation);
    RUN_TEST(test_avl_insert_rl_rotation);
    RUN_TEST(test_avl_insert_duplicate);
    RUN_TEST(test_avl_erase_leaf);
    RUN_TEST(test_avl_erase_two_children);
    RUN_TEST(test_avl_update_node);
    RUN_TEST(test_avl_erase_to_empty);
    RUN_TEST(test_avl_balance_after_erase);
    RUN_TEST(test_avl_stress_insertions);
    RUN_TEST(test_avl_erase_non_existent);
    RUN_TEST(test_avl_height_consistency);
    RUN_TEST(test_avl_update_to_existing);

    // Destroy the session.
    return UNITY_END();
}