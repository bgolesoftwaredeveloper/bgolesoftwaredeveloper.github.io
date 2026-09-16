// -----------------------------------------------------------------------------
// Package: priorityqueue
// File: queue.go
//
// Description:
//
//	This file defines a generic priority queue implementation backed by
//	a binary heap. Ordering is determined by a user-supplied Comparator,
//	allowing the queue to function as either a min-heap, max-heap, or any
//	custom priority ordering.
//
//	The PriorityQueue type owns all heap state and is responsible for
//	maintaining heap invariants, enforcing bounds safety, and defending
//	against misuse and internal corruption. All structural mutations
//	preserve the heap property defined by the comparator.
//
//	The core invariants enforced by this implementation are:
//
//	  - The heap slice stores all active items in heap order.
//	  - Each Item maintains an accurate index into the heap slice.
//	  - The comparator defines the total ordering of elements.
//	  - Nil receivers and invalid internal state are treated as
//	    programmer errors and fail fast via panic.
//	  - Invalid item usage is reported via explicit errors.
//
//	Defensive programming techniques are used throughout to ensure that
//	internal corruption, stale item references, and out-of-bounds access
//	are detected early and handled explicitly.
//
// Notes:
//   - Push, Pop, Remove, and priority updates run in O(log n) time.
//   - Peek and length queries run in O(1) time.
//   - Arbitrary item removal and priority mutation are supported.
//   - The zero value of PriorityQueue is not usable; use New to construct
//     a valid instance.
//
// -----------------------------------------------------------------------------
package priorityqueue

import "errors"

// PriorityQueue represents a generic priority queue backed by a binary heap.
//
// Internal invariants:
//   - heap is non-nil for all valid instances.
//   - heap.items contains all active items in heap order.
//   - Each Item.index accurately reflects its position in heap.items.
//   - The comparator defines the heap ordering and must not be nil.
//
// Nil receivers and invalid internal state are treated as programmer
// errors and will cause the program to panic.
type PriorityQueue[T any] struct {
	heap *internalHeap[T]
}

// New creates and returns a new, empty priority queue using the provided
// comparator to define element ordering.
// The comparator must not be nil. Passing a nil comparator is a
// programmer error and will cause the function to panic.
// The returned queue is guaranteed to be in a valid initial state.
func New[T any](comparator Comparator[T]) *PriorityQueue[T] {
	// Reject nil comparators, as heap ordering cannot be defined without one.
	if comparator == nil {
		panic("priority queue: comparator must not be nil")
	}

	// Allocate the internal heap with an empty item slice.
	return &PriorityQueue[T]{
		heap: &internalHeap[T]{
			items:      make([]*Item[T], 0),
			comparator: comparator,
		},
	}
}

// Length returns the number of items currently stored in the queue.
// The queue receiver and its internal heap must be non-nil. Calling
// Length on a nil receiver or a corrupted queue is a programmer error
// and will cause the function to panic.
func (queue *PriorityQueue[T]) Length() int {
	// Enforce non-nil receiver and heap.
	if queue == nil || queue.heap == nil {
		panic("priority queue: nil receiver or heap")
	}

	// The logical size of the queue is the heap slice length.
	return len(queue.heap.items)
}

// IsEmpty reports whether the priority queue contains no items.
// This method delegates to Length and therefore enforces the same
// invariants. A nil receiver or invalid internal state will cause
// a panic rather than being silently tolerated.
func (queue *PriorityQueue[T]) IsEmpty() bool {
	// Delegate to Length so invariant enforcement is centralized.
	return queue.Length() == 0
}

// swap exchanges two items in the heap slice and updates their stored
// indices to reflect the new positions.
// This is a low-level internal operation used exclusively by heap
// maintenance routines. The receiver must be valid, and both indices
// must be within bounds. Violations of these conditions indicate
// internal corruption and will cause a panic.
func (queue *PriorityQueue[T]) swap(source, destination int) {
	// Ensure the queue and heap are valid.
	if queue == nil || queue.heap == nil {
		panic("priority queue: nil receiver or heap")
	}

	// Validate indices before accessing the slice.
	if source < 0 || destination < 0 || source >= len(queue.heap.items) || destination >= len(queue.heap.items) {
		panic("priority queue: swap index out of bounds")
	}

	// Swap the item pointers.
	queue.heap.items[source], queue.heap.items[destination] = queue.heap.items[destination], queue.heap.items[source]

	// Update each item's stored index to reflect its new position.
	queue.heap.items[source].index = source
	queue.heap.items[destination].index = destination
}

// siftUp restores the heap property by moving the item at the given
// index upward until it is correctly ordered relative to its parent.
// This operation assumes the heap property may be violated only along
// the upward path from the given index. The receiver must be valid;
// nil receivers or corrupted state will cause a panic.
func (queue *PriorityQueue[T]) siftUp(index int) {
	// Ensure the queue and heap are valid.
	if queue == nil || queue.heap == nil {
		panic("priority queue: nil receiver or heap")
	}

	// Continue bubbling upward while not at the root.
	for index > 0 {
		// Compute the parent index.
		parent := (index - 1) / 2

		// Stop if the heap property is already satisfied.
		if !queue.heap.comparator(queue.heap.items[index], queue.heap.items[parent]) {
			break
		}

		// Swap the current item with its parent.
		queue.swap(index, parent)

		// Continue from the parent position.
		index = parent
	}
}

func (queue *PriorityQueue[T]) siftDown(index int) {
	// Ensure the queue and heap are valid.
	if queue == nil || queue.heap == nil {
		panic("priority queue: nil receiver or heap")
	}

	// Cache the heap size for bounds checking.
	size := queue.Length()

	for {
		// Compute child indices.
		left := 2*index + 1
		right := 2*index + 2
		best := index

		// Select the higher-priority child if it exists.
		if left < size && queue.heap.comparator(queue.heap.items[left], queue.heap.items[best]) {
			best = left
		}

		if right < size && queue.heap.comparator(queue.heap.items[right], queue.heap.items[best]) {
			best = right
		}

		// Stop if the heap property is satisfied.
		if best == index {
			break
		}

		// Swap with the best child and continue.
		queue.swap(index, best)

		index = best
	}
}

// Push inserts a new value into the priority queue with the given
// priority. The item is appended to the heap and then repositioned
// to restore heap order.
// The queue receiver must be non-nil and properly initialized.
// Violations of this requirement are treated as programmer errors
// and will cause a panic.
func (queue *PriorityQueue[T]) Push(value T, priority int) {
	// Enforce valid receiver and heap.
	if queue == nil || queue.heap == nil {
		panic("priority queue: nil receiver or heap")
	}

	// Create a new item positioned at the end of the heap.
	item := &Item[T]{
		Value:    value,
		Priority: priority,
		index:    queue.Length(),
	}

	// Append the item and restore heap order upward.
	queue.heap.items = append(queue.heap.items, item)
	queue.siftUp(item.index)
}

// Peek returns the highest-priority item without removing it from
// the queue.
// An error is returned if the queue is empty. Nil receivers or invalid
// internal state will cause a panic.
func (queue *PriorityQueue[T]) Peek() (*Item[T], error) {
	// Reject peeking from an empty queue.
	if queue.IsEmpty() {
		return nil, errors.New("priority queue is empty")
	}

	// The root of the heap always holds the highest-priority item.
	return queue.heap.items[0], nil
}

// UpdatePriority updates the priority of an existing item and restores
// the heap property as needed.
// The item must currently belong to the queue. Attempts to update a
// nil item, a removed item, or an item associated with a different
// queue will result in an error. Nil receivers or corrupted internal
// state are treated as programmer errors.
func (queue *PriorityQueue[T]) UpdatePriority(item *Item[T], newPriority int) error {
	// Reject invalid queue state.
	if queue == nil || queue.heap == nil {
		panic("priority queue: nil receiver or heap")
	}

	// Reject nil items.
	if item == nil {
		return errors.New("item is nil")
	}

	// Read the item's recorded index.
	index := item.index

	// Validate index bounds.
	if index < 0 || index >= queue.Length() {
		return errors.New("item is not in the queue")
	}

	// Ensure the item actually belongs to this heap position.
	if queue.heap.items[index] != item {
		return errors.New("item is not in the queue")
	}

	// Apply the new priority.
	item.Priority = newPriority

	// Restore heap invariants in both directions.
	queue.siftUp(index)
	queue.siftDown(index)

	return nil
}

// IncreasePriority raises the priority of an existing item.
// An error is returned if the item is nil, does not belong to the queue,
// or if the new priority violates the expected ordering constraints.
// Nil receivers are treated as programmer errors.
func (queue *PriorityQueue[T]) IncreasePriority(item *Item[T], newPriority int) error {
	// Reject invalid queue state.
	if queue == nil || queue.heap == nil {
		panic("priority queue: nil receiver or heap")
	}

	// Reject nil items.
	if item == nil {
		return errors.New("item is nil")
	}

	// Enforce monotonic priority increase.
	if newPriority < item.Priority {
		return errors.New("new priority is less than current priority")
	}

	// Delegate to the general update logic.
	return queue.UpdatePriority(item, newPriority)
}

func (queue *PriorityQueue[T]) DecreasePriority(item *Item[T], newPriority int) error {
	// Enforce valid receiver.
	if queue == nil || queue.heap == nil {
		panic("priority queue: nil receiver or heap")
	}

	// Reject nil items.
	if item == nil {
		return errors.New("item is nil")
	}

	// Enforce monotonic priority decrease.
	if newPriority > item.Priority {
		return errors.New("new priority is greater than current priority")
	}

	// Delegate to the general update logic.
	return queue.UpdatePriority(item, newPriority)
}

// DecreasePriority lowers the priority of an existing item.
// An error is returned if the item is nil, does not belong to the queue,
// or if the new priority violates the expected ordering constraints.
// Nil receivers or corrupted internal state will cause a panic.
func (queue *PriorityQueue[T]) Pop() (*Item[T], error) {
	// Reject popping from an empty queue.
	if queue.IsEmpty() {
		return nil, errors.New("priority queue is empty")
	}

	// Capture the root item.
	root := queue.heap.items[0]
	lastIndex := queue.Length() - 1

	// Move the last item to the root position.
	queue.swap(0, lastIndex)

	// Shrink the heap slice.
	queue.heap.items = queue.heap.items[:lastIndex]

	// Restore heap order downward if items remain.
	if !queue.IsEmpty() {
		queue.siftDown(0)
	}

	// Mark the removed item as no longer belonging to the heap.
	root.index = -1

	return root, nil
}

// Pop removes and returns the highest-priority item from the queue.
// The heap property is restored after removal. An error is returned
// if the queue is empty. Nil receivers or invalid internal state
// will cause a panic.
func (queue *PriorityQueue[T]) Remove(item *Item[T]) error {
	// Enforce valid receiver.
	if queue == nil || queue.heap == nil {
		panic("priority queue: nil receiver or heap")
	}

	// Reject nil items.
	if item == nil {
		return errors.New("item is nil")
	}

	// Read the item's index.
	index := item.index

	// Validate index bounds.
	if index < 0 || index >= queue.Length() {
		return errors.New("item is not in the queue")
	}

	// Ensure the item matches the heap entry.
	if queue.heap.items[index] != item {
		return errors.New("item is not in the queue")
	}

	// Swap the item with the last heap element.
	lastIndex := queue.Length() - 1

	queue.swap(index, lastIndex)

	// Remove the last element.
	queue.heap.items = queue.heap.items[:lastIndex]

	// Mark the item as removed.
	item.index = -1

	// Restore heap order if necessary.
	if index < queue.Length() {
		queue.siftUp(index)
		queue.siftDown(index)
	}

	return nil
}
