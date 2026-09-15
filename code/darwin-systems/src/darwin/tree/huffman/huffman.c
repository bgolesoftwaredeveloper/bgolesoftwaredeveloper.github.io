/**
 * @file        :   huffman.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the Huffman Coding algorithm for data compression.
 * @details     :   Provides functions for building an optimal prefix tree based on character 
 * frequencies, generating bit-codes, and managing a priority queue (Minimum-Heap).
 * @version     :   1.0
 * @date        :   2026-05-07
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include header files.
#include "darwin/tree/huffman.h"

/**
 * @brief       :   Swaps two HuffmanNode pointers.
 * @param       :   source: Pointer to the first node pointer.
 * @param       :   destination: Pointer to the second node pointer.
 * @return      :   void
 */
static inline void swap(HuffmanNode** source, HuffmanNode** destination)
{
    HuffmanNode* temp = *source;
    *source = *destination;
    *destination = temp;
}

/**
 * @brief       :   Allocates and initializes a new Huffman leaf or internal node.
 * @param       :   label: The character to store (use '#' for internal nodes).
 * @param       :   frequency: The weight or occurrence count of the character.
 * @return      :   HuffmanNode*: Pointer to the newly allocated node.
 */
static HuffmanNode* new(char label, size_t frequency)
{
    // Request memory for a single node structure.
    HuffmanNode* new_node = (HuffmanNode*)malloc(sizeof(HuffmanNode));

    if (new_node == NULL)
    {
        return NULL;
    }

    // Initialize node data and frequency.
    new_node->data = label;
    new_node->frequency = frequency;
    new_node->left_child = NULL;
    new_node->right_child = NULL;

    return new_node;
}

/**
 * @brief       :   Maintains the Min-Heap property by moving a node down.
 * @details     :   Ensures the parent node always has a smaller frequency than its children.
 * @param       :   queue: Pointer to the priority queue.
 * @param       :   index: The current index of the node to sink.
 * @return      :   void
 */
static void sink(PriorityQueue* queue, size_t index)
{
    size_t minimum = index;

    // Calculate standard binary heap children indices.
    size_t left_index = 2 * index + 1;
    size_t right_index = 2 * index + 2;

    // Check if left child exists and is smaller than the current minimum.
    if (left_index < queue->size && queue->list[left_index]->frequency < queue->list[minimum]->frequency)
    {
        minimum = left_index;
    }

    // Check if right child exists and is smaller than the current minimum.
    if (right_index < queue->size && queue->list[right_index]->frequency < queue->list[minimum]->frequency)
    {
        minimum = right_index;
    }

    // If the minimum is no longer the parent, swap and recurse downward.
    if (minimum != index)
    {
        swap(&queue->list[minimum], &queue->list[index]);

        sink(queue, minimum);
    }
}

/**
 * @brief       :   Maintains the Min-Heap property by moving a node up.
 * @details     :   Used during insertion to place a node in its correct hierarchical position.
 * @param       :   queue: Pointer to the priority queue.
 * @param       :   index: The current index of the node to swim.
 * @return      :   void
 */
static void swim(PriorityQueue* queue, size_t index)
{
    if (queue == NULL)
    {
        return;
    }

    // Move up the tree until we hit the root (index 0).
    while (index > 0)
    {
        // Find the parent index.
        size_t parent = (index - 1) / 2;

        // If the current node is larger than or equal to parent, heap propery is satisfied.
        if (queue->list[index]->frequency >= queue->list[parent]->frequency)
        {
            break;
        }

        // Otherwise, swap with parent and move up to the parent's.
        swap(&queue->list[index], &queue->list[parent]);

        index = parent;
    }
}

/**
 * @brief       :   Inserts a node into the Priority Queue.
 * @param       :   queue: Pointer to the priority queue.
 * @param       :   node: The HuffmanNode to insert.
 * @return      :   void
 */
static void push(PriorityQueue* queue, HuffmanNode* node)
{
    if (queue == NULL || node == NULL)
    {
        return;
    }

    // Ensure we dpn't exceed the allocated memory for the queue list.
    if (queue->size < queue->maximum_capacity)
    {
        // Add new node to the end of the heap.
        queue->list[queue->size] = node;

        // Re-balance the heap by moving the new node up.
        swim(queue, queue->size);

        queue->size++;
    }
}

/**
 * @brief       :   Extracts the node with the lowest frequency from the queue.
 * @param       :   queue: Pointer to the priority queue.
 * @return      :   HuffmanNode*: Pointer to the minimum node, or NULL if empty.
 */
static HuffmanNode* pop(PriorityQueue* queue)
{
    // Return NULL if the queue is empty of uninitialized.
    if (queue == NULL || queue->size == 0)
    {
        return NULL;
    }

    // The root of a Min-Heap (index 0) is always the minimum value.
    HuffmanNode* top = queue->list[0];

    // Replace the root with the last element in the heap and reduce size.
    queue->list[0] = queue->list[--queue->size];

    // Re-balance the heap by sinking the new root to its proper place.
    sink(queue, 0);

    return top;
}

/**
 * @brief       :   Synthesizes the optimal Huffman tree from a set of characters.
 * @details     :   Utilizes a greedy approach to merge the two nodes with the lowest frequencies.
 * @param       :   alphabet: Array of character strings.
 * @param       :   frequencies: Array of corresponding frequency weights.
 * @param       :   size: The number of unique characters.
 * @return      :   HuffmanNode*: The root of the final Huffman tree.
 */
HuffmanNode* build_tree(const char* alphabet[], const size_t frequencies[], size_t size)
{
    // Allocate the wrapper structure for the priority queue.
    PriorityQueue* queue = (PriorityQueue*)malloc(sizeof(PriorityQueue));

    if (queue == NULL)
    {
        return NULL;
    }

    // Initialize queue metadata and allocate the internal list of pointers.
    queue->size = 0;
    queue->maximum_capacity = size;
    queue->list = (HuffmanNode**)malloc(sizeof(HuffmanNode*) * size);

    if (queue->list == NULL)
    {
        free(queue);

        return NULL;
    }

    // Step 1: Create a leaf node for every character and push it into the Min-Heap.
    for (size_t index = 0; index < size; index++)
    {
        push(queue, new(alphabet[index][0], frequencies[index]));
    }

    // Step 2: Iteratively merge nodes until only one root remains.
    while (queue->size > 1)
    {
        // Extract the two nodes with the lowest frequency.
        HuffmanNode* left = pop(queue);
        HuffmanNode* right = pop(queue);

        // Create a new internal node ('#') with combined frequency.
        HuffmanNode* parent = new('#', left->frequency + right->frequency);

        if (parent == NULL)
        {
            free(queue->list);
            free(queue);

            return NULL;
        }

        // Build the tree link: Parent points to the two extracted nodes.
        parent->left_child = left;
        parent->right_child = right;

        // Push the new internal node back into the queue for further merging.
        push(queue, parent);
    }

    // The last remaining node in the queue is teh root of the complete tree.
    HuffmanNode* root = pop(queue);

    // Free the queue management structures (the nodes themselves persist as the tree).
    free(queue->list);
    free(queue);

    return root;
}

/**
 * @brief       :   Calculates the maximum height of the Huffman tree.
 * @param       :   root: Pointer to the root of the tree.
 * @return      :   size_t: The height of the tree.
 */
size_t get_height(HuffmanNode* root)
{
    // An empty tree has a height of 0.
    if (root == NULL)
    {
        return 0;
    }

    // Recursively find the heigh tof left and right subtrees.
    size_t left = get_height(root->left_child);
    size_t right = get_height(root->right_child);

    // Height is the maximum of the two paths plus the current node (+1)..
    return (left > right ? left : right) + 1;
}

/**
 * @brief       :   Maps characters to their corresponding bit-codes in a lookup table.
 * @param       :   root:   Current node in the traversal.
 * @param       :   table:  Array of strings to store binary paths.
 * @param       :   buffer: Temporary string to track the current path.
 * @param       :   depth:  Current depth in the tree.
 * @return      :   void
 */
void map_codes(HuffmanNode* root, char* table[], char* buffer, size_t depth)
{
    if (root == NULL)
    {
        return;
    }

    // If we reach a leaf node, we've found a full character code.
    if (root->left_child == NULL && root->right_child == NULL)
    {
        // Null-terminate the string in the buffer at the current depth.
        buffer[depth] = '\0';

        // Map character ASCII value to a dupplicated copy of the buffer string.
        table[(unsigned char)root->data] = strdup(buffer);

        return;
    }

    // Tranverse left: Assign '0' to the current depth and recurse. 
    if (root->left_child != NULL)
    {
        buffer[depth] = '0';

        map_codes(root->left_child, table, buffer, depth + 1);
    }

    // Traverse right: Assign '1' to the current depth and recurse.
    if (root->right_child != NULL)
    {
        buffer[depth] = '1';
        
        map_codes(root->right_child, table, buffer, depth + 1);
    }
}

/**
 * @brief       :   Traverses the tree and prints character codes to stdout.
 * @param       :   root:  Current node in the traversal.
 * @param       :   path:  Buffer to store the sequence of bits.
 * @param       :   depth: Current recursion depth.
 * @return      :   void
 */
void generate_codes(HuffmanNode* root, int path[], size_t depth)
{
    if (root == NULL)
    {
        return;
    }

    // Leaf node check: Print the accumulated path
    if (root->left_child == NULL && root->right_child == NULL)
    {
        printf("%s %c%c", "Character:", root->data, '\n');

        // Loop through the integer path array up to current depth.
        for (size_t index = 0; index < depth; index++)
        {
            printf("%i", path[index]);
        }

        printf("%c", '\n');

        return;
    }

    // Record '0' for left traversal and recurse.
    if (root->left_child != NULL)
    {
        path[depth] = 0;

        generate_codes(root->left_child, path, depth + 1);
    }

    // Record '1' for right traversal and recurse
    if (root->right_child != NULL)
    {
        path[depth] = 1;

        generate_codes(root->right_child, path, depth + 1);
    }
}

/**
 * @brief       :   Recursively frees all memory associated with the Huffman tree.
 * @param       :   root: Pointer to the root of the tree to delete.
 * @return      :   void
 */
void delete_tree(HuffmanNode* root)
{
    if (root == NULL)
    {
        return;
    }

    // Post-order traversal: visit children before freeing the parent node.
    delete_tree(root->left_child);
    delete_tree(root->right_child);

    // After children are freed, it is safe to free the current node.
    free(root);
}