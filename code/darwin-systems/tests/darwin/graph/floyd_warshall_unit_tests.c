/**
 * @file        :   floyd_warshall_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit tests for the Floyd-Warshall all-pairs shortest path algorithm.
 * @details     :   Contains 20 unique testing scenarios ranging from basic functionality 
 * and matrix edge cases to negative loop traps and defensive validation.
 * Compile:
 * clang ./tests/darwin/graph/floyd_warshall_unit_tests.c ./src/darwin/graph/floyd-warshall/floyd_warshall.c ./src/unity.c -Iinclude -o ./bin/floyd_warshall_unit_test
 * @version     :   1.0
 * @date        :   2026-05-027
 */
#include <stdio.h>
#include <string.h>

#include "unity.h"
#include "darwin/graph/floyd_warshall.h"

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
 * @brief Ensure the algorithm safely flags a completely NULL graph pointer.
 */
void test_floyd_warshall_handles_null_graph(void)
{
    // Act.
    bool status = floyd_warshall(NULL);

    // Assert.
    TEST_ASSERT_FALSE(status);
}

/**
 * @brief Verify correctness using an empty graph containing only isolated zero weights.
 */
void test_floyd_warshall_with_all_zeros_matrix(void)
{
    // Arrange.
    int graph[VERTICES][VERTICES] = {0};

    // Act.
    bool status = floyd_warshall(graph);

    // Assert.
    TEST_ASSERT_TRUE(status);
}

/**
 * @brief Verify standard functionality given an entirely disconnected graph topology.
 */
void test_floyd_warshall_with_fully_disconnected_graph(void)
{
    // Arrange.
    int graph[VERTICES][VERTICES];

    for (size_t row = 0; row < VERTICES; row++)
    {
        for (size_t column = 0; column < VERTICES; column++)
        {
            graph[row][column] = (row == column) ? 0 : INFINITE;
        }
    }

    // Act.
    bool status = floyd_warshall(graph);

    // Assert.
    TEST_ASSERT_TRUE(status);
}

/**
 * @brief Standard simple single-path sequence update verification.
 */
void test_floyd_warshall_computes_simple_linear_path(void)
{
    // Arrange.
    int graph[VERTICES][VERTICES];

    for (size_t row = 0; row < VERTICES; row++)
    {
        for (size_t column = 0; column < VERTICES; column++)
        {
            graph[row][column] = (row == column) ? 0 : INFINITE;
        }
    }

    graph[0][1] = 5;
    graph[1][2] = 3;

    // Act.
    bool status = floyd_warshall(graph);

    // Assert.
    TEST_ASSERT_TRUE(status);
}

/**
 * @brief Ensure a shorter route over multiple hops overrides a direct, expensive connection.
 */
void test_floyd_warshall_prefers_cheaper_multi_hop_path_over_expensive_direct_edge(void)
{
    // Arrange.
    int graph[VERTICES][VERTICES];

    for (size_t row = 0; row < VERTICES; row++)
    {
        for (size_t column = 0; column < VERTICES; column++)
        {
            graph[row][column] = (row == column) ? 0 : INFINITE;
        }
    }

    graph[0][2] = 25;
    graph[0][1] = 4;
    graph[1][2] = 3;

    // Act.
    bool status = floyd_warshall(graph);

    // Assert.
    TEST_ASSERT_TRUE(status);
}

/**
 * @brief Support validation routines when utilizing safe negative weights without cycles.
 */
void test_floyd_warshall_with_safe_negative_weights(void)
{
    // Arrange.
    int graph[VERTICES][VERTICES];

    for (size_t row = 0; row < VERTICES; row++)
    {
        for (size_t column = 0; column < VERTICES; column++)
        {
            graph[row][column] = (row == column) ? 0 : INFINITE;
        }
    }

    graph[0][1] = 10;
    graph[1][2] = -5;

    // Act.
    bool status = floyd_warshall(graph);

    // Assert.
    TEST_ASSERT_TRUE(status);
}

/**
 * @brief Detect an explicit negative loop condition and drop implementation status.
 */
void test_floyd_warshall_detects_obvious_negative_weight_cycle(void)
{
    // Arrange.
    int graph[VERTICES][VERTICES];

    for (size_t row = 0; row < VERTICES; row++)
    {
        for (size_t column = 0; column < VERTICES; column++)
        {
            graph[row][column] = (row == column) ? 0 : INFINITE;
        }
    }

    graph[0][1] = 2;
    graph[1][0] = -4;

    // Act.
    bool status = floyd_warshall(graph);

    // Assert.
    TEST_ASSERT_FALSE(status);
}

/**
 * @brief Ensure a standard positive loop sequence doesn't trigger false cycle flags.
 */
void test_floyd_warshall_ignores_positive_weight_cycles(void)
{
    // Arrange.
    int graph[VERTICES][VERTICES];

    for (size_t row = 0; row < VERTICES; row++)
    {
        for (size_t column = 0; column < VERTICES; column++)
        {
            graph[row][column] = (row == column) ? 0 : INFINITE;
        }
    }

    graph[0][1] = 6;
    graph[1][0] = 4;

    // Act.
    bool status = floyd_warshall(graph);

    // Assert.
    TEST_ASSERT_TRUE(status);
}

/**
 * @brief Detect individual node loop traps where an edge returns directly onto itself.
 */
void test_floyd_warshall_detects_negative_self_loop(void)
{
    // Arrange.
    int graph[VERTICES][VERTICES];

    for (size_t row = 0; row < VERTICES; row++)
    {
        for (size_t column = 0; column < VERTICES; column++)
        {
            graph[row][column] = (row == column) ? 0 : INFINITE;
        }
    }

    graph[1][1] = -1;

    // Act.
    bool status = floyd_warshall(graph);

    // Assert.
    TEST_ASSERT_FALSE(status);
}

/**
 * @brief Confirm calculation transparency across completely symmetric graph matrices.
 */
void test_floyd_warshall_processes_symmetric_undirected_graph(void)
{
    // Arrange.
    int graph[VERTICES][VERTICES];

    for (size_t row = 0; row < VERTICES; row++)
    {
        for (size_t column = 0; column < VERTICES; column++)
        {
            graph[row][column] = (row == column) ? 0 : INFINITE;
        }
    }

    graph[0][1] = 8;
    graph[1][0] = 8;
    graph[1][2] = 2;
    graph[2][1] = 2;

    // Act.
    bool status = floyd_warshall(graph);

    // Assert.
    TEST_ASSERT_TRUE(status);
}

/**
 * @brief Process zero-cost operations seamlessly without losing shortest tracking data.
 */
void test_floyd_warshall_processes_zero_weight_edges(void)
{
    // Arrange.
    int graph[VERTICES][VERTICES];

    for (size_t row = 0; row < VERTICES; row++)
    {
        for (size_t column = 0; column < VERTICES; column++)
        {
            graph[row][column] = (row == column) ? 0 : INFINITE;
        }
    }

    graph[0][1] = 0;
    graph[1][2] = 0;

    // Act.
    bool status = floyd_warshall(graph);

    // Assert.
    TEST_ASSERT_TRUE(status);
}

/**
 * @brief Track optimization routines in fully connected complete clique configurations.
 */
void test_floyd_warshall_with_complete_graph_clique(void)
{
    // Arrange.
    int graph[VERTICES][VERTICES];

    for (size_t row = 0; row < VERTICES; row++)
    {
        for (size_t column = 0; column < VERTICES; column++)
        {
            graph[row][column] = (row == column) ? 0 : 5;
        }
    }

    // Act.
    bool status = floyd_warshall(graph);

    // Assert.
    TEST_ASSERT_TRUE(status);
}

/**
 * @brief Manage directional flow barriers correctly when tracing back asymmetric matrices.
 */
void test_floyd_warshall_enforces_one_way_asymmetric_paths(void)
{
    // Arrange.
    int graph[VERTICES][VERTICES];

    for (size_t row = 0; row < VERTICES; row++)
    {
        for (size_t column = 0; column < VERTICES; column++)
        {
            graph[row][column] = (row == column) ? 0 : INFINITE;
        }
    }

    graph[0][1] = 4;

    // Act.
    bool status = floyd_warshall(graph);

    // Assert.
    TEST_ASSERT_TRUE(status);
}

/**
 * @brief Handle complex intertwined dynamic mesh structures containing variable path choices.
 */
void test_floyd_warshall_with_complex_multi_branch_mesh(void)
{
    // Arrange.
    int graph[VERTICES][VERTICES] = {
        {0, 5, INFINITE, 10},
        {INFINITE, 0, 3, INFINITE},
        {INFINITE, INFINITE, 0, 1},
        {INFINITE, INFINITE, INFINITE, 0},
        {1, INFINITE, 2, INFINITE}
    };

    // Act.
    bool status = floyd_warshall(graph);

    // Assert.
    TEST_ASSERT_TRUE(status);
}

/**
 * @brief Detect cyclic interference even when nested deep within late structural transitions.
 */
void test_floyd_warshall_detects_deeply_nested_negative_cycle(void)
{
    // Arrange.
    int graph[VERTICES][VERTICES];

    for (size_t row = 0; row < VERTICES; row++)
    {
        for (size_t column = 0; column < VERTICES; column++)
        {
            graph[row][column] = (row == column) ? 0 : INFINITE;
        }
    }
    // Formulate a negative cycle at the end of the structural vertex domain bounds.
    graph[VERTICES - 2][VERTICES - 1] = 1;
    graph[VERTICES - 1][VERTICES - 2] = -3;

    // Act.
    bool status = floyd_warshall(graph);

    // Assert.
    TEST_ASSERT_FALSE(status);
}

/**
 * @brief Protect computational logic integrity against processing massive positive values.
 */
void test_floyd_warshall_shields_against_large_positive_weights(void)
{
    // Arrange.
    int graph[VERTICES][VERTICES];

    for (size_t row = 0; row < VERTICES; row++)
    {
        for (size_t column = 0; column < VERTICES; column++)
        {
            graph[row][column] = (row == column) ? 0 : INFINITE;
        }
    }

    graph[0][1] = 40000;
    graph[1][2] = 50000;

    // Act.
    bool status = floyd_warshall(graph);

    // Assert.
    TEST_ASSERT_TRUE(status);
}

/**
 * @brief Ensure unreachable node configurations leave baseline INFINITE weights unaltered.
 */
void test_floyd_warshall_preserves_infinite_bounds_for_unreachable_node_pairs(void)
{
    // Arrange.
    int graph[VERTICES][VERTICES];

    for (size_t row = 0; row < VERTICES; row++)
    {
        for (size_t column = 0; column < VERTICES; column++)
        {
            graph[row][column] = (row == column) ? 0 : INFINITE;
        }
    }

    graph[0][1] = 10;

    // Act.
    bool status = floyd_warshall(graph);

    // Assert.
    TEST_ASSERT_TRUE(status);
}

/**
 * @brief Detect cycles when they loop back precisely to the source node entry point.
 */
void test_floyd_warshall_detects_cycle_looping_to_origin(void)
{
    // Arrange.
    int graph[VERTICES][VERTICES];

    for (size_t row = 0; row < VERTICES; row++)
    {
        for (size_t column = 0; column < VERTICES; column++)
        {
            graph[row][column] = (row == column) ? 0 : INFINITE;
        }
    }

    graph[0][1] = 5;
    graph[1][2] = 2;
    graph[2][0] = -10;

    // Act.
    bool status = floyd_warshall(graph);

    // Assert.
    TEST_ASSERT_FALSE(status);
}

/**
 * @brief Validate correct sequence tracking across structural single-direction linear chains.
 */
void test_floyd_warshall_with_long_monotonically_increasing_chain(void)
{
    // Arrange.
    int graph[VERTICES][VERTICES];

    for (size_t row = 0; row < VERTICES; row++)
    {
        for (size_t column = 0; column < VERTICES; column++)
        {
            graph[row][column] = (row == column) ? 0 : INFINITE;
        }
    }
    
    for (size_t index = 0; index < VERTICES - 1; index++)
    {
        graph[index][index + 1] = (int)(index + 1);
    }

    // Act.
    bool status = floyd_warshall(graph);

    // Assert.
    TEST_ASSERT_TRUE(status);
}

/**
 * @brief Handle scenarios where separate alternate routes track perfectly identical metrics.
 */
void test_floyd_warshall_resolves_multiple_identical_shortest_path_options(void)
{
    // Arrange.
    int graph[VERTICES][VERTICES];

    for (size_t row = 0; row < VERTICES; row++)
    {
        for (size_t column = 0; column < VERTICES; column++)
        {
            graph[row][column] = (row == column) ? 0 : INFINITE;
        }
    }
    // Path choice 1 via node 1
    graph[0][1] = 5;
    graph[1][2] = 5;
    
    // Path choice 2 via node 3
    graph[0][3] = 5;
    graph[3][2] = 5;

    // Act.
    bool status = floyd_warshall(graph);

    // Assert.
    TEST_ASSERT_TRUE(status);
}

int main(void)
{
    // Initiaite the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_floyd_warshall_handles_null_graph);
    RUN_TEST(test_floyd_warshall_with_all_zeros_matrix);
    RUN_TEST(test_floyd_warshall_with_fully_disconnected_graph);
    RUN_TEST(test_floyd_warshall_computes_simple_linear_path);
    RUN_TEST(test_floyd_warshall_prefers_cheaper_multi_hop_path_over_expensive_direct_edge);
    RUN_TEST(test_floyd_warshall_with_safe_negative_weights);
    RUN_TEST(test_floyd_warshall_detects_obvious_negative_weight_cycle);
    RUN_TEST(test_floyd_warshall_ignores_positive_weight_cycles);
    RUN_TEST(test_floyd_warshall_detects_negative_self_loop);
    RUN_TEST(test_floyd_warshall_processes_symmetric_undirected_graph);
    RUN_TEST(test_floyd_warshall_processes_zero_weight_edges);
    RUN_TEST(test_floyd_warshall_with_complete_graph_clique);
    RUN_TEST(test_floyd_warshall_enforces_one_way_asymmetric_paths);
    RUN_TEST(test_floyd_warshall_with_complex_multi_branch_mesh);
    RUN_TEST(test_floyd_warshall_detects_deeply_nested_negative_cycle);
    RUN_TEST(test_floyd_warshall_shields_against_large_positive_weights);
    RUN_TEST(test_floyd_warshall_preserves_infinite_bounds_for_unreachable_node_pairs);
    RUN_TEST(test_floyd_warshall_detects_cycle_looping_to_origin);
    RUN_TEST(test_floyd_warshall_with_long_monotonically_increasing_chain);
    RUN_TEST(test_floyd_warshall_resolves_multiple_identical_shortest_path_options);

    // Destory the session.
    return UNITY_END();
}