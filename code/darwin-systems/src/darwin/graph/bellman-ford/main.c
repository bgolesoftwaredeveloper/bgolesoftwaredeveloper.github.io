/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Entry point for demonstrating and testing the Bellman-Ford shortest path algorithm.
 * @details     :   This file instantiates a directed graph with 5 vertices and 8 edges, 
 * including positive and negative weights. It allocates memory for the graph structure, 
 * initializes the data, runs the Bellman-Ford path-finding routine starting from a 
 * source vertex of 0, and safely frees all allocated resources upon completion.
 * clang -Wall -Wextra -Wpedantic ./src/darwin/graph/bellman-ford/main.c ./src/darwin/graph/bellman-ford/bellman_ford.c -I./include -o ./bin/bellman_ford_test
 * @version     :   1.0
 * @date        :   2026-05-24
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/graph/bellman_ford.h"

int main(void)
{
    // Define variables for the graph.
    size_t total_vertices = 5;
    size_t total_edges = 8;

    // Allocate memory and initialize the graph structure.
    Graph* graph = initialize(total_vertices, total_edges);

    if (graph == NULL)
    {
        fprintf(stderr, "Error out of memory.\n");

        return 1;
    }

    // Initialize edge array data
    graph->array[0].source = 0;
    graph->array[0].destination = 1;
    graph->array[0].weight = -1;

    graph->array[1].source = 0;
    graph->array[1].destination = 2;
    graph->array[1].weight = 4;

    graph->array[2].source = 1;
    graph->array[2].destination = 2;
    graph->array[2].weight = 3;

    graph->array[3].source = 1;
    graph->array[3].destination = 3;
    graph->array[3].weight = 2;

    graph->array[4].source = 1;
    graph->array[4].destination = 4;
    graph->array[4].weight = 2;

    graph->array[5].source = 3;
    graph->array[5].destination = 2;
    graph->array[5].weight = 5;

    graph->array[6].source = 3;
    graph->array[6].destination = 1;
    graph->array[6].weight = 1;

    graph->array[7].source = 4;
    graph->array[7].destination = 3;
    graph->array[7].weight = -3;

    size_t source = 0;

    // Execute the Bellman Ford algorithm.
    if (!bellman_ford(graph, source))
    {
        fprintf(stderr, "Error in bellman_ford().\n");

        return 1;
    }

    // Clean up.
    free(graph->array);
    free(graph);

    return 0;
}