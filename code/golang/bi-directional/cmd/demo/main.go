// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//		This file provides a demonstration and validation harness for the
//		bidirectional graph implementation.
//
//		The main function exercises all public graph operations, including:
//	  - Graph construction
//	  - Vertex existence checks
//	  - Undirected edge insertion and removal
//	  - Neighbor enumeration
//	  - Vertex removal
//	  - Breadth-first traversal
//
//		The intent is not to provide exhaustive testing, but to serve as a
//		readable, executable reference showing correct usage patterns and
//		expected behavior.
//
// Notes:
//   - Output order involving map iteration may vary between runs.
//   - This file assumes the graph implementation is correct and focuses
//     on illustrating API usage rather than defensive testing.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/bi-directional/bidirectional"
)

func main() {
	// -------------------------------------------------------------------------
	// Graph construction.
	// -------------------------------------------------------------------------
	// Create a new empty undirected graph.
	graph := bidirectional.NewGraph()

	// -------------------------------------------------------------------------
	// Vertex and edge creation.
	// -------------------------------------------------------------------------
	// Add undirected edges between vertices.
	// Vertices are created implicitly if they do not already exist.
	graph.AddUndirectedEdge("A", "B")
	graph.AddUndirectedEdge("A", "C")
	graph.AddUndirectedEdge("B", "D")
	graph.AddUndirectedEdge("C", "D")
	graph.AddUndirectedEdge("D", "E")

	// -------------------------------------------------------------------------
	// Vertex existence checks.
	// -------------------------------------------------------------------------
	// Check whether specific vertices exist in the graph.
	fmt.Println("Has vertex A:", graph.HasVertex("A"))
	fmt.Println("Has vertex Z:", graph.HasVertex("Z"))

	// -------------------------------------------------------------------------
	// Edge existence checks.
	// -------------------------------------------------------------------------
	// Verify the presence or absence of undirected edges.
	fmt.Println("Has edge A <-> B:", graph.HasUndirectedEdge("A", "B"))
	fmt.Println("Has edge A <-> D:", graph.HasUndirectedEdge("A", "D"))

	// -------------------------------------------------------------------------
	// Neighbor enumeration.
	// -------------------------------------------------------------------------
	// Retrieve and display neighbors for selected vertices.
	fmt.Println("Neighbors of A:", graph.GetNeighbors("A"))
	fmt.Println("Neighbors of D:", graph.GetNeighbors("D"))

	// -------------------------------------------------------------------------
	// Breadth-first traversal.
	// -------------------------------------------------------------------------
	// Perform a BFS traversal starting from vertex A.
	bfsOrder := graph.BreadthFirstTraversal("A")

	fmt.Println("BFS traversal starting at A:", bfsOrder)

	// -------------------------------------------------------------------------
	// Undirected edge removal.
	// -------------------------------------------------------------------------
	// Remove an existing edge and verify its absence.
	graph.RemoveUndirectedEdge("A", "B")

	fmt.Println("Has edge A <-> B after removal:", graph.HasUndirectedEdge("A", "B"))
	fmt.Println("Neighbors of A after edge removal:", graph.GetNeighbors("A"))
	fmt.Println("Neighbors of B after edge removal:", graph.GetNeighbors("B"))

	// -------------------------------------------------------------------------
	// Vertex removal.
	// -------------------------------------------------------------------------
	// Remove a vertex entirely from the graph.
	graph.RemoveVertex("D")

	// Verify that the vertex and its incident edges have been removed.
	fmt.Println("Has vertex D after removal:", graph.HasVertex("D"))
	fmt.Println("Neighbors of B after removing D:", graph.GetNeighbors("B"))
	fmt.Println("Neighbors of C after removing D:", graph.GetNeighbors("C"))

	// -------------------------------------------------------------------------
	// Final traversal to observe the modified graph structure.
	// -------------------------------------------------------------------------
	// Perform another BFS traversal to reflect the updated topology.
	finalBfsOrder := graph.BreadthFirstTraversal("A")

	fmt.Println("Final BFS traversal starting at A:", finalBfsOrder)
}
