// -----------------------------------------------------------------------------
// Package: pairingheap
// File: pairing_heap_test.go
//
// Description:
//
//	This file contains unit tests for the PairingHeap data structure.
//	The tests validate correctness of heap operations including insertion,
//	minimum retrieval, extraction order, and decrease-key behavior.
//
//	The tests are written in a clear Arrange / Act / Assert style and favor
//	explicit variable naming and defensive checks over brevity.
//
// Intended Coverage:
//
//   - Heap initialization and empty-state behavior.
//   - Insert and FindMinimum correctness.
//   - ExtractMinimum ordering guarantees.
//   - DecreaseKey restructuring and invariant preservation.
//
// -----------------------------------------------------------------------------
package pairingheap

import "testing"

// TestNewPairingHeap_EmptyHeap verifies that a newly constructed heap
// starts in a valid and empty state.
func TestNewPairingHeap_EmptyHeap(test *testing.T) {
	// Arrange.
	comparator := func(left, right int) bool {
		return left < right
	}

	// Act.
	heap := NewPairingHeap(comparator)

	// Assert.
	if heap == nil {
		test.Fatal("expected heap instance to be non-nil")
	}

	if !heap.IsEmpty() {
		test.Fatal("expected newly created heap to be empty")
	}

	if heap.Size() != 0 {
		test.Fatalf("expected heap size to be 0, got %d", heap.Size())
	}
}

// TestInsertAndFindMinimum verifies that inserting values correctly
// updates the minimum element.
func TestInsertAndFindMinimum(test *testing.T) {
	// Arrange.
	comparator := func(left, right int) bool {
		return left < right
	}

	heap := NewPairingHeap(comparator)

	// Act.
	heap.Insert(42)
	heap.Insert(17)
	heap.Insert(88)
	heap.Insert(3)

	minimumValue, wasPresent := heap.FindMinimum()

	// Assert.
	if !wasPresent {
		test.Fatal("expected minimum value to be present")
	}

	if minimumValue != 3 {
		test.Fatalf("expected minimum value to be 3, got %d", minimumValue)
	}
}

// TestExtractMinimum_ReturnsSortedOrder verifies that repeatedly extracting
// the minimum returns values in strictly increasing order.
func TestExtractMinimum_ReturnsSortedOrder(test *testing.T) {
	// Arrange.
	comparator := func(left, right int) bool {
		return left < right
	}

	heap := NewPairingHeap(comparator)

	valuesToInsert := []int{42, 17, 88, 3, 29}
	expectedSortedOrder := []int{3, 17, 29, 42, 88}

	for _, value := range valuesToInsert {
		heap.Insert(value)
	}

	// Act.
	var extractedValues []int

	for !heap.IsEmpty() {
		minimumValue, wasPresent := heap.ExtractMinimum()

		if !wasPresent {
			test.Fatal("expected ExtractMinimum to return a value")
		}

		extractedValues = append(extractedValues, minimumValue)
	}

	// Assert.
	if len(extractedValues) != len(expectedSortedOrder) {
		test.Fatalf("expected %d extracted values, got %d", len(expectedSortedOrder), len(extractedValues))
	}

	for index := 0; index < len(expectedSortedOrder); index++ {
		if extractedValues[index] != expectedSortedOrder[index] {
			test.Fatalf("at index %d: expected %d, got %d", index, expectedSortedOrder[index], extractedValues[index])
		}
	}
}

// TestDecreaseKey_UpdatesMinimum verifies that decreasing a key
// correctly updates heap order and the reported minimum.
func TestDecreaseKey_UpdatesMinimum(test *testing.T) {
	// Arrange.
	comparator := func(left, right int) bool {
		return left < right
	}

	heap := NewPairingHeap(comparator)

	nodeHandle := heap.Insert(42)

	heap.Insert(17)
	heap.Insert(88)
	heap.Insert(29)

	// Act.
	heap.DecreaseKey(nodeHandle, 1)
	minimumValue, wasPresent := heap.FindMinimum()

	// Assert.
	if !wasPresent {
		test.Fatal("expected minimum value to be present after DecreaseKey")
	}

	if minimumValue != 1 {
		test.Fatalf("expected minimum value to be 1 after DecreaseKey, got %d", minimumValue)
	}
}

// TestExtractMinimum_OnEmptyHeap verifies that extracting from an empty heap
// returns the zero value and reports failure.
func TestExtractMinimum_OnEmptyHeap(test *testing.T) {
	// Arrange.
	comparator := func(left, right int) bool {
		return left < right
	}

	heap := NewPairingHeap(comparator)

	// Act.
	minimumValue, wasPresent := heap.ExtractMinimum()

	// Assert.
	if wasPresent {
		test.Fatal("expected ExtractMinimum to report no value on empty heap")
	}

	if minimumValue != 0 {
		test.Fatalf("expected zero value on empty heap, got %d", minimumValue)
	}
}

// TestInsert_MultipleSequentialInserts verifies that inserting values
// one-by-one preserves the correct minimum at each step.
func TestInsert_MultipleSequentialInserts(test *testing.T) {
	// Arrange.
	comparator := func(left, right int) bool {
		return left < right
	}

	heap := NewPairingHeap(comparator)

	insertedValues := []int{50, 40, 30, 20, 10}
	expectedMinimums := []int{50, 40, 30, 20, 10}

	// Act.
	for index, value := range insertedValues {
		heap.Insert(value)

		currentMinimum, wasPresent := heap.FindMinimum()

		if !wasPresent {
			test.Fatal("expected minimum value to be present after insertion")
		}

		// Assert.
		if currentMinimum != expectedMinimums[index] {
			test.Fatalf("after insert %d: expected minimum %d, got %d", value, expectedMinimums[index], currentMinimum)
		}
	}
}

// TestDecreaseKey_OnRootNode verifies that decreasing the key of the root
// does not corrupt the heap or change structure incorrectly.
func TestDecreaseKey_OnRootNode(test *testing.T) {
	// Arrange.
	comparator := func(left, right int) bool {
		return left < right
	}

	heap := NewPairingHeap(comparator)

	rootNodeHandle := heap.Insert(10)

	heap.Insert(20)
	heap.Insert(30)

	// Act.
	heap.DecreaseKey(rootNodeHandle, 1)
	minimumValue, wasPresent := heap.FindMinimum()

	// Assert.
	if !wasPresent {
		test.Fatal("expected minimum value to be present after DecreaseKey on root")
	}

	if minimumValue != 1 {
		test.Fatalf("expected minimum value to be 1, got %d", minimumValue)
	}
}

// TestExtractMinimum_SizeDecreases verifies that the heap size
// decreases correctly after each ExtractMinimum call.
func TestExtractMinimum_SizeDecreases(test *testing.T) {
	// Arrange.
	comparator := func(left, right int) bool {
		return left < right
	}

	heap := NewPairingHeap(comparator)

	valuesToInsert := []int{5, 4, 3, 2, 1}

	for _, value := range valuesToInsert {
		heap.Insert(value)
	}

	expectedSize := len(valuesToInsert)

	// Act & Assert.
	for !heap.IsEmpty() {
		if heap.Size() != expectedSize {
			test.Fatalf("expected heap size %d before extraction, got %d", expectedSize, heap.Size())
		}

		_, wasPresent := heap.ExtractMinimum()

		if !wasPresent {
			test.Fatal("expected ExtractMinimum to return a value")
		}

		expectedSize--
	}

	if heap.Size() != 0 {
		test.Fatalf("expected heap size to be 0 after all extractions, got %d", heap.Size())
	}
}

// TestDecreaseKey_MultipleOperations verifies that multiple decrease-key
// operations on different nodes preserve correct ordering.
func TestDecreaseKey_MultipleOperations(test *testing.T) {
	// Arrange.
	comparator := func(left, right int) bool {
		return left < right
	}

	heap := NewPairingHeap(comparator)

	nodeOneHundred := heap.Insert(100)
	nodeFifty := heap.Insert(50)

	heap.Insert(75)
	heap.Insert(200)

	// Act.
	heap.DecreaseKey(nodeOneHundred, 25)
	heap.DecreaseKey(nodeFifty, 10)

	minimumValue, wasPresent := heap.FindMinimum()

	// Assert.
	if !wasPresent {
		test.Fatal("expected minimum value to be present after multiple DecreaseKey calls")
	}

	if minimumValue != 10 {
		test.Fatalf("expected minimum value to be 10, got %d", minimumValue)
	}
}

// TestInsertAndExtract_SingleElement verifies correct behavior
// when inserting and extracting a single element.
func TestInsertAndExtract_SingleElement(test *testing.T) {
	// Arrange.
	comparator := func(left, right int) bool {
		return left < right
	}

	heap := NewPairingHeap(comparator)

	// Act.
	heap.Insert(99)
	minimumValue, wasPresent := heap.ExtractMinimum()

	// Assert.
	if !wasPresent {
		test.Fatal("expected ExtractMinimum to return a value for single-element heap")
	}

	if minimumValue != 99 {
		test.Fatalf("expected extracted value to be 99, got %d", minimumValue)
	}

	if !heap.IsEmpty() {
		test.Fatal("expected heap to be empty after extracting single element")
	}
}
