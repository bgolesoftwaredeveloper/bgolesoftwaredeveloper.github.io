// -----------------------------------------------------------------------------
// Package: priorityqueue
// File: comparator.go
//
// Description:
//
//	This file defines the Comparator type used to establish ordering
//	between items stored in a priority queue. A Comparator determines
//	which of two items has higher priority and therefore should appear
//	closer to the root of the heap.
//
//	The priority queue relies exclusively on the Comparator to maintain
//	heap order. No assumptions are made about the meaning of the Priority
//	field beyond what the Comparator enforces, allowing custom ordering
//	strategies to be defined by consumers.
//
//	Comparator functions must define a strict, deterministic ordering
//	over items. Comparators that violate transitivity or symmetry will
//	result in undefined behavior and may corrupt heap invariants.
//
// Notes:
//   - Comparator functions must not be nil.
//   - Comparator functions must be pure and free of side effects.
//   - Comparator behavior must remain stable for the lifetime of the queue.
//
// -----------------------------------------------------------------------------
package priorityqueue

// Comparator defines a comparison function used to order items within
// a priority queue.
//
// The function should return true if the first item ("compare") has
// higher priority than the second item ("against") according to the
// desired ordering.
//
// The Comparator must impose a strict weak ordering over items. Failure
// to do so may violate heap invariants and result in undefined behavior.
type Comparator[T any] func(compare, against *Item[T]) bool

// MinimumComparator returns a Comparator that orders items such that
// lower numeric Priority values represent higher priority.
// This comparator causes the priority queue to behave as a min-heap,
// where the item with the smallest Priority value is returned first.
func MinimumComparator[T any]() Comparator[T] {
	return func(compare, against *Item[T]) bool {
		return compare.Priority < against.Priority
	}
}

// MaximumComparator returns a Comparator that orders items such that
// higher numeric Priority values represent higher priority.
// This comparator causes the priority queue to behave as a max-heap,
// where the item with the largest Priority value is returned first.
func MaximumComparator[T any]() Comparator[T] {
	return func(compare, against *Item[T]) bool {
		return compare.Priority > against.Priority
	}
}
