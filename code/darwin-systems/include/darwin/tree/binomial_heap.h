
#ifndef BINOMIAL_HEAP_H
#define BINOMIAL_HEAP_H

#include <stddef.h>
#include <stdbool.h>

typedef struct BinomialNode
{
    int key;
    size_t degree;
    struct BinomialNode* child;
    struct BinomialNode* sibling;
    struct BinomialNode* parent;
} BinomialNode;

// Function prototypes.
BinomialNode* create(int key);
BinomialNode* binomial_union(BinomialNode* stream, BinomialNode* heap);
bool retrieve_minimum(BinomialNode* head, int* minimum);
bool extract_minimum(BinomialNode** head, int* key);
void print_tree(const BinomialNode* root);
void free_heap(BinomialNode* root);

#endif