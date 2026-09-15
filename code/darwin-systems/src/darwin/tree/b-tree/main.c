/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Demonstration executable for the B-Tree implementation.
 * @details     :   Validates structural integrity by executing sequential insertions,
 * lookups, and deletion cycles on public interface functions.
 * Compile:
 * clang -Wall -Wextra -Wpedantic ./src/darwin/tree/b-tree/main.c ./src/darwin/tree/b-tree/b_tree.c -I./include -o ./bin/b_tree_test
 * @version     :   1.0
 * @date        :   2026-05-21
 */
#include <stdio.h>

// Include header files.
#include "darwin/tree/b_tree.h"

int main(void)
{
    Node* root = NULL;

    printf("%s", "-- B-Tree demonstration --\n");

    // Anchor the root and insert nodes to work with.
    insert(&root, 10);
    insert(&root, 20);
    insert(&root, 5);
    insert(&root, 6);
    insert(&root, 12);
    insert(&root, 30);
    insert(&root, 7);
    insert(&root, 17);

    // Demonstration that we have implemented duplicate entry defense.
    insert(&root, 10);

    printf("%s", "Performing search operations:\n");

    int keys[] = {12, 30, 99};

    for (size_t index = 0; index < 3; index++)
    {
        int query = keys[index];

        // Query the node.
        const Node* node = search(root, query);

        if (node != NULL)
        {
            printf("%s [%2i] %s", "\tKey", query, "was found in the B-Tree structure.\n");
        }
        else
        {
            printf("%s", "\tKey was not found in the B-Tree structure.\n");
        }
    }

    printf("%c", '\n');

    // Erase. 
    erase(&root, 5);

    // Make sure its deleted.
    Node* verify_deleted = search(root, 12);

    if (verify_deleted == NULL)
    {
        printf("%s", "Confirmation: Key [12] is no longer accessible.\n\n");
    }
    else
    {
        printf("%s", "Warning key [12] was unexpectedly discovered.\n\n");
    }

    // Clean up.
    clean(root);

    // Avoid dangling pointers.
    root = NULL;

    return 0;
}