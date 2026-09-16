// -----------------------------------------------------------------------------
// Package: floydwarshall
// File: floyd_warshall.go
//
// Description:
//
//	This file provides an implementation of the Floyd–Warshall algorithm for
//	computing all-pairs shortest paths in a weighted directed graph.
//
//	The algorithm operates on an adjacency matrix representation of the graph
//	and incrementally improves shortest-path estimates by allowing each vertex
//	to act as an intermediate waypoint between every other pair of vertices.
//
//	Floyd–Warshall supports negative edge weights but assumes that the graph
//	contains no negative-weight cycles.
//
// Notes:
//   - The graph is represented as a square distance matrix.
//   - A large sentinel value is used to represent the absence of an edge.
//   - The input matrix is not mutated; a deep copy is produced.
//   - The runtime complexity is O(V³), where V is the number of vertices.
//
// -----------------------------------------------------------------------------
package floydwarshall

import "math"

// infinityDistance represents the absence of a direct edge between vertices.
// math.MaxInt / 2 is used to prevent integer overflow during relaxation.
const infinityDistance = math.MaxInt / 2

// CopyDistanceMatrix creates a deep copy of a square distance matrix.
// This ensures the original graph representation is not modified by the
// Floyd–Warshall algorithm.
func CopyDistanceMatrix(matrix [][]int) [][]int {
	// Guard against nil input.
	if matrix == nil {
		return nil
	}

	// Calculate the length of the matrix.
	count := len(matrix)

	// Initialize the copied matrix.
	copiedMatrix := make([][]int, count)

	for index := 0; index < count; index++ {
		copiedMatrix[index] = make([]int, count)
		copy(copiedMatrix[index], matrix[index])
	}

	return copiedMatrix
}

// FloydWarshall computes the shortest distances between all pairs of vertices
// in a weighted directed graph using the Floyd–Warshall algorithm.
//
// The input graph is provided as an adjacency matrix, where graph[i][j]
// represents the weight of the edge from vertex i to vertex j.
// If no edge exists, the value should be infinityDistance.
//
// The function returns a new matrix containing the shortest distances
// between all vertex pairs.
func FloydWarshall(graph [][]int) [][]int {
	// Guard against nil input.
	if graph == nil {
		return nil
	}

	count := len(graph)

	// Guard: empty graph yields no distances.
	if count == 0 {
		return [][]int{}
	}

	// Create a working copy of the distance matrix.
	matrix := CopyDistanceMatrix(graph)

	// Allow each vertex to act as an intermediate vertex in turn.
	for intermediate := 0; intermediate < count; intermediate++ {

		// Iterate over all possible start vertices.
		for source := 0; source < count; source++ {

			// Skip unreachable intermediate paths early.
			if matrix[source][intermediate] == infinityDistance {
				continue
			}

			// Iterate over all possible destination vertices.
			for destination := 0; destination < count; destination++ {

				// Skip unreachable paths from the intermediate vertex.
				if matrix[intermediate][destination] == infinityDistance {
					continue
				}

				// Compute the distance via the intermediate vertex.
				candidateDistance := matrix[source][intermediate] +
					matrix[intermediate][destination]

				// Relax the edge if a shorter path is discovered.
				if candidateDistance < matrix[source][destination] {
					matrix[source][destination] = candidateDistance
				}
			}
		}
	}

	return matrix
}
