// -----------------------------------------------------------------------------
// Package: floydwarshall
// File: floyd_warshall_test.go
//
// Description:
//
//	This file provides unit tests for the Floyd–Warshall all-pairs shortest
//	path algorithm implementation. The tests validate correctness across
//	common scenarios, including basic graphs, unreachable vertices, and
//	defensive input handling.
//
//	Each test follows a strict Arrange / Act / Assert structure to ensure
//	clarity of intent and ease of maintenance.
//
// Notes:
//   - Tests use math.Inf(1) to represent unreachable edges.
//   - The testing parameter name is intentionally "test" for clarity.
//   - Variable names favor explicit meaning over brevity.
//   - Floating-point comparisons use exact values where safe.
//
// -----------------------------------------------------------------------------
package floydwarshall

import (
	"math"
	"testing"
)

// TestFloydWarshall_BasicConnectedGraph verifies shortest paths in a small,
// connected weighted graph.
func TestFloydWarshall_BasicConnectedGraph(test *testing.T) {
	// Arrange.
	infinity := math.Inf(1)

	adjacencyList := [][]float64{
		{0, 3, infinity, 7},
		{8, 0, 2, infinity},
		{5, infinity, 0, 1},
		{2, infinity, infinity, 0},
	}

	expectedShortestDistances := [][]float64{
		{0, 3, 5, 6},
		{5, 0, 2, 3},
		{3, 6, 0, 1},
		{2, 5, 7, 0},
	}

	// Act.
	shortestDistances := FloydWarshall(adjacencyList)

	// Assert.
	for source := 0; source < len(shortestDistances); source++ {
		for destination := 0; destination < len(shortestDistances[source]); destination++ {
			if shortestDistances[source][destination] != expectedShortestDistances[source][destination] {
				test.Fatalf("incorrect distance from vertex %d to %d: expected %f, got %f",
					source,
					destination,
					expectedShortestDistances[source][destination],
					shortestDistances[source][destination])
			}
		}
	}
}

// TestFloydWarshall_UnreachableVertices verifies that unreachable paths remain
// infinite after algorithm execution.
func TestFloydWarshall_UnreachableVertices(test *testing.T) {
	// Arrange.
	infinity := math.Inf(1)

	adjacencyList := [][]float64{
		{0, 1, infinity},
		{infinity, 0, infinity},
		{infinity, infinity, 0},
	}

	// Act.
	shortestDistances := FloydWarshall(adjacencyList)

	// Assert.
	for source := 0; source < len(shortestDistances); source++ {
		for destination := 0; destination < len(shortestDistances[source]); destination++ {
			// Skip known reachable paths and self-distances.
			if (source == 0 && destination == 1) || source == destination {
				continue
			}

			if shortestDistances[source][destination] != infinity {
				test.Fatalf("expected unreachable path from vertex %d to %d to remain infinite",
					source,
					destination,
				)
			}
		}
	}
}

// TestFloydWarshall_EmptyMatrix verifies that an empty graph returns an empty
// result matrix.
func TestFloydWarshall_EmptyMatrix(test *testing.T) {
	// Arrange.
	adjacencyList := [][]float64{}

	// Act.
	shortestDistances := FloydWarshall(adjacencyList)

	// Assert.
	if len(shortestDistances) != 0 {
		test.Fatalf("expected empty result matrix, got length %d",
			len(shortestDistances))
	}
}

// TestFloydWarshall_NilMatrix verifies that a nil matrix input is handled
// defensively and results in an empty matrix.
func TestFloydWarshall_NilMatrix(test *testing.T) {
	// Arrange.
	var adjacencyList [][]float64

	// Act.
	shortestDistances := FloydWarshall(adjacencyList)

	// Assert.
	if shortestDistances == nil || len(shortestDistances) != 0 {
		test.Fatalf("expected empty non-nil matrix for nil input")
	}
}

// TestFloydWarshall_IntermediatePathDiscovery verifies that shorter paths
// are discovered via intermediate vertices.
func TestFloydWarshall_IntermediatePathDiscovery(test *testing.T) {
	// Arrange.
	infinity := math.Inf(1)

	adjacencyList := [][]float64{
		{0, 5, infinity},
		{infinity, 0, 2},
		{infinity, infinity, 0},
	}

	// Act.
	shortestDistances := FloydWarshall(adjacencyList)

	// Assert.
	if shortestDistances[0][2] != 7 {
		test.Fatalf("expected path from vertex 0 to 2 to have distance 7, got %f",
			shortestDistances[0][2])
	}
}

// TestFloydWarshall_DirectedGraph verifies that directionality is preserved
// in the computed shortest paths.
func TestFloydWarshall_DirectedGraph(test *testing.T) {
	// Arrange.
	infinity := math.Inf(1)

	adjacencyList := [][]float64{
		{0, 1, infinity},
		{infinity, 0, infinity},
		{infinity, 2, 0},
	}

	// Act.
	shortestDistances := FloydWarshall(adjacencyList)

	// Assert.
	if shortestDistances[2][0] != infinity {
		test.Fatalf("expected no path from vertex 2 to 0, got %f",
			shortestDistances[2][0])
	}

	if shortestDistances[0][1] != 1 {
		test.Fatalf("expected path from vertex 0 to 1 to have distance 1, got %f",
			shortestDistances[0][1])
	}
}

// TestFloydWarshall_ZeroWeightEdges verifies that zero-weight edges are
// handled correctly.
func TestFloydWarshall_ZeroWeightEdges(test *testing.T) {
	// Arrange.
	adjacencyList := [][]float64{
		{0, 0, 5},
		{0, 0, 1},
		{5, 1, 0},
	}

	// Act.
	shortestDistances := FloydWarshall(adjacencyList)

	// Assert.
	if shortestDistances[0][2] != 1 {
		test.Fatalf("expected shortest path from vertex 0 to 2 to be 1, got %f",
			shortestDistances[0][2])
	}
}

// TestFloydWarshall_NegativeEdgesWithoutCycle verifies that negative edge
// weights are handled correctly when no negative cycle exists.
func TestFloydWarshall_NegativeEdgesWithoutCycle(test *testing.T) {
	// Arrange.
	infinity := math.Inf(1)

	adjacencyList := [][]float64{
		{0, -2, infinity},
		{infinity, 0, 3},
		{infinity, infinity, 0},
	}

	// Act.
	shortestDistances := FloydWarshall(adjacencyList)

	// Assert.
	if shortestDistances[0][2] != 1 {
		test.Fatalf("expected shortest path from vertex 0 to 2 to be 1, got %f",
			shortestDistances[0][2])
	}
}

// TestFloydWarshall_NegativeCycle verifies that the algorithm panics when
// a negative cycle is present.
func TestFloydWarshall_NegativeCycle(test *testing.T) {
	// Arrange.
	defer func() {
		if recover() == nil {
			test.Fatalf("expected panic due to negative cycle, but no panic occurred")
		}
	}()

	adjacencyList := [][]float64{
		{0, -1, math.Inf(1)},
		{math.Inf(1), 0, -1},
		{-1, math.Inf(1), 0},
	}

	// Act.
	FloydWarshall(adjacencyList)

	// Assert.
	// Panic is expected before reaching this point.
}
