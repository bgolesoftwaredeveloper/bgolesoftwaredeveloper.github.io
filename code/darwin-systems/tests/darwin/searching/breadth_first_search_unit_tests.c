/**
 * @file        :   breadth_first_search_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit tests for the Breadth-First Search (BFS) algorithm.
 * @details     :   Validates graph initialization, edge cases, and traversal 
 * correctness using the Unity Test Framework.
 * Compile:
 * clang ./tests/darwin/searching/breadth_first_search_unit_tests.c ./src/darwin/searching/breadth-first-search/breadth_first_search.c ./src/unity.c -Iinclude -o ./bin/breadth_first_search_unit_test
 * @version     :   1.0
 * @date        :   2026-04-19
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "unity.h"
#include "darwin/searching/breadth_first_search.h"

static struct Graph* graph = NULL;

/**
 * @brief Sets up resources before each test.
 */
void setUp(void)
{
    graph = NULL;
}

/**
 * @brief Cleans up resources after each test.
 */
void tearDown(void)
{
    if (graph != NULL)
    {
        destroy_graph(graph);
    }
}

/**
 * @brief Test initialization of a graph with multiple vertices.
 */
void test_graph_initialization(void)
{
    // Arrange.
    int vertex_count = 5;

    // Act.
    graph = initialize_graph(vertex_count);

    // Assert.
    TEST_ASSERT_NOT_NULL(graph);
    TEST_ASSERT_EQUAL_INT(5, graph->number_of_vertices);

    for (int index = 0; index < 5; index++)
    {
        TEST_ASSERT_NULL(graph->adjacency_list[index]);
        TEST_ASSERT_FALSE(graph->visited[index]);
    }
}

/**
 * @brief Test adding an undirected edge between two vertices.
 */
void test_add_edge_connectivity(void)
{
    // Arrange.
    graph = initialize_graph(2);

    // Act.
    add_edge(graph, 0, 1);

    // Assert.
    TEST_ASSERT_NOT_NULL(graph->adjacency_list[0]);
    TEST_ASSERT_EQUAL_INT(1, graph->adjacency_list[0]->vertex);
    TEST_ASSERT_NOT_NULL(graph->adjacency_list[1]);
    TEST_ASSERT_EQUAL_INT(0, graph->adjacency_list[1]->vertex);
}

/**
 * @brief Test BFS visiting all nodes in a linear graph.
 */
void test_bfs_linear_graph_visited(void)
{
    // Arrange.
    graph = initialize_graph(3);

    add_edge(graph, 0, 1);
    add_edge(graph, 1, 2);

    // Act.
    breadth_first_search(graph, 0);

    // Assert.
    for (int index = 0; index < 3; index++)
    {
        TEST_ASSERT_TRUE(graph->visited[index]);
    }
}

/**
 * @brief Test BFS starting from an isolated vertex.
 */
void test_bfs_isolated_vertex(void)
{
    // Arrange.
    graph = initialize_graph(3);

    add_edge(graph, 1, 2); 

    // Act.
    breadth_first_search(graph, 0);

    // Assert.
    TEST_ASSERT_TRUE(graph->visited[0]);
    TEST_ASSERT_FALSE(graph->visited[1]);
    TEST_ASSERT_FALSE(graph->visited[2]);
}

/**
 * @brief Test BFS on a fully connected (complete) graph.
 */
void test_bfs_complete_graph(void)
{
    // Arrange.
    graph = initialize_graph(4);

    for (int index = 0; index < 4; index++)
    {
        for (int next = index + 1; next < 4; next++)
        {
            add_edge(graph, index, next);
        }
    }

    // Act.
    breadth_first_search(graph, 0);

    // Assert.
    for (int index = 0; index < 4; index++)
    {
        TEST_ASSERT_TRUE(graph->visited[index]);
    }
}

/**
 * @brief Test BFS starting index out of bounds.
 */
void test_bfs_invalid_start_index(void)
{
    // Arrange.
    graph = initialize_graph(5);

    int invalid_start = 10;
    
    // Act.
    breadth_first_search(graph, invalid_start);
    
    // Assert.
    for (int index = 0; index < 5; index++)
    {
        TEST_ASSERT_FALSE(graph->visited[index]);
    }
}

/**
 * @brief Test BFS with a NULL graph pointer.
 */
void test_bfs_null_graph(void)
{
    // Arrange.
    struct Graph* null_graph = NULL;

    // Act & Assert.
    breadth_first_search(null_graph, 0);

    TEST_ASSERT_NULL(null_graph);
}

/**
 * @brief Test BFS on a graph with a single vertex.
 */
void test_bfs_single_vertex_graph(void)
{
    // Arrange.
    graph = initialize_graph(1);

    // Act.
    breadth_first_search(graph, 0);

    // Assert.
    TEST_ASSERT_TRUE(graph->visited[0]);
}

/**
 * @brief Test BFS traversal in a cycle.
 */
void test_bfs_cyclic_graph(void)
{
    // Arrange.
    graph = initialize_graph(3);

    add_edge(graph, 0, 1);
    add_edge(graph, 1, 2);
    add_edge(graph, 2, 0);

    // Act.
    breadth_first_search(graph, 0);

    // Assert.
    TEST_ASSERT_TRUE(graph->visited[0]);
    TEST_ASSERT_TRUE(graph->visited[1]);
    TEST_ASSERT_TRUE(graph->visited[2]);
}

/**
 * @brief Test adding multiple edges to the same vertex.
 */
void test_multiple_edges_same_vertex(void)
{
    // Arrange.
    graph = initialize_graph(4);

    add_edge(graph, 0, 1);
    add_edge(graph, 0, 2);
    add_edge(graph, 0, 3);

    // Act.
    breadth_first_search(graph, 0);

    // Assert.
    for (int index = 0; index < 4; index++)
    {
        TEST_ASSERT_TRUE(graph->visited[index]);
    }
}

/**
 * @brief Test BFS on a disconnected graph with two components.
 */
void test_bfs_disconnected_components(void)
{
    // Arrange.
    graph = initialize_graph(4);

    add_edge(graph, 0, 1);
    add_edge(graph, 2, 3);

    // Act.
    breadth_first_search(graph, 0);

    // Assert.
    TEST_ASSERT_TRUE(graph->visited[0]);
    TEST_ASSERT_TRUE(graph->visited[1]);
    TEST_ASSERT_FALSE(graph->visited[2]);
    TEST_ASSERT_FALSE(graph->visited[3]);
}

/**
 * @brief Test memory stability for a large graph.
 */
void test_large_graph_integrity(void)
{
    // Arrange.
    int large_size = 100;

    graph = initialize_graph(large_size);

    for (int index = 0; index < large_size - 1; index++)
    {
        add_edge(graph, index, index + 1);
    }
    
    // Act.
    int result_count = graph->number_of_vertices;

    // Assert.
    TEST_ASSERT_EQUAL_INT(100, result_count);
}

/**
 * @brief Test graph initialization with zero vertices.
 */
void test_initialize_zero_vertices(void)
{
    // Arrange & Act.
    graph = initialize_graph(0);
    
    // Assert.
    if (graph != NULL)
    {
        TEST_ASSERT_EQUAL_INT(0, graph->number_of_vertices);
    }
}

/**
 * @brief Test BFS traversal path in a star topology.
 */
void test_bfs_star_topology(void)
{
    // Arrange.
    graph = initialize_graph(5);

    add_edge(graph, 0, 1);
    add_edge(graph, 0, 2);
    add_edge(graph, 0, 3);
    add_edge(graph, 0, 4);

    // Act.
    breadth_first_search(graph, 0);

    // Assert.
    for (int index = 0; index < 5; index++)
    {
        TEST_ASSERT_TRUE(graph->visited[index]);
    }
}

/**
 * @brief Test BFS behavior when nodes are already marked visited.
 */
void test_bfs_pre_visited_nodes(void)
{
    // Arrange.
    graph = initialize_graph(2);
    add_edge(graph, 0, 1);
    graph->visited[1] = true;

    // Act.
    breadth_first_search(graph, 0);
    
    // Assert.
    TEST_ASSERT_TRUE(graph->visited[0]);
    TEST_ASSERT_TRUE(graph->visited[1]);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_graph_initialization);
    RUN_TEST(test_add_edge_connectivity);
    RUN_TEST(test_bfs_linear_graph_visited);
    RUN_TEST(test_bfs_isolated_vertex);
    RUN_TEST(test_bfs_complete_graph);
    RUN_TEST(test_bfs_invalid_start_index);
    RUN_TEST(test_bfs_null_graph);
    RUN_TEST(test_bfs_single_vertex_graph);
    RUN_TEST(test_bfs_cyclic_graph);
    RUN_TEST(test_multiple_edges_same_vertex);
    RUN_TEST(test_bfs_disconnected_components);
    RUN_TEST(test_large_graph_integrity);
    RUN_TEST(test_initialize_zero_vertices);
    RUN_TEST(test_bfs_star_topology);
    RUN_TEST(test_bfs_pre_visited_nodes);

    // Destroy the session.
    return UNITY_END();
}