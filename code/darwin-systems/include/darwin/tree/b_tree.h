/**
 * @file        :   b_tree.h
 * @author      :   Braiden Gole
 * @brief       :   Interface definitions for a balanced B-Tree structure.
 * @details     :   Declares the node structures and core public function prototypes
 * required for creating, searching, inserting, and deleting from a B-Tree.
 * @version     :   1.0
 * @date        :   2026-05-21
 */
#ifndef B_TREE_H
#define B_TREE_H

#include <stddef.h>
#include <stdbool.h>

#define ORDER 4
#define MINIMUM_KEYS ((ORDER / 2) - 1)

/**
 * @struct      :   Node
 * @brief       :   Represents an individual multi-way node in the B-Tree.
 */
typedef struct Node
{
    int keys[ORDER - 1];
    size_t count;
    bool is_leaf;
    struct Node* children[ORDER];
} Node;

// Function prototypes.
Node* construct(bool is_leaf);
Node* search(Node* root, int key);
void insert(Node** root, int key);
void erase(Node** root, int key);
void traverse(const Node* root);
void clean(Node* root);

#endif