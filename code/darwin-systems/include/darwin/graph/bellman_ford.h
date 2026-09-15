/**
 * @file        :   bellman_ford.h
 * @author      :   Braiden Gole
 * @brief       :   Interface definitions for the Bellman-Ford shortest path algorithm.
 * @details     :   This header file declares the core Graph and Edge data structures, 
 * along with memory management and path-finding routine declarations 
 * necessary for executing single-source shortest paths on directed 
 * graphs with negative edge weights.
 * @version     :   1.0
 * @date        :   2026-05-24
 */
#ifndef BELLMAN_FORD_H
#define BELLMAN_FORD_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @struct      :   Edge
 * @brief       :   Represents a directed, weighted link between two vertices.
 * @details     :   Contains structural identity properties to map connectivity and 
 *                  traversal costs between specific nodes inside a network system.
 */
typedef struct
{
    size_t source;
    size_t destination;
    int weight;
} Edge;

/**
 * @struct      :   Graph
 * @brief       :   A container structure for a directed, weighted graph dataset.
 * @details     :   Maintains core scale limits tracking aggregate vertices and edges, 
 *                  alongside a dynamically allocated list of individual edge payloads.
 */
typedef struct
{
    size_t total_vertices;
    size_t total_edges;
    Edge* array;
} Graph;

// Function prototypes.
Graph* initialize(size_t total_vertices, size_t total_edges);
bool bellman_ford(const Graph* graph, size_t source);

#endif