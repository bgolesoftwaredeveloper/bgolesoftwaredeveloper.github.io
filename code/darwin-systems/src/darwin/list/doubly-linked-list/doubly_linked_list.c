/**
 * @file        :   doubly_linked_list.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of a generic Doubly Linked List.
 * @details     :   Provides a robust API for managing a list of void pointers with 
 * automatic memory cleanup via user-defined callbacks.
 * @version     :   1.0
 * @date        :   2026-05-13
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include header files.
#include "darwin/list/doubly_linked_list.h"

// Helper functions.

/**
 * @brief Prints string data stored in a node.
 * @param data Pointer to the string to be printed.
 */
void print(void* data)
{
    // Ensure we dont attempt to print a null pointer.
    if (data != NULL)
    {
        printf("[%s]", (char*)data);
    }
}

/**
 * @brief Allocates heap memory and copies a string literal into it.
 * @param text The source string literal.
 * @return A pointer to the new heap-allocated string.
 */
char* make_text(char* text)
{
    if (text == NULL)
    {
        return NULL;
    }

    // Allocate memory length of string + 1 byte for the null terminator.
    char* data = (char*)malloc(sizeof(char) * strlen(text) + 1);

    if (data == NULL)
    {
        return NULL;
    }

    // Copy the contents of the literal into our new heap memory.
    strcpy(data, text);

    return data;
}

/**
 * @brief Compares two strings for equality.
 * @param data The data stored in the list.
 * @param key The search term provided by the user.
 * @return true if strings match, false otherwise.
 */
bool is_match(void* data, void* key)
{
    // Safety check to avoid strcmp craashing on NULL.
    if (data == NULL || key == NULL)
    {
        return false;
    }

    // Returns true if strcmp returns 0 (string are identical).
    return strcmp((char*)data, (char*)key) == 0;
}

/**
 * @brief Default cleanup callback for heap-allocated data.
 * @param data Pointer to memory that needs to be freed.
 */
void destroy(void* data)
{
    // Standard cleanup for single heap allocation.
    free(data);
}

/**
 * @brief Initializes a new empty list.
 * @param uninitialize Function pointer to clean up user data.
 * @return Pointer to the allocated List manager, or NULL on failure.
 */
List* initialize(void (*uninitialize)(void*))
{
    // Allocate the "headquarters" of the list.
    List* list = (List*)malloc(sizeof(List));

    if (list == NULL)
    {
        return NULL;
    }

    // Set initial state: empty list.
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;

    // Requirement: A generic list must know how to clean its own data.
    if (uninitialize == NULL)
    {
        // Clean up manager if initialization is incomplete.
        free(list);

        return NULL;
    }

    list->uninitialize = uninitialize;

    return list;
}

/**
 * @brief Inserts a new node at the beginning of the list.
 * @param list Pointer to the list manager.
 * @param data Pointer to the user data to store.
 * @return true on success, false on failure.
 */
bool push_front(List* list, void* data)
{
    // Ensure we have valid pointers.
    if (list == NULL || data == NULL)
    {
        return false;
    }

    // Allocate for a new node.
    Node* new_node = (Node*)malloc(sizeof(Node));

    if (new_node == NULL)
    {
        return false;
    }

    // Configure new node to point 'next' to teh current head.
    new_node->data = data;
    new_node->previous = NULL;
    new_node->next = list->head;

    if (list->head != NULL)
    {
        // Link the old head back to the new node.
        list->head->previous = new_node;
    }
    else
    {
        // If list was empty, this new node is also the tail.
        list->tail = new_node;
    }

    // Update the manager's head to our new node.
    list->head = new_node;
    list->size++;

    return true;
}

/**
 * @brief Inserts a new node at the end of the list.
 * @param list Pointer to the list manager.
 * @param data Pointer to the user data to store.
 * @return true on success, false on failure.
 */
bool push_back(List* list, void* data)
{
    // Ensure we have valid pointers.
    if (list == NULL || data == NULL)
    {
        return false;
    }

    // Allocate memory for a new node.
    Node* new_node = (Node*)malloc(sizeof(Node));

    if (new_node == NULL)
    {
        return false;
    }

    // Configure new node to point 'previous' to the current tail.
    new_node->data = data;
    new_node->next = NULL;
    new_node->previous = list->tail;

    if (list->tail != NULL)
    {
        // Link the old tail forward to the new node.
        list->tail->next = new_node;
    }
    else
    {
        // If list was empty, this new node is also the head.
        list->head = new_node;
    }

    // Update the manager's tail to our new node.
    list->tail = new_node;
    list->size++;

    return true;
}

/**
 * @brief Iterates through the list and executes a callback for each element.
 * @param list Pointer to the list manager.
 * @param callback Function to execute on each data pointer.
 */
void forEach(const List* list, void (*callback)(void*))
{
    // Ensure we have a list, nodes, and a function to run.
    if (list == NULL || list->head == NULL || callback == NULL)
    {
        return;
    }

    Node* current = list->head;

    // Standard linear traversal.
    while (current != NULL)
    {
        callback(current->data);

        current = current->next;
    }
}

/**
 * @brief Searches for an element using a custom comparison predicate.
 * @param list Pointer to the list manager.
 * @param predicate Comparison function returning bool.
 * @param key The value to search for.
 * @return Pointer to the data if found, NULL otherwise.
 */
void* find(const List* list, bool (*predicate)(void*, void*), void* key)
{
    // Ensure we have a list, head, and a callback to run.
    if (list == NULL || list->head == NULL || predicate == NULL)
    {
        return NULL;
    }

    Node* current = list->head;

    while (current != NULL)
    {
        // Use the predicate function to check if this is the data we want.
        if (predicate(current->data, key))
        {
            return current->data;
        }

        current = current->next;
    }

    // Exhausted list without a match.
    return NULL;
}

/**
 * @brief Removes the first element and returns its data.
 * @param list Pointer to the list manager.
 * @return Pointer to user data (caller must now manage this memory).
 */
void* pop_front(List* list)
{
    // Ensure we have valid pointers.
    if (list == NULL || list->head == NULL)
    {
        return NULL;
    }

    Node* remove_me = list->head;

    void* data = remove_me->data;

    // Shift head forward.
    list->head = remove_me->next;

    if (list->head != NULL)
    {
        // Sever the link to the old head.
        list->head->previous = NULL;
    }
    else
    {
        // List is now empty.
        list->tail = NULL;
    }

    // Free only the node container, not the data.
    free(remove_me);

    list->size--;

    return data;
}

/**
 * @brief Removes the last element and returns its data.
 * @param list Pointer to the list manager.
 * @return Pointer to user data (caller must now manage this memory).
 */
void* pop_back(List* list)
{
    // Ensure we have valid pointers.
    if (list == NULL || list->tail == NULL)
    {
        return NULL;
    }

    Node* remove_me = list->tail;

    void* data = remove_me->data;

    // Shift the tail backwards.
    list->tail = remove_me->previous;

    if (list->tail != NULL)
    {
        // Sever the link to the old tail.
        list->tail->next = NULL;
    }
    else
    {
        // List is now empty.
        list->head = NULL;
    }

    free(remove_me);

    list->size--;

    return data;
}

/**
 * @brief Clears all nodes/data and destroys the list manager.
 * @param list Pointer to the list manager to be destroyed.
 */
void clear(List* list)
{
    // Ensure we have a list to clear.
    if (list == NULL || list->size == 0)
    {
        return;
    }

    Node* current = list->head;

    // Tear-down loop.
    while (current != NULL)
    {
        Node* next = current->next;

        // Use the saved callback to free the data.
        if (list->uninitialize != NULL)
        {
            list->uninitialize(current->data);
        }

        // Free the library's internal Node strucutre.
        free(current);

        current = next;
    }

    // Clear fields.
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;

    // Final clean up: Free the list manager itself.
    free(list);
}