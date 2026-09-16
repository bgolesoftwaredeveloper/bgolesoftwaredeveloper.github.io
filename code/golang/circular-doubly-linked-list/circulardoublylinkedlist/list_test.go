// -----------------------------------------------------------------------------
// Package: circulardoublylinkedlist
// File: list_test.go
//
// Description:
//
//	This file contains unit tests for the circular doubly linked list
//	implementation. The tests validate correctness, defensive behavior,
//	and preservation of circular and bidirectional invariants across all
//	public list operations.
//
//	Each test is structured using explicit Arrange / Act / Assert phases
//	and covers both normal and edge-case behavior, including nil receivers,
//	empty lists, single-element lists, and multi-element lists.
//
// Notes:
//   - All traversal is explicitly bounded by list size.
//   - Structural invariants are validated where applicable.
//   - No assumptions are made about internal memory layout.
//
// -----------------------------------------------------------------------------
package circulardoublylinkedlist

import "testing"

// -----------------------------------------------------------------------------
// Construction and Defensive State.
// -----------------------------------------------------------------------------

// TestNew_InitialState verifies that a newly created list is empty and valid.
func TestNew_InitialState(test *testing.T) {
	// Arrange.
	list := New()

	// Act & Assert.
	if list == nil {
		test.Fatalf("expected non-nil list")
	}

	if !list.IsEmpty() {
		test.Fatalf("expected new list to be empty")
	}

	if list.Size() != 0 {
		test.Fatalf("expected size 0, got %d", list.Size())
	}
}

// TestNilReceiver_DefensiveBehavior verifies nil receiver handling.
func TestNilReceiver_DefensiveBehavior(test *testing.T) {
	// Arrange.
	var list *List

	// Act & Assert.
	if list.Size() != 0 {
		test.Fatalf("expected size 0 for nil list")
	}

	if !list.IsEmpty() {
		test.Fatalf("expected nil list to be empty")
	}

	if list.Head() != nil {
		test.Fatalf("expected nil head for nil list")
	}

	if list.Tail() != nil {
		test.Fatalf("expected nil tail for nil list")
	}
}

// -----------------------------------------------------------------------------
// Insertion (Front and Back).
// -----------------------------------------------------------------------------

// TestPushFront_SingleAndMultiple verifies PushFront behavior.
func TestPushFront_SingleAndMultiple(test *testing.T) {
	// Arrange.
	list := New()

	// Act.
	list.PushFront(1)

	// Assert.
	if list.Size() != 1 {
		test.Fatalf("expected size 1, got %d", list.Size())
	}

	if list.Head().Value != 1 {
		test.Fatalf("unexpected head value")
	}

	// Act.
	list.PushFront(2)

	// Assert.
	if list.Size() != 2 {
		test.Fatalf("expected size 2")
	}

	if list.Head().Value != 2 {
		test.Fatalf("expected new head value 2")
	}

	// Validate circular invariants.
	if list.Head().Previous.Next != list.Head() {
		test.Fatalf("broken tail->next invariant")
	}
}

// TestPushBack_SingleAndMultiple verifies PushBack behavior.
func TestPushBack_SingleAndMultiple(test *testing.T) {
	// Arrange.
	list := New()

	// Act.
	list.PushBack(1)
	list.PushBack(2)

	// Assert.
	if list.Size() != 2 {
		test.Fatalf("expected size 2")
	}

	if list.Head().Value != 1 {
		test.Fatalf("unexpected head value")
	}

	if list.Tail().Value != 2 {
		test.Fatalf("unexpected tail value")
	}

	// Validate circular invariants.
	if list.Tail().Next != list.Head() {
		test.Fatalf("tail does not point to head")
	}
}

// -----------------------------------------------------------------------------
// Search.
// -----------------------------------------------------------------------------

// TestFind verifies predicate-based search.
func TestFind(test *testing.T) {
	// Arrange.
	list := New()

	list.PushBack(10)
	list.PushBack(20)
	list.PushBack(30)

	// Act.
	node := list.Find(func(value interface{}) bool {
		return value.(int) == 20
	})

	// Assert.
	if node == nil {
		test.Fatalf("expected node to be found")
	}

	if node.Value != 20 {
		test.Fatalf("unexpected value found")
	}
}

// TestFind_NotFound verifies Find returns nil when no match exists.
func TestFind_NotFound(test *testing.T) {
	// Arrange.
	list := New()

	list.PushBack(1)

	// Act.
	node := list.Find(func(value interface{}) bool {
		return value.(int) == 99
	})

	// Assert.
	if node != nil {
		test.Fatalf("expected nil when value not found")
	}
}

// -----------------------------------------------------------------------------
// Removal.
// -----------------------------------------------------------------------------

// TestPopFront verifies front removal behavior.
func TestPopFront(test *testing.T) {
	// Arrange.
	list := New()

	list.PushBack(1)
	list.PushBack(2)

	// Act.
	value, ok := list.PopFront()

	// Assert.
	if !ok || value != 1 {
		test.Fatalf("unexpected pop result")
	}

	if list.Size() != 1 {
		test.Fatalf("expected size 1")
	}
}

// TestPopBack verifies back removal behavior.
func TestPopBack(test *testing.T) {
	// Arrange.
	list := New()

	list.PushBack(1)
	list.PushBack(2)

	// Act.
	value, ok := list.PopBack()

	// Assert.
	if !ok || value != 2 {
		test.Fatalf("unexpected pop result")
	}

	if list.Size() != 1 {
		test.Fatalf("expected size 1")
	}
}

// TestPopSingle verifies behavior with a single element.
func TestPopSingle(test *testing.T) {
	// Arrange.
	list := New()

	list.PushBack(42)

	// Act.
	value, ok := list.PopFront()

	// Assert.
	if !ok || value != 42 {
		test.Fatalf("unexpected pop result")
	}

	if !list.IsEmpty() {
		test.Fatalf("expected list to be empty")
	}
}

// -----------------------------------------------------------------------------
// Reset and Cleanup.
// -----------------------------------------------------------------------------

// TestClear verifies that Clear resets all state.
func TestClear(test *testing.T) {
	// Arrange.
	list := New()

	list.PushBack(1)
	list.PushBack(2)

	// Act.
	list.Clear()

	// Assert.
	if !list.IsEmpty() {
		test.Fatalf("expected list to be empty after Clear")
	}

	if list.Head() != nil {
		test.Fatalf("expected nil head after Clear")
	}
}
