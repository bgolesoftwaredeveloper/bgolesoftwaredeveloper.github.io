/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of an advanced Circular Doubly Linked List (CDLL).
 * @details     :   This program demonstrates a "Galaxy" container managing "OrbitalNodes."
 * The system utilizes a circular doubly linked architecture where:
 * - The 'sentinel' node acts as the entry point.
 * - Every node has bidirectional links (forward/backward).
 * - The tail node's forward link points to the sentinel.
 * - The sentinel's backward link points to the tail.
 * Supports O(1) appending and O(n) search, update, and erasure.
 * Compile:
 * clang ./src/darwin/list/circular-doubly-list/main.c ./src/darwin/list/circular-doubly-list/circular_doubly_linked_list.c -I./include -o ./bin/circular_doubly_linked_list_test
 * @version     :   1.0
 * @date        :   2026-05-14
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/list/circular_doubly_linked_list.h"

int main(void)
{
    // Initialize the galaxy structure.
    Galaxy* galaxy = new_galaxy();

    if (galaxy == NULL)
    {
        fprintf(stderr, "Out of memory.\n");

        return 1;
    }

    printf("%s", "-- Initializing Galaxy --\n");

    // Populate the galaxy/
    append(galaxy, 10);
    append(galaxy, 20);
    append(galaxy, 30);
    append(galaxy, 40);

    // Print the current state to verify initial population and circularity.
    display(galaxy);

    printf("%c", '\n');

    printf("%s", "-- Searching for Node 30 --\n");

    // Perform an O(n) search to find the pointer to a specific node.)
    OrbitalNode* result = find(galaxy, 30);

    // Verify if the node exists before attempting to access its data members.
    if (result != NULL)
    {
        printf("%s %i%c", "Data:", result->rhythmic_data, '\n');
    }
    else
    {
        printf("%s", "Node 30 not found in the galaxy.\n");
    }

    printf("%c", '\n');

    printf("%s", "-- Updating Node 20 to 25 --\n");

    // We check the boolean return value to confirm the operation.
    if (update(galaxy, 20, 25))
    {
        printf("%c", '\n');
        printf("%s", "Updated node 20 to 25!\n");
    }

    display(galaxy);

    printf("%c", '\n');

    printf("%s", "-- Erasing Node 30 --\n");

    // Erase handles the unlinking of node 30.
    if (erase(galaxy, 30))
    {
        printf("%c", '\n');
        printf("%s", "Erased node 30!\n");
    }

    display(galaxy);

    printf("%c", '\n');

    printf("%s", "-- Re-searchign for Node 30 --\n");

    // Re-verify that the node is truly gone from the memory structure.
    result = find(galaxy, 30);

    if (result == NULL)
    {
        printf("%s", "Confirmed: Node 30 is no longer in the galaxy.\n");
    }

    printf("%c", '\n');

    printf("%s", "-- Clearing Galaxy --\n");

    // Clear the galaxy.
    clear(galaxy);

    free(galaxy);

    printf("%s", "Galaxy container destroyed.\n");

    return 0;
}