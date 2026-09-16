// -----------------------------------------------------------------------------
// Package: circularsinglylinkedlist
// File: list.go
//
// Description:
//
//	This file defines the List structure for the generic circular singly
//	linked list implementation. The List type owns all structural state,
//	including the tail pointer and element count, and is responsible for
//	maintaining circular invariants.
//
//	The core invariant enforced by this implementation is that when the
//	list is non-empty, the tail node always points to the head node via
//	tail.next, enabling constant-time append and prepend operations.
//
//	Accessor and utility methods are provided to query list state and
//	perform safe inspection without exposing internal structure.
//
// Notes:
//   - The tail pointer is the sole structural anchor for the list.
//   - The size field bounds all traversal to prevent infinite loops.
//   - Direct node mutation outside this package is intentionally prevented.
//
// -----------------------------------------------------------------------------
package circularsinglylinkedlist

import "fmt"

// List represents a circular singly linked list.
// The tail node always references the head node when the list is non-empty.
type List[T any] struct {
	tail *node[T]
	size int
}

// New creates and returns an empty circular singly linked list.
// The returned list contains no nodes and has a length of zero.
func New[T any]() *List[T] {
	return &List[T]{}
}

// Length returns the number of elements currently stored in the list.
// A nil list is treated as having zero length.
func (list *List[T]) Length() int {
	if list == nil {
		return 0
	}

	return list.size
}

// IsEmpty reports whether the list contains no elements.
// A nil list is considered empty.
func (list *List[T]) IsEmpty() bool {
	// Return true if the list is nil or contains no nodes.
	return list == nil || list.size == 0
}

// Head returns the head node of the list.
// For a non-empty list, this is the node referenced by tail.next.
func (list *List[T]) Head() *node[T] {
	// Return nil if the list or tail pointer is nil.
	if list == nil || list.tail == nil {
		return nil
	}

	// Return the head node.
	return list.tail.next
}

// Tail returns the tail node of the list.
// For an empty list, this method returns nil.
func (list *List[T]) Tail() *node[T] {
	// Return nil if the list receiver is nil.
	if list == nil {
		return nil
	}

	// Return the tail node.
	return list.tail
}

// String returns a string representation of the list.
// The output reflects one full traversal starting from the head.
func (list *List[T]) String() string {
	// Return an empty representation for a nil or empty list.
	if list == nil || list.size == 0 {
		return "[]"
	}

	// Initialize traversal starting at the head node.
	head := list.Head()
	current := head

	// Build the output string incrementally.
	out := "["

	// Traverse the list exactly once.
	for index := 0; index < list.size; index++ {
		if index > 0 {
			out += " -> "
		}

		out += fmt.Sprintf("%v", current.Value)

		current = current.next
	}

	// Close the list representation.
	out += "]"

	return out
}
