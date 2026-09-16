// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file demonstrates how to use the
//	kruskalminimumspanningtree package to construct an undirected,
//	weighted graph and compute its Minimum Spanning Tree (MST)
//	using Kruskal’s algorithm.
//
//	The example intentionally uses explicit identifiers and
//	step-by-step commentary to illustrate correct usage of the
//	public API.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/kruskal-minimum-spannning-tree/kruskalminimumspanningtree"
)

func main() {
	// -------------------------------------------------------------------------
	// Step 1: Construct a new undirected weighted graph.
	//
	// The graph is initialized with a fixed number of vertices.
	// Vertex identifiers are assumed to be zero-based and contiguous.
	// -------------------------------------------------------------------------
	graph := kruskalminimumspanningtree.NewUndirectedWeightedGraph(5)

	// -------------------------------------------------------------------------
	// Step 2: Insert weighted edges into the graph.
	//
	// Each call to AddEdge performs bounds checking and validation.
	// Invalid edges are rejected and not added to the graph.
	// -------------------------------------------------------------------------
	graph.AddEdge(0, 1, 4)
	graph.AddEdge(0, 2, 3)
	graph.AddEdge(1, 2, 1)
	graph.AddEdge(1, 3, 2)
	graph.AddEdge(2, 3, 4)
	graph.AddEdge(3, 4, 2)
	graph.AddEdge(2, 4, 5)

	// -------------------------------------------------------------------------
	// Step 3: Execute Kruskal’s algorithm on the constructed graph.
	//
	// The function returns:
	//   - The list of edges included in the Minimum Spanning Tree.
	//   - The total weight of the spanning tree.
	//
	// A nil slice indicates that no valid MST could be produced
	// (e.g., due to a disconnected graph).
	// -------------------------------------------------------------------------
	minimumSpanningTreeEdges, totalTreeWeight :=
		kruskalminimumspanningtree.Kruskal(graph)

	// -------------------------------------------------------------------------
	// Step 4: Validate the result before using it.
	//
	// Defensive usage ensures we do not assume success if the
	// graph was malformed or disconnected.
	// -------------------------------------------------------------------------
	if minimumSpanningTreeEdges == nil {
		fmt.Println("Failed to compute a minimum spanning tree.")

		return
	}

	// -------------------------------------------------------------------------
	// Step 5: Output the results.
	//
	// The edges are listed in the order they were selected by
	// Kruskal’s algorithm.
	// -------------------------------------------------------------------------
	fmt.Println("Minimum Spanning Tree Edges:")

	for _, edge := range minimumSpanningTreeEdges {
		fmt.Printf("\t%d -- %d (weight = %d)\n",
			edge.Start(),
			edge.End(),
			edge.Weight(),
		)
	}

	fmt.Printf("Total MST Weight: %d\n", totalTreeWeight)
}
