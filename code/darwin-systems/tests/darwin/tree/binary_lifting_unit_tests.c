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
 * clang ./tests/darwin/tree/binary_lifting_unit_tests.c ./src/darwin/tree/binary-lifting/binary_lifting.c ./src/unity.c -Iinclude -o ./bin/binary_lifting_unit_test
 * @version     :   1.0
 * @date        :   2026-05-23
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Include header files.
#include "unity.h"
#include "darwin/tree/binary_lifting.h"

/**
 * @brief Sets up resources before each test.
 */
void setUp(void)
{
    initialize(MAXIMUM_NODE_COUNT - 1);
}

/**
 * @brief Cleans up resources after each test.
 */
void tearDown(void)
{
    clean(MAXIMUM_NODE_COUNT - 1);
}

/**
 * @brief Validates initialization behavior over global arrays.
 */
void test_initialize_clears_global_tables(void)
{
    // Arrange.
    node_depth[5] = 42;
    visited[5] = true;
    ancestor_table[5][0] = 2;

    // Act.
    initialize(MAXIMUM_NODE_COUNT - 1);

    // Assert.
    TEST_ASSERT_EQUAL_INT(INVALID_NODE_ID, ancestor_table[5][0]);
    TEST_ASSERT_EQUAL_UINT(0, node_depth[5]);
    TEST_ASSERT_FALSE(visited[5]);
}

/**
 * @brief Verifies out of bounds filtering for adding nodes.
 */
void test_add_edge_out_of_bounds_fails(void)
{
    // Arrange.
    int source_node = -1;
    int destination_node = MAXIMUM_NODE_COUNT;

    // Act.
    bool tracking_status_one = add(source_node, 5);
    bool tracking_status_two = add(5, destination_node);

    // Assert.
    TEST_ASSERT_FALSE(tracking_status_one);
    TEST_ASSERT_FALSE(tracking_status_two);
}

/**
 * @brief Validates memory layout appending for successful edge addition.
 */
void test_add_edge_allocates_successfully(void)
{
    // Arrange.
    int source_node = 1;
    int destination_node = 2;

    // Act.
    bool tracking_status = add(source_node, destination_node);

    // Assert.
    TEST_ASSERT_TRUE(tracking_status);
    TEST_ASSERT_NOT_NULL(adjacency_list[source_node]);
    TEST_ASSERT_EQUAL_INT(destination_node, adjacency_list[source_node]->destination);
}

/**
 * @brief Verifies depth calculations on a single root element.
 */
void test_compute_depth_single_node(void)
{
    // Arrange.
    int root_node = 1;

    // Act.
    compute_depths_and_ancestors(root_node, INVALID_NODE_ID, 0);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(0, node_depth[root_node]);
    TEST_ASSERT_EQUAL_INT(INVALID_NODE_ID, ancestor_table[root_node][0]);
}

/**
 * @brief Validates that binary lifting steps match sequential depths.
 */
void test_compute_depth_linear_chain(void)
{
    // Arrange.
    add(1, 2); add(2, 1);
    add(2, 3); add(3, 2);

    // Act.
    compute_depths_and_ancestors(1, INVALID_NODE_ID, 0);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(0, node_depth[1]);
    TEST_ASSERT_EQUAL_UINT(1, node_depth[2]);
    TEST_ASSERT_EQUAL_UINT(2, node_depth[3]);
    TEST_ASSERT_EQUAL_INT(1, ancestor_table[2][0]);
    TEST_ASSERT_EQUAL_INT(2, ancestor_table[3][0]);
}

/**
 * @brief Verifies table population over exponential jumps.
 */
void test_sparse_table_exponential_jumps(void)
{
    // Arrange.
    add(1, 2);
    add(2, 1);
    add(2, 3);
    add(3, 2);
    add(3, 4);
    add(4, 3);
    add(4, 5); 
    add(5, 4);

    // Act.
    compute_depths_and_ancestors(1, INVALID_NODE_ID, 0);

    // Assert.
    TEST_ASSERT_EQUAL_INT(4, ancestor_table[5][0]);
    TEST_ASSERT_EQUAL_INT(3, ancestor_table[5][1]);
}

/**
 * @brief Validates that invalid query nodes immediately exit.
 */
void test_kth_ancestor_out_of_bounds_returns_invalid(void)
{
    // Arrange.
    int invalid_source_node = -5;

    // Act.
    int tracking_result = kth_ancestor(invalid_source_node, 1);

    // Assert.
    TEST_ASSERT_EQUAL_INT(INVALID_NODE_ID, tracking_result);
}

/**
 * @brief Verifies zero value queries map back to the starting node.
 */
void test_kth_ancestor_zero_returns_self(void)
{
    // Arrange.
    add(1, 2);
    add(2, 1);

    compute_depths_and_ancestors(1, INVALID_NODE_ID, 0);

    // Act.
    int tracking_result = kth_ancestor(2, 0);

    // Assert.
    TEST_ASSERT_EQUAL_INT(2, tracking_result);
}

/**
 * @brief Validates a single-step ancestor lookup query.
 */
void test_kth_ancestor_first_parent(void)
{
    // Arrange.
    add(1, 2);
    add(2, 1);

    compute_depths_and_ancestors(1, INVALID_NODE_ID, 0);

    // Act.
    int tracking_result = kth_ancestor(2, 1);

    // Assert.
    TEST_ASSERT_EQUAL_INT(1, tracking_result);
}

/**
 * @brief Verifies multiple level ancestor lifting execution.
 */
void test_kth_ancestor_large_jump(void)
{
    // Arrange.
    add(1, 2);
    add(2, 1);
    add(2, 3);
    add(3, 2);
    add(3, 4);
    add(4, 3);
    add(4, 5);
    add(5, 4);

    compute_depths_and_ancestors(1, INVALID_NODE_ID, 0);

    // Act.
    int tracking_result = kth_ancestor(5, 3);

    // Assert.
    TEST_ASSERT_EQUAL_INT(2, tracking_result);
}

/**
 * @brief Validates that querying past the root bounds drops to invalid.
 */
void test_kth_ancestor_past_root_returns_invalid(void)
{
    // Arrange.
    add(1, 2);
    add(2, 1);

    compute_depths_and_ancestors(1, INVALID_NODE_ID, 0);

    // Act.
    int tracking_result = kth_ancestor(2, 5);

    // Assert.
    TEST_ASSERT_EQUAL_INT(INVALID_NODE_ID, tracking_result);
}

/**
 * @brief Verifies out of bounds checks on LCA query parameters.
 */
void test_lowest_common_ancestor_out_of_bounds_returns_invalid(void)
{
    // Arrange.
    int invalid_source_node = -1;
    int valid_node = 2;

    // Act.
    int tracking_result_one = lowest_common_ancestor(invalid_source_node, valid_node);
    int tracking_result_two = lowest_common_ancestor(valid_node, MAXIMUM_NODE_COUNT);

    // Assert.
    TEST_ASSERT_EQUAL_INT(INVALID_NODE_ID, tracking_result_one);
    TEST_ASSERT_EQUAL_INT(INVALID_NODE_ID, tracking_result_two);
}

/**
 * @brief Validates that LCA of identical nodes returns that node id.
 */
void test_lowest_common_ancestor_same_node(void)
{
    // Arrange.
    add(1, 2);
    add(2, 1);

    compute_depths_and_ancestors(1, INVALID_NODE_ID, 0);

    // Act.
    int tracking_result = lowest_common_ancestor(2, 2);

    // Assert.
    TEST_ASSERT_EQUAL_INT(2, tracking_result);
}

/**
 * @brief Verifies LCA determination when one node is a parent of another.
 */
void test_lowest_common_ancestor_direct_lineage(void)
{
    // Arrange.
    add(1, 2);
    add(2, 1);
    add(2, 3);
    add(3, 2);

    compute_depths_and_ancestors(1, INVALID_NODE_ID, 0);

    // Act.
    int tracking_result = lowest_common_ancestor(1, 3);

    // Assert.
    TEST_ASSERT_EQUAL_INT(1, tracking_result);
}

/**
 * @brief Validates LCA detection over asymmetric branching configurations.
 */
void test_lowest_common_ancestor_split_branches(void)
{
    // Arrange.
    add(1, 2);
    add(2, 1);
    add(1, 3);
    add(3, 1);

    compute_depths_and_ancestors(1, INVALID_NODE_ID, 0);

    // Act.
    int tracking_result = lowest_common_ancestor(2, 3);

    // Assert.
    TEST_ASSERT_EQUAL_INT(1, tracking_result);
}

/**
 * @brief Verifies complex convergence inside deep structural nodes.
 */
void test_lowest_common_ancestor_deep_tree(void)
{
    // Arrange.
    add(1, 2);
    add(2, 1);
    add(1, 3);
    add(3, 1);
    add(2, 4);
    add(4, 2);
    add(2, 5);
    add(5, 2);
    add(5, 7);
    add(7, 5);
    add(3, 6);
    add(6, 3);

    compute_depths_and_ancestors(1, INVALID_NODE_ID, 0);

    // Act.
    int tracking_result_one = lowest_common_ancestor(4, 7);
    int tracking_result_two = lowest_common_ancestor(4, 6);

    // Assert.
    TEST_ASSERT_EQUAL_INT(2, tracking_result_one);
    TEST_ASSERT_EQUAL_INT(1, tracking_result_two);
}

/**
 * @brief Validates that depth difference code paths operate symmetrically.
 */
void test_lowest_common_ancestor_parameter_order_symmetry(void)
{
    // Arrange.
    add(1, 2);
    add(2, 1);
    add(2, 4);
    add(4, 2);
    add(1, 3);
    add(3, 1);

    compute_depths_and_ancestors(1, INVALID_NODE_ID, 0);

    // Act.
    int tracking_result_one = lowest_common_ancestor(4, 3);
    int tracking_result_two = lowest_common_ancestor(3, 4);

    // Assert.
    TEST_ASSERT_EQUAL_INT(1, tracking_result_one);
    TEST_ASSERT_EQUAL_INT(1, tracking_result_two);
}

/**
 * @brief Verifies multi-level binary lifting jump evaluations.
 */
void test_kth_ancestor_exact_power_of_two(void)
{
    // Arrange.
    add(1, 2);
    add(2, 1);
    add(2, 3);
    add(3, 2);
    add(3, 4);
    add(4, 3);
    add(4, 5);
    add(5, 4);

    compute_depths_and_ancestors(1, INVALID_NODE_ID, 0);

    // Act.
    int tracking_result = kth_ancestor(5, 4);

    // Assert.
    TEST_ASSERT_EQUAL_INT(1, tracking_result);
}

/**
 * @brief Validates that non-power-of-two values decompose cleanly.
 */
void test_kth_ancestor_non_power_of_two(void)
{
    // Arrange.
    add(1, 2);
    add(2, 1);
    add(2, 3);
    add(3, 2);
    add(3, 4);
    add(4, 3);
    add(4, 5);
    add(5, 4);

    compute_depths_and_ancestors(1, INVALID_NODE_ID, 0);

    // Act.
    int tracking_result = kth_ancestor(5, 3);

    // Assert.
    TEST_ASSERT_EQUAL_INT(2, tracking_result);
}

/**
 * @brief Verifies defensive check handling for cyclical dependencies.
 */
void test_compute_depth_prevents_cyclic_loops(void)
{
    // Arrange.
    add(1, 2);
    add(2, 1);
    add(2, 1);
    add(1, 2);

    // Act.
    compute_depths_and_ancestors(1, INVALID_NODE_ID, 0);

    // Assert.
    TEST_ASSERT_TRUE(visited[1]);
    TEST_ASSERT_TRUE(visited[2]);
}

/**
 * @brief Validates that invalid lookup table indexes return invalid codes.
 */
void test_sparse_table_unreachable_elements_remain_invalid(void)
{
    // Arrange.
    add(1, 2);
    add(2, 1);

    compute_depths_and_ancestors(1, INVALID_NODE_ID, 0);

    // Act.
    int tracking_result = ancestor_table[2][5];

    // Assert.
    TEST_ASSERT_EQUAL_INT(INVALID_NODE_ID, tracking_result);
}

/**
 * @brief Verifies mapping patterns over uniform star-shaped configurations.
 */
void test_star_topology_lca(void)
{
    // Arrange.
    add(1, 2);
    add(2, 1);
    add(1, 3);
    add(3, 1);
    add(1, 4);
    add(4, 1);
    add(1, 5);
    add(5, 1);

    compute_depths_and_ancestors(1, INVALID_NODE_ID, 0);

    // Act.
    int tracking_result_one = lowest_common_ancestor(2, 5);
    int tracking_result_two = lowest_common_ancestor(3, 4);

    // Assert.
    TEST_ASSERT_EQUAL_INT(1, tracking_result_one);
    TEST_ASSERT_EQUAL_INT(1, tracking_result_two);
}

/**
 * @brief Validates that deep structures execute without execution faults.
 */
void test_deep_linear_chain_lca(void)
{
    // Arrange.
    add(1, 2);
    add(2, 1);
    add(2, 3);
    add(3, 2);
    add(3, 4);
    add(4, 3);
    add(4, 5);
    add(5, 4);
    add(5, 6);
    add(6, 5);

    compute_depths_and_ancestors(1, INVALID_NODE_ID, 0);

    // Act.
    int tracking_result = lowest_common_ancestor(4, 6);

    // Assert.
    TEST_ASSERT_EQUAL_INT(4, tracking_result);
}

/**
 * @brief Verifies validation constraints for maximum node ceiling counts.
 */
void test_edge_addition_at_boundary_limits(void)
{
    // Arrange.
    int upper_node_id = MAXIMUM_NODE_COUNT - 1;

    // Act.
    bool tracking_status = add(upper_node_id, 0);

    // Assert.
    TEST_ASSERT_TRUE(tracking_status);
}

/**
 * @brief Validates execution tracking loops over completely disjoint roots.
 */
void test_disjoint_subtrees_returns_invalid_lca(void)
{
    // Arrange.
    add(1, 2);
    add(2, 1);
    add(3, 4);
    add(4, 3);

    compute_depths_and_ancestors(1, INVALID_NODE_ID, 0);
    compute_depths_and_ancestors(3, INVALID_NODE_ID, 0);

    // Act.
    int tracking_result = lowest_common_ancestor(2, 4);

    // Assert.
    TEST_ASSERT_EQUAL_INT(INVALID_NODE_ID, tracking_result);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_initialize_clears_global_tables);
    RUN_TEST(test_add_edge_out_of_bounds_fails);
    RUN_TEST(test_add_edge_allocates_successfully);
    RUN_TEST(test_compute_depth_single_node);
    RUN_TEST(test_compute_depth_linear_chain);
    RUN_TEST(test_sparse_table_exponential_jumps);
    RUN_TEST(test_kth_ancestor_out_of_bounds_returns_invalid);
    RUN_TEST(test_kth_ancestor_zero_returns_self);
    RUN_TEST(test_kth_ancestor_first_parent);
    RUN_TEST(test_kth_ancestor_large_jump);
    RUN_TEST(test_kth_ancestor_past_root_returns_invalid);
    RUN_TEST(test_lowest_common_ancestor_out_of_bounds_returns_invalid);
    RUN_TEST(test_lowest_common_ancestor_same_node);
    RUN_TEST(test_lowest_common_ancestor_direct_lineage);
    RUN_TEST(test_lowest_common_ancestor_split_branches);
    RUN_TEST(test_lowest_common_ancestor_deep_tree);
    RUN_TEST(test_lowest_common_ancestor_parameter_order_symmetry);
    RUN_TEST(test_kth_ancestor_exact_power_of_two);
    RUN_TEST(test_kth_ancestor_non_power_of_two);
    RUN_TEST(test_compute_depth_prevents_cyclic_loops);
    RUN_TEST(test_sparse_table_unreachable_elements_remain_invalid);
    RUN_TEST(test_star_topology_lca);
    RUN_TEST(test_deep_linear_chain_lca);
    RUN_TEST(test_edge_addition_at_boundary_limits);
    RUN_TEST(test_disjoint_subtrees_returns_invalid_lca);

    // Destroy the session.
    return UNITY_END();
}