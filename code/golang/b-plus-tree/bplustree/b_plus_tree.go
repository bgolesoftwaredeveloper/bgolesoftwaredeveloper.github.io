// -----------------------------------------------------------------------------
// Package: bplustree
// File: bplustree.go
//
// Description:
//
//	This file provides a robust implementation of a B+ Tree data structure.
//	Unlike a standard B-Tree, the B+ Tree stores all actual data (keys) in
//	leaf nodes, while internal nodes act as a routing index.
//
//	The implementation supports:
//	  - Insertion with automatic node splitting and height growth.
//	  - Deletion with rebalancing via borrowing and merging to maintain
//	    the minimum fill factor.
//	  - Range Searching using a linked-list of leaf nodes for O(log n + k)
//	    performance.
//	  - Updating keys via a Delete-and-Insert strategy to preserve sorting.
//
// Notes:
//   - Parameterized by 'degree' (M), where each node can have at most M keys.
//   - Leaf nodes are linked via a 'next' pointer for efficient range scans.
//   - Internal nodes "push up" keys during splits, while leaves "copy" keys.
//
// -----------------------------------------------------------------------------
package bplustree

import (
	"sort"
)

// Configuration constants for tree balancing.
const degree = 3
const minimumKeys = degree / 2

// Node represents a single element in the B+ Tree.
// It can serve as either an internal routing node or a data-bearing leaf.
type Node struct {
	isLeaf   bool
	keys     []int
	children []*Node
	next     *Node
}

// BPlusTree manages the root of the tree and provides the public API
// for data manipulation and retrieval.
type BPlusTree struct {
	root *Node
}

// NewBPlusTree initializes an empty B+ Tree with a single leaf root.
func NewBPlusTree() *BPlusTree {
	return &BPlusTree{
		root: &Node{
			isLeaf: true,
			keys:   make([]int, 0),
		},
	}
}

// splitChild handles the overflow of a child node by splitting it into two.
// The middle key is promoted to the parent to maintain the search index.
func (tree *BPlusTree) splitChild(parent *Node, index int) {
	fullNode := parent.children[index]
	newNode := &Node{isLeaf: fullNode.isLeaf}

	middleIndex := degree / 2
	middleKey := fullNode.keys[middleIndex]

	// Case 1: Splitting a Leaf Node.
	// In a B+ Tree, the middle key stays in the leaf but is also copied to parent.
	if fullNode.isLeaf {
		newNode.keys = append([]int{}, fullNode.keys[middleIndex:]...)
		fullNode.keys = fullNode.keys[:middleIndex]

		// Maintain the linked list of leaves.
		newNode.next = fullNode.next
		fullNode.next = newNode
	} else {
		// Case 2: Splitting an Internal Node.
		// The middle key moves up to the parent and is removed from the children.
		newNode.keys = append([]int{}, fullNode.keys[middleIndex+1:]...)
		newNode.children = append([]*Node{}, fullNode.children[middleIndex+1:]...)

		fullNode.keys = fullNode.keys[:middleIndex]
		fullNode.children = fullNode.children[:middleIndex+1]
	}

	// Insert the promoted middle key into the parent node.
	parent.keys = append(parent.keys, 0)
	copy(parent.keys[index+1:], parent.keys[index:])
	parent.keys[index] = middleKey

	// Link the new sibling node into the parent's child list.
	parent.children = append(parent.children, nil)
	copy(parent.children[index+2:], parent.children[index+1:])
	parent.children[index+1] = newNode
}

// insertNonFull performs a recursive insertion into a node that is
// guaranteed to have room for at least one more key.
func (tree *BPlusTree) insertNonFull(node *Node, key int) {
	// If we reached a leaf, this is the final destination for the key.
	if node.isLeaf {
		node.keys = append(node.keys, key)
		sort.Ints(node.keys)

		return
	}

	// Determine which child subtree the key belongs in.
	index := sort.SearchInts(node.keys, key)
	child := node.children[index]

	// If the targeted child is full, split it before descending.
	if len(child.keys) == degree {
		tree.splitChild(node, index)

		// After the split, decide which of the two new nodes to enter.
		if key > node.keys[index] {
			index++
		}
	}

	tree.insertNonFull(node.children[index], key)
}

// fill ensures that a child node has at least the minimum number of keys
// by either borrowing from a sibling or merging with one.
func (tree *BPlusTree) fill(parent *Node, index int) {
	// Attempt to borrow from the left sibling.
	if index != 0 && len(parent.children[index-1].keys) > minimumKeys {
		tree.borrowFromPrevious(parent, index)
	} else if index != len(parent.keys) && len(parent.children[index+1].keys) > minimumKeys {
		// Attempt to borrow from the right sibling.
		tree.borrowFromNext(parent, index)
	} else {
		// Borrowing failed; perform a merge.
		if index != len(parent.keys) {
			tree.merge(parent, index)
		} else {
			tree.merge(parent, index-1)
		}
	}
}

// borrowFromPrevious takes a key from the left sibling and moves it
// to the underfull node via the parent's separator key.
func (tree *BPlusTree) borrowFromPrevious(parent *Node, index int) {
	child := parent.children[index]
	sibling := parent.children[index-1]

	numberOfSiblings := len(sibling.keys)

	if child.isLeaf {
		// For leaves: Move key directly and update parent separator.
		key := sibling.keys[numberOfSiblings-1]
		child.keys = append([]int{key}, child.keys...)
		sibling.keys = sibling.keys[:numberOfSiblings-1]
		parent.keys[index-1] = child.keys[0]
	} else {
		// For internal nodes: Rotate through the parent.
		child.keys = append([]int{parent.keys[index-1]}, child.keys...)
		parent.keys[index-1] = sibling.keys[numberOfSiblings-1]
		sibling.keys = sibling.keys[:numberOfSiblings-1]

		child.children = append([]*Node{sibling.children[numberOfSiblings-1]}, child.children...)
		sibling.children = sibling.children[:numberOfSiblings-1]
	}
}

// borrowFromNext takes a key from the right sibling and moves it
// to the underfull node via the parent's separator key.
func (tree *BPlusTree) borrowFromNext(parent *Node, index int) {
	child := parent.children[index]
	sibling := parent.children[index+1]

	if child.isLeaf {
		// For leaves: Move key directly and update parent separator.
		key := sibling.keys[0]
		child.keys = append(child.keys, key)
		sibling.keys = sibling.keys[1:]
		parent.keys[index] = sibling.keys[0]
	} else {
		// For internal nodes: Rotate through the parent.
		child.keys = append(child.keys, parent.keys[index])
		parent.keys[index] = sibling.keys[0]
		sibling.keys = sibling.keys[1:]

		child.children = append(child.children, sibling.children[0])
		sibling.children = sibling.children[1:]
	}
}

// merge combines two sibling nodes into one, pulling the separator
// key down from the parent.
func (tree *BPlusTree) merge(parent *Node, index int) {
	child := parent.children[index]
	sibling := parent.children[index+1]

	// Combine child pointers or update leaf linked list.
	if !child.isLeaf {
		child.keys = append(child.keys, parent.keys[index])
		child.keys = append(child.keys, sibling.keys...)
		child.children = append(child.children, sibling.children...)
	} else {
		child.keys = append(child.keys, sibling.keys...)
		child.next = sibling.next
	}

	// Remove the key and sibling pointer from the parent.
	parent.keys = append(parent.keys[:index], parent.keys[index+1:]...)
	parent.children = append(parent.children[:index+1], parent.children[index+2:]...)
}

// deleteRecursive descends the tree to find and remove a key,
// triggering rebalancing (fill) as it backtracks.
func (tree *BPlusTree) deleteRecursive(node *Node, key int) {
	index := sort.SearchInts(node.keys, key)

	numberOfKeys := len(node.keys)

	// Base Case: Key removal from a leaf.
	if node.isLeaf {
		if index < numberOfKeys && node.keys[index] == key {
			node.keys = append(node.keys[:index], node.keys[index+1:]...)
		}

		return
	}

	// Recurse into the appropriate child.
	childIndex := index

	if index < numberOfKeys && node.keys[index] == key {
		childIndex = index + 1
	}

	tree.deleteRecursive(node.children[childIndex], key)

	// After return, check if child node fell below minimum capacity.
	if len(node.children[childIndex].keys) < minimumKeys {
		tree.fill(node, childIndex)
	}
}

// Insert adds a new key to the B+ Tree, splitting the root if the
// tree needs to grow in height.
func (tree *BPlusTree) Insert(key int) {
	root := tree.root

	// If the root is full, create a new root and split the old one.
	if len(root.keys) == degree {
		newRoot := &Node{isLeaf: false}
		newRoot.children = append(newRoot.children, root)

		tree.splitChild(newRoot, 0)
		tree.root = newRoot
	}

	tree.insertNonFull(tree.root, key)
}

// RangeSearch finds all keys between start and end (inclusive).
// It exploits the leaf-level linked list for high efficiency.
func (tree *BPlusTree) RangeSearch(start, end int) []int {
	current := tree.root

	// Step 1: Descend to the leftmost leaf containing the start key.
	for !current.isLeaf {
		index := sort.SearchInts(current.keys, start)
		current = current.children[index]
	}

	var results []int

	// Step 2: Traversal across the leaf list.
	for current != nil {
		for _, key := range current.keys {
			if key >= start && key <= end {
				results = append(results, key)
			}

			// Optimization: Stop if the current key exceeds our range.
			if key > end {
				return results
			}
		}

		current = current.next
	}

	return results
}

// Delete removes a key from the tree and handles root-level height reduction.
func (tree *BPlusTree) Delete(key int) {
	tree.deleteRecursive(tree.root, key)

	// If the root became empty, promote its only child as the new root.
	if !tree.root.isLeaf && len(tree.root.keys) == 0 {
		if len(tree.root.children) > 0 {
			tree.root = tree.root.children[0]
		}
	}
}

// Update modifies an existing key's value. In a B+ Tree, this requires
// deleting the old key and inserting the new one to maintain sort order.
func (tree *BPlusTree) Update(oldKey, newKey int) {
	tree.Delete(oldKey)
	tree.Insert(newKey)
}
