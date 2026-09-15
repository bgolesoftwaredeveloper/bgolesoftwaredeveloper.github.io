/**
 * @file        :   breadth_first_search.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the Breadth-First Search (BFS) algorithm.
 * @details     :   Traverses a graph level by level starting from a source node 
 * using a FIFO queue and a visited array to track progress.
 * @version     :   1.0
 * @date        :   2026-04-19
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/searching/breadth_first_search.h"

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
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));

    if (!memory_check(new_node))
    {
        return NULL;
    }

    new_node->vertex = vertex;
    new_node->next = NULL;

    return new_node;
}

/**
 * @brief           :   Allocates and initializes the graph structure and its members.
 * @param vertices  :   The total number of vertices in the graph.
 * @return          :   Pointer to the initialized Graph if successful; otherwise NULL.
 */
struct Graph* initialize_graph(int vertices)
{
    // Allocate the main graph container.
    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));

    if (!memory_check(graph))
    {
        return NULL;
    }

    graph->number_of_vertices = vertices;

    // Allocate the array of head pointers for the adjacency list.
    graph->adjacency_list = (struct Node**)malloc(sizeof(struct Node*) * vertices);

    if (!memory_check(graph->adjacency_list))
    {
        free(graph);

        return NULL;
    }

    // Allocate the visited tracker array.
    graph->visited = (bool*)malloc(sizeof(bool) * vertices);

    if (!memory_check(graph->visited))
    {
        free(graph->adjacency_list);
        free(graph);

        return NULL;
    }

    // Initialize all lists as empty and vertices as unvisited.
    for (int index = 0; index < vertices; index++)
    {
        graph->adjacency_list[index] = NULL;
        graph->visited[index] = false;
    }

    return graph;
}

/**
 * @brief           :   Adds an undirected edge between two vertices in the graph.
 * @param graph     :   Pointer to the graph structure.
 * @param source    :   The starting vertex index.
 * @param destination:  The ending vertex index.
 * @details         :   Creates new nodes for both the source and destination 
 *                      adjacency lists to represent a bidirectional connection.
 */
void add_edge(struct Graph* graph, int source, int destination)
{
    // Add destination to source's adjacency list.
    struct Node* new_node = create(destination);

    if (!memory_check(new_node))
    {
        return;
    }

    new_node->next = graph->adjacency_list[source];
    graph->adjacency_list[source] = new_node;

    // Add source to the destination's adjacency list (undirected).
    new_node = create(source);

    if (!memory_check(new_node))
    {
        return;
    }

    new_node->next = graph->adjacency_list[destination];
    graph->adjacency_list[destination] = new_node;
}

/**
 * @brief           :   Performs a level-order traversal of the graph.
 * @param graph     :   Pointer to the graph structure.
 * @param start     :   The vertex index where the traversal begins.
 * @details         :   Uses a dynamically allocated FIFO queue to visit all 
 *                      reachable nodes, printing them in a comma-separated list.
 */
void breadth_first_search(struct Graph* graph, int start)
{
    if (graph == NULL || start >= graph->number_of_vertices)
    {
        return;
    }

    // Allocate queue based on the number of vertices in the graph.
    // In BFS, a node is enqueued at most once, so size N is sufficient.
    int* queue = (int*)malloc(sizeof(int) * graph->number_of_vertices);

    if (!memory_check(queue))
    {
        return;
    }

    int front = 0;
    int rear = 0;

    // Mark the starting node as visited and enqueue it.
    graph->visited[start] = true;
    queue[rear++] = start;

    printf("%s", "BFS Traversal: [");

    while (front < rear)
    {
        // Dequeue the next vertex;
        int current = queue[front++];

        if (front > 1)
        {
            printf("%s", ", ");
        }

        printf("%i", current);

        // Traverse the adjacency list of the current vertex.
        struct Node* temp = graph->adjacency_list[current];

        while (temp != NULL)
        {
            int neighbor = temp->vertex;

            if (!graph->visited[neighbor])
            {
                graph->visited[neighbor] = true;
                queue[rear++] = neighbor;
            }

            temp = temp->next;
        }
    }

    printf("%s", "]\n");

    // Free dynamically allocated resources.
    free(queue);
}

/**
 * @brief           :   Deallocates all memory associated with the graph structure.
 * @param graph     :   Pointer to the graph to be destroyed.
 * @details         :   Iterates through each adjacency list to free individual nodes 
 *                      before freeing the list array, visited array, and graph container.
 */
void destroy_graph(struct Graph* graph)
{
    if (graph == NULL)
    {
        return;
    }

    // Iterate through each vertex to free its linked adjacency list.
    for (int index = 0; index < graph->number_of_vertices; index++)
    {
        struct Node* current = graph->adjacency_list[index];

        // Traverse the linked list at this vertex and free each node.
        while (current != NULL)
        {
            struct Node* temporary = current;
            current = current->next;

            free(temporary);
        }
    }

    // Free the primary arrays allocated during initialization.
    free(graph->adjacency_list);
    free(graph->visited);

    // Free the graph structure itself.
    free(graph);
}