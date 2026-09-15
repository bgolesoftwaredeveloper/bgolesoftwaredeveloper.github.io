/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Execution driver for demonstrating Kahn's topological sort algorithm.
 * @details     :      Initializes a directed acyclic graph (DAG) with dependency edges,
 * executes Kahn's algorithm to resolve a valid linear execution order,
 * and handles defensive runtime error checks and resource cleanup.
 * clang -Wall -Wextra -Wpedantic ./src/darwin/graph/kahns/main.c ./src/darwin/graph/kahns/kahns.c -I./include -o ./bin/kahns_test
 * @version     :   1.0
 * @date        :   2026-05-27
 */
#include <stdio.h>

// Include header files.
#include "darwin/graph/kahns.h"

int main(void)
{
    // Determine the number of vertices to work with.
    size_t number_of_vertices = 6;

    printf("%s %zu %s", "Initializinng a direct graph with", number_of_vertices, "vertices...\n");

    // Initialize the graph.
    Graph* graph = initialize(number_of_vertices);

    if (graph == NULL)
    {
        fprintf(stderr, "Out of memory.\n");

        return 1;
    }

    bool flag = true;

    // Add edges.
    flag &= add_edge(graph, 5, 2);
    flag &= add_edge(graph, 5, 0);
    flag &= add_edge(graph, 4, 0);
    flag &= add_edge(graph, 4, 1);
    flag &= add_edge(graph, 2, 3);
    flag &= add_edge(graph, 3, 1);

    if (!flag)
    {
        fprintf(stderr, "Failed to safely insert graph edges.\n");

        return 1;
    }

    // Perform the kahns algorithm.
    bool success = kahns(graph);

    if (success)
    {
        printf("%s", "Topological sort completed successfully!\n");
    }
    else
    {
        printf("%s", "Topological sort failed!\n");
    }

    // Clean up.
    clean(graph);

    return 0;
}