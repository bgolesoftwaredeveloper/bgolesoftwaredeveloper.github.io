// -----------------------------------------------------------------------------
// Package: circularsinglylinkedlist
// File: list_test.go
//
// Description:
//
//	This file contains unit tests for the core mutation, traversal, and
//	query operations of the circular singly linked list implementation.
//	The tests validate correctness, error handling, and enforcement of
//	circular structural invariants.
//
//	All tests are written to ensure that traversal is bounded, tail and
//	head references are maintained correctly, and edge cases are handled
//	defensively.
//
// Notes:
//   - Tests use Arrange / Act / Assert structure.
//   - All traversal assumes circular behavior.
//   - Both success and failure paths are validated.
//   - Internal helpers are tested from within the same package.
//
// -----------------------------------------------------------------------------
package circularsinglylinkedlist

import "testing"

// -----------------------------------------------------------------------------
// Construction and Basic State.
// -----------------------------------------------------------------------------

// TestAppend verifies appending elements to empty and non-empty lists.
func TestAppend(test *testing.T) {
	// Arrange.
	list := New[int]()

	// Act.
	list.Append(1)
	list.Append(2)
	list.Append(3)

	// Assert.
	if list.Length() != 3 {
		test.Fatalf("expected length 3, got %d.", list.Length())
	}

	if list.Tail().Value != 3 {
		test.Fatalf("expected tail value 3, got %d.", list.Tail().Value)
	}

	if list.Tail().Next().Value != 1 {
		test.Fatalf("expected tail.next to point to head.")
	}
}

// TestPrepend verifies prepending elements to the list.
func TestPrepend(test *testing.T) {
	// Arrange.
	list := New[int]()

	// Act.
	list.Prepend(2)
	list.Prepend(1)

	// Assert.
	if list.Head().Value != 1 {
		test.Fatalf("expected head value 1, got %d.", list.Head().Value)
	}

	if list.Length() != 2 {
		test.Fatalf("expected length 2, got %d.", list.Length())
	}
}

// -----------------------------------------------------------------------------
// Indexed Access and Internal Helpers.
// -----------------------------------------------------------------------------

// TestNodeAt verifies indexed node lookup.
func TestNodeAt(test *testing.T) {
	// Arrange.
	list := New[int]()
	list.Append(10)
	list.Append(20)
	list.Append(30)

	// Act.
	node, err := list.nodeAt(1)

	// Assert.
	if err != nil {
		test.Fatalf("unexpected error: %v.", err)
	}

	if node.Value != 20 {
		test.Fatalf("expected value 20, got %d.", node.Value)
	}
}

// TestNodeAtErrors verifies nodeAt error handling.
func TestNodeAtErrors(test *testing.T) {
	// Arrange.
	list := New[int]()

	// Act.
	_, errEmpty := list.nodeAt(0)

	// Assert.
	if errEmpty != ErrorEmptyList {
		test.Fatalf("expected ErrorEmptyList.")
	}

	// Arrange.
	list.Append(1)

	// Act.
	_, errRange := list.nodeAt(2)

	// Assert.
	if errRange != ErrorIndexOutOfRange {
		test.Fatalf("expected ErrorIndexOutOfRange.")
	}
}

// -----------------------------------------------------------------------------
// Search and Traversal.
// -----------------------------------------------------------------------------

// TestFind verifies predicate-based search.
func TestFind(test *testing.T) {
	// Arrange.
	list := New[int]()

	list.Append(5)
	list.Append(10)
	list.Append(15)

	// Act.
	node := list.Find(func(value int) bool { return value == 10 })

	// Assert.
	if node == nil {
		test.Fatalf("expected to find node.")
	}

	if node.Value != 10 {
		test.Fatalf("expected value 10, got %d.", node.Value)
	}
}

// TestFindNotFound verifies search miss behavior.
func TestFindNotFound(test *testing.T) {
	// Arrange.
	list := New[int]()

	list.Append(1)

	// Act.
	node := list.Find(func(value int) bool { return value == 99 })

	// Assert.
	if node != nil {
		test.Fatalf("expected nil result for missing value.")
	}
}

// TestForEach verifies bounded traversal.
func TestForEach(test *testing.T) {
	// Arrange.
	list := New[int]()

	list.Append(1)
	list.Append(2)
	list.Append(3)

	sum := 0

	// Act.
	list.ForEach(func(value int) { sum += value })

	// Assert.
	if sum != 6 {
		test.Fatalf("expected sum 6, got %d.", sum)
	}
}

// -----------------------------------------------------------------------------
// Insertion.
// -----------------------------------------------------------------------------

// TestInsertAt verifies insertion at arbitrary indices.
func TestInsertAt(test *testing.T) {
	// Arrange.
	list := New[int]()

	list.Append(1)
	list.Append(3)

	// Act.
	err := list.InsertAt(1, 2)

	// Assert.
	if err != nil {
		test.Fatalf("unexpected error: %v.", err)
	}

	if list.ToSlice()[1] != 2 {
		test.Fatalf("expected inserted value at index 1.")
	}
}

// TestInsertAtErrors verifies index validation.
func TestInsertAtErrors(test *testing.T) {
	// Arrange.
	list := New[int]()

	// Act.
	err := list.InsertAt(1, 10)

	// Assert.
	if err != ErrorIndexOutOfRange {
		test.Fatalf("expected ErrorIndexOutOfRange.")
	}
}

// -----------------------------------------------------------------------------
// Removal.
// -----------------------------------------------------------------------------

// TestRemoveAt verifies element removal.
func TestRemoveAt(test *testing.T) {
	// Arrange.
	list := New[int]()

	list.Append(1)
	list.Append(2)
	list.Append(3)

	// Act.
	value, err := list.RemoveAt(1)

	// Assert.
	if err != nil {
		test.Fatalf("unexpected error: %v.", err)
	}

	if value != 2 {
		test.Fatalf("expected removed value 2, got %d.", value)
	}

	if list.Length() != 2 {
		test.Fatalf("expected length 2.")
	}
}

// TestRemoveAtHead verifies head removal behavior.
func TestRemoveAtHead(test *testing.T) {
	// Arrange.
	list := New[int]()

	list.Append(1)
	list.Append(2)

	// Act.
	value, _ := list.RemoveAt(0)

	// Assert.
	if value != 1 {
		test.Fatalf("expected removed head value 1.")
	}

	if list.Head().Value != 2 {
		test.Fatalf("expected new head value 2.")
	}
}

// TestRemoveAtSingle verifies single-element removal.
func TestRemoveAtSingle(test *testing.T) {
	// Arrange.
	list := New[int]()

	list.Append(42)

	// Act.
	value, err := list.RemoveAt(0)

	// Assert.
	if err != nil {
		test.Fatalf("unexpected error.")
	}

	if value != 42 {
		test.Fatalf("expected removed value 42.")
	}

	if !list.IsEmpty() {
		test.Fatalf("expected list to be empty.")
	}
}

// -----------------------------------------------------------------------------
// Conversion and Representation.
// -----------------------------------------------------------------------------

// TestToSlice verifies slice conversion.
func TestToSlice(test *testing.T) {
	// Arrange.
	list := New[int]()

	list.Append(7)
	list.Append(8)

	// Act.
	slice := list.ToSlice()

	// Assert.
	if len(slice) != 2 {
		test.Fatalf("expected slice length 2.")
	}

	if slice[0] != 7 || slice[1] != 8 {
		test.Fatalf("unexpected slice contents.")
	}
}
