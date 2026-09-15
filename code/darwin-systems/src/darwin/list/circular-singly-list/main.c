/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Demonstration of a Radial Singly Linked List system.
 * @details     :   This program showcases a cursor-based circular structure
 * supporting O(1) insertions, deletions, and rotational 
 * traversal with automated head tracking.
 * Compile:
 * clang ./src/darwin/list/circular-singly-list/main.c ./src/darwin/list/circular-singly-list/circular_singly_linked_list.c -I./include -o ./bin/circular_singly_linked_list_test
 * @version     :   1.0
 * @date        :   2026-05-13
 */
#include <stdio.h>

// Include header files.
#include "darwin/list/circular_singly_linked_list.h"

/**
 * @brief Callback used with orbit_map to simulate energy decay.
 */
static inline void decay_energy(int* value)
{
    if (value != NULL)
    {
        *value -= 50;
    }
}

int main(void)
{
    printf("%s", "-- System Ignition --\n");

    // Ignite the system.
    RadialSystem* system = ignite();

    if (system == NULL)
    {
        fprintf(stderr, "Out of memory.\n");

        return 1;
    }

    printf("%s", "Expanding orbits with energy levels: 100, 200, 300, 400\n");

    printf("%c", '\n');

    // Expand orbits.
    expand_orbit(system, 100);
    expand_orbit(system, 200);
    expand_orbit(system, 300);
    expand_orbit(system, 400);

    printf("%s", "Current system state: ");

    // Broadcast current state.
    broadcast(system);

    printf("%c", '\n');

    printf("%s", "Rotating system by 2 steps...\n\n");

    // Rotate 2 steps.
    rotate(system, 2);

    printf("%s", "State after rotation: ");

    // Verify cursor shift.
    printf("%s %i%c", "Cursor:", peek(system), '\n');

    printf("%c", '\n');

    printf("%s", "Initiating Energy Decay...\n\n");

    // Apply functional mapping.
    orbit_map(system, decay_energy);

    // Broadcast after rotation.
    broadcast(system);

    printf("%c", '\n');

    printf("%s", "Collapsing the node following the cursor (300)...\n\n");

    // Collapse next.
    collapse_next(system);

    printf("%s", "State after collapse: ");

    // Broadcast after the collapse.
    broadcast(system);

    printf("%c", '\n');

    printf("%s", "Rotating by 10 steps (Module safety check)...\n\n");

    // Rotate 10 steps.
    rotate(system, 10);

    printf("%s", "Final state before shutdown: ");

    // Final broadcast.
    broadcast(system);

    printf("%c", '\n');

    printf("%s", "-- System Shutdown --\n");

    // Clean up.
    shutdown(&system);

    if (system == NULL)
    {
        printf("%s", "System successfully deallocated and pointer nullified.\n");
    }

    return 0;
}