// -----------------------------------------------------------------------------
// Package: avltree
// File: insert_delete.go
//
// Description:
//
//	This file implements the recursive insertion and deletion logic for
//	the AVL tree, including all required rebalancing operations.
//
//	Both insertion and deletion follow standard binary search tree
//	semantics, with additional height maintenance and balance checks
//	applied on the recursive unwind phase. When an imbalance is detected,
//	the appropriate rotation(s) are applied to restore AVL invariants.
//
//	All rebalancing logic is expressed in terms of single left and right
//	rotations defined in rotations.go.
//
// Notes:
//   - Duplicate keys are ignored during insertion.
//   - Deletion correctly handles leaf, single-child, and two-child cases.
//   - Height updates and balance checks must occur after every structural
//     modification to guarantee logarithmic tree height.
//
// -----------------------------------------------------------------------------
package avltree

// minimumValueNode returns the node with the smallest key in the subtree
// rooted at the provided junction node.
// This function is used during deletion to locate the inorder successor
// when removing a node that has two children.
func minimumValueNode(junction *node) *node {
	if junction == nil {
		return nil
	}

	current := junction

	// Traverse to the leftmost node in the subtree.
	for current.left != nil {
		current = current.left
	}

	return current
}

// insertNode recursively inserts a key into the AVL tree rooted at the
// provided junction node.
// The function returns the new root of the subtree after insertion and
// any necessary rebalancing. Standard binary search tree insertion rules
// are followed, after which AVL balance constraints are enforced.
// If the key already exists in the tree, the subtree is returned unchanged.
func insertNode(junction *node, key int) *node {
	// Base case: create a new node when the insertion point is reached.
	if junction == nil {
		return newNode(key)
	}

	// Traverse left or right according to BST ordering.
	if key < junction.key {
		junction.left = insertNode(junction.left, key)
	} else if key > junction.key {
		junction.right = insertNode(junction.right, key)
	} else {
		// Duplicate keys are not inserted.
		return junction
	}

	// Update the height of the current node after subtree modification.
	updateHeight(junction)

	// Compute the balance factor to detect imbalance.
	balance := balanceFactor(junction)

	// Defensive guard: rotation preconditions require child nodes.
	// These checks protect against structural corruption.
	if balance > 1 && junction.left != nil {
		// Left-Left case.
		if key < junction.left.key {
			return rotateRight(junction)
		}

		// Left-Right case.
		if key > junction.left.key {
			junction.left = rotateLeft(junction.left)
			return rotateRight(junction)
		}
	}

	if balance < -1 && junction.right != nil {
		// Right-Right case.
		if key > junction.right.key {
			return rotateLeft(junction)
		}

		// Right-Left case.
		if key < junction.right.key {
			junction.right = rotateRight(junction.right)
			return rotateLeft(junction)
		}
	}

	// No rebalancing required.
	return junction
}

// deleteNode recursively removes a key from the AVL tree rooted at the
// provided junction node.
// The function returns the new root of the subtree after deletion and any
// necessary rebalancing. All standard binary search tree deletion cases
// are handled, followed by AVL-specific height updates and rotations.
func deleteNode(junction *node, key int) *node {
	// Base case: key not found.
	if junction == nil {
		return nil
	}

	// Traverse left or right according to BST ordering.
	if key < junction.key {
		junction.left = deleteNode(junction.left, key)
	} else if key > junction.key {
		junction.right = deleteNode(junction.right, key)
	} else {
		// Case: node with one or zero children.
		if junction.left == nil || junction.right == nil {
			var replacement *node

			if junction.left != nil {
				replacement = junction.left
			} else {
				replacement = junction.right
			}

			// No child case.
			if replacement == nil {
				junction = nil
			} else {
				// One child case.
				junction = replacement
			}
		} else {
			// Case: node with two children.
			// Replace key with inorder successor.
			successor := minimumValueNode(junction.right)

			// Defensive guard: successor should never be nil here.
			if successor != nil {
				junction.key = successor.key
				junction.right = deleteNode(junction.right, successor.key)
			}
		}
	}

	// If the subtree is now empty, return immediately.
	if junction == nil {
		return nil
	}

	// Update the height of the current node after deletion.
	updateHeight(junction)

	// Compute the balance factor to detect imbalance.
	balance := balanceFactor(junction)

	// Defensive guards ensure child nodes exist before rotations.
	if balance > 1 && junction.left != nil {
		// Left-Left case.
		if balanceFactor(junction.left) >= 0 {
			return rotateRight(junction)
		}

		// Left-Right case.
		junction.left = rotateLeft(junction.left)

		return rotateRight(junction)
	}

	if balance < -1 && junction.right != nil {
		// Right-Right case.
		if balanceFactor(junction.right) <= 0 {
			return rotateLeft(junction)
		}

		// Right-Left case.
		junction.right = rotateRight(junction.right)

		return rotateLeft(junction)
	}

	// Tree is balanced at this node.
	return junction
}
