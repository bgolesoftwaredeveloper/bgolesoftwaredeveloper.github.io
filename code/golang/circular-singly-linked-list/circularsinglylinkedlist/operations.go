// -----------------------------------------------------------------------------
// Package: circularsinglylinkedlist
// File: ops.go
//
// Description:
//
//	This file contains core mutation, traversal, and query operations for
//	the generic circular singly linked list. The functions defined here
//	operate on the List type and are responsible for maintaining circular
//	structural invariants while providing safe, bounded access to list
//	contents.
//
//	The implementation enforces the invariant that when the list is
//	non-empty, the tail node always points to the head node, ensuring
//	constant-time append and prepend operations.
//
//	Error handling is explicit and defensive to prevent invalid access
//	and infinite traversal scenarios.
//
// Notes:
//   - All traversal is bounded by the list size to prevent infinite loops.
//   - Internal helper methods are unexported to preserve invariants.
//   - The list is generic and type-safe via Go type parameters.
//
// -----------------------------------------------------------------------------
package circularsinglylinkedlist

import "errors"

var (
	ErrorEmptyList       = errors.New("circular list is empty")
	ErrorIndexOutOfRange = errors.New("index out of range")
)

// Append inserts a new value at the end of the circular list.
// The inserted node becomes the new tail while preserving circular linkage.
func (list *List[T]) Append(value T) {
	// Return immediately if the list receiver is nil.
	if list == nil {
		return
	}

	// Allocate a new node to store the provided value.
	newNode := &node[T]{Value: value}

	// Handle insertion into an empty list.
	if list.size == 0 {
		newNode.next = newNode
		list.tail = newNode

		list.size = 1

		return
	}

	// Link the new node into the circular structure.
	newNode.next = list.tail.next

	list.tail.next = newNode
	list.tail = newNode
	list.size++
}

// Prepend inserts a new value at the front of the circular list.
// The head changes while the tail remains unchanged.
func (list *List[T]) Prepend(value T) {
	// Return immediately if the list receiver is nil.
	if list == nil {
		return
	}

	// Allocate a new node to store the provided value.
	newNode := &node[T]{Value: value}

	// Handle insertion into an empty list.
	if list.size == 0 {
		newNode.next = newNode
		list.tail = newNode

		list.size = 1

		return
	}

	// Insert the new node between the tail and the current head.
	newNode.next = list.tail.next

	list.tail.next = newNode
	list.size++
}

// nodeAt returns the node located at the specified index.
// This helper performs bounded traversal and is not exported.
func (list *List[T]) nodeAt(index int) (*node[T], error) {
	// Reject access if the list is nil or empty.
	if list == nil || list.size == 0 {
		return nil, ErrorEmptyList
	}

	// Reject indices outside valid bounds.
	if index < 0 || index >= list.size {
		return nil, ErrorIndexOutOfRange
	}

	// Begin traversal from the head of the list.
	current := list.Head()

	// Advance until the requested index is reached.
	for probe := 0; probe < index; probe++ {
		current = current.next
	}

	return current, nil
}

// Find searches the list for the first node whose value satisfies the predicate.
// The traversal completes at most one full cycle.
func (list *List[T]) Find(predicate func(T) bool) *node[T] {
	// Reject invalid state or predicate.
	if list == nil || list.size == 0 || predicate == nil {
		return nil
	}

	// Begin traversal from the head of the list.
	current := list.Head()

	// Iterate exactly once around the circular structure.
	for index := 0; index < list.size; index++ {
		if predicate(current.Value) {
			return current
		}

		current = current.next
	}

	return nil
}

// InsertAt inserts a value at the specified index.
// Index zero prepends, and index equal to the list size appends.
func (list *List[T]) InsertAt(index int, value T) error {
	// Reject insertion if the list receiver is nil.
	if list == nil {
		return ErrorEmptyList
	}

	// Reject indices outside valid insertion bounds.
	if index < 0 || index > list.size {
		return ErrorIndexOutOfRange
	}

	// Delegate to Prepend for index zero.
	if index == 0 {
		list.Prepend(value)

		return nil
	}

	// Delegate to Append for insertion at the end.
	if index == list.size {
		list.Append(value)

		return nil
	}

	// Locate the node preceding the insertion point.
	previous, err := list.nodeAt(index - 1)

	if err != nil {
		return err
	}

	// Insert the new node between the surrounding nodes.
	newNode := &node[T]{Value: value, next: previous.next}
	previous.next = newNode

	list.size++

	return nil
}

// RemoveAt removes and returns the value at the specified index.
// The circular structure and tail reference are updated as needed.
func (list *List[T]) RemoveAt(index int) (T, error) {
	var zero T

	// Reject removal from a nil or empty list.
	if list == nil || list.size == 0 {
		return zero, ErrorEmptyList
	}

	// Reject indices outside valid bounds.
	if index < 0 || list.size == 0 {
		return zero, ErrorIndexOutOfRange
	}

	// Handle removal of the head node.
	if index == 0 {
		head := list.tail.next

		// Handle single-element list removal.
		if list.size == 1 {
			list.tail = nil
			list.size = 0

			head.next = nil

			return head.Value, nil
		}

		// Bypass the head node.
		list.tail.next = head.next
		list.size--

		head.next = nil

		return head.Value, nil
	}

	// Locate the node preceding the removal target.
	previous, err := list.nodeAt(index - 1)

	if err != nil {
		return zero, err
	}

	// Remove the target node from the circular chain.
	target := previous.next
	previous.next = target.next

	// Update the tail reference if the tail was removed.
	if target == list.tail {
		list.tail = previous
	}

	list.size--

	target.next = nil

	return target.Value, nil
}

// ForEach applies the provided function to each element in the list.
// Traversal is bounded to exactly one full cycle.
func (list *List[T]) ForEach(visit func(T)) {
	// Reject invalid state or visitor function.
	if list == nil || list.size == 0 || visit == nil {
		return
	}

	// Begin traversal from the head of the list.
	current := list.Head()

	// Visit each element exactly once.
	for index := 0; index < list.size; index++ {
		visit(current.Value)

		current = current.next
	}
}

// ToSlice returns a linear slice containing the list values.
// The slice represents one complete traversal starting from the head.
func (list *List[T]) ToSlice() []T {
	// Return nil for a nil or empty list.
	if list == nil || list.size == 0 {
		return nil
	}

	// Allocate a slice with exact capacity.
	values := make([]T, 0, list.size)

	// Begin traversal from the head of the list.
	current := list.Head()

	// Append each value exactly once.
	for index := 0; index < list.size; index++ {
		values = append(values, current.Value)

		current = current.next
	}

	return values
}
