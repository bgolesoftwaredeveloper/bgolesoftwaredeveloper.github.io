// -----------------------------------------------------------------------------
// Package: eytzingerlevelorder
// File: binary_heap_eytzinger.go
//
// Description:
//
//	This file contains an explicit implementation of a binary min-heap stored
//	using Eytzinger (level-order) layout. The heap is represented as a contiguous
//	slice, where parent–child relationships are derived arithmetically rather
//	than through pointer structures.
//
//	The implementation prioritizes correctness, clarity, and defensive
//	programming over micro-optimizations. All public operations preserve the
//	min-heap invariant and are safe to call on a nil receiver.
//
//	The heap supports dynamic insertion, removal of the minimum element,
//	in-place heap construction from an existing slice, and validation of
//	internal invariants for debugging and testing purposes.
//
// Design Notes:
//   - The heap is a min-heap (the smallest element is stored at the root).
//   - Elements are stored in level-order (Eytzinger layout).
//   - Parent and child relationships are computed using index arithmetic.
//   - The underlying slice grows dynamically as elements are inserted.
//   - All public methods guard against nil receivers.
//   - Internal helpers defensively validate indices before use.
//   - The zero value of BinaryHeapEytzingerLayout represents a valid empty heap.
//
// Intended Use:
//
//	This package is intended for educational purposes, algorithmic
//	implementations (e.g., Dijkstra’s or Prim’s algorithms), and as a reference
//	implementation of an array-backed binary heap in Go.
//
// -----------------------------------------------------------------------------
package eytzingerlevelorder

import "errors"

// BinaryHeapEytzingerLayout represents a binary min-heap stored in
// Eytzinger (level-order) layout.
// The heap invariant guarantees that each parent node is less than or equal
// to its children. The smallest element is always stored at index 0.
type BinaryHeapEytzingerLayout struct {
	elements []int
}

// leftChildIndex computes the index of the left child for a given parent index.
// If the computed child index is out of bounds or the input index is invalid,
// the second return value is false.
func leftChildIndex(index, numberOfElements int) (int, bool) {
	// Reject invalid parent indices.
	if index < 0 {
		return 0, false
	}

	// Compute the left child index using Eytzinger layout arithmetic.
	childIndex := 2*index + 1

	// Ensure the computed index falls within the heap bounds.
	if childIndex >= numberOfElements {
		return 0, false
	}

	return childIndex, true
}

// rightChildIndex computes the index of the right child for a given parent index.
// If the computed child index is out of bounds or the input index is invalid,
// the second return value is false.
func rightChildIndex(index, numberOfElements int) (int, bool) {
	// Reject invalid parent indices.
	if index < 0 {
		return 0, false
	}

	// Compute the right child index using Eytzinger layout arithmetic.
	childIndex := 2*index + 2

	// Ensure the computed index falls within the heap bounds.
	if childIndex >= numberOfElements {
		return 0, false
	}

	return childIndex, true
}

// parentIndex computes the index of the parent for a given child index.
// If the provided index refers to the root or is invalid, the second return
// value is false.
func parentIndex(index int) (int, bool) {
	// The root node (index 0) has no parent.
	if index <= 0 {
		return 0, false
	}

	// Compute the parent index using integer division.
	return (index - 1) / 2, true
}

// siftUpFromIndex restores the heap invariant by moving the element at the
// specified index upward until it is correctly positioned.
// This method performs no action if the receiver is nil or the index is invalid.
func (heap *BinaryHeapEytzingerLayout) siftUpFromIndex(index int) {
	// Guard against nil receivers.
	if heap == nil {
		return
	}

	// Ensure the starting index is within bounds.
	if index < 0 || index >= len(heap.elements) {
		return
	}

	currentIndex := index

	// Continue bubbling upward until the root is reached.
	for currentIndex > 0 {
		// Compute the parent index of the current node.
		currentParentIndex, ok := parentIndex(currentIndex)

		if !ok {
			return
		}

		// If the heap invariant is satisfied, stop.
		if heap.elements[currentIndex] >= heap.elements[currentParentIndex] {
			return
		}

		// Swap the current element with its parent.
		heap.elements[currentIndex], heap.elements[currentParentIndex] =
			heap.elements[currentParentIndex], heap.elements[currentIndex]

		// Move upward to continue validation.
		currentIndex = currentParentIndex
	}
}

// siftDownFromIndex restores the heap invariant by moving the element at the
// specified index downward until it is correctly positioned.
// This method performs no action if the receiver is nil or the index is invalid.
func (heap *BinaryHeapEytzingerLayout) siftDownFromIndex(index int) {
	// Guard against nil receivers.
	if heap == nil {
		return
	}

	numberOfElements := len(heap.elements)

	// Ensure the starting index is within bounds.
	if index < 0 || index >= numberOfElements {
		return
	}

	currentIndex := index

	for {
		// Determine whether a left child exists.
		currentLeftChild, leftExists := leftChildIndex(currentIndex, numberOfElements)

		// If no left child exists, this node is a leaf.
		if !leftExists {
			return
		}

		// Assume the left child is the smaller child initially.
		indexOfSmallerChild := currentLeftChild

		// Check if a right child exists.
		currentRightChild, rightExist := rightChildIndex(currentIndex, numberOfElements)

		// If both children exist, select the smaller one.
		if rightExist &&
			heap.elements[currentRightChild] < heap.elements[currentLeftChild] {
			indexOfSmallerChild = currentRightChild
		}

		// If the heap invariant is satisfied, stop.
		if heap.elements[currentIndex] <= heap.elements[indexOfSmallerChild] {
			return
		}

		// Swap the current element with the smaller child.
		heap.elements[currentIndex], heap.elements[indexOfSmallerChild] =
			heap.elements[indexOfSmallerChild], heap.elements[currentIndex]

		// Continue sifting downward.
		currentIndex = indexOfSmallerChild
	}
}

// heapifyInPlace transforms the current element slice into a valid min-heap
// using a bottom-up heap construction algorithm.
// This operation runs in linear time and performs no action on nil receivers.
func (heap *BinaryHeapEytzingerLayout) heapifyInPlace() {
	// Guard against nil receivers.
	if heap == nil {
		return
	}

	numberOfElements := len(heap.elements)

	// Heaps with zero or one element are already valid.
	if numberOfElements <= 1 {
		return
	}

	// Identify the last non-leaf node.
	lastParentIndex := (numberOfElements - 2) / 2

	// Restore the heap invariant from the bottom up.
	for currentParentindex := lastParentIndex; currentParentindex >= 0; currentParentindex-- {
		heap.siftDownFromIndex(currentParentindex)
	}
}

// IsEmpty reports whether the heap contains no elements.
// This method is safe to call on a nil receiver.
// This operation runs in constant time.
func (heap *BinaryHeapEytzingerLayout) IsEmpty() bool {
	// Guard against nil receivers.
	if heap == nil {
		return true
	}

	return len(heap.elements) == 0
}

// Size returns the number of elements currently stored in the heap.
// This method is safe to call on a nil receiver.
// This operation runs in constant time.
func (heap *BinaryHeapEytzingerLayout) Size() int {
	// Guard against nil receivers.
	if heap == nil {
		return 0
	}

	return len(heap.elements)
}

// IsValidMinimumHeap verifies that the heap satisfies the min-heap invariant
// for all stored elements.
// This method is intended for debugging and testing.
// This operation runs in linear time.
func (heap *BinaryHeapEytzingerLayout) IsValidMinimumHeap() bool {
	// Guard against nil receivers.
	if heap == nil {
		return true
	}

	numberOfElements := len(heap.elements)

	// Validate each node against its children.
	for currentIndex := 0; currentIndex < numberOfElements; currentIndex++ {
		currentLeftChildIndex, leftChildExists := leftChildIndex(currentIndex, numberOfElements)

		if leftChildExists &&
			heap.elements[currentIndex] > heap.elements[currentLeftChildIndex] {
			return false
		}

		currentRightChildIndex, rightChildExists := rightChildIndex(currentIndex, numberOfElements)

		if rightChildExists &&
			heap.elements[currentIndex] > heap.elements[currentRightChildIndex] {
			return false
		}
	}

	return true
}

// NewBinaryHeapEyzingerLayout creates and returns a new empty binary heap.
// The returned heap is initialized to a valid zero state.
func NewBinaryHeapEyzingerLayout() *BinaryHeapEytzingerLayout {
	return &BinaryHeapEytzingerLayout{
		elements: make([]int, 0),
	}
}

// Clone creates and returns a deep copy of the heap.
// Mutations to the returned heap do not affect the original.
// If the receiver is nil, nil is returned.
func (heap *BinaryHeapEytzingerLayout) Clone() *BinaryHeapEytzingerLayout {
	// Guard against nil receivers.
	if heap == nil {
		return nil
	}

	return &BinaryHeapEytzingerLayout{
		elements: append([]int(nil), heap.elements...),
	}
}

// BuildHeapFromSlice constructs a binary heap from the provided slice.
// The input slice is copied, and the heap is built in-place using heapify.
// If the input slice is nil, an empty heap is returned.
func BuildHeapFromSlice(items []int) *BinaryHeapEytzingerLayout {
	// Guard against nil receivers.
	if items == nil {
		return NewBinaryHeapEyzingerLayout()
	}

	// Copy the input slice to avoid mutating the caller’s data.
	heap := &BinaryHeapEytzingerLayout{
		elements: append([]int(nil), items...),
	}

	// Restore heap invariants in linear time.
	heap.heapifyInPlace()

	return heap
}

// Push inserts a new element into the heap and restores the heap invariant.
// This method is safe to call on a nil receiver.
// This operation runs in logarithmic time.
func (heap *BinaryHeapEytzingerLayout) Push(item int) {
	// Guard against nil receivers.
	if heap == nil {
		return
	}

	// Append the new element at the end of the heap.
	heap.elements = append(heap.elements, item)

	// Restore heap invariants by bubbling upward.
	lastInsertedIndex := len(heap.elements) - 1
	heap.siftUpFromIndex(lastInsertedIndex)
}

// Contains reports whether the specified value exists in the heap.
// This operation runs in linear time and does not modify the heap.
func (heap *BinaryHeapEytzingerLayout) Contains(target int) bool {
	// Guard against nil receivers.
	if heap == nil {
		return false
	}

	// Linearly scan the heap elements.
	for _, currentValue := range heap.elements {
		if currentValue == target {
			return true
		}
	}

	return false
}

// Pop removes and returns the minimum element from the heap.
// If the heap is empty or nil, an error is returned.
// This operation runs in logarithmic time.
func (heap *BinaryHeapEytzingerLayout) Pop() (int, error) {
	// Guard against nil receivers.
	if heap == nil {
		return 0, errors.New("heap is nil")
	}

	if heap.IsEmpty() {
		return 0, errors.New("cannot pop from an empty heap")
	}

	// Capture the root value to return.
	root := heap.elements[0]

	// Move the last element to the root position.
	lastElementIndex := len(heap.elements) - 1
	lastElementValue := heap.elements[lastElementIndex]

	heap.elements[0] = lastElementValue
	heap.elements = heap.elements[:lastElementIndex]

	// Restore heap invariants if elements remain.
	if !heap.IsEmpty() {
		heap.siftDownFromIndex(0)
	}

	return root, nil
}

// Clear removes all elements from the heap and resets it to an empty state.
// This method is safe to call on a nil receiver.
// This operation runs in constant time.
func (heap *BinaryHeapEytzingerLayout) Clear() {
	// Guard against nil receivers.
	if heap == nil {
		return
	}

	heap.elements = heap.elements[:0]
}
