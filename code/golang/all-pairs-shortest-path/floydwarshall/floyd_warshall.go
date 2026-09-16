// -----------------------------------------------------------------------------
// Package: floydwarshall
// File: floyd_warshall.go
//
// Description:
//
//	This file provides an implementation of the Floyd–Warshall algorithm for
//	computing shortest paths between all pairs of vertices in a weighted graph.
//	The algorithm uses dynamic programming to iteratively refine shortest
//	distances by allowing progressively larger sets of intermediate vertices.
//
//	The graph is represented as an adjacency matrix, where each entry denotes
//	the direct edge weight between two vertices. The algorithm supports negative
//	edge weights but does not permit negative cycles.
//
// Notes:
//   - The input matrix is treated as immutable and is not modified.
//   - Absence of an edge must be represented using math.Inf(1).
//   - The algorithm runs in O(V³) time and O(V²) space.
//   - A negative cycle is detected if any diagonal entry becomes negative.
//
// -----------------------------------------------------------------------------
package floydwarshall

import "math"

// FloydWarshall computes the shortest path distances between all pairs of
// vertices in a weighted graph using the Floyd–Warshall algorithm.
//
// The input adjacencyMatrix must be a square matrix where:
//   - adjacencyMatrix[i][j] represents the weight of the edge from vertex i to j.
//   - math.Inf(1) represents the absence of a direct edge.
//
// The function returns a new matrix containing the shortest distances between
// all vertex pairs. If a negative cycle is detected, the function panics, as
// shortest paths are undefined in that case.
func FloydWarshall(matrix [][]float64) [][]float64 {
	// Reject a nil matrix outright.
	if matrix == nil {
		return [][]float64{}
	}

	numberOfVertices := len(matrix)

	// Handle empty graph explicitly.
	if numberOfVertices == 0 {
		return [][]float64{}
	}

	infinity := math.Inf(1)

	// Validate that the adjacency matrix is well-formed.
	// Each row must exist and have the same length as the number of vertices.
	for rowIndex := 0; rowIndex < numberOfVertices; rowIndex++ {
		// Panic if a row is nil.
		// A nil row prevents safe indexing and indicates a malformed matrix.
		if matrix[rowIndex] == nil {
			panic("floyd-warshall nil row in adjacency matrix")
		}

		// Panic if the matrix is not square.
		// Floyd–Warshall requires a V×V adjacency matrix.
		if len(matrix[rowIndex]) != numberOfVertices {
			panic("floyd-warshall adjacency matrix must be square")
		}
	}

	// Create a deep copy of the input matrix so it is not mutated.
	shortestDistanceMatrix := make([][]float64, numberOfVertices)

	for source := 0; source < numberOfVertices; source++ {
		shortestDistanceMatrix[source] = make([]float64, numberOfVertices)

		for destination := 0; destination < numberOfVertices; destination++ {
			shortestDistanceMatrix[source][destination] = matrix[source][destination]
		}

		// Ensure zero distance from a vertex to itself unless explicitly negative.
		if shortestDistanceMatrix[source][source] > 0 {
			shortestDistanceMatrix[source][source] = 0
		}
	}

	// Core Floyd–Warshall dynamic programming loops.
	for intermediateIndex := 0; intermediateIndex < numberOfVertices; intermediateIndex++ {
		for source := 0; source < numberOfVertices; source++ {
			// Skip if source cannot reach the intermediate vertex.
			if shortestDistanceMatrix[source][intermediateIndex] == infinity {
				continue
			}

			for destination := 0; destination < numberOfVertices; destination++ {
				// Skip if intermediate cannot reach the destination vertex.
				if shortestDistanceMatrix[intermediateIndex][destination] == infinity {
					continue
				}

				distance := shortestDistanceMatrix[source][intermediateIndex] +
					shortestDistanceMatrix[intermediateIndex][destination]

				if distance < shortestDistanceMatrix[source][destination] {
					shortestDistanceMatrix[source][destination] = distance
				}
			}
		}
	}

	// Negative cycle detection: distance from a vertex to itself should never be negative.
	for vertexIndex := 0; vertexIndex < numberOfVertices; vertexIndex++ {
		if shortestDistanceMatrix[vertexIndex][vertexIndex] < 0 {
			panic("floyd-warshall negative cycle detected")
		}
	}

	return shortestDistanceMatrix
}
