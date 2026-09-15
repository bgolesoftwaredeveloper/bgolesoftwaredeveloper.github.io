/**
 * @file        :   rope.c
 * @author      :   Braiden Gole
 * @brief       :   Defensive string pattern matching implementation for Rope data structures.
 * @details     :   Provides functions to safely calculate the total length of a Rope,
 * retrieve characters at absolute indices across discrete tree blocks, 
 * and execute substring searches spanning across leaf node boundaries 
 * without requiring contiguous string flattening.
 * @version     :   1.0
 * @date        :   2026-06-06
 */
#include <stdio.h>
#include <string.h>

// Include header files.
#include "darwin/pattern-matching/rope.h"

/**
 * @brief       :   Retrieves a single character at a specific absolute index in the Rope.
 * @details     :   Traverses down the binary tree structure. If an internal node is encountered,
 *                  it compares the index against the left child's weight to route left or right.
 *                  If a leaf is reached, it reads directly from the string buffer.
 * @param       :   root: Constant pointer to the constant root or sub-root Node.
 * @param       :   index: The absolute index of the character to look up.
 * @param       :   out: Pointer to a char buffer where the found character will be stored.
 * @return      :   bool: true if the character was safely retrieved, false if out of bounds or pointers are NULL.
 */
static inline bool at(const Node* const root, size_t index, char* const out)
{
    // Validate inputs are not NULL pointers.
    if (root == NULL || out == NULL)
    {
        return false;
    }

    // Initialize a mutable traversal pointer to walk the tree.
    const Node* traversal_node = root;

    // Loop through the tree hierachy until running out of valid nodes.
    while (traversal_node != NULL)
    {
        // Check if the current node is a leaf (both children are NULL).
        if (traversal_node->left == NULL && traversal_node->right == NULL)
        {
            // Verify that the string buffer exists and the index falls within this leaf's weight boundary.
            if (traversal_node->leaf != NULL && index < traversal_node->weight)
            {
                *out = traversal_node->leaf[index];

                return true;
            }

            return false;
        }

        // An internal node must posses at least a valid left branch.
        if (traversal_node->left == NULL)
        {
            return false;
        }

        // If index is smaller than left-subtree weight, dive left.
        if (index < traversal_node->weight)
        {
            traversal_node = traversal_node->left;
        }
        else
        {
            // Subtract the left-subtree weight from our index offset and dive right.
            index -= traversal_node->weight;

            traversal_node = traversal_node->right;
        }
    }

    return false;
}

/**
 * @brief       :   Recursively calculates the total character length of the Rope structure.
 * @details     :   Summates weights through the right-side branches since internal node weights 
 *                  already represent the total length of their respective left subtrees.
 * @param       :   root: Constant pointer to the constant root Node of the Rope.
 * @return      :   size_t: The total cumulative number of characters stored across all leaf nodes.
 */
static inline size_t calculate_rope_length(const Node* const root)
{
    // An empty node of sub-tree contributes zero to the total length.
    if (root == NULL)
    {
        return 0;
    }

    // If it is a leaf node, its weight is exactly the length of its string segment.
    if (root->left == NULL && root->right == NULL)
    {
        return root->weight;
    }

    // Total lengths equals the current left subtree capacity plus the right childs recursive length.
    return root->weight + calculate_rope_length(root->right);
}

/**
 * @brief       :   Scans the Rope structure to find the first occurrence of a string pattern.
 * @details     :   Executes a dynamic character-by-character validation matrix. It handles 
 *                  complex pattern matching safely across distinct physical memory chunk barriers.
 * @param       :   root: Constant pointer to the constant root Node of the Rope.
 * @param       :   pattern: The null-terminated substring sequence to search for.
 * @param       :   out_index: Pointer to a size_t buffer that receives the initial matching index offset.
 * @return      :   bool: true if an exact match is confirmed, false if invalid or no match is found.
 */
bool rope(const Node* const root, const char* const pattern, size_t* const out_index)
{
    // Validate that all operational pointers are safe to read and write.
    if (root == NULL || pattern == NULL || out_index == NULL)
    {
        return false;
    }

    // Cahce the pattern length to establish.
    size_t pattern_length = strlen(pattern);

    // An empty search pattern vacuously matches at the absolute index.
    if (pattern_length == 0)
    {
        *out_index = 0;

        return true;
    }

    // Compute the entire capacity of the target rope.
    size_t rope_length = calculate_rope_length(root);

    // Optimization: A pattern cannot physically reside inside a shorter string container.
    if (pattern_length > rope_length)
    {
        return false;
    }

    // Establish the maximum search boundary offset to prevent scanning past out-of-bounds index points.
    size_t maximum = rope_length - pattern_length;

    // Shift the search window incrementally across teh virtual text array layout.
    for (size_t index = 0; index <= maximum; index++)
    {
        bool found = true;

        // Match characters of the pattern against consecutive tree indices.
        for (size_t pattern_index = 0; pattern_index < pattern_length; pattern_index++)
        {
            char rope_character = '\0';

            size_t absolute_rope_position = index + pattern_index;

            // Safely fetch the current structural character through tree-walk abstraction.
            if (!at(root, absolute_rope_position, &rope_character))
            {
                found = false;

                break;
            }

            // Mismatch detected: Abort current window and advance outer shift window.
            if (rope_character != pattern[pattern_index])
            {
                found = false;

                break;
            }
        }

        // Match verified: If the inner loop fully completed without breaking, a match is confirmed.
        if (found)
        {
            *out_index = index;

            return true;
        }
    }

    // Complete text scanned without confirming any valid matches.
    return false;
}