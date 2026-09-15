/**
 * @file        :   bi_directional.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of a bi-directional Binary Search Tree (BST).
 * @details     :   This source file contains structural operations for a standard
 * binary search tree enhanced with explicit parent pointers. It
 * features standard node insertion, deletion, lookup, values updating,
 * recursive memory deallocation, and horizontal structural visualization.
 * @version     :   1.0
 * @date        :   2026-05-24
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/graph/bi_directional.h"

/**
 * @brief       :   Finds the node with the minimum data value in a subtree.
 * @details     :   Traverses deep left down the subtree to locate the absolute lowest node.
 * @param       :   node: The root node of the target subtree.
 * @return      :   Node*: Pointer to the minimum node found, or NULL if subtree is empty.
 */
static inline Node* minimum_value(Node* node)
{
    if (node == NULL)
    {
        return NULL;
    }

    // The leftmost node will always carry the minimum value in a valid BST.
    Node* current = node;

    while (current != NULL && current->left != NULL)
    {
        current = current->left;
    }

    return current;
}

/**
 * @brief       :   Allocates and constructs a new tree node.
 * @param       :   data: Integer value to be assigned to the new node.
 * @param       :   parent: Pointer to the parent node context.
 * @return      :   Node*: Pointer to the allocated node, or NULL on allocation failure.
 */
Node* construct(int data, Node* parent)
{
    Node* new_node = (Node*)malloc(sizeof(Node));

    if (new_node == NULL)
    {
        return NULL;
    }

    new_node->data = data;

    // Explicity nullify pointers to prevent dangling memory states.
    new_node->left = NULL;
    new_node->right = NULL;

    new_node->parent = parent;

    return new_node;
}

/**
 * @brief       :   Recursively inserts an integer value into the tree.
 * @details     :   Navigates standard BST branches while managing up-to-date parent relationships.
 * @param       :   root: Pointer to the root node of the current subtree.
 * @param       :   data: The integer value to inject.
 * @return      :   Node*: Pointer to the updated subtree root.
 */
Node* insert(Node* root, int data)
{
    // Base case: Destination reached.
    if (root == NULL)
    {
        return construct(data, NULL);
    }

    // Traverse left if data is smaller than current node's data.
    if (data < root->data)
    {
        root->left = insert(root->left, data);

        // Explicitly stitch parent relationship back to current frame context.
        if (root->left != NULL)
        {
            root->left->parent = root;
        }
    }
    // Traverse right if data is larger than current node's data.
    else if (data > root->data)
    {
        root->right = insert(root->right, data);

        // Explicitly stictch parent relationship back to current frame context.
        if (root->right != NULL)
        {
            root->right->parent = root;
        }
    }

    return root;
}

/**
 * @brief       :   Searches for a specific integer key inside the tree.
 * @param       :   root: Pointer to the root node of the current subtree.
 * @param       :   key: Target value to locate.
 * @return      :   Node*: Pointer to the matching node, or NULL if not discovered.
 */
Node* search(Node* root, int key)
{
    // Base case: Key is found or reached a dead-end leaf path.
    if (root == NULL || root->data == key)
    {
        return root;
    }

    if (key < root->data)
    {
        return search(root->left, key);
    }

    return search(root->right, key);
}

Node* update(Node* root, int data, int new_data)
{
    // Defensive check to avoid modifying tree if value is missing.
    if (!search(root, data))
    {
        return root;
    }

    root = erase(root, data);
    root = insert(root, new_data);

    return root;
}

Node* erase(Node* root, int key)
{
    if (root == NULL)
    {
        return root;
    }

    // Step 1: Traverse the tree to locate the deletion target node.
    if (key < root->data)
    {
        root->left = erase(root->left, key);
    }
    else if (key > root->data)
    {
        root->right = erase(root->right, key);
    }
    else
    {
        // Step 2: Key found. Handle extraction and structural repair.
        Node* temp = NULL;

        // No left child (Promotes right subtree node or handles leaf).
        if (root->left == NULL)
        {
            temp = root->right;

            if (temp != NULL)
            {
                temp->parent = root->parent;
            }

            free(root);

            return temp;
        }
        // No right child (Promotes left subtree node).
        else if (root->right == NULL)
        {
            temp = root->left;

            if (temp != NULL)
            {
                temp->parent = root->parent;
            }

            free(root);

            return temp;
        }

        // Case 3: Two children. Locate in-order successor (minimum node of right subtree).
        temp = minimum_value(root->right);

        // Copy successor data payload to target location.
        root->data = temp->data;

        // Recurse down right tree to safely dismantle duplicate.
        root->right = erase(root->right, temp->data);

        // Repair bi-directional link if a new right branch root emerged.
        if (root->right != NULL)
        {
            root->right->parent = root;
        }
    }

    return root;
}

/**
 * @brief       :   Displays the tree structure sideways to the console.
 * @details     :   Performs reverse in-order traversal to format depth horizontally using tabs
 *                  and custom unicode branch markers distinguishing left/right alignments.
 * @param       :   root: Pointer to the node context to analyze and output.
 * @param       :   level: The current recursion level layer tracking formatting indents.
 */
void display(const Node* root, int level)
{
    if (root == NULL)
    {
        return;
    }

    // Process right subtree first to display higher elements on to of screen.
    display(root->right, level + 1);

    // Dynamic horizontal spacing calculation based on deep branch tier.
    for (int index = 0; index < level; index++)
    {
        printf("%c", '\t');
    }

    // Render nodes with specialized directional branch graphics.
    if (root->parent != NULL)
    {
        if (root->data > root->parent->data)
        {
            printf("┌── %i (P: %i)%c", root->data, root->parent->data, '\n');
        }
        else
        {
            printf("└── %i (P: %i)%c", root->data, root->parent->data, '\n');
        }
    }
    else
    {
        printf("%i (Root)%c", root->data, '\n');
    }

    // Process left subtree last to map lower elements toward the bottom.
    display(root->left, level + 1);
}

/**
 * @brief       :   Frees all allocated node elements within the tree.
 * @details     :   Performs a post-order traversal to safely drop children before parents.
 * @param       :   root: Pointer to the root node of the tree to wipe.
 */
void clean(Node* root)
{
    if (root == NULL)
    {
        return;
    }

    clean(root->left);
    clean(root->right);

    free(root);
}