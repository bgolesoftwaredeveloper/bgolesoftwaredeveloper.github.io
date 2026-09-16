// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file provides a simple demonstration of the
//	BinaryHeapEytzingerLayout data structure.
//
//	The program constructs a binary min-heap using Eytzinger (level-order)
//	layout, performs a sequence of insertions and removals, and prints
//	results to illustrate heap behavior.
//
//	The demonstration is intentionally verbose and linear to emphasize
//	correct usage patterns, heap invariants, and expected outcomes.
//
// Intended Use:
//
//		This file is intended for:
//	  - Manual testing
//	  - Educational walkthroughs
//	  - Sanity-checking heap behavior during development
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/eytzingerlevelorder/eytzingerlevelorder"
)

func main() {
	// -------------------------------------------------------------------------
	// Step 1: Create a new empty heap instance.
	// -------------------------------------------------------------------------
	// Instantiate a new binary heap using the provided constructor.
	heap := eytzingerlevelorder.NewBinaryHeapEyzingerLayout()

	// Report the initial state of the heap.
	fmt.Println("Initial heap state:")
	fmt.Println("\tIs empty:", heap.IsEmpty())
	fmt.Println("\tSize:", heap.Size())
	fmt.Println("\tIs valid min-heap:", heap.IsValidMinimumHeap())
	fmt.Println()

	// -------------------------------------------------------------------------
	// Step 2: Insert values into the heap.
	// -------------------------------------------------------------------------
	// Define a set of values to insert.
	valuesToInsert := []int{7, 2, 9, 1, 5, 3}

	fmt.Println("Inserting values into the heap:")

	// Insert each value one-by-one.
	for insertionIndex := 0; insertionIndex < len(valuesToInsert); insertionIndex++ {

		currentValue := valuesToInsert[insertionIndex]

		fmt.Println("\tPushing:", currentValue)

		// Insert the value and restore heap invariants.
		heap.Push(currentValue)

		// Validate the heap after each insertion.
		fmt.Println("\t\tHeap valid:", heap.IsValidMinimumHeap())
		fmt.Println("\t\tHeap size:", heap.Size())
	}

	fmt.Println()

	// -------------------------------------------------------------------------
	// Step 3: Remove elements from the heap.
	// -------------------------------------------------------------------------
	fmt.Println("Removing elements from the heap (Pop):")

	// Continue popping until the heap becomes empty.
	for !heap.IsEmpty() {
		// Remove the smallest element.
		poppedValue, popError := heap.Pop()

		if popError != nil {
			fmt.Println("\tError during pop:", popError)
			break
		}

		// Display the removed value and current heap state.
		fmt.Println("\tPopped:", poppedValue)
		fmt.Println("\t\tHeap valid:", heap.IsValidMinimumHeap())
		fmt.Println("\t\tRemaining size:", heap.Size())
	}

	fmt.Println()

	// -------------------------------------------------------------------------
	// Step 4: Demonstrate clearing the heap.
	// -------------------------------------------------------------------------
	// Reinsert a few values.
	fmt.Println("Rebuilding heap for Clear demonstration.")

	heap.Push(4)
	heap.Push(1)
	heap.Push(6)

	fmt.Println("\tSize before Clear:", heap.Size())
	fmt.Println()

	// Clear all elements from the heap.
	heap.Clear()

	// Validate the heap state after clearing.
	fmt.Println("After Clear:")
	fmt.Println("\tIs empty:", heap.IsEmpty())
	fmt.Println("\tSize:", heap.Size())
	fmt.Println("\tIs valid min-heap:", heap.IsValidMinimumHeap())
}
