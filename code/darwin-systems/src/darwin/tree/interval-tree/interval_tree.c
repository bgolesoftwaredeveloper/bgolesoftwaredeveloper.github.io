/**
 * @file        :   interval_tree.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of an augmented Interval Search Tree.
 * @details     :   Contains the functional implementation for managing an interval 
 * tree. Nodes are ordered by their interval's lower boundary, and 
 * each node tracks the maximum high value among its subtrees to 
 * provide efficient search operations.
 * @version     :   1.0
 * @date        :   2026-05-21
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/tree/interval_tree.h"

/**
 * @brief       :   Checks if two discrete intervals overlap.
 * @details     :   Determines overlap mathematically using the logic that two closed
 *                  intervals $[A, B]$ and $[C, D]$ overlap if $A \le D$ and $C \le B$.
 * @param       :   interval The first interval context.
 * @param       :   other The second interval context to check against.
 * @return      :   true if the intervals overlap, false otherwise.
 */
static inline bool overlaps(const Interval interval, const Interval other)
{
    if (interval.low <= other.high && other.low <= interval.high)
    {
        return true;
    }

    return false;
}

/**
 * @brief       :   Finds the node with the minimum lower boundary value in a subtree.
 * @details     :   Traverses down the leftmost branches of the provided node path to 
 *                  locate the smallest key (inorder successor candidate).
 * @param       :   node The root node of the subtree to search.
 * @return      :   Pointer to the node with the absolute lowest interval boundary.
 */
static inline Node* minimum_value(Node* node)
{
    Node* current = node;

    // Loop down to find the leftmost leaf.
    while (current != NULL && current->left != NULL)
    {
        current = current->left;
    }

    return current;
}

/**
 * @brief       :   Recalculates and updates the maximum high value metadata of a node.
 * @details     :   Inspects the direct left and right children subtrees to re-evaluate
 *                  and set the node's local maximum parameter accurately.
 * @param       :   node The node whose maximum attribute needs adjustment.
 */
static inline void fix_maximum(Node* node)
{
    // Base case: Cannot update a null node.
    if (node == NULL)
    {
        return;
    }

    // Default the maximum value to the node's own high boundary.
    node->maximum = node->interval->high;

    // If a left child exists and has a larger maximum value, update it.
    if (node->left != NULL && node->left->maximum > node->maximum)
    {
        node->maximum = node->left->maximum;
    }

    // If a right child exists and has an even larger maximum value, update it.
    if (node->right != NULL && node->right->maximum > node->maximum)
    {
        node->maximum = node->right->maximum;
    }
}

/**
 * @brief       :   Allocates and initializes memory for a new interval tree node.
 * @details     :   Performs safe structural allocations for both the Node framework 
 *                  and its internal nested Interval instance.
 * @param       :   interval The boundary constraints to copy into the new node.
 * @return      :   Pointer to the newly created Node, or NULL if heap space is exhausted.
 */
static inline Node* construct(Interval interval)
{
    // Allocate a new node.
    Node* new_node = (Node*)malloc(sizeof(Node));

    if (new_node == NULL)
    {
        return NULL;
    }

    // Allocate heap memory for the interval nested Interval structure.
    new_node->interval = (Interval*)malloc(sizeof(Interval));

    if (new_node->interval == NULL)
    {
        free(new_node);

        return NULL;
    }

    // Copy the range fields from argument parameter.
    new_node->interval->low = interval.low;
    new_node->interval->high = interval.high;

    // A brand new node's initial maximum is simply its own high boundary value.
    new_node->maximum = interval.high;

    new_node->left = NULL;
    new_node->right = NULL;

    return new_node;
}

/**
 * @brief       :   Inserts a new interval systematically into the tree.
 * @details     :   Uses the interval low boundary value as the target placement key. 
 *                  Re-evaluates the tree's maximum property as the recursion unwinds.
 * @param       :   root Pointer to the current root of the tree/subtree.
 * @param       :   interval The geometric range properties to add.
 * @return      :   The updated subtree root pointer.
 */
Node* insert(Node* root, Interval interval)
{
    // Base case: If tree/branch is empty, build a new node here.
    if (root == NULL)
    {
        return construct(interval);
    }

    int low = root->interval->low;

    // If new interval's low value is smaller, recurse down this left branch.
    if (interval.low < low)
    {
        root->left = insert(root->left, interval);
    }
    else
    {
        root->right = insert(root->right, interval);
    }

    // As recursion unwinds, expand the local maximum if the new interval's high exceeds it.
    if (root->maximum < interval.high)
    {
        root->maximum = interval.high;
    }

    return root;
}

/**
 * @brief       :   Searches for any interval in the tree that overlaps with the target.
 * @details     :   Employs an optimized search pruning algorithm. If the left child's
 *                  maximum value is smaller than the target's low value, it completely 
 *                  prunes the left side and switches directly to the right.
 * @param       :   root Read-only pointer to the root of the tree/subtree.
 * @param       :   interval The query range constraint to evaluate.
 * @return      :   Pointer to the first overlapping Node found, or NULL if no overlap exists.
 */
const Node* search(const Node* root, Interval interval)
{
    // Base case: Root is completely empty or the current node matches the overlap criteria.
    if (root == NULL || overlaps(*(root->interval), interval))
    {
        return root;
    }

    // If left child exists and its subtree maximum high is >= target's low bound,
    // an overlapping range can mathematically exist somewhere in the left subtree.
    if (root->left != NULL && root->left->maximum >= interval.low)
    {
        return search(root->left, interval);
    }

    // Otherwise, the left subtree cannot possibly hold a match; skip it and search right.
    return search(root->right, interval);
}

/**
 * @brief       :   Erases an exact matching interval from the tree.
 * @details     :   Performs standard binary search tree element deletion. Clears standard
 *                  leaf/single-child environments and processes dual-child nodes via 
 *                  inorder successor replacement before correcting ancestor bounds.
 * @param       :   root Pointer to the current tree/subtree root frame.
 * @param       :   interval The exact range boundary description to clear.
 * @return      :   The updated subtree root pointer.
 */
Node* erase(Node* root, Interval interval)
{
    // Base case: Item not found in the path.
    if (root == NULL)
    {
        return NULL;
    }

    // 1. Traverse left if the target low is less than current low boundary.
    if (interval.low < root->interval->low)
    {
        root->left = erase(root->left, interval);
    }
    // 2. Traverse right if the target low is greater than current low boundary.
    else if (interval.low > root->interval->low)
    {
        root->right = erase(root->right, interval);
    }
    // 3. Low bound matches but high does not, look further right for duplicates.
    else
    {
        // If low bound matches but high does not, look further right for duplicates.
        if (root->interval->high != interval.high)
        {
            root->right = erase(root->right, interval);
        }
        else
        {
            // Found the exact targeted node to remove.
            Node* temp = NULL;

            // Case: No left child (has only right child or no children).
            if (root->left == NULL)
            {
                temp = root->right;

                free(root->interval);
                free(root);

                // Return the right branch to link to parent node.
                return temp;
            }
            // Case: No right child (has only left child).
            else if (root->right == NULL)
            {
                temp = root->left;

                free(root->interval);
                free(root);

                // Return the left branch tom link to parent node.
                return temp;
            }

            // Case: Node has two distinct active child branches.
            // Fetch the smallest key in the right subtree.
            temp = minimum_value(root->right);

            // Overwrite current node data values with the in-order successor's data.
            root->interval->low = temp->interval->low;
            root->interval->high = temp->interval->high;

            // Recurse into the right child path to erase the duplicate successor node.
            root->right = erase(root->right, *(temp->interval));
        }
    }

    // Re-verify and correct the maximum boundaries since the structural changes occurred.
    fix_maximum(root);

    return root;
}

/**
 * @brief       :   Prints the structured tree to stdout using in-order traversal.
 * @details     :   Visits the left child, displays the current node's range boundaries 
 *                  and local metadata maximums, then visits the right child.
 * @param       :   root Read-only pointer to the tree/subtree root element.
 */
void in_order(const Node* root)
{
    if (root == NULL)
    {
        return;
    }

    in_order(root->left);
    printf("%c[%i %i] maximum = %i%c", '\t', root->interval->low, root->interval->high, root->maximum, '\n');
    in_order(root->right);
}

/**
 * @brief       :   Completely tears down the tree framework and deallocates memory.
 * @details     :   Uses a safe post-order traversal mechanism to dismantle sub-elements
 *                  and inner interval pointer elements without leaving dangling targets.
 * @param       :   root Pointer to the root node of the tree to destroy.
 */
void destroy(Node* root)
{
    // Base case: Empty leaf reached.
    if (root == NULL)
    {
        return;
    }

    // Step down to the bottom of the left branch path.
    destroy(root->left);

    // Step down to the bottom of the right branch path.
    destroy(root->right);

    // Free internal tracking structure safely before freeing the parent container node.
    if (root->interval != NULL)
    {
        free(root->interval);
    }

    // Clear actual node heap context.
    free(root);
}