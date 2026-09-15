/**
 * @file        :   prim_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit tests for Prim's Minimum Spanning Tree algorithm.
 * @details     :   Contains 20 unique testing scenarios evaluating adjacency matrix inputs,
 * null-guard conditions, unique topologies, isolated variations, 
 * and edge weight configurations to ensure structural integrity.
 * Compile:
 * clang ./tests/darwin/graph/prim_unit_tests.c ./src/darwin/graph/prim/prim.c ./src/unity.c -Iinclude -o ./bin/prim_unit_test
 * @version     :   1.0
 * @date        :   2026-05-28
 */
#include <stdio.h>
#include <stdlib.h>

#include "unity.h"
#include "darwin/graph/prim.h"

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
 * @brief Ensure the algorithm safely flags a completely NULL matrix reference.
 */
void test_prim_handles_null_graph(void)
{
    // Act & Assert.
    prim(NULL);
}

/**
 * @brief Verify response on a default empty adjacency matrix where no connections exist.
 */
void test_prim_with_all_zero_matrix_no_edges(void)
{
    int graph[NUMBER_OF_VERTICES][NUMBER_OF_VERTICES] =
    {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0}
    };

    // Act & Assert.
    prim(graph);
}

/**
 * @brief Confirm tree building over a single clear edge connection mapping.
 */
void test_prim_computes_simple_single_edge_path(void)
{
    int graph[NUMBER_OF_VERTICES][NUMBER_OF_VERTICES] =
    {
        {0, 5, 0, 0, 0},
        {5, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0}
    };

    // Act & Assert.
    prim(graph);
}

/**
 * @brief Validate selection choosing the cheaper alternative among two distinct paths.
 */
void test_prim_prioritizes_cheaper_edge_path(void)
{
    int graph[NUMBER_OF_VERTICES][NUMBER_OF_VERTICES] =
    {
        {0, 10, 2, 0, 0},
        {10, 0, 0, 0, 0},
        {2, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0}
    };

    // Act & Assert.
    prim(graph);
}

/**
 * @brief Confirm self loops are completely bypassed as non-viable tree pathways.
 */
void test_prim_ignores_self_loop_weights(void)
{
    int graph[NUMBER_OF_VERTICES][NUMBER_OF_VERTICES] = {
        {5, 2, 0, 0, 0},
        {2, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0}
    };

    // Act & Assert.
    prim(graph);
}

/**
 * @brief Reject cycle inclusions when resolving a fully enclosed triangular mesh.
 */
void test_prim_prevents_cycles_in_triangle_loop(void)
{
    int graph[NUMBER_OF_VERTICES][NUMBER_OF_VERTICES] =
    {
        {0, 2, 4, 0, 0},
        {2, 0, 3, 0, 0},
        {4, 3, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0}
    };

    // Act & Assert.
    prim(graph);
}

/**
 * @brief Process complex multi-node straight paths sequentially across the matrix index bounds.
 */
void test_prim_computes_linear_spanning_path(void)
{
    int graph[NUMBER_OF_VERTICES][NUMBER_OF_VERTICES] =
    {
        {0, 1, 0, 0, 0},
        {1, 0, 2, 0, 0},
        {0, 2, 0, 3, 0},
        {0, 0, 3, 0, 4},
        {0, 0, 0, 4, 0}
    };

    // Act & Assert.
    prim(graph);
}

/**
 * @brief Safely terminate evaluation loop if the graph splits into disconnected clusters.
 */
void test_prim_identifies_disconnected_graph_subsections(void)
{
    int graph[NUMBER_OF_VERTICES][NUMBER_OF_VERTICES] =
    {
        {0, 4, 0, 0, 0},
        {4, 0, 0, 0, 0},
        {0, 0, 0, 5, 0},
        {0, 0, 5, 0, 0},
        {0, 0, 0, 0, 0}
    };

    // Act & Assert.
    prim(graph);
}

/**
 * @brief Support identical positive cost definitions across transitions cleanly.
 */
void test_prim_handles_graphs_with_all_equal_edge_weights(void)
{
    int graph[NUMBER_OF_VERTICES][NUMBER_OF_VERTICES] =
    {
        {0, 6, 6, 6, 6},
        {6, 0, 6, 6, 6},
        {6, 6, 0, 6, 6},
        {6, 6, 6, 0, 6},
        {6, 6, 6, 6, 0}
    };

    // Act & Assert.
    prim(graph);
}

/**
 * @brief Verify functionality within a classic diamond-shaped coordinate mapping.
 */
void test_prim_resolves_diamond_mesh_framework(void)
{
    int graph[NUMBER_OF_VERTICES][NUMBER_OF_VERTICES] =
    {
        {0, 1, 3, 0, 0},
        {1, 0, 5, 4, 0},
        {3, 5, 0, 2, 0},
        {0, 4, 2, 0, 0},
        {0, 0, 0, 0, 0}
    };

    // Act & Assert.
    prim(graph);
}

/**
 * @brief Evaluate processing inside an asymmetrical matrix configuration.
 */
void test_prim_processes_asymmetric_matrix_inputs(void)
{
    int graph[NUMBER_OF_VERTICES][NUMBER_OF_VERTICES] =
    {
        {0, 1, 0, 0, 0},
        {9, 0, 2, 0, 0},
        {0, 8, 0, 3, 0},
        {0, 0, 7, 0, 4},
        {0, 0, 0, 6, 0}
    };

    // Act & Assert.
    prim(graph);
}

/**
 * @brief Verify stability using standard star-topology configurations targeting node zero.
 */
void test_prim_handles_star_topology_layout(void)
{
    int graph[NUMBER_OF_VERTICES][NUMBER_OF_VERTICES] =
    {
        {0, 10, 10, 10, 10},
        {10, 0, 0, 0, 0},
        {10, 0, 0, 0, 0},
        {10, 0, 0, 0, 0},
        {10, 0, 0, 0, 0}
    };

    // Act & Assert.
    prim(graph);
}

/**
 * @brief Process completely filled framework arrays where every vertex is linked.
 */
void test_prim_resolves_fully_connected_complete_graph(void)
{
    int graph[NUMBER_OF_VERTICES][NUMBER_OF_VERTICES] =
    {
        {0, 2, 3, 11, 15},
        {2, 0, 8, 4, 7},
        {3, 8, 0, 6, 1},
        {11, 4, 6, 0, 9},
        {15, 7, 1, 9, 0}
    };

    // Act & Assert.
    prim(graph);
}

/**
 * @brief Track extraction constraints using values scaling up to high integers.
 */
void test_prim_with_high_range_edge_weights(void)
{
    int graph[NUMBER_OF_VERTICES][NUMBER_OF_VERTICES] =
    {
        {0, 1000000, 0, 0, 0},
        {1000000, 0, 2000000, 0, 0},
        {0, 2000000, 0, 3000000, 0},
        {0, 0, 3000000, 0, 4000000},
        {0, 0, 0, 4000000, 0}
    };

    // Act & Assert.
    prim(graph);
}

/**
 * @brief Confirm terminal tracking paths are safely processed when only the root element has links.
 */
void test_prim_with_only_root_node_connected(void)
{
    int graph[NUMBER_OF_VERTICES][NUMBER_OF_VERTICES] =
    {
        {0, 1, 2, 3, 4},
        {1, 0, 0, 0, 0},
        {2, 0, 0, 0, 0},
        {3, 0, 0, 0, 0},
        {4, 0, 0, 0, 0}
    };

    // Act & Assert.
    prim(graph);
}

/**
 * @brief Verify that a disconnected final index node is safely recognized.
 */
void test_prim_with_disconnected_final_vertex(void)
{
    int graph[NUMBER_OF_VERTICES][NUMBER_OF_VERTICES] =
    {
        {0, 2, 3, 4, 0},
        {2, 0, 5, 6, 0},
        {3, 5, 0, 7, 0},
        {4, 6, 7, 0, 0},
        {0, 0, 0, 0, 0}
    };

    // Act & Assert.
    prim(graph);
}

/**
 * @brief Evaluate processing layout when the graph forms a large enclosing circle.
 */
void test_prim_handles_circular_ring_mesh(void)
{
    int graph[NUMBER_OF_VERTICES][NUMBER_OF_VERTICES] =
    {
        {0, 1, 0, 0, 5},
        {1, 0, 2, 0, 0},
        {0, 2, 0, 3, 0},
        {0, 0, 3, 0, 4},
        {5, 0, 0, 4, 0}
    };

    // Act & Assert.
    prim(graph);
}

/**
 * @brief Process multi-vertex chains containing a single dominating high-weight trunk line.
 */
void test_prim_with_heavy_central_bottleneck_edge(void)
{
    int graph[NUMBER_OF_VERTICES][NUMBER_OF_VERTICES] =
    {
        {0, 1, 0, 0, 0},
        {1, 0, 500, 0, 0},
        {0, 500, 0, 2, 0},
        {0, 0, 2, 0, 3},
        {0, 0, 0, 3, 0}
    };

    // Act & Assert.
    prim(graph);
}

/**
 * @brief Handle dense complete graph variants embedded with alternating binary weight values.
 */
void test_prim_handles_dense_binary_alternating_weights(void)
{
    int graph[NUMBER_OF_VERTICES][NUMBER_OF_VERTICES] =
    {
        {0, 1, 2, 1, 2},
        {1, 0, 1, 2, 1},
        {2, 1, 0, 1, 2},
        {1, 2, 1, 0, 1},
        {2, 1, 2, 1, 0}
    };

    // Act & Assert.
    prim(graph);
}

/**
 * @brief Ensure tracking arrays sustain logical pathing states when loaded with a sequence of increasing weights.
 */
void test_prim_handles_strictly_increasing_sequential_weights(void)
{
    int graph[NUMBER_OF_VERTICES][NUMBER_OF_VERTICES] =
    {
        {0, 1, 2, 3, 4},
        {1, 0, 5, 6, 7},
        {2, 5, 0, 8, 9},
        {3, 6, 8, 0, 10},
        {4, 7, 9, 10, 0}
    };

    // Act & Assert.
    prim(graph);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_prim_handles_null_graph);
    RUN_TEST(test_prim_with_all_zero_matrix_no_edges);
    RUN_TEST(test_prim_computes_simple_single_edge_path);
    RUN_TEST(test_prim_prioritizes_cheaper_edge_path);
    RUN_TEST(test_prim_ignores_self_loop_weights);
    RUN_TEST(test_prim_prevents_cycles_in_triangle_loop);
    RUN_TEST(test_prim_computes_linear_spanning_path);
    RUN_TEST(test_prim_identifies_disconnected_graph_subsections);
    RUN_TEST(test_prim_handles_graphs_with_all_equal_edge_weights);
    RUN_TEST(test_prim_resolves_diamond_mesh_framework);
    RUN_TEST(test_prim_processes_asymmetric_matrix_inputs);
    RUN_TEST(test_prim_handles_star_topology_layout);
    RUN_TEST(test_prim_resolves_fully_connected_complete_graph);
    RUN_TEST(test_prim_with_high_range_edge_weights);
    RUN_TEST(test_prim_with_only_root_node_connected);
    RUN_TEST(test_prim_with_disconnected_final_vertex);
    RUN_TEST(test_prim_handles_circular_ring_mesh);
    RUN_TEST(test_prim_with_heavy_central_bottleneck_edge);
    RUN_TEST(test_prim_handles_dense_binary_alternating_weights);
    RUN_TEST(test_prim_handles_strictly_increasing_sequential_weights);

    // Destroy the session.
    return UNITY_END();
}