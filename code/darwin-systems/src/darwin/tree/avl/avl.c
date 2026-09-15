/**
 * @file        :   avl.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of a self-balancing AVL binary search tree.
 * @details     :   Provides logarithmic time complexity for insertion, deletion, 
 * and search by maintaining a balance factor between subtrees.
 * @version     :   1.0
 * @date        :   2026-04-19
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/tree/avl.h"

/**
 * @brief       :   Returns the larger of two integers.
 * @param       :   compare: First value to compare.
 * @param       :   against: Second value to compare.
 * @return      :   int:     The maximum value.
 */
static inline int retrieve_maximum(int compare, int against)
{
    return (compare > against) ? compare : against;
}

/**
 * @brief       :   Safely retrieves the height of a node.
 * @param       :   node: Pointer to the node.
 * @return      :   int:  The height of the node, or 0 if NULL.
 */
static inline int retrieve_node_height(Node* node)
{
    return (node == NULL) ? 0 : node->height;
}

/**
 * @brief       :   Locates the node with the smallest key in a subtree.
 * @details     :   Traverses to the leftmost leaf. Used during deletion.
 * @param       :   node:  The root of the subtree to search.
 * @return      :   Node*: Pointer to the minimum node.
 */
static inline Node* retrieve_minimum_value(Node* node)
{
    if (node == NULL)
    {
        return NULL;
    }

    Node* current = node;

    // Follow the left pointers until we reach the smallest value.
    while (current->left_child != NULL)
    {
        current = current->left_child;
    }

    return current;
}

/**
 * @brief       :   Performs a left rotation on an unbalanced subtree.
 * @param       :   unbalanced_root: The pivot node for the rotation.
 * @return      :   Node*:           The new root of the subtree.
 */
static inline Node* rotate_left(Node* unbalanced_root)
{
    // Pointer reassignment: Move the right child up to the root.
    Node* new_root = unbalanced_root->right_child;
    Node* subtree_to_move = new_root->left_child;

    // Perform the actual rotation.
    new_root->left_child = unbalanced_root;
    unbalanced_root->right_child = subtree_to_move;

    // Recalculate heights starting from the lower node in the new structure. 
    unbalanced_root->height = 1 + retrieve_maximum(retrieve_node_height(unbalanced_root->left_child),
        retrieve_node_height(unbalanced_root->right_child));
    
    new_root->height = 1 + retrieve_maximum(retrieve_node_height(new_root->left_child),
        retrieve_node_height(new_root->right_child));

    return new_root;
}

/**
 * @brief       :   Performs a right rotation on an unbalanced subtree.
 * @param       :   unbalanced_root: The pivot node for the rotation.
 * @return      :   Node*:           The new root of the subtree.
 */
static inline Node* rotate_right(Node* unbalanced_root)
{
    // Pointer reassignment: Move the left child up to the root.
    Node* new_root = unbalanced_root->left_child;
    Node* subtree_to_move = new_root->right_child;

    // Perform the actual rotation.
    new_root->right_child = unbalanced_root;
    unbalanced_root->left_child = subtree_to_move;

    // Recalculate heights starting from the lower node in the new structure.
    unbalanced_root->height = 1 + retrieve_maximum(retrieve_node_height(unbalanced_root->left_child),
        retrieve_node_height(unbalanced_root->right_child));

    new_root->height = 1 + retrieve_maximum(retrieve_node_height(new_root->left_child), retrieve_node_height(new_root->right_child));

    return new_root;
}

/**
 * @brief       :   Calculates the difference in height between subtrees.
 * @param       :   node: Pointer to the node to check.
 * @return      :   int:  Balance factor (left height - right height).
 */
static inline int calculate_balance_factor(Node* node)
{
    if (node == NULL)
    {
        return 0;
    }

    // A balance factor > 1 means left-heavy; < -1 means right-heavy.
    return retrieve_node_height(node->left_child) - retrieve_node_height(node->right_child);
}

/**
 * @brief       :   Searches for a specific key in the tree.
 * @param       :   node: The root of the subtree to search.
 * @param       :   key:  The value to look for.
 * @return      :   Node*: Pointer to the node if found, NULL otherwise.
 */
static inline Node* search(Node* node, int key)
{
    if (node == NULL || node->key == key)
    {
        return node;
    }

    if (key > node->key)
    {
        return search(node->right_child, key);
    }

    return search(node->left_child, key);
}

/**
 * @brief       :   Allocates and initializes a new AVL node.
 * @param       :   key:   The integer value to store.
 * @return      :   Node*: Pointer to the newly allocated node.
 */
Node* initialize(int key)
{
    // Request memory from the heap for a single node.
    Node* node = (Node*)malloc(sizeof(Node));

    if (node == NULL)
    {
        return NULL;
    }

    // Set initial state.
    node->key = key;
    node->left_child = NULL;
    node->right_child = NULL;
    node->height = 1;

    return node;
}

/**
 * @brief       :   Inserts a key into the tree and maintains balance.
 * @details     :   Standard BST insertion followed by recursive re-balancing.
 * @param       :   node: Current root of the subtree.
 * @param       :   key:  The value to insert.
 * @return      :   Node*: The root of the resulting balanced tree.
 */
Node* insert(Node* node, int key)
{
    // 1. Recursive search for the insertion point.
    if (node == NULL)
    {
        return initialize(key);
    }

    if (key < node->key)
    {
        node->left_child = insert(node->left_child, key);
    }
    else if (key > node->key)
    {
        node->right_child = insert(node->right_child, key);
    }
    else
    {
        // AVL trees usually do not allow duplicate keys.
        return node;
    }

    // 2. Update height of this ancestor node on the way back up to the stack.
    node->height = 1 + retrieve_maximum(retrieve_node_height(node->left_child),
        retrieve_node_height(node->right_child));

        
    // 3. Check for imbalances.
    int balance = calculate_balance_factor(node);

    // Left-Left (LL) case.
    if (balance > 1 && key < node->left_child->key)
    {
        return rotate_right(node);
    }

    // Right-Right (RR) case.
    if (balance < -1 && key > node->right_child->key)
    {
        return rotate_left(node);
    }

    // Left-Right (LR) case.
    if (balance > 1 && key > node->left_child->key)
    {
        node->left_child = rotate_left(node->left_child);

        return rotate_right(node);
    }

    // Right-Left (RL) case.
    if (balance < -1 && key < node->right_child->key)
    {
        node->right_child = rotate_right(node->right_child);

        return rotate_left(node);
    }

    return node;
}

/**
 * @brief       :   Updates an existing key by re-positioning it.
 * @details     :   Deletes the old key and inserts the new one to preserve properties.
 * @param       :   root:    Pointer to the tree root.
 * @param       :   key:     The key to be replaced.
 * @param       :   new_key: The new key value.
 * @param       :   success: A flag to determine if the update was successful.
 * @return      :   Node*:   The root of the updated tree.
 */
Node* update(Node* root, int key, int new_key, bool* success)
{
    if (root == NULL)
    {
        return NULL;
    }

    if (key == new_key)
    {
        return root;
    }

    // Check if the new key already exists to prevent losing a node
    // during the delete-insert cycle. 
    if (search(root, new_key) != NULL)
    {
        if (success)
        {
            *success = false;
        }

        return root;
    }

    // In an AVL tree, we cannot simply change the key in place because it would
    // violate the BST property and potentially the balance property.
    root = erase(root, key);
    root = insert(root, new_key);

    if (success)
    {
        *success = true;
    }

    return root;
}

/**
 * @brief       :   Removes a key from the tree and re-balances.
 * @param       :   root: Pointer to the tree root.
 * @param       :   key:  The key value to remove.
 * @return      :   Node*: The root of the resulting balanced tree.
 */
Node* erase(Node* root, int key)
{
    // 1. Recursive search for the target node to delete.
    if (root == NULL)
    {
        return root;
    }

    if (key < root->key)
    {
        root->left_child = erase(root->left_child, key);
    }
    else if (key > root->key)
    {
        root->right_child = erase(root->right_child, key);
    }
    else
    {
        // Target node found.
        if ((root->left_child == NULL) || (root->right_child == NULL))
        {
            // Case: Node with only one child or no children.
            Node* temp = root->left_child ? root->left_child : root->right_child;

            if (temp == NULL)
            {
                temp = root;

                root = NULL;
            }
            else
            {
                Node* free_me = root;

                root = temp;
                temp = free_me;
            }

            free(temp);
        }
        else
        {
            // Case: Node with two children.
            // Replace with the in-order successor (smallest in right subtree).
            Node* temp = retrieve_minimum_value(root->right_child);

            root->key = temp->key;

            root->right_child = erase(root->right_child, temp->key);
        }
    }

    // If the tree only had one node, return.  
    if (root == NULL)
    {
        return root;
    }

    // 2. Update height after deletion.  
    root->height = 1 + retrieve_maximum(retrieve_node_height(root->left_child),
        retrieve_node_height(root->right_child));

    // 3. Re-balance if the deletion created an imbalance.
    int balance = calculate_balance_factor(root);

    // Left-Left (LL) case.
    if (balance > 1 && calculate_balance_factor(root->left_child) >= 0)
    {
        return rotate_right(root);
    }

    // Left-Right (LR) case.
    if (balance > 1 && calculate_balance_factor(root->left_child) < 0)
    {
        root->left_child = rotate_left(root->left_child);

        return rotate_right(root);
    }

    // Right-Right (RR) case.
    if (balance < -1 && calculate_balance_factor(root->right_child) <= 0)
    {
        return rotate_left(root);
    }

    // Right-Left (RL) case.
    if (balance < -1 && calculate_balance_factor(root->right_child) > 0)
    {
        root->right_child = rotate_right(root->right_child);

        return rotate_left(root);
    }

    return root;
}

/**
 * @brief       :   Recursively frees all memory in the tree.
 * @details     :   Uses post-order traversal to ensure children are freed before parents.
 * @param       :   root: Pointer to the root of the tree to clear.
 * @return      :   void
 */
void clean_up(Node* root)
{
    if (root == NULL)
    {
        return;
    }

    // Tranverse to the bottom of the tree first.
    clean_up(root->left_child);
    clean_up(root->right_child);

    // Free the current node only after its children are gone.
    free(root);
}