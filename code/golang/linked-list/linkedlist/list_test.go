// -----------------------------------------------------------------------------
// Package: linkedlist
// File: list_test.go
//
// Description:
//
//	This file contains unit tests for the linkedlist package. The tests
//	verify correctness, safety, and defensive behavior for all exported
//	functions and methods of the LinkedList type.
//
//	The test suite exercises normal operation, edge cases, and failure
//	conditions to ensure that list invariants are preserved and that no
//	operation panics due to invalid input or state.
//
// Design Notes:
//   - Tests are written using explicit Arrange / Act / Assert sections.
//   - Defensive behaviors such as nil receiver handling are validated.
//   - Tests favor clarity and correctness over brevity.
//
// Intended Use:
//
//	This file is intended to validate the behavior of the linkedlist
//	implementation during development and refactoring.
//
// -----------------------------------------------------------------------------
package linkedlist

import "testing"

// -----------------------------------------------------------------------------
// Construction and Initial State
// -----------------------------------------------------------------------------

// TestNewCreatesEmptyList verifies that New returns a valid empty list.
func TestNewCreatesEmptyList(test *testing.T) {
	// Arrange.
	list := New()

	// Act.
	size := list.Size()
	listRepresentation := list.String()

	// Assert.
	if size != 0 {
		test.Fatalf("expected size 0, got %d", size)
	}

	if listRepresentation != "[]" {
		test.Fatalf("expected empty list representation, got %s", listRepresentation)
	}
}

// -----------------------------------------------------------------------------
// Size Queries and Nil Receiver Safety
// -----------------------------------------------------------------------------

// TestSizeOnNilReceiver verifies that Size returns zero for a nil receiver.
func TestSizeOnNilReceiver(test *testing.T) {
	// Arrange.
	var list *LinkedList

	// Act.
	size := list.Size()

	// Assert.
	if size != 0 {
		test.Fatalf("expected size 0 for nil receiver, got %d", size)
	}
}

// -----------------------------------------------------------------------------
// Append Operations
// -----------------------------------------------------------------------------

// TestAppendOnEmptyList verifies that Append correctly inserts into an empty list.
func TestAppendOnEmptyList(test *testing.T) {
	// Arrange.
	list := New()

	// Act.
	list.Append(10)

	// Assert.
	if list.Size() != 1 {
		test.Fatalf("expected size 1, got %d", list.Size())
	}

	if list.String() != "[10]" {
		test.Fatalf("unexpected list contents: %s", list.String())
	}
}

// TestAppendMultipleValues verifies that multiple values are appended in order.
func TestAppendMultipleValues(test *testing.T) {
	// Arrange.
	list := New()

	// Act.
	list.Append(1)
	list.Append(2)
	list.Append(3)

	// Assert.
	expected := "[1 -> 2 -> 3]"

	if list.String() != expected {
		test.Fatalf("expected %s, got %s", expected, list.String())
	}

	if list.Size() != 3 {
		test.Fatalf("expected size 3, got %d", list.Size())
	}
}

// TestAppendOnNilReceiver verifies that Append is safe on a nil receiver.
func TestAppendOnNilReceiver(test *testing.T) {
	// Arrange.
	var list *LinkedList

	// Act and Assert.
	defer func() {
		if recover() != nil {
			test.Fatalf("expected Append on nil receiver not to panic")
		}
	}()

	list.Append(5)
}

// -----------------------------------------------------------------------------
// Prepend Operations
// -----------------------------------------------------------------------------

// TestPrependOnEmptyList verifies that Prepend inserts at the head of an empty list.
func TestPrependOnEmptyList(test *testing.T) {
	// Arrange.
	list := New()

	// Act.
	list.Prepend(5)

	// Assert.
	if list.String() != "[5]" {
		test.Fatalf("unexpected list contents: %s", list.String())
	}

	if list.Size() != 1 {
		test.Fatalf("expected size 1, got %d", list.Size())
	}
}

// TestPrependMultipleValues verifies that multiple prepends reverse insertion order.
func TestPrependMultipleValues(test *testing.T) {
	// Arrange.
	list := New()

	// Act.
	list.Prepend(3)
	list.Prepend(2)
	list.Prepend(1)

	// Assert.
	expected := "[1 -> 2 -> 3]"

	if list.String() != expected {
		test.Fatalf("expected %s, got %s", expected, list.String())
	}
}

// -----------------------------------------------------------------------------
// Traversal and Slice Conversion
// -----------------------------------------------------------------------------

// TestToSliceOnEmptyList verifies that ToSlice returns an empty slice for an empty list.
func TestToSliceOnEmptyList(test *testing.T) {
	// Arrange.
	list := New()

	// Act.
	slice := list.ToSlice()

	// Assert.
	if len(slice) != 0 {
		test.Fatalf("expected empty slice, got length %d", len(slice))
	}
}

// TestToSliceReturnsCorrectOrder verifies traversal order in ToSlice.
func TestToSliceReturnsCorrectOrder(test *testing.T) {
	// Arrange.
	list := New()

	list.Append(1)
	list.Append(2)
	list.Append(3)

	// Act.
	slice := list.ToSlice()

	// Assert.
	expected := []int{1, 2, 3}

	for index := range expected {
		if slice[index] != expected[index] {
			test.Fatalf("at index %d, expected %d, got %d", index, expected[index], slice[index])
		}
	}
}

// TestToSliceOnNilReceiver verifies that ToSlice is safe on a nil receiver.
func TestToSliceOnNilReceiver(test *testing.T) {
	// Arrange.
	var list *LinkedList

	// Act.
	slice := list.ToSlice()

	// Assert.
	if len(slice) != 0 {
		test.Fatalf("expected empty slice for nil receiver, got length %d", len(slice))
	}
}

// -----------------------------------------------------------------------------
// Removal by Value
// -----------------------------------------------------------------------------

// TestRemoveFromEmptyList verifies that Remove fails on an empty list.
func TestRemoveFromEmptyList(test *testing.T) {
	// Arrange.
	list := New()

	// Act.
	removed := list.Remove(10)

	// Assert.
	if removed {
		test.Fatalf("expected removal to fail on empty list")
	}
}

// TestRemoveHead verifies that Remove correctly removes the head node.
func TestRemoveHead(test *testing.T) {
	// Arrange.
	list := New()

	list.Append(1)
	list.Append(2)

	// Act.
	removed := list.Remove(1)

	// Assert.
	if !removed {
		test.Fatalf("expected removal to succeed")
	}

	if list.String() != "[2]" {
		test.Fatalf("unexpected list contents: %s", list.String())
	}
}

// TestRemoveMiddle verifies that Remove correctly removes a middle node.
func TestRemoveMiddle(test *testing.T) {
	// Arrange.
	list := New()

	list.Append(1)
	list.Append(2)
	list.Append(3)

	// Act.
	removed := list.Remove(2)

	// Assert.
	if !removed {
		test.Fatalf("expected removal to succeed")
	}

	if list.String() != "[1 -> 3]" {
		test.Fatalf("unexpected list contents: %s", list.String())
	}
}

// TestRemoveNotFound verifies that Remove fails when the value is not present.
func TestRemoveNotFound(test *testing.T) {
	// Arrange.
	list := New()

	list.Append(1)
	list.Append(2)

	// Act.
	removed := list.Remove(99)

	// Assert.
	if removed {
		test.Fatalf("expected removal to fail for missing value")
	}

	if list.Size() != 2 {
		test.Fatalf("expected size to remain unchanged")
	}
}

// -----------------------------------------------------------------------------
// Removal by Index
// -----------------------------------------------------------------------------

// TestRemoveAtHead verifies that RemoveAt removes the head element.
func TestRemoveAtHead(test *testing.T) {
	// Arrange.
	list := New()

	list.Append(1)
	list.Append(2)

	// Act.
	value, ok := list.RemoveAt(0)

	// Assert.
	if !ok || value != 1 {
		test.Fatalf("unexpected removal result")
	}

	if list.String() != "[2]" {
		test.Fatalf("unexpected list contents: %s", list.String())
	}
}

// TestRemoveAtMiddle verifies that RemoveAt removes a middle element.
func TestRemoveAtMiddle(test *testing.T) {
	// Arrange.
	list := New()

	list.Append(1)
	list.Append(2)
	list.Append(3)

	// Act.
	value, ok := list.RemoveAt(1)

	// Assert.
	if !ok || value != 2 {
		test.Fatalf("unexpected removal result")
	}

	if list.String() != "[1 -> 3]" {
		test.Fatalf("unexpected list contents: %s", list.String())
	}
}

// TestRemoveAtOutOfBounds verifies that RemoveAt rejects invalid indices.
func TestRemoveAtOutOfBounds(test *testing.T) {
	// Arrange.
	list := New()

	list.Append(1)

	// Act.
	_, ok := list.RemoveAt(5)

	// Assert.
	if ok {
		test.Fatalf("expected removal to fail for out-of-bounds index")
	}
}

// -----------------------------------------------------------------------------
// String Representation
// -----------------------------------------------------------------------------

// TestStringOnEmptyList verifies the string representation of an empty list.
func TestStringOnEmptyList(test *testing.T) {
	// Arrange.
	list := New()

	// Act.
	listRepresentation := list.String()

	// Assert.
	if listRepresentation != "[]" {
		test.Fatalf("expected [], got %s", listRepresentation)
	}
}

// TestStringOnNilReceiver verifies that String is safe on a nil receiver.
func TestStringOnNilReceiver(test *testing.T) {
	// Arrange.
	var list *LinkedList

	// Act.
	listRepresentation := list.String()

	// Assert.
	if listRepresentation != "[]" {
		test.Fatalf("expected [] for nil receiver, got %s", listRepresentation)
	}
}
