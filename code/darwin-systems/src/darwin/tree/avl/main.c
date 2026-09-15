/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Demonstration and manual testing suite for the AVL Tree library.
 * @details     :   Validates core AVL operations including balanced insertion, 
 * dynamic node updates, and re-balancing erasures. Also serves 
 * as a reference for proper memory lifecycle management.
 * Compile:
 * clang -Wall -Wextra -Wpedantic ./src/darwin/tree/avl/main.c ./src/darwin/tree/avl/avl.c -I./include -o ./bin/avl_test
 * @version     :   1.0
 * @date        :   2026-04-19
 */
#include <stdio.h>

// Include header files.
#include "darwin/tree/avl.h"

// Function prototypes.
void print_inorder(const Node* root);

int main(void)
{
    // Initialization.
    Node* root = initialize(30);

    bool success = false;

    // Insert a few nodes to work with.
    root = insert(root, 20);
    root = insert(root, 40);
    root = insert(root, 10);
    root = insert(root, 25);

    printf("%s", "Current Tree (In-order):\n");
    print_inorder(root);
    printf("%s", "\n\n");

    // Updating node.
    printf("%s", "--- 2. Updating Node --\n");
    
    // Changing 40 to 15 will trigger a re-balance as 15 moves to the left side.
    root = update(root, 40, 15, &success);

    if (success)
    {
        printf("%s", "Tree after update:\n");
        print_inorder(root);
        printf("%s", "\n\n");
    }
    else
    {
        fprintf(stderr, "Update was not successful!\n");
    }
    
    // Deleting node.
    printf("%s", "--- 3. Deleting Node ---\n");

    // Removing the root (30) forces the tree to promote an in-order successor.
    root = erase(root, 30);

    printf("%s", "Final Tree: ");
    print_inorder(root);
    printf("%s", "\n\n");

    // Cleaning up.
    printf("%s", "--- 4. Cleaning Up ---\n");

    clean_up(root);

    root = NULL;

    printf("%s", "Memory cleared successfully.\n");

    return 0;
}

/**
 * @brief       :   Performs an in-order traversal to print the tree structure.
 * @details     :   Traverses the tree (Left, Root, Right) to display keys and 
 *                  their respective heights. This provides a sorted view of 
 *                  the data while allowing for manual balance verification.
 * @param       :   root: A constant pointer to the root node of the tree.
 * @return      :   void
 */
void print_inorder(const Node* root)
{
    if (root == NULL)
    {
        return;
    }

    print_inorder(root->left_child);
    printf("%c%i (height: %i)%c", '\t', root->key, root->height, '\n');
    print_inorder(root->right_child);
}