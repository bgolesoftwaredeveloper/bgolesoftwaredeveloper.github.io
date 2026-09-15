/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Test harness for a C11 lock-striped concurrent hash table.
 * @details     :   This program validates the thread safety and operational correctness of 
 * the hash table implementation. It performs basic single-threaded operations 
 * (insertions, updates, lookups) to establish a baseline, prints the initial state, 
 * spawns a multi-threaded thread pool to simulate a concurrent write-heavy workload, 
 * and verifies consistency with a post-concurrency layout dump.
 * Compile:
 * clang ./src/darwin/hash-table/concurrent/main.c ./src/darwin/hash-table/concurrent/concurrent_hash_table.c -I./include -o ./bin/concurrent_table_test
 * @version     :   1.0
 * @date        :   2026-05-25
 */
#include <stdio.h>

// Include header files.
#include "darwin/hash-table/concurrent_hash_table.h"

int main(void)
{
    // Allocate a new concurrent hash table.
    ConcurrentHashTable* table = initialize();

    if (table == NULL)
    {
        return 1;
    }

    // Insert values to work with.
    insert(table, "Admin", (void*)(intptr_t)1001);
    insert(table, "Guest", (void*)(intptr_t)2002);
    insert(table, "Service", (void*)(intptr_t)3003);

    // Update an existing key.
    insert(table, "Guest", (void*)(intptr_t)2555);

    // Lookup values.
    void* admin_value = lookup(table, "Admin");
    void* guest_value = lookup(table, "Guest");

    // Print the initial snapshot of the table contents to standard output.
    display(table);

    // Set up the POSIX thread pool tracking structures.
    pthread_t pool[NUMBER_OF_THREADS];

    // Array of thread execution arguments providing contextual identity and shared table pointers.
    ThreadPayload payload[NUMBER_OF_THREADS];

    // Spawn the configured cluster of worker threads to execute concurrent workloads.
    for (uint32_t thread_index = 0; thread_index < NUMBER_OF_THREADS; thread_index++)
    {
        payload[thread_index].table = table;
        payload[thread_index].id = thread_index;

        // Pass the precise payload element address so workers don't read unaligned stack allocations.
        if (pthread_create(&pool[thread_index], NULL, worker_thread_task, &payload[thread_index]) != 0)
        {
            fprintf(stderr, "Error failed to spawn thread cluster.\n");

            return 1;
        }
    }

    // Await termination tracking barriers for all operating lanes to achieve synchronous resolution.
    for (uint32_t thread_index = 0; thread_index < NUMBER_OF_THREADS; thread_index++)
    {
        pthread_join(pool[thread_index], NULL);
    }

    // Print the final structure snapshot of the table following concurrent mutation passes.
    display(table);

    // Target a specific thread.
    const char* key = "Key-Thread1-Index3";

    // Query the table for the dynamic worker key.
    void* value = lookup(table, key);

    // Verify if our targeted trace variable survived the multi-threaded routine.
    if (value != NULL)
    {
        printf("%s '%s' %s %ld", "Lookup verification found key", key, "with payload tracking value:", (intptr_t)value);
    }
    else
    {
        printf("%s", "Not found.\n");
    }

    // Safely lock down stripes, purge remaining chain links, and release all table memory structures.
    destroy(table);

    return 0;
}