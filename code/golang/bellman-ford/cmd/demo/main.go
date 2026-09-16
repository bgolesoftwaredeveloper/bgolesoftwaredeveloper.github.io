// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file serves as the entry point for demonstrating the Bellman–Ford
//	single-source shortest path algorithm. It constructs a directed, weighted
//	graph containing negative edge weights, invokes the Bellman–Ford
//	implementation, and prints the resulting shortest-path distances.
//
//	The example graph is intentionally chosen to exercise key properties of
//	the algorithm, including edge relaxation over multiple passes and safe
//	handling of negative weights without introducing negative-weight cycles.
//
// Notes:
//   - The graph is defined using an edge-list representation.
//   - Vertex identifiers are zero-based and contiguous.
//   - The source vertex for shortest-path computation is vertex 0.
//   - Output distances are indexed by destination vertex.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/bellman-ford/bellmanford"
)

func main() {
	// Define a directed graph with weighted edges.
	// The graph contains negative weights but no negative-weight cycles.
	graph := bellmanford.Graph{
		Count: 5,
		Edges: []bellmanford.Edge{
			{Source: 0, Destination: 1, Weight: 6},
			{Source: 0, Destination: 2, Weight: 7},
			{Source: 1, Destination: 2, Weight: 8},
			{Source: 1, Destination: 3, Weight: 5},
			{Source: 1, Destination: 4, Weight: -4},
			{Source: 2, Destination: 3, Weight: -3},
			{Source: 2, Destination: 4, Weight: 9},
			{Source: 3, Destination: 1, Weight: -2},
			{Source: 4, Destination: 0, Weight: 2},
			{Source: 4, Destination: 3, Weight: 7},
		},
	}

	// Execute the Bellman–Ford algorithm using vertex 0 as the source.
	// The function returns the shortest path distances or an error if a
	// negative-weight cycle is detected.
	shortestDistances, err := bellmanford.BellmanFord(graph, 0)

	if err != nil {
		fmt.Println("Error:", err)

		return
	}

	// Output the computed shortest distances from the source vertex.
	fmt.Println("Shortest distances from source vertex 0:")

	for index, distance := range shortestDistances {
		fmt.Printf("\tVertex %d -> Distance %d\n", index, distance)
	}
}
