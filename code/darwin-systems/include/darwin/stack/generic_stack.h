/**
 * @file        :   generic_stack.h
 * @author      :   Braiden Gole
 * @brief       :   Header definition for a generic, dynamic stack.
 * @details     :   This header defines the data structures and function 
 * prototypes for a type-agnostic stack. It utilizes 
 * void pointers to allow for any data type storage 
 * while managing memory dynamically.
 * @version     :   1.0
 * @date        :   2026-04-19
 */
#ifndef GENERIC_STACK_H
#define GENERIC_STACK_H

#include <stdbool.h>
#include <stddef.h>

// Initial capacity for the internal stack buffering.
#define INITIAL_CAPACITY 8

/**
 * @struct          :   GenericStack
 * @brief           :   Structure to represent a generic stack.
 * @var data        :   Pointer to the raw memory buffer.
 * @var item_size   :   The size of each element in bytes.
 * @var size        :   The current number of items in the stack.
 * @var capacity    :   The total number of items the buffer can currently hold.
 */
typedef struct
{
    void* data;

    size_t item_size;
    size_t size;
    size_t capacity;
} GenericStack;

// Function prototypes.
bool initialize(GenericStack* stack, size_t item_size);
bool is_empty(const GenericStack* stack);
size_t retrieve_stack_size(const GenericStack* stack);
bool push(GenericStack* stack, const void* item);
bool peek(const GenericStack* stack, void* target);
bool reserve(GenericStack* stack, size_t new_capacity);
bool shrink(GenericStack* stack);
bool pop(GenericStack* stack, void* target);
void clear(GenericStack* stack);

#endif