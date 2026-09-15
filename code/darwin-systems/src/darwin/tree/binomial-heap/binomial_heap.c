/**
 * @file        :   binomial_heap.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation file for a robust, pointer-networked Binomial Heap.
 * Provides memory allocation routines, pointer stitching for merging 
 * and linking, tree consolidation via Union operations, structural 
 * inspections, and complete recursive tree cleanup.
 * * @version   :   1.0
 * @date        :   2026-05-19
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/tree/binomial_heap.h"

/**
 * @brief Combines two binomial trees of the same order by linking them.
 *        Makes the child node the leftmost child of the parent node.
 * @param child Pointer to the root node of the tree becoming the child.
 * @param parent Pointer to the root node of the tree becoming the parent.
 */
static inline void binomial_link(BinomialNode* child, BinomialNode* parent)
{
    // Ensure both pointers exist before mutating memory.
    if (child == NULL || parent == NULL)
    {
        return;
    }

    // Stitch the child into the parent's leftmost hierachy branch.
    child->parent = parent;
    child->sibling = parent->child;

    // Reposition parent's pricipal entry pointer and update its structural order.
    parent->child = child;
    parent->degree++;
}

/**
 * @brief Interleaves the root lists of two binomial heaps in ascending order of degrees.
 * @param stream Pointer to the head of the first binomial heap root list.
 * @param heap Pointer to the head of the second binomial heap root list.
 * @return Pointer to the head of the combined, sorted root list.
 */
static inline BinomialNode* binomial_heap_merge(BinomialNode* stream, BinomialNode* heap)
{
    // If either collection is absent, instantly preserve and retunr the surviving stream.
    if (stream == NULL)
    {
        return heap;
    }

    if (heap == NULL)
    {
        return stream;
    }

    BinomialNode* head = NULL;
    BinomialNode** tail = &head;

    // Advance through both lists, weaving elements together by matching degrees.
    while (stream != NULL && heap != NULL)
    {
        if (stream->degree <= heap->degree)
        {
            *tail = stream;
            stream = stream->sibling;
        }
        else
        {
            *tail = heap;
            heap = heap->sibling;
        }

        tail = &((*tail)->sibling);
    }

    // Attach any remaining nodes left over from the unexhausted stream.
    *tail = stream ? stream : heap;

    return head;
}

/**
 * @brief Allocates and initializes a single binomial heap node.
 * @param key The integer payload value to store within the new node.
 * @return Pointer to the newly allocated BinomialNode, or NULL upon exhaustion.
 */
BinomialNode* create(int key)
{
    // Allocate a new node.
    BinomialNode* new_node = (BinomialNode*)malloc(sizeof(BinomialNode));

    if (new_node == NULL)
    {
        return NULL;
    }

    // Initialize its data members.
    new_node->key = key;
    new_node->degree = 0;

    new_node->child = NULL;
    new_node->sibling = NULL;
    new_node->parent = NULL;

    return new_node;
}

/**
 * @brief Merges two binomial heaps together and consolidates trees of matching degrees.
 * @param stream Pointer to the first binomial heap.
 * @param heap Pointer to the second binomial heap.
 * @return Pointer to the consolidated, valid binomial heap root list.
 */
BinomialNode* binomial_union(BinomialNode* stream, BinomialNode* heap)
{
    // Return the active collection if the counterpart is empty.
    if (stream == NULL)
    {
        return heap;
    }

    if (heap == NULL)
    {
        return stream;
    }

    // Interleave both lists linearly by degree before beginning consolidation.
    BinomialNode* head = binomial_heap_merge(stream, heap);

    if (head == NULL)
    {
        return NULL;
    }

    BinomialNode* previous = NULL;
    BinomialNode* current = head;
    BinomialNode* next = current->sibling;

    // Scan through the root lists and resolve back-to-baack.
    while (next != NULL)
    {
        // Case 1 & 2: Degrees differ, or three consecutive trees share identical degrees.
        if ((current->degree != next->degree) || 
            (next->sibling != NULL && next->sibling->degree == current->degree))
        {
            previous = current;
            current = next;
        }
        // Case 3: Current node has a smaller or equivalent key.
        // Absorb next into current.
        else if (current->key <= next->key)
        {
            current->sibling = next->sibling;

            binomial_link(next, current);
        }
        // Case 4: Next node has the smaller key.
        // Absorb current into next.
        else
        {
            if (previous == NULL)
            {
                head = next;
            }
            else
            {
                previous->sibling = next;
            }

            binomial_link(current, next);

            current = next;
        }

        next = current->sibling;
    }

    return head;
}

/**
 * @brief Scans the primary root list to identify and retrieve the absolute minimum key.
 * @param head Pointer to the head node of the binomial heap.
 * @param minimum Out-parameter pointer where the extracted minimum key will be written.
 * @return True if the value was recovered successfully, false if the heap is empty.
 */
bool retrieve_minimum(BinomialNode* head, int* minimum)
{
    if (head == NULL || minimum == NULL)
    {
        return false;
    }

    BinomialNode* node = head;
    BinomialNode* trace = head->sibling;

    // Linear root scan to locate teh absolute minimum key.
    while (trace != NULL)
    {
        if (trace->key < node->key)
        {
            node = trace;
        }
        
        trace = trace->sibling;
    }

    // Export payload to caller context.
    *minimum = node->key;

    return true;
}

/**
 * @brief Extracts the absolute minimum node, balances remaining sub-trees, and merges them.
 * @param head Double pointer to the heap root node, updated to reflect the new structure.
 * @param key Out-parameter pointer to store the integer value of the extracted minimum node.
 * @return True if extraction and recombination succeeded, false otherwise.
 */
bool extract_minimum(BinomialNode** head, int* key)
{
    // Ensure we have valid head pointer and a key address.
    if (head == NULL || *head == NULL || key == NULL)
    {
        return false;
    }

    BinomialNode* target = *head;
    BinomialNode* minimum_predecessor = NULL;
    BinomialNode* trace = *head;
    BinomialNode* running_predecessor = NULL;

    // Locate the target minimum root and track its direct tracking predessor.
    while (trace != NULL)
    {
        if (trace->key < target->key)
        {
            target = trace;
            minimum_predecessor = running_predecessor;
        }

        running_predecessor = trace;
        trace = trace->sibling;
    }

    // Set out-parameter data payload.
    *key = target->key;

    // Isolate and pop the target out of the main root list.
    if (minimum_predecessor == NULL)
    {
        *head = target->sibling;
    }
    else
    {
        minimum_predecessor->sibling = target->sibling;
    }

    BinomialNode* reverse_head = NULL;
    BinomialNode* trace_child = target->child;

    // Reverese the child list of the extracted node to build a valid standalone heap list.
    while (trace_child != NULL)
    {
        BinomialNode* next = trace_child->sibling;

        trace_child->parent = NULL;
        trace_child->sibling = reverse_head;
        reverse_head = trace_child;

        trace_child = next;
    }

    // Recombine the remaining primary heap with the newly reveresed sub-tree heap.
    *head = binomial_union(*head, reverse_head);

    // Free the isolated minimum node.
    free(target);

    return true;
}

/**
 * @brief Iterates laterally and drills down hierarchies to print the structure.
 * @param root  Const pointer to the structural starting point node.
 */
void print_tree(const BinomialNode* root)
{
    if (root == NULL)
    {
        return;
    }

    // Traverse root streams and systematically descend down child pathways.
    while (root != NULL)
    {
        printf("%i (degree: %zu)%c", root->key, root->degree, '\n');

        if (root->child != NULL)
        {
            printf("%s", "--> Children: [");
            print_tree(root->child);
            printf("%c", '\n');
        }

        root = root->sibling;
    }
}

/**
 * @brief Executes a deep post-order traversal to clean up all nodes safely.
 * @param root  Starting node root of the tree or sub-tree structure to be freed.
 */
void free_heap(BinomialNode* root)
{
    if (root == NULL)
    {
        return;
    }

    // Copy directional navigation vectors before reclaiming the current structural root.
    BinomialNode* child = root->child;
    BinomialNode* sibling = root->sibling;

    // Clear child hierachies.
    if (child != NULL)
    {
        free_heap(child);
    }

    // Clear lateral siblings.
    if (sibling != NULL)
    {
        free_heap(sibling);
    }

    // Securely reclaim memory.
    free(root);
}