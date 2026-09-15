/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Test driver for Kruskal's Minimum Spanning Tree (MST) algorithm.
 * @details     :   Initializes a weighted, undirected graph with 4 vertices and 5 edges,
 * populates it with sample edge data, and runs Kruskal's algorithm using 
 * a Disjoint Set Union (DSU) data structure. It then outputs the selected 
 * edges of the resulting MST along with its total minimum weight.
 * clang -Wall -Wextra -Wpedantic ./src/darwin/graph/kruskal/main.c ./src/darwin/graph/kruskal/kruskal.c -I./include -o ./bin/kruskal_test
 * @version     :   1.0
 * @date        :   2026-05-27
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/graph/kruskal.h"

int main(void)
{
    // Define constnants to work with.
    #define NUMBER_OF_VERTICES 4
    #define NUMBER_OF_EDGES 5

    // Define edges with source, destination, and weight.
    Edge edges[] =
    {
        {0, 1, 10},
        {0, 2, 6},
        {0, 3, 5},
        {1, 3, 15},
        {2, 3, 4}
    };

    // Initialize the graph.
    Graph* graph = initialize(NUMBER_OF_VERTICES, NUMBER_OF_EDGES);

    if (graph == NULL)
    {
        return 1;
    }

    // Add edge data to the graph.
    for (size_t index = 0; index < NUMBER_OF_EDGES; index++)
    {
        graph->edges[index].source = edges[index].source;
        graph->edges[index].destination = edges[index].destination;
        graph->edges[index].weight = edges[index].weight;
    }

    // Perform Kruskal algorithm.
    kruskal(graph);

    // Clean up.
    free(graph->edges);
    free(graph);

    return 0;
}