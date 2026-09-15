/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Demonstration of bi-directional Binary Search Tree operations.
 * @details     :   This program serves as a practical demonstration of the bi-directional 
 * BST API. It walks through building a tree structure, executing structural 
 * modifications like updating and erasing elements, and displaying horizontal 
 * tree maps to demonstrate real-time parent-pointer tracking.
 * clang -Wall -Wextra -Wpedantic ./src/darwin/graph/bi-directional/main.c ./src/darwin/graph/bi-directional/bi_directional.c -I./include -o ./bin/bi_directional_test
 * @version     :   1.0
 * @date        :   2026-05-23
 */
#include <stdio.h>

// Include header files.
#include "darwin/graph/bi_directional.h"

int main(void)
{
    // Initialize the root.
    Node* root = NULL;

    // Anchor the root.
    root = insert(root, 50);
    root = insert(root, 30);
    root = insert(root, 70);
    root = insert(root, 20);

    // Show the first state of the tree with its initial elements.
    display(root, 0);

    // Demonstrate updating a value by changing leaf node 20 to 25.
    root = update(root, 20, 25);

    // Show. the second state of the tree reflecting the modification.
    display(root, 0);

    // Demonstrate erasing node 30 to see child promotion and parent repair.
    root = erase(root, 30);

    // Show the final layout of the tree after erasing.
    display(root, 0);

    // Free all remaining nodes to finish the execution cleanly.
    clean(root);
}