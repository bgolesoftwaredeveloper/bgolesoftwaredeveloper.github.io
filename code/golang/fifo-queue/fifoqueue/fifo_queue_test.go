// -----------------------------------------------------------------------------
// Package: fifoqueue
// File: fifo_queue_test.go
//
// Description:
//
//	This file contains unit tests for the generic FIFO (First-In, First-Out)
//	queue implementation. The tests validate both normal operation and
//	defensive behavior, including nil receiver handling, empty queue
//	operations, and internal state integrity.
//
//	Each test is structured to clearly separate setup, execution, and
//	verification phases, ensuring that failures are easy to diagnose and
//	that queue invariants are preserved across all operations.
//
// Notes:
//   - All public methods of Queue are exercised.
//   - Defensive error paths are explicitly validated.
//   - Tests avoid reliance on internal implementation details.
//   - The standard testing package is used exclusively.
//
// -----------------------------------------------------------------------------
package fifoqueue

import "testing"

// -----------------------------------------------------------------------------
// Construction and Initial State.
// -----------------------------------------------------------------------------

// TestNew_InitialState verifies that a newly created queue is empty
// and initialized in a valid state.
func TestNew_InitialState(test *testing.T) {
	// Arrange.
	queue := New[int]()

	// Act.
	length := queue.Length()
	isEmpty := queue.IsEmpty()

	// Assert.
	if length != 0 {
		test.Fatalf("expected length 0, got %d", length)
	}

	if !isEmpty {
		test.Fatalf("expected queue to be empty")
	}
}

// -----------------------------------------------------------------------------
// Length and IsEmpty.
// -----------------------------------------------------------------------------

// TestLength_NilReceiver verifies that calling Length on a nil queue
// returns zero and does not panic.
func TestLength_NilReceiver(test *testing.T) {
	// Arrange.
	var queue *Queue[int]

	// Act.
	length := queue.Length()

	// Assert.
	if length != 0 {
		test.Fatalf("expected length 0 for nil queue, got %d", length)
	}
}

// TestIsEmpty_NilReceiver verifies that a nil queue is treated as empty.
func TestIsEmpty_NilReceiver(test *testing.T) {
	// Arrange.
	var queue *Queue[int]

	// Act.
	isEmpty := queue.IsEmpty()

	// Assert.
	if !isEmpty {
		test.Fatalf("expected nil queue to be empty")
	}
}

// -----------------------------------------------------------------------------
// Enqueue.
// -----------------------------------------------------------------------------

// TestEnqueueAndLength verifies that Enqueue correctly adds elements
// and updates the queue length.
func TestEnqueueAndLength(test *testing.T) {
	// Arrange.
	queue := New[int]()

	// Act.
	if err := queue.Enqueue(1); err != nil {
		test.Fatalf("unexpected enqueue error: %v", err)
	}
	if err := queue.Enqueue(2); err != nil {
		test.Fatalf("unexpected enqueue error: %v", err)
	}

	// Assert.
	if queue.Length() != 2 {
		test.Fatalf("expected length 2, got %d", queue.Length())
	}
}

// TestEnqueue_NilReceiver verifies that Enqueue on a nil queue
// returns an error.
func TestEnqueue_NilReceiver(test *testing.T) {
	// Arrange.
	var queue *Queue[int]

	// Act.
	err := queue.Enqueue(42)

	// Assert.
	if err == nil {
		test.Fatalf("expected error when enqueueing on nil queue")
	}
}

// -----------------------------------------------------------------------------
// Peek.
// -----------------------------------------------------------------------------

// TestPeek_ReturnsFront verifies that Peek returns the front element
// without removing it.
func TestPeek_ReturnsFront(test *testing.T) {
	// Arrange.
	queue := New[int]()

	_ = queue.Enqueue(10)
	_ = queue.Enqueue(20)

	// Act.
	value, err := queue.Peek()

	// Assert.
	if err != nil {
		test.Fatalf("unexpected peek error: %v", err)
	}

	if value != 10 {
		test.Fatalf("expected peek value 10, got %d", value)
	}

	if queue.Length() != 2 {
		test.Fatalf("expected length to remain 2 after peek, got %d", queue.Length())
	}
}

// TestPeek_EmptyQueue verifies that Peek on an empty queue
// returns an error.
func TestPeek_EmptyQueue(test *testing.T) {
	// Arrange.
	queue := New[int]()

	// Act.
	_, err := queue.Peek()

	// Assert.
	if err == nil {
		test.Fatalf("expected error when peeking empty queue")
	}
}

// TestPeek_NilReceiver verifies that Peek on a nil queue
// returns an error.
func TestPeek_NilReceiver(test *testing.T) {
	// Arrange.
	var queue *Queue[int]

	// Act.
	_, err := queue.Peek()

	// Assert.
	if err == nil {
		test.Fatalf("expected error when peeking nil queue")
	}
}

// -----------------------------------------------------------------------------
// Dequeue.
// -----------------------------------------------------------------------------

// TestDequeue_FIFOOrder verifies that Dequeue removes elements
// in FIFO order.
func TestDequeue_FIFOOrder(test *testing.T) {
	// Arrange.
	queue := New[int]()

	_ = queue.Enqueue(1)
	_ = queue.Enqueue(2)
	_ = queue.Enqueue(3)

	// Act & Assert.
	for expected := 1; expected <= 3; expected++ {
		value, err := queue.Dequeue()

		if err != nil {
			test.Fatalf("unexpected dequeue error: %v", err)
		}

		if value != expected {
			test.Fatalf("expected %d, got %d", expected, value)
		}
	}

	if !queue.IsEmpty() {
		test.Fatalf("expected queue to be empty after dequeuing all elements")
	}
}

// TestDequeue_EmptyQueue verifies that Dequeue on an empty queue
// returns an error.
func TestDequeue_EmptyQueue(test *testing.T) {
	// Arrange.
	queue := New[int]()

	// Act.
	_, err := queue.Dequeue()

	// Assert.
	if err == nil {
		test.Fatalf("expected error when dequeuing empty queue")
	}
}

// TestDequeue_NilReceiver verifies that Dequeue on a nil queue
// returns an error.
func TestDequeue_NilReceiver(test *testing.T) {
	// Arrange.
	var queue *Queue[int]

	// Act.
	_, err := queue.Dequeue()

	// Assert.
	if err == nil {
		test.Fatalf("expected error when dequeuing nil queue")
	}
}

// -----------------------------------------------------------------------------
// Clear.
// -----------------------------------------------------------------------------

// TestClear_ResetsQueue verifies that Clear removes all elements
// and resets the queue state.
func TestClear_ResetsQueue(test *testing.T) {
	// Arrange.
	queue := New[int]()

	_ = queue.Enqueue(5)
	_ = queue.Enqueue(6)

	// Act.
	queue.Clear()

	// Assert.
	if queue.Length() != 0 {
		test.Fatalf("expected length 0 after clear, got %d", queue.Length())
	}

	if !queue.IsEmpty() {
		test.Fatalf("expected queue to be empty after clear")
	}
}

// TestClear_NilReceiver verifies that calling Clear on a nil queue
// does not panic.
func TestClear_NilReceiver(test *testing.T) {
	// Arrange.
	var queue *Queue[int]

	// Act.
	queue.Clear()

	// Assert.
	// No assertion required; test passes if no panic occurs.
}
