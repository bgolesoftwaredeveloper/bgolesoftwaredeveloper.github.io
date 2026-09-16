// -----------------------------------------------------------------------------
// Package: treap
// File: treap_test.go
//
// Description:
//
//	This file contains unit tests for the generic Treap data structure.
//	The tests validate functional correctness, order statistics, tree balance,
//	and strict maintenance of Binary Search Tree (BST) and Heap invariants.
//
//	The test suite focuses on:
//
//	  - Correct insertion and deletion operations.
//	  - Exact order-statistic queries (FindKth and Rank).
//	  - Structural invariance (BST, Max-Heap, and Subtree Size constraints).
//	  - Handling of edge cases such as empty states and duplicate keys.
//	  - Deterministic random engine behavior via custom seeds.
//
// Notes:
//   - All tests use explicit Arrange / Act / Assert structure.
//   - The testing parameter is intentionally named "test" for clarity.
//   - Descriptive variable names are used throughout.
//   - Structural assertions run recursively using helper functions.
//
// -----------------------------------------------------------------------------
package treap

import (
	"cmp"
	"math/rand/v2"
	"sort"
	"testing"
)

// verifyTreapInvariants performs structural assertions checking BST ordering,
// Max-Heap priority layout, and accurate subtree sizing.
func verifyTreapInvariants[KeyType cmp.Ordered](test *testing.T, targetTreap *Treap[KeyType]) {
	test.Helper()

	var validateSubtree func(node *Node[KeyType]) int

	validateSubtree = func(node *Node[KeyType]) int {
		if node == nil {
			return 0
		}

		leftSubtreeSize := validateSubtree(node.Left)
		rightSubtreeSize := validateSubtree(node.Right)

		if node.Left != nil && node.Left.Key > node.Key {
			test.Fatalf("BST invariant violation: left child key (%v) > parent key (%v)",
				node.Left.Key,
				node.Key,
			)
		}

		if node.Right != nil && node.Right.Key < node.Key {
			test.Fatalf("BST invariant violation: right child key (%v) < parent key (%v)",
				node.Right.Key,
				node.Key,
			)
		}

		if node.Left != nil && node.Left.Priority > node.Priority {
			test.Fatalf("Max-Heap invariant violation: left child priority (%d) > parent priority (%d)",
				node.Left.Priority,
				node.Priority,
			)
		}

		if node.Right != nil && node.Right.Priority > node.Priority {
			test.Fatalf("Max-Heap invariant violation: right child priority (%d) > parent priority (%d)",
				node.Right.Priority,
				node.Priority,
			)
		}

		expectedSize := 1 + leftSubtreeSize + rightSubtreeSize

		if node.Size != expectedSize {
			test.Fatalf("Subtree size violation: node (%v) size is %d, expected %d",
				node.Key,
				node.Size,
				expectedSize,
			)
		}

		return expectedSize
	}

	validateSubtree(targetTreap.Root)
}

// TestNewTreap verifies that initializing a Treap creates an empty structure.
func TestNewTreap(test *testing.T) {
	// Arrange.
	seedState := uint64(1)
	seedStream := uint64(2)

	// Act.
	treapInstance := NewTreap[int](seedState, seedStream)

	// Assert.
	if treapInstance.Root != nil {
		test.Fatalf("expected root node to be nil for new Treap")
	}

	verifyTreapInvariants(test, treapInstance)
}

// TestTreap_InsertSingle verifies that inserting a single key creates a valid root.
func TestTreap_InsertSingle(test *testing.T) {
	// Arrange.
	treapInstance := NewTreap[int](10, 20)
	targetKey := 42

	// Act.
	treapInstance.Insert(targetKey)

	// Assert.
	retrievedKey, isFound := treapInstance.FindKth(0)
	if !isFound || retrievedKey != targetKey {
		test.Fatalf("expected key %d at rank 0, got %d (found: %v)",
			targetKey,
			retrievedKey,
			isFound,
		)
	}

	verifyTreapInvariants(test, treapInstance)
}

// TestTreap_InsertSequential verifies that keys inserted in ascending order
// maintain sorted order statistics and structural invariants.
func TestTreap_InsertSequential(test *testing.T) {
	// Arrange.
	treapInstance := NewTreap[int](1, 1)
	numberOfKeys := 10

	// Act.
	for currentKey := 1; currentKey <= numberOfKeys; currentKey++ {
		treapInstance.Insert(currentKey)
	}

	// Assert.
	for indexRank := 0; indexRank < numberOfKeys; indexRank++ {
		expectedValue := indexRank + 1
		retrievedKey, _ := treapInstance.FindKth(indexRank)

		if retrievedKey != expectedValue {
			test.Fatalf("expected key %d at rank %d, got %d",
				expectedValue,
				indexRank,
				retrievedKey,
			)
		}
	}

	verifyTreapInvariants(test, treapInstance)
}

// TestTreap_InsertReverseSequential verifies that keys inserted in descending order
// maintain proper internal ordering.
func TestTreap_InsertReverseSequential(test *testing.T) {
	// Arrange.
	treapInstance := NewTreap[int](5, 5)
	startingKey := 10

	// Act.
	for currentKey := startingKey; currentKey >= 1; currentKey-- {
		treapInstance.Insert(currentKey)
	}

	// Assert.
	for indexRank := 0; indexRank < startingKey; indexRank++ {
		expectedValue := indexRank + 1
		retrievedKey, _ := treapInstance.FindKth(indexRank)

		if retrievedKey != expectedValue {
			test.Fatalf("expected key %d at rank %d, got %d",
				expectedValue,
				indexRank,
				retrievedKey,
			)
		}
	}

	verifyTreapInvariants(test, treapInstance)
}

// TestTreap_FindKthOutOfBounds verifies defensive return values for invalid ranks.
func TestTreap_FindKthOutOfBounds(test *testing.T) {
	// Arrange.
	treapInstance := NewTreap[int](1, 2)
	treapInstance.Insert(10)
	treapInstance.Insert(20)

	// Act.
	_, isNegativeFound := treapInstance.FindKth(-1)
	_, isExceedingFound := treapInstance.FindKth(2)

	// Assert.
	if isNegativeFound {
		test.Fatalf("expected isFound to be false for negative rank query")
	}

	if isExceedingFound {
		test.Fatalf("expected isFound to be false for rank query beyond total count")
	}
}

// TestTreap_RankExistingKeys verifies position reporting for keys present in the tree.
func TestTreap_RankExistingKeys(test *testing.T) {
	// Arrange.
	treapInstance := NewTreap[int](100, 200)
	keysToInsert := []int{50, 20, 80, 10, 30}

	for _, key := range keysToInsert {
		treapInstance.Insert(key)
	}

	expectedRanks := map[int]int{
		10: 0,
		20: 1,
		30: 2,
		50: 3,
		80: 4,
	}

	// Act & Assert.
	for key, expectedRank := range expectedRanks {
		calculatedRank := treapInstance.Rank(key)
		if calculatedRank != expectedRank {
			test.Fatalf("expected rank for key %d to be %d, got %d",
				key,
				expectedRank,
				calculatedRank,
			)
		}
	}
}

// TestTreap_RankNonExistingKeys verifies relative rank queries for keys not in the tree.
func TestTreap_RankNonExistingKeys(test *testing.T) {
	// Arrange.
	treapInstance := NewTreap[int](42, 84)

	treapInstance.Insert(10)
	treapInstance.Insert(30)
	treapInstance.Insert(50)

	// Act & Assert.
	if rank := treapInstance.Rank(5); rank != 0 {
		test.Fatalf("expected rank 0 for key below minimum, got %d", rank)
	}

	if rank := treapInstance.Rank(25); rank != 1 {
		test.Fatalf("expected rank 1 for key between 10 and 30, got %d", rank)
	}

	if rank := treapInstance.Rank(60); rank != 3 {
		test.Fatalf("expected rank 3 for key above maximum, got %d", rank)
	}
}

// TestTreap_DeleteLeaf verifies removing a leaf node correctly updates the tree.
func TestTreap_DeleteLeaf(test *testing.T) {
	// Arrange.
	treapInstance := NewTreap[int](1, 2)

	treapInstance.Insert(10)
	treapInstance.Insert(20)

	// Act.
	treapInstance.Delete(20)

	// Assert.
	if _, isFound := treapInstance.FindKth(1); isFound {
		test.Fatalf("expected second node to be removed from tree")
	}

	verifyTreapInvariants(test, treapInstance)
}

// TestTreap_DeleteRoot verifies deleting the current root restructures correctly.
func TestTreap_DeleteRoot(test *testing.T) {
	// Arrange.
	treapInstance := NewTreap[int](7, 14)

	treapInstance.Insert(20)
	treapInstance.Insert(10)
	treapInstance.Insert(30)

	rootKeyBeforeDeletion := treapInstance.Root.Key

	// Act.
	treapInstance.Delete(rootKeyBeforeDeletion)

	// Assert.
	if treapInstance.Rank(10) != 0 || treapInstance.Rank(30) != 1 {
		test.Fatalf("expected remaining keys 10 and 30 to hold ranks 0 and 1 after root deletion")
	}

	verifyTreapInvariants(test, treapInstance)
}

// TestTreap_DeleteNonExistentKey verifies that removing a missing key leaves the tree intact.
func TestTreap_DeleteNonExistentKey(test *testing.T) {
	// Arrange.
	treapInstance := NewTreap[int](9, 9)

	treapInstance.Insert(10)
	treapInstance.Insert(20)

	nonExistentKey := 999

	// Act.
	treapInstance.Delete(nonExistentKey)

	// Assert.
	retrievedKey, _ := treapInstance.FindKth(0)

	if retrievedKey != 10 {
		test.Fatalf("expected first element to remain 10, got %d", retrievedKey)
	}

	verifyTreapInvariants(test, treapInstance)
}

// TestTreap_DeleteFromEmptyTreap verifies safety when attempting deletion on an empty tree.
func TestTreap_DeleteFromEmptyTreap(test *testing.T) {
	// Arrange.
	treapInstance := NewTreap[int](0, 0)

	// Act.
	treapInstance.Delete(5)

	// Assert.
	if treapInstance.Root != nil {
		test.Fatalf("expected root to remain nil after deleting from empty tree")
	}
}

// TestTreap_DeleteAll verifies sequential deletion until the tree is empty.
func TestTreap_DeleteAll(test *testing.T) {
	// Arrange.
	treapInstance := NewTreap[int](123, 456)
	elementsToInsert := []int{15, 8, 23, 42, 4}

	for _, element := range elementsToInsert {
		treapInstance.Insert(element)
	}

	// Act.
	for _, element := range elementsToInsert {
		treapInstance.Delete(element)
		verifyTreapInvariants(test, treapInstance)
	}

	// Assert.
	if treapInstance.Root != nil {
		test.Fatalf("expected treap to be completely empty after removing all elements")
	}
}

// TestTreap_DuplicateInsertions verifies handling duplicate key values.
func TestTreap_DuplicateInsertions(test *testing.T) {
	// Arrange.
	treapInstance := NewTreap[int](11, 22)

	duplicateKey := 10

	// Act.
	treapInstance.Insert(duplicateKey)
	treapInstance.Insert(duplicateKey)
	treapInstance.Insert(duplicateKey)

	// Assert.
	if treapInstance.Root.Size != 3 {
		test.Fatalf("expected total size 3 for duplicate keys, got %d",
			treapInstance.Root.Size,
		)
	}

	treapInstance.Delete(duplicateKey)

	if treapInstance.Root.Size != 2 {
		test.Fatalf("expected total size 2 after single instance deletion, got %d",
			treapInstance.Root.Size,
		)
	}

	verifyTreapInvariants(test, treapInstance)
}

// TestTreap_StringKeys verifies generic support for ordered string types.
func TestTreap_StringKeys(test *testing.T) {
	// Arrange.
	treapInstance := NewTreap[string](55, 66)
	wordList := []string{"orange", "apple", "banana"}

	// Act.
	for _, word := range wordList {
		treapInstance.Insert(word)
	}

	// Assert.
	retrievedWord, _ := treapInstance.FindKth(0)

	if retrievedWord != "apple" {
		test.Fatalf("expected 'apple' at rank 0, got '%s'", retrievedWord)
	}

	calculatedRank := treapInstance.Rank("banana")

	if calculatedRank != 1 {
		test.Fatalf("expected rank 1 for 'banana', got %d", calculatedRank)
	}

	verifyTreapInvariants(test, treapInstance)
}

// TestTreap_Float64Keys verifies generic support for floating point types.
func TestTreap_Float64Keys(test *testing.T) {
	// Arrange.
	treapInstance := NewTreap[float64](3, 9)
	floatList := []float64{3.14, 1.41, 2.71, 0.57}

	// Act.
	for _, floatValue := range floatList {
		treapInstance.Insert(floatValue)
	}

	// Assert.
	retrievedValue, _ := treapInstance.FindKth(0)
	if retrievedValue != 0.57 {
		test.Fatalf("expected 0.57 at rank 0, got %f", retrievedValue)
	}

	verifyTreapInvariants(test, treapInstance)
}

// TestTreap_Split verifies the Split primitive divides keys correctly relative to target threshold.
func TestTreap_Split(test *testing.T) {
	// Arrange.
	treapInstance := NewTreap[int](1, 2)

	for value := 10; value <= 50; value += 10 {
		treapInstance.Insert(value)
	}

	splitThreshold := 30

	// Act.
	leftSubtree, rightSubtree := Split(treapInstance.Root, splitThreshold)

	// Assert.
	if leftSubtree == nil || rightSubtree == nil {
		test.Fatalf("expected non-nil subtrees after Split operation")
	}

	if leftSubtree.Size != 3 || rightSubtree.Size != 2 {
		test.Fatalf("expected left size 3 and right size 2, got left=%d, right=%d",
			leftSubtree.Size,
			rightSubtree.Size,
		)
	}
}

// TestTreap_Merge verifies combining two disjoint treap structures.
func TestTreap_Merge(test *testing.T) {
	// Arrange.
	firstTreap := NewTreap[int](1, 2)

	firstTreap.Insert(10)
	firstTreap.Insert(20)

	secondTreap := NewTreap[int](3, 4)

	secondTreap.Insert(30)
	secondTreap.Insert(40)

	// Act.
	mergedRoot := Merge(firstTreap.Root, secondTreap.Root)

	// Assert.
	if mergedRoot.Size != 4 {
		test.Fatalf("expected merged root size 4, got %d", mergedRoot.Size)
	}
}

// TestTreap_StressRandomDataset verifies correctness against standard slice sorting across 1000 items.
func TestTreap_StressRandomDataset(test *testing.T) {
	// Arrange.
	treapInstance := NewTreap[int](999, 888)
	elementCount := 1000
	insertedElements := make([]int, elementCount)

	for index := 0; index < elementCount; index++ {
		randomValue := rand.IntN(10000)
		insertedElements[index] = randomValue
		treapInstance.Insert(randomValue)
	}

	verifyTreapInvariants(test, treapInstance)

	// Act.
	sort.Ints(insertedElements)

	// Assert.
	for indexRank := 0; indexRank < elementCount; indexRank++ {
		retrievedKey, _ := treapInstance.FindKth(indexRank)
		if retrievedKey != insertedElements[indexRank] {
			test.Fatalf("mismatch at rank %d: expected %d, got %d",
				indexRank,
				insertedElements[indexRank],
				retrievedKey,
			)
		}
	}

	for index := 0; index < elementCount/2; index++ {
		treapInstance.Delete(insertedElements[index])
	}

	verifyTreapInvariants(test, treapInstance)
}

// TestTreap_DeterministicSeed verifies that two instances with matching seed configurations produce identical structures.
func TestTreap_DeterministicSeed(test *testing.T) {
	// Arrange.
	stateSeed := uint64(12345)
	streamSeed := uint64(67890)

	firstTreap := NewTreap[int](stateSeed, streamSeed)
	secondTreap := NewTreap[int](stateSeed, streamSeed)

	insertionValues := []int{45, 12, 89, 33, 27, 91, 5, 62}

	// Act.
	for _, value := range insertionValues {
		firstTreap.Insert(value)
		secondTreap.Insert(value)
	}

	// Assert.
	var compareNodes func(source, target *Node[int]) bool

	compareNodes = func(source, target *Node[int]) bool {
		if source == nil && target == nil {
			return true
		}
		if source == nil || target == nil {
			return false
		}
		return source.Key == target.Key &&
			source.Priority == target.Priority &&
			compareNodes(source.Left, target.Left) &&
			compareNodes(source.Right, target.Right)
	}

	if !compareNodes(firstTreap.Root, secondTreap.Root) {
		test.Fatalf("expected identical internal structures for identical seeds")
	}
}
