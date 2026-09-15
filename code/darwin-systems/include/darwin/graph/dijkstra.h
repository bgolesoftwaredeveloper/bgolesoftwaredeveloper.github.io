/**
 * @file        :   dijkstra.h
 * @author      :   Braiden Gole
 * @brief       :   Header definition for Dijkstra's shortest path algorithm.
 * @details     :   Defines the data structures and function declarations required
 * to model and compute shortest paths in a weighted graph using
 * a flattened row-major adjacency matrix context.
 * @version     :   1.0
 * @date        :   2026-05-24
 */
#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <stddef.h>
#include <stdbool.h>

#define NUMBER_OF_VERTICES 9

/**
 * @struct  Graph
 * @brief   Encapsulates the state and memory context of the graph system.
 * @details This structure unifies the graph's structural definition with the
 * dynamic runtime tracking structures needed during pathfinding execution.
 */
typedef struct
{
    const int* adjacency_matrix;
    size_t total_vertices;
    int* shortest_distances;
    bool* visited;
} Graph;


// Function prototypes.
bool dijkstra(const Graph* context, size_t source);
void print_shortest_paths(const Graph* context);

#endif