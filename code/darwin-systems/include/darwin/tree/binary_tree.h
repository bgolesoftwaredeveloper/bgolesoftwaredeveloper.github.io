/**
 * @file        :   binary_tree.h
 * @author      :   Braiden Gole
 * @brief       :   Header file for a custom Binary Search Tree (BST).
 * @details     :   Defines the core Node structure and exposes API signatures
 * for insertion, search, erasure, sorted traversals, and 
 * heap memory optimization utilities using size_t keys.
 * @version     :   1.0
 * @date        :   2026-05-17
 */
#ifndef BINARY_SEARCH_H
#define BINARY_SEARCH_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @struct      :   Node
 * @brief       :   Represents a single node within the binary search tree.
 * @details     :   Contains an unsigned data payload alongside references
 *                  to structural child nodes down the hierarchy.
 */
typedef struct Node
{
    size_t data;
    struct Node* left;
    struct Node* right;
} Node;

// Function prototypes.
Node* insert(Node* root, size_t value);
bool search(const Node* root, size_t target);
Node* erase(Node* root, size_t target);
void pre_order(const Node* root);
void in_order(const Node* root);
void post_order(const Node* root);
void clean(Node* root);

#endif