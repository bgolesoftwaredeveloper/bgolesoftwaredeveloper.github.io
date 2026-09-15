/**
 * @file        :   tuple.h
 * @author      :   Braiden Gole
 * @brief       :   Header definition for a pharmaceutical data tuple.
 * @details     :   Provides structures and function prototypes for managing
 * a collection of pharmaceutical records. Supports brand-based retrieval
 * and structured storage of pharmacokinetics and regulatory data.
 * @version     :   1.0
 * @date        :   2026-04-20
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include header files.
#include "darwin/tuple/tuple.h"

/**
 * @brief       :   Initializes a new Tuple instance.
 * @details     :   Allocates heap memory for the Tuple structure and its 
 *                  internal array of pharmaceutical pointers.
 * @param       :   capacity The maximum number of items the tuple can hold.
 * @return      :   A pointer to the allocated Tuple, or NULL if allocation fails.
 */
Tuple* create(size_t capacity)
{
    // Ensure we aren't creating a zero-length container.
    if (capacity == 0)
    {
        return NULL;
    }

    // Allocate memory for the main Tuple container structure.
    Tuple* tuple = (Tuple*)malloc(sizeof(Tuple));

    if (tuple == NULL)
    {
        return NULL;
    }

    // Initialize metadata: Current count is zero, limit is the provided capacity.
    tuple->size = 0;
    tuple->capacity = capacity;

    // Allocate the internal array of pointers to Pharmaceutical objects.
    tuple->items = (Pharmaceutical**)malloc(sizeof(Pharmaceutical*) * capacity);

    // Clean up the Tuple container if the internal array allocation fails.
    if (tuple->items == NULL)
    {
        free(tuple);

        return NULL;
    }

    return tuple;
}

/**
 * @brief       :   Adds a Pharmaceutical item to the tuple.
 * @details     :   Performs a shallow copy of the pharmaceutical data and 
 *                  stores it in the next available index.
 * @param       :   tuple A pointer to the Tuple instance.
 * @param       :   item A pointer to the Pharmaceutical struct to be added.
 * @return      :   True if successful, false if tuple is full or NULL.
 */
bool add(Tuple* tuple, const Pharmaceutical* item)
{
    // Safety check for null pointer or overflow.
    if (tuple == NULL || item == NULL)
    {
        return false;
    }

    // Block addition if the tuple has reached its predefined capacity.
    if (tuple->size >= tuple->capacity)
    {
        return false;
    }

    // Allocate heap memory for a new record to ensure the Tuple owns its data.
    Pharmaceutical* new_pharmaceutical = (Pharmaceutical*)malloc(sizeof(Pharmaceutical));

    if (new_pharmaceutical == NULL)
    {
        return false;
    }

    // Copy the contents of the provided item into teh newly allocated memory.
    memcpy(new_pharmaceutical, item, sizeof(Pharmaceutical));

    // Store the pointer in the next available slot and increment the size counter.
    tuple->items[tuple->size] = new_pharmaceutical;
    tuple->size++;

    return true;
}

/**
 * @brief       :   Retrieves an item at a specific index.
 * @details     :   Bounds checks the provided index against the current size.
 * @param       :   tuple A pointer to the constant Tuple instance.
 * @param       :   index The position of the item to retrieve.
 * @return      :   A pointer to the Pharmaceutical at the index, or NULL if invalid.
 */
Pharmaceutical* at(const Tuple* tuple, size_t index)
{
    // Prevent access if the tuple is null.
    if (tuple == NULL)
    {
        return NULL;
    }

    // Bounds check: Ensure the requested index is within the initialized range.
    if (index >= tuple->size)
    {
        return NULL;
    }

    // Return the pointer to the record at the specified index.
    return tuple->items[index];
}

/**
 * @brief       :   Searches for a pharmaceutical by brand name.
 * @details     :   Iterates through the collection and performs string comparison.
 * @param       :   tuple A pointer to the constant Tuple instance.
 * @param       :   name The brand name string to search for.
 * @return      :   A pointer to the found Pharmaceutical, or NULL if no match.
 */
Pharmaceutical* by_brand_name(const Tuple* tuple, const char* name)
{
    // Validate inputs to prevent crashes during string comparison.
    if (tuple == NULL || name == NULL)
    {
        return NULL;
    }

    // Lineary traverse the collection.
    for (size_t index = 0; index < tuple->size; index++)
    {
        // Check if the current item's brand name matches the search query.
        if (strcmp(tuple->items[index]->brandName, name) == 0)
        {
            return tuple->items[index];
        }
    }

    // Return NULL if the entire loop completes without a match.
    return NULL;
}

/**
 * @brief       :   Deallocates all memory associated with the Tuple.
 * @details     :   Frees each stored Pharmaceutical pointer, the internal
 *                  array, and the Tuple structure itself.
 * @param       :   tuple A pointer to the Tuple instance to be destroyed.
 * @return      :   Void.
 */
void destroy(Tuple* tuple)
{
    if (tuple == NULL)
    {
        return;
    }

    // Iterate through all active records and free each individual Pharmaceutical object.
    for (size_t index = 0; index < tuple->size; index++)
    {
        free(tuple->items[index]);
    }

    // Free the array that held the pointers.
    free(tuple->items);

    // Free the tuple container itself.
    free(tuple);
}