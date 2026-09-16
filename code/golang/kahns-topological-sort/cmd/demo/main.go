// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file demonstrates the use of Kahn's Algorithm for performing
//	a topological sort on a directed graph.
//
//	The example graph represents a simple dependency structure where
//	certain tasks must be completed before others can begin. The graph
//	is processed using Kahn's Algorithm to produce a valid execution
//	order that respects all dependencies.
//
//	The demonstration is intentionally explicit and heavily commented
//	to illustrate how the algorithm behaves step by step, rather than
//	optimizing for brevity.
//
// Design Notes:
//   - The graph is constructed using an adjacency list.
//   - Nodes represent tasks; edges represent dependency direction.
//   - The algorithm detects cycles implicitly via node exhaustion.
//   - Output ordering may vary due to Go's randomized map iteration.
//
// Intended Use:
//
//	This file serves as a reference and teaching example for
//	topological sorting using Kahn's Algorithm in Go.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/kahns-topological-sort/kahnstopologicalsort"
)

func main() {
	// -------------------------------------------------------------------------
	// Step 1: Define a directed graph using an adjacency list.
	//
	// Each key represents a node in the graph.
	// Each slice contains nodes that depend on the key node.
	//
	// In dependency terms:
	//   A -> B means "A must be completed before B".
	// -------------------------------------------------------------------------
	graph := map[string][]string{
		"compile": {"link"},
		"design":  {"compile"},
		"test":    {"package"},
		"link":    {"package"},
		"package": {},
	}

	// -------------------------------------------------------------------------
	// Step 2: Perform the topological sort using Kahn's Algorithm.
	//
	// The algorithm will:
	//   - Compute in-degrees for all nodes.
	//   - Repeatedly select nodes with zero in-degree.
	//   - Remove their outgoing edges.
	//   - Detect cycles if nodes remain unprocessed.
	// -------------------------------------------------------------------------
	topologicalOrder, ok := kahnstopologicalsort.TopologicalSortKahn(graph)

	// -------------------------------------------------------------------------
	// Step 3: Interpret the result.
	//
	// If ok is false, the graph contains a cycle and no valid ordering
	// exists. If ok is true, the returned slice contains a valid order
	// in which all dependencies are satisfied.
	// -------------------------------------------------------------------------
	if !ok {
		fmt.Println("The graph contains a cycle. No topological ordering exists.")

		return
	}

	// -------------------------------------------------------------------------
	// Step 4: Output the computed topological ordering.
	//
	// This ordering represents one valid sequence in which tasks
	// can be executed while respecting all dependencies.
	// -------------------------------------------------------------------------
	fmt.Println("Topological ordering:")

	for index, node := range topologicalOrder {
		fmt.Printf("\t%d: %s\n", index+1, node)
	}
}
