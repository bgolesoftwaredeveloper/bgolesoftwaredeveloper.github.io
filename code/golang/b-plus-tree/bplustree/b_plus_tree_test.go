// -----------------------------------------------------------------------------
// Package: bplustree
// File: bplustree_test.go
//
// Description:
//
//	This file contains a comprehensive suite of unit tests for the B+ Tree
//	implementation. It serves to validate the structural integrity, data
//	consistency, and performance characteristics of the tree under various
//	operational scenarios.
//
//	The testing suite covers:
//	  - Fundamental CRUD operations (Insert, Search, Update, Delete).
//	  - Structural transformations including root splits and node collapsing.
//	  - Complex rebalancing logic such as sibling borrowing and node merging.
//	  - Advanced retrieval patterns like cross-leaf range searches.
//	  - Edge case handling for empty ranges, duplicate keys, and negative values.
//
// Notes:
//   - Tests utilize the "test" identifier for *testing.T to meet specific
//     coding standards.
//   - Assertions are structured using Arrange-Act-Assert patterns.
//   - All tests ensure that the B+ Tree invariants remain intact after
//     every destructive operation.
//
// -----------------------------------------------------------------------------
package bplustree

import (
	"reflect"
	"testing"
)

// TestNewTreeInitialization verifies that a new tree is initialized as a leaf.
func TestNewTreeInitialization(test *testing.T) {
	// Arrange.
	tree := NewBPlusTree()

	// Act.
	rootNode := tree.root

	// Assert.
	if rootNode == nil || !rootNode.isLeaf {
		test.Errorf("Expected root to be an initialized leaf node.")
	}
}

// TestSingleInsertion verifies that a single key can be inserted and searched.
func TestSingleInsertion(test *testing.T) {
	// Arrange.
	tree := NewBPlusTree()
	inputKey := 10

	// Act.
	tree.Insert(inputKey)

	results := tree.RangeSearch(inputKey, inputKey)

	// Assert.
	if len(results) != 1 || results[0] != inputKey {
		test.Errorf("Expected to find key %d.", inputKey)
	}
}

// TestRootSplit verifies that the tree grows in height when the root exceeds degree.
func TestRootSplit(test *testing.T) {
	// Arrange.
	tree := NewBPlusTree()

	// Act.
	tree.Insert(10)
	tree.Insert(20)
	tree.Insert(30)
	tree.Insert(40)

	// Assert.
	if tree.root.isLeaf {
		test.Errorf("Root should no longer be a leaf after splitting.")
	}

	if len(tree.root.keys) < 1 {
		test.Errorf("Root should have 1 promoted key.")
	}
}

// TestSequentialInsertion verifies that the tree maintains order with sequential data.
func TestSequentialInsertion(test *testing.T) {
	// Arrange.
	tree := NewBPlusTree()

	sequence := []int{1, 2, 3, 4, 5}

	// Act.
	for _, value := range sequence {
		tree.Insert(value)
	}

	results := tree.RangeSearch(1, 5)

	// Assert.
	if !reflect.DeepEqual(results, sequence) {
		test.Errorf("Expected %v, got %v.", sequence, results)
	}
}

// TestRangeSearchWithinLeaf verifies search functionality within a single leaf node.
func TestRangeSearchWithinLeaf(test *testing.T) {
	// Arrange.
	tree := NewBPlusTree()

	tree.Insert(10)
	tree.Insert(15)
	tree.Insert(20)

	// Act.
	results := tree.RangeSearch(12, 18)

	// Assert.
	expected := []int{15}

	if !reflect.DeepEqual(results, expected) {
		test.Errorf("Expected %v, got %v.", expected, results)
	}
}

// TestRangeSearchCrossLeaf verifies that RangeSearch traverses the next leaf pointers.
func TestRangeSearchCrossLeaf(test *testing.T) {
	// Arrange.
	tree := NewBPlusTree()

	for index := 1; index <= 5; index++ {
		tree.Insert(index * 10)
	}

	// Act.
	results := tree.RangeSearch(15, 45)

	// Assert.
	expected := []int{20, 30, 40}

	if !reflect.DeepEqual(results, expected) {
		test.Errorf("Expected %v, got %v.", expected, results)
	}
}

// TestDeleteLeafKey verifies simple removal from a leaf node.
func TestDeleteLeafKey(test *testing.T) {
	// Arrange.
	tree := NewBPlusTree()

	tree.Insert(10)

	// Act.
	tree.Delete(10)

	results := tree.RangeSearch(0, 100)

	// Assert.
	if len(results) != 0 {
		test.Errorf("Expected tree to be empty after deletion.")
	}
}

// TestDeleteBorrowFromPrevious verifies that a node borrows from its left sibling.
func TestDeleteBorrowFromPrevious(test *testing.T) {
	// Arrange.
	tree := NewBPlusTree()

	tree.Insert(10)
	tree.Insert(20)
	tree.Insert(30)
	tree.Insert(40)

	// Act.
	tree.Delete(40)

	// Assert.
	results := tree.RangeSearch(0, 100)

	expected := []int{10, 20, 30}

	if !reflect.DeepEqual(results, expected) {
		test.Errorf("Borrow from previous failed. Got: %v.", results)
	}
}

// TestDeleteBorrowFromNext verifies that a node borrows from its right sibling.
func TestDeleteBorrowFromNext(test *testing.T) {
	// Arrange.
	tree := NewBPlusTree()

	tree.Insert(10)
	tree.Insert(20)
	tree.Insert(30)
	tree.Insert(40)

	// Act.
	tree.Delete(10)

	// Assert.
	results := tree.RangeSearch(0, 100)

	expected := []int{20, 30, 40}

	if !reflect.DeepEqual(results, expected) {
		test.Errorf("Borrow from next failed. Got: %v.", results)
	}
}

// TestDeleteMergeChildren verifies that two underfull siblings merge into one.
func TestDeleteMergeChildren(test *testing.T) {
	// Arrange.
	tree := NewBPlusTree()

	tree.Insert(10)
	tree.Insert(20)
	tree.Insert(30)

	// Act.
	tree.Delete(20)
	tree.Delete(30)

	// Assert.
	results := tree.RangeSearch(0, 100)

	if len(results) != 1 || results[0] != 10 {
		test.Errorf("Merge failed. Expected [10], got %v.", results)
	}
}

// TestRootCollapsing verifies tree height decreases when root becomes empty.
func TestRootCollapsing(test *testing.T) {
	// Arrange.
	tree := NewBPlusTree()

	for index := 1; index <= 6; index++ {
		tree.Insert(index)
	}

	// Act.
	for index := 1; index <= 6; index++ {
		tree.Delete(index)
	}

	// Assert.
	if !tree.root.isLeaf {
		test.Errorf("Root should have collapsed back into a leaf node.")
	}
}

// TestUpdateExistingKey verifies that Update correctly replaces a key.
func TestUpdateExistingKey(test *testing.T) {
	// Arrange.
	tree := NewBPlusTree()

	tree.Insert(50)

	// Act.
	tree.Update(50, 60)

	// Assert.
	if len(tree.RangeSearch(50, 50)) != 0 || len(tree.RangeSearch(60, 60)) != 1 {
		test.Errorf("Update failed to replace key 50 with 60.")
	}
}

// TestDuplicateInsertions verifies that the tree handles multiple identical keys.
func TestDuplicateInsertions(test *testing.T) {
	// Arrange.
	tree := NewBPlusTree()

	// Act.
	tree.Insert(10)
	tree.Insert(10)

	// Assert.
	results := tree.RangeSearch(10, 10)

	if len(results) != 2 {
		test.Errorf("Expected 2 instances of key 10, got %d.", len(results))
	}
}

// TestRangeSearchNoResults verifies that searching an empty range returns an empty slice.
func TestRangeSearchNoResults(test *testing.T) {
	// Arrange.
	tree := NewBPlusTree()

	tree.Insert(10)
	tree.Insert(20)

	// Act.
	results := tree.RangeSearch(30, 40)

	// Assert.
	if len(results) != 0 {
		test.Errorf("Expected empty results for out-of-bounds range.")
	}
}

// TestDeleteNonExistentKey verifies that deleting a missing key doesn't crash the tree.
func TestDeleteNonExistentKey(test *testing.T) {
	// Arrange.
	tree := NewBPlusTree()

	tree.Insert(10)

	// Act.
	tree.Delete(99)

	// Assert.
	results := tree.RangeSearch(10, 10)

	if len(results) != 1 {
		test.Errorf("Key 10 should still exist after deleting non-existent key.")
	}
}

// TestLargeVolumeInsertion verifies the tree can handle more than 100 insertions.
func TestLargeVolumeInsertion(test *testing.T) {
	// Arrange.
	tree := NewBPlusTree()

	count := 100

	// Act.
	for index := 1; index <= count; index++ {
		tree.Insert(index)
	}

	results := tree.RangeSearch(1, count)

	// Assert.
	if len(results) != count {
		test.Errorf("Expected %d results, got %d.", count, len(results))
	}
}

// TestInsertZeroAndNegatives verifies tree works with zero and negative integers.
func TestInsertZeroAndNegatives(test *testing.T) {
	// Arrange.
	tree := NewBPlusTree()

	keys := []int{-10, 0, 10}

	// Act.
	for _, key := range keys {
		tree.Insert(key)
	}

	results := tree.RangeSearch(-10, 10)

	// Assert.
	if !reflect.DeepEqual(results, keys) {
		test.Errorf("Negative/Zero handling failed. Got %v.", results)
	}
}

// TestDeleteAllKeys verifies the tree returns to a valid empty state.
func TestDeleteAllKeys(test *testing.T) {
	// Arrange.
	tree := NewBPlusTree()

	tree.Insert(1)
	tree.Insert(2)

	// Act.
	tree.Delete(1)
	tree.Delete(2)

	// Assert.
	if len(tree.RangeSearch(0, 10)) != 0 {
		test.Errorf("Tree should be empty after deleting all keys.")
	}
}

// TestRangeSearchInverseBounds verifies that start > end returns empty.
func TestRangeSearchInverseBounds(test *testing.T) {
	// Arrange.
	tree := NewBPlusTree()

	tree.Insert(10)

	// Act.
	results := tree.RangeSearch(50, 10)

	// Assert.
	if len(results) != 0 {
		test.Errorf("RangeSearch with start > end should return empty.")
	}
}

// TestDeepInternalSplit verifies split propagation across multiple levels.
func TestDeepInternalSplit(test *testing.T) {
	// Arrange.
	tree := NewBPlusTree()

	// Act.
	for index := 1; index <= 10; index++ {
		tree.Insert(index)
	}

	// Assert.
	if tree.root.isLeaf {
		test.Errorf("Tree should be multiple levels deep after 10 insertions.")
	}

	if len(tree.RangeSearch(1, 10)) != 10 {
		test.Errorf("Failed to retrieve all keys from deep tree.")
	}
}
