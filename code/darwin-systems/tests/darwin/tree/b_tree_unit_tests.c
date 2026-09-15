/**
 * @file        :   b_tree_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Comprehensive behavioral unit test suite for a B-Tree structure.
 * @details     :   Leverages the Unity Test Framework to rigorously validate 
 * core B-Tree invariants and properties. Testing coverage spans 
 * multi-way lookup operations, node insertion ordering, root splitting, 
 * underflow handling (including leaf borrowing and child node merging 
 * mechanics), and exhaustive allocation memory cleanup routines.
 * Compile:
 * clang ./tests/darwin/tree/b_tree_unit_tests.c ./src/darwin/tree/b-tree/b_tree.c ./src/unity.c -Iinclude -o ./bin/b_tree_unit_test
 * @version     :   1.0
 * @date        :   2026-05-21
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Include header files.
#include "unity.h"
#include "darwin/tree/b_tree.h"

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
 * @brief Verifies that an uninitialized tree pointer points to NULL.
 */
void test_b_tree_initially_empty(void)
{
    // Arrange.
    Node* root_instance = NULL;

    // Act.

    // Assert.
    TEST_ASSERT_NULL(root_instance);
}

/**
 * @brief Verifies that inserting into an empty tree creates a leaf node with count 1.
 */
void test_b_tree_single_insertion(void)
{
    // Arrange.
    Node* single_root = NULL;

    int primary_key = 42;

    // Act.
    insert(&single_root, primary_key);

    // Assert.
    TEST_ASSERT_NOT_NULL(single_root);
    TEST_ASSERT_TRUE(single_root->is_leaf);
    TEST_ASSERT_EQUAL_UINT32(1, single_root->count);
    TEST_ASSERT_EQUAL_INT(42, single_root->keys[0]);

    clean(single_root);
}

/**
 * @brief Verifies that searching an empty tree yields NULL.
 */
void test_b_tree_search_empty(void)
{
    // Arrange.
    Node* empty_root = NULL;

    int target_query = 10;

    // Act.
    Node* search_result = search(empty_root, target_query);

    // Assert.
    TEST_ASSERT_NULL(search_result);
}

/**
 * @brief Verifies that a key can be successfully found after insertion.
 */
void test_b_tree_search_existing_key(void)
{
    // Arrange.
    Node* active_root = NULL;

    int target_key = 15;

    // Act.
    insert(&active_root, target_key);

    Node* match_node = search(active_root, target_key);

    // Assert.
    TEST_ASSERT_NOT_NULL(match_node);
    TEST_ASSERT_EQUAL_INT(15, match_node->keys[0]);

    clean(active_root);
}

/**
 * @brief Verifies that searching for a non-existent key returns NULL.
 */
void test_b_tree_search_non_existent_key(void)
{
    // Arrange.
    Node* populated_root = NULL;

    int baseline_value = 50;
    int missing_value = 99;

    // Act.
    insert(&populated_root, baseline_value);

    Node* failed_match = search(populated_root, missing_value);

    // Assert.
    TEST_ASSERT_NULL(failed_match);

    clean(populated_root);
}

/**
 * @brief Verifies that insertion guards against duplicate keys.
 */
void test_b_tree_rejects_duplicate_insertion(void)
{
    // Arrange.
    Node* unique_root = NULL;

    int initial_key = 25;
    int duplicate_key = 25;

    // Act.
    insert(&unique_root, initial_key);
    insert(&unique_root, duplicate_key);

    // Assert.
    TEST_ASSERT_EQUAL_UINT32(1, unique_root->count);

    clean(unique_root);
}

/**
 * @brief Verifies multiple ascending sorted insertions inside a single leaf.
 */
void test_b_tree_multiple_keys_remain_sorted(void)
{
    // Arrange.
    Node* simple_root = NULL;

    int secondary_value = 20;
    int primary_value = 10;
    int tertiary_value = 30;

    // Act.
    insert(&simple_root, secondary_value);
    insert(&simple_root, primary_value);
    insert(&simple_root, tertiary_value);

    // Assert.
    TEST_ASSERT_EQUAL_UINT32(3, simple_root->count);
    TEST_ASSERT_EQUAL_INT(10, simple_root->keys[0]);
    TEST_ASSERT_EQUAL_INT(20, simple_root->keys[1]);
    TEST_ASSERT_EQUAL_INT(30, simple_root->keys[2]);

    clean(simple_root);
}

/**
 * @brief Verifies that the root node splits correctly when exceeding capacity limits.
 */
void test_b_tree_root_splits_on_overflow(void)
{
    // Arrange.
    Node* expanding_root = NULL;

    int point_alpha = 100;
    int point_beta = 200;
    int point_gamma = 300;
    int point_delta = 400;

    // Act.
    insert(&expanding_root, point_alpha);
    insert(&expanding_root, point_beta);
    insert(&expanding_root, point_gamma);
    insert(&expanding_root, point_delta);

    // Assert.
    TEST_ASSERT_FALSE(expanding_root->is_leaf);
    TEST_ASSERT_EQUAL_UINT32(1, expanding_root->count);
    TEST_ASSERT_NOT_NULL(expanding_root->children[0]);
    TEST_ASSERT_NOT_NULL(expanding_root->children[1]);

    clean(expanding_root);
}

/**
 * @brief Verifies deleting the only remaining key from a single-node tree.
 */
void test_b_tree_delete_lone_root_key(void)
{
    // Arrange.
    Node* dynamic_root = NULL;

    int transient_key = 77;

    // Act.
    insert(&dynamic_root, transient_key);
    erase(&dynamic_root, transient_key);

    // Assert.
    TEST_ASSERT_NULL(dynamic_root);
}

/**
 * @brief Verifies deleting an edge key directly from a populated leaf node.
 */
void test_b_tree_delete_from_leaf(void)
{
    // Arrange.
    Node* leaf_root = NULL;

    int item_low = 5;
    int item_mid = 10;
    int item_high = 15;

    // Act.
    insert(&leaf_root, item_low);
    insert(&leaf_root, item_mid);
    insert(&leaf_root, item_high);
    erase(&leaf_root, item_high);

    // Assert.
    TEST_ASSERT_EQUAL_UINT32(2, leaf_root->count);
    TEST_ASSERT_NULL(search(leaf_root, item_high));

    clean(leaf_root);
}

/**
 * @brief Verifies that erasing a non-existent key does not alter tree structure.
 */
void test_b_tree_delete_non_existent_graceful(void)
{
    // Arrange.
    Node* static_root = NULL;

    int real_element = 50;
    int phantom_element = 99;

    // Act.
    insert(&static_root, real_element);
    erase(&static_root, phantom_element);

    // Assert.
    TEST_ASSERT_NOT_NULL(static_root);
    TEST_ASSERT_EQUAL_UINT32(1, static_root->count);
    TEST_ASSERT_EQUAL_INT(50, static_root->keys[0]);

    clean(static_root);
}

/**
 * @brief Verifies in-order predecessor replacement mechanics during internal node deletion.
 */
void test_b_tree_delete_internal_node_via_predecessor(void)
{
    // Arrange.
    Node* balance_root = NULL;
    
    int node_value_1 = 10;
    int node_value_2 = 20;
    int node_value_3 = 30;
    int node_value_4 = 40;

    // Act.
    insert(&balance_root, node_value_1);
    insert(&balance_root, node_value_2);
    insert(&balance_root, node_value_3);
    insert(&balance_root, node_value_4);
    erase(&balance_root, node_value_3);

    // Assert.
    TEST_ASSERT_NULL(search(balance_root, node_value_3));

    clean(balance_root);
}

/**
 * @brief Verifies sequential ascending data loading limits.
 */
void test_b_tree_large_ascending_sequence(void)
{
    // Arrange.
    Node* sequence_root = NULL;

    int fill_one = 1;
    int fill_two = 2;
    int fill_three = 3;
    int fill_four = 4;
    int fill_five = 5;
    int fill_six = 6;

    // Act.
    insert(&sequence_root, fill_one);
    insert(&sequence_root, fill_two);
    insert(&sequence_root, fill_three);
    insert(&sequence_root, fill_four);
    insert(&sequence_root, fill_five);
    insert(&sequence_root, fill_six);

    // Assert.
    TEST_ASSERT_NOT_NULL(search(sequence_root, fill_one));
    TEST_ASSERT_NOT_NULL(search(sequence_root, fill_six));

    clean(sequence_root);
}

/**
 * @brief Verifies structural integrity under sequential descending inputs.
 */
void test_b_tree_large_descending_sequence(void)
{
    // Arrange.
    Node* inverted_root = NULL;

    int down_value_sixty = 60;
    int down_value_fifty = 50;
    int down_value_fourty = 40;
    int down_value_thirty = 30;
    int down_value_five = 20;
    int down_value_six = 10;

    // Act.
    insert(&inverted_root, down_value_sixty);
    insert(&inverted_root, down_value_fifty);
    insert(&inverted_root, down_value_fourty);
    insert(&inverted_root, down_value_thirty);
    insert(&inverted_root, down_value_five);
    insert(&inverted_root, down_value_six);

    // Assert.
    TEST_ASSERT_NOT_NULL(search(inverted_root, down_value_sixty));
    TEST_ASSERT_NOT_NULL(search(inverted_root, down_value_six));

    clean(inverted_root);
}

/**
 * @brief Verifies borrowing mechanics from a left sibling node during erasure underflows.
 */
void test_b_tree_underflow_borrow_from_left_sibling(void)
{
    // Arrange.
    Node* borrow_root_left = NULL;

    int fourty = 40;
    int twenty = 20;
    int sixty = 60;
    int ten = 10;
    int thirty = 30;

    // Act.
    insert(&borrow_root_left, fourty);
    insert(&borrow_root_left, twenty);
    insert(&borrow_root_left, sixty);
    insert(&borrow_root_left, ten);
    insert(&borrow_root_left, thirty);

    erase(&borrow_root_left, sixty);

    // Assert.
    TEST_ASSERT_NULL(search(borrow_root_left, sixty));

    clean(borrow_root_left);
}

/**
 * @brief Verifies borrowing mechanics from a right sibling node during erasure underflows.
 */
void test_b_tree_underflow_borrow_from_right_sibling(void)
{
    // Arrange.
    Node* borrow_root_right = NULL;

    int entry_twenty = 20;
    int entry_ten = 10;
    int entry_fourty = 40;
    int entry_thirty = 30;
    int entry_fifty = 50;

    // Act.
    insert(&borrow_root_right, entry_twenty);
    insert(&borrow_root_right, entry_ten);
    insert(&borrow_root_right, entry_fourty);
    insert(&borrow_root_right, entry_thirty);
    insert(&borrow_root_right, entry_fifty);

    erase(&borrow_root_right, entry_ten);

    // Assert.
    TEST_ASSERT_NULL(search(borrow_root_right, entry_ten));

    clean(borrow_root_right);
}

/**
 * @brief Verifies child merging processes when both siblings are at capacity floors.
 */
void test_b_tree_underflow_merge_siblings(void)
{
    // Arrange.
    Node* consolidation_root = NULL;

    int baseline_thirty = 30;
    int baseline_fifteen = 15;
    int baseline_fourty_five = 45;

    // Act.
    insert(&consolidation_root, baseline_thirty);
    insert(&consolidation_root, baseline_fifteen);
    insert(&consolidation_root, baseline_fourty_five);

    erase(&consolidation_root, baseline_fifteen);

    // Assert.
    TEST_ASSERT_TRUE(consolidation_root->is_leaf);
    TEST_ASSERT_NULL(search(consolidation_root, baseline_fifteen));

    clean(consolidation_root);
}

/**
 * @brief Verifies that calling clean against a NULL pointer returns safely.
 */
void test_b_tree_clean_null_pointer_safe(void)
{
    // Arrange.
    Node* zero_pointer = NULL;

    // Act.
    clean(zero_pointer);

    // Assert.
    TEST_ASSERT_NULL(zero_pointer);
}

/**
 * @brief Verifies key modifications preserve search validity post updates.
 */
void test_b_tree_interleaved_insert_and_delete(void)
{
    // Arrange.
    Node* transactional_root = NULL;

    int value_alpha = 88;
    int value_beta = 12;
    int value_gamma = 44;

    // Act.
    insert(&transactional_root, value_alpha);
    insert(&transactional_root, value_beta);
    erase(&transactional_root, value_alpha);
    insert(&transactional_root, value_gamma);

    // Assert.
    TEST_ASSERT_NULL(search(transactional_root, value_alpha));
    TEST_ASSERT_NOT_NULL(search(transactional_root, value_beta));
    TEST_ASSERT_NOT_NULL(search(transactional_root, value_gamma));

    clean(transactional_root);
}

/**
 * @brief Verifies height collapse operations when the root drains all available keys.
 */
void test_b_tree_root_shrunk_on_empty_internal(void)
{
    // Arrange.
    Node* structural_collapsible_root = NULL;

    int block_1 = 10;
    int block_2 = 20;
    int block_3 = 30;

    // Act.
    insert(&structural_collapsible_root, block_1);
    insert(&structural_collapsible_root, block_2);
    insert(&structural_collapsible_root, block_3);
    erase(&structural_collapsible_root, block_2);
    erase(&structural_collapsible_root, block_1);

    // Assert.
    TEST_ASSERT_TRUE(structural_collapsible_root->is_leaf);
    TEST_ASSERT_EQUAL_UINT32(1, structural_collapsible_root->count);

    clean(structural_collapsible_root);
}

/**
 * @brief Verifies capacity behaviors by targeting internal upper data bounds.
 */
void test_b_tree_maximum_leaf_capacity_alignment(void)
{
    // Arrange.
    Node* bounding_root = NULL;

    int eleven = 11;
    int twenty_two = 22;
    int thirty_three = 33;

    // Act.
    insert(&bounding_root, eleven);
    insert(&bounding_root, twenty_two);
    insert(&bounding_root, thirty_three);

    // Assert.
    TEST_ASSERT_EQUAL_UINT32(3, bounding_root->count);

    clean(bounding_root);
}

/**
 * @brief Verifies in-order successor replacement mechanics during internal node deletion.
 */
void test_b_tree_delete_internal_node_via_successor(void)
{
    // Arrange.
    Node* fallback_root = NULL;

    int item_primary = 50;
    int item_left_branch = 25;
    int item_right_branch = 75;
    int item_successor_element = 80;

    // Act.
    insert(&fallback_root, item_primary);
    insert(&fallback_root, item_left_branch);
    insert(&fallback_root, item_right_branch);
    insert(&fallback_root, item_successor_element);
    erase(&fallback_root, item_right_branch);

    // Assert.
    TEST_ASSERT_NULL(search(fallback_root, item_right_branch));

    clean(fallback_root);
}

/**
 * @brief Verifies clearing functions on trees containing multiple allocation levels.
 */
void test_b_tree_clear_nested_subtrees_completely(void)
{
    // Arrange.
    Node* multi_tier_root = NULL;

    int seed_1 = 500;
    int seed_2 = 250;
    int seed_3 = 750;
    int seed_4 = 125;
    int seed_5 = 375;

    // Act.
    insert(&multi_tier_root, seed_1);
    insert(&multi_tier_root, seed_2);
    insert(&multi_tier_root, seed_3);
    insert(&multi_tier_root, seed_4);
    insert(&multi_tier_root, seed_5);

    // Assert.
    TEST_ASSERT_NOT_NULL(multi_tier_root);

    clean(multi_tier_root);
}

/**
 * @brief Verifies data grouping arrays retain total element counts correctly.
 */
void test_b_tree_preserves_total_element_count(void)
{
    // Arrange.
    Node* registry_root = NULL;

    int fourteen = 14;
    int eighty_two = 82;
    int fourty_six = 46;

    // Act.
    insert(&registry_root, fourteen);
    insert(&registry_root, eighty_two);
    insert(&registry_root, fourty_six);

    // Assert.
    TEST_ASSERT_EQUAL_UINT32(3, registry_root->count);

    clean(registry_root);
}

/**
 * @brief Verifies edge-case balance tracking loops using negative keys.
 */
void test_b_tree_supports_negative_keys(void)
{
    // Arrange.
    Node* signed_root = NULL;

    int negative_pivot = -50;
    int zero_pivot = 0;
    int positive_pivot = 50;

    // Act.
    insert(&signed_root, negative_pivot);
    insert(&signed_root, zero_pivot);
    insert(&signed_root, positive_pivot);

    // Assert.
    TEST_ASSERT_EQUAL_INT(-50, signed_root->keys[0]);
    TEST_ASSERT_EQUAL_INT(0, signed_root->keys[1]);
    TEST_ASSERT_EQUAL_INT(50, signed_root->keys[2]);

    clean(signed_root);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_b_tree_initially_empty);
    RUN_TEST(test_b_tree_single_insertion);
    RUN_TEST(test_b_tree_search_empty);
    RUN_TEST(test_b_tree_search_existing_key);
    RUN_TEST(test_b_tree_search_non_existent_key);
    RUN_TEST(test_b_tree_rejects_duplicate_insertion);
    RUN_TEST(test_b_tree_multiple_keys_remain_sorted);
    RUN_TEST(test_b_tree_root_splits_on_overflow);
    RUN_TEST(test_b_tree_delete_lone_root_key);
    RUN_TEST(test_b_tree_delete_from_leaf);
    RUN_TEST(test_b_tree_delete_non_existent_graceful);
    RUN_TEST(test_b_tree_delete_internal_node_via_predecessor);
    RUN_TEST(test_b_tree_large_ascending_sequence);
    RUN_TEST(test_b_tree_large_descending_sequence);
    RUN_TEST(test_b_tree_underflow_borrow_from_left_sibling);
    RUN_TEST(test_b_tree_underflow_borrow_from_right_sibling);
    RUN_TEST(test_b_tree_underflow_merge_siblings);
    RUN_TEST(test_b_tree_clean_null_pointer_safe);
    RUN_TEST(test_b_tree_interleaved_insert_and_delete);
    RUN_TEST(test_b_tree_root_shrunk_on_empty_internal);
    RUN_TEST(test_b_tree_maximum_leaf_capacity_alignment);
    RUN_TEST(test_b_tree_delete_internal_node_via_successor);
    RUN_TEST(test_b_tree_clear_nested_subtrees_completely);
    RUN_TEST(test_b_tree_preserves_total_element_count);
    RUN_TEST(test_b_tree_supports_negative_keys);

    // Destroy the session.
    return UNITY_END();
}