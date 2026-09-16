// -----------------------------------------------------------------------------
// Package: treap
// File: treap.go
//
// Description:
//
//	This file provides a generic implementation of a Treap data structure—a
//	randomized binary search tree combining standard Binary Search Tree (BST)
//	ordering with Max-Heap priorities.
//
//	The primary purpose of this data structure is to maintain balanced dynamic
//	key sets while guaranteeing expected O(log n) time complexity for search,
//	insertion, deletion, and order-statistic operations.
//
//	The Treap implementation is built using fundamental functional primitives:
//
//	  1. Split — divides a tree into two subtrees based on a target key.
//	  2. Merge — joins two ordered subtrees while preserving heap invariants.
//
// Key Features & Design Principles:
//
//   - Fully generic key support using Go's cmp.Ordered constraint.
//   - Order statistics tracking (Rank and FindKth) via subtree size metadata.
//   - Modern pseudo-random priority generation powered by math/rand/v2.
//   - Functional tree restructuring operations without manual rotations.
//
// Structural Invariants:
//
//   - BST Invariant: Left.Key <= Parent.Key < Right.Key
//   - Max-Heap Invariant: Parent.Priority >= Child.Priority
//   - Size Invariant: Node.Size == 1 + Node.Left.Size + Node.Right.Size
//
// Intended Use:
//
//	This package is intended as a high-performance, general-purpose balanced
//	search tree for applications requiring dynamic sorting, index-based order
//	queries, or range splits.
//
// -----------------------------------------------------------------------------
package treap

import (
	"cmp"
	"math/rand/v2"
)

// Node represents an individual element inside the balanced treap structure.
// It stores a comparable key, a randomized heap priority, subtree metadata,
// and pointers to left and right child nodes.
type Node[KeyType cmp.Ordered] struct {
	Key      KeyType
	Priority uint64
	Size     int
	Left     *Node[KeyType]
	Right    *Node[KeyType]
}

// Treap manages the root node of the tree and holds the dedicated pseudo-random
// engine used for priority generation.
type Treap[KeyType cmp.Ordered] struct {
	Root         *Node[KeyType]
	randomEngine *rand.Rand
}

// size safely calculates and returns the size of a subtree.
// Returns 0 if the provided node pointer is nil.
func size[KeyType cmp.Ordered](node *Node[KeyType]) int {
	if node == nil {
		return 0
	}

	return node.Size
}

// refreshMetadata recalculates and updates the subtree size for a given node
// based on the sizes of its immediate left and right child nodes.
func (node *Node[KeyType]) refreshMetadata() {
	if node != nil {
		node.Size = 1 + size(node.Left) + size(node.Right)
	}
}

// isolateMatchingKey splits a subtree to isolate nodes matching a specific target key
// from nodes strictly smaller than that key.
func isolateMatchingKey[KeyType cmp.Ordered](branch *Node[KeyType], key KeyType) (strictlyLesser, matchingKey *Node[KeyType]) {
	if branch == nil {
		return nil, nil
	}

	// Traverse right if the current node key is strictly smaller than target key.
	if branch.Key < key {
		var rightMatches *Node[KeyType]

		branch.Right, rightMatches = isolateMatchingKey(branch.Right, key)
		branch.refreshMetadata()

		return branch, rightMatches
	}

	// Traverse left if current node key matches or is larger than target key.
	var leftLesser *Node[KeyType]

	leftLesser, branch.Left = isolateMatchingKey(branch.Left, key)
	branch.refreshMetadata()

	return leftLesser, branch
}

// NewTreap initializes and returns a new generic Treap instance powered by a
// PCG pseudo-random generator configured with custom state and stream values.
func NewTreap[KeyType cmp.Ordered](state, stream uint64) *Treap[KeyType] {
	source := rand.NewPCG(state, stream)

	return &Treap[KeyType]{
		randomEngine: rand.New(source),
	}
}

// Split divides a treap rooted at parent into two disjoint subtrees:
//   - leftSubtree: contains all nodes with keys <= key
//   - rightSubtree: contains all nodes with keys > key
func Split[KeyType cmp.Ordered](parent *Node[KeyType], key KeyType) (leftSubtree, rightSubtree *Node[KeyType]) {
	// Base case: splitting an empty subtree yields two nil roots.
	if parent == nil {
		return nil, nil
	}

	// Current key belongs in the left subtree; split its right child branch.
	if parent.Key <= key {
		var extractedRight *Node[KeyType]

		parent.Right, extractedRight = Split(parent.Right, key)
		parent.refreshMetadata()

		return parent, extractedRight
	}

	// Current key belongs in the right subtree; split its left child branch.
	var extractedLeft *Node[KeyType]

	extractedLeft, parent.Left = Split(parent.Left, key)
	parent.refreshMetadata()

	return extractedLeft, parent
}

// Insert adds a new key to the Treap. Generates a random 64-bit priority
// and uses functional Split and Merge primitives to maintain structural balance.
func (treap *Treap[KeyType]) Insert(key KeyType) {
	// Allocate a fresh node with a randomized heap priority.
	freshNode := &Node[KeyType]{
		Key:      key,
		Priority: treap.randomEngine.Uint64(),
		Size:     1,
	}

	// Split tree around insertion key and merge new node into position.
	lesserPart, greaterPart := Split(treap.Root, key)

	treap.Root = Merge(Merge(lesserPart, freshNode), greaterPart)
}

// Merge combines two disjoint treaps into a single tree while maintaining heap order.
// Precondition: All keys in leftTree must be <= all keys in rightTree.
func Merge[KeyType cmp.Ordered](leftTree, rightTree *Node[KeyType]) *Node[KeyType] {
	// Base cases: merging with an empty tree returns the non-empty branch.
	if leftTree == nil {
		return rightTree
	}

	if rightTree == nil {
		return leftTree
	}

	// Preserve Max-Heap priority invariant.
	if leftTree.Priority > rightTree.Priority {
		leftTree.Right = Merge(leftTree.Right, rightTree)
		leftTree.refreshMetadata()

		return leftTree
	}

	rightTree.Left = Merge(leftTree, rightTree.Left)
	rightTree.refreshMetadata()

	return rightTree
}

// FindKth performs an order-statistic search returning the 0-indexed element
// corresponding to rank in the sorted sequence of keys.
// Returns default zero-value and false if rank is out of bounds.
func (treap *Treap[KeyType]) FindKth(rank int) (KeyType, bool) {
	traversalNode := treap.Root

	for traversalNode != nil {
		leftSize := size(traversalNode.Left)

		if rank < leftSize {
			// Target item resides within the left subtree.
			traversalNode = traversalNode.Left
		} else if rank == leftSize {
			// Found matching node corresponding to index rank.
			return traversalNode.Key, true
		} else {
			// Subtract left subtree size plus current node to search right subtree.
			rank -= (leftSize + 1)
			traversalNode = traversalNode.Right
		}
	}

	var defaultZero KeyType

	return defaultZero, false
}

// Rank computes the 0-indexed position (count of strictly smaller keys)
// for a target key within the Treap.
func (treap *Treap[KeyType]) Rank(key KeyType) int {
	traversalNode := treap.Root
	accumulatedRank := 0

	for traversalNode != nil {
		if key <= traversalNode.Key {
			// Search left branch for smaller keys.
			traversalNode = traversalNode.Left
		} else {
			// Account for left subtree elements plus current parent node.
			accumulatedRank += size(traversalNode.Left) + 1
			traversalNode = traversalNode.Right
		}
	}

	return accumulatedRank
}

// Delete removes one instance matching the target key from the Treap.
// If the key does not exist, the treap structure remains unaffected.
func (treap *Treap[KeyType]) Delete(key KeyType) {
	// Partition tree to isolate matching key node.
	lesserOrEqualPart, strictlyGreaterPart := Split(treap.Root, key)
	strictlyLesserPart, matchingNode := isolateMatchingKey(lesserOrEqualPart, key)

	// Remove target node by merging its immediate left and right children.
	if matchingNode != nil {
		matchingNode = Merge(matchingNode.Left, matchingNode.Right)
	}

	// Reconstruct overall tree structure.
	treap.Root = Merge(Merge(strictlyLesserPart, matchingNode), strictlyGreaterPart)
}
