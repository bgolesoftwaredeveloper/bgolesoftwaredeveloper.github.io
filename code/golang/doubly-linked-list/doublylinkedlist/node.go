// -----------------------------------------------------------------------------
// Package: doublylinkedlist
// File: node.go
//
// Description:
//
//	This file defines the internal node structure used by the doubly linked
//	list implementation. The node type represents a single element in the
//	list and maintains explicit links to both the previous and next nodes.
//
//	The node type is intentionally unexported to enforce encapsulation and
//	prevent external packages from depending on internal list mechanics.
//
// Design Notes:
//   - Each node stores a single value of type interface{}.
//   - The previous pointer references the preceding node or nil.
//   - The next pointer references the following node or nil.
//   - Node instances are allocated dynamically during list operations.
//   - Node fields are fully cleared when unlinked to allow garbage collection.
//
// Intended Use:
//
//	This type is strictly an internal implementation detail of the
//	doublylinkedlist package and must not be referenced outside the package.
//
// -----------------------------------------------------------------------------
package doublylinkedlist

// node represents a single element in a doubly linked list.
//
// Invariants:
//   - The value field stores the element data.
//   - The previous field is either nil or points to the preceding node.
//   - The next field is either nil or points to the subsequent node.
//
// Ownership and Mutability:
//   - Nodes are allocated and managed exclusively by DoublyLinkedList methods.
//   - Nodes must not be created or modified outside the doublylinkedlist package.
//
// Notes:
//   - This type is unexported to preserve list integrity.
type node struct {
	value    interface{}
	previous *node
	next     *node
}
