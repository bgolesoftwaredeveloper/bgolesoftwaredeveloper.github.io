/**
 * @file        :   binary_tree.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation file for a custom Binary Search Tree (BST).
 * Provides memory allocation, data insertion, searching, 
 * element node deletion (erasing), and structural traversal 
 * utilities utilizing the unsigned size_t data type.
 * @version     :   1.0
 * @date        :   2026-05-17
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/tree/binary_tree.h"

/**
 * @brief       :   Locates the node with the smallest value in a subtree.
 * @details     :   Traverses down to the leftmost leaf. Used during deletion.
 * @param       :   root: The root of the subtree to search (Read-Only).
 * @return      :   const Node*: Pointer to the minimum node.
 */
static inline const Node* find_minimum(const Node* root)
{
    if (root == NULL)
    {
        return NULL;
    }

    // Follow the left pointers until we reach the smallest value.
    while (root->left != NULL)
    {
        root = root->left;
    }

    return root;
}

/**
 * @brief       :   Allocates and initializes a new binary tree node.
 * @param       :   value: The size_t value to store in the node.
 * @return      :   Node*: Pointer to the newly allocated node.
 */
static inline Node* make(size_t value)
{
    // Requested memory from the heap for a single node structure.
    Node* new_node = (Node*)malloc(sizeof(Node));

    if (new_node == NULL)
    {
        return NULL;
    }

    // Set initial state.
    new_node->data = value;
    new_node->left = NULL;
    new_node->right = NULL;

    return new_node;
}

/**
 * @brief       :   Inserts a value into the tree structure.
 * @details     :   Standard recursive BST insertion searching for a open leaf spot.
 * @param       :   root:  Current root of the subtree.
 * @param       :   value: The size_t value to insert.
 * @return      :   Node*: The root of the resulting tree.
 */
Node* insert(Node* root, size_t value)
{
    if (root == NULL)
    {
        return make(value);
    }

    // Direct the value to its correct structural branch.
    if (value < root->data)
    {
        root->left = insert(root->left, value);
    }
    else
    {
        root->right = insert(root->right, value);
    }

    return root;
}

/**
 * @brief       :   Searches for a specific target value in the tree.
 * @param       :   root:   The root of the subtree to search (Read-Only).
 * @param       :   target: The value to look for.
 * @return      :   bool:   True if found, false otherwise.
 */
bool search(const Node* root, size_t target)
{
    if (root == NULL)
    {
        return false;
    }

    if (root->data == target)
    {
        return true;
    }

    // Halve the search space using properties of BST.
    return (target < root->data) ? search(root->left, target) : search(root->right, target);
}

/**
 * @brief       :   Removes a value from the tree and restructures.
 * @param       :   root:   Pointer to the tree root.
 * @param       :   target: The size_t value to remove.
 * @return      :   Node*:  The root of the resulting tree.
 */
Node* erase(Node* root, size_t target)
{
    if (root == NULL)
    {
        return NULL;
    }

    // 1. Recursive search for the target node to delete.
    if (target < root->data)
    {
        root->left = erase(root->left, target);
    }
    else if (target > root->data)
    {
        root->right = erase(root->right, target);
    }
    else
    {
        // 2. Target node found. Handle restructuring cases.
        if (root->left == NULL)
        {
            // Case: Node with only a right child or no children.
            Node* right = root->right;
            
            free(root);

            return right;
        }
        else if (root->right == NULL)
        {
            // Case: Node with only a left child.
            Node* left = root->left;

            free(root);

            return left;
        }

        // Case: Node with two children.
        // Replace with the in-order successor (smallest in right subtree).
        const Node* temp = find_minimum(root->right);

        root->data = temp->data;

        root->right = erase(root->right, temp->data);
    }

    return root;
}

/**
 * @brief       :   Executes a pre-order traversal (Root, Left, Right).
 * @param       :   root: Pointer to the tree root (Read-Only).
 * @return      :   void
 */
void pre_order(const Node* root)
{
    if (root != NULL)
    {
        // Process parent first, then dive into children.
        printf("%zu ", root->data);

        pre_order(root->left);
        pre_order(root->right);
    }
}

/**
 * @brief       :   Executes an in-order traversal (Left, Root, Right).
 * @details     :   Prints elements in sorted, ascending order.
 * @param       :   root: Pointer to the tree root (Read-Only).
 * @return      :   void
 */
void in_order(const Node* root)
{
    if (root != NULL)
    {
        in_order(root->left);

        // Process elements sequentially out of the left tree.
        printf("%zu ", root->data);

        in_order(root->right);
    }
}

/**
 * @brief       :   Executes a post-order traversal (Left, Right, Root).
 * @param       :   root: Pointer to the tree root (Read-Only).
 * @return      :   void
 */
void post_order(const Node* root)
{
    if (root != NULL)
    {
        post_order(root->left);
        post_order(root->right);

        // Process children completely before tracking back to parents.
        printf("%zu ", root->data);
    }
}

/**
 * @brief       :   Recursively frees all memory in the tree.
 * @details     :   Uses post-order traversal to ensure children are freed before parents.
 * @param       :   root: Pointer to the root of the tree to clear.
 * @return      :   void
 */
void clean(Node* root)
{
    if (root != NULL)
    {
        // Traverse to the bottom of the tree first.
        clean(root->left);
        clean(root->right);

        // Free the current node only after its children are gone
        free(root);
    }
}