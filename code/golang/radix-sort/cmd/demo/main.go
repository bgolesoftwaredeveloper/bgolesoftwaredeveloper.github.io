// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file serves as the primary entry point for demonstrating the
//	radixsort package implementation.
//
//	It initializes a dataset containing a variety of unsigned integers,
//	invokes the RadixSort function, and provides robust error handling to
//	account for the defensive checks (such as negative value detection)
//	implemented in the core algorithm.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"
	"log"

	"github.com/bgolesoftwaredeveloper/radix-sort/radixsort"
)

func main() {
	// Initialize a collection of unsorted non-negative integers.
	// This dataset includes varying magnitudes to test the LSD logic.
	unsortedData := []int{170, 45, 75, 90, 802, 24, 2, 66}

	fmt.Printf("Original dataset: %v\n", unsortedData)

	// Execute the RadixSort algorithm.
	// The function returns an error if defensive checks (like negative
	// number detection) are triggered during the validation phase.
	err := radixsort.RadixSort(unsortedData)

	if err != nil {
		log.Fatalf("Sorting failed: %v", err)
	}

	// Output the final state of the slice to verify the sort was successful.
	// At this stage, the data is ordered from least to greatest.
	fmt.Printf("Sorted dataset:   %v\n", unsortedData)
}
