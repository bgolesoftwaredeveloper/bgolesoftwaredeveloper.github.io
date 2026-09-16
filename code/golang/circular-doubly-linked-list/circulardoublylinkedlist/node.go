// -----------------------------------------------------------------------------
// Package: circulardoublylinkedlist
// File: node.go
//
// Description:
//
//	This file defines the Node structure used by the circular doubly linked
//	list implementation. A Node represents a single element in the list and
//	contains a stored value along with bidirectional links to neighboring
//	nodes.
//
//	Each node participates in a circular structure such that, when the list
//	is non-empty:
//	  - The Previous pointer of the head node references the tail node.
//	  - The Next pointer of the tail node references the head node.
//
//	Nodes are not intended to be constructed or manipulated directly by
//	consumers of the package. All node creation and linkage is managed
//	internally by the List type to ensure structural invariants are
//	consistently preserved.
//
// Notes:
//   - Nodes form a fully circular, doubly linked topology when the list
//     contains at least one element.
//   - A single-node list forms a self-referential cycle.
//   - Direct mutation of Node fields outside this package may corrupt
//     list invariants and is strongly discouraged.
//
// -----------------------------------------------------------------------------
package circulardoublylinkedlist

// node represents a single element in a circular doubly linked list.
// Each node stores an arbitrary value and maintains pointers to both the
// previous and next nodes in the circular sequence.
type node struct {
	Value    interface{}
	Previous *node
	Next     *node
}
