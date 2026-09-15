/**
 * @file        :   dictionary.h
 * @author      :   Braiden Gole
 * @brief       :   Header definitions for a dynamic, generic Hash Map.
 * @details     :   This module defines the structures and interface for a 
 * dictionary that supports string keys and generic void 
 * pointer values. It utilizes linked-list chaining for 
 * collision resolution and supports dynamic resizing 
 * based on configurable load factor thresholds.
 * @version     :   1.0
 * @date        :   2026-05-12
 */
#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stddef.h>

// Default capacity and behavior thresholds.
#define INITIAL_SIZE 8
#define GROWTH_THRESHOLD 0.7
#define SHRINK_THRESHOLD 0.1

/**
 * @struct      :   Entry
 * @brief       :   A single node within the hash table bucket.
 * @details     :   Contains the key-value pair and a pointer to the next 
 *                  entry to facilitate collision resolution via a 
 *                  singly-linked list.
 */
typedef struct Entry
{
    char* key;
    void* value;
    struct Entry* next;
} Entry;

/**
 * @struct      :   Dictionary
 * @brief       :   The primary controller structure for the Hash Map.
 * @details     :   Maintains state regarding the total capacity, the number 
 *                  of elements currently stored, and the pointer to the 
 *                  array of bucket pointers.
 */
typedef struct
{
    size_t size;
    size_t count;
    Entry** buckets;
} Dictionary;

// Function prototypes.
Dictionary* initialize(void);
void insert(Dictionary* dictionary, const char* key, void* value);
void* retrieve(const Dictionary* dictionary, const char* key);
void erase(Dictionary* dictionary, const char* key);
void uninitialize(Dictionary* dictionary);

#endif