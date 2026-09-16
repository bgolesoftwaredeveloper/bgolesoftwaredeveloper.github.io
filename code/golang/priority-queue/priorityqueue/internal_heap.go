// -----------------------------------------------------------------------------
// Package: priorityqueue
// File: internal_heap.go
//
// Description:
//
//	This file defines the internalHeap type used by the priority queue
//	implementation. The internalHeap encapsulates the underlying binary
//	heap storage and comparator logic required to maintain priority
//	ordering.
//
//	The internalHeap type is intentionally unexported and exists solely
//	as an implementation detail of PriorityQueue. It is not safe for
//	direct use and does not enforce invariants on its own; all validation
//	and structural guarantees are provided by the owning PriorityQueue.
//
//	The heap stores pointers to Item values in a contiguous slice, where
//	parent-child relationships are determined by index arithmetic.
//	Ordering between items is defined exclusively by the supplied
//	Comparator function.
//
// Notes:
//   - internalHeap does not perform bounds checking or validation.
//   - All heap invariants are enforced by PriorityQueue methods.
//   - Direct mutation of items or comparator after construction
//     violates internal invariants and results in undefined behavior.
//
// -----------------------------------------------------------------------------
package priorityqueue

// internalHeap represents the backing storage and ordering logic for a
// priority queue.
//
// Internal invariants (enforced by PriorityQueue):
//   - items contains all active queue elements in heap order.
//   - Each Item.index matches its position within items.
//   - comparator defines a strict ordering over items and is non-nil.
//
// The internalHeap type is not safe for independent use. All interactions
// with heap state must be mediated by PriorityQueue to preserve correctness.
type internalHeap[T any] struct {
	items      []*Item[T]
	comparator Comparator[T]
}
