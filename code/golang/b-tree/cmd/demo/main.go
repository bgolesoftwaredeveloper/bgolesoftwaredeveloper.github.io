// -----------------------------------------------------------------------------
// File: main.go
//
// Description:
//
//	This program demonstrates the usage of a B-Tree implementation.
//	It performs insertion, search, deletion, and in-order traversal
//	to validate correctness and behavior.
//
//	The example intentionally exercises:
//	  - Root splitting.
//	  - Internal and leaf insertions.
//	  - Successful and failed searches.
//	  - Deletions that trigger borrowing and merging.
//	  - Structural integrity after modifications.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/b-tree/btree"
)

func main() {
	// -------------------------------------------------------------------------
	// Step 1: Construct a new B-Tree.
	//
	// Minimum degree t = 2:
	//   - Each node can have at most 3 keys.
	//   - Each internal node has between 2 and 4 children.
	// -------------------------------------------------------------------------
	tree := btree.NewBTree(2)

	// -------------------------------------------------------------------------
	// Step 2: Insert keys.
	//
	// These insertions are chosen to:
	//   - Force node splits.
	//   - Populate both internal and leaf nodes.
	//   - Exercise recursive insertion logic.
	// -------------------------------------------------------------------------
	valuesToInsert := []int{10, 20, 5, 6, 12, 30, 7, 17}

	fmt.Println("Inserting values:")

	for _, value := range valuesToInsert {
		fmt.Printf("\tInserting %d\n", value)

		tree.Insert(value)
	}

	// -------------------------------------------------------------------------
	// Step 3: Print the tree using in-order traversal.
	//
	// In-order traversal of a B-Tree prints keys in sorted order.
	// This confirms that structural invariants are preserved.
	// -------------------------------------------------------------------------
	fmt.Print("\nTree contents (in-order): ")
	tree.PrintInOrder()

	// -------------------------------------------------------------------------
	// Step 4: Search for keys.
	//
	// Demonstrates both successful and unsuccessful lookups.
	// -------------------------------------------------------------------------
	searchKeys := []int{6, 15, 17, 99}

	fmt.Println("\nSearch results:")

	for _, key := range searchKeys {
		found := tree.Search(key)

		fmt.Printf("\tSearch(%d) = %v\n", key, found)
	}

	// -------------------------------------------------------------------------
	// Step 5: Remove keys.
	//
	// These removals are chosen to trigger:
	//   - Leaf deletion.
	//   - Internal node deletion.
	//   - Borrowing from siblings.
	//   - Merging nodes.
	// -------------------------------------------------------------------------
	valuesToRemove := []int{6, 13, 7, 4, 2, 16}

	fmt.Println("\nRemoving keys:")

	for _, value := range valuesToRemove {
		fmt.Printf("\tRemoving %d\n", value)

		tree.Remove(value)

		// Print tree after each removal to observe structural changes.
		fmt.Print("\t\tKeys: ")
		tree.PrintInOrder()
		fmt.Println()
	}

	// -------------------------------------------------------------------------
	// Step 6: Final tree state.
	//
	// Confirms that the tree remains valid after all operations.
	// -------------------------------------------------------------------------
	fmt.Print("\nFinal tree contents (in-order): ")
	tree.PrintInOrder()

	fmt.Println("\nDemo complete.")
}
