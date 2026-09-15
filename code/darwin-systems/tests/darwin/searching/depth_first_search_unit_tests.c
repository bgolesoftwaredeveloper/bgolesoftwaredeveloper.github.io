/**
 * @file        :   depth_first_search_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit tests for the Depth-First Search (DFS) algorithm.
 * @details     :   Validates graph integrity, recursive traversal depth, 
 * and edge case handling using the Unity Test Framework.
 * Compile:
 * clang ./tests/darwin/searching/depth_first_search_unit_tests.c ./src/darwin/searching/depth-first-search/depth_first_search.c ./src/unity.c -Iinclude -o ./bin/depth_first_search_unit_test
 * @version     :   1.0
 * @date        :   2026-04-19
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "unity.h"
#include "darwin/searching/depth_first_search.h"

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
 * @brief Test initialization and default state of the graph.
 */
void test_graph_initialization_state(void)
{
    // Arrange.
    int vertex_count = 10;

    // Act.
    graph = initialize_graph(vertex_count);

    // Assert.
    TEST_ASSERT_NOT_NULL(graph);
    TEST_ASSERT_EQUAL_INT(10, graph->number_of_vertices);
    
    for (int vertex_index = 0; vertex_index < 10; vertex_index++)
    {
        TEST_ASSERT_FALSE(graph->visited[vertex_index]);
    }
}

/**
 * @brief Test adding an edge and checking the internal adjacency list.
 */
void test_add_edge_internal_structure(void)
{
    // Arrange.
    graph = initialize_graph(3);

    // Act.
    add_edge(graph, 0, 2);

    // Assert.
    TEST_ASSERT_EQUAL_INT(2, graph->adjacency_list[0]->vertex);
    TEST_ASSERT_EQUAL_INT(0, graph->adjacency_list[2]->vertex);
}

/**
 * @brief Test DFS on a completely disconnected graph.
 */
void test_dfs_no_edges_visited(void)
{
    // Arrange.
    graph = initialize_graph(5);

    // Act.
    depth_first_search(graph, 0);

    // Assert.
    TEST_ASSERT_TRUE(graph->visited[0]);

    for (int unreached_index = 1; unreached_index < 5; unreached_index++)
    {
        TEST_ASSERT_FALSE(graph->visited[unreached_index]);
    }
}

/**
 * @brief Test DFS depth on a single long chain (Path Graph).
 */
void test_dfs_deep_path_traversal(void)
{
    // Arrange.
    int size = 50;

    graph = initialize_graph(size);

    for (int link_index = 0; link_index < size - 1; link_index++)
    {
        add_edge(graph, link_index, link_index + 1);
    }

    // Act.
    depth_first_search(graph, 0);

    // Assert.
    for (int node_index = 0; node_index < size; node_index++)
    {
        TEST_ASSERT_TRUE(graph->visited[node_index]);
    }
}

/**
 * @brief Test DFS starting from a leaf node in a tree structure.
 */
void test_dfs_leaf_node_start(void)
{
    // Arrange.
    graph = initialize_graph(4);

    add_edge(graph, 0, 1);
    add_edge(graph, 0, 2);
    add_edge(graph, 2, 3);

    // Act.
    depth_first_search(graph, 3);

    // Assert.
    for (int visited_index = 0; visited_index < 4; visited_index++)
    {
        TEST_ASSERT_TRUE(graph->visited[visited_index]);
    }
}

/**
 * @brief Test DFS with multiple disconnected components.
 */
void test_dfs_multiple_islands(void)
{
    // Arrange.
    graph = initialize_graph(6);

    add_edge(graph, 0, 1);
    add_edge(graph, 1, 2);
    add_edge(graph, 3, 4);
    add_edge(graph, 4, 5);

    // Act.
    depth_first_search(graph, 0);

    // Assert.
    TEST_ASSERT_TRUE(graph->visited[2]);
    TEST_ASSERT_FALSE(graph->visited[3]);
}

/**
 * @brief Test DFS handling of self-loops.
 */
void test_dfs_self_loop_handling(void)
{
    // Arrange.
    graph = initialize_graph(2);

    add_edge(graph, 0, 0); 
    add_edge(graph, 0, 1);

    // Act.
    depth_first_search(graph, 0);

    // Assert.
    TEST_ASSERT_TRUE(graph->visited[0]);
    TEST_ASSERT_TRUE(graph->visited[1]);
}

/**
 * @brief Test DFS on a Complete Graph (K4).
 */
void test_dfs_complete_k4_graph(void)
{
    // Arrange.
    graph = initialize_graph(4);

    for (int outer_index = 0; outer_index < 4; outer_index++) 
    {
        for (int inner_index = outer_index + 1; inner_index < 4; inner_index++) 
        {
            add_edge(graph, outer_index, inner_index);
        }
    }

    // Act.
    depth_first_search(graph, 2);

    // Assert.
    for (int check_index = 0; check_index < 4; check_index++) 
    {
        TEST_ASSERT_TRUE(graph->visited[check_index]);
    }
}

/**
 * @brief Test behavior with a start index equal to vertex count.
 */
void test_dfs_out_of_bounds_index(void)
{
    // Arrange.
    graph = initialize_graph(3);
    
    // Act.
    depth_first_search(graph, 3);

    // Assert.
    for (int index = 0; index < 3; index++) 
    {
        TEST_ASSERT_FALSE(graph->visited[index]);
    }
}

/**
 * @brief Test DFS when the graph pointer is NULL.
 */
void test_dfs_uninitialized_graph_pointer(void)
{
    // Arrange.
    struct Graph* null_ptr = NULL;
    
    // Act.
    depth_first_search(null_ptr, 0);
    
    // Assert.
    TEST_ASSERT_NULL(null_ptr);
}

/**
 * @brief Test DFS on a single node graph.
 */
void test_dfs_minimal_graph(void)
{
    // Arrange.
    graph = initialize_graph(1);
    
    // Act.
    depth_first_search(graph, 0);

    // Assert.
    TEST_ASSERT_TRUE(graph->visited[0]);
}

/**
 * @brief Test DFS on a Wheel Topology.
 */
void test_dfs_wheel_topology(void)
{
    // Arrange.
    graph = initialize_graph(5);

    for (int spoke_index = 1; spoke_index <= 4; spoke_index++) 
    {
        add_edge(graph, 0, spoke_index);
        add_edge(graph, spoke_index, (spoke_index % 4) + 1);
    }

    // Act.
    depth_first_search(graph, 0);

    // Assert.
    for (int verify_index = 0; verify_index < 5; verify_index++) 
    {
        TEST_ASSERT_TRUE(graph->visited[verify_index]);
    }
}

/**
 * @brief Test if destroy_graph handles a NULL pointer.
 */
void test_destroy_graph_null_safety(void)
{
    // Arrange.
    struct Graph* null_graph = NULL;
    
    // Act.
    destroy_graph(null_graph);

    // Assert.
    TEST_ASSERT_NULL(null_graph);
}

/**
 * @brief Test the adjacency list memory deallocation loop logic.
 */
void test_graph_memory_cleanup_integrity(void)
{
    // Arrange.
    graph = initialize_graph(2);

    add_edge(graph, 0, 1);
    
    // Act.
    destroy_graph(graph);

    graph = NULL;

    // Assert.
    TEST_ASSERT_NULL(graph);
}

/**
 * @brief Test DFS with a large number of edges (Dense Graph).
 */
void test_dfs_dense_graph_traversal(void)
{
    // Arrange.
    int vertices = 20;

    graph = initialize_graph(vertices);

    for (int source_index = 0; source_index < vertices; source_index++) 
    {
        for (int dest_index = source_index + 1; dest_index < vertices; dest_index++) 
        {
            add_edge(graph, source_index, dest_index);
        }
    }

    // Act.
    depth_first_search(graph, vertices - 1);

    // Assert.
    for (int result_index = 0; result_index < vertices; result_index++)
    {
        TEST_ASSERT_TRUE(graph->visited[result_index]);
    }
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_graph_initialization_state);
    RUN_TEST(test_add_edge_internal_structure);
    RUN_TEST(test_dfs_no_edges_visited);
    RUN_TEST(test_dfs_deep_path_traversal);
    RUN_TEST(test_dfs_leaf_node_start);
    RUN_TEST(test_dfs_multiple_islands);
    RUN_TEST(test_dfs_self_loop_handling);
    RUN_TEST(test_dfs_complete_k4_graph);
    RUN_TEST(test_dfs_out_of_bounds_index);
    RUN_TEST(test_dfs_uninitialized_graph_pointer);
    RUN_TEST(test_dfs_minimal_graph);
    RUN_TEST(test_dfs_wheel_topology);
    RUN_TEST(test_destroy_graph_null_safety);
    RUN_TEST(test_graph_memory_cleanup_integrity);
    RUN_TEST(test_dfs_dense_graph_traversal);

    // Destroy the session.
    return UNITY_END();
}