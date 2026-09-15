/**
 * @file        :   interval_tree.h
 * @author      :   Braiden Gole
 * @brief       :   Interface and structures for an augmented Interval Search Tree.
 * @details     :   Provides the structural definitions and function prototypes for
 * managing an interval search tree. The tree uses the low boundary 
 * of an interval as its primary key for BST ordering and maintains 
 * a subtree maximum high value at each node to optimize overlap queries.
 * @version     :   1.0
 * @date        :   2026-05-21
 */
#ifndef INTERVAL_TREE_H
#define INTERVAL_TREE_H

#include <stdbool.h>

/**
 * @struct  :   Interval
 * @brief   :   Represents a closed numeric range boundary.
 * @details :   Defines a mathematical closed interval $[low, high]$ where low is 
 *              always assumed to be less than or equal to high.
 */
typedef struct Interval
{
    int low;
    int high;
} Interval;

/**
 * @struct  :   Node
 * @brief   :   An augmented binary search tree node containing interval data.
 * @details :   Contains a pointer to the managed interval metadata, the maximum 
 *              high value present across its entire child subtree, and pointers 
 *              to left and right child branches.
 */
typedef struct Node
{
    Interval* interval;

    int maximum;

    struct Node* left;
    struct Node* right;
} Node;

// Function prototypes.
Node* insert(Node* root, Interval interval);
const Node* search(const Node* root, Interval interval);
Node* erase(Node* root, Interval interval);
void in_order(const Node* root);
void destroy(Node* root);

#endif