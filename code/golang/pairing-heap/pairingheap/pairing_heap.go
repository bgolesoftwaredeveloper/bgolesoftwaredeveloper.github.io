// -----------------------------------------------------------------------------
// Package: pairingheap
// File: pairing_heap.go
//
// Description:
//
//	This file contains a reference implementation of a generic pairing heap
//	written in Go. A pairing heap is a self-adjusting heap data structure that
//	supports efficient meld (merge), insert, and decrease-key operations.
//
//	Unlike binary heaps, a pairing heap is represented as a multi-way tree.
//	The heap invariant is maintained by linking trees only when required,
//	deferring structural work until extract-min is performed.
//
//	This implementation prioritizes:
//
//	  - Readability and explicitness.
//	  - Defensive programming and invariant preservation.
//	  - Long, meaningful identifier names.
//	  - Clear separation of structural responsibilities.
//
//	The pairing heap is parameterized over a generic value type and uses a
//	user-supplied comparator function to define ordering.
//
// Design Notes:
//   - The heap is a min-heap.
//   - Each node maintains pointers to its leftmost child and next sibling.
//   - Parent pointers are maintained to support decrease-key.
//   - ExtractMin performs a two-pass pairing strategy.
//   - The zero value of PairingHeap is not valid; use the constructor.
//
// Intended Use:
//
//	This package is intended for educational purposes, experimentation,
//	and as a foundation for more advanced heap-based algorithms such as
//	Dijkstra, Prim, and event simulation systems.
//
// -----------------------------------------------------------------------------
package pairingheap

// comparisonFunction defines a strict weak ordering over values.
// It must return true if left is strictly less than right.
type comparisonFunction[Value any] func(left, right Value) bool

// pairingHeapNode represents a single node within the pairing heap.
// Nodes are linked together in a left-child / right-sibling structure.
type pairingHeapNode[Value any] struct {
	value Value

	parent *pairingHeapNode[Value]
	left   *pairingHeapNode[Value]
	next   *pairingHeapNode[Value]
}

// PairingHeap represents a min-oriented pairing heap.
type PairingHeap[Value any] struct {
	root       *pairingHeapNode[Value]
	count      int
	comparator comparisonFunction[Value]
}

// NewPairingHeap constructs a new empty pairing heap.
// A non-nil comparator must be provided.
func NewPairingHeap[Value any](comparator comparisonFunction[Value]) *PairingHeap[Value] {
	// Reject a nil comparator to avoid undefined ordering.
	if comparator == nil {
		panic("pairing heap required a non-nil comparator")
	}

	// Allocate and return the initialized heap.
	return &PairingHeap[Value]{
		root:       nil,
		count:      0,
		comparator: comparator,
	}
}

// ensureInitialized validates that the heap receiver is usable.
func (heap *PairingHeap[Value]) ensureInitialized() {
	// Reject nil receivers or heaps missing a comparator.
	if heap == nil || heap.comparator == nil {
		panic("pairing heap is nil or uninitialized")
	}
}

// meldTwoNodes combines two heap roots into a single heap.
// The smaller root becomes the parent of the larger.
func (heap *PairingHeap[Value]) meldTwoNodes(source, target *pairingHeapNode[Value]) *pairingHeapNode[Value] {
	// If either node is nil, return the other directly.
	if source == nil {
		return target
	}

	if target == nil {
		return source
	}

	// Ensure the source node contains the smaller value.
	if heap.comparator(target.value, source.value) {
		source, target = target, source
	}

	// Attach the target node as the leftmost child of the source.
	target.parent = source
	target.next = source.left

	source.left = target

	// Return the new root of the melded heap.
	return source
}

// mergeSiblingLists performs the two-pass pairing strategy used by ExtractMinimum.
// It merges a list of sibling subtrees into a single heap.
func (heap *PairingHeap[Value]) mergeSiblingLists(sibling *pairingHeapNode[Value]) *pairingHeapNode[Value] {
	// A zero- or one-node list is already a valid heap.
	if sibling == nil || sibling.next == nil {
		return sibling
	}

	// Store intermediate heaps formed during the first pass.
	var pairedNodes []*pairingHeapNode[Value]

	// Iterate over siblings from left to right.
	current := sibling

	for current != nil {
		// Select the first node in the pair.
		currentSibling := current
		nextSibling := current.next

		// Advance the cursor before breaking links.
		if nextSibling != nil {
			current = nextSibling.next
		} else {
			current = nil
		}

		// Isolate the first node from the sibling list.
		currentSibling.next = nil

		if nextSibling != nil {
			// Isolate the second node and meld the pair.
			nextSibling.next = nil
			pairedNodes = append(pairedNodes, heap.meldTwoNodes(currentSibling, nextSibling))
		} else {
			// Handle an odd leftover node.
			pairedNodes = append(pairedNodes, currentSibling)
		}
	}

	lengthOfPairedNodes := len(pairedNodes)

	// Second pass: merge the paired heaps from right to left.
	result := pairedNodes[lengthOfPairedNodes-1]

	for index := lengthOfPairedNodes - 2; index >= 0; index-- {
		result = heap.meldTwoNodes(result, pairedNodes[index])
	}

	// Return the fully merged heap.
	return result
}

// detachNodeFromParent removes a node from its parent's child list.
func (heap *PairingHeap[Value]) detachNodeFromParent(node *pairingHeapNode[Value]) {
	// Retrieve the parent reference.
	parent := node.parent

	// If the node has no parent, nothing needs to be done.
	if parent == nil {
		return
	}

	// If the node is the leftmost child, update the parent's pointer.
	if parent.left == node {
		parent.left = node.next
	} else {
		// Otherwise, scan siblings to find the node.
		currentSibling := parent.left

		for currentSibling != nil && currentSibling.next != node {
			currentSibling = currentSibling.next
		}

		// Bypass the node in the sibling list.
		if currentSibling != nil {
			currentSibling.next = node.next
		}
	}

	// Fully detach the node from the tree.
	node.parent = nil
	node.next = nil
}

// IsEmpty reports whether the heap contains zero elements.
func (heap *PairingHeap[Value]) IsEmpty() bool {
	// Ensure the heap is valid before inspection.
	heap.ensureInitialized()

	// An empty heap has a zero element count.
	return heap.count == 0
}

// Size returns the number of elements currently stored in the heap.
func (heap *PairingHeap[Value]) Size() int {
	// Ensure the heap is valid before inspection.
	heap.ensureInitialized()

	// Return the tracked element count.
	return heap.count
}

func (heap *PairingHeap[Value]) Insert(value Value) *pairingHeapNode[Value] {
	// Ensure the heap is initialized.
	heap.ensureInitialized()

	// Allocate a standalone node.
	newNode := &pairingHeapNode[Value]{
		value: value,
	}

	// Meld the new node with the existing root.
	heap.root = heap.meldTwoNodes(heap.root, newNode)
	heap.count++

	// Return the node handle for optional decrease-key usage.
	return newNode
}

// FindMinimum returns the smallest value in the heap without removing it.
func (heap *PairingHeap[Value]) FindMinimum() (Value, bool) {
	// Ensure the heap is initialized.
	heap.ensureInitialized()

	// An empty heap has no minimum.
	if heap.root == nil {
		var zero Value

		return zero, false
	}

	// The root always contains the minimum value.
	return heap.root.value, true
}

// ExtractMinimum removes and returns the smallest value in the heap.
func (heap *PairingHeap[Value]) ExtractMinimum() (Value, bool) {
	// Ensure the heap is initialized.
	heap.ensureInitialized()

	// Reject extraction from an empty heap.
	if heap.root == nil {
		var zero Value

		return zero, false
	}

	// Save the minimum value for return.
	minimumValue := heap.root.value

	// Merge the root's children using the two-pass strategy.
	newRoot := heap.mergeSiblingLists(heap.root.left)

	// Clear the parent pointer on the new root.
	if newRoot != nil {
		newRoot.parent = nil
	}

	// Replace the root and decrement the element count.
	heap.root = newRoot
	heap.count--

	return minimumValue, true
}

// DecreaseKey reduces the value stored in a node and restores heap order.
func (heap *PairingHeap[Value]) DecreaseKey(node *pairingHeapNode[Value], value Value) {
	// Ensure the heap is initialized.
	heap.ensureInitialized()

	// Reject invalid node usage.
	if node == nil {
		panic("cannod decrease key on nil node")
	}

	// Enforce the strict decrease-key invariant.
	if !heap.comparator(value, node.value) {
		panic("new value does not decrease key")
	}

	// Update the node's value.
	node.value = value

	// If the node is already the root, no restructuring is needed.
	if node.parent == nil {
		return
	}

	// Cut the node from its current position.
	heap.detachNodeFromParent(node)

	// Meld the node back into the heap at the root.
	heap.root = heap.meldTwoNodes(heap.root, node)
}
