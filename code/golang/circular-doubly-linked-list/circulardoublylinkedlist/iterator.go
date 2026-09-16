// -----------------------------------------------------------------------------
// Package: circulardoublylinkedlist
// File: foreach.go
//
// Description:
//
//	This file defines iteration utilities for the circular doubly linked list
//	implementation. The functionality provided here allows safe, bounded
//	traversal of list elements without exposing internal node structure or
//	requiring consumers to manage circular invariants manually.
//
//	The iteration logic is explicitly bounded by the list’s size field to
//	prevent infinite traversal, which is a common risk when working with
//	circular data structures.
//
// Notes:
//   - Iteration begins at the head node and proceeds forward.
//   - Traversal always executes at most one full cycle.
//   - Nil receivers and empty lists are handled defensively.
//   - Callbacks are invoked in logical list order.
//
// -----------------------------------------------------------------------------
package circulardoublylinkedlist

// ForEach invokes the provided callback function once for each element in the
// list, in forward order starting from the head.
// The traversal is bounded to a single full pass through the list. If the
// receiver is nil, the list is empty, or the callback is nil, this method
// performs no action.
func (list *List) ForEach(callback func(value interface{})) {
	// Guard against invalid receiver state or nil callback.
	if list == nil || list.head == nil || callback == nil {
		return
	}

	// Start traversal at the head node.
	current := list.head

	// Iterate exactly size elements to avoid infinite looping.
	for index := 0; index < list.size; index++ {
		callback(current.Value)

		current = current.Next
	}
}
