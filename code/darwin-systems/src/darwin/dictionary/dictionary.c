/**
 * @file        :   dictionary.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of a dynamic, generic dictionary using a Hash Table.
 * @details     :   This module provides a hash map implementation using linked-list 
 * chaining for collision resolution. It features dynamic memory 
 * management including:
 * - Automatic growth (upsizing) when the load factor exceeds 0.7.
 * - Automatic shrinking (downsizing) when the load factor drops below 0.1.
 * - Key ownership through string duplication (strdup).
 * - Generic value storage using void pointers.
 * * The hashing is performed using the djb2 algorithm, providing 
 * efficient distribution for string-based keys.
 * @version     :   1.0
 * @date        :   2026-05-12
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include header files.
#include "darwin/dictionary/dictionary.h"

// Function prototypes.
static inline void resize(Dictionary* dictionary, size_t size);

/**
 * @brief       :   Generates a hash index for a given string key.
 * @details     :   Implements the djb2 algorithm, a non-cryptographic hash 
 *                  function known for its speed and low collision rate.
 * @param       :   key: The string to be hashed.
 * @param       :   size: The current capacity of the dictionary bucket array.
 * @return      :   size_t: The calculated index within the bounds of [0, size-1].
 */
static inline size_t hash(const char* key, size_t size)
{
    // Check for invalid input to prevent division by zero or null pointer access.
    if (key == NULL || size == 0)
    {
        return 0;
    }

    // Initialize the hash value with the djb2 magic number.
    unsigned long int value = 5381;

    int character = 0;

    // Iterate through every character in the string until the null terminator.
    while ((character = *key++))
    {
        value = ((value << 5) + value) + character;
    }

    // Constrain the final value wihtin the currrent array size.
    return (size_t)value % size;
}

/**
 * @brief       :   Allocates and initializes a new dictionary.
 * @details     :   Sets up the initial bucket array with INITIAL_SIZE and sets 
 *                  all bucket pointers to NULL.
 * @return      :   Dictionary*: A pointer to the initialized dictionary, or 
 *                  NULL if allocation fails.
 */
Dictionary* initialize(void)
{
    // Allocate memory for the top-level dictionary structure.
    Dictionary* dictionary = (Dictionary*)malloc(sizeof(Dictionary));

    if (dictionary == NULL)
    {
        return NULL;
    }

    // Set default starting values for the dictionary capacity and item count.
    dictionary->size = INITIAL_SIZE;
    dictionary->count = 0;

    // Allocate the array of bucket pointers and initialize them to NULL.
    dictionary->buckets = (Entry**)calloc(dictionary->size, sizeof(Entry*));

    if (dictionary->buckets == NULL)
    {
        // Clean up the parent structure if the bucket allocation fails.
        free(dictionary);

        return NULL;
    }

    return dictionary;
}

/**
 * @brief       :   Inserts a key-value pair into the dictionary.
 * @details     :   If the key already exists, the value is updated. If the 
 *                  load factor exceeds GROWTH_THRESHOLD, the dictionary 
 *                  is resized to double its capacity.
 * @param       :   dictionary: The dictionary instance.
 * @param       :   key: The string key.
 * @param       :   value: A pointer to the value to be stored.
 * @return      :   void
 */
void insert(Dictionary* dictionary, const char* key, void* value)
{
    // Guard against invalid inputs to prevent segmentation faults.
    if (dictionary == NULL || key == NULL || value == NULL)
    {
        return;
    }

    // Calculate the current density of the hash table.
    double load_factor = (double)dictionary->count / dictionary->size;

    if (load_factor > GROWTH_THRESHOLD)
    {
        // Expand the table if the density exceeds the recommended threshold.
        resize(dictionary, dictionary->size * 2);
    }

    // Calculate the bucket index for the given key.
    size_t index = hash(key, dictionary->size);

    Entry* entry = dictionary->buckets[index];

    // Traverse the linked list at the calculated index to look for existing keys.
    while (entry != NULL)
    {
        // If a match is found, update the value and exit to prevent duplicates.
        if (strcmp(entry->key, key) == 0)
        {
            entry->value = value;

            // No duplicates.
            return;
        }

        entry = entry->next;
    }

    // Create a new entry container for they key-value pair.
    Entry* new_entry = (Entry*)malloc(sizeof(Entry));

    if (new_entry == NULL)
    {
        return;
    }

    // Duplicate the string to ensure the dictionary owns its own copy.
    new_entry->key = strdup(key);

    if (new_entry->key == NULL)
    {
        free(new_entry);

        return;
    }

    // Assign the value and link the entry to the head of the bucket's list.
    new_entry->value = value;
    new_entry->next = dictionary->buckets[index];

    // Update the bucket pointer and increment the total item count.
    dictionary->buckets[index] = new_entry;
    dictionary->count++;
}

/**
 * @brief       :   Retrieves a value associated with a key.
 * @param       :   dictionary: The dictionary instance.
 * @param       :   key: The key to look up.
 * @return      :   void*: Pointer to the value if found, otherwise NULL.
 */
void* retrieve(const Dictionary* dictionary, const char* key)
{
    // Validate that the search parameters are not NULL.
    if (dictionary == NULL || key == NULL)
    {
        return NULL;
    }

    // Determine which bucket the key would be stored in.
    size_t index = hash(key, dictionary->size);

    Entry* entry = dictionary->buckets[index];

    // Linear search through the bucket's linked list for a matching key string.
    while (entry != NULL)
    {
        // Return the associated value pointer if the keys match.
        if (strcmp(entry->key, key) == 0)
        {
            return entry->value;
        }

        entry = entry->next;
    }

    // Return NULL if the key was not found in the dictionary.
    return NULL;
}

/**
 * @brief       :   Resizes the internal bucket array and rehashes entries.
 * @details     :   Allocates a new bucket array of the specified size and 
 *                  moves all existing entries to their new locations based 
 *                  on the updated hash size.
 * @param       :   dictionary: The dictionary instance.
 * @param       :   size: The new capacity for the bucket array.
 * @return      :   void
 */
static inline void resize(Dictionary* dictionary, size_t size)
{
    // Ensure the dictionary exists and the new size is valid.
    if (dictionary == NULL || size < INITIAL_SIZE)
    {
        return;
    }

    // Allocate and zero-initialize a new, larger or smaller array of buckets.
    Entry** new_buckets = (Entry**)calloc(size, sizeof(Entry*));

    if (new_buckets == NULL)
    {
        return;
    }

    // Iterate through every bucket in the old array.
    for (size_t index = 0; index < dictionary->size; index++)
    {
        Entry* entry = dictionary->buckets[index];

        // Move each entry in the old bucket to its new position in the new array.
        while (entry != NULL)
        {
            // Store the next pointer before modifying the current entry.
            Entry* next = entry->next;

            // Re-hash the key using the new table size.
            size_t new_index = hash(entry->key, size);

            // Insert the entry into its new bucket.
            entry->next = new_buckets[new_index];
            new_buckets[new_index] = entry;

            // Advance to the next entry in the old list.
            entry = next;
        }
    }

    // Free the old array of bucket pointers.
    free(dictionary->buckets);

    // Update the dictionary to use the new array and size.
    dictionary->buckets = new_buckets;
    dictionary->size = size;
}

/**
 * @brief       :   Removes a key and its associated value from the dictionary.
 * @details     :   Frees the duplicated key string and the entry container. 
 *                  If the load factor drops below SHRINK_THRESHOLD, the 
 *                  dictionary is resized to half its capacity.
 * @param       :   dictionary: The dictionary instance.
 * @param       :   key: The key to be removed.
 * @return      :   void
 */
void erase(Dictionary* dictionary, const char* key)
{
    // Confirm input validity before attempting removal logic.
    if (dictionary == NULL || key == NULL)
    {
        return;
    }

    // Locate the bucket where the key resides.
    size_t index = hash(key, dictionary->size);

    Entry* entry = dictionary->buckets[index];
    Entry* previous = NULL;

    // Traverse the list to find the specific entry to delete.
    while (entry != NULL)
    {
        // Check if the current entry's key matches the target.
        if (strcmp(entry->key, key) == 0)
        {
            // Update pointers to bypass the entry being removed.
            if (previous != NULL)
            {
                previous->next = entry->next;
            }
            else
            {
                // If the entry is at the head, update the bucket pointer itself.
                dictionary->buckets[index] = entry->next;
            }

            // Clean up the memory allocated for the key and the enry node.
            free(entry->key);
            free(entry);

            dictionary->count--;

            // Calculate the new density to determine if memory can be relaimed.
            double load_factor = (double)dictionary->count / dictionary->size;

            // Contract the table if it is significantly under-utilized.
            if (dictionary->size > INITIAL_SIZE && load_factor < SHRINK_THRESHOLD)
            {
                resize(dictionary, dictionary->size / 2);
            }

            // Exit immediately to avoid accessing freed memory.
            return;
        }

        // Keep track of the previous node to maintain list continuity.
        previous = entry;
        entry = entry->next;
    }
}

/**
 * @brief       :   Frees all memory associated with the dictionary.
 * @details     :   Iterates through every bucket and linked list, freeing 
 *                  keys and entries. Finally frees the bucket array and 
 *                  the dictionary struct itself.
 * @param       :   dictionary: The dictionary instance to be destroyed.
 * @return      :   void
 */
void uninitialize(Dictionary* dictionary)
{
    // Guard against double-freeing of NULL pointer acces.
    if (dictionary == NULL)
    {
        return;
    }

    // Iterate over each bucket to clear individual entry chains.
    for (size_t index = 0; index < dictionary->size; index++)
    {
        Entry* entry = dictionary->buckets[index];

        // Follow the linked list chain and free each node.
        while (entry != NULL)
        {
            // Preserve the entry pointer to free it after advancing the loop.
            Entry* temp = entry;

            entry = entry->next;

            // Release the duplicated key string memory.
            free(temp->key);

            // Release the entry container.
            free(temp);
        }
    }

    // Release the bucket pointer array memory.
    if (dictionary->buckets != NULL)
    {
        free(dictionary->buckets);
    }

    // Release the top-level dictionary structure.
    free(dictionary);
}