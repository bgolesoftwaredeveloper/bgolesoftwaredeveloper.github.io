// -----------------------------------------------------------------------------
// Package: avltree
// File: avl_tree_test.go
//
// Description:
//
//	This file contains unit tests for the AVL tree implementation. The
//	tests exercise all public-facing functionality exposed by the
//	AVLTree type, including tree creation, insertion, searching, and
//	deletion.
//
//	Although internal helpers such as rotations and height maintenance
//	are not tested directly, their correctness is implicitly validated
//	through observable tree behavior under various insertion and deletion
//	scenarios.
//
// Notes:
//   - Tests follow the Arrange / Act / Assert pattern consistently.
//   - Each test is deterministic and independent.
//   - Internal tree structure is not accessed directly, preserving
//     encapsulation guarantees.
//
// -----------------------------------------------------------------------------
package avltree

import "testing"

// -----------------------------------------------------------------------------
// Construction and Defensive Behavior.
// -----------------------------------------------------------------------------

// TestNew_EmptyTree verifies that a newly created AVL tree is empty and
// behaves correctly when queried.
func TestNew_EmptyTree(test *testing.T) {
	// Arrange.
	tree := New()

	// Act.
	found := tree.Search(42)

	// Assert.
	if found {
		test.Fatalf("expected search on empty tree to return false")
	}
}

// TestNilReceiverSafety verifies that calling methods on a nil AVLTree
// does not cause a panic.
func TestNilReceiverSafety(test *testing.T) {
	// Arrange.
	var tree *AVLTree

	// Act.
	tree.Insert(10)

	found := tree.Search(10)

	tree.Delete(10)

	// Assert.
	if found {
		test.Fatalf("expected nil tree search to return false")
	}
}

// -----------------------------------------------------------------------------
// Insertion.
// -----------------------------------------------------------------------------

// TestInsert_SingleElement verifies that inserting a single key stores
// the value correctly.
func TestInsert_SingleElement(test *testing.T) {
	// Arrange.
	tree := New()

	// Act.
	tree.Insert(10)

	found := tree.Search(10)

	// Assert.
	if !found {
		test.Fatalf("expected inserted key to be found in tree")
	}
}

// TestInsert_DuplicateKey verifies that inserting a duplicate key does
// not corrupt the tree or create duplicate entries.
func TestInsert_DuplicateKey(test *testing.T) {
	// Arrange.
	tree := New()

	tree.Insert(10)

	// Act.
	tree.Insert(10)

	found := tree.Search(10)

	// Assert.
	if !found {
		test.Fatalf("expected duplicate insert to preserve existing key")
	}
}

// TestInsert_MultipleElements verifies correct behavior after inserting
// multiple keys that require AVL rebalancing.
func TestInsert_MultipleElements(test *testing.T) {
	// Arrange.
	tree := New()

	keys := []int{30, 20, 40, 10, 25, 35, 50}

	// Act.
	for _, key := range keys {
		tree.Insert(key)
	}

	// Assert.
	for _, key := range keys {
		if !tree.Search(key) {
			test.Fatalf("expected key %d to be present after insertion", key)
		}
	}
}

// TestAVL_RebalancingBehavior verifies that the AVL tree remains functional
// after sequences that require multiple rotations.
func TestAVL_RebalancingBehavior(test *testing.T) {
	// Arrange.
	tree := New()

	keys := []int{3, 2, 1, 4, 5, 6}

	// Act.
	for _, key := range keys {
		tree.Insert(key)
	}

	// Assert.
	for _, key := range keys {
		if !tree.Search(key) {
			test.Fatalf("expected key %d to be present after rebalancing", key)
		}
	}
}

// -----------------------------------------------------------------------------
// Search.
// -----------------------------------------------------------------------------

// TestSearch_MissingKey verifies that searching for a non-existent key
// returns false.
func TestSearch_MissingKey(test *testing.T) {
	// Arrange.
	tree := New()

	tree.Insert(10)
	tree.Insert(20)

	// Act.
	found := tree.Search(15)

	// Assert.
	if found {
		test.Fatalf("expected search for missing key to return false")
	}
}

// -----------------------------------------------------------------------------
// Deletion.
// -----------------------------------------------------------------------------

// TestDelete_LeafNode verifies deletion of a leaf node.
func TestDelete_LeafNode(test *testing.T) {
	// Arrange.
	tree := New()

	tree.Insert(10)
	tree.Insert(5)

	// Act.
	tree.Delete(5)

	found := tree.Search(5)

	// Assert.
	if found {
		test.Fatalf("expected deleted leaf node to be absent")
	}
}

// TestDelete_NodeWithOneChild verifies deletion of a node that has exactly
// one child.
func TestDelete_NodeWithOneChild(test *testing.T) {
	// Arrange.
	tree := New()

	tree.Insert(10)
	tree.Insert(5)
	tree.Insert(2)

	// Act.
	tree.Delete(5)

	found := tree.Search(5)

	// Assert.
	if found {
		test.Fatalf("expected node with one child to be deleted")
	}

	if !tree.Search(2) {
		test.Fatalf("expected child of deleted node to remain in tree")
	}
}

// TestDelete_NodeWithTwoChildren verifies deletion of a node with two
// children and correct successor replacement.
func TestDelete_NodeWithTwoChildren(test *testing.T) {
	// Arrange.
	tree := New()

	keys := []int{20, 10, 30, 25, 40}

	for _, key := range keys {
		tree.Insert(key)
	}

	// Act.
	tree.Delete(20)

	found := tree.Search(20)

	// Assert.
	if found {
		test.Fatalf("expected node with two children to be deleted")
	}

	// Ensure remaining keys are still present.
	for _, key := range []int{10, 25, 30, 40} {
		if !tree.Search(key) {
			test.Fatalf("expected key %d to remain after deletion", key)
		}
	}
}

// TestDelete_NonExistentKey verifies that deleting a key that does not
// exist does not affect the tree.
func TestDelete_NonExistentKey(test *testing.T) {
	// Arrange.
	tree := New()

	tree.Insert(10)
	tree.Insert(20)

	// Act.
	tree.Delete(99)

	// Assert.
	if !tree.Search(10) || !tree.Search(20) {
		test.Fatalf("expected tree to remain unchanged after deleting missing key")
	}
}
