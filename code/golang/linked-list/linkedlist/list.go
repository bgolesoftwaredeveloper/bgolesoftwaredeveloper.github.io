// -----------------------------------------------------------------------------
// Package: linkedlist
// File: list.go
//
// Description:
//
//	This file contains an explicit implementation of a singly linked list.
//	The implementation prioritizes clarity, correctness, and safe pointer
//	manipulation over performance micro-optimizations.
//
//	The list supports insertion at both the head and tail, removal by value
//	and index, traversal, and conversion to a slice representation. All
//	operations preserve internal invariants and never panic due to invalid
//	input.
//
// Design Notes:
//   - The list stores integer values.
//   - The head pointer always references the first element or nil.
//   - The size field tracks the number of elements and is updated eagerly.
//   - Removal operations safely handle missing values and out-of-bounds indices.
//   - The zero value of LinkedList represents a valid empty list.
//
// Intended Use:
//
//	This package is intended for learning and demonstration of pointer-based
//	data structures in Go and as a reference implementation.
//
// -----------------------------------------------------------------------------
package linkedlist

import (
	"fmt"
	"strings"
)

// LinkedList represents a singly linked list of integers.
// The list maintains a pointer to the head node and an explicit size counter.
type LinkedList struct {
	head *node
	size int
}

// New creates and returns a new empty linked list.
// The returned list is initialized to a valid zero state.
func New() *LinkedList {
	return &LinkedList{}
}

// Size returns the number of elements currently stored in the list.
// This operation runs in constant time.
func (list *LinkedList) Size() int {
	if list == nil {
		return 0
	}

	return list.size
}

// Append inserts a value at the end of the list.
// If the list is empty, the new value becomes the head node.
// This operation runs in linear time.
func (list *LinkedList) Append(value int) {
	if list == nil {
		return
	}

	newNode := &node{value: value}

	// Handle the empty list case by assigning the head directly.
	if list.head == nil {
		list.head = newNode
		list.size++

		return
	}

	// Traverse to the last node.
	current := list.head

	for current.next != nil {
		current = current.next
	}

	// Link the new node at the tail.
	current.next = newNode
	list.size++
}

// Prepend inserts a value at the beginning of the list.
// The new value becomes the head node.
// This operation runs in constant time.
func (list *LinkedList) Prepend(value int) {
	if list == nil {
		return
	}

	// Create a new node and point it to the current head.
	list.head = &node{
		value: value,
		next:  list.head,
	}

	// Increment the size.
	list.size++
}

// ToSlice returns a slice containing all list elements in traversal order.
// The returned slice is a copy and modifications to it do not affect the list.
// This operation runs in linear time.
func (list *LinkedList) ToSlice() []int {
	if list == nil {
		return []int{}
	}

	// Preallocate a slice with capacity equal to the list size.
	result := make([]int, 0, list.size)

	// Traverse the list and append each value ot the slice.
	current := list.head

	for current != nil {
		result = append(result, current.value)
		current = current.next
	}

	// Return the populated slice.
	return result
}

// Remove removes the first occurrence of the specified value from the list.
// If the value is found, the element is removed and true is returned.
// If the value is not found, the list remains unchanged and false is returned.
// This operation runs in linear time.
func (list *LinkedList) Remove(value int) bool {
	// Handle removal from an empty list.
	if list == nil || list.head == nil {
		return false
	}

	// Handle removal of the head node.
	if list.head.value == value {
		list.head = list.head.next
		list.size--

		return true
	}

	// Traverse while the next node does not match the value.
	current := list.head

	for current.next != nil && current.next.value != value {
		current = current.next
	}

	// If the value was not found, do not modify the list.
	if current.next == nil {
		return false
	}

	// Unlink the matching node.
	current.next = current.next.next
	list.size--

	return true
}

// RemoveAt removes the element at the specified zero-based index.
// If the index is valid, the removed value and true are returned.
// If the index is out of bounds, zero and false are returned.
// This operation runs in linear time.
func (list *LinkedList) RemoveAt(index int) (int, bool) {
	if list == nil {
		return 0, false
	}

	// Reject invalid indices.
	if index < 0 || index >= list.size {
		return 0, false
	}

	// Handle removal of the head node.
	if index == 0 {
		removed := list.head

		list.head = removed.next
		list.size--

		return removed.value, true
	}

	// Traverse to the node preceding the removal target.
	current := list.head

	for probe := 0; probe < index-1; probe++ {
		current = current.next
	}

	// Unlink the target node.
	removed := current.next
	current.next = removed.next
	list.size--

	return removed.value, true
}

// String returns a human-readable representation of the list.
// The format displays elements in traversal order using arrow notation.
// This method allows the list to satisfy the fmt.Stringer interface.
func (list *LinkedList) String() string {
	if list == nil || list.head == nil {
		return "[]"
	}

	// Initialize a string builder for efficient concatenation.
	var builder strings.Builder

	// Write the opening bracket.
	builder.WriteString("[")

	// Traverse the list and append each value.
	current := list.head

	for current != nil {
		builder.WriteString(fmt.Sprintf("%d", current.value))

		// Append a separator when another element follows.
		if current.next != nil {
			builder.WriteString((" -> "))
		}

		current = current.next
	}

	// Write the closing bracket.
	builder.WriteString("]")

	// Return the formatted string representation.
	return builder.String()
}
