// -----------------------------------------------------------------------------
// Package: linkcuttree
// File: link_cut_tree.go
//
// Description:
//
//	This file contains a reference implementation of a Link-Cut Tree (LCT)
//	using splay trees as the underlying auxiliary data structure.
//
//	A Link-Cut Tree maintains a dynamic forest of rooted trees under edge
//	insertions (link) and deletions (cut). It supports fast path decomposition
//	by representing each preferred path as a splay tree.
//
//	This implementation prioritizes:
//
//	  - Correctness and conceptual clarity.
//	  - Explicit naming and defensive guards.
//	  - Heavy internal documentation explaining *why* each step exists.
//	  - Educational readability over extreme performance tuning.
//
//	The data structure supports:
//
//	  - Linking two nodes belonging to different trees.
//	  - Cutting an existing edge.
//	  - Rerooting a tree at an arbitrary node.
//	  - Connectivity queries.
//
// Design Notes:
//
//   - Each node participates in two structures simultaneously:
//     1. The represented forest (conceptual tree).
//     2. An auxiliary splay tree (preferred path).
//
//   - Parent pointers mean *different things* depending on context:
//
//   - parent: splay parent OR path-parent depending on position.
//
//   - left/right: splay tree children only.
//
//   - Lazy propagation is used to reverse paths when rerooting.
//
// Intended Use:
//
//	This implementation is intended for:
//
//	  - Algorithmic education.
//	  - Competitive programming reference.
//	  - Advanced dynamic tree experimentation.
//	  - Systems-level algorithm study.
//
// -----------------------------------------------------------------------------
package linkcuttree

// LCTNode represents a single node in a Link-Cut Tree.
// Each node participates in an auxiliary splay tree representing a preferred
// path in the represented forest.
type LCTNode struct {
	left   *LCTNode
	right  *LCTNode
	parent *LCTNode

	reverseFlag bool
	subtreeSize int
}

// isSplayRoot reports whether this node is the root of its auxiliary splay tree.
// This helper assumes a non-nil receiver and is only called on valid nodes.
func (node *LCTNode) isSplayRoot() bool {
	// Treat nil nodes and parentless nodes as splay roots.
	if node == nil || node.parent == nil {
		return true
	}

	return node.parent.left != node && node.parent.right != node
}

// pushDown propagates any pending lazy path reversals to the node’s children.
// This function must be called before any structural modification or traversal
// that depends on the correct left/right child ordering.
func (node *LCTNode) pushDown() {
	// No action is required if there is no pending reversal.
	if node == nil || !node.reverseFlag {
		return
	}

	// Swap the left and right children to reverse the path orientation.
	node.left, node.right = node.right, node.left

	if node.left != nil {
		node.left.reverseFlag = !node.left.reverseFlag
	}

	if node.right != nil {
		node.right.reverseFlag = !node.right.reverseFlag
	}

	// Clear the reversal flag on this node after propagation.
	node.reverseFlag = false
}

// update recomputes cached auxiliary subtree metadata for this node.
// The computation is based solely on the current splay-tree children.
// Calling this method on a nil receiver has no effect.
func (node *LCTNode) update() {
	// Guard against nil receivers.
	if node == nil {
		return
	}

	// Start with the node itself.
	node.subtreeSize = 1

	// Incorporate the sizes of the left and right subtrees if present.
	if node.left != nil {
		node.subtreeSize += node.left.subtreeSize
	}

	if node.right != nil {
		node.subtreeSize += node.right.subtreeSize
	}
}

// rotate performs a single splay rotation between this node and its parent.
// Lazy propagation is flushed prior to rotation to ensure correctness.
// After rotation, subtree metadata is recomputed bottom-up.
func (node *LCTNode) rotate() {
	parentNode := node.parent
	grandparent := parentNode.parent

	// Ensure pending lazy operations are applied before structural changes.
	parentNode.pushDown()
	node.pushDown()

	if parentNode.left == node {
		parentNode.left = node.right

		if node.right != nil {
			node.right.parent = parentNode
		}

		node.right = parentNode
	} else {
		parentNode.right = node.left

		if node.left != nil {
			node.left.parent = parentNode
		}

		node.left = parentNode
	}

	// Reassign parent pointers.
	parentNode.parent = node
	node.parent = grandparent

	// Reattach to the grandparent if it exists.
	if grandparent != nil {
		if grandparent.left == parentNode {
			grandparent.left = node
		} else if grandparent.right == parentNode {
			grandparent.right = node
		}
	}

	// Recompute subtree metadata after rotation.
	parentNode.update()
	node.update()
}

// splay moves this node to the root of its auxiliary splay tree.
// The operation applies lazy propagation top-down before performing
// rotations to ensure structural correctness.
func (node *LCTNode) splay() {
	// Guard against nil receivers.
	if node == nil {
		return
	}

	var ancestorStack []*LCTNode

	current := node

	// Collect all ancestors up to the splay root.
	for {
		ancestorStack = append(ancestorStack, current)

		if current.isSplayRoot() {
			break
		}

		current = current.parent
	}

	// Push down lazy flags from the root toward the node.
	for index := len(ancestorStack) - 1; index >= 0; index-- {
		ancestorStack[index].pushDown()
	}

	// Perform splay rotations until the node becomes the root.
	for !node.isSplayRoot() {
		parentNode := node.parent

		if parentNode.isSplayRoot() {
			node.rotate()
		} else {
			grandparent := parentNode.parent

			// Distinguish between zig-zig and zig-zag cases.
			if (grandparent.left == parentNode) == (parentNode.left == node) {
				parentNode.rotate()
				node.rotate()
			} else {
				node.rotate()
				node.rotate()
			}
		}
	}
}

// access exposes the preferred path from this node to the root of the
// represented tree.
// The operation rearranges auxiliary splay trees so that this node becomes
// the rightmost node in its preferred path.
func (node *LCTNode) access() {
	// Guard against nil receivers.
	if node == nil {
		return
	}

	var lastVisited *LCTNode

	current := node

	// Walk upward through path-parent links, rebuilding preferred paths.
	for current != nil {
		current.splay()

		// Detach the previous preferred path and attach the new one.
		current.right = lastVisited
		current.update()

		lastVisited = current

		current = current.parent
	}

	// Ensure the accessed node is the splay root.
	node.splay()
}

// MakeRoot reroots the represented tree at this node.
// This is achieved by exposing the path to the current root and
// lazily reversing the preferred path.
func (node *LCTNode) MakeRoot() {
	// Guard against nil receivers.
	if node == nil {
		return
	}

	node.access()
	node.reverseFlag = !node.reverseFlag
}

// FindRoot returns the root of the represented tree containing this node.
// The operation exposes the path to the root and then traverses to the
// leftmost node of the auxiliary splay tree.
func (node *LCTNode) FindRoot() *LCTNode {
	// Guard against nil receivers.
	if node == nil {
		return nil
	}

	node.access()

	current := node
	current.pushDown()

	for current.left != nil {
		current = current.left
		current.pushDown()
	}

	current.splay()

	return current
}

// Connected reports whether two nodes belong to the same represented tree.
// The check is performed by comparing the roots of the trees containing
// each node. If either node is nil, the function returns false.
func Connected(reference, candidate *LCTNode) bool {
	if reference == nil || candidate == nil {
		return false
	}

	return reference.FindRoot() == candidate.FindRoot()
}

// Link attaches the subtree rooted at child beneath parent.
// The operation is ignored if the two nodes already belong to the same tree.
func Link(child, parent *LCTNode) {
	// Guard against nil receivers.
	if child == nil || parent == nil {
		return
	}

	child.MakeRoot()

	// Prevent cycles by ensuring the trees are distinct.
	if child.FindRoot() == parent.FindRoot() {
		return
	}

	child.parent = parent
}

// Cut removes the edge between child and parent if it exists.
// After the operation, the two nodes belong to separate trees.
func Cut(child, parent *LCTNode) {
	// Guard against nil receivers.
	if child == nil || parent == nil {
		return
	}

	child.MakeRoot()
	parent.access()

	// Validate that the edge is directly represented.
	if parent.left != child || child.right != nil {
		return
	}

	// Sever the connection.
	parent.left.parent = nil
	parent.left = nil
}
