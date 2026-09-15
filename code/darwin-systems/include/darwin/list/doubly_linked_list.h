/**
 * @file        :   doubly_linked_list.h
 * @author      :   Braiden Gole
 * @brief       :   Header definition for a generic Doubly Linked List (DLL).
 * @details     :   This library provides a container for void pointers, allowing
 * it to store any data type. It features automated memory 
 * management via a user-defined destructor callback.
 * @version     :   1.0
 * @date        :   2026-05-13
 */
#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @struct Node
 * @brief  Individual container for data within the list.
 */
typedef struct Node
{
    void* data;
    struct Node* previous;
    struct Node* next;
} Node;

/**
 * @struct List
 * @brief  The management structure for the Doubly Linked List.
 */
typedef struct 
{
    Node* head;
    Node* tail;
    size_t size;

    void (*uninitialize)(void*);
} List;

// Helper prototypes.
void print(void* data);
char* make_text(char* text);
bool is_match(void* data, void* key);
void destroy(void* data);

// Function prototypes.
List* initialize(void (*uninitialize)(void*));
bool push_front(List* list, void* data);
bool push_back(List* list, void* data);
void forEach(const List* list, void (*callback)(void*));
void* find(const List* list, bool (*predicate)(void*, void*), void* key);
void* pop_front(List* list);
void* pop_back(List* list);
void clear(List* list);

#endif