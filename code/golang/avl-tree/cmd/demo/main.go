// -----------------------------------------------------------------------------
// File: main.go
//
// Description:
//
//	This file provides a simple executable demonstration of the AVL tree
//	implementation. It exercises all public-facing operations exposed by
//	the avltree package, including tree creation, insertion, searching,
//	and deletion.
//
//	The demonstration uses a deterministic set of keys chosen to trigger
//	AVL rotations and rebalancing internally, allowing correctness of
//	both structural and logical behavior to be observed via output.
//
//	This file is intended for validation, debugging, and instructional
//	purposes, and does not expose or rely on any internal implementation
//	details of the AVL tree.
//
// Notes:
//   - Output is purely informational and reflects logical operations only.
//   - Structural rebalancing occurs internally and is not directly visible.
//   - This program is suitable for use as a reference example or test driver.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/avltree/avltree"
)

func main() {
	// Create a new, empty AVL tree instance.
	// The returned tree has a nil root and contains no keys.
	tree := avltree.New()

	// Define a deterministic sequence of keys to insert.
	// This sequence is intentionally chosen to trigger AVL rotations.
	keys := []int{30, 20, 40, 10, 25, 35, 50, 5}

	fmt.Println("Inserting values:")

	// Insert each key into the AVL tree.
	// Rebalancing occurs internally as needed.
	for _, key := range keys {
		fmt.Printf("  Insert %d\n", key)

		tree.Insert(key)
	}

	// Separate insertion output from subsequent operations.
	fmt.Println()

	// Define a set of keys to search for.
	// This includes both existing and non-existing values.
	searchKeys := []int{25, 15, 50, 100}

	fmt.Println("Searching values:")

	// Perform searches and report whether each key is present.
	for _, key := range searchKeys {
		found := tree.Search(key)

		fmt.Printf("  Search %d -> %v\n", key, found)
	}

	// Separate search output from deletion phase.
	fmt.Println()

	// Delete keys that trigger rebalancing.
	deleteKeys := []int{10, 30}

	fmt.Println("Deleting values:")

	// Delete each key from the AVL tree.
	// Deletion may cause rotations to restore AVL invariants.
	for _, key := range deleteKeys {
		fmt.Printf("  Delete %d\n", key)

		tree.Delete(key)
	}

	// Separate deletion output from final verification.
	fmt.Println()

	// Re-run searches after deletion.
	fmt.Println("Searching after deletions:")

	// Re-run searches after deletions to confirm correctness.
	for _, key := range searchKeys {
		found := tree.Search(key)

		fmt.Printf("  Search %d -> %v\n", key, found)
	}

	// Final output indicating program completion.
	fmt.Println()
	fmt.Println("AVL tree demonstration complete.")
}
