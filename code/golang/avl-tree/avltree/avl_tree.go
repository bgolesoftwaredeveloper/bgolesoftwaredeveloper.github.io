// -----------------------------------------------------------------------------
// Package: avltree
// File: avl_tree.go
//
// Description:
//
//     This file defines the public-facing AVLTree type and its associated
//     operations. The AVLTree acts as a thin wrapper around the internal
//     node-based implementation, exposing a safe and minimal API for
//     inserting, searching, and deleting keys.
//
//     All balancing logic, height maintenance, and structural manipulation
//     are delegated to internal helper functions. This design ensures that
//     consumers of the package cannot violate AVL invariants through direct
//     node access or mutation.
//
// Notes:
//   - The AVLTree type intentionally exposes no traversal or structural
//     inspection methods.
//   - All operations are safe to call on an empty tree.
//   - Defensive guards are included to prevent misuse or unexpected states.
//
// -----------------------------------------------------------------------------

package avltree

// AVLTree represents a self-balancing binary search tree.
// The tree maintains a pointer to its root node. When the tree is empty,
// the root pointer is nil.
type AVLTree struct {
	root *node
}

// New creates and returns a new, empty AVL tree.
// The returned tree contains no nodes and is ready for use.
func New() *AVLTree {
	return &AVLTree{root: nil}
}

func (tree *AVLTree) Insert(key int) {
	// Defensive guard: allow Insert to be called on a nil receiver.
	// This prevents a panic and mirrors safe no-op behavior.
	if tree == nil {
		return
	}

	// Delegate insertion and rebalancing to the internal helper.
	tree.root = insertNode(tree.root, key)
}

func (tree *AVLTree) Search(key int) bool {
	// Defensive guard: searching a nil tree always fails safely.
	if tree == nil {
		return false
	}

	// Start traversal from the root node.
	current := tree.root

	// Iteratively traverse the tree using BST ordering.
	for current != nil {
		// Move left if the key is smaller than the current node's key.
		if key < current.key {
			current = current.left
		} else if key > current.key {
			// Move right if the key is larger than the current node's key.
			current = current.right
		} else {
			// Key found.
			return true
		}
	}

	// Key not found in the tree.
	return false
}

func (tree *AVLTree) Delete(key int) {
	// Defensive guard: allow Delete to be called on a nil receiver.
	if tree == nil {
		return
	}

	// Delegate deletion and rebalancing to the internal helper.
	tree.root = deleteNode(tree.root, key)
}
