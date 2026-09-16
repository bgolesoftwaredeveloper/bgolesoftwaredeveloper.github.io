// -----------------------------------------------------------------------------
// Package: balltree
// File: node.go
//
// Description:
//
//		This file defines the Node structure used internally by the Ball Tree
//		data structure. Each node represents a hyperspherical region ("ball")
//		that encloses a subset of points in the underlying metric space.
//
//		Nodes form a binary tree:
//	  - Internal nodes store a center point, a radius, and references to
//	    left and right child nodes.
//	  - Leaf nodes store explicit indices of points contained within the ball
//	    and do not have child nodes.
//
//		The Node type is intentionally simple and unexported in its fields,
//		as it is meant to be manipulated only by the Ball Tree implementation
//		itself.
//
// Notes:
//   - A node is considered a leaf only if both children are nil.
//   - Internal nodes should not contain point indices.
//   - Leaf nodes should not have child pointers.
//   - The tree may temporarily contain partially populated nodes during
//     construction, but query logic is resilient to such states.
//
// -----------------------------------------------------------------------------
package balltree

// Node represents a single node in the Ball Tree.
// Each node defines a ball via a center point and radius.
type Node struct {
	center []float64
	radius float64

	indices []int

	left  *Node
	right *Node
}

// isLeaf reports whether the node is a leaf node.
// A node is considered a leaf only if it has no children.
func (node *Node) isLeaf() bool {
	// Treat a nil node as a leaf to simplify recursive traversal logic.
	if node == nil {
		return true
	}

	// A node is a leaf if and only if both child pointers are nil.
	return node.left == nil && node.right == nil
}
