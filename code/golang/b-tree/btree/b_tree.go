// -----------------------------------------------------------------------------
// Package: btree
// File: btree.go
//
// Description:
//
//	This file provides an implementation of a B-Tree data structure with
//	support for insertion, search, and deletion operations.
//
//	The implementation follows the classical CLRS definition of a B-Tree
//	parameterized by a minimum degree (t). Each node maintains between
//	(t-1) and (2t-1) keys (except for the root), and all leaves reside at
//	the same depth.
//
//	Deletion is implemented with full support for borrowing from siblings,
//	merging nodes when underflow occurs, and shrinking the tree height when
//	the root becomes empty.
//
// Notes:
//   - This implementation allows duplicate keys.
//   - Keys are stored as integers and ordered using standard integer
//     comparison.
//   - All operations preserve B-Tree invariants at every step.
//   - The tree is maintained entirely in memory.
//
// -----------------------------------------------------------------------------
package btree

import "fmt"

// BTreeNode represents a single node within the B-Tree.
// Each node stores a sorted slice of keys and, if not a leaf,
// pointers to its child nodes.
type BTreeNode struct {
	minimumDegree int
	isLeafNode    bool
	storedKeys    []int
	childPointers []*BTreeNode
}

// BTree represents the B-Tree itself, tracking the root node
// and the global minimum degree parameter.
type BTree struct {
	minimumDegree int
	root          *BTreeNode
}

// NewBTree constructs and returns a new empty B-Tree with the
// specified minimum degree.
// The minimum degree must be at least 2.
func NewBTree(minimumDegree int) *BTree {
	// Enforce the B-Tree invariant that minimum degree t ≥ 2.
	if minimumDegree < 2 {
		panic("minimum degree must be at least 2")
	}

	// Initialize an empty root node.
	// The root starts as a leaf and may grow later.
	root := &BTreeNode{
		minimumDegree: minimumDegree,
		isLeafNode:    true,
		storedKeys:    make([]int, 0, 2*minimumDegree-1),
		childPointers: make([]*BTreeNode, 0, 2*minimumDegree),
	}

	// Return the constructed tree.
	return &BTree{
		minimumDegree: minimumDegree,
		root:          root,
	}
}

// splitChildAtIndex splits a full child node at the given index.
// The median key is promoted into the current node, and the
// overflowing child is split into two sibling nodes.
func (node *BTreeNode) splitChildAtIndex(index int) {
	// Guard: invalid receiver.
	if node == nil {
		return
	}

	// Guard: index must refer to an existing child.
	if index < 0 || index >= len(node.childPointers) {
		return
	}

	minimumDegree := node.minimumDegree

	// Fetch the child to be split.
	fullChild := node.childPointers[index]

	// Guard: child must exist.
	if fullChild == nil {
		return
	}

	// Only split when the child is actually full.
	if len(fullChild.storedKeys) < (2*node.minimumDegree - 1) {
		return
	}

	// Allocate the new right sibling node.
	newRightSibling := &BTreeNode{
		minimumDegree: minimumDegree,
		isLeafNode:    fullChild.isLeafNode,
		storedKeys:    make([]int, 0, 2*minimumDegree-1),
		childPointers: make([]*BTreeNode, 0, 2*minimumDegree),
	}

	// Identify the median key to promote upward.
	medianKeyIndex := minimumDegree - 1
	medianKeyToPromote := fullChild.storedKeys[medianKeyIndex]

	// Move keys after the median into the new sibling.
	newRightSibling.storedKeys = append(newRightSibling.storedKeys, fullChild.storedKeys[medianKeyIndex+1:]...)

	// Retain only keys before the median in the original child.
	fullChild.storedKeys = fullChild.storedKeys[:medianKeyIndex]

	// If the child is internal, split its child pointers as well.
	if !fullChild.isLeafNode {
		newRightSibling.childPointers = append(newRightSibling.childPointers, fullChild.childPointers[minimumDegree:]...)
		fullChild.childPointers = fullChild.childPointers[:minimumDegree]
	}

	// Insert the promoted key into the current node.
	node.storedKeys = append(node.storedKeys, 0)
	copy(node.storedKeys[index+1:], node.storedKeys[index:])
	node.storedKeys[index] = medianKeyToPromote

	// Insert the new sibling pointer immediately after the split child.
	node.childPointers = append(node.childPointers, nil)
	copy(node.childPointers[index+2:], node.childPointers[index+1:])
	node.childPointers[index+1] = newRightSibling
}

// insertNonFull inserts a key into a node that is guaranteed
// not to be full at the time of invocation.
func (node *BTreeNode) insertNonFull(key int) {
	// Guard: invalid receiver.
	if node == nil {
		return
	}

	// Start from the rightmost key.
	index := len(node.storedKeys) - 1

	// Case 1: Leaf insertion.
	if node.isLeafNode {
		// Make room for the new key.
		node.storedKeys = append(node.storedKeys, 0)

		// Shift keys rightward to preserve sorted order.
		for index >= 0 && key < node.storedKeys[index] {
			node.storedKeys[index+1] = node.storedKeys[index]
			index--
		}

		// Insert key at its sorted position.
		node.storedKeys[index+1] = key

		return
	}

	// Case 2: Internal node — find child to descend into.
	for index >= 0 && key < node.storedKeys[index] {
		index--
	}

	targetIndex := index + 1

	// Bounds and existence checks.
	if targetIndex < 0 || targetIndex >= len(node.childPointers) {
		return
	}

	if node.childPointers[targetIndex] == nil {
		return
	}

	// If the child is full, split it before descending.
	if len(node.childPointers[targetIndex].storedKeys) == (2*node.minimumDegree - 1) {
		node.splitChildAtIndex(targetIndex)

		// After split, determine which child to descend into.
		if key > node.storedKeys[targetIndex] {
			targetIndex++
		}
	}

	// Recurse into the appropriate child.
	if targetIndex < len(node.childPointers) && node.childPointers[targetIndex] != nil {
		node.childPointers[targetIndex].insertNonFull(key)
	}
}

// Insert inserts a key into the B-Tree, splitting the root
// if necessary to maintain B-Tree invariants.
func (tree *BTree) Insert(key int) {
	// Guard: invalid receiver.
	if tree == nil {
		return
	}

	// If the tree is empty, initialize the root.
	if tree.root == nil {
		tree.root = &BTreeNode{
			minimumDegree: tree.minimumDegree,
			isLeafNode:    true,
			storedKeys:    []int{key},
			childPointers: make([]*BTreeNode, 0, 2*tree.minimumDegree),
		}

		return
	}

	// If the root is full, split it and grow the tree height.
	if len(tree.root.storedKeys) == (2*tree.minimumDegree - 1) {
		previous := tree.root

		newRoot := &BTreeNode{
			minimumDegree: tree.minimumDegree,
			isLeafNode:    false,
			storedKeys:    make([]int, 0, 2*tree.minimumDegree-1),
			childPointers: make([]*BTreeNode, 0, 2*tree.minimumDegree),
		}

		// Old root becomes child of new root.
		newRoot.childPointers = append(newRoot.childPointers, previous)
		newRoot.splitChildAtIndex(0)

		tree.root = newRoot
	}

	// Insert into a guaranteed non-full root.
	tree.root.insertNonFull(key)
}

// getPredecessor returns the largest key in the subtree rooted at this node.
// The predecessor is obtained by following rightmost child pointers until
// a leaf node is reached.
func (node *BTreeNode) getPredecessor() int {
	// Defensive guard.
	if node == nil || len(node.storedKeys) == 0 {
		return 0
	}

	current := node

	// Follow rightmost children until reaching a leaf.
	for !current.isLeafNode {
		if len(current.childPointers) == 0 {
			break
		}

		current = current.childPointers[len(current.childPointers)-1]

		if current == nil {
			return 0
		}
	}

	// The predecessor is the largest key in that leaf.
	return current.storedKeys[len(current.storedKeys)-1]
}

// getSuccessor returns the smallest key in the subtree rooted at this node.
// The successor is obtained by following leftmost child pointers until
// a leaf node is reached.
func (node *BTreeNode) getSuccessor() int {
	// Defensive guard.
	if node == nil || len(node.storedKeys) == 0 {
		return 0
	}

	current := node

	// Follow leftmost children until reaching a leaf.
	for !current.isLeafNode {
		if len(current.childPointers) == 0 {
			break
		}

		current = current.childPointers[0]

		if current == nil {
			return 0
		}
	}

	// The successor is the smallest key in that leaf.
	return current.storedKeys[0]
}

// searchWithinNode searches for a key within the subtree rooted at this node.
// The function returns the index at which the key was found and a boolean
// indicating whether the key exists in the subtree.
func (node *BTreeNode) searchWithinNode(key int) (int, bool) {
	// Defensive guard.
	if node == nil {
		return 0, false
	}

	index := 0

	lengthOfKeys := len(node.storedKeys)

	// Locate the first key ≥ target.
	for index < lengthOfKeys && key > node.storedKeys[index] {
		index++
	}

	// If found in this node, return success.
	if index < lengthOfKeys && node.storedKeys[index] == key {
		return index, true
	}

	// If leaf, key does not exist.
	if node.isLeafNode {
		return index, false
	}

	// Otherwise descend into the appropriate child.
	if index >= len(node.childPointers) || node.childPointers[index] == nil {
		return index, false
	}

	return node.childPointers[index].searchWithinNode(key)
}

// Search determines whether a given key exists within the B-Tree.
// The function returns true if the key is present and false otherwise.
func (tree *BTree) Search(key int) bool {
	// Defensive guard.
	if tree == nil || tree.root == nil {
		return false
	}

	_, found := tree.root.searchWithinNode(key)

	return found
}

// findFirstKeyGreaterOrEqual returns the smallest index i such that
// storedKeys[i] >= key. If all keys in the node are smaller than key,
// the returned index equals len(storedKeys).
func (node *BTreeNode) findFirstKeyGreaterOrEqual(key int) int {
	// Defensive guard.
	if node == nil {
		return 0
	}

	// Start scanning keys from the leftmost position.
	index := 0

	// Advance until we find the first key that is
	// greater than or equal to the target key.
	// This preserves sorted order semantics.
	for index < len(node.storedKeys) && node.storedKeys[index] < key {
		index++
	}

	// Return the insertion/search position.
	//
	// Possible meanings of the returned index:
	//   - index < len(storedKeys): candidate key position
	//   - index == len(storedKeys): descend into rightmost child
	return index
}

// borrowFromLeftSibling restores the minimum key invariant for the child
// at the given index by borrowing a key from its immediate left sibling.
// The separating key from the parent is moved down into the child, and
// the sibling's largest key is promoted into the parent.
func (node *BTreeNode) borrowFromLeftSibling(index int) {
	// Defensive guard.
	if node == nil {
		return
	}

	// The child must have a left sibling to borrow from.
	// index must be > 0, and within the bounds of child pointers.
	if index <= 0 || index >= len(node.childPointers) {
		return
	}

	// Identify the child that is underfull and its left sibling.
	child := node.childPointers[index]
	leftSibling := node.childPointers[index-1]

	// Guard: both nodes must exist.
	if child == nil || leftSibling == nil {
		return
	}

	// Left sibling must have at least one key to lend.
	// (In practice, callers ensure it has ≥ minimumDegree keys.)
	if len(leftSibling.storedKeys) == 0 {
		return
	}

	// Step 1:
	// Move the separating key from the parent down into the child.
	// This key becomes the new leftmost key of the child.
	child.storedKeys = append([]int{node.storedKeys[index-1]}, child.storedKeys...)

	// Step 2:
	// Promote the largest key from the left sibling up into the parent,
	// replacing the separating key that was just moved down.
	node.storedKeys[index-1] = leftSibling.storedKeys[len(leftSibling.storedKeys)-1]

	// Remove the promoted key from the left sibling.
	leftSibling.storedKeys = leftSibling.storedKeys[:len(leftSibling.storedKeys)-1]

	// Step 3 (internal nodes only):
	// If the sibling is not a leaf, its rightmost child pointer
	// must be transferred to become the new leftmost child of `child`.
	if !leftSibling.isLeafNode {
		child.childPointers = append([]*BTreeNode{
			leftSibling.childPointers[len(leftSibling.childPointers)-1],
		}, child.childPointers...)

		// Remove the transferred child pointer from the sibling.
		leftSibling.childPointers = leftSibling.childPointers[:len(leftSibling.childPointers)-1]
	}
}

// borrowFromRightSibling restores the minimum key invariant for the child
// at the given index by borrowing a key from its immediate right sibling.
// The separating key from the parent is moved down into the child, and
// the sibling's smallest key is promoted into the parent.
func (node *BTreeNode) borrowFromRightSibling(index int) {
	// Defensive guard.
	if node == nil {
		return
	}

	// The child must have a right sibling to borrow from.
	// index must be within bounds, and index+1 must exist.
	if index < 0 || index+1 >= len(node.childPointers) {
		return
	}

	// Identify the underfull child and its immediate right sibling.
	child := node.childPointers[index]
	rightSibling := node.childPointers[index+1]

	// Guard: both nodes must exist.
	if child == nil || rightSibling == nil {
		return
	}

	// Right sibling must have at least one key to lend.
	// (Callers ensure it has ≥ minimumDegree keys.)
	if len(rightSibling.storedKeys) == 0 {
		return
	}

	// Step 1:
	// Move the separating key from the parent down into the child.
	// This key becomes the new rightmost key of the child.
	child.storedKeys = append(child.storedKeys, node.storedKeys[index])

	// Step 2:
	// Promote the smallest key from the right sibling up into the parent,
	// replacing the separating key that was just moved down.
	node.storedKeys[index] = rightSibling.storedKeys[0]

	// Remove the promoted key from the right sibling.
	rightSibling.storedKeys = rightSibling.storedKeys[1:]

	// Step 3 (internal nodes only):
	// If the sibling is not a leaf, its leftmost child pointer
	// must be transferred to become the new rightmost child of `child`.
	if !rightSibling.isLeafNode {
		child.childPointers = append(child.childPointers, rightSibling.childPointers[0])

		// Remove the transferred child pointer from the sibling.
		rightSibling.childPointers = rightSibling.childPointers[1:]
	}
}

// ensureChildHasEnoughKeys guarantees that the child at the given index
// contains at least the minimum number of keys before descent during
// deletion. The function may borrow from a sibling or merge children
// and returns the index of the child node that should be descended into
// after any structural modification.
func (node *BTreeNode) ensureChildHasEnoughKeys(index int) int {
	// Defensive guard.
	if node == nil {
		return index
	}

	// Index must refer to a valid child pointer.
	if index < 0 || index >= len(node.childPointers) {
		return index
	}

	// Case 1: Borrow from the left sibling.
	// If a left sibling exists and has at least minimumDegree keys,
	// rotate a key from the sibling through the parent into the child.
	if index > 0 && len(node.childPointers[index-1].storedKeys) >= node.minimumDegree {
		node.borrowFromLeftSibling(index)

		// Child index remains unchanged after borrowing.
		return index
	}

	// Compute the index of the rightmost child once for reuse.
	lastChildIndex := len(node.childPointers) - 1

	// Case 2: Borrow from the right sibling.
	// If a right sibling exists and has at least minimumDegree keys,
	// rotate a key from the sibling through the parent into the child.
	if index < lastChildIndex && len(node.childPointers[index+1].storedKeys) >= node.minimumDegree {
		node.borrowFromRightSibling(index)

		// Child index remains unchanged after borrowing.
		return index
	}

	// Case 3: Merge with the right sibling.
	// Borrowing is impossible, so merge the child with its right sibling.
	// The separating key from the parent is pulled down.
	if index < lastChildIndex {
		node.mergeChildrenAtIndex(index)

		// After merge, the merged node occupies the original index.
		return index
	}

	// Case 4: Merge with the left sibling.
	// This occurs only when the child is the rightmost child.
	// Merge the child into its left sibling.
	node.mergeChildrenAtIndex(index - 1)

	// After merging with the left sibling, the valid descent index shifts left.
	return index - 1
}

// mergeChildrenAtIndex merges the child at index and its immediate right
// sibling into a single node, pulling down the separating key from the
// current node. This operation reduces the number of children by one
// and is used during deletion when borrowing is not possible.
func (node *BTreeNode) mergeChildrenAtIndex(index int) {
	// Defensive guard.
	if node == nil {
		return
	}

	// Guard: ensure index and index+1 refer to valid sibling children.
	// We are merging child[index] and child[index+1].
	if index < 0 || index+1 >= len(node.childPointers) {
		return
	}

	// Fetch the two children that will be merged.
	leftChild := node.childPointers[index]
	rightChild := node.childPointers[index+1]

	// Guard: both children must exist.
	if leftChild == nil || rightChild == nil {
		return
	}

	// Step 1: Pull the separator key down from the parent into the left child.
	// This key logically belongs between the two child key ranges.
	leftChild.storedKeys = append(leftChild.storedKeys, node.storedKeys[index])

	// Step 2: Append all keys from the right child into the left child.
	// After this, the left child contains:
	//   (t−1) keys + 1 separator + (t−1) keys = (2t−1) keys.
	leftChild.storedKeys = append(leftChild.storedKeys, rightChild.storedKeys...)

	// Step 3: If these are internal nodes, move all child pointers as well.
	// The merged node must preserve key-to-child alignment.
	if !leftChild.isLeafNode {
		leftChild.childPointers = append(leftChild.childPointers, rightChild.childPointers...)
	}

	// Step 4: Remove the separator key from the parent node.
	// Shift keys left to close the gap.
	copy(node.storedKeys[index:], node.storedKeys[index+1:])
	node.storedKeys = node.storedKeys[:len(node.storedKeys)-1]

	// Step 5: Remove the right child pointer from the parent.
	// Shift remaining child pointers left to maintain alignment.
	copy(node.childPointers[index+1:], node.childPointers[index+2:])
	node.childPointers = node.childPointers[:len(node.childPointers)-1]
}

// removeKeyFromLeaf removes the key at the specified index from a leaf node.
// This operation does not require further rebalancing.
func (node *BTreeNode) removeKeyFromLeaf(index int) {
	// Defensive guard.
	if node == nil {
		return
	}

	// This should never happen unless invariants were violated.
	if index < 0 || index >= len(node.storedKeys) {
		panic("btree invariant violated")
	}

	// Remove the key by shifting left.
	copy(node.storedKeys[index:], node.storedKeys[index+1:])
	node.storedKeys = node.storedKeys[:len(node.storedKeys)-1]
}

// removeKeyFromInternalNode removes the key at the specified index from
// an internal node. The key is replaced by either its predecessor or
// successor when possible, or by merging child nodes when required.
func (node *BTreeNode) removeKeyFromInternalNode(index int) {
	// Defensive guard.
	if node == nil {
		return
	}

	// This condition should never be violated in a correct B-Tree.
	// If it happens, higher-level logic has already failed.
	if index < 0 || index >= len(node.storedKeys) {
		panic("btree invariant violated")
	}

	// An internal node must have a right child for every key.
	if index+1 >= len(node.childPointers) {
		return
	}

	// Store the key to be removed from this internal node.
	key := node.storedKeys[index]

	// Identify the two children that bracket the key:
	//   leftChild  contains keys < key
	//   rightChild contains keys > key
	leftChild := node.childPointers[index]
	rightChild := node.childPointers[index+1]

	// Case 1 (CLRS): The left child has at least t keys.
	// Replace the key with its predecessor and delete recursively.
	if len(leftChild.storedKeys) >= node.minimumDegree {
		// Find the largest key in the left subtree.
		predecessor := leftChild.getPredecessor()

		// Replace the target key with its predecessor.
		node.storedKeys[index] = predecessor

		// Recursively remove the predecessor from the left subtree.
		leftChild.removeFromNode(predecessor)

		return
	}

	// Case 2 (CLRS): The right child has at least t keys.
	// Replace the key with its successor and delete recursively.
	if len(rightChild.storedKeys) >= node.minimumDegree {
		// Find the smallest key in the right subtree.
		successor := rightChild.getSuccessor()

		// Replace the target key with its successor.
		node.storedKeys[index] = successor

		// Recursively remove the successor from the right subtree.
		rightChild.removeFromNode(successor)

		return
	}

	// Case 3 (CLRS): Both children have only t−1 keys.
	// Merge the key and the right child into the left child.
	node.mergeChildrenAtIndex(index)

	// After merging:
	// - The original key now resides in the merged child.
	// - The parent has one fewer key and child.
	// Continue deletion in the merged subtree.
	if index < len(node.childPointers) && node.childPointers[index] != nil {
		node.childPointers[index].removeFromNode(key)
	}
}

// removeFromNode removes a key from the subtree rooted at this node.
// The function ensures that all B-Tree invariants are preserved by
// performing borrowing or merging operations as needed before descent.
func (node *BTreeNode) removeFromNode(key int) {
	// Defensive guard.
	if node == nil {
		return
	}

	// Locate the first key in this node that is >= the target key.
	// This determines whether the key exists here or which child to descend into.
	index := node.findFirstKeyGreaterOrEqual(key)

	// Case 1: The key is present in this node.
	if index < len(node.storedKeys) && node.storedKeys[index] == key {
		// Subcase 1a: Leaf node — safe to delete directly.
		if node.isLeafNode {
			node.removeKeyFromLeaf(index)
		} else {
			// Subcase 1b: Internal node — delegate to internal deletion logic,
			// which replaces the key with a predecessor/successor or merges children.
			node.removeKeyFromInternalNode(index)
		}

		return
	}

	// Case 2: Key is not in this node.
	// If this is a leaf, the key does not exist in the tree.
	if node.isLeafNode {
		return
	}

	// Guard: ensure the child index is valid before descending.
	if index < 0 || index >= len(node.childPointers) || node.childPointers[index] == nil {
		return
	}

	// Case 3: Descending into a child.
	// Before descent, ensure the target child has at least t keys.
	// This guarantees that deletion will not cause an underflow later.
	if len(node.childPointers[index].storedKeys) < node.minimumDegree {
		// This may borrow from a sibling or merge children.
		// The returned index indicates the correct child to descend into
		// after structural modifications.
		index = node.ensureChildHasEnoughKeys(index)
	}

	// Final descent into the appropriate child subtree.
	if index < len(node.childPointers) && node.childPointers[index] != nil {
		node.childPointers[index].removeFromNode(key)
	}
}

// Remove deletes a key from the B-Tree if it exists.
// After deletion, the function ensures that the tree height is reduced
// when the root becomes empty and has a single child.
func (tree *BTree) Remove(key int) {
	// Defensive guard.
	if tree == nil || tree.root == nil {
		return
	}

	// Perform top-down deletion starting at the root.
	// All rebalancing (borrowing/merging) happens during descent,
	// so no fix-up is required on the way back up.
	tree.root.removeFromNode(key)

	// After deletion, the root may have become empty.
	// This can only happen if the root was an internal node
	// and its last key was merged downward.
	if len(tree.root.storedKeys) == 0 && !tree.root.isLeafNode {
		// Shrink the tree height by promoting the sole remaining child
		// as the new root. This is the only case where the tree height
		// is allowed to decrease.
		if len(tree.root.childPointers) > 0 {
			tree.root = tree.root.childPointers[0]
		}
	}
}

// printInOrderRecursive performs an in-order traversal of the subtree
// rooted at this node, printing keys in ascending order.
func (node *BTreeNode) printInOrderRecursive() {
	// Defensive guard.
	if node == nil {
		return
	}

	// Cache the number of keys in this node.
	// A B-Tree node with k keys has either 0 children (leaf)
	// or k+1 children (internal node).
	lengthOfKeys := len(node.storedKeys)

	// Iterate through each key in ascending order.
	for index := 0; index < lengthOfKeys; index++ {
		// For internal nodes, first traverse the left child
		// associated with the current key.
		if !node.isLeafNode {
			node.childPointers[index].printInOrderRecursive()
		}

		// Visit (print) the current key after its left subtree.
		fmt.Printf("%d ", node.storedKeys[index])
	}

	// After processing all keys, traverse the rightmost child.
	// This child corresponds to values greater than the last key.
	if !node.isLeafNode && len(node.childPointers) > len(node.storedKeys) {
		node.childPointers[lengthOfKeys].printInOrderRecursive()
	}
}

// PrintInOrder prints all keys stored in the B-Tree in sorted order
// using an in-order traversal.
func (tree *BTree) PrintInOrder() {
	// Defensive guard.
	if tree == nil || tree.root == nil {
		fmt.Println("(empty)")

		return
	}

	// Perform an in-order traversal starting from the root.
	// This prints all keys in strictly ascending order.
	tree.root.printInOrderRecursive()
	fmt.Println()
}
