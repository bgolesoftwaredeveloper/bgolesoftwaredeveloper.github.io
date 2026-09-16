// -----------------------------------------------------------------------------
// Package: circularsinglylinkedlist
// File: node.go
//
// Description:
//
//	This file defines the Node structure used by the generic circular
//	singly linked list implementation. Each node stores a value and a
//	single forward reference to the next node in the list.
//
//	The circular invariant ensures that the final node in the list points
//	back to the head node, enabling constant-time wraparound traversal
//	and efficient append and prepend operations.
//
// Notes:
//   - The next field is unexported to prevent external mutation.
//   - Structural integrity is maintained exclusively by the List type.
//   - Nodes should not be manipulated directly by consumers.
//
// -----------------------------------------------------------------------------
package circularsinglylinkedlist

// node represents a single element in a circular singly linked list.
// Each node stores a value and a reference to the next node in the cycle.
type node[T any] struct {
	Value T
	next  *node[T]
}

// Next returns the next node in the cycle.
// Note: In a circular list, the "next" of the last node is the head.
func (node *node[T]) Next() *node[T] {
	// Return nil if the receiver node is nil.
	if node == nil {
		return nil
	}

	// Return the next node in the circular chain.
	return node.next
}
