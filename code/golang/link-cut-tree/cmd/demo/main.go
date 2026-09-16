// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file provides a minimal demonstration of the Link-Cut Tree (LCT)
//	implementation. The example constructs a small dynamic forest, performs
//	link and cut operations, and verifies connectivity between nodes.
//
//	The purpose of this file is illustrative rather than exhaustive. It shows
//	how the public API of the Link-Cut Tree can be used safely and clearly,
//	while relying on the internal structure to maintain correctness.
//
//	This demonstration assumes that all Link-Cut Tree invariants are enforced
//	by the underlying implementation and focuses only on high-level usage.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/link-cut-tree/linkcuttree"
)

func main() {
	// -------------------------------------------------------------------------
	// Step 1: Construct independent nodes.
	// -------------------------------------------------------------------------
	// Each node initially represents a singleton tree in the forest.
	rootNode := &linkcuttree.LCTNode{}
	intermediateNode := &linkcuttree.LCTNode{}
	terminalNode := &linkcuttree.LCTNode{}
	disconnectedNode := &linkcuttree.LCTNode{}

	fmt.Println("nodes created")

	// -------------------------------------------------------------------------
	// Step 2: Build a tree via link operations.
	// -------------------------------------------------------------------------
	// The following structure is formed:
	//
	//     rootNode
	//        |
	//   intermediateNode
	//        |
	//    terminalNode
	//
	// disconnectedNode remains isolated.
	linkcuttree.Link(intermediateNode, rootNode)
	linkcuttree.Link(terminalNode, intermediateNode)

	fmt.Println("tree constructed via link operations")

	// -------------------------------------------------------------------------
	// Step 3: Connectivity queries.
	// -------------------------------------------------------------------------
	// Verify connectivity within the tree and isolation of the unused node.
	fmt.Println("rootNode ↔ terminalNode connected:",
		linkcuttree.Connected(rootNode, terminalNode))

	fmt.Println("rootNode ↔ disconnectedNode connected:",
		linkcuttree.Connected(rootNode, disconnectedNode))

	// -------------------------------------------------------------------------
	// Step 4: Cut operation.
	// -------------------------------------------------------------------------
	// Remove the edge between intermediateNode and terminalNode, splitting
	// the structure into two separate trees.
	linkcuttree.Cut(terminalNode, intermediateNode)

	fmt.Println("edge between intermediateNode and terminalNode cut")

	// -------------------------------------------------------------------------
	// Step 5: Connectivity after cut.
	// -------------------------------------------------------------------------
	// Verify that the cut successfully separated the trees.
	fmt.Println("rootNode ↔ terminalNode connected after cut:",
		linkcuttree.Connected(rootNode, terminalNode))

	fmt.Println("intermediateNode ↔ terminalNode connected after cut:",
		linkcuttree.Connected(intermediateNode, terminalNode))

	// -------------------------------------------------------------------------
	// Step 6: Rerooting demonstration.
	// -------------------------------------------------------------------------
	// Reroot the remaining tree at intermediateNode. This changes the logical
	// root without altering connectivity.
	intermediateNode.MakeRoot()

	fmt.Println("tree rerooted at intermediateNode")

	fmt.Println("rootNode ↔ intermediateNode connected after reroot:",
		linkcuttree.Connected(rootNode, intermediateNode))
}
