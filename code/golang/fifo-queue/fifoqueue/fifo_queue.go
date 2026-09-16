// -----------------------------------------------------------------------------
// Package: fifoqueue
// File: queue.go
//
// Description:
//
//	This file defines a generic FIFO (First-In, First-Out) queue
//	implementation backed by a dynamically sized slice and a moving
//	head index. The Queue type owns all internal state and is responsible
//	for enforcing ordering, bounds safety, and memory compaction.
//
//	The core invariant enforced by this implementation is that the logical
//	contents of the queue are stored in data[head:], and the head index
//	always lies within the range [0, len(data)].
//
//	Defensive programming techniques are used throughout to ensure that
//	nil receivers, invalid operations, and internal state corruption are
//	handled safely and explicitly.
//
// Notes:
//   - Enqueue and Dequeue operations run in amortized O(1) time.
//   - Automatic slice compaction prevents unbounded memory retention.
//   - Nil receivers are tolerated for inspection operations.
//   - Misuse and invalid state are reported via explicit errors.
//
// -----------------------------------------------------------------------------
package fifoqueue

import "errors"

// Queue represents a FIFO (First-In, First-Out) queue.
//
// Internal invariants:
//   - head is always in the range [0, len(data)].
//   - Elements in data[head:] represent the logical contents of the queue.
//   - data[:head] contains stale values and must not be accessed.
//
// The zero value of Queue is not usable; use New to obtain a properly
// initialized instance.
type Queue[T any] struct {
	data []T
	head int
}

// New creates and returns a new, empty queue.
// The returned queue is guaranteed to be in a valid initial state.
func (queue *Queue[T]) Length() int {
	// Treat a nil receiver as an empty queue.
	if queue == nil {
		return 0
	}

	// Compute the logical length by subtracting the head index
	// from the total slice length.
	length := len(queue.data) - queue.head

	// Defensively clamp negative values, which indicate
	// internal state corruption.
	if length < 0 {
		return 0
	}

	return length
}

// Length returns the number of elements currently stored in the queue.
// A nil queue or an invalid internal state is treated as having zero length.
func (queue *Queue[T]) IsEmpty() bool {
	// Delegate to Length so that nil and corrupted states
	// are handled consistently in one place.
	return queue.Length() == 0
}

// IsEmpty reports whether the queue contains no elements.
// A nil queue is considered empty.
func New[T any]() *Queue[T] {
	// Allocate an empty slice and initialize the head index to zero.
	return &Queue[T]{
		data: make([]T, 0),
		head: 0,
	}
}

// Enqueue appends a value to the back of the queue.
// An error is returned if the queue receiver is nil.
func (queue *Queue[T]) Enqueue(value T) error {
	// Reject enqueue operations on a nil receiver to prevent
	// silent state loss or panics.
	if queue == nil {
		return errors.New("enqueue on nil queue")
	}

	// Append the value to the end of the underlying slice.
	queue.data = append(queue.data, value)

	return nil
}

// Peek returns the front element of the queue without removing it.
// An error is returned if the queue is nil, empty, or internally corrupted.
func (queue *Queue[T]) Peek() (T, error) {
	var zero T

	// Disallow peeking on a nil queue.
	if queue == nil {
		return zero, errors.New("peek on nil queue")
	}

	// Reject peeking from an empty queue.
	if queue.IsEmpty() {
		return zero, errors.New("queue is empty")
	}

	// Verify that the head index is within valid bounds
	// before performing slice access.
	if queue.head < 0 || queue.head >= len(queue.data) {
		return zero, errors.New("queue internal state corrupted")
	}

	// Return the front element without modifying state.
	return queue.data[queue.head], nil
}

// Dequeue removes and returns the front element of the queue.
// An error is returned if the queue is nil, empty, or internally corrupted.
func (queue *Queue[T]) Dequeue() (T, error) {
	var zero T

	// Disallow dequeue operations on a nil receiver.
	if queue == nil {
		return zero, errors.New("dequeue on nil queue")
	}

	// Reject dequeue operations on an empty queue.
	if queue.IsEmpty() {
		return zero, errors.New("queue is empty")
	}

	// Ensure the head index is valid before accessing the slice.
	if queue.head < 0 || queue.head >= len(queue.data) {
		return zero, errors.New("queue internal state corrupted")
	}

	// Read the front element.
	value := queue.data[queue.head]

	// Advance the head index to logically remove the element.
	queue.head++

	// Compact the underlying slice when a large unused prefix
	// has accumulated to prevent unbounded memory retention.
	if queue.head > 0 && queue.head*2 >= len(queue.data) {
		queue.data = append([]T(nil), queue.data[queue.head:]...)
		queue.head = 0
	}

	return value, nil
}

// Clear removes all elements from the queue and resets it to an empty state.
// Calling Clear on a nil queue has no effect.
func (queue *Queue[T]) Clear() {
	// Silently ignore nil receivers.
	if queue == nil {
		return
	}

	// Reset the slice length while retaining capacity.
	queue.data = queue.data[:0]

	// Reset the head index to its initial position.
	queue.head = 0
}
