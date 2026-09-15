/**
 * @file        :   dijkstra.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of Dijkstra's shortest path algorithm.
 * @details     :   Provides a robust pathfinding implementation using a single-dimensional
 * flattened row-major adjacency matrix. Includes thorough defensive 
 * programming constraints to prevent arithmetic integer overflows, null 
 * pointer references, and invalid negative graph weights.
 * @version     :   1.0
 * @date        :   2026-05-24
 */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// Include header files.
#include "darwin/graph/dijkstra.h"

/**
 * @brief       :   Finds the unvisited vertex with the minimum shortest distance value.
 * @details     :   Scans the current path context state to find the next optimal node 
 *                  to process in the greedy frontier loop.
 * @param       :   context Pointer to the constant Graph execution state.
 * @return      :   size_t The index of the unvisited vertex with the minimum distance.
 *                  Returns SIZE_MAX if parameters are invalid or no vertex is found.
 */
static inline size_t find_minimum_distance(const Graph* context)
{
    // Ensure all internal arrays required for scanning are populated.
    if (context == NULL || context->shortest_distances == NULL || context->visited == NULL)
    {
        return SIZE_MAX;
    }

    int minimum_value = INT_MAX;

    size_t minimum_vertex_index = SIZE_MAX;

    // Iterate through all vertices to find the absolute smallest weights remaining.
    for (size_t index = 0; index < context->total_vertices; index++)
    {
        if (!context->visited[index] && context->shortest_distances[index] <= minimum_value)
        {
            minimum_value = context->shortest_distances[index];
            minimum_vertex_index = index;
        }
    }

    return minimum_vertex_index;
}

/**
 * @brief       :   Executes Dijkstra's single-source shortest path algorithm.
 * @details     :   Calculates the minimum cost required to travel from a designated source
 *                  vertex to all other vertices within a non-negatively weighted graph.
 * @param       :   context Pointer to the active Graph context structure containing tracking vectors.
 * @param       :   source The index boundary where path tracking begins.
 * @return      :   bool Returns true if the path mapping completes flawlessly; false if 
 *                  a defensive constraint or input check fails.
 */
bool dijkstra(const Graph* context, size_t source)
{
    // Verify all operation data buffers are not null pointers.
    if (context == NULL || context->adjacency_matrix == NULL || context->shortest_distances == NULL || context->visited == NULL)
    {
        return false;
    }

    // Protect against processing empty graph objects.
    if (context->total_vertices == 0)
    {
        return false;
    }

    // Ensure the designated source fits within the graph's space bounds.
    if (source >= context->total_vertices)
    {
        return false;
    }

    // Pre-scan the entire matrix to deny invalid negative edge paths.
    for (size_t row_index = 0; row_index < context->total_vertices; row_index++)
    {
        for (size_t column_index = 0; column_index < context->total_vertices; column_index++)
        {
            size_t evaluation_index = (row_index * context->total_vertices) + column_index;

            if (context->adjacency_matrix[evaluation_index] < 0)
            {
                return false;
            }
        }
    }

    // Reset/Initialize the search arrays before processing.
    for (size_t index = 0; index < context->total_vertices; index++)
    {
        context->shortest_distances[index] = INT_MAX;
        context->visited[index] = false;
    }

    // The distance from the source vertex to itself is always zero.
    context->shortest_distances[source] = 0;

    // Core optimzation iteration: Process up to (V - 1) vertices.
    for (size_t count = 0; count < context->total_vertices - 1; count++)
    {
        // Pick the next closest unvisited node index.
        size_t current_index = find_minimum_distance(context);

        // If the index is a sentinel, or remaining components are unreachable, stop processing.
        if (current_index == SIZE_MAX || context->shortest_distances[current_index] == INT_MAX)
        {
            break;
        }

        // Sanity check to protect memory operations against sudden context manipulation.
        if (current_index >= context->total_vertices)
        {
            return false;
        }

        // Mark the current node as finalized and processed.
        context->visited[current_index] = true;

        // Relax neighboring vertex pathways from the current index node.
        for (size_t neighbor_index = 0; neighbor_index < context->total_vertices; neighbor_index++)
        {
            // Math lookup converting 2D coordinates into the flat 1D matrix layout.
            size_t index_offset = (current_index * context->total_vertices) + neighbor_index;
            
            int edge_weight = context->adjacency_matrix[index_offset];

            // Ignore instances where no edge exist (0 weight) or the destination node is settled.
            if (edge_weight == 0 || context->visited[neighbor_index])
            {
                continue;
            }

            // Prevent potential mathematical rollover overflow errors.
            if (context->shortest_distances[current_index] > INT_MAX - edge_weight)
            {
                continue;
            }

            int calculated_alternative_distance = context->shortest_distances[current_index] + edge_weight;

            // Relaxation: Update tracking arrays if a shorter path option is found.
            if (calculated_alternative_distance < context->shortest_distances[neighbor_index])
            {
                context->shortest_distances[neighbor_index] = calculated_alternative_distance;
            }
        }
    }

    return true;
}

/**
 * @brief       :   Safely prints the computed shortest paths to standard output.
 * @details     :   Iterates through the calculated values and cleanly prints each node's 
 *                  total path weight cost or an explicit INFINITY label if unreachable.
 * @param       :   context Pointer to the constant Graph execution state.
 */
void print_shortest_paths(const Graph* context)
{
    // Assure context state arrays exist before execution print.
    if (context == NULL || context->shortest_distances == NULL)
    {
        return;
    }

    printf("%s", "Vertex\tDistance from source\n");

    // Format and output the data elements to the standard console stream.
    for (size_t index = 0; index < context->total_vertices; index++)
    {
        if (context->shortest_distances[index] == INT_MAX)
        {
            printf("%zu \t\t\tINFINITY\n", index);
        }
        else
        {
            printf("%zu \t\t\t %i\n", index, context->shortest_distances[index]);
        }
    }
}