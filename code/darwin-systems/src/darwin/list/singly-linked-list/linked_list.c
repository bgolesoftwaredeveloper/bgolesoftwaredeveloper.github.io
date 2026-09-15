/**
 * @file        :   linked_list.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of a classic singly linked list.
 * @details     :   This module provides a linear data structure using dynamic 
 * memory allocation. Key features include:
 * - O(n) tail insertion (push) for maintaining order.
 * - Higher-order mapping for functional data transformation.
 * - Value-based node removal and updates.
 * - Manual memory management with deep-clean uninitialization
 * @version     :   1.0
 * @date        :   2026-05-13
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/list/linked_list.h"

/**
 * @brief       :   Allocates and initializes a new empty list container.
 * @details     :   Sets the head pointer to NULL and resets the size counter to zero.
 * @return      :   List*: A pointer to the newly allocated List structure, or NULL if allocation fails.
 */
List* initialize(void)
{
    // Allocate memory for the List management structure.
    List* list = (List*)malloc(sizeof(List));

    // Return NULL if the system failed to provide memory.
    if (list == NULL)
    {
        return NULL;
    }

    // Explicitly set initial state to prevent garbage values.
    list->head = NULL;
    list->size = 0;

    return list;
}

/**
 * @brief       :   Appends a new value to the end (tail) of the list.
 * @details     :   Traverses the entire list to locate the tail before insertion.
 * @param       :   list: The list instance to modify.
 * @param       :   value: The integer data to store in the new node.
 * @return      :   bool: true if the node was successfully added, false otherwise.
 */
bool append(List* list, int value)
{
    // Prevent dereferencing a NULL list pointer.
    if (list == NULL)
    {
        return false;
    }

    // Allocate memory for the new individual node.
    Node* new_node = (Node*)malloc(sizeof(Node));

    if (new_node == NULL)
    {
        return false;
    }

    // Initialize node data and set the next pointer to NULL (it will be the new tail).
    new_node->data = value;
    new_node->next = NULL;

    // Handle the case where the list is currently empty.
    if (list->head == NULL)
    {
        list->head = new_node;
    }
    else
    {
        // Initiate the traversal.
        Node* current = list->head;

        // Iterate until we find the node that current points to NULL.
        while (current->next != NULL)
        {
            current = current->next;
        }

        // Link the current tail to the new node.
        current->next = new_node;
    }

    list->size++;

    return true;
}

/**
 * @brief       :   Applies a provided function to every element in the list.
 * @details     :   Iterates from head to tail, passing a pointer to each 
 *                  node's data to the callback function.
 * @param       :   list: The list instance.
 * @param       :   function: A callback function taking an int* (allows modification).
 * @return      :   void
 */
void map(List* list, void (*function)(int*))
{
    // Guard against NULL pointers for both the container and the callback.
    if (list == NULL || function == NULL)
    {
        return;
    }

    Node* current = list->head;

    // Walk the entire chain until current becomes NULL.
    while (current != NULL)
    {
        // Pass the addresss of the data so the function can modify it.
        function(&current->data);
        
        current = current->next;
    }
}

/**
 * @brief       :   Finds the first instance of a value and replaces it.
 * @param       :   list: The list instance.
 * @param       :   value: The target value to search for.
 * @param       :   new_value: The value to overwrite the target with.
 * @return      :   bool: true if a replacement occurred, false if not found.
 */
bool update(List* list, int value, int new_value)
{
    // Check for valid pointers.
    if (list == NULL)
    {
        return false;
    }

    Node* current = list->head;

    while (current != NULL)
    {
        // Check if current node's data matches the search target.
        if (current->data == value)
        {
            // Perform the update and exit immediately.
            current->data = new_value;

            return true;
        }

        current = current->next;
    }

    // Return false if the loop completes without finding the value.
    return false;
}

/**
 * @brief       :   Removes the first node containing the specified value.
 * @details     :   Maintains list continuity by linking the previous node 
 *                  directly to the next node before freeing the target.
 * @param       :   list: The list instance.
 * @param       :   value: The value to be removed from the list.
 * @return      :   bool: true if the node was removed, false otherwise.
 */
bool erase(List* list, int value)
{
    // Cannot erase from a NULL list or an empty list.
    if (list == NULL || list->head == NULL)
    {
        return false;
    }

    Node* current = list->head;
    Node* previous = NULL;

    // Search for the node containing the value.
    while (current != NULL)
    {
        if (current->data == value)
        {
            // If previous is NULL, we are removing the head node.
            if (previous == NULL)
            {
                list->head = current->next;
            }
            else
            {
                // Link the previous node to the one following the current node.
                previous->next = current->next;
            }

            // Deallocate the memory for the target node.
            free(current);

            list->size--;

            return true;
        }

        // Keep a reference to the current node before moving to the next.
        previous = current;
        current = current->next;
    }

    return false;
}

/**
 * @brief       :   Frees all nodes and the top-level list container.
 * @details     :   Iteratively deallocates each node to prevent memory leaks.
 * @param       :   list: The list instance to destroy.
 * @return      :   void
 */
void uninitialize(List* list)
{
    // Return early if the pointer is already NULL.
    if (list == NULL)
    {
        return;
    }

    Node* current = list->head;

    // Free all individual nodes in the heap.
    while (current != NULL)
    {
        // Store the next pointer so we don't lose it after freeing current.
        Node* next = current->next;

        free(current);

        current = next;
    }

    // Free the management structure itself.
    free(list);
}