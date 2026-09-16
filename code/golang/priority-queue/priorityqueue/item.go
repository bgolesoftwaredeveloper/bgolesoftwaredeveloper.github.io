// -----------------------------------------------------------------------------
// Package: priorityqueue
// File: item.go
//
// Description:
//
//	This file defines the Item type used by the priority queue heap
//	implementation. An Item represents a single value stored in the
//	queue together with its associated priority and its current
//	position within the heap.
//
//	The Item type is intentionally minimal and tightly coupled to the
//	internal heap representation. Each Item maintains a back-reference
//	to its position in the heap slice via the unexported index field,
//	allowing efficient priority updates and arbitrary removal in
//	O(log n) time.
//
//	The index field is managed exclusively by the priority queue and
//	must not be modified by consumers. External mutation of index or
//	assumptions about its value will result in undefined behavior and
//	is treated as a violation of the queue’s internal invariants.
//
// Notes:
//   - Value and Priority are exported to allow user inspection.
//   - index is unexported and owned entirely by the queue.
//   - A negative index indicates that the item is not currently
//     contained in any priority queue.
//
// -----------------------------------------------------------------------------
package priorityqueue

// Item represents a single element stored in a priority queue.
//
// Internal invariants:
//   - index is the current position of the item within the heap slice.
//   - index >= 0 implies the item is actively stored in the queue.
//   - index < 0 indicates the item has been removed or was never inserted.
//   - index is updated exclusively by heap maintenance operations.
//
// Consumers may read Value and Priority but must not mutate index.
// All structural modifications involving an Item must be performed
// through the PriorityQueue API.
type Item[T any] struct {
	Value    T
	Priority int
	index    int
}
