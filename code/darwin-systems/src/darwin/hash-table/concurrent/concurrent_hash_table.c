/**
 * @file        :   concurrent_hash_table.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of a lock-striped concurrent hash table.
 * @details     :   This module provides a high-throughput, thread-safe hash table using
 * a combination of POSIX mutex striping and C11 atomic operations. Key features include:
 * - Fine-grained concurrency via split lock-stripes (NUMBER_OF_STRIPES).
 * - Lock-free lookups using atomic load operations with acquire-release semantics.
 * - Uniform key distribution using the Jenkins One-at-a-time hash algorithm.
 * - Dynamic structural snapshotting with synchronized all-stripe locking.
 * - Robust error handling with initialization rollbacks and deep-clean deallocation.
 * @version     :   1.0
 * @date        :   2026-05-25
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/hash-table/concurrent_hash_table.h"

/**
 * @brief       :   Generates a 32-bit hash value for a given string key.
 * @details     :   Uses the Jenkins One-at-a-time hash algorithm to provide uniform
 *                  distribution and minimize collision rates.
 * @param       :   key: The null-terminated string key to hash.
 * @return      :   uint32_t: The computed 32-bit hash value.
 */
static inline uint32_t calculate_jenkins(const char* key)
{
    // Initialize the hash accumulation state variables.
    uint32_t hash_value = 0;

    // Walk through the character array sequentially until hitting the null terminator.
    while (*key)
    {
        hash_value += (uint32_t)*key;
        hash_value += (hash_value << 10);
        hash_value ^= (hash_value >> 6);

        key++;
    }

    // Perform final avalanche mixing steps to ensure even distribution of high and low order bits.
    hash_value += (hash_value << 3);
    hash_value ^= (hash_value >> 11);
    hash_value += (hash_value << 15);

    return hash_value;
}

/**
 * @brief       :   Maps a string key to a specific hash table bucket index.
 * @param       :   key: The null-terminated string key.
 * @return      :   uint32_t: The bucket index bounded by NUMBER_OF_BUCKETS.
 */
static inline uint32_t retrieve_bucket_index(const char* key)
{
    return calculate_jenkins(key) % NUMBER_OF_BUCKETS;
}

/**
 * @brief       :   Maps a bucket index to its corresponding synchronization lock stripe.
 * @param       :   bucket_index: The calculated target bucket index.
 * @return      :   uint32_t: The lock stripe index bounded by NUMBER_OF_STRIPES.
 */
static inline uint32_t retrieve_stripe_index(uint32_t bucket_index)
{
    return bucket_index % NUMBER_OF_STRIPES;
}

/**
 * @brief       :   Simulates concurrent load processing within a worker thread.
 * @details     :   Executes a structured loop sequence of interleaved insertions, 
 *                  lookups, payload modifications, and complete item obliterations.
 * @param       :   argument: A pointer to a ThreadPayload structure containing task context.
 * @return      :   void*: Always returns NULL upon completion.
 */
void* worker_thread_task(void* argument)
{
    // Extract thread configuration and execution reference contexts.
    ThreadPayload* payload = (ThreadPayload*)argument;

    // Guard against malformed tracking state blocks or missing tables.
    if (payload == NULL || payload->table == NULL)
    {
        return NULL;
    }

    ConcurrentHashTable* table = payload->table;

    uint32_t id = payload->id;

    // Populate unique keys concurrently across multiple threads.
    for (uint32_t action_index = 0; action_index < OPERATIONS_PER_THREAD; action_index++)
    {
        // Allocate temporary tracking heap space for the formatted string.
        char* key = (char*)malloc(sizeof(char) * 64);

        if (key == NULL)
        {
            continue;
        }

        // Build a deterministic key name unique to this thread identity and index combination.
        snprintf(key, 64, "Key-Thread%u-Index%u", id, action_index);

        // Derive an arbitrary tracking payload integer mapping to pass as data.
        intptr_t value = (intptr_t)(action_index * 100);

        // Commit entry to the map.
        insert(table, key, (void*)value);

        // Discard local stack buffer frame string to avoid.
        free(key);
    }

    // Verify persistence and execute atomic modificatioins over active records.
    for (uint32_t action_index = 0; action_index < OPERATIONS_PER_THREAD; action_index++)
    {
        char buffer[64];

        // Regenerate identical target lookup keys using local stack buffers.
        snprintf(buffer, sizeof(buffer), "Key-Thread%u-Index%u", id, action_index);

        // Query key matching locks-free using atomic acquire read-paths.
        void* retrieved_value = lookup(table, buffer);

        // If found, update its mapping to test read-to-write update transistions.
        if (retrieved_value != NULL)
        {
            intptr_t updated_value = ((intptr_t)retrieved_value) + 5;

            insert(table, buffer, (void*)updated_value);
        }
    }

    // Erase records to challenge table shrinking sequences across lock lanes.
    for (uint32_t action_index = 0; action_index < OPERATIONS_PER_THREAD; action_index++)
    {
        char buffer[64];

        snprintf(buffer, sizeof(buffer), "Key-Thread%u-Index%u", id, action_index);

        // Extract node allocations and clean up matching entries safely.
        erase(table, buffer);
    }

    return NULL;
}

/**
 * @brief       :   Allocates and initializes a new concurrent striped hash table.
 * @details     :   Zero-allocates memory for the table manager and initializes a 
 *                  segmented pool of mutual exclusion locks. Performs recursive rollbacks 
 *                  to clean up partially allocated mutexes if initialization fails mid-way.
 * @return      :   ConcurrentHashTable*: Pointer to the allocated structure, or NULL if it fails.
 */
ConcurrentHashTable* initialize(void)
{
    // Zero-initialize structural backing to ensure bucket pointers are initially explicitly NULL.
    ConcurrentHashTable* table = (ConcurrentHashTable*)calloc(1, sizeof(ConcurrentHashTable));

    if (table == NULL)
    {
        return NULL;
    }

    // Sequentially initialize internal mutex boundaries separating collision zones.
    for (uint32_t stripe_index = 0; stripe_index < NUMBER_OF_STRIPES; stripe_index++)
    {
        if (pthread_mutex_init(&table->strip_locks[stripe_index], NULL))
        {
            // Rollback and completely tear down any successfully initialized mutexes prior failure.
            for (uint32_t rollback_index = 0; rollback_index < stripe_index; rollback_index++)
            {
                pthread_mutex_destroy(&table->strip_locks[rollback_index]);
            }

            // Release core manager container memory context block.
            free(table);

            return NULL;
        }
    }

    return table;
}

/**
 * @brief       :   Inserts a new key-value pair or updates an existing key's value.
 * @details     :   Acquires a fine-grained stripe lock based on the mapped bucket index. 
 *                  Traverses the bucket chain atomically; if the key exists, updates its value.
 *                  Otherwise, prepends a new node to the bucket head using release memory semantics.
 * @param       :   table: The concurrent hash table instance.
 * @param       :   key: The unique string key associated with the entry.
 * @param       :   value: Universal pointer payload to store.
 * @return      :   bool: true if insertion/update succeeded, false on allocation failure.
 */
bool insert(ConcurrentHashTable* table, const char* key, void* value)
{
    // Disallow operations using undefined container or empty lookup key structures.
    if (table == NULL || key == NULL)
    {
        return false;
    }

    // Determine target location boundaries across the matrix.
    uint32_t bucket_index = retrieve_bucket_index(key);
    uint32_t stripe_index = retrieve_stripe_index(bucket_index);

    // Lock down the corresponding data stripe to prevent concurrent write modifications.
    pthread_mutex_lock(&table->strip_locks[stripe_index]);

    // Use aquire memory sematics to obtain the current bucket head chain visibility securely.
    Node* current = atomic_load_explicit(&table->buckets[bucket_index], memory_order_acquire);

    // Traverse the collision chain to check if the target key already exists.
    while (current != NULL)
    {
        if (strcmp(current->key, key) == 0)
        {
            // Key match discovered: Update the value payload inside the existing node.
            current->value = value;
            pthread_mutex_unlock(&table->strip_locks[stripe_index]);

            return true;
        }

        // Advance to the next element in the chain using an atomic load operation.
        current = atomic_load_explicit(&current->next, memory_order_acquire);
    }

    // Key was not found: Allocate a new node structure container on the heap.
    Node* new_node = (Node*)malloc(sizeof(Node));

    if (new_node == NULL)
    {
        pthread_mutex_unlock(&table->strip_locks[stripe_index]);

        return false;
    }

    // Duplicate string key explicitly to prevent out-of-scope.
    new_node->key = strdup(key);
    new_node->value = value;

    // Capture the existing head entry without strict serialization synchronization constraints.
    Node* old_head = atomic_load_explicit(&table->buckets[bucket_index], memory_order_relaxed);

    // Link our new node next pointer to point to the current head of the bucket chain.
    atomic_store_explicit(&new_node->next, old_head, memory_order_relaxed);

    // Swap the bucket head pointer with ournew node using release meomry order sematics.
    atomic_store_explicit(&table->buckets[bucket_index], new_node, memory_order_release);

    // Release the fine-grained stripe lock to open up access for other threads.
    pthread_mutex_unlock(&table->strip_locks[stripe_index]);

    return true;
}

/**
 * @brief       :   Queries the table for a key and returns its associated value payload.
 * @details     :   Performs completely lock-free lookups using atomic loads with acquire memory 
 *                  semantics, protecting the reader thread from experiencing pipeline stall bottlenecks.
 * @param       :   table: Read-only reference pointer to the concurrent hash table.
 * @param       :   key: The unique string key to look up.
 * @return      :   void*: Pointer to the stored payload if found, or NULL if not found.
 */
void* lookup(const ConcurrentHashTable* table, const char* key)
{
    if (table == NULL || key == NULL)
    {
        return NULL;
    }

    // Isolate target lane without locking any stripe mutex modules.
    uint32_t bucket_index = retrieve_bucket_index(key);

    // Atomically load bucket chain head with acquire sematics to safely read writer modifications.
    Node* current = atomic_load_explicit(&table->buckets[bucket_index], memory_order_acquire);

    // Scan down the list safely while writers concurrently modify link references.
    while (current != NULL)
    {
        if (strcmp(current->key, key) == 0)
        {
            return current->value;
        }

        // Advance downstream using an explicit atomic load synchronization link pattern.
        current = atomic_load_explicit(&current->next, memory_order_acquire);
    }

    return NULL;
}

/**
 * @brief       :   Removes a specific key-value node from the table.
 * @details     :   Acquires the corresponding stripe lock, unlinks the target node 
 *                  from the bucket list chain, and deallocates both the node and its duplicated key string.
 * @param       :   table: The concurrent hash table instance.
 * @param       :   key: The string key identifier to purge.
 * @return      :   bool: true if the key was found and successfully removed, false otherwise.
 */
bool erase(ConcurrentHashTable* table, const char* key)
{
    if (table == NULL || key == NULL)
    {
        return false;
    }

    uint32_t bucket_index = retrieve_bucket_index(key);
    uint32_t stripe_index = retrieve_stripe_index(bucket_index);

    // Serialize node changes under the corresponding lane stripe lock.
    pthread_mutex_lock(&table->strip_locks[stripe_index]);

    // Use relaxed visibility ordering since access is isolated under the mutex lock.
    Node* current = atomic_load_explicit(&table->buckets[bucket_index], memory_order_relaxed);
    Node* previous = NULL;

    // Traverse the chain while tracking the trailing parent references pointer node.
    while (current != NULL)
    {
        if (strcmp(current->key, key) == 0)
        {
            // Capture the next element link ahead of the item being dropped.
            Node* next = atomic_load_explicit(&current->next, memory_order_relaxed);

            if (previous == NULL)
            {
                // Unlink matching node from the bucket head entry using release memory sematics.
                atomic_store_explicit(&table->buckets[bucket_index], next, memory_order_release);
            }
            else
            {
                // Unlink matching node from inside the chain by joining the parent to the child link.
                atomic_store_explicit(&previous->next, next, memory_order_release);
            }

            // Free the duplicate key string heap allocation.
            free(current->key);
            free(current);

            pthread_mutex_unlock(&table->strip_locks[stripe_index]);

            return true;
        }

        // Shif parents up and step forward along the trailing node tracking sequence path.
        previous = current;
        current = atomic_load_explicit(&current->next, memory_order_relaxed);
    }

    pthread_mutex_unlock(&table->strip_locks[stripe_index]);

    return false;
}

/**
 * @brief       :   Dumps a consistent visual map of the table layout to standard output.
 * @details     :   Acquires all stripe locks in ascending order to freeze the state of 
 *                  the table, preventing writer contamination while scanning the buckets.
 * @param       :   table: The concurrent hash table instance to inspect.
 * @return      :   void
 */
void display(ConcurrentHashTable* table)
{
    if (table == NULL)
    {
        return;
    }

    // Cascade locks in ascending index order to avoid deadlocks.
    for (uint32_t stripe_index = 0; stripe_index < NUMBER_OF_STRIPES; stripe_index++)
    {
        pthread_mutex_lock((pthread_mutex_t*)&table->strip_locks[stripe_index]);
    }

    printf("%s", "-- Concurrent Hash table --\n");

    bool is_empty = true;

    // Scan buckets sequentially while the table state is frozen.
    for (uint32_t bucket_index = 0; bucket_index < NUMBER_OF_BUCKETS; bucket_index++)
    {
        Node* current = atomic_load_explicit(&table->buckets[bucket_index], memory_order_relaxed);

        if (current != NULL)
        {
            is_empty = false;

            printf("%s %u", "Bucket:", bucket_index);

            // Traverse and display each node collision link sequence layout explicitly.
            while (current != NULL)
            {
                printf("{ Key \"%s\", Value %p}", current->key, current->value);

                current = atomic_load_explicit(&current->next, memory_order_relaxed);

                if (current != NULL)
                {
                    printf("%s", " -> ");
                }
            }

            printf("%c", '\n');
        }
    }

    if (is_empty)
    {
        printf("%s", "The hash table is empty.\n");
    }

    // Relase stripe locks in reverse order to unwind the cascade safely.
    for (int32_t stripe_index = NUMBER_OF_STRIPES - 1; stripe_index >= 0; stripe_index--)
    {
        pthread_mutex_unlock((pthread_mutex_t*)&table->strip_locks[stripe_index]);
    }
}

/**
 * @brief       :   Completely dismantles and deallocates the concurrent hash table.
 * @details     :   Locks every stripe to isolate memory, loops over all bucket indices 
 *                  to systematically clear active memory chains, destroys the underlying 
 *                  mutex pool, and clears the top-level table structure.
 * @param       :   table: The concurrent hash table instance to destroy.
 * @return      :   void
 */
void destroy(ConcurrentHashTable* table)
{
    if (table == NULL)
    {
        return;
    }

    // Freeze all operations across all stripes prior to starting memory.
    for (uint32_t stripe_index = 0; stripe_index < NUMBER_OF_STRIPES; stripe_index++)
    {
        pthread_mutex_lock(&table->strip_locks[stripe_index]);
    }

    // Reclaim all remaining heap-allocated chain items across every bucket layer.
    for (uint32_t bucket_index = 0; bucket_index < NUMBER_OF_BUCKETS; bucket_index++)
    {
        Node* current = atomic_load_explicit(&table->buckets[bucket_index], memory_order_relaxed);

        while (current != NULL)
        {
            Node* temp = current;

            current = atomic_load_explicit(&current->next, memory_order_relaxed);
            
            // Clear allocations.
            free(temp->key);
            free(temp);
        }      
    }

    // Unlock and systematically dismatle underlying POSIX system mutex handles.
    for (uint32_t stripe_index = 0; stripe_index < NUMBER_OF_STRIPES; stripe_index++)
    {
        pthread_mutex_unlock(&table->strip_locks[stripe_index]);
        pthread_mutex_destroy(&table->strip_locks[stripe_index]);
    }

    // Free top-level structure tracking layout context frame block.
    free(table);
}