/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Demonstration entry point for the BFS algorithm.
 * @details     :   Initializes a graph, populates it with edges to create a 
 * specific topology, and performs a Breadth-First Search traversal.
 * 
 * Compile:
 * clang ./src/darwin/searching/breadth-first-search/main.c ./src/darwin/searching/breadth-first-search/breadth_first_search.c -I./include -o ./bin/breadth_first_search_test
 * @version     :   1.0
 * @date        :   2026-04-19
 */
#include <stdio.h>

// Include header files.
#include "darwin/searching/breadth_first_search.h"

int main(void)
{
    // Define the number of vertices for our demonstration graph.
    int vertices = 6;

    // Initialize the graph structure.
    struct Graph* graph = initialize_graph(vertices);

    if (graph == NULL)
    {
        fprintf(stderr, "Failed to initialize graph memory.\n");

        return 1;
    }

    // Add edges to create a sample graph.
    add_edge(graph, 0, 1);
    add_edge(graph, 0, 2);
    add_edge(graph, 1, 3);
    add_edge(graph, 1, 4);
    add_edge(graph, 2, 4);
    add_edge(graph, 3, 4);
    add_edge(graph, 3, 5);
    add_edge(graph, 4, 5);

    // Execute breadth first search.
    breadth_first_search(graph, 0);

    // Clean up allocated memory before exiting.
    destroy_graph(graph);

    return 0;
}