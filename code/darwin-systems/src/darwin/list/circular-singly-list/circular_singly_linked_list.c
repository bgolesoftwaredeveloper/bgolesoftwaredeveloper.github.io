/**
 * @file        :   circular_singly_linked_list.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of a Radial (Circular) Singly Linked List.
 * @details     :   This system utilizes a cursor-based approach to manage a 
 * circular loop of nodes. By tracking the "tail" (cursor), 
 * the system provides O(1) access to both the beginning 
 * and end of the orbit.
 * @version     :   1.0
 * @date        :   2026-05-13
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Include headder files.
#include "darwin/list/circular_singly_linked_list.h"

/**
 * @brief Performs a defensive null-check on the management structure.
 * @param system Pointer to the RadialSystem to validate.
 * @return true if the system is initialized, false otherwise.
 */
static inline bool is_system_valid(const RadialSystem* system)
{
    return (system != NULL);
}

/**
 * @brief Allocates and initializes the RadialSystem management structure.
 * @return A pointer to the new RadialSystem, or NULL on allocation failure.
 */
RadialSystem* ignite(void)
{
    // Allocate the container that tracks the cursor and node count.
    RadialSystem* system = (RadialSystem*)malloc(sizeof(RadialSystem));

    if (system == NULL)
    {
        return NULL;
    }

    // Initialize to a neutral/empty state.
    system->cursor = NULL;
    system->count = 0;

    return system;
}

/**
 * @brief Integrates a new node into the circular orbit.
 * @param system Pointer to the management structure.
 * @param value The energy level (data) to store in the node.
 */
void expand_orbit(RadialSystem* system, int value)
{
    if (!is_system_valid(system))
    {
        return;
    }

    // Create a new orbital unit.
    SolarNode* new_node = (SolarNode*)malloc(sizeof(SolarNode));

    if (new_node == NULL)
    {
        return;
    }

    new_node->energy = value;

    if (system->cursor == NULL)
    {
        // Initial node creation: Node must point to itself to maintain circularity.
        new_node->next_orbit = new_node;

        system->cursor = new_node;
    }
    else
    {
        // Splicing the node: It points to the head, and the current tail points to it.
        new_node->next_orbit = system->cursor->next_orbit;

        system->cursor->next_orbit = new_node;

        // Move the cursor focus to the newly added node.
        system->cursor = new_node;
    }

    system->count++;
}

/**
 * @brief Deallocates the node immediately following the cursor.
 * @param system Pointer to the management structure.
 */
void collapse_next(RadialSystem* system)
{
    // Verify there is a sytem and at least one node to reomve.
    if (!is_system_valid(system) || system->cursor == NULL || system->count <= 0)
    {
        return;
    }

    // The 'target; is always the node after the cursor.
    SolarNode* target = system->cursor->next_orbit;

    if (system->count == 1)
    {
        // If it's the last node, nullify the cursor to prevent dangling pointers.
        system->cursor = NULL;
    }
    else
    {
        // Standard defense against corrupted pointers.
        if (target == NULL)
        {
            return;
        }

        // Bypass the target node by linking cursor directly to target's successor.
        system->cursor->next_orbit = target->next_orbit;
    }

    // Release memory and update the population count.
    free(target);

    system->count--;
}

/**
 * @brief Shifts the cursor position forward through the orbit.
 * @param system Pointer to the management structure.
 * @param steps Number of nodes to move the cursor forward.
 */
void rotate(RadialSystem* system, int steps)
{
    // Ensure we have a valid system.
    if (!is_system_valid(system) || system->cursor == NULL || steps <= 0)
    {
        return;
    }

    // Optimization: Use modulo to avoid unnecessary full loops around the ring.
    size_t actual_steps = steps % system->count;

    for (size_t index = 0; index < actual_steps; index++)
    {
        system->cursor = system->cursor->next_orbit;
    }
}

/**
 * @brief Applies a user-defined callback function to every node in the orbit.
 * @param system Pointer to the management structure.
 * @param callback A function pointer that receives a pointer to each node's energy value.
 */
void orbit_map(RadialSystem* system, void (*callback)(int* value))
{
    if (!is_system_valid(system) || system->cursor == NULL)
    {
        return;
    }

    SolarNode* scanner = system->cursor->next_orbit;

    for (size_t index = 0; index < system->count; index++)
    {
        callback(&(scanner->energy));

        scanner = scanner->next_orbit;
    }
}

/**
 * @brief Retrieves the energy level of the node currently under the cursor.
 * @param system Pointer to the management structure (read-only).
 * @return The energy value of the cursor node, or -1 if the system is empty or invalid.
 */
int peek(const RadialSystem* system)
{
    return (system && system->cursor != NULL) ? system->cursor->energy : -1;
}

/**
 * @brief Iterates through the entire orbit once and prints node data.
 * @param system Pointer to the management structure.
 */
void broadcast(const RadialSystem* system)
{
    if (!is_system_valid(system) || system->cursor == NULL)
    {
        return;
    }

    // Scan from the first node (cursor's next orbit).
    SolarNode* scanner = system->cursor->next_orbit;

    for (size_t index = 0; index < system->count; index++)
    {
        if (scanner == NULL)
        {
            break;
        }

        printf("[%i]", scanner->energy);

        // Advance to the next link in the ring.
        scanner = scanner->next_orbit;
    }

    printf("%c", '\n');
}

/**
 * @brief Performs a full system teardown, freeing all nodes and the manager.
 * @param system Address of the system pointer to allow for nullification.
 */
void shutdown(RadialSystem** system)
{
    if (system == NULL || *system == NULL)
    {
        return;
    }

    // Capture the local pointer for easier access during the cleanup loop.
    RadialSystem* radial = *system;

    // Incrementally remove each node until the count reaches zero.
    while (radial->count > 0)
    {
        collapse_next(radial);
    }

    // Free the management structure itself.
    free(radial);

    // Set the original pointer to NULL to prevent use-after-free.
    *system = NULL;
}