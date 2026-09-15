/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Comprehensive demonstration of Kosaraju's SCC algorithm.
 * @details     :   Tests complex cycle detection and reachability.
 * Compile:
 * clang -Wall -Wextra -Wpedantic ./src/darwin/graph/kosaraju/main.c ./src/darwin/graph/kosaraju/kosaraju.c ./src/darwin/stack/generic-stack/generic_stack.c -I./include -o ./bin/kosaraju_test
 * @version     :   1.0
 * @date        :   2026-05-06
 */
#include <stdio.h>

// Include header files.
#include "darwin/graph/kosaraju.h"

int main(void)
{
    // Define a vertex count large enough for a complex demonstration.
    const size_t VERTEX_COUNT = 6;

    // Initialize the graph.
    Graph* graph = initialize_graph(6);

    if (graph == NULL)
    {
        fprintf(stderr, "Fatal: Failed to allocate graph.\n");

        return 1;
    }

    printf("%s %zu %s", "Constructing graph with", VERTEX_COUNT, "vertices...\n");

    // Building SCC Group A: Cycle {0, 1, 2}.
    add_edge(graph, 0, 1);
    add_edge(graph, 1, 2);
    add_edge(graph, 2, 0);

    // Building SCC Group B: Cycle {3, 4}.
    add_edge(graph, 2, 3);
    add_edge(graph, 3, 4);
    add_edge(graph, 4, 3);

    // Building SCC Group C: Cycle {5, 6, 7}.
    add_edge(graph, 4, 5);
    add_edge(graph, 5, 6);
    add_edge(graph, 6, 7);
    add_edge(graph, 7, 5);

    // Execute the algorithm and print the componets to the console.
    strongly_connected_components(graph);

    printf("%c", '\n');

    // Destroy the graph.
    destroy_graph(graph);

    printf("%s", "Execution completed.\n");

    return 0;
}