/**
 * @file        :   circular_doubly_linked_list.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the Circular Doubly Linked List (CDLL) logic.
 * @details     :   This module provides the functional logic for managing a 
 * "Galaxy" container. It handles bidirectional pointer arithmetic 
 * to ensure that the list remains circular at all times, supporting 
 * rotational traversal and self-referencing nodes.
 * @version     :   1.0
 * @date        :   2026-05-14
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/list/circular_doubly_linked_list.h"

/**
 * @brief Initializes a new Galaxy controller on the heap.
 * @return A pointer to the initialized Galaxy, or NULL if allocation fails.
 */
Galaxy* new_galaxy(void)
{
    // Allocate memory for the central management structure.
    Galaxy* galaxy = (Galaxy*)malloc(sizeof(Galaxy));

    if (galaxy == NULL)
    {
        return NULL;
    }

    // Initialize the galaxy as an empty void.
    galaxy->population_count = 0;
    galaxy->sentinel = NULL;

    return galaxy;
}

/**
 * @brief Creates a standalone OrbitalNode with self-referencing links.
 * @param value The rhythmic data to be stored in the node.
 * @return A pointer to the new node, or NULL if allocation fails.
 */
static inline OrbitalNode* new_orbital_node(int value)
{
    // Allocate memory for a singly orbital unit.
    OrbitalNode* node = (OrbitalNode*)malloc(sizeof(OrbitalNode));

    if (node == NULL)
    {
        return NULL;
    }

    // Set the data and establish self-circularity for a single-node state.
    node->rhythmic_data = value;
    node->forward_link = node;
    node->backward_link = node;

    return node;
}

/**
 * @brief Integrates a new value into the Galaxy by appending it to the tail.
 * @param galaxy The container managing the orbital sequence.
 * @param value The rhythmic data to append.
 */
void append(Galaxy* galaxy, int value)
{
    // Safety check for valid container.
    if (galaxy == NULL)
    {
        return;
    }

    // Prepare the new node for insertion.
    OrbitalNode* node = new_orbital_node(value);

    if (node == NULL)
    {
        return;
    }

    // If the galaxy is empty, the new node becomes the primary sentinel.
    if (galaxy->sentinel == NULL)
    {
        galaxy->sentinel = node;
    }
    else
    {
        // Identify the current tail via the sentinel's backward link.
        OrbitalNode* tail = galaxy->sentinel->backward_link;

        // Link the new node forward to teh head and backward to the tail.
        node->forward_link = galaxy->sentinel;
        node->backward_link = tail;

        // Bridge the tail forward to the new node.
        tail->forward_link = node;

        // Close the circle by pointing the head backward to the new tail.
        galaxy->sentinel->backward_link = node;
    }

    // Increment the total count of nodes in the system.
    galaxy->population_count++;
}

/**
 * @brief Traverses the Galaxy to find a node containing specific rhythmic data.
 * @param galaxy The container to search through.
 * @param value The target data to locate.
 * @return Pointer to the located OrbitalNode, or NULL if not found.
 */
OrbitalNode* find(const Galaxy* galaxy, int value)
{
    // Return NULL if the galaxy is non-existent or empty.
    if (galaxy == NULL || galaxy->sentinel == NULL)
    {
        return NULL;
    }

    // Begin traversal at the sentinel position.
    OrbitalNode* current = galaxy->sentinel;

    // Iterate exactly once through the population count to avoid infinite loops.
    for (size_t index = 0; index < galaxy->population_count; index++)
    {
        // Check if the current node holds the target payload.
        if (current->rhythmic_data == value)
        {
            return current;
        }

        // Advance to the next node in the orbital sequence.
        current = current->forward_link;

        // Defensive check against broken links.
        if (current == NULL)
        {
            break;
        }
    }

    return NULL;
}

/**
 * @brief Updates the data payload of an existing node within the Galaxy.
 * @param galaxy The container holding the nodes.
 * @param value The current value to search for.
 * @param new_value The new data to assign to the node.
 * @return True if the update was successful, false otherwise.
 */
bool update(Galaxy* galaxy, int value, int new_value)
{
    // Validate that the galaxy contains data.
    if (galaxy == NULL || galaxy->sentinel == NULL)
    {
        return false;
    }

    // Locate the specific node requireing an uppdate.
    OrbitalNode* node = find(galaxy, value);

    // If located, modify the rhythmic data and return success.
    if (node != NULL)
    {
        node->rhythmic_data = new_value;

        return true;
    }

    return false;
}

/**
 * @brief Removes an OrbitalNode from the Galaxy and repairs the broken links.
 * @param galaxy The container from which to erase the node.
 * @param value The target data payload to identify for removal.
 * @return True if the node was found and erased, false otherwise.
 */
bool erase(Galaxy* galaxy, int value)
{
    // Validate galaxy existence and content.
    if (galaxy == NULL || galaxy->sentinel == NULL)
    {
        return false;
    }

    // Search for the node to be removed.
    OrbitalNode* current = find(galaxy, value);

    // If the node is not in the system, exit.
    if (current == NULL)
    {
        return false;
    }

    // If the node is not in the system, exit.
    if (galaxy->population_count == 1)
    {
        galaxy->sentinel = NULL;
    }
    else
    {
        // Capture the neighboring nodes to bridge the gap.
        OrbitalNode* previous = current->backward_link;
        OrbitalNode* next = current->forward_link;

        // Re-link neighbors to bypass the current node.
        if (previous != NULL && next != NULL)
        {
            previous->forward_link = next;
            next->backward_link = previous;

            // If the sentinel was the target, shift it to the next available node.
            if (galaxy->sentinel == current)
            {
                galaxy->sentinel = next;
            }
        }
    }

    // Wipe the node from memory.
    free(current);

    galaxy->population_count--;

    return true;
}

/**
 * @brief Prints the sequence of nodes and confirms circularity back to the sentinel.
 * @param galaxy The container to visualize.
 */
void display(const Galaxy* galaxy)
{
    // Exit if there us nothing to display.
    if (galaxy == NULL || galaxy->sentinel == NULL)
    {
        return;
    }

    OrbitalNode* current = galaxy->sentinel;

    // Report the total population count.
    printf("%s %zu%c", "Nodes:", galaxy->population_count, '\n');

    size_t index = 0;
    
    do
    {
        // Output the rhythmic data of the current node.
        printf("%i", current->rhythmic_data);

        // Print spacers between nodes except after the last one.
        if (index < galaxy->population_count - 1)
        {
            printf("%s", " <-> ");
        }

        // Rotate to the next node.
        current = current->forward_link;
        index++;
    } while (current != galaxy->sentinel);
    
    // Finalize the output by showing the return to the start.
    printf("%s %i%c", "\tBack to start:", galaxy->sentinel->rhythmic_data, '\n');
}

/**
 * @brief Deallocates all nodes within the Galaxy and resets the container state.
 * @param galaxy The container to empty.
 */
void clear(Galaxy* galaxy)
{
    // Check if the galaxy is already empty.
    if (galaxy == NULL || galaxy->sentinel == NULL)
    {
        return;
    }

    OrbitalNode* current = galaxy->sentinel;

    size_t count = galaxy->population_count;

    // Traverse and free each node individually.
    for (size_t index = 0; index < count; index++)
    {
        OrbitalNode* temp = current;

        // Advance the pointer before freeing the current memory.
        current = current->forward_link;

        free(temp);
    }

    // Reset the management structure to an empty state.
    galaxy->sentinel = NULL;
    galaxy->population_count = 0;
}