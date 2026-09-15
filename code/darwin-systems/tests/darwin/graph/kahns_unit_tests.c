/**
 * @file        :   floyd_warshall_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit tests for Kahn's topological sort algorithm.
 * @details     :   Contains 20 unique testing scenarios ranging from graph validation,
 * empty and single-node buffers, to complex cycle traps, branching meshes, 
 * and defensive parameter boundary guard verifications.
 * Compile:
 * clang ./tests/darwin/graph/kahns_unit_tests.c ./src/darwin/graph/kahns/kahns.c ./src/unity.c -Iinclude -o ./bin/kahns_unit_test
 * @version     :   1.0
 * @date        :   2026-05-027
 */
#include <stdio.h>

#include "unity.h"
#include "darwin/graph/kahns.h"

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
 * @brief Ensure the algorithm safely flags a completely NULL graph pointer context.
 */
void test_kahns_handles_null_graph(void)
{
    // Act.
    bool status = kahns(NULL);

    // Assert.
    TEST_ASSERT_FALSE(status);
}

/**
 * @brief Confirm graph initialization gracefully rejects a zero vertex capacity allocation request.
 */
void test_kahns_initialize_rejects_zero_vertices(void)
{
    // Act.
    Graph* graph = initialize(0);

    // Assert.
    TEST_ASSERT_NULL(graph);
    
    // Clean.
    clean(graph);
}

/**
 * @brief Verify add_edge defends against a NULL graph context pointer.
 */
void test_kahns_add_edge_handles_null_graph(void)
{
    // Act.
    bool status = add_edge(NULL, 0, 1);

    // Assert.
    TEST_ASSERT_FALSE(status);
}

/**
 * @brief Verify add_edge rejects inputs where source violates number of vertices bounds.
 */
void test_kahns_add_edge_guards_against_out_of_bounds_source_index(void)
{
    // Arrange.
    Graph* graph = initialize(3);
    
    TEST_ASSERT_NOT_NULL(graph);

    // Act.
    bool status = add_edge(graph, 3, 1);

    // Assert.
    TEST_ASSERT_FALSE(status);

    // Clean.
    clean(graph);
}

/**
 * @brief Verify add_edge rejects inputs where destination violates number of vertices bounds.
 */
void test_kahns_add_edge_guards_against_out_of_bounds_destination_index(void)
{
    // Arrange.
    Graph* graph = initialize(3);

    TEST_ASSERT_NOT_NULL(graph);

    // Act.
    bool status = add_edge(graph, 1, 4);

    // Assert.
    TEST_ASSERT_FALSE(status);

    // Clean.
    clean(graph);
}

/**
 * @brief Process an empty initialized graph containing zero edge connections successfully.
 */
void test_kahns_with_fully_disconnected_empty_graph(void)
{
    // Arrange.
    Graph* graph = initialize(4);

    TEST_ASSERT_NOT_NULL(graph);

    // Act.
    bool status = kahns(graph);

    // Assert.
    TEST_ASSERT_TRUE(status);

    // Clean.
    clean(graph);
}

/**
 * @brief Support trivial resolution on a isolated graph domain possessing exactly one node.
 */
void test_kahns_with_isolated_single_node_graph(void)
{
    // Arrange.
    Graph* graph = initialize(1);

    TEST_ASSERT_NOT_NULL(graph);

    // Act.
    bool status = kahns(graph);

    // Assert.
    TEST_ASSERT_TRUE(status);

    // Clean.
    clean(graph);
}

/**
 * @brief Confirm sequence verification over a standard single-dependency pair mapping.
 */
void test_kahns_computes_simple_two_node_single_edge_path(void)
{
    // Arrange.
    Graph* graph = initialize(2);

    TEST_ASSERT_NOT_NULL(graph);

    bool edge_added = add_edge(graph, 0, 1);

    TEST_ASSERT_TRUE(edge_added);

    // Act.
    bool status = kahns(graph);

    // Assert.
    TEST_ASSERT_TRUE(status);

    // Clean.
    clean(graph);
}

/**
 * @brief Standard simple multi-node single-path sequential tracking validation.
 */
void test_kahns_computes_straight_linear_dependency_chain(void)
{
    // Arrange.
    Graph* graph = initialize(4);

    TEST_ASSERT_NOT_NULL(graph);

    bool flag = true;

    flag &= add_edge(graph, 0, 1);
    flag &= add_edge(graph, 1, 2);
    flag &= add_edge(graph, 2, 3);

    TEST_ASSERT_TRUE(flag);

    // Act.
    bool status = kahns(graph);

    // Assert.
    TEST_ASSERT_TRUE(status);

    // Clean.
    clean(graph);
}

/**
 * @brief Track optimization routines in fully split binary branch routing structures.
 */
void test_kahns_processes_symmetrical_diverging_binary_tree_branches(void)
{
    // Arrange.
    Graph* graph = initialize(3);

    TEST_ASSERT_NOT_NULL(graph);

    bool flag = true;

    flag &= add_edge(graph, 0, 1);
    flag &= add_edge(graph, 0, 2);

    TEST_ASSERT_TRUE(flag);

    // Act.
    bool status = kahns(graph);

    // Assert.
    TEST_ASSERT_TRUE(status);

    // Clean.
    clean(graph);
}

/**
 * @brief Handle scenarios where separate concurrent paths converge onto an identical node.
 */
void test_kahns_resolves_converging_multiple_parent_dependencies(void)
{
    // Arrange.
    Graph* graph = initialize(3);

    TEST_ASSERT_NOT_NULL(graph);

    bool flag = true;

    flag &= add_edge(graph, 0, 2);
    flag &= add_edge(graph, 1, 2);

    TEST_ASSERT_TRUE(flag);

    // Act.
    bool status = kahns(graph);

    // Assert.
    TEST_ASSERT_TRUE(status);

    // Clean.
    clean(graph);
}

/**
 * @brief Verify standard functionality inside classic diamond-shaped topology frameworks.
 */
void test_kahns_resolves_standard_four_node_diamond_dependency_mesh(void)
{
    // Arrange.
    Graph* graph = initialize(4);

    TEST_ASSERT_NOT_NULL(graph);

    bool flag = true;

    flag &= add_edge(graph, 0, 1);
    flag &= add_edge(graph, 0, 2);
    flag &= add_edge(graph, 1, 3);
    flag &= add_edge(graph, 2, 3);

    TEST_ASSERT_TRUE(flag);

    // Act.
    bool status = kahns(graph);

    // Assert.
    TEST_ASSERT_TRUE(status);

    // Clean.
    clean(graph);
}

/**
 * @brief Detect individual node loop traps where an edge loops directly back onto itself.
 */
void test_kahns_detects_explicit_single_node_self_loop_cycle(void)
{
    // Arrange.
    Graph* graph = initialize(2);

    TEST_ASSERT_NOT_NULL(graph);

    bool flag = true;

    flag &= add_edge(graph, 0, 1);
    flag &= add_edge(graph, 1, 1);

    TEST_ASSERT_TRUE(flag);

    // Act.
    bool status = kahns(graph);

    // Assert.
    TEST_ASSERT_FALSE(status);

    // Clean.
    clean(graph);
}

/**
 * @brief Detect an explicit direct two-node cyclic loop trap and drop implementation status.
 */
void test_kahns_detects_obvious_two_node_back_and_forth_cyclic_loop(void)
{
    // Arrange.
    Graph* graph = initialize(2);

    TEST_ASSERT_NOT_NULL(graph);

    bool flag = true;

    flag &= add_edge(graph, 0, 1);
    flag &= add_edge(graph, 1, 0);

    TEST_ASSERT_TRUE(flag);

    // Act.
    bool status = kahns(graph);

    // Assert.
    TEST_ASSERT_FALSE(status);

    // Clean.
    clean(graph);
}

/**
 * @brief Detect cycles when they loop back precisely to the source node entry point.
 */
void test_kahns_detects_multi_node_cycle_looping_back_to_origin(void)
{
    // Arrange.
    Graph* graph = initialize(3);

    TEST_ASSERT_NOT_NULL(graph);

    bool flag = true;

    flag &= add_edge(graph, 0, 1);
    flag &= add_edge(graph, 1, 2);
    flag &= add_edge(graph, 2, 0);

    TEST_ASSERT_TRUE(flag);

    // Act.
    bool status = kahns(graph);

    // Assert.
    TEST_ASSERT_FALSE(status);

    // Clean.
    clean(graph);
}

/**
 * @brief Detect cyclic interference even when nested deep within late structural transitions.
 */
void test_kahns_detects_deeply_nested_cycle_isolated_from_source_roots(void)
{
    // Arrange.
    Graph* graph = initialize(5);

    TEST_ASSERT_NOT_NULL(graph);

    bool flag = true;

    flag &= add_edge(graph, 0, 1);
    flag &= add_edge(graph, 1, 2);
    flag &= add_edge(graph, 2, 3);
    flag &= add_edge(graph, 3, 4);
    flag &= add_edge(graph, 4, 2);

    TEST_ASSERT_TRUE(flag);

    // Act.
    bool status = kahns(graph);

    // Assert.
    TEST_ASSERT_FALSE(status);

    // Clean.
    clean(graph);
}

/**
 * @brief Ensure cycle rules are evaluated correctly when a graph contains fully separate components.
 */
void test_kahns_handles_multiple_disconnected_valid_sub_components(void)
{
    // Arrange.
    Graph* graph = initialize(4);

    TEST_ASSERT_NOT_NULL(graph);

    bool flag = true;

    flag &= add_edge(graph, 0, 1);
    flag &= add_edge(graph, 2, 3);

    TEST_ASSERT_TRUE(flag);

    // Act.
    bool status = kahns(graph);

    // Assert.
    TEST_ASSERT_TRUE(status);

    // Clean.
    clean(graph);
}

/**
 * @brief Verify cycle flags are thrown if any single standalone cluster contains a cyclic loop.
 */
void test_kahns_detects_cycle_hidden_in_isolated_secondary_sub_component(void)
{
    // Arrange.
    Graph* graph = initialize(5);

    TEST_ASSERT_NOT_NULL(graph);

    bool flag = true;

    flag &= add_edge(graph, 0, 1);
    flag &= add_edge(graph, 2, 3);
    flag &= add_edge(graph, 3, 4);
    flag &= add_edge(graph, 4, 2);

    TEST_ASSERT_TRUE(flag);

    // Act.
    bool status = kahns(graph);

    // Assert.
    TEST_ASSERT_FALSE(status);

    // Clean.
    clean(graph);
}

/**
 * @brief Handle complex intertwined dynamic mesh structures containing variable dependency paths.
 */
void test_kahns_with_complex_multi_branch_intertwined_dag(void)
{
    // Arrange.
    Graph* graph = initialize(6);

    TEST_ASSERT_NOT_NULL(graph);

    bool flag = true;

    flag &= add_edge(graph, 5, 2);
    flag &= add_edge(graph, 5, 0);
    flag &= add_edge(graph, 4, 0);
    flag &= add_edge(graph, 4, 1);
    flag &= add_edge(graph, 2, 3);
    flag &= add_edge(graph, 3, 1);

    TEST_ASSERT_TRUE(flag);

    // Act.
    bool status = kahns(graph);

    // Assert.
    TEST_ASSERT_TRUE(status);

    // Clean.
    clean(graph);
}

/**
 * @brief Confirm calling clean against a safe NULL graph pointer context executes transparently.
 */
void test_kahns_clean_handles_null_graph_gracefully(void)
{
    // Act & Assert (Should pass without triggering a segmentation fault/crash).
    clean(NULL);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_kahns_handles_null_graph);
    RUN_TEST(test_kahns_initialize_rejects_zero_vertices);
    RUN_TEST(test_kahns_add_edge_handles_null_graph);
    RUN_TEST(test_kahns_add_edge_guards_against_out_of_bounds_source_index);
    RUN_TEST(test_kahns_add_edge_guards_against_out_of_bounds_destination_index);
    RUN_TEST(test_kahns_with_fully_disconnected_empty_graph);
    RUN_TEST(test_kahns_with_isolated_single_node_graph);
    RUN_TEST(test_kahns_computes_simple_two_node_single_edge_path);
    RUN_TEST(test_kahns_computes_straight_linear_dependency_chain);
    RUN_TEST(test_kahns_processes_symmetrical_diverging_binary_tree_branches);
    RUN_TEST(test_kahns_resolves_converging_multiple_parent_dependencies);
    RUN_TEST(test_kahns_resolves_standard_four_node_diamond_dependency_mesh);
    RUN_TEST(test_kahns_detects_explicit_single_node_self_loop_cycle);
    RUN_TEST(test_kahns_detects_obvious_two_node_back_and_forth_cyclic_loop);
    RUN_TEST(test_kahns_detects_multi_node_cycle_looping_back_to_origin);
    RUN_TEST(test_kahns_detects_deeply_nested_cycle_isolated_from_source_roots);
    RUN_TEST(test_kahns_handles_multiple_disconnected_valid_sub_components);
    RUN_TEST(test_kahns_detects_cycle_hidden_in_isolated_secondary_sub_component);
    RUN_TEST(test_kahns_with_complex_multi_branch_intertwined_dag);
    RUN_TEST(test_kahns_clean_handles_null_graph_gracefully);

    // Destroy the session.
    return UNITY_END();
}