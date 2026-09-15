/**
 * @file        :   linked_list.h
 * @author      :   Braiden Gole
 * @brief       :   Header file for a dynamic singly linked list implementation.
 * @details     :   Defines the structures and function prototypes for a 
 * linear linked list storing integer data. This module 
 * is designed for:
 * - Dynamic memory management.
 * - Tail-based insertion (O(n)).
 * - Sequential data processing via function pointers.
 * @version     :   1.0
 * @date        :   2026-05-13
 */
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @struct      :   Node
 * @brief       :   Represents an individual element in the linked list.
 * @var data    :   The integer value stored within the node.
 * @var next    :   Pointer to the subsequent node in the sequence (NULL if tail).
 */
typedef struct Node
{
    int data;
    struct Node* next;
} Node;

/**
 * @struct      :   List
 * @brief       :   The management structure for the singly linked list.
 * @var size    :   Current number of nodes present in the list (allows O(1) count).
 * @var head    :   Pointer to the first node in the sequence (NULL if empty).
 */
typedef struct
{
    size_t size;
    Node* head;
} List;

// Function prtotypes.
List* initialize(void);
bool append(List* list, int value);
void map(List* list, void (*function)(int*));
bool update(List* list, int value, int new_value);
bool erase(List* list, int value);
void uninitialize(List* list);

#endif