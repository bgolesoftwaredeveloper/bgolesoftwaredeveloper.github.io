/**
 * @file        :   bi_directional.h
 * @author      :   Braiden Gole
 * @brief       :   Header file defining the interface for a bi-directional Binary Search Tree.
 * @details     :   Provides the Node structure token and functional declarations required for 
 * creating, navigating, modifying, and visualizing a Binary Search Tree 
 * that maintains explicit upwards links via parent pointers.
 * @version     :   1.0
 * @date        :   2026-05-24
 */
#ifndef BI_DIRECTIONAL_H
#define BI_DIRECTIONAL_H

/**
 * @struct      :   Node
 * @brief       :   Represents a single element within the bi-directional BST.
 */
typedef struct Node
{
    int data;
    
    struct Node* left;
    struct Node* right;
    struct Node* parent;
} Node;

// Function prototypes.
Node* construct(int data, Node* parent);
Node* insert(Node* root, int data);
Node* update(Node* root, int data, int new_data);
Node* search(Node* root, int key);
Node* erase(Node* root, int key);
void display(const Node* root, int level);
void clean(Node* root);

#endif