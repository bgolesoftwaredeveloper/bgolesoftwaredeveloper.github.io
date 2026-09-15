/**
 * @file        :   bellman_ford.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the Bellman-Ford shortest path algorithm.
 * @details     :   This source file provides operations for initializing a graph 
 * structure and computing single-source shortest paths using the 
 * Bellman-Ford algorithm. It handles directed graphs containing negative 
 * edge weights, detects negative-weight cycles, formats clear tabular results, 
 * and manages memory cleanup.
 * @version     :   1.0
 * @date        :   2026-05-24
 */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// Include header files.
#include "darwin/graph/bellman_ford.h"

/**
 * @brief       :   Allocates and constructs a graph object container.
 * @details     :   Dynamically allocates memory for the core Graph structure and its underlying edge array payload.
 * @param       :   total_vertices: Total number of structural nodes in the system.
 * @param       :   total_edges: Capacity size for the aggregate edge connectivity array.
 * @return      :   Graph*: Pointer to the allocated graph instance, or NULL on allocation failure.
 */
Graph* initialize(size_t total_vertices, size_t total_edges)
{
    // Prevent initializing an empty graph or zero-byte spaces.
    if (total_vertices == 0 || total_edges == 0)
    {
        return NULL;
    }

    // Allocate memory for the main graph container structure.
    Graph* graph = (Graph*)malloc(sizeof(Graph));

    if (graph == NULL)
    {
        return NULL;
    }

    // Set the graph properties.
    graph->total_vertices = total_vertices;
    graph->total_edges = total_edges;

    // Allocate memory block capable of storing the complete list of edges.
    graph->array = (Edge*)malloc(sizeof(Edge) * graph->total_edges);

    if (graph->array == NULL)
    {
        free(graph);

        return NULL;
    }

    return graph;
}

/**
 * @brief       :   Displays the final calculated distances cleanly in a console grid.
 * @details     :   Iterates across computed distances using fixed-width layout specifiers to ensure
 *                  perfect column alignment even with expanding multi-digit indexes. Handles unreached nodes safely.
 * @param       :   distance: Array tracking the final numerical weight results from the source.
 * @param       :   total_vertices: The total count of entries inside the tracking database array.
 */
static inline void print_results(const int distance[], size_t total_vertices)
{
    // Ensure the results array pointer is completely valid before looping.
    if (distance == NULL || total_vertices == 0)
    {
        return;
    }

    // Clean header with aligned columns.
    printf("%-8s %s\n", "Vertex", "Distance from Source");

    for (size_t index = 0; index < total_vertices; index++)
    {
        if (distance[index] == INT_MAX)
        {
            // Safely handle unreachable vertices and keep alignment
            printf("%-8zu INF\n", index);
        }
        else
        {
            // %-8zu left-aligns the index within an 8-character wide block
            printf("%-8zu %i\n", index, distance[index]);
        }
    }
}

/**
 * @brief       :   Executes the main path-finding operations and validation checks.
 * @details     :   Performs V-1 passes of structural relaxation over all input data edges, followed by
 *                  a primary loop pass specifically designed to flag valid path distortion from negative loops.
 * @param       :   graph: Const pointer referencing the target dataset payload struct.
 * @param       :   source: Zero-indexed node identifier defining where path computation starts.
 */
bool bellman_ford(const Graph* graph, size_t source)
{
    // Validate basic component layout contexts before processing.
    if (graph == NULL || graph->array == NULL || graph->total_vertices == 0 || graph->total_edges == 0)
    {
        return false;
    }

    // Validate that the designated vertex fits inside the graph domain boundaries.
    if (source >= graph->total_vertices)
    {
        return false;
    }

    size_t total_vertices = graph->total_vertices;
    size_t total_edges = graph->total_edges;

    // Dynamically allocate a distance array tracking cost to every vertex.
    int* distance = (int*)malloc(sizeof(int) * total_vertices);

    if (distance == NULL)
    {
        return false;
    }

    // Initialize all distances to infinity (INT_MAX) as standard baseline.
    for (size_t index = 0; index < total_vertices; index++)
    {
        distance[index] = INT_MAX;
    }

    // The shortest path from the source vertex to itself is always 0.
    distance[source] = 0;

    // Phase 1: Relax all graph edges (total_vertices - 1) times.
    for (size_t relaxation_step = 1; relaxation_step < total_vertices; relaxation_step++)
    {
        for (size_t edge_index = 0; edge_index < total_edges; edge_index++)
        {
            size_t current_source = graph->array[edge_index].source;
            size_t current_destination = graph->array[edge_index].destination;
            int current_weight = graph->array[edge_index].weight;

            // Shield indices from corrupting memory out-of-bounds.
            if (current_source >= total_vertices || current_destination >= total_vertices)
            {
                continue;
            }

            // If the source node is reachable, check if walking this edge offers a shorter route.
            if (distance[current_source] != INT_MAX)
            {
                // Defensive Check: Prevent signed integer overflow/underflow before adding weights
                if (current_weight > 0 && distance[current_source] > INT_MAX - current_weight)
                {
                    continue;
                }
                if (current_weight < 0 && distance[current_source] < INT_MIN - current_weight)
                {
                    continue;
                }

                if (distance[current_source] + current_weight < distance[current_destination])
                {
                    distance[current_destination] = distance[current_source] + current_weight;
                }
            }
        }
    }

    // Phase 2: Check the graph for active negative-weight cycles.
    bool negative_cycle_detected = false;

    for (size_t edge_index = 0; edge_index < total_edges; edge_index++)
    {
        size_t current_source = graph->array[edge_index].source;
        size_t current_destination = graph->array[edge_index].destination;
        int current_weight = graph->array[edge_index].weight;

        // Skip parsing if indices are malformed.
        if (current_source >= total_vertices || current_destination >= total_vertices)
        {
            continue;
        }

        if (distance[current_source] != INT_MAX)
        {
            // Defensive Check: Protect cycle evaluation step from data overflows
            if ((current_weight > 0 && distance[current_source] > INT_MAX - current_weight) ||
                (current_weight < 0 && distance[current_source] < INT_MIN - current_weight))
            {
                continue;
            }

            // Verified comparison against target destination node to map variations accurately
            if (distance[current_source] + current_weight < distance[current_destination])
            {
                printf("%s", "Graph contains a negative weight cycle!\n");

                negative_cycle_detected = true;

                break;
            }
        }
    }

    // Clean execution routing based on discovery status
    if (negative_cycle_detected)
    {
        free(distance);

        return false;
    }

    // Print the results.
    print_results(distance, total_vertices);

    free(distance);

    return true;
}