// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file provides a minimal executable entry point demonstrating the
//	usage of the Floyd–Warshall algorithm implementation. It constructs a
//	sample weighted graph as an adjacency matrix, invokes the algorithm,
//	and prints the resulting all-pairs shortest path distance matrix.
//
//	The example graph includes unreachable edges and finite weights to
//	illustrate how math.Inf(1) is used to represent the absence of a direct
//	connection between vertices.
//
// Notes:
//   - This file exists purely for demonstration and validation purposes.
//   - The graph is small to keep output readable and interpretable.
//   - Output values represent shortest path distances between vertex pairs.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"
	"math"

	"github.com/bgolesoftwaredeveloper/all-pairs-shortest-path/floydwarshall"
)

func main() {
	// Define a sentinel value representing the absence of a direct edge.
	// math.Inf(1) is used so arithmetic involving unreachable paths behaves
	// correctly under addition and comparison.
	infinity := math.Inf(1)

	// Construct an adjacency matrix representing a weighted directed graph.
	// Each entry [i][j] denotes the cost of traveling from vertex i to vertex j.
	// A value of infinity indicates no direct edge exists.
	matrix := [][]float64{
		{0, 3, infinity, 7},
		{8, 0, 2, infinity},
		{5, infinity, 0, 1},
		{2, infinity, infinity, 0},
	}

	// Execute the Floyd–Warshall algorithm to compute shortest paths between
	// all pairs of vertices in the graph.
	shortestDistances := floydwarshall.FloydWarshall(matrix)
	lengthOfShortestDistances := len(shortestDistances)

	// Output the resulting distance matrix in row-major order.
	// Each row corresponds to a source vertex, and each column corresponds
	// to a destination vertex.
	fmt.Println("All-pairs shortest path distance matrix:")

	for source := 0; source < lengthOfShortestDistances; source++ {
		fmt.Print("\t[")

		for destination := 0; destination < lengthOfShortestDistances; destination++ {
			if destination < lengthOfShortestDistances-1 {
				fmt.Printf("%.0f, ", shortestDistances[source][destination])
			} else {
				fmt.Printf("%.0f", shortestDistances[source][destination])
			}
		}

		fmt.Println("]")
	}
}
