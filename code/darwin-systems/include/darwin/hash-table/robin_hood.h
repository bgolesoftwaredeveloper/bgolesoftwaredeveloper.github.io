/**
 * @file        :   robin_hood.h
 * @author      :   Braiden Gole
 * @brief       :   Definitions and API prototypes for a Robin Hood Hash Table.
 * @details     :   Provides structures, capacity macros, and operations for an
 * open-addressed, linear-probed hash table utilizing Robin Hood
 * displacement policies to enforce tight variance boundaries over
 * probe sequence lengths.
 * @version     :   1.0
 * @date        :   2026-05-26
 */
#ifndef ROBIN_HOOD_H
#define ROBIN_HOOD_H

#include <stdbool.h>
#include <stddef.h>

#define INITIAL_CAPACITY 8
#define LOAD_FACTOR_THRESHOLD 0.7
#define UNINITIALIZED_HASH 0

/**
 * @struct      :   Entry
 * @brief       :   Represents an individual bucket slot record inside the map array.
 * @details     :   Tracks the key string resource ownership alongside its value, 
 *                  cached calculation hash, and current neighborhood displacement metrics.
 */
typedef struct
{
    char* key;
    int value;
    unsigned int hash_value;
    size_t probe_length;
} Entry;

/**
 * @struct      :   RobinHood
 * @brief       :   Top-level manager tracking map context properties.
 * @details     :   Maintains scale configurations, element occupancy trackers, and 
 *                  the primary data pointer targeting the contiguous array matrix.
 */
typedef struct
{
    size_t capacity;
    size_t size;
    Entry* buckets;
} RobinHood;

/**
 * @struct      :   KeyValuePair
 * @brief       :   Static data pair initializer utility structure.
 * @details     :   Used within user-facing test frameworks to map unmanaged static 
 *                  association chains prior to ingestion looping processes.
 */
typedef struct
{
    const char* key;
    int value;
} KeyValuePair;

// Function prototypes.
RobinHood* initialize_map(void);
bool put(RobinHood* map, const char* key, int value);
bool get(const RobinHood* map, const char* key, int* value);
bool erase(RobinHood* map, const char* key);
void display(const RobinHood* map);
void clean(RobinHood* map);

#endif