/**
 * @file        :   huffman.h
 * @author      :   Braiden Gole
 * @brief       :   Header file for Huffman Coding tree and priority queue structures.
 * @details     :   Defines the core data structures and function prototypes required 
 * to build an optimal prefix-free binary tree for data compression.
 * @version     :   1.0
 * @date        :   2026-05-07
 */
#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stddef.h>

#define MAXIMUM_HEIGHT 64

/**
 * @struct      :   HuffmanNode
 * @brief       :   A single node within the Huffman tree structure.
 * @details     :   Acts as a leaf (storing character data) or an internal node 
 *                  (storing the sum of frequencies from its children).
 */
typedef struct HuffmanNode
{
    char data;
    size_t frequency;
    struct HuffmanNode* left_child;
    struct HuffmanNode* right_child;
} HuffmanNode;

/**
 * @struct      :   PriorityQueue
 * @brief       :   A Minimum-Heap used to build the Huffman tree.
 * @details     :   Organizes HuffmanNodes based on frequency, allowing O(log n) 
 *                  extraction of the two least-frequent nodes.
 */
typedef struct
{
    size_t size;
    size_t maximum_capacity;
    HuffmanNode** list;
} PriorityQueue;

/**
 * @struct      :   CodeTable
 * @brief       :   A lookup structure for mapped binary codes.
 * @details     :   Stores bit-string representations for all 256 ASCII characters, 
 *                  allowing O(1) retrieval during the encoding process.
 */
typedef struct
{
    char* codes[256];
} CodeTable;

// Function prototypes.
HuffmanNode* build_tree(const char* alphabet[], const size_t frequencies[], size_t size);
size_t get_height(HuffmanNode* root);
void map_codes(HuffmanNode* root, char* table[], char* buffer, size_t depth);
void generate_codes(HuffmanNode* root, int path[], size_t depth);
void delete_tree(HuffmanNode* root);

#endif