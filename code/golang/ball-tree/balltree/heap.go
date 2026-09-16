// -----------------------------------------------------------------------------
// Package: balltree
// File: heap.go
//
// Description:
//
//		This file provides a bounded max-heap implementation specialized for
//		tracking nearest-neighbor candidates during Ball Tree search operations.
//
//		The heap maintains at most a fixed number of Neighbor entries, ordered
//		by descending distance. The largest (worst) distance is always stored
//		at the root, enabling efficient pruning during k-nearest neighbor (k-NN)
//		searches.
//
//		When the heap reaches capacity, new candidates are only accepted if they
//		improve upon the current worst distance. This ensures that the heap always
//		contains the best k neighbors encountered so far.
//
//		The implementation is intentionally defensive:
//	  - All methods are safe to call on a nil *maximumNeighborHeap receiver.
//	  - Invalid indices passed to internal helpers are ignored safely.
//	  - Zero or negative heap capacities are handled gracefully.
//	  - Read operations return safe zero values instead of panicking.
//
// Notes:
//   - This heap is not intended to be a general-purpose priority queue.
//   - Ordering is based solely on Neighbor.Distance.
//   - The heap invariant is maintained using sift-up and sift-down operations.
//   - Final results can be retrieved in ascending distance order.
//
// -----------------------------------------------------------------------------
package balltree

import "sort"

// maximumNeighborHeap represents a bounded max-heap of Neighbor values.
// The element with the largest distance is always stored at the root.
type maximumNeighborHeap struct {
	maximumSize int
	items       []Neighbor
}

// newMaximumNeighborHeap constructs and returns a heap with the specified
// maximum capacity. The heap initially contains no elements.
func newMaximumNeighborHeap(maximumSize int) *maximumNeighborHeap {
	// Clamp negative capacities to zero.
	if maximumSize < 0 {
		maximumSize = 0
	}

	return &maximumNeighborHeap{
		maximumSize: maximumSize,
		items:       make([]Neighbor, 0, maximumSize),
	}
}

// isFull reports whether the heap has reached its maximum capacity.
func (heap *maximumNeighborHeap) isFull() bool {
	// Treat a nil heap as not full.
	if heap == nil {
		return false
	}

	return len(heap.items) >= heap.maximumSize
}

// maximumDistance returns the largest distance currently stored in the heap.
// If the heap is empty or nil, zero is returned.
func (heap *maximumNeighborHeap) maximumDistance() float64 {
	// Return a safe zero value if no elements are present.
	if heap == nil || len(heap.items) == 0 {
		return 0
	}

	return heap.items[0].Distance
}

// siftUp restores the heap invariant by moving the element at the given index
// upward until it is no longer larger than its parent.
func (heap *maximumNeighborHeap) siftUp(childIndex int) {
	// Ensure the heap exists and the index is within bounds.
	if heap == nil || childIndex < 0 || childIndex >= len(heap.items) {
		return
	}

	for childIndex > 0 {
		parentIndex := (childIndex - 1) / 2

		if heap.items[childIndex].Distance <= heap.items[parentIndex].Distance {
			break
		}

		heap.items[childIndex], heap.items[parentIndex] = heap.items[parentIndex], heap.items[childIndex]

		childIndex = parentIndex
	}
}

// siftDown restores the heap invariant by moving the element at the given index
// downward until it is no longer smaller than either child.
func (heap *maximumNeighborHeap) siftDown(parentIndex int) {
	// Ensure the heap exists and the index is within bounds.
	if heap == nil || parentIndex < 0 || parentIndex >= len(heap.items) {
		return
	}

	numberOfItems := len(heap.items)

	for {
		leftChildIndex := 2*parentIndex + 1
		rightChildIndex := 2*parentIndex + 2
		largestIndex := parentIndex

		if leftChildIndex < numberOfItems && heap.items[leftChildIndex].Distance > heap.items[largestIndex].Distance {
			largestIndex = leftChildIndex
		}

		if rightChildIndex < numberOfItems && heap.items[rightChildIndex].Distance > heap.items[largestIndex].Distance {
			largestIndex = rightChildIndex
		}

		if largestIndex == parentIndex {
			return
		}

		heap.items[parentIndex], heap.items[largestIndex] = heap.items[largestIndex], heap.items[parentIndex]

		parentIndex = largestIndex
	}
}

// tryAdd attempts to insert a candidate neighbor into the heap.
// The candidate is accepted only if it improves upon the current
// worst-distance element when the heap is at capacity.
func (heap *maximumNeighborHeap) tryAdd(candidate Neighbor) {
	// Ignore insertions into a nil or zero-capacity heap.
	if heap == nil || heap.maximumSize == 0 {
		return
	}

	// Add directly if capacity has not yet been reached.
	if len(heap.items) < heap.maximumSize {
		heap.items = append(heap.items, candidate)
		heap.siftUp(len(heap.items) - 1)

		return
	}

	// Replace the root if the candidate is closer than the current worst.
	if candidate.Distance < heap.items[0].Distance {
		heap.items[0] = candidate
		heap.siftDown(0)
	}
}

// sortedAscending returns the heap contents ordered by increasing distance.
// The heap itself is not modified.
func (heap *maximumNeighborHeap) sortedAscending() []Neighbor {
	// Return nil if the heap does not exist.
	if heap == nil {
		return nil
	}

	// Create a shallow copy to avoid mutating internal state.
	copied := append([]Neighbor(nil), heap.items...)

	sort.Slice(copied, func(compare, against int) bool {
		return copied[compare].Distance < copied[against].Distance
	})

	return copied
}
