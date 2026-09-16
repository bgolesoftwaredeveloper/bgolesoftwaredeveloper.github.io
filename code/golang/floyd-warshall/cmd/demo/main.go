// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file demonstrates the usage of the Floyd–Warshall algorithm on a
//	small weighted directed graph represented as an adjacency matrix.
//
//	The example graph includes both direct edges and absent connections,
//	allowing the algorithm to compute indirect shortest paths by considering
//	all vertices as potential intermediate waypoints.
//
//	The purpose of this file is illustrative rather than performance-focused.
//	It shows how to construct the input matrix, invoke the algorithm, and
//	interpret the resulting all-pairs shortest-path distance matrix.
//
// Notes:
//   - A large sentinel value is used to represent missing edges.
//   - The graph does not contain negative-weight cycles.
//   - The demonstration assumes the FloydWarshall function exists in scope.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/floyd-warshall/floydwarshall"
)

func main() {
	// between two vertices in the graph.
	infinityDistance := 1 << 30

	// Construct the adjacency matrix for a weighted directed graph.
	// Each cell graph[i][j] represents the cost of traveling from
	// vertex i to vertex j.
	graph := [][]int{
		{0, 3, infinityDistance, 7},
		{8, 0, 2, infinityDistance},
		{5, infinityDistance, 0, 1},
		{2, infinityDistance, infinityDistance, 0},
	}

	// Display the initial adjacency matrix so the original graph
	// structure is visible before shortest-path computation.
	fmt.Println("Initial distance matrix:")

	PrintDistanceMatrix(graph, infinityDistance)

	// Invoke the Floyd–Warshall algorithm to compute shortest paths
	// between all pairs of vertices.
	shortestDistanceMatrix := floydwarshall.FloydWarshall(graph)

	// Display the resulting matrix to demonstrate how indirect paths
	// improve distance estimates between vertex pairs.
	fmt.Println("\nShortest-path distance matrix:")

	PrintDistanceMatrix(shortestDistanceMatrix, infinityDistance)
}

// PrintDistanceMatrix outputs a distance matrix in a readable tabular form.
// Entries equal to infinityDistance are printed as "INF" to indicate
// unreachable vertex pairs.
func PrintDistanceMatrix(matrix [][]int, infinityDistance int) {
	for rowIndex := 0; rowIndex < len(matrix); rowIndex++ {
		for columnIndex := 0; columnIndex < len(matrix[rowIndex]); columnIndex++ {
			if matrix[rowIndex][columnIndex] == infinityDistance {
				fmt.Print("INF\t")
			} else {
				fmt.Printf("%d\t", matrix[rowIndex][columnIndex])
			}
		}

		fmt.Println()
	}
}
