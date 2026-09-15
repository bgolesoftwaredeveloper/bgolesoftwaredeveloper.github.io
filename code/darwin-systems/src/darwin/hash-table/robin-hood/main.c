/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   File for a single-threaded Robin Hood Hash Table.
 * @details     :   Validates the core operations of the Robin Hood hash table 
 * by bulk inserting structured test elements, provoking an
 * automatic table resize, displaying internal bucket states 
 * (verifying Probe Sequence Lengths), performing lookups, 
 * and handling clean memory deallocation.
 * Compile:
 * clang ./src/darwin/hash-table/robin-hood/main.c ./src/darwin/hash-table/robin-hood/robin_hood.c -I./include -o ./bin/robin_hood_test
 * @version     :   1.0
 * @date        :   2026-05-26
 */
#include <stdio.h>

// Include header files.
#include "darwin/hash-table/robin_hood.h"

int main(void)
{
    // Initialize the Robin Hood map container and allocate the initial structure.
    RobinHood* map = initialize_map();

    if (map == NULL)
    {
        fprintf(stderr, "Out of memory.\n");

        return 1;
    }

    // Static array of key-value associations used to bulk populate.
    KeyValuePair items[] =
    {
        {"Alpha", 100},
        {"Beta", 200},
        {"Gamma", 300},
        {"Delta", 400},
        {"Epsilon", 500},
        {"Zeta", 600},
        {"Eta", 700}
    };

    // Calculate the number of items.
    size_t number_of_items = sizeof(items) / sizeof(items[0]);

    // Bulk insertion: Populate the hash table using a structured loop.
    for (size_t index = 0; index < number_of_items; index++)
    {
        printf("%s {%s, %i}%s", "Inserting pair:", items[index].key, items[index].value, "...\n");

        // The 'put' operation internally monitors the load factor threshold.
        if (!put(map, items[index].key, items[index].value))
        {
            fprintf(stderr, "Failed to insert key.\n");

            return 1;
        }
    }

    printf("%c", '\n');

    // Show the values inserted.
    display(map);

    // Retrieve every inserted item to validate lookups.
    for (size_t index = 0; index < number_of_items; index++)
    {
        int retrieved_value = 0;

        if (get(map, items[index].key, &retrieved_value))
        {
            printf("%s %s => %i (%s, %i)%c", "Verified:", 
                items[index].key, 
                retrieved_value, 
                "Expected", items[index].value, '\n');
        }
        else
        {
            printf("%s", "Could not retrieve key.\n");
        }
    }

    printf("%c", '\n');

    // Clean up the deeep internal key strings and the map bucket infrastructure.
    clean(map);

    // Avoid dangling pointers.
    map = NULL;

    return 0;
}