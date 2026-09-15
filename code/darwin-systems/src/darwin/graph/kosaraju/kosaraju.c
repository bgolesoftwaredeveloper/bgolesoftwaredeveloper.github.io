/**
 * @file        :   kosaraju.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of Kosaraju's Algorithm for finding SCCs.
 * @details     :   This source file contains the logic for graph manipulation,
 * graph transposition, and the two-pass Depth First Search (DFS)
 * required to identify Strongly Connected Components in a 
 * directed graph using a generic stack for vertex ordering.
 * @version     :   1.0
 * @date        :   2026-05-08
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Include header files.
#include "darwin/graph/kosaraju.h"
#include "darwin/stack/generic_stack.h"

/**
 * @brief       :   Initializes a new directed graph.
 * @param       :   vertices: The total number of nodes in the graph.
 * @return      :   Graph*: Pointer to the allocated graph, or NULL on failure.
 */
Graph* initialize_graph(size_t vertices)
{
    // Preventing initializing a graph with zero nodes.
    if (vertices == 0)
    {
        return NULL;
    }

    // Allocate memory for the graph structure itself.
    Graph* graph = (Graph*)malloc(sizeof(Graph));

    if (graph == NULL)
    {
        return NULL;
    }

    // Allocate an array of pointers (adjacent list heads), one for each vertex.
    graph->adjacency_list = (Node**)malloc(sizeof(Node*) * vertices);

    if (graph->adjacency_list == NULL)
    {
        free(graph);

        return NULL;
    }

    // Explicitly set all adjacency list heads to NULL to prevent garbage pointers.
    for (size_t index = 0; index < vertices; index++)
    {
        graph->adjacency_list[index] = NULL;
    }

    graph->total_vertices = vertices;

    return graph;
}

/**
 * @brief       :   Adds a directed edge between two vertices.
 * @details     :   Performs defensive bounds checking and handles allocation failure.
 * @param       :   graph: Pointer to the graph.
 * @param       :   source: Origin vertex index.
 * @param       :   destination: Target vertex index.
 */
void add_edge(Graph* graph, size_t source, size_t destination)
{
    // Ensuring the graph exists before attempting to modify it.
    if (graph == NULL || graph->adjacency_list == NULL)
    {
        return;
    }

    // Validate that indices are within the range [0, total_vertices - 1].
    if (source >= graph->total_vertices || destination >= graph->total_vertices)
    {
        return;
    }

    // Create a new adjacency node for the destination vertex.
    Node* new_node = (Node*)malloc(sizeof(Node));

    if (new_node == NULL)
    {
        return;
    }

    // Standard linked-list insertion at the head of the source's list.
    new_node->id = destination;
    new_node->next = graph->adjacency_list[source];

    graph->adjacency_list[source] = new_node;
}

/**
 * @brief       :   Frees all memory associated with the graph.
 * @details     :   Deallocates all nodes in the adjacency list and the graph structure.
 * @param       :   graph: Pointer to the graph to be destroyed.
 */
void destroy_graph(Graph* graph)
{
    if (graph == NULL)
    {
        return;
    }

    // Iterate through every vertex to free its individual edge list.
    if (graph->adjacency_list != NULL)
    {
        for (size_t index = 0; index < graph->total_vertices; index++)
        {
            Node* current = graph->adjacency_list[index];

            while (current != NULL)
            {
                Node* temp = current;
                current = current->next;

                free(temp);
            }
        }

        // Free the array of head pointers.
        free(graph->adjacency_list);
    }

    // Free the graph container.
    free(graph);
}

/**
 * @brief       :   First pass DFS to determine vertex finishing times.
 * @details     :   Recursively visits nodes and pushes them onto the stack after
 *                  all their neighbors have been processed.
 * @param       :   graph: Pointer to the graph.
 * @param       :   vertex: Current vertex being visited.
 * @param       :   visited: Boolean array tracking visited nodes.
 * @param       :   stack: Pointer to the GenericStack for storing finish order.
 */
void compute_finish_order(Graph* graph, size_t vertex, bool visited[], GenericStack* stack)
{
    // Safety check for NULL pointers.
    if (graph == NULL || visited == NULL || stack == NULL)
    {
        return;
    }

    // Boundary check for the current vertex.
    if (vertex >= graph->total_vertices)
    {
        return;
    }

    // Mark the current node as visited.
    visited[vertex] = true;

    // Explore all unvisited neighbors.
    for (Node* walker = graph->adjacency_list[vertex]; walker != NULL; walker = walker->next)
    {
        if (walker->id < graph->total_vertices && !visited[walker->id])
        {
            compute_finish_order(graph, walker->id, visited, stack);
        }
    }

    // All reachable nodes from this vertex are processed; push it to the stack.
    push(stack, &vertex);
}

/**
 * @brief       :   Second pass DFS to discover components in the transposed graph.
 * @details     :   Recursively visits and prints nodes belonging to the same SCC.
 * @param       :   graph: Pointer to the transposed graph.
 * @param       :   vertex: Current vertex being visited.
 * @param       :   visited: Boolean array tracking visited nodes.
 */
void component_dfs(Graph* graph, size_t vertex, bool visited[])
{
    // Safety check for NULL pointers.
    if (graph == NULL || visited == NULL)
    {
        return;
    }

    // Boundary check for the current vertex.
    if (vertex >= graph->total_vertices)
    {
        return;
    }

    // Mark node as visited and print it as part of the current SCC.
    visited[vertex] = true;

    printf("%zu ", vertex);

    // In the transposed graph, we follow the reversed edges.
    for (Node* walker = graph->adjacency_list[vertex]; walker != NULL; walker = walker->next)
    {
        if (walker->id < graph->total_vertices && !visited[walker->id])
        {
            component_dfs(graph, walker->id, visited);
        }
    }
}

/**
 * @brief       :   Creates a new graph with all edges reversed.
 * @param       :   graph: Pointer to the source graph.
 * @return      :   Graph*: Pointer to the newly allocated transposed graph.
 */
Graph* generate_transposed(Graph* graph)
{
    if (graph == NULL)
    {
        return NULL;
    }

    // Create a new graph structure with the same vertex count.
    Graph* transposed = initialize_graph(graph->total_vertices);

    if (transposed == NULL)
    {
        return NULL;
    }
    
    // For every edge A -> B in original, add edge B -> A in transposed.
    for (size_t vertex = 0; vertex < graph->total_vertices; vertex++)
    {
        for (Node* walker = graph->adjacency_list[vertex]; walker != NULL; walker = walker->next)
        {
            add_edge(transposed, walker->id, vertex);
        }
    }

    return transposed;
}

/**
 * @brief       :   Orchestrates the Kosaraju algorithm to identify and print SCCs.
 * @details     :   Handles initialization of the stack and visited array, manages
 *                  the two DFS passes, and performs final memory cleanup.
 * @param       :   graph: Pointer to the directed graph to analyze.
 */
void strongly_connected_components(Graph* graph)
{
    // Early exit if the graph is invalid or empty.
    if (graph == NULL || graph->total_vertices == 0)
    {
        return;
    }

    // Pass 1: Order vertices by finishing time.
    GenericStack stack;

    if (!initialize(&stack, sizeof(size_t)))
    {
        return;
    }

    // Initialize teh visited array to all false.
    bool* visited = (bool*)calloc(graph->total_vertices, sizeof(bool));

    if (visited == NULL)
    {
        clear(&stack);

        return;
    }

    for (size_t index = 0; index < graph->total_vertices; index++)
    {
        if (!visited[index])
        {
            compute_finish_order(graph, index, visited, &stack);
        }
    }

    // Pass 2: Reverse the graph.
    Graph* transposed = generate_transposed(graph);

    if (transposed == NULL)
    {
        free(visited);
        clear(&stack);

        return;
    }

    // Reset the visited array for the second DFS pass.
    for (size_t index = 0; index < graph->total_vertices; index++)
    {
        visited[index] = false;
    }

    printf("%s", "Strongly connected components:\n");

    // Pass 3: Extract components in the order provided by the stack.
    while (!is_empty(&stack))
    {
        size_t vertex = 0;

        if (pop(&stack, &vertex))
        {
            // If the popped vertex is unvisited, it's the root of a new SCC.
            if (!visited[vertex])
            {
                component_dfs(transposed, vertex, visited);

                printf("%c", '\n');
            }
        }        
    }

    // Final cleanup of temporary resources.
    free(visited);

    destroy_graph(transposed);

    clear(&stack);
}