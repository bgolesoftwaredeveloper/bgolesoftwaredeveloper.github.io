/**
 * @file        :   depth_first_search.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the Depth-First Search (DFS) algorithm.
 * @details     :   Traverses a graph by exploring as far as possible along each 
 * branch before backtracking, using recursion (system stack) and a visited array.
 * @version     :   1.0
 * @date        :   2026-04-19
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Include header files.
#include "darwin/searching/depth_first_search.h"

/**
 * @brief           :   Utility to check if a memory block was successfully allocated.
 * @param block     :   Pointer to the memory block to check.
 * @return          :   True if the block is not NULL; otherwise false.
 */
static inline bool memory_check(void* block)
{
    return (block != NULL);
}

/**
 * @brief           :   Creates and initializes a new Node for the adjacency list.
 * @param vertex    :   The vertex index to be stored in the node.
 * @return          :   Pointer to the new Node if successful; otherwise NULL.
 */
static inline struct Node* create(int vertex)
{
    // Allocate memory for the node.
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));

    if (!memory_check(new_node))
    {
        return NULL;
    }

    // Set node data and initialize the next pointer to NULL.
    new_node->vertex = vertex;
    new_node->next = NULL;

    return new_node;
}

/**
 * @brief           :   Allocates and initializes the graph structure.
 * @param vertices  :   The total number of vertices in the graph.
 * @return          :   Pointer to the initialized Graph if successful; otherwise NULL.
 */
struct Graph* initialize_graph(int vertices)
{
    // Allocate memory for the main graph container.
    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));

    if (!memory_check(graph))
    {
        return NULL;
    }

    // Allocate an array of pointers for the adjacency lists (one per vertex).
    graph->number_of_vertices = vertices;
    graph->adjacency_list = (struct Node**)malloc(sizeof(struct Node*) * vertices);

    if (!memory_check(graph->adjacency_list))
    {
        free(graph);

        return NULL;
    }

    // Allocate the boolean array to track visited status during search.
    graph->visited = (bool*)malloc(sizeof(bool) * vertices);

    if (!memory_check(graph->visited))
    {
        free(graph->adjacency_list);
        free(graph);

        return NULL;
    }

    // Initialize all list heads to NULL and visited flags to false.
    for (int index = 0; index < vertices; index++)
    {
        graph->adjacency_list[index] = NULL;
        graph->visited[index] = false;
    }

    return graph;
}

/**
 * @brief           :   Adds an undirected edge between two vertices.
 * @param graph     :   Pointer to the graph structure.
 * @param source    :   The starting vertex index.
 * @param destination:  The ending vertex index.
 */
void add_edge(struct Graph* graph, int source, int destination)
{
    // Create node for destination and link it to source's list.
    struct Node* destination_node = create(destination);

    if (!memory_check(destination_node))
    {
        return;
    }

    destination_node->next = graph->adjacency_list[source];
    graph->adjacency_list[source] = destination_node;

    // Since it's undirected, create node for source and link it to destination's list.
    struct Node* source_node = create(source);

    if (!memory_check(source_node))
    {
        return;
    }

    source_node->next = graph->adjacency_list[destination];
    graph->adjacency_list[destination] = source_node;
}

/**
 * @brief           :   Internal recursive helper for the DFS traversal.
 * @param graph     :   Pointer to the graph structure.
 * @param vertex    :   The current vertex index being explored.
 * @param initial   :   Pointer to a flag to manage comma formatting in output.
 */
static inline void dfs_recursive(struct Graph* graph, int vertex, bool* initial)
{
    // Mark the current node as visited.
    graph->visited[vertex] = true;

    // Handle comma printing: Don't print a comma before the initial item.
    if (!(*initial))
    {
        printf("%s", ", ");
    }

    // Print the current vertex and update the initial flag.
    printf("%i", vertex);

    *initial = false;

    // Recur for all vertices adjacent to this vertex.
    struct Node* adjacent_node = graph->adjacency_list[vertex];

    // Iterate through all neighbors.
    while (adjacent_node != NULL)
    {
        int connected_vertex = adjacent_node->vertex;

        // If the neighbor hasn't been visited, dive deeper (recursive step).
        if (!graph->visited[connected_vertex])
        {
            dfs_recursive(graph, connected_vertex, initial);
        }

        // Move to the next neighbor in the linked list.
        adjacent_node = adjacent_node->next;
    }
}

/**
 * @brief           :   Initiates a depth-first search traversal.
 * @param graph     :   Pointer to the graph structure.
 * @param start     :   The vertex index where the traversal begins.
 * @details         :   Resets visited status and uses a recursive helper to explore 
 * the graph as deep as possible before backtracking.
 */
void depth_first_search(struct Graph* graph, int start)
{
    if (graph == NULL || start >= graph->number_of_vertices)
    {
        return;
    }

    // Reset the visited array so previous searches don't interfere.
    for (int index = 0; index < graph->number_of_vertices; index++)
    {
        graph->visited[index] = false;
    }

    printf("DFS Traversal: [");

    // Flag used to manage pretty-printing commas.
    bool initial = true;

    // Start the recursive exploration.
    dfs_recursive(graph, start, &initial);

    printf("]\n");
}

/**
 * @brief           :   Deallocates all memory associated with the graph.
 * @param graph     :   Pointer to the graph to be destroyed.
 */
void destroy_graph(struct Graph* graph)
{
    if (graph == NULL)
    {
        return;
    }

    // Free every individual node in every adjacency list.
    for (int index = 0; index < graph->number_of_vertices; index++)
    {
        struct Node* current = graph->adjacency_list[index];

        while (current != NULL)
        {
            struct Node* temp = current;
            current = current->next;

            free(temp);
        }
    }

    // Free dynamically allocated resources.
    free(graph->adjacency_list);
    free(graph->visited);
    free(graph);
}