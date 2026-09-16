// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This program demonstrates the difference between a naive boolean flag
//	representation and a compact bitset representation.
//
//	The example initializes both structures with the same number of flags,
//	sets a small subset of indices, and then inspects their behavior using
//	common operations such as setting, clearing, counting, and iteration.
//
//	The goal is not to benchmark raw performance, but to illustrate:
//
//	  - How identical logical behavior can be achieved with different
//	    memory layouts.
//	  - How a compact bitset enables efficient bulk operations.
//	  - How iteration over set bits can skip unused regions efficiently.
//
// Notes:
//   - This program assumes fixed-size flag collections.
//   - Output is intended for demonstration and inspection.
//   - No concurrency or synchronization is involved.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/space-savings/spacesavings"
)

func main() {
	// Define the number of flags to be stored.
	numberOfFlags := 128

	// Construct a naive boolean flag store.
	naiveFlags := spacesavings.NewNaiveBooleanFlags(numberOfFlags)

	// Construct a compact bitset with the same logical capacity.
	compactBitset := spacesavings.NewCompactBitset(numberOfFlags)

	// Select a small set of indices to activate.
	indicesToSet := []int{3, 7, 42, 64, 99}

	// Set the same indices in both data structures.
	for _, index := range indicesToSet {
		naiveFlags.Set(index)
		compactBitset.Set(index)
	}

	// Report whether specific indices are set.
	fmt.Println("Flag checks:")

	for _, index := range indicesToSet {
		fmt.Printf("\tindex %d -> naive=%v, bitset=%v\n",
			index,
			naiveFlags.IsSet(index),
			compactBitset.IsSet(index),
		)
	}

	// Count the number of active flags using the compact bitset.
	fmt.Println()
	fmt.Printf("Total set bits (bitset): %d\n", compactBitset.Count())

	// Demonstrate iteration over set bits.
	fmt.Println()
	fmt.Println("Iterating over set bits:")

	compactBitset.ForEach(func(index int) {
		fmt.Printf("  bit %d is set\n", index)
	})

	// Clear a single bit and demonstrate the effect.
	indexToClear := 42

	naiveFlags.Clear(indexToClear)
	compactBitset.Clear(indexToClear)

	fmt.Println()
	fmt.Printf("After clearing index %d:\n", indexToClear)
	fmt.Printf("\tnaive=%v\n", naiveFlags.IsSet(indexToClear))
	fmt.Printf("\tbitset=%v\n", compactBitset.IsSet(indexToClear))

	// Clear all bits in the compact bitset.
	compactBitset.ClearAll()

	fmt.Println()
	fmt.Printf("After clearing all bits, count=%d\n", compactBitset.Count())
}
