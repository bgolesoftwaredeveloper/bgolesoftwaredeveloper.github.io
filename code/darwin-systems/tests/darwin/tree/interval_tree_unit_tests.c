/**
 * @file        :   interval_tree_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Comprehensive unit test suite for the Interval Tree.
 * @details     :   Uses the Unity Test Framework to validate Interval Tree properties,
 * including basic insertions, absolute boundary searches, node erasures,
 * subtree maximum alignment tracking, and edge case boundaries.
 * Compile:
 * clang ./tests/darwin/tree/interval_tree_unit_tests.c ./src/darwin/tree/interval-tree/interval_tree.c ./src/unity.c -Iinclude -o ./bin/interval_tree_unit_test
 * @version     :   1.0
 * @date        :   2026-05-21
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Include header files.
#include "unity.h"
#include "darwin/tree/interval_tree.h"

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
 * @brief Test 1: Verifies that inserting into an empty tree creates a valid root.
 */
void test_insert_into_empty_tree_creates_root(void)
{
    // Arrange.
    Node* root = NULL;
    
    Interval val = {10, 20};

    // Act.
    root = insert(root, val);

    // Assert.
    TEST_ASSERT_NOT_NULL(root);
    TEST_ASSERT_EQUAL_INT(10, root->interval->low);
    TEST_ASSERT_EQUAL_INT(20, root->interval->high);
    TEST_ASSERT_EQUAL_INT(20, root->maximum);

    destroy(root);
}

/**
 * @brief Test 2: Verifies that a left child is correctly assigned when its low value is smaller.
 */
void test_insert_smaller_low_goes_left(void)
{
    // Arrange.
    Node* root = NULL;

    Interval parent = {20, 30};
    Interval child = {10, 15};

    // Act.
    root = insert(root, parent);
    root = insert(root, child);

    // Assert.
    TEST_ASSERT_NOT_NULL(root->left);
    TEST_ASSERT_EQUAL_INT(10, root->left->interval->low);

    destroy(root);
}

/**
 * @brief Test 3: Verifies that a right child is correctly assigned when its low value is larger.
 */
void test_insert_larger_low_goes_right(void)
{
    // Arrange.
    Node* root = NULL;

    Interval parent = {20, 30};
    Interval child = {25, 35};

    // Act.
    root = insert(root, parent);
    root = insert(root, child);

    // Assert.
    TEST_ASSERT_NOT_NULL(root->right);
    TEST_ASSERT_EQUAL_INT(25, root->right->interval->low);

    destroy(root);
}

/**
 * @brief Test 4: Verifies that the parent maximum updates correctly when a right child has a higher value.
 */
void test_insert_updates_parent_maximum_from_right(void)
{
    // Arrange.
    Node* root = NULL;

    Interval parent = {10, 20};
    Interval child = {15, 30};

    // Act.
    root = insert(root, parent);
    root = insert(root, child);

    // Assert.
    TEST_ASSERT_EQUAL_INT(30, root->maximum);

    destroy(root);
}

/**
 * @brief Test 5: Verifies that the parent maximum updates correctly when a left child has a higher value.
 */
void test_insert_updates_parent_maximum_from_left(void)
{
    // Arrange.
    Node* root = NULL;

    Interval parent = {20, 25};
    Interval child = {5, 40};

    // Act.
    root = insert(root, parent);
    root = insert(root, child);

    // Assert.
    TEST_ASSERT_EQUAL_INT(40, root->maximum);

    destroy(root);
}

/**
 * @brief Test 6: Verifies that looking up an exact matching interval returns the valid matching node.
 */
void test_search_exact_match_returns_node(void)
{
    // Arrange.
    Node* root = NULL;

    Interval target = {15, 25};

    root = insert(root, target);

    // Act.
    const Node* result = search(root, target);

    // Assert.
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_INT(15, result->interval->low);

    destroy(root);
}

/**
 * @brief Test 7: Verifies that searching for a subset overlapping interval returns the container node.
 */
void test_search_partial_overlap_returns_match(void)
{
    // Arrange.
    Node* root = NULL;

    Interval stored = {10, 50};
    Interval query = {20, 30};

    root = insert(root, stored);

    // Act.
    const Node* result = search(root, query);

    // Assert.
    TEST_ASSERT_NOT_NULL(result);

    destroy(root);
}

/**
 * @brief Test 8: Verifies that overlapping only on the single boundary edge condition succeeds.
 */
void test_search_boundary_touch_overlaps(void)
{
    // Arrange.
    Node* root = NULL;

    Interval stored = {10, 20};
    Interval query = {20, 30};

    root = insert(root, stored);

    // Act.
    const Node* result = search(root, query);

    // Assert.
    TEST_ASSERT_NOT_NULL(result);

    destroy(root);
}

/**
 * @brief Test 9: Verifies that searching an empty tree structure safely returns NULL.
 */
void test_search_empty_tree_returns_null(void)
{
    // Arrange.
    Node* root = NULL;

    Interval query = {5, 10};

    // Act.
    const Node* result = search(root, query);

    // Assert.
    TEST_ASSERT_NULL(result);
}

/**
 * @brief Test 10: Verifies that an out-of-bounds interval return value resolves to NULL.
 */
void test_search_no_overlap_returns_null(void)
{
    // Arrange.
    Node* root = NULL;

    Interval stored = {5, 10};
    Interval query = {20, 30};

    root = insert(root, stored);

    // Act.
    const Node* result = search(root, query);

    // Assert.
    TEST_ASSERT_NULL(result);

    destroy(root);
}

/**
 * @brief Test 11: Verifies that erasing the absolute last root node results in a clean NULL tree.
 */
void test_erase_root_only_node_leaves_null(void)
{
    // Arrange.
    Node* root = NULL;

    Interval target = {10, 20};

    root = insert(root, target);

    // Act.
    root = erase(root, target);

    // Assert.
    TEST_ASSERT_NULL(root);
}

/**
 * @brief Test 12: Verifies that erasing a matching node containing only a single right leaf re-links cleanly.
 */
void test_erase_node_with_only_right_child(void)
{
    // Arrange.
    Node* root = NULL;

    Interval parent = {10, 20};
    Interval right_child = {20, 30};

    root = insert(root, parent);
    root = insert(root, right_child);

    // Act.
    root = erase(root, parent);

    // Assert.
    TEST_ASSERT_NOT_NULL(root);
    TEST_ASSERT_EQUAL_INT(20, root->interval->low);

    destroy(root);
}

/**
 * @brief Test 13: Verifies that erasing a matching node containing only a single left leaf re-links cleanly.
 */
void test_erase_node_with_only_left_child(void)
{
    // Arrange.
    Node* root = NULL;

    Interval parent = {30, 40};
    Interval left_child = {15, 25};

    root = insert(root, parent);
    root = insert(root, left_child);

    // Act.
    root = erase(root, parent);

    // Assert.
    TEST_ASSERT_NOT_NULL(root);
    TEST_ASSERT_EQUAL_INT(15, root->interval->low);

    destroy(root);
}

/**
 * @brief Test 14: Verifies that deleting a dual-child node successfully swaps via an inorder successor.
 */
void test_erase_node_with_two_children(void)
{
    // Arrange.
    Node* root = NULL;

    Interval parent = {50, 60};
    Interval left_child = {30, 40};
    Interval right_child = {70, 80};

    root = insert(root, parent);
    root = insert(root, left_child);
    root = insert(root, right_child);

    // Act.
    root = erase(root, parent);

    // Assert.
    TEST_ASSERT_EQUAL_INT(70, root->interval->low);

    destroy(root);
}

/**
 * @brief Test 15: Verifies that erasing an interval with shared identical low keys targets the correct high range duplicate.
 */
void test_erase_matching_low_different_high(void)
{
    // Arrange.
    Node* root = NULL;

    Interval first = {10, 20};
    Interval second = {10, 40};

    root = insert(root, first);
    root = insert(root, second);

    // Act.
    root = erase(root, second);

    // Assert.
    TEST_ASSERT_EQUAL_INT(20, root->maximum);

    destroy(root);
}

/**
 * @brief Test 16: Verifies that erasing a non-existent item safely triggers no memory/structural alterations.
 */
void test_erase_non_existent_node_does_nothing(void)
{
    // Arrange.
    Node* root = NULL;

    Interval actual = {10, 20};
    Interval phantom = {99, 100};

    root = insert(root, actual);

    // Act.
    root = erase(root, phantom);

    // Assert.
    TEST_ASSERT_NOT_NULL(root);
    TEST_ASSERT_EQUAL_INT(10, root->interval->low);

    destroy(root);
}

/**
 * @brief Test 17: Verifies that the maximum value re-adjusts downwards if the highest interval branch gets erased.
 */
void test_erase_recalculates_maximum_downward(void)
{
    // Arrange.
    Node* root = NULL;

    Interval target = {20, 100};
    Interval support = {10, 30};

    root = insert(root, support);
    root = insert(root, target);

    // Act.
    root = erase(root, target);

    // Assert.
    TEST_ASSERT_EQUAL_INT(30, root->maximum);

    destroy(root);
}

/**
 * @brief Test 18: Verifies that searching deep down a pruned branch layout correctly resolves to the matching right side path.
 */
void test_search_pruning_moves_to_right_subtree(void)
{
    // Arrange.
    Node* root = NULL;

    Interval parent = {20, 25};
    Interval left_child = {10, 12};
    Interval right_child = {40, 50};
    Interval query = {42, 45};

    root = insert(root, parent);
    root = insert(root, left_child);
    root = insert(root, right_child);

    // Act.
    const Node* result = search(root, query);

    // Assert.
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_INT(40, result->interval->low);

    destroy(root);
}

/**
 * @brief Test 19: Verifies nested structural tracking properties over multiple tiered elements.
 */
void test_multiple_insertions_maintain_max_properties(void)
{
    // Arrange.
    Node* root = NULL;
    Interval data_idx = {30, 35};
    Interval data_cox = {20, 22};
    Interval data_aux = {40, 41};
    Interval data_ext = {15, 60};

    // Act.
    root = insert(root, data_idx);
    root = insert(root, data_cox);
    root = insert(root, data_aux);
    root = insert(root, data_ext);

    // Assert.
    TEST_ASSERT_EQUAL_INT(60, root->maximum);

    destroy(root);
}

/**
 * @brief Test 20: Verifies passing a NULL tree directly into the destroy routine exits safely without failures.
 */
void test_destroy_null_tree_exits_safely(void)
{
    // Arrange.
    Node* root = NULL;

    // Act.
    destroy(root);

    // Assert.
    TEST_ASSERT_NULL(root);
}

/**
 * @brief Main execution frame for executing the suite of 20 unit tests.
 */
int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();
    
    // Register the tests to run.
    RUN_TEST(test_insert_into_empty_tree_creates_root);
    RUN_TEST(test_insert_smaller_low_goes_left);
    RUN_TEST(test_insert_larger_low_goes_right);
    RUN_TEST(test_insert_updates_parent_maximum_from_right);
    RUN_TEST(test_insert_updates_parent_maximum_from_left);
    RUN_TEST(test_search_exact_match_returns_node);
    RUN_TEST(test_search_partial_overlap_returns_match);
    RUN_TEST(test_search_boundary_touch_overlaps);
    RUN_TEST(test_search_empty_tree_returns_null);
    RUN_TEST(test_search_no_overlap_returns_null);
    RUN_TEST(test_erase_root_only_node_leaves_null);
    RUN_TEST(test_erase_node_with_only_right_child);
    RUN_TEST(test_erase_node_with_only_left_child);
    RUN_TEST(test_erase_node_with_two_children);
    RUN_TEST(test_erase_matching_low_different_high);
    RUN_TEST(test_erase_non_existent_node_does_nothing);
    RUN_TEST(test_erase_recalculates_maximum_downward);
    RUN_TEST(test_search_pruning_moves_to_right_subtree);
    RUN_TEST(test_multiple_insertions_maintain_max_properties);
    RUN_TEST(test_destroy_null_tree_exits_safely);

    // Destroy the session.
    return UNITY_END();
}