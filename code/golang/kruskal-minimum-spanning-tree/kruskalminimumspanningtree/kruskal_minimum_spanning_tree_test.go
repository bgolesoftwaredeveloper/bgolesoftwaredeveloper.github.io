// -----------------------------------------------------------------------------
// Package: kruskalminimumspanningtree
// File: kruskal_minimum_spanning_tree_test.go
//
// Description:
//
//	This file contains unit tests for the Kruskal minimum spanning
//	tree implementation. The tests validate correctness, defensive
//	behavior, and edge-case handling under a variety of graph
//	configurations.
//
//	All tests follow a strict Arrange / Act / Assert structure and
//	use explicit, descriptive identifiers to maximize clarity.
//
// -----------------------------------------------------------------------------
package kruskalminimumspanningtree

import "testing"

// TestKruskal_ComputesValidMinimumSpanningTree verifies that Kruskal’s
// algorithm correctly computes a minimum spanning tree for a connected
// undirected weighted graph.
func TestKruskal_ComputesValidMinimumSpanningTree(test *testing.T) {
	// Arrange.
	graph := NewUndirectedWeightedGraph(5)

	graph.AddEdge(0, 1, 4)
	graph.AddEdge(0, 2, 3)
	graph.AddEdge(1, 2, 1)
	graph.AddEdge(1, 3, 2)
	graph.AddEdge(2, 3, 4)
	graph.AddEdge(3, 4, 2)
	graph.AddEdge(2, 4, 5)

	expectedEdgeCount := 4
	expectedTotalWeight := 8

	// Act.
	minimumSpanningTreeEdges, totalTreeWeight := Kruskal(graph)

	// Assert.
	if minimumSpanningTreeEdges == nil {
		test.Fatalf("expected non-nil MST result")
	}

	if len(minimumSpanningTreeEdges) != expectedEdgeCount {
		test.Fatalf("expected %d MST edges, got %d", expectedEdgeCount, len(minimumSpanningTreeEdges))
	}

	if totalTreeWeight != expectedTotalWeight {
		test.Fatalf("expected total weight %d, got %d", expectedTotalWeight, totalTreeWeight)
	}
}

// TestKruskal_ReturnsNilForDisconnectedGraph verifies that Kruskal’s
// algorithm fails gracefully when the input graph is disconnected.
func TestKruskal_ReturnsNilForDisconnectedGraph(test *testing.T) {
	// Arrange.
	graph := NewUndirectedWeightedGraph(4)

	graph.AddEdge(0, 1, 1)
	graph.AddEdge(2, 3, 1)

	// Act.
	minimumSpanningTreeEdges, totalTreeWeight := Kruskal(graph)

	// Assert.
	if minimumSpanningTreeEdges != nil {
		test.Fatalf("expected nil MST for disconnected graph")
	}

	if totalTreeWeight != 0 {
		test.Fatalf("expected total weight 0 for disconnected graph, got %d", totalTreeWeight)
	}
}

// TestKruskal_ReturnsNilForNilGraph verifies that Kruskal’s algorithm
// safely handles a nil graph pointer.
func TestKruskal_ReturnsNilForNilGraph(test *testing.T) {
	// Arrange.
	var graph *UndirectedWeightedGraph = nil

	// Act.
	minimumSpanningTreeEdges, totalTreeWeight := Kruskal(graph)

	// Assert.
	if minimumSpanningTreeEdges != nil {
		test.Fatalf("expected nil MST for nil graph input")
	}

	if totalTreeWeight != 0 {
		test.Fatalf("expected total weight 0 for nil graph input, got %d", totalTreeWeight)
	}
}

// TestKruskal_ReturnsNilForGraphWithNoVertices verifies that Kruskal’s
// algorithm does not produce an MST for a graph with zero vertices.
func TestKruskal_ReturnsNilForGraphWithNoVertices(test *testing.T) {
	// Arrange.
	graph := NewUndirectedWeightedGraph(0)

	// Act.
	minimumSpanningTreeEdges, totalTreeWeight := Kruskal(graph)

	// Assert.
	if minimumSpanningTreeEdges != nil {
		test.Fatalf("expected nil MST for empty graph")
	}

	if totalTreeWeight != 0 {
		test.Fatalf("expected total weight 0 for empty graph, got %d", totalTreeWeight)
	}
}

// TestAddEdge_RejectsInvalidEdges verifies that AddEdge defensively
// rejects invalid vertex indices and negative weights.
func TestAddEdge_RejectsInvalidEdges(test *testing.T) {
	// Arrange.
	graph := NewUndirectedWeightedGraph(3)

	// Act.
	addNegativeWeight := graph.AddEdge(0, 1, -1)
	addInvalidStart := graph.AddEdge(-1, 1, 1)
	addInvalidEnd := graph.AddEdge(0, 5, 1)

	// Assert.
	if addNegativeWeight {
		test.Fatalf("expected AddEdge to reject negative weight")
	}

	if addInvalidStart {
		test.Fatalf("expected AddEdge to reject invalid start vertex")
	}

	if addInvalidEnd {
		test.Fatalf("expected AddEdge to reject invalid end vertex")
	}

	if len(graph.edges) != 0 {
		test.Fatalf("expected graph to contain 0 edges, got %d", len(graph.edges))
	}
}

// TestKruskal_ComputesMSTForSingleVertexGraph verifies that a graph
// containing a single vertex produces a valid empty MST.
func TestKruskal_ComputesMSTForSingleVertexGraph(test *testing.T) {
	// Arrange.
	graph := NewUndirectedWeightedGraph(1)

	// Act.
	minimumSpanningTreeEdges, totalTreeWeight := Kruskal(graph)

	// Assert.
	if minimumSpanningTreeEdges == nil {
		test.Fatalf("expected non-nil MST result for single vertex graph")
	}

	if len(minimumSpanningTreeEdges) != 0 {
		test.Fatalf("expected 0 MST edges for single vertex graph, got %d", len(minimumSpanningTreeEdges))
	}

	if totalTreeWeight != 0 {
		test.Fatalf("expected total weight 0 for single vertex graph, got %d", totalTreeWeight)
	}
}

// TestKruskal_HandlesDuplicateEdges verifies that duplicate edges
// do not cause incorrect MST construction.
func TestKruskal_HandlesDuplicateEdges(test *testing.T) {
	// Arrange.
	graph := NewUndirectedWeightedGraph(3)

	graph.AddEdge(0, 1, 1)
	graph.AddEdge(0, 1, 1)
	graph.AddEdge(1, 2, 2)
	graph.AddEdge(0, 2, 3)

	expectedEdgeCount := 2
	expectedTotalWeight := 3

	// Act.
	minimumSpanningTreeEdges, totalTreeWeight := Kruskal(graph)

	// Assert.
	if minimumSpanningTreeEdges == nil {
		test.Fatalf("expected non-nil MST result for graph with duplicate edges")
	}

	if len(minimumSpanningTreeEdges) != expectedEdgeCount {
		test.Fatalf("expected %d MST edges, got %d", expectedEdgeCount, len(minimumSpanningTreeEdges))
	}

	if totalTreeWeight != expectedTotalWeight {
		test.Fatalf("expected total weight %d, got %d", expectedTotalWeight, totalTreeWeight)
	}
}

// TestKruskal_ChoosesLowerWeightAmongParallelEdges verifies that
// Kruskal’s algorithm selects the minimum-weight edge when parallel
// edges exist between the same vertices.
func TestKruskal_ChoosesLowerWeightAmongParallelEdges(test *testing.T) {
	// Arrange.
	graph := NewUndirectedWeightedGraph(2)

	graph.AddEdge(0, 1, 10)
	graph.AddEdge(0, 1, 1)

	expectedEdgeCount := 1
	expectedTotalWeight := 1

	// Act.
	minimumSpanningTreeEdges, totalTreeWeight := Kruskal(graph)

	// Assert.
	if minimumSpanningTreeEdges == nil {
		test.Fatalf("expected non-nil MST result for parallel edge graph")
	}

	if len(minimumSpanningTreeEdges) != expectedEdgeCount {
		test.Fatalf("expected %d MST edge, got %d", expectedEdgeCount, len(minimumSpanningTreeEdges))
	}

	if totalTreeWeight != expectedTotalWeight {
		test.Fatalf("expected total weight %d, got %d", expectedTotalWeight, totalTreeWeight)
	}
}

// TestKruskal_ProducesAcyclicResult verifies that the resulting MST
// contains no cycles by ensuring no more than V-1 edges are selected.
func TestKruskal_ProducesAcyclicResult(test *testing.T) {
	// Arrange.
	graph := NewUndirectedWeightedGraph(4)

	graph.AddEdge(0, 1, 1)
	graph.AddEdge(1, 2, 1)
	graph.AddEdge(2, 3, 1)
	graph.AddEdge(3, 0, 1)
	graph.AddEdge(0, 2, 2)

	expectedMaximumEdges := 3

	// Act.
	minimumSpanningTreeEdges, _ := Kruskal(graph)

	// Assert.
	if minimumSpanningTreeEdges == nil {
		test.Fatalf("expected non-nil MST result for cyclic graph")
	}

	if len(minimumSpanningTreeEdges) > expectedMaximumEdges {
		test.Fatalf("expected at most %d MST edges, got %d", expectedMaximumEdges, len(minimumSpanningTreeEdges))
	}
}

// TestKruskal_IgnoresSelfLoops verifies that self-loop edges do not
// affect the correctness of the MST.
func TestKruskal_IgnoresSelfLoops(test *testing.T) {
	// Arrange.
	graph := NewUndirectedWeightedGraph(3)

	graph.AddEdge(0, 0, 1)
	graph.AddEdge(0, 1, 2)
	graph.AddEdge(1, 2, 3)

	expectedEdgeCount := 2
	expectedTotalWeight := 5

	// Act.
	minimumSpanningTreeEdges, totalTreeWeight := Kruskal(graph)

	// Assert.
	if minimumSpanningTreeEdges == nil {
		test.Fatalf("expected non-nil MST result when self-loops are present")
	}

	if len(minimumSpanningTreeEdges) != expectedEdgeCount {
		test.Fatalf("expected %d MST edges, got %d", expectedEdgeCount, len(minimumSpanningTreeEdges))
	}

	if totalTreeWeight != expectedTotalWeight {
		test.Fatalf("expected total weight %d, got %d", expectedTotalWeight, totalTreeWeight)
	}
}
