// -----------------------------------------------------------------------------
// Package: avltree
// File: node.go
//
// Description:
//
//	This file defines the internal node structure used by the AVL tree
//	implementation. A node represents a single element in the tree and
//	stores a key value along with references to its left and right children.
//
//	Each node additionally tracks its height, which is a critical component
//	of the AVL balancing invariant. Heights are used to compute balance
//	factors and determine when and how rotations must be applied to restore
//	logarithmic tree height.
//
//	Nodes are strictly an internal implementation detail and are not
//	intended to be constructed, modified, or inspected directly by
//	consumers of the avltree package. All node allocation, mutation, and
//	structural changes are managed by higher-level tree operations to
//	ensure AVL invariants are consistently preserved.
//
// Height Definition:
//
//	The height of a node is defined as:
//
//	    1 + max(height(left), height(right))
//
//	Nil child nodes are defined to have height 0.
//
// Notes:
//   - Direct mutation of node fields outside this package may corrupt
//     AVL balance guarantees and is strongly discouraged.
//   - Height values are maintained eagerly during insertions, deletions,
//     and rotations to ensure balance checks remain O(1).
//
// -----------------------------------------------------------------------------
package avltree

// node represents a single node within an AVL tree.
// Each node stores a key and maintains pointers to its left and right
// child subtrees, along with a cached height value used for balancing.
type node struct {
	key    int
	left   *node
	right  *node
	height int
}

// newNode allocates and initializes a new AVL tree node.
// The newly created node has no children and a height of 1, reflecting
// the definition that a leaf node has height 1.
func newNode(key int) *node {
	// Allocate and initialize a leaf node.
	// Both child pointers are nil, and height is set explicitly to 1.
	return &node{
		key:    key,
		left:   nil,
		right:  nil,
		height: 1,
	}
}

// heightOf safely returns the height of a node.
// Nil nodes are defined to have height 0, which simplifies balance and
// height calculations by eliminating the need for nil checks at call sites.
func heightOf(node *node) int {
	// A nil node represents an empty subtree and has height 0 by definition.
	if node == nil {
		return 0
	}

	// Return the cached height for non-nil nodes.
	return node.height
}

// updateHeight recomputes and updates the height of a node based on the
// heights of its left and right children.
// This function must be called after any structural modification to a
// node's children (such as insertion, deletion, or rotation) to ensure
// height values remain accurate.
func updateHeight(junction *node) {
	// Retrieve the heights of the left and right subtrees.
	leftHeight := heightOf(junction.left)
	rightHeight := heightOf(junction.right)

	// The node height is defined as one plus the maximum child height.
	// This preserves the AVL height invariant.
	if leftHeight > rightHeight {
		junction.height = leftHeight + 1
	} else {
		junction.height = rightHeight + 1
	}
}

// balanceFactor computes the AVL balance factor for a node.
// The balance factor is defined as:
//
//	height(left subtree) - height(right subtree)
//
// A balance factor outside the range [-1, 1] indicates that the node
// violates AVL balance constraints and requires rebalancing.
func balanceFactor(junction *node) int {
	// Subtract right subtree height from left subtree height.
	// The sign indicates whether the node is left-heavy or right-heavy.
	return heightOf(junction.left) - heightOf(junction.right)
}
