/**
 * @file        :   rope.h
 * @author      :   Braiden Gole
 * @brief       :   Definitions and API prototypes for the Rope string pattern matching algorithm.
 * @details     :   Defines the binary tree Node structure used to represent complex, segmented 
 * strings and exports the main pattern matching interface.
 * @version     :   1.0
 * @date        :   2026-06-06
 */
#ifndef ROPE_H
#define ROPE_H

#include <stdbool.h>
#include <stddef.h>

typedef struct Node
{
    char* leaf;
    size_t weight;

    struct Node* left;
    struct Node* right;
} Node;

// Function prototypes.
bool rope(const Node* const root, const char* const pattern, size_t* const out_index);

#endif