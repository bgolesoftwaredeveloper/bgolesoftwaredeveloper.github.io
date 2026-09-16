// -----------------------------------------------------------------------------
// Package: avltree
// File: rotations.go
//
// Description:
//
//	This file implements the fundamental rotation operations used by the
//	AVL tree to restore balance after insertions and deletions.
//
//	Rotations preserve the binary search tree ordering invariant while
//	restructuring the tree to ensure that the height difference between
//	left and right subtrees remains within the allowable range [-1, 1].
//
//	Only single left and single right rotations are defined here. More
//	complex rebalancing cases (left-right and right-left) are composed
//	by combining these primitives in higher-level logic.
//
// Notes:
//   - Rotations must always be followed by height recomputation of all
//     affected nodes.
//   - Incorrect rotation logic or height updates will silently violate
//     AVL invariants and degrade performance guarantees.
//   - These functions are intended for internal use only and should not
//     be invoked directly by consumers of the avltree package.
//
// -----------------------------------------------------------------------------
package avltree

// rotateLeft performs a single left rotation around the given junction node.
// The rotation promotes the right child of the junction node to become the
// new root of the subtree, while the junction node becomes the left child
// of the new root. Any subtree previously attached to the left of the new
// root is transferred to the right of the junction node.
// This operation preserves binary search tree ordering and is used to
// correct right-heavy AVL imbalance cases.
func rotateLeft(junction *node) *node {
	// Identify the right child of the junction node.
	// This node will become the new root after rotation.
	newRoot := junction.right

	// Preserve the subtree that will be transferred.
	// This subtree must not be lost during restructuring.
	transferredSubtree := newRoot.left

	// Perform the rotation by reassigning pointers.
	// The junction becomes the left child of the new root.
	newRoot.left = junction

	// Attach the transferred subtree as the right child of the junction.
	junction.right = transferredSubtree

	// Update heights bottom-up.
	// The junction node must be updated before the new root.
	updateHeight(junction)
	updateHeight(newRoot)

	return newRoot
}

// rotateRight performs a single right rotation around the given junction node.
// The rotation promotes the left child of the junction node to become the
// new root of the subtree, while the junction node becomes the right child
// of the new root. Any subtree previously attached to the right of the new
// root is transferred to the left of the junction node.
// This operation preserves binary search tree ordering and is used to
// correct left-heavy AVL imbalance cases.
func rotateRight(junction *node) *node {
	// Identify the left child of the junction node.
	// This node will become the new root after rotation.
	newRoot := junction.left

	// Preserve the subtree that will be transferred.
	// This subtree must not be lost during restructuring.
	transferredSubtreee := newRoot.right

	// Perform the rotation by reassigning pointers.
	// The junction becomes the right child of the new root.
	newRoot.right = junction

	// Attach the transferred subtree as the left child of the junction.
	junction.left = transferredSubtreee

	// Update heights bottom-up.
	// The junction node must be updated before the new root.
	updateHeight(junction)
	updateHeight(newRoot)

	return newRoot
}
