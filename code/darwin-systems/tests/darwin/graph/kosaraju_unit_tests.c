/**
 * @file        :   kosaraju_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit tests for Kosaraju's SCC implementation.
 * @details     :   Utilizes the Unity Test Framework to validate graph 
 * initialization, edge manipulation, transposition, 
 * and SCC orchestration.
 * Compile:
 * clang ./tests/darwin/graph/kosaraju_unit_tests.c ./src/darwin/graph/kosaraju/kosaraju.c ./src/darwin/stack/generic-stack/generic_stack.c ./src/unity.c -Iinclude -o ./bin/kosaraju_unit_test
 * @version     :   1.0
 * @date        :   2026-05-05
 */
#include <stdio.h>
#include <string.h>

#include "unity.h"
#include "darwin/graph/kosaraju.h"

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
 * @brief Validates that a graph correctly allocates memory for a given size.
 */
void test_initialize_graph_valid_allocation(void)
{
    // Arrange.
    size_t vertices = 5;

    // Act.
    Graph* graph = initialize_graph(vertices);

    // Assert.
    TEST_ASSERT_NOT_NULL(graph);
    TEST_ASSERT_EQUAL_INT(5, graph->total_vertices);

    destroy_graph(graph);
}

/**
 * @brief Ensures initialization returns NULL when zero vertices are requested.
 */
void test_initialize_graph_zero_vertices_returns_null(void)
{
    // Arrange.
    size_t vertices = 0;

    // Act.
    Graph* graph = initialize_graph(vertices);

    // Assert.
    TEST_ASSERT_NULL(graph);
}

/**
 * @brief Validates that edges are correctly added to the adjacency list.
 */
void test_add_edge_successful_insertion(void)
{
    // Arrange.
    Graph* graph = initialize_graph(2);

    // Act.
    add_edge(graph, 0, 1);

    // Assert.
    TEST_ASSERT_NOT_NULL(graph->adjacency_list[0]);
    TEST_ASSERT_EQUAL_INT(1, graph->adjacency_list[0]->id);

    destroy_graph(graph);
}

/**
 * @brief Ensures add_edge handles NULL graph pointers without crashing.
 */
void test_add_edge_null_graph_fails_gracefully(void)
{
    // Arrange.
    Graph* graph = NULL;

    // Act.
    add_edge(graph, 0, 1);

    // Assert.
    TEST_ASSERT_NULL(graph);
}

/**
 * @brief Validates that add_edge rejects source indices that are out of bounds.
 */
void test_add_edge_source_out_of_bounds(void)
{
    // Arrange.
    Graph* graph = initialize_graph(2);

    // Act.
    add_edge(graph, 5, 1);

    // Assert.
    TEST_ASSERT_NULL(graph->adjacency_list[0]);
    TEST_ASSERT_NULL(graph->adjacency_list[1]);

    destroy_graph(graph);
}

/**
 * @brief Validates that add_edge rejects destination indices that are out of bounds.
 */
void test_add_edge_destination_out_of_bounds(void)
{
    // Arrange.
    Graph* graph = initialize_graph(2);

    // Act.
    add_edge(graph, 0, 5);

    // Assert.
    TEST_ASSERT_NULL(graph->adjacency_list[0]);

    destroy_graph(graph);
}

/**
 * @brief Validates that generate_transposed correctly reverses a single edge.
 */
void test_generate_transposed_correct_reversal(void)
{
    // Arrange.
    Graph* graph = initialize_graph(2);

    add_edge(graph, 0, 1);

    // Act.
    Graph* transposed = generate_transposed(graph);

    // Assert.
    TEST_ASSERT_NOT_NULL(transposed->adjacency_list[1]);
    TEST_ASSERT_EQUAL_INT(0, transposed->adjacency_list[1]->id);

    destroy_graph(graph);
    destroy_graph(transposed);
}

/**
 * @brief Ensures generate_transposed returns NULL when input is NULL.
 */
void test_generate_transposed_null_input(void)
{
    // Arrange.
    Graph* graph = NULL;

    // Act.
    Graph* transposed = generate_transposed(graph);

    // Assert.
    TEST_ASSERT_NULL(transposed);
}

/**
 * @brief Validates that a transposed graph retains the same vertex count.
 */
void test_generate_transposed_retains_vertex_count(void)
{
    // Arrange.
    Graph* graph = initialize_graph(10);

    // Act.
    Graph* transposed = generate_transposed(graph);

    // Assert.
    TEST_ASSERT_EQUAL_INT(10, transposed->total_vertices);

    destroy_graph(graph);
    destroy_graph(transposed);
}

/**
 * @brief Ensures the SCC orchestrator handles NULL graph input gracefully.
 */
void test_strongly_connected_components_null_graph(void)
{
    // Arrange.
    Graph* graph = NULL;

    // Act.
    strongly_connected_components(graph);

    // Assert.
    TEST_ASSERT_NULL(graph);
}

/**
 * @brief Ensures destroy_graph handles NULL input without a segmentation fault.
 */
void test_destroy_graph_null_graph_handled(void)
{
    // Arrange.
    Graph* graph = NULL;

    // Act.
    destroy_graph(graph);

    // Assert.
    TEST_ASSERT_NULL(graph);
}

/**
 * @brief Validates that self-loops are correctly stored in the graph.
 */
void test_add_edge_self_loop_insertion(void)
{
    // Arrange.
    Graph* graph = initialize_graph(1);

    // Act.
    add_edge(graph, 0, 0);

    // Assert.
    TEST_ASSERT_NOT_NULL(graph->adjacency_list[0]);
    TEST_ASSERT_EQUAL_INT(0, graph->adjacency_list[0]->id);

    destroy_graph(graph);
}

/**
 * @brief Validates that multiple edges between the same nodes are stored.
 */
void test_add_edge_duplicate_edge_insertion(void)
{
    // Arrange.
    Graph* graph = initialize_graph(2);

    // Act.
    add_edge(graph, 0, 1);
    add_edge(graph, 0, 1);

    // Assert.
    TEST_ASSERT_NOT_NULL(graph->adjacency_list[0]->next);
    TEST_ASSERT_EQUAL_INT(1, graph->adjacency_list[0]->next->id);

    destroy_graph(graph);
}

/**
 * @brief Validates that the adjacency list is properly zero-initialized.
 */
void test_initialize_graph_heads_are_null(void)
{
    // Arrange.
    size_t count = 3;

    // Act.
    Graph* graph = initialize_graph(count);

    // Assert.
    TEST_ASSERT_NULL(graph->adjacency_list[0]);
    TEST_ASSERT_NULL(graph->adjacency_list[1]);
    TEST_ASSERT_NULL(graph->adjacency_list[2]);

    destroy_graph(graph);
}

/**
 * @brief Ensures the SCC algorithm executes on a single-node graph.
 */
void test_scc_execution_on_single_node(void)
{
    // Arrange.
    Graph* graph = initialize_graph(1);

    // Act.
    strongly_connected_components(graph);

    // Assert.
    TEST_ASSERT_EQUAL_INT(1, graph->total_vertices);

    destroy_graph(graph);
}

/**
 * @brief Ensures the SCC algorithm executes on a fully disconnected graph.
 */
void test_scc_execution_on_disconnected_graph(void)
{
    // Arrange.
    Graph* graph = initialize_graph(3);

    // Act.
    strongly_connected_components(graph);

    // Assert.
    TEST_ASSERT_NULL(graph->adjacency_list[0]);

    destroy_graph(graph);
}

/**
 * @brief Ensures generate_transposed works on an empty graph with no edges.
 */
void test_generate_transposed_empty_graph(void)
{
    // Arrange.
    Graph* graph = initialize_graph(5);

    // Act.
    Graph* transposed = generate_transposed(graph);

    // Assert.
    TEST_ASSERT_NOT_NULL(transposed);
    TEST_ASSERT_NULL(transposed->adjacency_list[0]);

    destroy_graph(graph);
    destroy_graph(transposed);
}

/**
 * @brief Validates that the total_vertices field is correctly set.
 */
void test_graph_member_total_vertices_consistency(void)
{
    // Arrange.
    size_t expected = 42;

    // Act.
    Graph* graph = initialize_graph(expected);

    // Assert.
    TEST_ASSERT_EQUAL_INT(expected, graph->total_vertices);

    destroy_graph(graph);
}

/**
 * @brief Ensures the SCC algorithm handles a simple cyclic graph without failure.
 */
void test_scc_execution_on_cyclic_graph(void)
{
    // Arrange.
    Graph* graph = initialize_graph(2);

    add_edge(graph, 0, 1);
    add_edge(graph, 1, 0);

    // Act.
    strongly_connected_components(graph);

    // Assert.
    TEST_ASSERT_NOT_NULL(graph->adjacency_list[0]);

    destroy_graph(graph);
}

/**
 * @brief Ensures initialization fails if memory allocation for the list fails.
 * @details This simulates a very large allocation that is likely to fail.
 */
void test_initialize_graph_extreme_size_fails(void)
{
    // Arrange.
    size_t huge_count = (size_t)-1;

    // Act.
    Graph* graph = initialize_graph(huge_count);

    // Assert.
    TEST_ASSERT_NULL(graph);
}

/**
 * @brief Main test runner.
 */
int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_initialize_graph_valid_allocation);
    RUN_TEST(test_initialize_graph_zero_vertices_returns_null);
    RUN_TEST(test_add_edge_successful_insertion);
    RUN_TEST(test_add_edge_null_graph_fails_gracefully);
    RUN_TEST(test_add_edge_source_out_of_bounds);
    RUN_TEST(test_add_edge_destination_out_of_bounds);
    RUN_TEST(test_generate_transposed_correct_reversal);
    RUN_TEST(test_generate_transposed_null_input);
    RUN_TEST(test_generate_transposed_retains_vertex_count);
    RUN_TEST(test_strongly_connected_components_null_graph);
    RUN_TEST(test_destroy_graph_null_graph_handled);
    RUN_TEST(test_add_edge_self_loop_insertion);
    RUN_TEST(test_add_edge_duplicate_edge_insertion);
    RUN_TEST(test_initialize_graph_heads_are_null);
    RUN_TEST(test_scc_execution_on_single_node);
    RUN_TEST(test_scc_execution_on_disconnected_graph);
    RUN_TEST(test_generate_transposed_empty_graph);
    RUN_TEST(test_graph_member_total_vertices_consistency);
    RUN_TEST(test_scc_execution_on_cyclic_graph);
    RUN_TEST(test_initialize_graph_extreme_size_fails);

    // Destory the session.
    return UNITY_END();
}