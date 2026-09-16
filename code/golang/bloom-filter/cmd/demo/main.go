// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	  This file provides a demonstration of the defensive Counting Bloom Filter
//	  implementation. It showcases initialization using optimal parameters,
//	  concurrent-safe additions, membership testing, and the removal of
//	  elements—a feature unique to the counting variant of this data structure.
//
//	  The demonstration includes:
//	- Creating a filter with a target false-positive rate.
//	- Adding multiple keys to the filter.
//	- Testing for existence (probabilistic "maybe" vs definitive "no").
//	- Removing a key and verifying its absence.
//	- Handling edge cases like empty data or nil receivers gracefully.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/bloom-filter/bloomfilter"
)

func main() {
	// 1. Initialize the filter: Expecting 1000 items with a 1% false-positive rate.
	// This will calculate the optimal bucket count and hash iterations automatically.
	filter := bloomfilter.NewOptimalBloomFilter(1000, 0.01)

	// 2. Define some test datasets.
	braiden := []byte("braiden_gole")
	brant := []byte("brant_gole")
	ghost := []byte("non_existent_user")

	// 3. Demonstrate Addition.
	fmt.Println("--- Adding Users ---")

	filter.Add(braiden)
	filter.Add(brant)

	fmt.Printf("Added: %s\n", braiden)
	fmt.Printf("Added: %s\n", brant)

	// 4. Demonstrate Membership Testing.
	// We expect 'userA' to be likely present and 'ghost' to be definitely absent.
	fmt.Println("\n--- Testing Membership ---")

	fmt.Printf("Contains '%s'? %v\n", braiden, filter.Contains(braiden))
	fmt.Printf("Contains '%s'? %v\n", ghost, filter.Contains(ghost))

	// 5. Demonstrate Removal (Counting Filter specific).
	// We remove 'userA' and verify that it no longer reports as present.
	fmt.Println("\n--- Removing User ---")

	filter.Remove(braiden)

	fmt.Printf("Removed: %s\n", braiden)
	fmt.Printf("Contains '%s' after removal? %v\n", braiden, filter.Contains(braiden))

	// 6. Demonstrate Defensive Nil-Safety.
	// The code is designed not to panic even if the receiver is nil.
	fmt.Println("\n--- Defensive Nil Check ---")

	var nilFilter *bloomfilter.BloomFilter

	nilFilter.Add([]byte("panic_test"))

	fmt.Println("Nil filter 'Add' handled safely.")
}
