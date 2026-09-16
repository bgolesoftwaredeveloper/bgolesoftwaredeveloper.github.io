// -----------------------------------------------------------------------------
// Package: circulardoublylinkedlist
// File: list.go
//
// Description:
//
//	This file defines the List structure for a circular doubly linked list
//	implementation. The List type owns all structural state, including the
//	head pointer and element count, and is responsible for maintaining all
//	circular and bidirectional invariants.
//
//	The core invariant enforced by this implementation is that when the list
//	is non-empty, the head node’s Previous pointer always references the tail
//	node, and the tail node’s Next pointer always references the head node,
//	enabling constant-time insertion and removal at both ends.
//
//	All list operations are designed to tolerate nil receivers and empty
//	states without panicking, allowing safe use in defensive and low-level
//	code paths.
//
// Notes:
//   - The head pointer is the sole structural anchor for the list.
//   - The size field bounds all traversal to prevent infinite loops.
//   - Bidirectional links are always updated atomically per operation.
//   - Direct node mutation outside this package is intentionally discouraged.
//
// -----------------------------------------------------------------------------
package circulardoublylinkedlist

// List represents a circular doubly linked list.
// The list maintains a pointer to the head node and tracks the number of
// elements currently stored. When non-empty, the list guarantees full
// circular connectivity in both forward and backward directions.
type List struct {
	head *node
	size int
}

// New creates and returns an empty circular doubly linked list.
// The returned list contains no nodes and is immediately safe for use with
// all list operations.
func New() *List {
	return &List{}
}

// Size returns the number of elements currently stored in the list.
// A nil list receiver is treated as having zero elements.
func (list *List) Size() int {
	// Guard against a nil receiver.
	if list == nil {
		return 0
	}

	// Return the tracked element count.
	return list.size
}

// IsEmpty reports whether the list contains no elements.
// A nil list receiver is considered empty.
func (list *List) IsEmpty() bool {
	return list == nil || list.size == 0
}

// Head returns the head node of the list.
// For a non-empty list, this is the first logical element. If the list is
// empty or the receiver is nil, this method returns nil.
func (list *List) Head() *node {
	// Guard against a nil receiver.
	if list == nil {
		return nil
	}

	return list.head
}

// Tail returns the tail node of the list.
// The tail is defined as the node immediately preceding the head in the
// circular structure. If the list is empty or the receiver is nil, this
// method returns nil.
func (list *List) Tail() *node {
	// Guard against a nil receiver or empty list.
	if list == nil || list.head == nil {
		return nil
	}

	// The tail is always referenced by head.Previous.
	return list.head.Previous
}

// PushFront inserts a new value at the front of the list.
// The inserted node becomes the new head. If the list is empty, the new
// node is linked to itself to establish circular invariants. If the list
// receiver is nil, the operation is a no-op.
func (list *List) PushFront(value interface{}) {
	// Guard against a nil receiver.
	if list == nil {
		return
	}

	// Allocate a new node to store the value.
	newNode := &node{Value: value}

	// Handle the empty list case.
	if list.head == nil {
		// Link the node to itself to form a single-node circle.
		newNode.Previous = newNode
		newNode.Next = newNode

		// Assign the new node as the head.
		list.head = newNode
	} else {
		// Retrieve the current tail node.
		tail := list.head.Previous

		// Link the new node between the tail and current head.
		newNode.Previous = tail
		newNode.Next = list.head

		// Update existing nodes to reference the new head.
		tail.Next = newNode
		list.head.Previous = newNode

		// Update the head pointer.
		list.head = newNode
	}

	list.size++
}

// PushBack inserts a new value at the end of the list.
// The inserted node becomes the new tail while preserving circular and
// bidirectional invariants. If the list is empty, this operation behaves
// identically to PushFront. If the receiver is nil, the operation is a
// no-op.
func (list *List) PushBack(value interface{}) {
	// Guard against a nil receiver.
	if list == nil {
		return
	}

	// Delegate to PushFront if the list is empty.
	if list.head == nil {
		list.PushFront(value)

		return
	}

	// Allocate a new node for the value.
	newNode := &node{Value: value}

	// Retrieve the current tail node.
	tail := list.head.Previous

	// Link the new node between the tail and the head.
	newNode.Previous = tail
	newNode.Next = list.head

	// Update existing nodes to reference the new tail.
	tail.Next = newNode
	list.head.Previous = newNode

	// Increment the list size to reflect insertion.
	list.size++
}

// Find searches the list for the first node whose value satisfies the
// provided predicate function.
// Traversal begins at the head node and proceeds forward through the list
// for exactly one full cycle. If the list is empty, the receiver is nil,
// or the predicate is nil, this method returns nil.
func (list *List) Find(predicate func(interface{}) bool) *node {
	// Guard against invalid receiver state or predicate.
	if list == nil || list.head == nil || predicate == nil {
		return nil
	}

	// Start traversal at the head node.
	current := list.head

	// Traverse exactly size nodes to avoid infinite looping.
	for index := 0; index < list.size; index++ {
		// Apply the predicate to the current node's value.
		if predicate(current.Value) {
			return current
		}

		// Advance to the next node in the list.
		current = current.Next
	}

	// No matching node was found.
	return nil
}

// PopFront removes and returns the value at the front of the list.
// If the list is empty or the receiver is nil, this method returns
// (nil, false). Otherwise, it removes the head node, updates all
// structural links, and returns the stored value along with true.
func (list *List) PopFront() (interface{}, bool) {
	// Guard against an invalid receiver or empty list.
	if list == nil || list.head == nil {
		return nil, false
	}

	// Capture the value stored at the head node.
	value := list.head.Value

	// Handle the single-node list case.
	if list.size == 1 {
		// Clearing the head breaks the circular structure.
		list.head = nil
		list.size = 0

		return value, true
	}

	// Retrieve the tail node after advancing the head.
	tail := list.head.Previous

	// Advance the head pointer to the next node.
	list.head = list.head.Next

	// Restore circular links between the new head and tail.
	list.head.Previous = tail
	tail.Next = list.head

	// Decrement the list size to reflect removal.
	list.size--

	return value, true
}

// PopBack removes and returns the value at the end of the list.
// If the list is empty or the receiver is nil, this method returns
// (nil, false). Otherwise, it removes the tail node, preserves circular
// invariants, and returns the removed value along with true.
func (list *List) PopBack() (interface{}, bool) {
	// Guard against an invalid receiver or empty list.
	if list == nil || list.head == nil {
		return nil, false
	}

	// Retrieve the current tail node.
	tail := list.head.Previous

	// Capture the value stored at the tail.
	value := tail.Value

	// Handle the single-node list case.
	if list.size == 1 {
		// Clearing the head removes the only node.
		list.head = nil
		list.size = 0

		return value, true
	}

	// Identify the node that will become the new tail.
	newTail := tail.Previous

	// Update links to bypass the removed tail.
	newTail.Next = list.head
	list.head.Previous = newTail

	// Decrement the list size to reflect removal.
	list.size--

	return value, true
}

// Clear removes all elements from the list.
// After this operation, the list is empty and may be reused. If the
// receiver is nil, this method performs no action.
func (list *List) Clear() {
	// Guard against a nil receiver.
	if list == nil {
		return
	}

	// Reset all structural state.
	list.head = nil
	list.size = 0
}
