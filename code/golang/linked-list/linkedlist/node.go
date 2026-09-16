// -----------------------------------------------------------------------------
// Package: linkedlist
// File: node.go
//
// Description:
//
//	This file defines the internal node structure used by the linked list
//	implementation. Nodes represent individual elements in the list and
//	maintain a reference to the next node in traversal order.
//
//	The node type is intentionally unexported to enforce encapsulation and
//	prevent external packages from mutating list internals directly.
//
// Design Notes:
//   - Each node stores a single integer value.
//   - The next pointer is either nil or references the subsequent node.
//   - Nodes are owned exclusively by the LinkedList structure.
//
// Intended Use:
//
//	This file is intended for internal use by the linkedlist package only
//	and is not part of the public API.
//
// -----------------------------------------------------------------------------
package linkedlist

// node represents a single element in a singly linked list.
//
// Invariants:
//   - The value field stores the element data.
//   - The next field is either nil or points to the next node in the list.
//
// Ownership and Mutability:
//   - Nodes are allocated and managed exclusively by LinkedList methods.
//   - Nodes must not be created or modified outside the linkedlist package.
//
// Notes:
//   - This type is unexported to preserve list integrity.
type node struct {
	value int
	next  *node
}
