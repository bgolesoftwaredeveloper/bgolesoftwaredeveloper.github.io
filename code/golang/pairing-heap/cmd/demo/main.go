// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file provides a small, self-contained demonstration of the pairing
//	heap data structure implemented in the pairingheap package.
//
//	The program constructs a min-oriented pairing heap of integers, inserts
//	several values, performs a decrease-key operation, and then repeatedly
//	extracts the minimum element while printing the results.
//
//	The purpose of this file is purely demonstrative: it shows the expected
//	usage patterns, invariants, and observable behavior of the pairing heap
//	API without relying on tests or external frameworks.
//
// Intended Demonstration Flow:
//
//  1. Construct an empty pairing heap with an integer comparator.
//  2. Insert multiple values into the heap.
//  3. Perform a decrease-key operation using a node handle.
//  4. Repeatedly extract the minimum value until the heap is empty.
//  5. Print each extracted value to verify sorted order.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/pairing-heap/pairingheap"
)

func main() {
	// -------------------------------------------------------------------------
	// Step 1: Define a comparator for integers.
	//
	// The pairing heap requires a strict weak ordering. For integers, this
	// comparator simply returns true when the left value is smaller than
	// the right value, producing a min-heap.
	// -------------------------------------------------------------------------
	comparator := func(left, right int) bool {
		return left < right
	}

	// -------------------------------------------------------------------------
	// Step 2: Construct an empty pairing heap using the comparator.
	//
	// The returned heap is fully initialized and ready for use. The zero
	// value of PairingHeap is not valid, so construction must go through
	// NewPairingHeap.
	// -------------------------------------------------------------------------
	heap := pairingheap.NewPairingHeap(comparator)

	// -------------------------------------------------------------------------
	// Step 3: Insert several values into the heap.
	//
	// Each insertion returns a node handle. These handles can later be used
	// to perform decrease-key operations without searching the heap.
	// -------------------------------------------------------------------------
	node := heap.Insert(42)

	heap.Insert(17)
	heap.Insert(88)
	heap.Insert(3)
	heap.Insert(29)

	// -------------------------------------------------------------------------
	// Step 4: Perform a decrease-key operation.
	//
	// We reduce the value stored in nodeA from 42 down to 1. This should
	// cause the node to be cut from its current position and melded back
	// into the heap at the root, restoring heap order.
	// -------------------------------------------------------------------------
	heap.DecreaseKey(node, 1)

	// -------------------------------------------------------------------------
	// Step 5: Repeatedly extract the minimum value.
	//
	// Each call to ExtractMinimum removes the smallest value currently stored
	// in the heap. The values should be returned in strictly increasing order,
	// demonstrating that the heap invariants are preserved.
	// -------------------------------------------------------------------------
	fmt.Print("Extracting values in sorted order: ")

	for !heap.IsEmpty() {
		minimumValue, _ := heap.ExtractMinimum()

		fmt.Printf("%d ", minimumValue)
	}

	fmt.Println()
}
