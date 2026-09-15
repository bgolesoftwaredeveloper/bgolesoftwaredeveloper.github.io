/**
 * @file        :   circular_doubly_linked_list.h
 * @author      :   Braiden Gole
 * @brief       :   Interface for an advanced Circular Doubly Linked List (CDLL).
 * @details     :   Defines a "Galaxy" container that manages "OrbitalNodes" in a 
 * continuous, bidirectional loop. This implementation ensures 
 * O(1) tail access via the sentinel's backward link and 
 * consistent circularity during all mutations.
 * @version     :   1.0
 * @date        :   2026-05-14
 * @copyright   :   Copyright (c) 2026 Darwin Systems
 */
#ifndef CIRCULAR_DOUBLY_LINKED_LIST_H
#define CIRCULAR_DOUBLY_LINKED_LIST_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @struct OrbitalNode
 * @brief  The fundamental unit of the circular chain.
 */
typedef struct OrbitalNode
{
    int rhythmic_data;
    struct OrbitalNode* forward_link;
    struct OrbitalNode* backward_link;
} OrbitalNode;

/**
 * @struct Galaxy
 * @brief  Controller structure for the circular doubly linked list.
 */
typedef struct 
{
    size_t population_count;
    OrbitalNode* sentinel;
} Galaxy;

// Function prototypes.
Galaxy* new_galaxy(void);
void append(Galaxy* galaxy, int value);
OrbitalNode* find(const Galaxy* galaxy, int value);
bool update(Galaxy* galaxy, int value, int new_value);
bool erase(Galaxy* galaxy, int value);
void display(const Galaxy* galaxy);
void clear(Galaxy* galaxy);

#endif