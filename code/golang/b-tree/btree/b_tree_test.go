// -----------------------------------------------------------------------------
// Package: btree
// File: btree_test.go
//
// Description:
//
//	This file contains unit tests for the B-Tree implementation.
//	The tests verify correctness of insertion, search, deletion,
//	node splitting, borrowing, merging, and root height reduction.
//
//	All tests are written to exercise both normal and edge-case
//	execution paths while preserving B-Tree invariants.
//
// Testing Notes:
//   - Minimum degree t = 2 is used to force frequent splits and merges.
//   - Tests use explicit Arrange / Act / Assert sections.
//   - The identifier `test` is used instead of the conventional `t`
//     for clarity and consistency with project conventions.
//
// -----------------------------------------------------------------------------
package btree

import "testing"

// TestNewBTreeCreatesValidEmptyTree verifies that a new B-Tree is initialized
// correctly with an empty root node.
func TestNewBTreeCreatesValidEmptyTree(test *testing.T) {
	// Arrange.
	minimumDegree := 2

	// Act.
	tree := NewBTree(minimumDegree)

	// Assert.
	if tree == nil {
		test.Fatal("expected non-nil tree")
	}

	if tree.root == nil {
		test.Fatal("expected root node to be initialized")
	}

	if !tree.root.isLeafNode {
		test.Fatal("expected root to start as a leaf")
	}

	if len(tree.root.storedKeys) != 0 {
		test.Fatal("expected root to contain no keys")
	}
}

// TestInsertAndSearchSingleKey verifies insertion and search of a single key.
func TestInsertAndSearchSingleKey(test *testing.T) {
	// Arrange.
	tree := NewBTree(2)

	key := 42

	// Act.
	tree.Insert(key)
	found := tree.Search(key)

	// Assert.
	if !found {
		test.Fatalf("expected key %d to be found after insertion", key)
	}
}

// TestInsertMultipleKeysAndSearch verifies insertion of many keys and
// confirms they are all searchable.
func TestInsertMultipleKeysAndSearch(test *testing.T) {
	// Arrange.
	tree := NewBTree(2)

	keysToInsert := []int{10, 20, 5, 6, 12, 30, 7, 17}

	// Act.
	for _, value := range keysToInsert {
		tree.Insert(value)
	}

	// Assert.
	for _, value := range keysToInsert {
		if !tree.Search(value) {
			test.Fatalf("expected key %d to be found in tree", value)
		}
	}
}

// TestSearchForMissingKey verifies that searching for a non-existent key
// returns false.
func TestSearchForMissingKey(test *testing.T) {
	// Arrange.
	tree := NewBTree(2)

	tree.Insert(10)
	tree.Insert(20)

	// Act.
	found := tree.Search(99)

	// Assert.
	if found {
		test.Fatal("expected missing key to not be found")
	}
}

// TestRootSplittingOccurs verifies that inserting enough keys causes
// the root to split and become an internal node.
func TestRootSplittingOccurs(test *testing.T) {
	// Arrange.
	tree := NewBTree(2)

	keysToInsert := []int{1, 2, 3, 4}

	// Act.
	for _, value := range keysToInsert {
		tree.Insert(value)
	}

	// Assert.
	if tree.root.isLeafNode {
		test.Fatal("expected root to be internal after split")
	}

	if len(tree.root.childPointers) < 2 {
		test.Fatal("expected root to have multiple children after split")
	}
}

// TestRemoveLeafKey verifies removal of a key from a leaf node.
func TestRemoveLeafKey(test *testing.T) {
	// Arrange.
	tree := NewBTree(2)

	tree.Insert(5)
	tree.Insert(10)
	tree.Insert(15)

	// Act.
	tree.Remove(10)

	// Assert.
	if tree.Search(10) {
		test.Fatal("expected key 10 to be removed")
	}
}

// TestRemoveInternalKeyTriggersReplacement verifies that removing an internal
// key correctly replaces it with predecessor or successor.
func TestRemoveInternalKeyTriggersReplacement(test *testing.T) {
	// Arrange.
	tree := NewBTree(2)

	keysToInsert := []int{10, 20, 5, 6, 12, 30, 7, 17}

	for _, value := range keysToInsert {
		tree.Insert(value)
	}

	// Act.
	tree.Remove(10)

	// Assert.
	if tree.Search(10) {
		test.Fatal("expected internal key 10 to be removed")
	}
}

// TestRemoveTriggersBorrowing verifies deletion logic that requires borrowing
// from a sibling node.
func TestRemoveTriggersBorrowing(test *testing.T) {
	// Arrange.
	tree := NewBTree(2)

	keysToInsert := []int{1, 2, 3, 4, 5}

	for _, value := range keysToInsert {
		tree.Insert(value)
	}

	// Act.
	tree.Remove(1)
	tree.Remove(2)

	// Assert.
	if tree.Search(1) || tree.Search(2) {
		test.Fatal("expected removed keys to be absent")
	}
}

// TestRemoveTriggersMerge verifies deletion logic that requires merging nodes.
func TestRemoveTriggersMerge(test *testing.T) {
	// Arrange.
	tree := NewBTree(2)

	keysToInsert := []int{1, 2, 3, 4}

	for _, value := range keysToInsert {
		tree.Insert(value)
	}

	// Act.
	tree.Remove(1)
	tree.Remove(2)
	tree.Remove(3)

	// Assert.
	if tree.Search(1) || tree.Search(2) || tree.Search(3) {
		test.Fatal("expected merged keys to be removed")
	}
}

// TestRootShrinksAfterDeletion verifies that the tree height shrinks
// when the root becomes empty.
func TestRootShrinksAfterDeletion(test *testing.T) {
	// Arrange.
	tree := NewBTree(2)

	keysToInsert := []int{1, 2, 3, 4}

	for _, value := range keysToInsert {
		tree.Insert(value)
	}

	// Act.
	for _, value := range keysToInsert {
		tree.Remove(value)
	}

	// Assert.
	if tree.root == nil {
		test.Fatal("expected root to remain allocated")
	}

	if !tree.root.isLeafNode {
		test.Fatal("expected root to shrink back to leaf")
	}

	if len(tree.root.storedKeys) != 0 {
		test.Fatal("expected root to contain no keys after full deletion")
	}
}

// TestDuplicateKeyInsertion verifies that duplicate keys are allowed
// and remain searchable.
func TestDuplicateKeyInsertion(test *testing.T) {
	// Arrange.
	tree := NewBTree(2)

	key := 42

	// Act.
	tree.Insert(key)
	tree.Insert(key)

	// Assert.
	if !tree.Search(key) {
		test.Fatal("expected duplicate key to be searchable")
	}
}

// TestRemoveFromEmptyTree verifies that removing a key from an empty
// tree does not panic or corrupt state.
func TestRemoveFromEmptyTree(test *testing.T) {
	// Arrange.
	tree := NewBTree(2)

	// Act.
	tree.Remove(42)

	// Assert.
	if tree.root == nil {
		test.Fatal("expected root to remain initialized")
	}

	if len(tree.root.storedKeys) != 0 {
		test.Fatal("expected tree to remain empty after removal")
	}
}

// TestSearchOnEmptyTree verifies that searching an empty tree
// always returns false.
func TestSearchOnEmptyTree(test *testing.T) {
	// Arrange.
	tree := NewBTree(2)

	// Act.
	found := tree.Search(100)

	// Assert.
	if found {
		test.Fatal("expected search on empty tree to return false")
	}
}

// TestInsertAscendingOrder verifies that inserting keys in strictly
// ascending order preserves correctness.
func TestInsertAscendingOrder(test *testing.T) {
	// Arrange.
	tree := NewBTree(2)

	keysToInsert := []int{1, 2, 3, 4, 5, 6, 7, 8}

	// Act.
	for _, value := range keysToInsert {
		tree.Insert(value)
	}

	// Assert.
	for _, value := range keysToInsert {
		if !tree.Search(value) {
			test.Fatalf("expected key %d to be found after ascending inserts", value)
		}
	}
}

// TestInsertDescendingOrder verifies that inserting keys in strictly
// descending order preserves correctness.
func TestInsertDescendingOrder(test *testing.T) {
	// Arrange.
	tree := NewBTree(2)

	keysToInsert := []int{8, 7, 6, 5, 4, 3, 2, 1}

	// Act.
	for _, value := range keysToInsert {
		tree.Insert(value)
	}

	// Assert.
	for _, value := range keysToInsert {
		if !tree.Search(value) {
			test.Fatalf("expected key %d to be found after descending inserts", value)
		}
	}
}

// TestRemoveNonExistentKey verifies that removing a key that does not
// exist does not affect the tree.
func TestRemoveNonExistentKey(test *testing.T) {
	// Arrange.
	tree := NewBTree(2)

	keysToInsert := []int{10, 20, 30}

	for _, value := range keysToInsert {
		tree.Insert(value)
	}

	// Act.
	tree.Remove(999)

	// Assert.
	for _, value := range keysToInsert {
		if !tree.Search(value) {
			test.Fatalf("expected key %d to remain after removing non-existent key", value)
		}
	}
}

// TestRepeatedInsertAndRemove verifies stability under repeated
// insert/remove cycles.
func TestRepeatedInsertAndRemove(test *testing.T) {
	// Arrange.
	tree := NewBTree(2)

	keysToUse := []int{5, 10, 15, 20}

	// Act.
	for iterationIndex := 0; iterationIndex < 3; iterationIndex++ {
		for _, value := range keysToUse {
			tree.Insert(value)
		}

		for _, value := range keysToUse {
			tree.Remove(value)
		}
	}

	// Assert.
	for _, value := range keysToUse {
		if tree.Search(value) {
			test.Fatalf("expected key %d to be absent after repeated cycles", value)
		}
	}

	if len(tree.root.storedKeys) != 0 {
		test.Fatal("expected tree to be empty after repeated insert/remove cycles")
	}
}

// TestRemoveAllButOneKey verifies that deleting down to a single key
// leaves a valid B-Tree structure.
func TestRemoveAllButOneKey(test *testing.T) {
	// Arrange.
	tree := NewBTree(2)

	keysToInsert := []int{1, 2, 3, 4, 5}

	for _, value := range keysToInsert {
		tree.Insert(value)
	}

	// Act.
	for removalIndex := 0; removalIndex < len(keysToInsert)-1; removalIndex++ {
		tree.Remove(keysToInsert[removalIndex])
	}

	// Assert.
	lastKey := keysToInsert[len(keysToInsert)-1]

	if !tree.Search(lastKey) {
		test.Fatalf("expected remaining key %d to still exist", lastKey)
	}

	if len(tree.root.storedKeys) != 1 {
		test.Fatal("expected exactly one key to remain in the tree")
	}
}
