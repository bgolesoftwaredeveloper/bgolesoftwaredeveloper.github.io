// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file demonstrates usage of the Dijkstra shortest-path algorithm
//	implemented in the dijkstra package.
//
//	A small directed, weighted graph is constructed using an adjacency
//	list representation. Dijkstra’s algorithm is then invoked to compute
//	the shortest distance from a chosen start vertex to all other reachable
//	vertices in the graph.
//
// Notes:
//   - The graph uses non-negative edge weights, as required by Dijkstra’s algorithm.
//   - Vertices are identified using integer labels.
//   - The output distances represent the minimum total path cost from
//     the start vertex to each destination vertex.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/greedy-single-source-shortest-path/dijkstra"
)

func main() {
	// Construct a new directed graph.
	graph := dijkstra.NewGraph()

	// Add directed edges with associated weights.
	graph.AddDirectedEdge(0, 1, 4)
	graph.AddDirectedEdge(0, 2, 1)
	graph.AddDirectedEdge(2, 1, 2)
	graph.AddDirectedEdge(1, 3, 1)
	graph.AddDirectedEdge(2, 3, 5)

	// Choose the start vertex for shortest-path computation.
	vertex := 0

	// Compute shortest distances from the start vertex
	// to all other reachable vertices.
	shortestDistances := dijkstra.Dijkstra(graph, vertex)

	// Handle invalid graph or start vertex cases defensively.
	if shortestDistances == nil {
		fmt.Println("Failed to compute shortest paths.")

		return
	}

	// Display the computed shortest distances.
	for destinationVertex, distance := range shortestDistances {
		fmt.Printf("Shortest distance from vertex %d to vertex %d is %d\n",
			vertex,
			destinationVertex,
			distance,
		)
	}
}
