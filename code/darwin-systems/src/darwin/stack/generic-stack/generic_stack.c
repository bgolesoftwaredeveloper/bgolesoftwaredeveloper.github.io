/**
 * @file        :   generic_stack.h
 * @author      :   Braiden Gole
 * @brief       :   Header definition for a generic, dynamic stack.
 * @details     :   Provides a type-agnostic stack structure using void pointers.
 * Supports dynamic resizing and manual memory management.
 * @version     :   1.0
 * @date        :   2026-04-19
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include header files.
#include "darwin/stack/generic_stack.h"

/**
 * @brief       :   Initializes the stack with a specified item size.
 * @param       :   stack:      Pointer to the stack structure.
 * @param       :   item_size:  Size of the data type to be stored.
 * @return      :   bool:       True if allocation succeeded, false otherwise.
 */
bool initialize(GenericStack* stack, size_t item_size)
{
    if (stack == NULL || item_size == 0)
    {
        return false;
    }

    // Set initial metadata.
    stack->item_size = item_size;
    stack->size = 0;
    stack->capacity = INITIAL_CAPACITY;

    // Allocate the initial memory block on the heap.
    stack->data = (void*)malloc(INITIAL_CAPACITY * item_size);

    return (stack->data != NULL);
}

/**
 * @brief       :   Checks if the stack contains no elements.
 * @param       :   stack:  Pointer to the stack structure.
 * @return      :   bool:   True if empty, false otherwise.
 */
bool is_empty(const GenericStack* stack)
{
    return (stack == NULL || stack->size == 0);
}

/**
 * @brief       :   Returns the current number of elements in the stack.
 * @param       :   stack:   Pointer to the stack structure.
 * @return      :   size_t:  The number of items currently stored.
 */
size_t retrieve_stack_size(const GenericStack* stack)
{
    return (stack != NULL) ? stack->size : 0;
}

/**
 * @brief       :   Pushes a new element onto the top of the stack.
 * @details     :   Automatically doubles the internal buffer if capacity is reached.
 * @param       :   stack:    Pointer to the stack structure.
 * @param       :   element:  Pointer to the source data to be copied.
 * @return      :   bool:     True if successful, false if memory allocation failed.
 */
bool push(GenericStack* stack, const void* item)
{
    if (stack == NULL || item == NULL)
    {
        return false;
    }

    // Check if the internal buffer needs expansion.
    if (stack->size == stack->capacity)
    {
        size_t new_capacity = stack->capacity * 2;

        // Reallocate memory to accomodate more items.
        void* new_stack = (void*)realloc(stack->data, new_capacity * stack->item_size);

        if (new_stack == NULL)
        {
            return false;
        }

        stack->data = new_stack;
        stack->capacity = new_capacity;
    }

    // Calculation destination: Offset = (current size * size of each item).
    // Cast to char* is used for the byte-level pointer arithmetic.
    void* destination = (char*)stack->data + (stack->size * stack->item_size);

    // Copu the contents from the source pointer to the stack buffer.
    memcpy(destination, item, stack->item_size);

    // Increment the item count.
    stack->size++;

    return true;
}

/**
 * @brief       :   Retrieves the top element without removing it.
 * @param       :   stack:   Pointer to the stack structure.
 * @param       :   target:  Pointer to memory where the top item will be copied.
 * @return      :   bool:    True if successful, false if stack is empty.
 */
bool peek(const GenericStack* stack, void* target)
{
    // Ensure the stack is not empty and a target memory address is provided.
    if (stack == NULL || stack->size == 0 || target == NULL)
    {
        return false;
    }

    // Locate the last item added to the stack (size - 1).
    void* source = (char*)stack->data + ((stack->size - 1) * stack->item_size);

    // Copy the data to the user-provided target.
    memcpy(target, source, stack->item_size);

    return true;
}

/**
 * @brief       :   Manually triggers a capacity adjustment.
 * @details     :   Useful if the user knows they are about to push many items 
 *                  and wants to avoid multiple reallocations.
 * @param       :   stack:         Pointer to the stack structure.
 * @param       :   new_capacity:  The desired total capacity.
 * @return      :   bool:          True if successful.
 */
bool reserve(GenericStack* stack, size_t new_capacity)
{
    // Only expand: Do not use reserve to shrink.
    if (stack == NULL || new_capacity <= stack->capacity)
    {
        return false;
    }

    // Explicitly reallocate to the new requested size.
    void* new_block = (void*)realloc(stack->data, new_capacity * stack->item_size);

    if (new_block == NULL)
    {
        return false;
    }

    stack->data = new_block;
    stack->capacity = new_capacity;

    return true;
}

/**
 * @brief       :   Shrinks the buffer to fit the current number of elements.
 * @details     :   Reduces memory footprint by releasing unused capacity.
 * @param       :   stack:  Pointer to the stack structure.
 * @return      :   bool:   True if successful.
 */
bool shrink(GenericStack* stack)
{
    // Cannot shrink an empty or invalid stack.
    if (stack == NULL || stack->size == 0)
    {
        return false;
    }

    // Reallocate exactly to the size used.
    void* new_block = (void*)realloc(stack->data, stack->size * stack->item_size);

    if (new_block == NULL)
    {
        return false;
    }

    stack->data = new_block;
    stack->capacity = stack->size;

    return true;
}

/**
 * @brief       :   Removes the top element and copies it to a target pointer.
 * @param       :   stack:   Pointer to the stack structure.
 * @param       :   target:  Pointer to memory where the popped item will be stored.
 * @return      :   bool:    True if an item was popped, false if stack was empty.
 */
bool pop(GenericStack* stack, void* target)
{
    // Check for empty stack.
    if (stack == NULL || stack->size == 0)
    {
        return 0;
    }

    // Decrement size first to point to the top item.
    stack->size--;

    // If a target pointer is provided, copy the data before it is "forgotten."
    if (target != NULL)
    {
        void* source = (char*)stack->data + (stack->size * stack->item_size);

        memcpy(target, source, stack->item_size);
    }

    return true;
}

/**
 * @brief       :   Frees all resources associated with the stack.
 * @param       :   stack:  Pointer to the stack to be cleared.
 * @return      :   void
 */
void clear(GenericStack* stack)
{
    if (stack != NULL)
    {
        // Free the heap-allocated buffer.
        free(stack->data);

        // Reset all pointers and values to prevent accidental reuse.
        stack->data = NULL;
        stack->size = 0;
        stack->capacity = 0;
    }
}