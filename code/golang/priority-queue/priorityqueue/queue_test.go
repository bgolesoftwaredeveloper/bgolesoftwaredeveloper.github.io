// -----------------------------------------------------------------------------
// Package: priorityqueue
// File: queue_test.go
//
// Description:
//
//	This file defines unit tests for the PriorityQueue implementation.
//	The tests exercise all public APIs, including insertion, inspection,
//	priority mutation, arbitrary removal, and ordered extraction.
//
//	The test suite validates correctness under normal usage and ensures
//	that heap invariants are preserved across all supported operations.
//	Both structural behavior (ordering) and API behavior (errors and
//	state transitions) are verified.
//
//	The tests are written to be explicit rather than minimal, favoring
//	readability and correctness over conciseness. Each test follows a
//	clear Arrange / Act / Assert structure.
//
// Notes:
//   - Tests use a minimum comparator (min-heap semantics).
//   - Error cases are explicitly validated where applicable.
//   - Panic-inducing programmer errors are not recovered from unless
//     explicitly required by the test.
//
// -----------------------------------------------------------------------------
package priorityqueue

import "testing"

// -----------------------------------------------------------------------------
// Construction and Basic Properties.
// -----------------------------------------------------------------------------

// TestNewAndBasicProperties verifies that a newly constructed priority
// queue is initialized correctly and reports expected basic properties.
func TestNewAndBasicProperties(test *testing.T) {
	// Arrange.
	queue := New(MinimumComparator[int]())

	// Act.
	length := queue.Length()
	empty := queue.IsEmpty()

	// Assert.
	if length != 0 {
		test.Fatalf("expected length 0, got %d", length)
	}

	if !empty {
		test.Fatalf("expected queue to be empty")
	}
}

// -----------------------------------------------------------------------------
// Insertion and Inspection.
// -----------------------------------------------------------------------------

// TestPushAndPeek verifies that Push inserts items correctly and that
// Peek returns the highest-priority item without removal.
func TestPushAndPeek(test *testing.T) {
	// Arrange.
	queue := New(MinimumComparator[int]())

	queue.Push(100, 5)
	queue.Push(200, 1)
	queue.Push(300, 3)

	// Act.
	item, err := queue.Peek()

	// Assert.
	if err != nil {
		test.Fatalf("unexpected error from Peek: %v", err)
	}

	if item.Value != 200 || item.Priority != 1 {
		test.Fatalf(
			"expected peek value=200 priority=1, got value=%d priority=%d",
			item.Value,
			item.Priority,
		)
	}
}

// -----------------------------------------------------------------------------
// Ordered Removal.
// -----------------------------------------------------------------------------

// TestPopOrder verifies that Pop removes items in correct priority order.
func TestPopOrder(test *testing.T) {
	// Arrange.
	queue := New(MinimumComparator[int]())

	queue.Push(100, 5)
	queue.Push(200, 1)
	queue.Push(300, 3)
	queue.Push(400, 4)
	queue.Push(500, 2)

	expected := []struct {
		value    int
		priority int
	}{
		{200, 1},
		{500, 2},
		{300, 3},
		{400, 4},
		{100, 5},
	}

	// Act & Assert.
	for index, exp := range expected {
		item, err := queue.Pop()

		if err != nil {
			test.Fatalf("unexpected error on Pop at index %d: %v", index, err)
		}

		if item.Value != exp.value || item.Priority != exp.priority {
			test.Fatalf(
				"pop %d: expected value=%d priority=%d, got value=%d priority=%d",
				index,
				exp.value,
				exp.priority,
				item.Value,
				item.Priority,
			)
		}
	}

	if !queue.IsEmpty() {
		test.Fatalf("expected queue to be empty after popping all items")
	}
}

// -----------------------------------------------------------------------------
// Priority Mutation.
// -----------------------------------------------------------------------------

// TestDecreaseAndIncreasePriority verifies that priority mutation functions
// correctly reorder items within the heap.
func TestDecreaseAndIncreasePriority(test *testing.T) {
	// Arrange.
	queue := New(MinimumComparator[int]())

	queue.Push(100, 5)
	queue.Push(200, 1)
	queue.Push(300, 3)

	item, _ := queue.Peek()

	// Act: decrease priority (raise importance).
	err := queue.DecreasePriority(item, 0)

	// Assert.
	if err != nil {
		test.Fatalf("unexpected error from DecreasePriority: %v", err)
	}

	updated, _ := queue.Peek()

	if updated.Priority != 0 {
		test.Fatalf("expected priority 0 after decrease, got %d", updated.Priority)
	}

	// Act: increase priority (lower importance).
	err = queue.IncreasePriority(updated, 10)

	// Assert.
	if err != nil {
		test.Fatalf("unexpected error from IncreasePriority: %v", err)
	}

	updated, _ = queue.Peek()

	if updated.Value == 200 {
		test.Fatalf("expected item 200 to no longer be highest priority after increase")
	}
}

// TestUpdatePriority verifies that UpdatePriority correctly reorders the heap.
func TestUpdatePriority(test *testing.T) {
	// Arrange.
	queue := New(MinimumComparator[int]())

	queue.Push(100, 5)
	queue.Push(200, 1)
	queue.Push(300, 3)

	item, _ := queue.Peek()

	// Act.
	err := queue.UpdatePriority(item, 10)

	// Assert.
	if err != nil {
		test.Fatalf("unexpected error from UpdatePriority: %v", err)
	}

	newTop, _ := queue.Peek()

	if newTop.Value == item.Value {
		test.Fatalf("expected updated item to lose top priority after UpdatePriority")
	}
}

// -----------------------------------------------------------------------------
// Arbitrary Removal.
// -----------------------------------------------------------------------------

// TestRemove verifies that an arbitrary item can be removed from the queue.
func TestRemove(test *testing.T) {
	// Arrange.
	queue := New(MinimumComparator[int]())

	queue.Push(100, 5)
	queue.Push(200, 1)
	queue.Push(300, 3)

	item, _ := queue.Peek()

	// Act.
	err := queue.Remove(item)

	// Assert.
	if err != nil {
		test.Fatalf("unexpected error from Remove: %v", err)
	}

	if queue.Length() != 2 {
		test.Fatalf("expected queue length 2 after removal, got %d", queue.Length())
	}

	newTop, _ := queue.Peek()

	if newTop.Value == item.Value {
		test.Fatalf("removed item still present in queue")
	}
}

// -----------------------------------------------------------------------------
// Error Conditions.
// -----------------------------------------------------------------------------

// TestErrorConditions verifies expected error behavior for invalid operations.
func TestErrorConditions(test *testing.T) {
	// Arrange.
	queue := New(MinimumComparator[int]())

	// Act & Assert: Peek on empty queue.
	if _, err := queue.Peek(); err == nil {
		test.Fatalf("expected error from Peek on empty queue")
	}

	// Act & Assert: Pop on empty queue.
	if _, err := queue.Pop(); err == nil {
		test.Fatalf("expected error from Pop on empty queue")
	}

	// Act & Assert: UpdatePriority with nil item.
	if err := queue.UpdatePriority(nil, 1); err == nil {
		test.Fatalf("expected error from UpdatePriority with nil item")
	}

	// Act & Assert: Remove with nil item.
	if err := queue.Remove(nil); err == nil {
		test.Fatalf("expected error from Remove with nil item")
	}
}
