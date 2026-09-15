/**
 * @file        :   skip_list.h
 * @author      :   Braiden Gole
 * @brief       :   Definitions and function prototypes for a Skip List data structure.
 * @details     :   Provides a probabilistic alternative to balanced trees, using 
 * hierarchical forward pointer layers to achieve logarithmic search,
 * insertion, and deletion average time complexities.
 * @version     :   1.0
 * @date        :   2026-05-25
 */
#ifndef SKIP_LIST_H
#define SKIP_LIST_H

#include <stddef.h>
#include <stdbool.h>
#include <limits.h>

#define MAXIMUM_SKIP_LIST_LEVEL 6

/**
 * @struct Node
 * @brief Individual structural node inside the Skip List.
 * @details Houses the key-value payload data and an array of forward pointers
 * corresponding to the randomized lane height assigned during instantiation.
 */
typedef struct Node
{
    int key;
    int value;
    struct Node** forward;
} Node;

/**
 * @struct SkipList
 * @brief Master controller tracking the structural bounds of the Skip List.
 * @details Retains a direct link to the initialized sentinel header node and
 * manages the current global peak level across all active entries.
 */
typedef struct SkipList
{
    size_t level;
    Node* header;
} SkipList;

// Function prototypes.
Node* construct(int key, int value, size_t level);
SkipList* initialize(void);
bool insert(SkipList* list, int key, int value);
bool search(const SkipList* list, int key, int* value);
void display(const SkipList* list);
void clean(SkipList* list);

#endif