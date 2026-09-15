/**
 * @file        :   bellman_ford_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit tests for the Bellman-Ford shortest path algorithm.
 * @details     :   Contains 20 unique testing scenarios ranging from basic functionality 
 * and negative cycle traps to robust boundary conditions and edge cases.
 * Compile:
 * clang ./tests/darwin/graph/bellman_ford_unit_tests.c ./src/darwin/graph/bellman-ford/bellman_ford.c ./src/unity.c -Iinclude -o ./bin/bellman_ford_unit_test
 * @version     :   1.0
 * @date        :   2026-05-24
 */
#include <stdio.h>
#include <stdlib.h>

#include "unity.h"
#include "darwin/graph/bellman_ford.h"

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
 * @brief Ensure initialization succeeds given completely valid metrics.
 */
void test_initialize_with_valid_parameters(void)
{
    // Arrange.
    size_t total_vertices = 5;
    size_t total_edges = 10;

    // Act.
    Graph* graph = initialize(total_vertices, total_edges);

    // Assert.
    TEST_ASSERT_NOT_NULL(graph);
    TEST_ASSERT_NOT_NULL(graph->array);
    TEST_ASSERT_EQUAL_INT(5, graph->total_vertices);
    TEST_ASSERT_EQUAL_INT(10, graph->total_edges);
    
    free(graph->array);
    free(graph);
}

/**
 * @brief Guard against initializing a graph with zero vertices.
 */
void test_initialize_fails_with_zero_vertices(void)
{
    // Arrange.
    size_t total_vertices = 0;
    size_t total_edges = 5;

    // Act.
    Graph* graph = initialize(total_vertices, total_edges);

    // Assert.
    TEST_ASSERT_NULL(graph);
}

/**
 * @brief Guard against initializing a graph with zero edges.
 */
void test_initialize_fails_with_zero_edges(void)
{
    // Arrange.
    size_t total_vertices = 5;
    size_t total_edges = 0;

    // Act.
    Graph* graph = initialize(total_vertices, total_edges);

    // Assert.
    TEST_ASSERT_NULL(graph);
}

/**
 * @brief Ensure the algorithm safely handles a completely NULL graph reference.
 */
void test_bellman_ford_handles_null_graph(void)
{
    // Arrange.
    size_t source = 0;

    // Act.
    bool status = bellman_ford(NULL, source);

    // Assert.
    TEST_ASSERT_FALSE(status);
}

/**
 * @brief Guard against setting an initial source vertex that is out of bounds.
 */
void test_bellman_ford_fails_when_source_vertex_out_of_bounds(void)
{
    // Arrange.
    size_t total_vertices = 3;
    size_t total_edges = 2;
    size_t source = 3;

    Graph* graph = initialize(total_vertices, total_edges);

    TEST_ASSERT_NOT_NULL(graph);

    // Act.
    bool status = bellman_ford(graph, source);

    // Assert.
    TEST_ASSERT_FALSE(status);

    free(graph->array);
    free(graph);
}

/**
 * @brief Ensure isolated edges with out-of-bounds target domains are caught safely.
 */
void test_bellman_ford_skips_malformed_out_of_bounds_edges(void)
{
    // Arrange.
    size_t total_vertices = 3;
    size_t total_edges = 1;
    size_t source = 0;

    Graph* graph = initialize(total_vertices, total_edges);

    TEST_ASSERT_NOT_NULL(graph);

    graph->array[0].source = 0;
    graph->array[0].destination = 99;
    graph->array[0].weight = 5;

    // Act.
    bool status = bellman_ford(graph, source);

    // Assert.
    TEST_ASSERT_TRUE(status);

    free(graph->array);
    free(graph);
}

/**
 * @brief Standard simple single-path validation.
 */
void test_bellman_ford_computes_simple_linear_path(void)
{
    // Arrange.
    size_t total_vertices = 3;
    size_t total_edges = 2;
    size_t source = 0;

    Graph* graph = initialize(total_vertices, total_edges);

    TEST_ASSERT_NOT_NULL(graph);

    graph->array[0].source = 0;
    graph->array[0].destination = 1;
    graph->array[0].weight = 10;

    graph->array[1].source = 1;
    graph->array[1].destination = 2;
    graph->array[1].weight = 5;

    // Act.
    bool status = bellman_ford(graph, source);

    // Assert.
    TEST_ASSERT_TRUE(status);

    free(graph->array);
    free(graph);
}

/**
 * @brief Ensure a shorter path with more hops is selected over a direct long path.
 */
void test_bellman_ford_prefers_cheaper_multi_hop_path_over_expensive_direct_edge(void)
{
    // Arrange.
    size_t total_vertices = 3;
    size_t total_edges = 3;
    size_t source = 0;

    Graph* graph = initialize(total_vertices, total_edges);

    TEST_ASSERT_NOT_NULL(graph);

    graph->array[0].source = 0;
    graph->array[0].destination = 2;
    graph->array[0].weight = 20;

    graph->array[1].source = 0;
    graph->array[1].destination = 1;
    graph->array[1].weight = 4;

    graph->array[2].source = 1;
    graph->array[2].destination = 2;
    graph->array[2].weight = 3;

    // Act.
    bool status = bellman_ford(graph, source);

    // Assert.
    TEST_ASSERT_TRUE(status);

    free(graph->array);
    free(graph);
}

/**
 * @brief Handle standard graph structure with missing connections (unreachable nodes).
 */
void test_bellman_ford_handles_unreachable_vertices(void)
{
    // Arrange.
    size_t total_vertices = 3;
    size_t total_edges = 1;
    size_t source = 0;

    Graph* graph = initialize(total_vertices, total_edges);

    TEST_ASSERT_NOT_NULL(graph);

    graph->array[0].source = 0;
    graph->array[0].destination = 1;
    graph->array[0].weight = 2;

    // Act.
    bool status = bellman_ford(graph, source);

    // Assert.
    TEST_ASSERT_TRUE(status);

    free(graph->array);
    free(graph);
}

/**
 * @brief Validate functionality when evaluating a single single-node graph.
 */
void test_bellman_ford_with_single_vertex_graph(void)
{
    // Arrange.
    size_t total_vertices = 1;
    size_t total_edges = 1;
    size_t source = 0;

    Graph* graph = initialize(total_vertices, total_edges);

    TEST_ASSERT_NOT_NULL(graph);

    graph->array[0].source = 0;
    graph->array[0].destination = 0;
    graph->array[0].weight = 5;

    // Act.
    bool status = bellman_ford(graph, source);

    // Assert.
    TEST_ASSERT_TRUE(status);

    free(graph->array);
    free(graph);
}

/**
 * @brief Standard multi-branch alternative routing containing safe negative links.
 */
void test_bellman_ford_with_safe_negative_weights(void)
{
    // Arrange.
    size_t total_vertices = 3;
    size_t total_edges = 2;
    size_t source = 0;

    Graph* graph = initialize(total_vertices, total_edges);

    TEST_ASSERT_NOT_NULL(graph);

    graph->array[0].source = 0;
    graph->array[0].destination = 1;
    graph->array[0].weight = 6;

    graph->array[1].source = 1;
    graph->array[1].destination = 2;
    graph->array[1].weight = -4;

    // Act.
    bool status = bellman_ford(graph, source);

    // Assert.
    TEST_ASSERT_TRUE(status);

    free(graph->array);
    free(graph);
}

/**
 * @brief Detect an explicit negative cycle trap and flag failure state.
 */
void test_bellman_ford_detects_obvious_negative_weight_cycle(void)
{
    // Arrange.
    size_t total_vertices = 2;
    size_t total_edges = 2;
    size_t source = 0;

    Graph* graph = initialize(total_vertices, total_edges);

    TEST_ASSERT_NOT_NULL(graph);

    graph->array[0].source = 0;
    graph->array[0].destination = 1;
    graph->array[0].weight = 2;

    graph->array[1].source = 1;
    graph->array[1].destination = 0;
    graph->array[1].weight = -5;

    // Act.
    bool status = bellman_ford(graph, source);

    // Assert.
    TEST_ASSERT_FALSE(status);

    free(graph->array);
    free(graph);
}

/**
 * @brief Ensure a standard positive-weight loop path doesn't trigger false cycle flags.
 */
void test_bellman_ford_ignores_positive_weight_cycles(void)
{
    // Arrange.
    size_t total_vertices = 2;
    size_t total_edges = 2;
    size_t source = 0;

    Graph* graph = initialize(total_vertices, total_edges);

    TEST_ASSERT_NOT_NULL(graph);

    graph->array[0].source = 0;
    graph->array[0].destination = 1;
    graph->array[0].weight = 4;

    graph->array[1].source = 1;
    graph->array[1].destination = 0;
    graph->array[1].weight = 2;

    // Act.
    bool status = bellman_ford(graph, source);

    // Assert.
    TEST_ASSERT_TRUE(status);

    free(graph->array);
    free(graph);
}

/**
 * @brief Detect isolated negative loops disconnected from the source root.
 */
void test_bellman_ford_ignores_unreachable_negative_weight_cycle(void)
{
    // Arrange.
    size_t total_vertices = 4;
    size_t total_edges = 3;
    size_t source = 0;

    Graph* graph = initialize(total_vertices, total_edges);

    TEST_ASSERT_NOT_NULL(graph);

    graph->array[0].source = 0;
    graph->array[0].destination = 1;
    graph->array[0].weight = 1;

    graph->array[1].source = 2;
    graph->array[1].destination = 3;
    graph->array[1].weight = -2;

    graph->array[2].source = 3;
    graph->array[2].destination = 2;
    graph->array[2].weight = -2;

    // Act.
    bool status = bellman_ford(graph, source);

    // Assert.
    TEST_ASSERT_TRUE(status);

    free(graph->array);
    free(graph);
}

/**
 * @brief Validate detection of an explicit negative self-loop single-node structure.
 */
void test_bellman_ford_detects_negative_self_loop(void)
{
    // Arrange.
    size_t total_vertices = 2;
    size_t total_edges = 2;
    size_t source = 0;

    Graph* graph = initialize(total_vertices, total_edges);

    TEST_ASSERT_NOT_NULL(graph);

    graph->array[0].source = 0;
    graph->array[0].destination = 1;
    graph->array[0].weight = 1;

    graph->array[1].source = 1;
    graph->array[1].destination = 1;
    graph->array[1].weight = -1;

    // Act.
    bool status = bellman_ford(graph, source);

    // Assert.
    TEST_ASSERT_FALSE(status);

    free(graph->array);
    free(graph);
}

/**
 * @brief Ensure zero-weight edges are handled correctly without path calculation loss.
 */
void test_bellman_ford_processes_zero_weight_edges(void)
{
    // Arrange.
    size_t total_vertices = 3;
    size_t total_edges = 2;
    size_t source = 0;

    Graph* graph = initialize(total_vertices, total_edges);

    TEST_ASSERT_NOT_NULL(graph);

    graph->array[0].source = 0;
    graph->array[0].destination = 1;
    graph->array[0].weight = 0;

    graph->array[1].source = 1;
    graph->array[1].destination = 2;
    graph->array[1].weight = 0;

    // Act.
    bool status = bellman_ford(graph, source);

    // Assert.
    TEST_ASSERT_TRUE(status);

    free(graph->array);
    free(graph);
}

/**
 * @brief Multi-branch structural test matching original configuration layout design parameters.
 */
void test_bellman_ford_with_complex_multi_branch_mesh(void)
{
    // Arrange.
    size_t total_vertices = 4;
    size_t total_edges = 5;
    size_t source = 0;

    Graph* graph = initialize(total_vertices, total_edges);

    TEST_ASSERT_NOT_NULL(graph);

    graph->array[0].source = 0;
    graph->array[0].destination = 1;
    graph->array[0].weight = 4;

    graph->array[1].source = 0;
    graph->array[1].destination = 2;
    graph->array[1].weight = 1;

    graph->array[2].source = 2;
    graph->array[2].destination = 1;
    graph->array[2].weight = 2;

    graph->array[3].source = 1;
    graph->array[3].destination = 3;
    graph->array[3].weight = 1;

    graph->array[4].source = 2;
    graph->array[4].destination = 3;
    graph->array[4].weight = 5;

    // Act.
    bool status = bellman_ford(graph, source);

    // Assert.
    TEST_ASSERT_TRUE(status);

    free(graph->array);
    free(graph);
}

/**
 * @brief Handle graph patterns where array entries appear completely unsorted.
 */
void test_bellman_ford_resolves_unsorted_edge_arrays(void)
{
    // Arrange.
    size_t total_vertices = 3;
    size_t total_edges = 2;
    size_t source = 0;

    Graph* graph = initialize(total_vertices, total_edges);

    TEST_ASSERT_NOT_NULL(graph);

    graph->array[0].source = 1;
    graph->array[0].destination = 2;
    graph->array[0].weight = 3;

    graph->array[1].source = 0;
    graph->array[1].destination = 1;
    graph->array[1].weight = 2;

    // Act.
    bool status = bellman_ford(graph, source);

    // Assert.
    TEST_ASSERT_TRUE(status);

    free(graph->array);
    free(graph);
}

/**
 * @brief Shield calculations against potential integer overflow under intense scaling limits.
 */
void test_bellman_ford_guards_against_positive_integer_overflow(void)
{
    // Arrange.
    size_t total_vertices = 3;
    size_t total_edges = 2;
    size_t source = 0;

    Graph* graph = initialize(total_vertices, total_edges);

    TEST_ASSERT_NOT_NULL(graph);

    graph->array[0].source = 0;
    graph->array[0].destination = 1;
    graph->array[0].weight = INT_MAX - 10;

    graph->array[1].source = 1;
    graph->array[1].destination = 2;
    graph->array[1].weight = 20;

    // Act.
    bool status = bellman_ford(graph, source);

    // Assert.
    TEST_ASSERT_TRUE(status);

    free(graph->array);
    free(graph);
}

/**
 * @brief Shield calculations against integer underflow from large negative edge values.
 */
void test_bellman_ford_guards_against_negative_integer_underflow(void)
{
    // Arrange.
    size_t total_vertices = 3;
    size_t total_edges = 2;
    size_t source = 0;

    Graph* graph = initialize(total_vertices, total_edges);

    TEST_ASSERT_NOT_NULL(graph);

    graph->array[0].source = 0;
    graph->array[0].destination = 1;
    graph->array[0].weight = INT_MIN + 10;

    graph->array[1].source = 1;
    graph->array[1].destination = 2;
    graph->array[1].weight = -20;

    // Act.
    bool status = bellman_ford(graph, source);

    // Assert.
    TEST_ASSERT_TRUE(status);

    free(graph->array);
    free(graph);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_initialize_with_valid_parameters);
    RUN_TEST(test_initialize_fails_with_zero_vertices);
    RUN_TEST(test_initialize_fails_with_zero_edges);
    RUN_TEST(test_bellman_ford_handles_null_graph);
    RUN_TEST(test_bellman_ford_fails_when_source_vertex_out_of_bounds);
    RUN_TEST(test_bellman_ford_skips_malformed_out_of_bounds_edges);
    RUN_TEST(test_bellman_ford_computes_simple_linear_path);
    RUN_TEST(test_bellman_ford_prefers_cheaper_multi_hop_path_over_expensive_direct_edge);
    RUN_TEST(test_bellman_ford_handles_unreachable_vertices);
    RUN_TEST(test_bellman_ford_with_single_vertex_graph);
    RUN_TEST(test_bellman_ford_with_safe_negative_weights);
    RUN_TEST(test_bellman_ford_detects_obvious_negative_weight_cycle);
    RUN_TEST(test_bellman_ford_ignores_positive_weight_cycles);
    RUN_TEST(test_bellman_ford_ignores_unreachable_negative_weight_cycle);
    RUN_TEST(test_bellman_ford_detects_negative_self_loop);
    RUN_TEST(test_bellman_ford_processes_zero_weight_edges);
    RUN_TEST(test_bellman_ford_with_complex_multi_branch_mesh);
    RUN_TEST(test_bellman_ford_resolves_unsorted_edge_arrays);
    RUN_TEST(test_bellman_ford_guards_against_positive_integer_overflow);
    RUN_TEST(test_bellman_ford_guards_against_negative_integer_underflow);

    // Destroy the session.
    return UNITY_END();
}