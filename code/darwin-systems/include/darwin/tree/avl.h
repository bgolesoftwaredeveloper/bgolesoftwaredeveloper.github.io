/**
 * @file        :   avl.h
 * @author      :   Braiden Gole
 * @brief       :   Header definition for a self-balancing AVL binary search tree.
 * @details     :   Defines the Node structure and core operational prototypes for 
 * the AVL tree, ensuring O(log n) performance for all operations.
 * @version     :   1.0
 * @date        :   2026-04-19
 */
#ifndef AVL_H
#define AVL_H

#include <stdbool.h>

/**
 * @struct      :   Node
 * @brief       :   The fundamental building block of the AVL tree.
 * @param       :   key:         The integer value stored in the node.
 * @param       :   height:      The height of the node used for balance calculations.
 * @param       :   left_child:  Pointer to the left subtree (lesser values).
 * @param       :   right_child: Pointer to the right subtree (greater values).
 */
typedef struct Node
{
    int key;
    int height;

    struct Node* left_child;
    struct Node* right_child;
} Node;

// Function prototypes.
Node* initialize(int key);
Node* insert(Node* node, int key);
Node* update(Node* root, int key, int new_key, bool* success);
Node* erase(Node* root, int key);
void clean_up(Node* root);

#endif