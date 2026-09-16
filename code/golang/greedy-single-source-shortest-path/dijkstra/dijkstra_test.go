// -----------------------------------------------------------------------------
// Package: dijkstra
// File: dijkstra_test.go
//
// Description:
//
//	This file contains unit tests for the Dijkstra single-source shortest
//	path algorithm implementation.
//
//	The tests construct small directed graphs with known shortest paths
//	and verify that the computed distances match expected results.
//	Each test follows a clear Arrange / Act / Assert structure to
//	improve readability and diagnostic clarity.
//
// Notes:
//   - Tests assume non-negative edge weights.
//   - Graphs are intentionally small and deterministic.
//   - Map iteration order is not relied upon in assertions.
//
// -----------------------------------------------------------------------------
package dijkstra

import (
	"math"
	"testing"
)

// TestDijkstraComputesCorrectShortestDistances verifies that Dijkstra’s
// algorithm computes correct shortest distances for a simple directed graph.
func TestDijkstraComputesCorrectShortestDistances(test *testing.T) {
	// Arrange.
	graph := NewGraph()

	graph.AddDirectedEdge(0, 1, 4)
	graph.AddDirectedEdge(0, 2, 1)
	graph.AddDirectedEdge(2, 1, 2)

	vertex := 0

	expectedDistances := map[int]int{
		0: 0,
		1: 3,
		2: 1,
	}

	// Act.
	computedDistances := Dijkstra(graph, vertex)

	// Assert.
	if computedDistances == nil {
		test.Fatalf("Expected non-nil distance map, got nil.")
	}

	if len(computedDistances) != len(expectedDistances) {
		test.Fatalf("Expected %d distances, got %d.",
			len(expectedDistances),
			len(computedDistances),
		)
	}

	for expectedVertex, expectedDistance := range expectedDistances {
		computedDistance, exists := computedDistances[expectedVertex]

		if !exists {
			test.Fatalf("Expected distance for vertex %d not found.",
				expectedVertex,
			)
		}

		if computedDistance != expectedDistance {
			test.Fatalf("Expected distance %d for vertex %d, got %d.",
				expectedDistance,
				expectedVertex,
				computedDistance,
			)
		}
	}
}

// TestDijkstraReturnsNilForNilGraph verifies that Dijkstra’s algorithm
// defensively returns nil when provided a nil graph.
func TestDijkstraReturnsNilForNilGraph(test *testing.T) {
	// Arrange.
	var graph *Graph = nil

	vertex := 0

	// Act.
	computedDistances := Dijkstra(graph, vertex)

	// Assert.
	if computedDistances != nil {
		test.Fatalf("Expected nil result for nil graph input.")
	}
}

// TestDijkstraReturnsNilForMissingStartVertex verifies that Dijkstra’s
// algorithm returns nil when the start vertex does not exist in the graph.
func TestDijkstraReturnsNilForMissingStartVertex(test *testing.T) {
	// Arrange.
	graph := NewGraph()

	graph.AddDirectedEdge(1, 2, 3)
	graph.AddDirectedEdge(2, 3, 4)

	nonExistentVertex := 0

	// Act.
	computedDistances := Dijkstra(graph, nonExistentVertex)

	// Assert.
	if computedDistances != nil {
		test.Fatalf("Expected nil result for missing start vertex %d.",
			nonExistentVertex,
		)
	}
}

// TestDijkstraHandlesSingleVertexGraph verifies correct behavior when
// the graph contains only a single vertex with no edges.
func TestDijkstraHandlesSingleVertexGraph(test *testing.T) {
	// Arrange.
	graph := NewGraph()

	graph.AddDirectedEdge(0, 0, 0)

	vertex := 0

	// Act.
	computedDistances := Dijkstra(graph, vertex)

	// Assert.
	if computedDistances == nil {
		test.Fatalf("Expected non-nil distance map for single-vertex graph.")
	}

	distanceToSelf, exists := computedDistances[vertex]
	if !exists {
		test.Fatalf("Expected distance entry for start vertex.")
	}

	if distanceToSelf != 0 {
		test.Fatalf("Expected distance 0 for start vertex, got %d.",
			distanceToSelf,
		)
	}
}

// TestDijkstraHandlesUnreachableVertex verifies that vertices which are
// present in the graph but unreachable from the start vertex retain
// an infinite distance.
func TestDijkstraHandlesUnreachableVertex(test *testing.T) {
	// Arrange.
	graph := NewGraph()

	graph.AddDirectedEdge(0, 1, 2)
	graph.AddDirectedEdge(2, 3, 4)

	vertex := 0

	// Act.
	computedDistances := Dijkstra(graph, vertex)

	// Assert.
	if computedDistances == nil {
		test.Fatalf("Expected non-nil distance map, got nil.")
	}

	distanceToUnreachableVertex, exists := computedDistances[2]
	if !exists {
		test.Fatalf("Expected distance entry for unreachable vertex.")
	}

	if distanceToUnreachableVertex != math.MaxInt {
		test.Fatalf("Expected infinite distance for unreachable vertex, got %d.",
			distanceToUnreachableVertex,
		)
	}
}

// TestDijkstraPrefersShorterIndirectPath verifies that the algorithm
// prefers a shorter indirect path over a longer direct edge.
func TestDijkstraPrefersShorterIndirectPath(test *testing.T) {
	// Arrange.
	graph := NewGraph()

	graph.AddDirectedEdge(0, 1, 10)
	graph.AddDirectedEdge(0, 2, 3)
	graph.AddDirectedEdge(2, 1, 2)

	vertex := 0

	expectedDistance := 5

	// Act.
	computedDistances := Dijkstra(graph, vertex)

	// Assert.
	if computedDistances == nil {
		test.Fatalf("Expected non-nil distance map, got nil.")
	}

	computedDistance, exists := computedDistances[1]
	if !exists {
		test.Fatalf("Expected distance entry for vertex 1.")
	}

	if computedDistance != expectedDistance {
		test.Fatalf("Expected distance %d for vertex 1, got %d.",
			expectedDistance,
			computedDistance,
		)
	}
}

// TestDijkstraHandlesZeroWeightEdges verifies that zero-weight edges
// are handled correctly by the algorithm.
func TestDijkstraHandlesZeroWeightEdges(test *testing.T) {
	// Arrange.
	graph := NewGraph()

	graph.AddDirectedEdge(0, 1, 0)
	graph.AddDirectedEdge(1, 2, 0)

	vertex := 0

	expectedDistances := map[int]int{
		0: 0,
		1: 0,
		2: 0,
	}

	// Act.
	computedDistances := Dijkstra(graph, vertex)

	// Assert.
	if computedDistances == nil {
		test.Fatalf("Expected non-nil distance map, got nil.")
	}

	for expectedVertex, expectedDistance := range expectedDistances {
		computedDistance, exists := computedDistances[expectedVertex]

		if !exists {
			test.Fatalf("Expected distance entry for vertex %d.",
				expectedVertex,
			)
		}

		if computedDistance != expectedDistance {
			test.Fatalf("Expected distance %d for vertex %d, got %d.",
				expectedDistance,
				expectedVertex,
				computedDistance,
			)
		}
	}
}

// TestDijkstraIgnoresNegativeWeightEdges verifies that edges with
// negative weights are ignored during graph construction.
func TestDijkstraIgnoresNegativeWeightEdges(test *testing.T) {
	// Arrange.
	graph := NewGraph()

	graph.AddDirectedEdge(0, 1, -5)
	graph.AddDirectedEdge(0, 2, 3)

	vertex := 0

	// Act.
	computedDistances := Dijkstra(graph, vertex)

	// Assert.
	if computedDistances == nil {
		test.Fatalf("Expected non-nil distance map, got nil.")
	}

	if _, exists := computedDistances[1]; exists {
		test.Fatalf("Expected vertex 1 to be unreachable due to negative edge.")
	}

	distanceToVertexTwo, exists := computedDistances[2]

	if !exists {
		test.Fatalf("Expected distance entry for vertex 2.")
	}

	if distanceToVertexTwo != 3 {
		test.Fatalf("Expected distance 3 for vertex 2, got %d.",
			distanceToVertexTwo,
		)
	}
}

// TestDijkstraHandlesCyclicGraph verifies that the algorithm
// terminates correctly and computes valid distances in a graph
// containing cycles.
func TestDijkstraHandlesCyclicGraph(test *testing.T) {
	// Arrange.
	graph := NewGraph()

	graph.AddDirectedEdge(0, 1, 1)
	graph.AddDirectedEdge(1, 2, 1)
	graph.AddDirectedEdge(2, 0, 1)
	graph.AddDirectedEdge(1, 3, 2)

	vertex := 0

	expectedDistances := map[int]int{
		0: 0,
		1: 1,
		2: 2,
		3: 3,
	}

	// Act.
	computedDistances := Dijkstra(graph, vertex)

	// Assert.
	if computedDistances == nil {
		test.Fatalf("Expected non-nil distance map, got nil.")
	}

	for expectedVertex, expectedDistance := range expectedDistances {
		computedDistance, exists := computedDistances[expectedVertex]

		if !exists {
			test.Fatalf("Expected distance entry for vertex %d.",
				expectedVertex,
			)
		}

		if computedDistance != expectedDistance {
			test.Fatalf("Expected distance %d for vertex %d, got %d.",
				expectedDistance,
				expectedVertex,
				computedDistance,
			)
		}
	}
}
