// -----------------------------------------------------------------------------
// Package: floydwarshall
// File: floyd_warshall_test.go
//
// Description:
//
//	This file contains unit tests for the FloydWarshall algorithm.
//	The tests validate correctness of all-pairs shortest-path computation
//	across a variety of graph configurations, including:
//
//	  - Basic graphs with direct and indirect paths.
//	  - Graphs with unreachable vertices.
//	  - Single-vertex and empty graphs.
//	  - Graphs containing negative edge weights (without negative cycles).
//
//	The test suite emphasizes clarity and correctness rather than performance,
//	and uses explicit Arrange / Act / Assert structure throughout.
//
// Notes:
//   - All tests use the testing parameter name "test".
//   - Distance matrices use a large sentinel value to represent infinity.
//   - The input graph is assumed to contain no negative-weight cycles.
//
// -----------------------------------------------------------------------------
package floydwarshall

import "testing"

// TestFloydWarshall_BasicGraph verifies that indirect paths
// are correctly discovered and preferred over longer direct edges.
func TestFloydWarshall_BasicGraph(test *testing.T) {
	// Arrange.
	infinityDistance := 1 << 30

	graph := [][]int{
		{0, 3, infinityDistance, 7},
		{8, 0, 2, infinityDistance},
		{5, infinityDistance, 0, 1},
		{2, infinityDistance, infinityDistance, 0},
	}

	expectedShortestDistances := [][]int{
		{0, 3, 5, 6},
		{5, 0, 2, 3},
		{3, 6, 0, 1},
		{2, 5, 7, 0},
	}

	// Act.
	actualShortestDistances := FloydWarshall(graph)

	// Assert.
	for sourceVertex := 0; sourceVertex < len(expectedShortestDistances); sourceVertex++ {
		for destinationVertex := 0; destinationVertex < len(expectedShortestDistances); destinationVertex++ {
			expectedDistance := expectedShortestDistances[sourceVertex][destinationVertex]
			actualDistance := actualShortestDistances[sourceVertex][destinationVertex]

			if actualDistance != expectedDistance {
				test.Fatalf("distance mismatch from %d to %d: expected %d, got %d",
					sourceVertex,
					destinationVertex,
					expectedDistance,
					actualDistance,
				)
			}
		}
	}
}

// TestFloydWarshall_UnreachableVertices verifies that unreachable
// vertex pairs remain marked as infinityDistance.
func TestFloydWarshall_UnreachableVertices(test *testing.T) {
	// Arrange.
	infinityDistance := 1 << 30

	graph := [][]int{
		{0, 1, infinityDistance},
		{infinityDistance, 0, infinityDistance},
		{infinityDistance, infinityDistance, 0},
	}

	// Act.
	shortestDistances := FloydWarshall(graph)

	// Assert.
	if shortestDistances[0][2] != infinityDistance {
		test.Fatalf("expected vertex 0 to be unreachable from vertex 2")
	}

	if shortestDistances[1][2] != infinityDistance {
		test.Fatalf("expected vertex 1 to be unreachable from vertex 2")
	}
}

// TestFloydWarshall_SingleVertex verifies that a graph with a
// single vertex produces a zero-distance self-loop.
func TestFloydWarshall_SingleVertex(test *testing.T) {
	// Arrange.
	graph := [][]int{
		{0},
	}

	// Act.
	shortestDistances := FloydWarshall(graph)

	// Assert.
	if shortestDistances[0][0] != 0 {
		test.Fatalf("expected distance from vertex to itself to be 0, got %d",
			shortestDistances[0][0],
		)
	}
}

// TestFloydWarshall_EmptyGraph verifies that an empty graph
// returns a nil distance matrix.
func TestFloydWarshall_EmptyGraph(test *testing.T) {
	// Arrange.
	var graph [][]int

	// Act.
	shortestDistances := FloydWarshall(graph)

	// Assert.
	if shortestDistances != nil {
		test.Fatalf("expected nil result for empty graph")
	}
}

// TestFloydWarshall_NegativeEdgeWeights verifies that the algorithm
// correctly handles negative edge weights in the absence of
// negative-weight cycles.
func TestFloydWarshall_NegativeEdgeWeights(test *testing.T) {
	// Arrange.
	infinityDistance := 1 << 30

	graph := [][]int{
		{0, 1, infinityDistance},
		{infinityDistance, 0, -2},
		{4, infinityDistance, 0},
	}

	expectedShortestDistances := [][]int{
		{0, 1, -1},
		{2, 0, -2},
		{4, 5, 0},
	}

	// Act.
	actualShortestDistances := FloydWarshall(graph)

	// Assert.
	for sourceVertex := 0; sourceVertex < len(expectedShortestDistances); sourceVertex++ {
		for destinationVertex := 0; destinationVertex < len(expectedShortestDistances); destinationVertex++ {
			expectedDistance := expectedShortestDistances[sourceVertex][destinationVertex]
			actualDistance := actualShortestDistances[sourceVertex][destinationVertex]

			if actualDistance != expectedDistance {
				test.Fatalf("distance mismatch from %d to %d: expected %d, got %d",
					sourceVertex,
					destinationVertex,
					expectedDistance,
					actualDistance,
				)
			}
		}
	}
}

// TestFloydWarshall_InputImmutability verifies that the input
// distance matrix is not modified by the algorithm.
func TestFloydWarshall_InputImmutability(test *testing.T) {
	// Arrange.
	infinityDistance := 1 << 30

	originalGraph := [][]int{
		{0, 2, infinityDistance},
		{infinityDistance, 0, 3},
		{1, infinityDistance, 0},
	}

	graphCopy := CopyDistanceMatrix(originalGraph)

	// Act.
	_ = FloydWarshall(originalGraph)

	// Assert.
	for rowIndex := 0; rowIndex < len(originalGraph); rowIndex++ {
		for columnIndex := 0; columnIndex < len(originalGraph[rowIndex]); columnIndex++ {
			if originalGraph[rowIndex][columnIndex] != graphCopy[rowIndex][columnIndex] {
				test.Fatalf("input matrix modified at [%d][%d]",
					rowIndex,
					columnIndex,
				)
			}
		}
	}
}

// TestFloydWarshall_AlreadyOptimalGraph verifies that a graph
// whose direct edges are already optimal remains unchanged.
func TestFloydWarshall_AlreadyOptimalGraph(test *testing.T) {
	// Arrange.
	infinityDistance := 1 << 30

	graph := [][]int{
		{0, 1, 3},
		{infinityDistance, 0, 2},
		{infinityDistance, infinityDistance, 0},
	}

	expectedShortestDistances := CopyDistanceMatrix(graph)

	// Act.
	actualShortestDistances := FloydWarshall(graph)

	// Assert.
	for sourceVertex := 0; sourceVertex < len(graph); sourceVertex++ {
		for destinationVertex := 0; destinationVertex < len(graph); destinationVertex++ {
			if actualShortestDistances[sourceVertex][destinationVertex] !=
				expectedShortestDistances[sourceVertex][destinationVertex] {

				test.Fatalf("unexpected distance change from %d to %d",
					sourceVertex,
					destinationVertex,
				)
			}
		}
	}
}

// TestFloydWarshall_SymmetricGraph verifies correct behavior
// when the input graph is undirected (symmetric matrix).
func TestFloydWarshall_SymmetricGraph(test *testing.T) {
	// Arrange.
	infinityDistance := 1 << 30

	graph := [][]int{
		{0, 4, infinityDistance},
		{4, 0, 1},
		{infinityDistance, 1, 0},
	}

	expectedShortestDistances := [][]int{
		{0, 4, 5},
		{4, 0, 1},
		{5, 1, 0},
	}

	// Act.
	actualShortestDistances := FloydWarshall(graph)

	// Assert.
	for sourceVertex := 0; sourceVertex < len(graph); sourceVertex++ {
		for destinationVertex := 0; destinationVertex < len(graph); destinationVertex++ {
			if actualShortestDistances[sourceVertex][destinationVertex] !=
				expectedShortestDistances[sourceVertex][destinationVertex] {

				test.Fatalf("distance mismatch from %d to %d: expected %d, got %d",
					sourceVertex,
					destinationVertex,
					expectedShortestDistances[sourceVertex][destinationVertex],
					actualShortestDistances[sourceVertex][destinationVertex],
				)
			}
		}
	}
}

// TestFloydWarshall_MultiStepImprovement verifies that shortest
// paths requiring multiple intermediate vertices are discovered.
func TestFloydWarshall_MultiStepImprovement(test *testing.T) {
	// Arrange.
	infinityDistance := 1 << 30

	graph := [][]int{
		{0, 1, infinityDistance, infinityDistance},
		{infinityDistance, 0, 1, infinityDistance},
		{infinityDistance, infinityDistance, 0, 1},
		{infinityDistance, infinityDistance, infinityDistance, 0},
	}

	// Act.
	shortestDistances := FloydWarshall(graph)

	// Assert.
	if shortestDistances[0][3] != 3 {
		test.Fatalf("expected distance from 0 to 3 to be 3, got %d",
			shortestDistances[0][3],
		)
	}
}

// TestFloydWarshall_DiagonalStability verifies that diagonal
// distances remain zero for graphs without negative cycles.
func TestFloydWarshall_DiagonalStability(test *testing.T) {
	// Arrange.
	infinityDistance := 1 << 30

	graph := [][]int{
		{0, 2, infinityDistance},
		{infinityDistance, 0, 3},
		{1, infinityDistance, 0},
	}

	// Act.
	shortestDistances := FloydWarshall(graph)

	// Assert.
	for vertexIndex := 0; vertexIndex < len(graph); vertexIndex++ {
		if shortestDistances[vertexIndex][vertexIndex] != 0 {
			test.Fatalf("expected diagonal at [%d][%d] to remain 0, got %d",
				vertexIndex,
				vertexIndex,
				shortestDistances[vertexIndex][vertexIndex],
			)
		}
	}
}

// TestFloydWarshall_InfinityPreservation verifies that vertices
// with no connecting path remain unreachable.
func TestFloydWarshall_InfinityPreservation(test *testing.T) {
	// Arrange.
	infinityDistance := 1 << 30

	graph := [][]int{
		{0, infinityDistance},
		{infinityDistance, 0},
	}

	// Act.
	shortestDistances := FloydWarshall(graph)

	// Assert.
	if shortestDistances[0][1] != infinityDistance {
		test.Fatalf("expected vertex 1 to remain unreachable from vertex 0")
	}

	if shortestDistances[1][0] != infinityDistance {
		test.Fatalf("expected vertex 0 to remain unreachable from vertex 1")
	}
}

// TestFloydWarshall_ZeroWeightEdges verifies that zero-weight
// edges are handled correctly during relaxation.
func TestFloydWarshall_ZeroWeightEdges(test *testing.T) {
	// Arrange.
	infinityDistance := 1 << 30

	graph := [][]int{
		{0, 0, infinityDistance},
		{infinityDistance, 0, 0},
		{5, infinityDistance, 0},
	}

	// Act.
	shortestDistances := FloydWarshall(graph)

	// Assert.
	if shortestDistances[0][2] != 0 {
		test.Fatalf("expected distance from 0 to 2 to be 0, got %d",
			shortestDistances[0][2],
		)
	}
}
