/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Demonstration program for a custom Binary Search Tree (BST) API 
 * handling unsigned size_t data types. It benchmarks core tree operations 
 * including memory allocation, structural node insertion, deep search, 
 * multiple recursive traversal ordering strategies, node deletion, and 
 * post-order memory clearance
 * clang -Wall -Wextra -Wpedantic ./src/darwin/tree/binary-tree/main.c ./src/darwin/tree/binary-tree/binary_tree.c -I./include -o ./bin/binary_tree_test
 * @version     :   1.0
 * @date        :   2026-05-17
 */
#include <stdio.h>

// Include header files.
#include "darwin/tree/binary_tree.h"

int main(void)
{
    // Initialize the root pointer.
    Node* root = NULL;

    printf("%s", "-- Inserting Nodes --\n");

    // Anchor the root.
    root = insert(root, 50);

    // The root is anchored insert more entries into the root node.
    insert(root, 30);
    insert(root, 20);
    insert(root, 40);
    insert(root, 70);
    insert(root, 60);
    insert(root, 80);

    // Pre-order traversal.
    printf("%s", "Pre-order traversal: ");
    pre_order(root);
    printf("%c", '\n');

    // In-order traversal.
    printf("%s", "In-order traversal: ");
    in_order(root);
    printf("%c", '\n');

    // Post-order traversal.
    printf("%s", "Post-order traversal: ");
    post_order(root);
    printf("%c", '\n');

    size_t fourty = 40;
    size_t ninety_nine = 99;

    printf("%s", "-- Searching --\n");

    // Search for the targets.
    printf("%s %zu %s %s", "Is", fourty, "in the tree?", search(root, fourty) ? "Yes\n" : "No\n");
    printf("%s %zu %s %s", "Is", ninety_nine, "in the tree?", search(root, ninety_nine) ? "Yes\n" : "No\n");

    printf("%s", "-- Erasing --\n");

    // Erase what nodes exists in the tree.
    root = erase(root, 20);
    root = erase(root, 70);

    // Show the tree after erasing.
    printf("%s", "In-order traversal: ");
    in_order(root);
    printf("%c", '\n');

    // Clean allocated memory.
    clean(root);

    // Clear out the primary ponter safely prevent dangling reference vulnerbilities.
    root = NULL;

    return 0;
}