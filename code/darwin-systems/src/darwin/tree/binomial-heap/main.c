/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Execution entry point demonstrating a standalone reference 
 * implementation of a Binomial Heap data structure. 
 * * This driver program validates defensive runtime mechanics, 
 * heap consolidation via tree union pipelines, structural tracking 
 * via pointer network layouts, and deterministic deep post-order 
 * memory reclamation.
 * clang -Wall -Wextra -Wpedantic ./src/darwin/tree/binomial-heap/main.c ./src/darwin/tree/binomial-heap/binomial_heap.c -I./include -o ./bin/binomial_heap_test
 * @version     :   1.0
 * @date        :   2026-05-19
 */
#include <stdio.h>

// Include header file.
#include "darwin/tree/binomial_heap.h"

int main(void)
{
    // Initialize the heap pointer to NULL.
    BinomialNode* heap = NULL;

    // The values to work with.
    int values[] = {12, 7, 25, 15, 28, 33, 4};

    // Calculate the size of the values.
    size_t size = sizeof(values) / sizeof(values[0]);

    // Insert each value into the heap.
    for (size_t index = 0; index < size; index++)
    {
        BinomialNode* node = create(values[index]);

        if (node != NULL)
        {
            heap = binomial_union(heap, node);
        }
    }

    // Ensure nodes were inserted.
    if (heap != NULL)
    {
        print_tree(heap);
    }
    else
    {
        printf("%s", "Heap is empty.\n");
    }

    int minimum = 0;

    // Retrieve the minimum item.
    if (retrieve_minimum(heap, &minimum))
    {
        printf("%s %i%c", "Minimum value inside heap is:", minimum, '\n');
    }

    int extracted_key = 0;

    // Extract the minimum from the heap.
    if (extract_minimum(&heap, &extracted_key))
    {
        printf("%s %i%c", "Successfully extracted absolute minimum:", extracted_key, '\n');
    }

    // Clean up.
    free_heap(heap);

    // Guard against dangling pointers.
    heap = NULL;

    return 0;
}