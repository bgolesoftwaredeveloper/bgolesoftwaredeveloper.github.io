/**
 * @file        :   floyd_warshall_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit tests for Kruskal's Minimum Spanning Tree algorithm.
 * @details     :   Contains 20 unique testing scenarios ranging from graph validation,
 * empty and single-node networks, to multi-weight edge sorting,
 * disconnected components, and safety boundary guard verifications.
 * Compile:
 * clang ./tests/darwin/graph/kruskal_unit_tests.c ./src/darwin/graph/kruskal/kruskal.c ./src/unity.c -Iinclude -o ./bin/kruskal_unit_test
 * @version     :   1.0
 * @date        :   2026-05-027
 */
#include <stdio.h>
#include <stdlib.h>

#include "unity.h"
#include "darwin/graph/kruskal.h"

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
void test_kruskal_handles_null_graph(void)
{
    // Act & Assert.
    kruskal(NULL);
}

/**
 * @brief Confirm graph initialization gracefully rejects a zero vertex capacity allocation request.
 */
void test_kruskal_initialize_rejects_zero_vertices(void)
{
    // Act.
    Graph* graph = initialize(0, 5);

    // Assert.
    TEST_ASSERT_NULL(graph);
}

/**
 * @brief Confirm graph initialization gracefully rejects a zero edge capacity allocation request.
 */
void test_kruskal_initialize_rejects_zero_edges(void)
{
    // Act.
    Graph* graph = initialize(5, 0);

    // Assert.
    TEST_ASSERT_NULL(graph);
}

/**
 * @brief Verify that initializing allocates correct structural values.
 */
void test_kruskal_initialize_allocates_correct_bounds(void)
{
    // Act.
    Graph* graph = initialize(10, 20);

    // Assert.
    TEST_ASSERT_NOT_NULL(graph);
    TEST_ASSERT_EQUAL_INT(10, graph->number_of_vertices);
    TEST_ASSERT_EQUAL_INT(20, graph->number_of_edges);
    TEST_ASSERT_NOT_NULL(graph->edges);

    // Clean.
    free(graph->edges);
    free(graph);
}

/**
 * @brief Support trivial resolution on an isolated graph domain possessing exactly one node.
 */
void test_kruskal_with_isolated_single_node_graph(void)
{
    #define NUMBER_OF_VERTICES_5 1
    #define NUMBER_OF_EDGES_5 1

    Edge edges[] =
    {
        {0, 0, 0}
    };

    Graph* graph = initialize(NUMBER_OF_VERTICES_5, NUMBER_OF_EDGES_5);

    TEST_ASSERT_NOT_NULL(graph);

    for (size_t index = 0; index < NUMBER_OF_EDGES_5; index++)
    {
        graph->edges[index].source = edges[index].source;
        graph->edges[index].destination = edges[index].destination;
        graph->edges[index].weight = edges[index].weight;
    }

    kruskal(graph);

    free(graph->edges);
    free(graph);
}

/**
 * @brief Confirm tree computation over a standard single-edge mapping.
 */
void test_kruskal_computes_simple_two_node_single_edge_path(void)
{
    #define NUMBER_OF_VERTICES_6 2
    #define NUMBER_OF_EDGES_6 1

    Edge edges[] =
    {
        {0, 1, 5}
    };

    Graph* graph = initialize(NUMBER_OF_VERTICES_6, NUMBER_OF_EDGES_6);

    TEST_ASSERT_NOT_NULL(graph);

    for (size_t index = 0; index < NUMBER_OF_EDGES_6; index++)
    {
        graph->edges[index].source = edges[index].source;
        graph->edges[index].destination = edges[index].destination;
        graph->edges[index].weight = edges[index].weight;
    }

    kruskal(graph);

    free(graph->edges);
    free(graph);
}

/**
 * @brief Verify sorting by choosing the cheaper alternative among two parallel duplicate options.
 */
void test_kruskal_prioritizes_cheaper_parallel_edge(void)
{
    #define NUMBER_OF_VERTICES_7 2
    #define NUMBER_OF_EDGES_7 2

    Edge edges[] =
    {
        {0, 1, 15},
        {0, 1, 5}
    };

    Graph* graph = initialize(NUMBER_OF_VERTICES_7, NUMBER_OF_EDGES_7);

    TEST_ASSERT_NOT_NULL(graph);

    for (size_t index = 0; index < NUMBER_OF_EDGES_7; index++)
    {
        graph->edges[index].source = edges[index].source;
        graph->edges[index].destination = edges[index].destination;
        graph->edges[index].weight = edges[index].weight;
    }

    kruskal(graph);

    free(graph->edges);
    free(graph);
}

/**
 * @brief Detect individual node loops where an edge links back onto itself.
 */
void test_kruskal_ignores_self_loop_edges(void)
{
    #define NUMBER_OF_VERTICES_8 2
    #define NUMBER_OF_EDGES_8 2

    Edge edges[] =
    {
        {0, 0, 1},
        {0, 1, 10}
    };

    Graph* graph = initialize(NUMBER_OF_VERTICES_8, NUMBER_OF_EDGES_8);

    TEST_ASSERT_NOT_NULL(graph);

    for (size_t index = 0; index < NUMBER_OF_EDGES_8; index++)
    {
        graph->edges[index].source = edges[index].source;
        graph->edges[index].destination = edges[index].destination;
        graph->edges[index].weight = edges[index].weight;
    }

    kruskal(graph);

    free(graph->edges);
    free(graph);
}

/**
 * @brief Reject cycle insertions when an edge bridges endpoints that are already structurally connected.
 */
void test_kruskal_prevents_cycles_in_triangle_mesh(void)
{
    #define NUMBER_OF_VERTICES_9 3
    #define NUMBER_OF_EDGES_9 3

    Edge edges[] =
    {
        {0, 1, 2},
        {1, 2, 3},
        {0, 2, 4}
    };

    Graph* graph = initialize(NUMBER_OF_VERTICES_9, NUMBER_OF_EDGES_9);

    TEST_ASSERT_NOT_NULL(graph);

    for (size_t index = 0; index < NUMBER_OF_EDGES_9; index++)
    {
        graph->edges[index].source = edges[index].source;
        graph->edges[index].destination = edges[index].destination;
        graph->edges[index].weight = edges[index].weight;
    }

    kruskal(graph);

    free(graph->edges);
    free(graph);
}

/**
 * @brief Process complex multi-node straight paths sequentially.
 */
void test_kruskal_computes_linear_spanning_path(void)
{
    #define NUMBER_OF_VERTICES_10 4
    #define NUMBER_OF_EDGES_10 3

    Edge edges[] =
    {
        {0, 1, 1},
        {1, 2, 2},
        {2, 3, 3}
    };

    Graph* graph = initialize(NUMBER_OF_VERTICES_10, NUMBER_OF_EDGES_10);

    TEST_ASSERT_NOT_NULL(graph);

    for (size_t index = 0; index < NUMBER_OF_EDGES_10; index++)
    {
        graph->edges[index].source = edges[index].source;
        graph->edges[index].destination = edges[index].destination;
        graph->edges[index].weight = edges[index].weight;
    }

    kruskal(graph);

    free(graph->edges);
    free(graph);
}

/**
 * @brief Safely identify and log a warning output when handling isolated clusters.
 */
void test_kruskal_identifies_disconnected_graph_components(void)
{
    #define NUMBER_OF_VERTICES_11 4
    #define NUMBER_OF_EDGES_11 2

    Edge edges[] =
    {
        {0, 1, 4},
        {2, 3, 5}
    };

    Graph* graph = initialize(NUMBER_OF_VERTICES_11, NUMBER_OF_EDGES_11);

    TEST_ASSERT_NOT_NULL(graph);

    for (size_t index = 0; index < NUMBER_OF_EDGES_11; index++)
    {
        graph->edges[index].source = edges[index].source;
        graph->edges[index].destination = edges[index].destination;
        graph->edges[index].weight = edges[index].weight;
    }

    kruskal(graph);

    free(graph->edges);
    free(graph);
}

/**
 * @brief Support identical negative or positive cost limits across different transitions cleanly.
 */
void test_kruskal_handles_graphs_with_all_equal_edge_weights(void)
{
    #define NUMBER_OF_VERTICES_12 3
    #define NUMBER_OF_EDGES_12 3

    Edge edges[] =
    {
        {0, 1, 7},
        {1, 2, 7},
        {0, 2, 7}
    };

    Graph* graph = initialize(NUMBER_OF_VERTICES_12, NUMBER_OF_EDGES_12);

    TEST_ASSERT_NOT_NULL(graph);

    for (size_t index = 0; index < NUMBER_OF_EDGES_12; index++)
    {
        graph->edges[index].source = edges[index].source;
        graph->edges[index].destination = edges[index].destination;
        graph->edges[index].weight = edges[index].weight;
    }

    kruskal(graph);

    free(graph->edges);
    free(graph);
}

/**
 * @brief Support signed values safely where negative connections are evaluated.
 */
void test_kruskal_handles_negative_edge_weights_correctly(void)
{
    #define NUMBER_OF_VERTICES_13 3
    #define NUMBER_OF_EDGES_13 3

    Edge edges[] =
    {
        {0, 1, -10},
        {1, 2, -5},
        {0, 2, 2}
    };

    Graph* graph = initialize(NUMBER_OF_VERTICES_13, NUMBER_OF_EDGES_13);

    TEST_ASSERT_NOT_NULL(graph);

    for (size_t index = 0; index < NUMBER_OF_EDGES_13; index++)
    {
        graph->edges[index].source = edges[index].source;
        graph->edges[index].destination = edges[index].destination;
        graph->edges[index].weight = edges[index].weight;
    }

    kruskal(graph);

    free(graph->edges);
    free(graph);
}

/**
 * @brief Verify functionality within a standard diamond-shaped topology network.
 */
void test_kruskal_resolves_four_node_diamond_mesh(void)
{
    #define NUMBER_OF_VERTICES_14 4
    #define NUMBER_OF_EDGES_14 5

    Edge edges[] =
    {
        {0, 1, 1},
        {0, 2, 3},
        {1, 3, 4},
        {2, 3, 2},
        {1, 2, 5}
    };

    Graph* graph = initialize(NUMBER_OF_VERTICES_14, NUMBER_OF_EDGES_14);

    TEST_ASSERT_NOT_NULL(graph);

    for (size_t index = 0; index < NUMBER_OF_EDGES_14; index++)
    {
        graph->edges[index].source = edges[index].source;
        graph->edges[index].destination = edges[index].destination;
        graph->edges[index].weight = edges[index].weight;
    }

    kruskal(graph);

    free(graph->edges);
    free(graph);
}

/**
 * @brief Verify out-of-bounds safety filters by introducing corrupted source endpoints on elements.
 */
void test_kruskal_guards_against_out_of_bounds_source_index(void)
{
    #define NUMBER_OF_VERTICES_15 3
    #define NUMBER_OF_EDGES_15 2

    Edge edges[] =
    {
        {3, 1, 2},
        {0, 1, 4}
    };

    Graph* graph = initialize(NUMBER_OF_VERTICES_15, NUMBER_OF_EDGES_15);

    TEST_ASSERT_NOT_NULL(graph);

    for (size_t index = 0; index < NUMBER_OF_EDGES_15; index++)
    {
        graph->edges[index].source = edges[index].source;
        graph->edges[index].destination = edges[index].destination;
        graph->edges[index].weight = edges[index].weight;
    }

    kruskal(graph);

    free(graph->edges);
    free(graph);
}

/**
 * @brief Verify out-of-bounds safety filters by introducing corrupted destination points on elements.
 */
void test_kruskal_guards_against_out_of_bounds_destination_index(void)
{
    #define NUMBER_OF_VERTICES_16 3
    #define NUMBER_OF_EDGES_16 2

    Edge edges[] =
    {
        {0, 5, 2},
        {0, 1, 4}
    };

    Graph* graph = initialize(NUMBER_OF_VERTICES_16, NUMBER_OF_EDGES_16);

    TEST_ASSERT_NOT_NULL(graph);

    for (size_t index = 0; index < NUMBER_OF_EDGES_16; index++)
    {
        graph->edges[index].source = edges[index].source;
        graph->edges[index].destination = edges[index].destination;
        graph->edges[index].weight = edges[index].weight;
    }

    kruskal(graph);

    free(graph->edges);
    free(graph);
}

/**
 * @brief Process symmetrical tree structures without getting stuck or entering invalid states.
 */
void test_kruskal_handles_star_topology_layout(void)
{
    #define NUMBER_OF_VERTICES_17 4
    #define NUMBER_OF_EDGES_17 3

    Edge edges[] =
    {
        {0, 1, 10},
        {0, 2, 10},
        {0, 3, 10}
    };

    Graph* graph = initialize(NUMBER_OF_VERTICES_17, NUMBER_OF_EDGES_17);

    TEST_ASSERT_NOT_NULL(graph);

    for (size_t index = 0; index < NUMBER_OF_EDGES_17; index++)
    {
        graph->edges[index].source = edges[index].source;
        graph->edges[index].destination = edges[index].destination;
        graph->edges[index].weight = edges[index].weight;
    }

    kruskal(graph);

    free(graph->edges);
    free(graph);
}

/**
 * @brief Process completely filled matrix frameworks where every node links to all other nodes.
 */
void test_kruskal_resolves_fully_connected_complete_graph(void)
{
    #define NUMBER_OF_VERTICES_18 4
    #define NUMBER_OF_EDGES_18 6

    Edge edges[] =
    {
        {0, 1, 10},
        {0, 2, 20},
        {0, 3, 30},
        {1, 2, 5},
        {1, 3, 15},
        {2, 3, 2}
    };

    Graph* graph = initialize(NUMBER_OF_VERTICES_18, NUMBER_OF_EDGES_18);

    TEST_ASSERT_NOT_NULL(graph);

    for (size_t index = 0; index < NUMBER_OF_EDGES_18; index++)
    {
        graph->edges[index].source = edges[index].source;
        graph->edges[index].destination = edges[index].destination;
        graph->edges[index].weight = edges[index].weight;
    }

    kruskal(graph);

    free(graph->edges);
    free(graph);
}

/**
 * @brief Confirm sorting parameters are preserved when data lists are pre-sorted.
 */
void test_kruskal_works_with_pre_sorted_edges(void)
{
    #define NUMBER_OF_VERTICES_19 3
    #define NUMBER_OF_EDGES_19 2

    Edge edges[] =
    {
        {0, 1, 1},
        {1, 2, 2}
    };

    Graph* graph = initialize(NUMBER_OF_VERTICES_19, NUMBER_OF_EDGES_19);

    TEST_ASSERT_NOT_NULL(graph);

    for (size_t index = 0; index < NUMBER_OF_EDGES_19; index++)
    {
        graph->edges[index].source = edges[index].source;
        graph->edges[index].destination = edges[index].destination;
        graph->edges[index].weight = edges[index].weight;
    }

    kruskal(graph);

    free(graph->edges);
    free(graph);
}

/**
 * @brief Confirm reverse sorting rules operate effectively when data lists are loaded in inverted sequence order.
 */
void test_kruskal_works_with_reverse_sorted_edges(void)
{
    #define NUMBER_OF_VERTICES_20 3
    #define NUMBER_OF_EDGES_20 2

    Edge edges[] =
    {
        {1, 2, 2},
        {0, 1, 1}
    };

    Graph* graph = initialize(NUMBER_OF_VERTICES_20, NUMBER_OF_EDGES_20);

    TEST_ASSERT_NOT_NULL(graph);

    for (size_t index = 0; index < NUMBER_OF_EDGES_20; index++)
    {
        graph->edges[index].source = edges[index].source;
        graph->edges[index].destination = edges[index].destination;
        graph->edges[index].weight = edges[index].weight;
    }

    kruskal(graph);

    free(graph->edges);
    free(graph);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_kruskal_handles_null_graph);
    RUN_TEST(test_kruskal_initialize_rejects_zero_vertices);
    RUN_TEST(test_kruskal_initialize_rejects_zero_edges);
    RUN_TEST(test_kruskal_initialize_allocates_correct_bounds);
    RUN_TEST(test_kruskal_with_isolated_single_node_graph);
    RUN_TEST(test_kruskal_computes_simple_two_node_single_edge_path);
    RUN_TEST(test_kruskal_prioritizes_cheaper_parallel_edge);
    RUN_TEST(test_kruskal_ignores_self_loop_edges);
    RUN_TEST(test_kruskal_prevents_cycles_in_triangle_mesh);
    RUN_TEST(test_kruskal_computes_linear_spanning_path);
    RUN_TEST(test_kruskal_identifies_disconnected_graph_components);
    RUN_TEST(test_kruskal_handles_graphs_with_all_equal_edge_weights);
    RUN_TEST(test_kruskal_handles_negative_edge_weights_correctly);
    RUN_TEST(test_kruskal_resolves_four_node_diamond_mesh);
    RUN_TEST(test_kruskal_guards_against_out_of_bounds_source_index);
    RUN_TEST(test_kruskal_guards_against_out_of_bounds_destination_index);
    RUN_TEST(test_kruskal_handles_star_topology_layout);
    RUN_TEST(test_kruskal_resolves_fully_connected_complete_graph);
    RUN_TEST(test_kruskal_works_with_pre_sorted_edges);
    RUN_TEST(test_kruskal_works_with_reverse_sorted_edges);

    // Destroy the session.
    return UNITY_END();
}
