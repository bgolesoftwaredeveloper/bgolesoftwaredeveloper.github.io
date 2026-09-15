/**
 * @file        :   dijkstra_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit test suite for Dijkstra's shortest path algorithm.
 * @details     :   Implements 20 automated assertion checks using the Unity 
 * Test Framework. Covers structural edge cases, null reference tracking, 
 * path relaxation correctness, and mathematical overflow verification.
 * Compile:
 * clang ./tests/darwin/graph/dijkstra_unit_tests.c ./src/darwin/graph/dijkstra/dijkstra.c ./src/unity.c -Iinclude -o ./bin/dijkstra_unit_test
 * @version     :   1.0
 * @date        :   2026-05-24
 */
#include <stdio.h>
#include <stdlib.h>

#include "unity.h"
#include "darwin/graph/dijkstra.h"

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
 * @brief Verifies a standard normal 3-vertex linear chain.
 */
void test_dijkstra_standard_linear_graph(void)
{
    // Arrange.
    int adjacency_matrix[9] = {
        0, 5, 0,
        0, 0, 3,
        0, 0, 0
    };
    
    int shortest_distances[3] = {0};
    bool visited_vertices[3] = {false};

    Graph graph_context = {
        .adjacency_matrix = adjacency_matrix,
        .total_vertices = 3,
        .shortest_distances = shortest_distances,
        .visited = visited_vertices
    };

    size_t source_vertex = 0;

    // Act.
    bool execution_success = dijkstra(&graph_context, source_vertex);

    // Assert.
    TEST_ASSERT_TRUE(execution_success);
    TEST_ASSERT_EQUAL_INT(0, graph_context.shortest_distances[0]);
    TEST_ASSERT_EQUAL_INT(5, graph_context.shortest_distances[1]);
    TEST_ASSERT_EQUAL_INT(8, graph_context.shortest_distances[2]);
}


/**
 * @brief Verifies behavior when the context pointer itself is NULL.
 */
void test_dijkstra_null_context(void)
{
    // Arrange.
    Graph* null_graph_context = NULL;

    size_t source_vertex = 0;

    // Act.
    bool execution_success = dijkstra(null_graph_context, source_vertex);

    // Assert.
    TEST_ASSERT_FALSE(execution_success);
}


/**
 * @brief Verifies handling of an unallocated internal adjacency matrix pointer.
 */
void test_dijkstra_null_adjacency_matrix(void)
{
    // Arrange.
    int shortest_distances[1] = {0};
    bool visited_vertices[1] = {false};

    Graph graph_context = {
        .adjacency_matrix = NULL,
        .total_vertices = 1,
        .shortest_distances = shortest_distances,
        .visited = visited_vertices
    };

    size_t source_vertex = 0;

    // Act.
    bool execution_success = dijkstra(&graph_context, source_vertex);

    // Assert.
    TEST_ASSERT_FALSE(execution_success);
}


/**
 * @brief Verifies handling of an unallocated shortest distances vector pointer.
 */
void test_dijkstra_null_shortest_distances_array(void)
{
    // Arrange.
    int adjacency_matrix[1] = {0};
    bool visited_vertices[1] = {false};

    Graph graph_context = {
        .adjacency_matrix = adjacency_matrix,
        .total_vertices = 1,
        .shortest_distances = NULL,
        .visited = visited_vertices
    };

    size_t source_vertex = 0;

    // Act.
    bool execution_success = dijkstra(&graph_context, source_vertex);

    // Assert.
    TEST_ASSERT_FALSE(execution_success);
}


/**
 * @brief Verifies handling of an unallocated visited state array pointer.
 */
void test_dijkstra_null_visited_array(void)
{
    // Arrange.
    int adjacency_matrix[1] = {0};
    int shortest_distances[1] = {0};

    Graph graph_context = {
        .adjacency_matrix = adjacency_matrix,
        .total_vertices = 1,
        .shortest_distances = shortest_distances,
        .visited = NULL
    };

    size_t source_vertex = 0;

    // Act.
    bool execution_success = dijkstra(&graph_context, source_vertex);

    // Assert.
    TEST_ASSERT_FALSE(execution_success);
}


/**
 * @brief Verifies failure condition when total vertices count parameter is 0.
 */
void test_dijkstra_zero_vertices(void)
{
    // Arrange.
    int adjacency_matrix[1] = {0};

    int shortest_distances[1] = {0};
    bool visited_vertices[1] = {false};

    Graph graph_context = {
        .adjacency_matrix = adjacency_matrix,
        .total_vertices = 0,
        .shortest_distances = shortest_distances,
        .visited = visited_vertices
    };

    size_t source_vertex = 0;

    // Act.
    bool execution_success = dijkstra(&graph_context, source_vertex);

    // Assert.
    TEST_ASSERT_FALSE(execution_success);
}


/**
 * @brief Verifies error mitigation when the starting source index is out of structural array bounds.
 */
void test_dijkstra_source_index_out_of_bounds(void)
{
    // Arrange.
    int adjacency_matrix[4] = {0, 1, 1, 0};

    int shortest_distances[2] = {0};
    bool visited_vertices[2] = {false};

    Graph graph_context = {
        .adjacency_matrix = adjacency_matrix,
        .total_vertices = 2,
        .shortest_distances = shortest_distances,
        .visited = visited_vertices
    };

    size_t out_of_bounds_source_vertex = 5;

    // Act.
    bool execution_success = dijkstra(&graph_context, out_of_bounds_source_vertex);

    // Assert.
    TEST_ASSERT_FALSE(execution_success);
}


/**
 * @brief Verifies safety intercept when a negative edge weight is located inside the matrix configuration.
 */
void test_dijkstra_negative_edge_weight_detection(void)
{
    // Arrange.
    int adjacency_matrix[4] = {
        0, -2,
        0,  0
    };

    int shortest_distances[2] = {0};
    bool visited_vertices[2] = {false};

    Graph graph_context = {
        .adjacency_matrix = adjacency_matrix,
        .total_vertices = 2,
        .shortest_distances = shortest_distances,
        .visited = visited_vertices
    };

    size_t source_vertex = 0;

    // Act.
    bool execution_success = dijkstra(&graph_context, source_vertex);

    // Assert.
    TEST_ASSERT_FALSE(execution_success);
}


/**
 * @brief Verifies single isolated point graph initialization bounds.
 */
void test_dijkstra_single_isolated_vertex(void)
{
    // Arrange.
    int adjacency_matrix[1] = {0};
    int shortest_distances[1] = {0};

    bool visited_vertices[1] = {false};

    Graph graph_context = {
        .adjacency_matrix = adjacency_matrix,
        .total_vertices = 1,
        .shortest_distances = shortest_distances,
        .visited = visited_vertices
    };

    size_t source_vertex = 0;

    // Act.
    bool execution_success = dijkstra(&graph_context, source_vertex);

    // Assert.
    TEST_ASSERT_TRUE(execution_success);
    TEST_ASSERT_EQUAL_INT(0, graph_context.shortest_distances[0]);
}


/**
 * @brief Verifies unreachable target nodes record a constant value of INT_MAX.
 */
void test_dijkstra_unreachable_node_returns_infinity(void)
{
    // Arrange.
    int adjacency_matrix[4] = {
        0, 0,
        0, 0
    };

    int shortest_distances[2] = {0};
    bool visited_vertices[2] = {false};

    Graph graph_context = {
        .adjacency_matrix = adjacency_matrix,
        .total_vertices = 2,
        .shortest_distances = shortest_distances,
        .visited = visited_vertices
    };

    size_t source_vertex = 0;

    // Act.
    bool execution_success = dijkstra(&graph_context, source_vertex);

    // Assert.
    TEST_ASSERT_TRUE(execution_success);
    TEST_ASSERT_EQUAL_INT(0, graph_context.shortest_distances[0]);
    TEST_ASSERT_EQUAL_INT(INT_MAX, graph_context.shortest_distances[1]);
}


/**
 * @brief Verifies optimization select tracks the absolute minimal cost when multiple graph options exist.
 */
void test_dijkstra_selects_minimum_cost_path_among_multiple(void)
{
    // Arrange.
    int adjacency_matrix[9] = {
        0, 10,  1,
        0,  0,  0,
        0,  2,  0
    };

    int shortest_distances[3] = {0};
    bool visited_vertices[3] = {false};

    Graph graph_context = {
        .adjacency_matrix = adjacency_matrix,
        .total_vertices = 3,
        .shortest_distances = shortest_distances,
        .visited = visited_vertices
    };

    size_t source_vertex = 0;

    // Act.
    bool execution_success = dijkstra(&graph_context, source_vertex);

    // Assert.
    TEST_ASSERT_TRUE(execution_success);
    TEST_ASSERT_EQUAL_INT(0, graph_context.shortest_distances[0]);
    TEST_ASSERT_EQUAL_INT(3, graph_context.shortest_distances[1]);
    TEST_ASSERT_EQUAL_INT(1, graph_context.shortest_distances[2]);
}


/**
 * @brief Verifies overflow validation stops path expansion when edge calculation threatens storage layout metrics.
 */
void test_dijkstra_integer_overflow_protection(void)
{
    // Arrange.
    int adjacency_matrix[4] = {
        0, INT_MAX,
        0, 0
    };

    int shortest_distances[2] = {0};
    bool visited_vertices[2] = {false};

    Graph graph_context = {
        .adjacency_matrix = adjacency_matrix,
        .total_vertices = 2,
        .shortest_distances = shortest_distances,
        .visited = visited_vertices
    };

    size_t source_vertex = 0;

    // Act.
    bool execution_success = dijkstra(&graph_context, source_vertex);

    // Assert.
    TEST_ASSERT_TRUE(execution_success);
    TEST_ASSERT_EQUAL_INT(INT_MAX, graph_context.shortest_distances[1]);
}


/**
 * @brief Verifies computation logic handles cyclic connection setups safely without hang loops.
 */
void test_dijkstra_cyclic_graph_termination(void)
{
    // Arrange.
    int adjacency_matrix[9] = {
        0, 4, 0,
        0, 0, 2,
        1, 0, 0
    };

    int shortest_distances[3] = {0};
    bool visited_vertices[3] = {false};

    Graph graph_context = {
        .adjacency_matrix = adjacency_matrix,
        .total_vertices = 3,
        .shortest_distances = shortest_distances,
        .visited = visited_vertices
    };

    size_t source_vertex = 0;

    // Act.
    bool execution_success = dijkstra(&graph_context, source_vertex);

    // Assert.
    TEST_ASSERT_TRUE(execution_success);
    TEST_ASSERT_EQUAL_INT(0, graph_context.shortest_distances[0]);
    TEST_ASSERT_EQUAL_INT(4, graph_context.shortest_distances[1]);
    TEST_ASSERT_EQUAL_INT(6, graph_context.shortest_distances[2]);
}


/**
 * @brief Verifies proper state translation on structural 4-vertex fully interconnected mesh topology profiles.
 */
void test_dijkstra_fully_connected_mesh_graph(void)
{
    // Arrange.
    int adjacency_matrix[16] = {
        0, 2, 4, 6,
        2, 0, 1, 5,
        4, 1, 0, 1,
        6, 5, 1, 0
    };

    int shortest_distances[4] = {0};
    bool visited_vertices[4] = {false};

    Graph graph_context = {
        .adjacency_matrix = adjacency_matrix,
        .total_vertices = 4,
        .shortest_distances = shortest_distances,
        .visited = visited_vertices
    };

    size_t source_vertex = 0;

    // Act.
    bool execution_success = dijkstra(&graph_context, source_vertex);

    // Assert.
    TEST_ASSERT_TRUE(execution_success);
    TEST_ASSERT_EQUAL_INT(0, graph_context.shortest_distances[0]);
    TEST_ASSERT_EQUAL_INT(2, graph_context.shortest_distances[1]);
    TEST_ASSERT_EQUAL_INT(3, graph_context.shortest_distances[2]);
    TEST_ASSERT_EQUAL_INT(4, graph_context.shortest_distances[3]);
}


/**
 * @brief Verifies execution capability from the absolute terminal boundary index location.
 */
void test_dijkstra_execution_from_last_vertex_index(void)
{
    // Arrange.
    int adjacency_matrix[9] = {
        0, 0, 0,
        3, 0, 0,
        2, 4, 0
    };

    int shortest_distances[3] = {0};
    bool visited_vertices[3] = {false};

    Graph graph_context = {
        .adjacency_matrix = adjacency_matrix,
        .total_vertices = 3,
        .shortest_distances = shortest_distances,
        .visited = visited_vertices
    };

    size_t source_vertex = 2;

    // Act.
    bool execution_success = dijkstra(&graph_context, source_vertex);

    // Assert.
    TEST_ASSERT_TRUE(execution_success);
    TEST_ASSERT_EQUAL_INT(2, graph_context.shortest_distances[0]);
    TEST_ASSERT_EQUAL_INT(4, graph_context.shortest_distances[1]);
    TEST_ASSERT_EQUAL_INT(0, graph_context.shortest_distances[2]);
}


/**
 * @brief Verifies handling of large edge weights that sum right up to the maximum border limit.
 */
void test_dijkstra_near_overflow_valid_accumulation(void)
{
    // Arrange.
    int adjacency_matrix[4] = {
        0, (INT_MAX - 10),
        0, 0
    };

    int shortest_distances[2] = {0};
    bool visited_vertices[2] = {false};

    Graph graph_context = {
        .adjacency_matrix = adjacency_matrix,
        .total_vertices = 2,
        .shortest_distances = shortest_distances,
        .visited = visited_vertices
    };

    size_t source_vertex = 0;

    // Act.
    bool execution_success = dijkstra(&graph_context, source_vertex);

    // Assert.
    TEST_ASSERT_TRUE(execution_success);
    TEST_ASSERT_EQUAL_INT((INT_MAX - 10), graph_context.shortest_distances[1]);
}


/**
 * @brief Verifies processing flow against a graph structured purely as a backwards unidirectional track configuration.
 */
void test_dijkstra_reverse_directional_pathway(void)
{
    // Arrange.
    int adjacency_matrix[9] = {
        0, 0, 0,
        8, 0, 0,
        0, 4, 0
    };

    int shortest_distances[3] = {0};
    bool visited_vertices[3] = {false};

    Graph graph_context = {
        .adjacency_matrix = adjacency_matrix,
        .total_vertices = 3,
        .shortest_distances = shortest_distances,
        .visited = visited_vertices
    };

    size_t source_vertex = 0;

    // Act.
    bool execution_success = dijkstra(&graph_context, source_vertex);

    // Assert.
    TEST_ASSERT_TRUE(execution_success);
    TEST_ASSERT_EQUAL_INT(0, graph_context.shortest_distances[0]);
    TEST_ASSERT_EQUAL_INT(INT_MAX, graph_context.shortest_distances[1]);
    TEST_ASSERT_EQUAL_INT(INT_MAX, graph_context.shortest_distances[2]);
}


/**
 * @brief Verifies structural optimization layers function precisely over highly disconnected graph component layouts.
 */
void test_dijkstra_disconnected_subgraphs(void)
{
    // Arrange.
    int adjacency_matrix[16] = {
        0, 3, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 6,
        0, 0, 0, 0
    };

    int shortest_distances[4] = {0};
    bool visited_vertices[4] = {false};

    Graph graph_context = {
        .adjacency_matrix = adjacency_matrix,
        .total_vertices = 4,
        .shortest_distances = shortest_distances,
        .visited = visited_vertices
    };

    size_t source_vertex = 0;

    // Act.
    bool execution_success = dijkstra(&graph_context, source_vertex);

    // Assert.
    TEST_ASSERT_TRUE(execution_success);
    TEST_ASSERT_EQUAL_INT(0, graph_context.shortest_distances[0]);
    TEST_ASSERT_EQUAL_INT(3, graph_context.shortest_distances[1]);
    TEST_ASSERT_EQUAL_INT(INT_MAX, graph_context.shortest_distances[2]);
    TEST_ASSERT_EQUAL_INT(INT_MAX, graph_context.shortest_distances[3]);
}


/**
 * @brief Verifies graph setups containing multiple equivalent alternate minimal cost vectors resolve safely.
 */
void test_dijkstra_duplicate_weight_paths_resolution(void)
{
    // Arrange.
    int adjacency_matrix[16] = {
        0, 5, 5, 0,
        0, 0, 0, 1,
        0, 0, 0, 1,
        0, 0, 0, 0
    };

    int shortest_distances[4] = {0};
    bool visited_vertices[4] = {false};

    Graph graph_context = {
        .adjacency_matrix = adjacency_matrix,
        .total_vertices = 4,
        .shortest_distances = shortest_distances,
        .visited = visited_vertices
    };

    size_t source_vertex = 0;

    // Act.
    bool execution_success = dijkstra(&graph_context, source_vertex);

    // Assert.
    TEST_ASSERT_TRUE(execution_success);
    TEST_ASSERT_EQUAL_INT(6, graph_context.shortest_distances[3]);
}


/**
 * @brief Verifies execution capability over a dense 5-vertex structure profile layout mapping.
 */
void test_dijkstra_dense_five_vertex_matrix(void)
{
    // Arrange.
    int adjacency_matrix[25] = {
        0,  2,  9,  0,  0,
        0,  0,  4,  3,  0,
        0,  0,  0,  0,  1,
        0,  0,  2,  0,  7,
        0,  0,  0,  0,  0
    };

    int shortest_distances[5] = {0};
    bool visited_vertices[5] = {false};

    Graph graph_context = {
        .adjacency_matrix = adjacency_matrix,
        .total_vertices = 5,
        .shortest_distances = shortest_distances,
        .visited = visited_vertices
    };

    size_t source_vertex = 0;

    // Act.
    bool execution_success = dijkstra(&graph_context, source_vertex);

    // Assert.
    TEST_ASSERT_TRUE(execution_success);
    TEST_ASSERT_EQUAL_INT(0, graph_context.shortest_distances[0]);
    TEST_ASSERT_EQUAL_INT(2, graph_context.shortest_distances[1]);
    TEST_ASSERT_EQUAL_INT(6, graph_context.shortest_distances[2]);
    TEST_ASSERT_EQUAL_INT(5, graph_context.shortest_distances[3]);
    TEST_ASSERT_EQUAL_INT(7, graph_context.shortest_distances[4]);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_dijkstra_standard_linear_graph);
    RUN_TEST(test_dijkstra_null_context);
    RUN_TEST(test_dijkstra_null_adjacency_matrix);
    RUN_TEST(test_dijkstra_null_shortest_distances_array);
    RUN_TEST(test_dijkstra_null_visited_array);
    RUN_TEST(test_dijkstra_zero_vertices);
    RUN_TEST(test_dijkstra_source_index_out_of_bounds);
    RUN_TEST(test_dijkstra_negative_edge_weight_detection);
    RUN_TEST(test_dijkstra_single_isolated_vertex);
    RUN_TEST(test_dijkstra_unreachable_node_returns_infinity);
    RUN_TEST(test_dijkstra_selects_minimum_cost_path_among_multiple);
    RUN_TEST(test_dijkstra_integer_overflow_protection);
    RUN_TEST(test_dijkstra_cyclic_graph_termination);
    RUN_TEST(test_dijkstra_fully_connected_mesh_graph);
    RUN_TEST(test_dijkstra_execution_from_last_vertex_index);
    RUN_TEST(test_dijkstra_near_overflow_valid_accumulation);
    RUN_TEST(test_dijkstra_reverse_directional_pathway);
    RUN_TEST(test_dijkstra_disconnected_subgraphs);
    RUN_TEST(test_dijkstra_duplicate_weight_paths_resolution);
    RUN_TEST(test_dijkstra_dense_five_vertex_matrix);

    // Destroy the session.
    return UNITY_END();
}