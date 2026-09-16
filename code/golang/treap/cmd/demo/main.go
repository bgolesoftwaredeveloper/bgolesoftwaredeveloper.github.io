// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This program demonstrates the usage and functionality of the generic
//	Treap data structure.
//
//	The example initializes a new Treap instance using a time-based random seed,
//	populates it with an unsorted set of integer keys, and performs common
//	operations including order-statistic queries, rank lookups, and key deletion.
//
//	The goal is to demonstrate practical applications of the Treap structure:
//
//	  - Dynamic insertion of unsorted elements into a balanced BST.
//	  - Efficient O(log n) order-statistic lookup via FindKth.
//	  - Exact position determination via Rank calculations.
//	  - Dynamic node removal and automatic tree restructuring via Delete.
//
// Notes:
//   - The PCG pseudo-random generator is seeded dynamically using Unix time.
//   - Output is intended for direct demonstration and inspection.
//   - Order-statistics are 0-indexed.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"
	"time"

	"github.com/bgolesoftwaredeveloper/treap/treap"
)

func main() {
	// Generate random engine seeds using current timestamp and golden ratio constant.
	seedState := uint64(time.Now().UnixNano())
	seedStream := uint64(0x9E3779B97F4A7C15)

	// Construct a generic Treap instance for integer keys.
	treapEngine := treap.NewTreap[int](seedState, seedStream)

	// Define an unsorted slice of integer keys to populate the Treap.
	numbers := []int{50, 20, 80, 10, 30, 70, 90, 40}

	// Insert each key into the Treap.
	for _, number := range numbers {
		treapEngine.Insert(number)
	}

	// Query and print elements in sorted order using 0-indexed ranks.
	for k := 0; k < len(numbers); k++ {
		value, found := treapEngine.FindKth(k)

		if found {
			fmt.Printf("%d-th smallest element: %d.\n", k, value)
		}
	}

	fmt.Println()

	// Define a set of keys to perform rank queries against.
	lookupValues := []int{10, 40, 80, 100}

	// Compute and print the 0-indexed position (rank) for each lookup value.
	for _, value := range lookupValues {
		rank := treapEngine.Rank(value)

		fmt.Printf("Rank (index) of %d: %d.\n", value, rank)
	}

	fmt.Println()

	// Remove a target key from the Treap.
	treapEngine.Delete(50)

	// Retrieve the new 3rd smallest element following deletion.
	value, _ := treapEngine.FindKth(3)

	fmt.Printf("New 3rd smallest element after deleting 50: %d.\n", value)
}
