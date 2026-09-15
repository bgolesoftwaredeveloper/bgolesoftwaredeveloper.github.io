/**
 * @file        :   kahns.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of Kahn's algorithm for Topological Sorting.
 * @details     :   Provides a memory-safe adjacency list graph implementation
 * and structural dependency sorting using Kahn's linear time 
 * algorithm. Includes robust out-of-bounds guards and cascading 
 * heap allocation error cleanups.
 * @version     :   1.0
 * @date        :   2026-05-27
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/graph/kahns.h"

/**
 * @brief Allocates and initializes a single adjacency list node.
 * @param destination The target vertex index this node points to.
 * @return Pointer to the newly allocated Node, or NULL on failure.
 */
static inline Node* make(size_t destination)
{
    // Allocate memory for the single node.
    Node* new_node = (Node*)malloc(sizeof(Node));

    if (new_node == NULL)
    {
        return NULL;
    }

    // Assign node payload and initialize the link pointer.
    new_node->destination = destination;
    new_node->next = NULL;

    return new_node;
}

/**
 * @brief Allocates and initializes a directed graph structure.
 * @param vertices Total fixed number of vertices the graph will hold.
 * @return Pointer to the allocated Graph container, or NULL on failure.
 */
Graph* initialize(size_t vertices)
{
    // Guard against empty graphs.
    if (vertices == 0)
    {
        return NULL;
    }

    // Allocate the primary managing structure.
    Graph* graph = (Graph*)malloc(sizeof(Graph));

    if (graph == NULL)
    {
        return NULL;
    }

    // Allocate the array of adjacency list pointers.
    graph->number_of_vertices = vertices;
    graph->adjacency_lists = (Node**)malloc(sizeof(Node*) * vertices);

    if (graph->adjacency_lists == NULL)
    {
        free(graph);

        return NULL;
    }

    // Allocate and zero-initialize the dependency tracking array.
    graph->in_degree = (size_t*)calloc(vertices, sizeof(size_t));

    if (graph->in_degree == NULL)
    {
        free(graph->adjacency_lists);
        free(graph);

        return NULL;
    }

    // Explicitly initialize all headds of adjacency lists to NULL.
    for (size_t index = 0; index < vertices; index++)
    {
        graph->adjacency_lists[index] = NULL;
    }

    return graph;
}

/**
 * @brief Establishes a directed dependency edge (source -> destination).
 * @param graph Pointer to the Graph structure.
 * @param source Originating vertex index.
 * @param destination Target vertex index.
 * @return true if insertion succeeds, false on defensive guard violations or allocation failure.
 */
bool add_edge(Graph* graph, size_t source, size_t destination)
{
    // Guard against NULL context and verify indices fit inside the graph limit bounds.
    if (graph == NULL || source >= graph->number_of_vertices || destination >= graph->number_of_vertices)
    {
        return false;
    }

    // Allocate the destination node wrapper.
    Node* new_node = make(destination);

    if (new_node == NULL)
    {
        return false;
    }

    // Push the node onto the front of the source vertex's adjacency linked list.
    new_node->next = graph->adjacency_lists[source];
    
    graph->adjacency_lists[source] = new_node;

    // Increment dependency tracking context for the destination node.
    graph->in_degree[destination]++;

    return true;
}

/**
 * @brief Evaluates the dependency graph and processes a topological sort using Kahn's algorithm.
 * @param graph Pointer to the Graph structure.
 * @return true if the graph is a valid DAG and successfully sorted, false if a cycle exists or memory fails.
 */
bool kahns(const Graph* graph)
{
    // Guard against processing a non-existent context.
    if (graph == NULL)
    {
        return false;
    }

    size_t number_of_vertices = graph->number_of_vertices;

    // Allocate linear tracking space to behave as the processing queue.
    size_t* queue = (size_t*)malloc(sizeof(size_t) * number_of_vertices);

    if (queue == NULL)
    {
        return false;
    }

    size_t front = 0;
    size_t rear = 0;

    // Allocate linear tracking space for the final sorted result array.
    size_t* topologocal_order = (size_t*)malloc(sizeof(size_t) * number_of_vertices);

    if (topologocal_order == NULL)
    {
        free(queue);

        return false;
    }

    size_t topologocal_index = 0;

    // Phase 1: Seed the queue with all initial vertices that have an in-order of zero.
    for (size_t index = 0; index < number_of_vertices; index++)
    {
        if (graph->in_degree[index] == 0)
        {
            queue[rear++] = index;
        }
    }

    // Phase 2: Core processing evaluation loop.
    while (front < rear)
    {
        // Pull the next independent vertex out of the queue structure.
        size_t item = queue[front++];

        // Record the extracted vertex into the output sorting path context.
        topologocal_order[topologocal_index++] = item;

        // Traverse all target adjacent connections of the extracted node.
        Node* temp = graph->adjacency_lists[item];

        while (temp != NULL)
        {
            size_t neighbor = temp->destination;

            // Ensure internal graph structure have broken indices bounds.
            if (neighbor >= number_of_vertices)
            {
                free(queue);
                free(topologocal_order);

                return false;
            }

            // Remove dependecy edge from parent node.
            if (graph->in_degree[neighbor] > 0)
            {
                graph->in_degree[neighbor]--;
            }

            // If the neighbor node has dropped down to 0 input dependencies, move it into the queue.
            if (graph->in_degree[neighbor] == 0)
            {
                queue[rear++] = neighbor;
            }

            temp = temp->next;
        }
    }

    bool success = true;

    // Phase 3: Checking tracking metrics vs total vertices to see if any cycles stopped the code.
    if (topologocal_index != number_of_vertices)
    {
        success = false;
    }
    else
    {
        // Display the graph.
        printf("%s", "Topological order: ");

        for (size_t index = 0; index < number_of_vertices; index++)
        {
            printf("%zu ", topologocal_order[index]);
        }

        printf("%c", '\n');
    }

    // Free the heap.
    free(queue);
    free(topologocal_order);

    return success;
}

/**
 * @brief Traverses the entire graph and fully releases all associated heap memory.
 * @param graph Pointer to the Graph structure to clean.
 */
void clean(Graph* graph)
{
    // Guard against clearing unallocated context.
    if (graph == NULL)
    {
        return;
    }

    // Free linked lists associated with each array slot indeex.
    for (size_t index = 0; index < graph->number_of_vertices; index++)
    {
        Node* temp = graph->adjacency_lists[index];

        while (temp != NULL)
        {
            Node* free_me = temp;

            temp = temp->next;

            free(free_me);
        }
    }

    // Free structural metadata arrays and the managing structure wrapper.
    free(graph->adjacency_lists);
    free(graph->in_degree);
    free(graph);
}