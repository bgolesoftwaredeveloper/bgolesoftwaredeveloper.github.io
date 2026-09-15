/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Test driver for a Huffman Coding implementation.
 * @details     :   This program demonstrates the full lifecycle of a Huffman Tree:
 * 1. Constructing an optimal tree from alphabet/frequency pairs.
 * 2. Mapping the tree to a CodeTable for O(1) lookup.
 * 3. Recursive traversal to display prefix-free binary codes.
 * 4. Systematic memory deallocation of both the tree and code strings.
 * Compile:
 * clang -Wall -Wextra -Wpedantic ./src/darwin/tree/huffman/main.c ./src/darwin/tree/huffman/huffman.c -I./include -o ./bin/huffman_test
 * @version     :   1.0
 * @date        :   2026-05-07
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/tree/huffman.h"

int main(void)
{
    const char* alphabet[] = {"A", "B", "C", "D", "E"};

    size_t frequencies[] = {5, 9, 12, 13, 16};
    size_t size = sizeof(alphabet) / sizeof(alphabet[0]);

    // 1. Initialize the code table to NULL.
    CodeTable table;

    for (int index = 0; index < 256; index++)
    {
        table.codes[index] = NULL;
    }

    printf("%s", "-- Huffman Tree --\n");
    printf("%s", "Symbols: A, B, C, D, E\n");

    // 2. Build tree.
    HuffmanNode* root = build_tree(alphabet, frequencies, size);

    if (root == NULL)
    {
        fprintf(stderr, "Error: Failed to synthesize Huffman Tree.\b");

        return 1;
    }

    // 3. Map codes into the structure.
    char buffer[MAXIMUM_HEIGHT];

    map_codes(root, table.codes, buffer, 0);

    printf("%c", '\n');

    // 4. Print results from the table.
    printf("%s", "-- Code Table --\n");
    printf("------------------------\n");

    for (int index = 0; index < 256; index++)
    {
        if (table.codes[index] != NULL)
        {
            printf("%s %c %s %s%c", "Char:", (char)index, "Code:", table.codes[index], '\n');
        }
    }

    printf("%s", "------------------------\n");

    int path_buffer[MAXIMUM_HEIGHT];

    // 5. Generate codes.
    printf("%s", "\nGenerated Huffman Codes:\n");
    printf("%s", "------------------------\n");

    generate_codes(root, path_buffer, 0);

    printf("%s", "------------------------\n");

    // 6. Clean up.
    for (int index = 0; index < 256; index++)
    {
        if (table.codes[index] != NULL)
        {
            free(table.codes[index]);
        }
    }

    delete_tree(root);

    printf("%s", "\nTree memory successfully deallocated.\n");

    return 0;
}