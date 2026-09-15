/**
 * @file        :   circular_singly_linked_list.h
 * @author      :   Braiden Gole
 * @brief       :   Header definitions for a Radial (Circular) Singly Linked List.
 * @details     :   This file contains the orbital node structures and the 
 * management system for a tail-referenced circular list. 
 * It provides the interface for ignition, expansion, 
 * rotation, and shutdown of the link system.
 * @version     :   1.0
 * @date        :   2026-05-13
 */
#ifndef CIRCULAR_SINGLY_LINKED_LIST_H
#define CIRCULAR_SINGLY_LINKED_LIST_H

#include <stddef.h>

/**
 * @struct SolarNode
 * @brief  The fundamental link in the circular orbit.
 */
typedef struct SolarNode
{
    int energy;
    struct SolarNode* next_orbit;
} SolarNode;

/**
 * @struct RadialSystem
 * @brief  The control structure for managing the circular list.
 */
typedef struct
{
    SolarNode* cursor;
    size_t count;
} RadialSystem;

// Function prototypes.
RadialSystem* ignite(void);
void expand_orbit(RadialSystem* system, int value);
void collapse_next(RadialSystem* system);
void rotate(RadialSystem* system, int steps);
void orbit_map(RadialSystem* system, void (*callback)(int* value));
int peek(const RadialSystem* system);
void broadcast(const RadialSystem* system);
void shutdown(RadialSystem** system);

#endif