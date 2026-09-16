// -----------------------------------------------------------------------------
// Package: bplustree
// File: main.go
//
// Description:
//
//	This file provides a robust implementation of a B+ Tree data structure.
//	Unlike a standard B-Tree, the B+ Tree stores all actual data (keys) in
//	leaf nodes, while internal nodes act as a routing index.
//
//	The implementation supports:
//	  - Insertion with automatic node splitting and height growth.
//	  - Deletion with rebalancing via borrowing and merging to maintain
//	    the minimum fill factor.
//	  - Range Searching using a linked-list of leaf nodes for O(log n + k)
//	    performance.
//	  - Updating keys via a Delete-and-Insert strategy to preserve sorting.
//
// Notes:
//   - Parameterized by 'degree' (M), where each node can have at most M keys.
//   - Leaf nodes are linked via a 'next' pointer for efficient range scans.
//   - Internal nodes "push up" keys during splits, while leaves "copy" keys.
//   - This version includes a main function for direct demonstration.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/b-plus-tree/bplustree"
)

func main() {
	// Initialize a new B+ Tree.
	tree := bplustree.NewBPlusTree()

	// Demonstrate Insertion.
	fmt.Println("Inserting keys: 10, 20, 5, 15, 25, 30")

	keys := []int{10, 20, 5, 15, 25, 30}

	for _, key := range keys {
		tree.Insert(key)
	}

	// Demonstrate Range Search.
	fmt.Println("Range Search (10 to 25):", tree.RangeSearch(10, 25))

	// Demonstrate Update (Change 15 to 100).
	fmt.Println("Updating key 15 to 100...")

	tree.Update(15, 100)

	// Demonstrate Deletion.
	fmt.Println("Deleting key 20...")

	tree.Delete(20)

	// Show final state using Range Search.
	fmt.Println("Final Tree Keys (Range 0 to 150):", tree.RangeSearch(0, 150))
}
