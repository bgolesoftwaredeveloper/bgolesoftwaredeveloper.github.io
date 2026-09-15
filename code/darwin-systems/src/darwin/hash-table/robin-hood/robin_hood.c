/**
 * @file        :   robin_hood.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of a single-threaded Robin Hood Hash Table.
 * @details     :   This module provides a cache-conscious, open-addressed hash table
 * using linear probing optimized by the Robin Hood displacement algorithm. Features include:
 * - Localized element packed arrays minimizing CPU cache line fetch degradation.
 * - Reduced lookup variance via "take from the rich, give to the poor" insertion shifts.
 * - High performance 404/Not-Found early exit execution branching on lookups.
 * - Tombstone-free backward-shift item erasure to maintain structured packed sequences.
 * - Dynamic capacity scaling paired with defensive allocation tracking checks.
 * @version     :   1.0
 * @date        :   2026-05-26
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include header files.
#include "darwin/hash-table/robin_hood.h"

/**
 * @brief       :   Generates a 32-bit hash value for a given string key.
 * @details     :   Implements a lightweight bitwise multiplicative mixer to maximize
 *                  distribution uniformity and prevent 0 evaluations reserved for empty slots.
 * @param       :   key: The null-terminated string key to hash.
 * @return      :   unsigned int: The computed 32-bit hash value.
 */
static inline unsigned int hash(const char* key)
{
    if (key == NULL)
    {
        return UNINITIALIZED_HASH;
    }


    unsigned int hash_value = 0;

    // Process each character of the string via continueous bit.
    while (*key)
    {
        hash_value ^= (unsigned char)*key++;
        hash_value *= 0x5bd1e995;
        hash_value ^= hash_value << 15;
    }

    // Explicitly reserve 0 as a flag for uninitialized map metadata.
    return hash_value == UNINITIALIZED_HASH ? 1 : hash_value;
}

/**
 * @brief       :   Low-level internal insertion logic handling the Robin Hood placement rules.
 * @details     :   Probes the flat array sequentially. If an empty slot is encountered, the item 
 *                  is dropped. If a key duplicate is matched, the value is updated. If a slot's resident
 *                  is "richer" (lower probe length) than the current element, a swap occurs, and the
 *                  displaced element continues probing down the line.
 * @param       :   buckets: Pointer to the destination flat Entry array.
 * @param       :   capacity: The maximum allocation layout ceiling of the destination array.
 * @param       :   entry: The structural Entry data object currently being inserted.
 * @return      :   bool: true if insertion or collision processing succeeded, false on validation error.
 */
static inline bool raw_insert(Entry* buckets, size_t capacity, Entry entry)
{
    // Guard to protect structural operations.
    if (buckets == NULL || entry.key == NULL)
    {
        return false;
    }

    // Compute the ideal array position using standard methematical wrapping.
    size_t index = (size_t)(entry.hash_value % capacity);

    // Baseline the current entry's displacement measurement before searching.
    entry.probe_length = 0;

    bool cycle = true;

    // Traverse along collision lines until a suitable location criteria is satisfied.
    while (cycle)
    {
        // An unallocated slot is reached.
        if (buckets[index].key == NULL)
        {
            buckets[index] = entry;
            
            break;
        }

        // Matching key found. Reclaim the duplicate insertion string allocation.
        if (strcmp(buckets[index].key, entry.key) == 0)
        {
            buckets[index].value = entry.value;

            free(entry.key);

            break;
        }

        // The element in hand has traveled further than the current resident element.
        if (entry.probe_length > buckets[index].probe_length)
        {
            Entry temp = buckets[index];
            buckets[index] = entry;
            entry = temp;
        }

        // Advance downstream linearly across the table tracking bounds with modular protection.
        index = (index + 1) % capacity;
        entry.probe_length++;
    }

    return true;
}

/**
 * @brief       :   Dynamically scales and doubles the capacity tracking window of the map.
 * @details     :   Allocates a clean, doubled memory workspace using calloc, rehashes all valid
 *                  active entries into the new block using raw_insert, and frees the old bucket workspace.
 * @param       :   map: The RobinHood map instance requesting restructuring layout expansions.
 * @return      :   bool: true if table expansion and rehashing succeeded, false on allocation constraints.
 */
static inline bool resize(RobinHood* map)
{
    // Prevent actions targeting undefined container contexts.
    if (map == NULL)
    {
        return false;
    }

    size_t old_capacity = map->capacity;

    Entry* old_buckets = map->buckets;

    // Numerical calculation guard checking for memory size multiplication overflows.
    if (old_capacity > SIZE_MAX / 2)
    {
        return false;
    }

    // Determine the expanded memory allocation footprint scale.
    size_t new_capacity = old_capacity * 2;

    // Secure a zero-initialized contiguous destinationn buffer array.
    Entry* new_buckets = (Entry*)calloc(new_capacity, sizeof(Entry));

    // Abort gracefully if the system fails to accommodate the expanded heap allocation request.
    if (new_buckets == NULL)
    {
        return false;
    }

    // Systematically migrate entries from the old layout arrays to the new backing space.
    for (size_t index = 0; index < old_capacity; index++)
    {
        if (old_buckets[index].key != NULL)
        {
            raw_insert(new_buckets, new_capacity, old_buckets[index]);
        }
    }

    // Finalize top-level structural redirection mappings.
    map->capacity = new_capacity;
    map->buckets = new_buckets;

    // Reclaim the now outmoded previous generation storage array worksapce.
    free(old_buckets);

    return true;
}

/**
 * @brief       :   Allocates and zero-initializes a new Robin Hood map instance container.
 * @details     :   Sets baseline initialization configurations and secures a flat backing array 
 *                  for metadata tracking buckets. Clears parent contexts safely on heap aborts.
 * @return      :   RobinHood*: Pointer to the newly allocated map instance, or NULL if out of memory.
 */
RobinHood* initialize_map(void)
{
    // Allocate the master structure configuration descriptor container.
    RobinHood* map = (RobinHood*)malloc(sizeof(RobinHood));

    if (map == NULL)
    {
        return NULL;
    }

    // Setup map structural properties to defaults.
    map->capacity = INITIAL_CAPACITY;
    map->size = 0;

    // Alocate the primary array layer.
    map->buckets = (Entry*)calloc(map->capacity, sizeof(Entry));

    if (map->buckets == NULL)
    {
        free(map);

        return NULL;
    }

    return map;
}

/**
 * @brief       :   Inserts a new key-value pair map entry or performs an element value overwrite.
 * @details     :   Monitors density ratios against LOAD_FACTOR_THRESHOLD, prompting resizing operations
 *                  when boundaries are breached. Allocates and replicates the targeted string context.
 * @param       :   map: The RobinHood map instance.
 * @param       :   key: The unique null-terminated identification string.
 * @param       :   value: Integer value payload mapping.
 * @return      :   bool: true if allocation and map registration succeeded, false otherwise.
 */
bool put(RobinHood* map, const char* key, int value)
{
    // Block operations utilizing malformed parameters.
    if (map == NULL || key == NULL)
    {
        return false;
    }

    int dummy_value = 0;
    unsigned int target_hash_value = hash(key);
    
    size_t index = (size_t)(target_hash_value % map->capacity);
    size_t current_probe_length = 0;

    bool cycle = true;

    while (cycle)
    {
        Entry current = map->buckets[index];

        // Standard Robin Hood early exit optimizations.
        if (current.key == NULL || current_probe_length > current.probe_length)
        {
            break;
        }

        if (strcmp(current.key, key) == 0)
        {
            // Update value in-place.
            map->buckets[index].value = value;

            return true;
        }

        index = (index + 1) % map->capacity;
        current_probe_length++;
    }

    // Access map load density metrics to initiate dynamic scaling.
    if ((double)map->size / (double)map->capacity >= LOAD_FACTOR_THRESHOLD)
    {
        if (!resize(map))
        {
            return false;
        }
    }

    Entry entry = {NULL};

    size_t key_length = strlen(key);

    // Allocate the key.
    entry.key = (char*)malloc(key_length + 1);

    if (entry.key == NULL)
    {
        return false;
    }

    // Copy the key.
    memcpy(entry.key, key, key_length + 1);

    // Finalize staging data object parameters prior to indexing insertion strategies.
    entry.value = value;
    entry.hash_value = hash(key);
    entry.probe_length = 0;

    // Dispatch the payload object to the low-level slot calculation.
    if (raw_insert(map->buckets, map->capacity, entry))
    {
        map->size++;

        return true;
    }

    return false;
}

/**
 * @brief       :   Queries the map structure for a specific key and extracts its payload.
 * @details     :   Leverages Robin Hood invariants to evaluate lookups with early-exit performance paths;
 *                  if the tracking search probe length outgrows the slot resident's probe length, the
 *                  lookup terminates aggressively knowing the item cannot exist further down the array.
 * @param       :   map: Read-only pointer referencing the targeted map.
 * @param       :   key: The identification search key string.
 * @param       :   value: Destination address output pointer to populate with the matching data.
 * @return      :   bool: true if the element was successfully verified, false if missing or validation failed.
 */
bool get(const RobinHood* map, const char* key, int* value)
{
    // Explicit pointer sanity validation across input channels.
    if (map == NULL || map->buckets == NULL || key == NULL || value == NULL)
    {
        return false;
    }

    unsigned int hash_value = hash(key);
    
    // Calculate the baseline ideal offset for index mapping.
    size_t index = (size_t)(hash_value % map->capacity);
    size_t current_probe_length = 0;

    bool cycle = true;

    // Scan down contiguous blocks segments using sequential open-address logic.
    while (cycle)
    {
        Entry current = map->buckets[index];

        // Robin Hood early exit invariant check.
        if (current.key == NULL || current_probe_length > current.probe_length)
        {
            return false;
        }

        // Key match established: Extract map value payload contents.
        if (strcmp(current.key, key) == 0)
        {
            *value = current.value;

            return true;
        }

        // Advance to check next sequential array tracking offfset position.
        index = (index + 1) % map->capacity;
        current_probe_length++;
    }

    return true;
}

/**
 * @brief       :   Removes a specific key association from the table and optimizes remaining elements.
 * @details     :   Performs a localized target search sequence. Upon locating the record, frees the key
 *                  string and starts a backward shift sequence. Contiguous displaced items down the array
 *                  are pulled backward into the gap and have their probe lengths decremented until a stable
 *                  element (probe length 0) or empty slot seals the compaction run.
 * @param       :   map: The RobinHood hash table instance.
 * @param       :   key: The string key identifier to discard.
 * @return      :   bool: true if item was matched and purged successfully, false if not found.
 */
bool erase(RobinHood* map, const char* key)
{
    // Guard against processing executions targeting unpopulated tables.
    if (map == NULL || map->buckets == NULL || key == NULL || map->size == 0)
    {
        return false;
    }

    unsigned int hash_value = hash(key);

    size_t index = (size_t)(hash_value % map->capacity);
    size_t current_probe_length = 0;

    bool cycle = true;

    // Search out the exact cell location matching the specified target key.
    while (cycle)
    {
        Entry current = map->buckets[index];

        // Utilize early-exit optimization rules to catch missing entries quickly.
        if (current.key == NULL || current_probe_length > current.probe_length)
        {
            return false;
        }

        // Free internal heap string and clear the slot mapping indicator.
        if (strcmp(current.key, key) == 0)
        {
            free(map->buckets[index].key);

            map->buckets[index].key = NULL;

            break;
        }

        index = (index + 1) % map->capacity;
        current_probe_length++;
    }

    // Set up indices for the tombstone-free Backward-Shift deletion step.
    size_t current_index = index;
    size_t next_index = (current_index + 1) % map->capacity;

    cycle = true;

    // Shift subsequent displaced itmes backward to prevent sequence gap fragmentation.
    while (cycle)
    {
        // Loop stoppung condition: Hit an unallocated slot boundary.
        if (map->buckets[next_index].key == NULL)
        {
            break;
        }

        // Loop stopping condition: Hit an item sitting perfectly in its own ideal slot.
        if (map->buckets[next_index].probe_length == 0)
        {
            break;
        }

        // Migrating entry back by one slot position to seal the gap.
        map->buckets[current_index] = map->buckets[next_index];
        map->buckets[current_index].probe_length--;

        // Blank out the donor slot layout before looping to prevent duplication states.
        map->buckets[next_index].key = NULL;

        // Step forward to inspect the next slot link down the continuous run array block.
        current_index = next_index;
        next_index = (next_index + 1) % map->capacity;
    }

    // Reduce the active size counter tracking state safely.
    map->size--;

    return true;
}

/**
 * @brief       :   Dumps a visualization structural snapshot of the table configuration.
 * @details     :   Loops across the allocated capacity boundary printing the exact storage index,
 *                  associated string keys, stored values, distance offsets, and computed mathematical ideality.
 * @param       :   map: Const pointer to the printable RobinHood context map block.
 * @return      :   void
 */
void display(const RobinHood* map)
{
    // Prevent null formatting print errors from corrupting pipelines.
    if (map == NULL)
    {
        return;
    }

    if (map->buckets == NULL)
    {
        printf("%s", "Bucket array is NULL.\n");

        return;
    }

    printf("%s", "-- Robin Hood Table --\n");

    // Linearly loop over every slot within the allocated capacity boundaries.
    for (size_t index = 0; index < map->capacity; index++)
    {
        // Evaluate bucket status to select the matching display format string.
        if (map->buckets[index].key != NULL)
        {
            // Compute the target item's ideal index position for diagnostic comparison metrics.
            size_t ideal_index = (size_t)(map->buckets[index].hash_value % map->capacity);

            printf("%s [%04zu]: %s '%s' %s %i %s %zu %zu%c", "Bucket", index,
                "Key =", map->buckets[index].key, 
                "Value=", map->buckets[index].value, 
                "Probe length =", map->buckets[index].probe_length, 
                ideal_index, '\n');
        }
        else
        {
            printf("%s [%04zi]: %s", "Bucket",  index, "[Empty]\n");
        }
    }

    printf("%c", '\n');
}

/**
 * @brief       :   Tears down allocations and completely dismantles the map structure memory frame.
 * @details     :   Iterates linearly down the allocated table array, freeing dynamically assigned key allocations,
 *                  clears out the buckets storage buffer array, and terminates the top-level mapping instance.
 * @param       :   map: The map structure targeted for full lifecycle destruction.
 * @return      :   void
 */
void clean(RobinHood* map)
{
    // Avoid executino tasks if dealing with pre-cleared contexts.
    if (map == NULL)
    {
        return;
    }

    // Clear and deallocate all nested items within the tracking boundary array martix.
    if (map->buckets != NULL)
    {
        for (size_t index = 0; index < map->capacity; index++)
        {
            // Free the copied string allocation key to prevent leaks.
            free(map->buckets[index].key);
        }

        // Release the bucket tracking structure collection container block.
        free(map->buckets);
    }

    // Release the parent table manager master descriptor block.
    free(map);
}