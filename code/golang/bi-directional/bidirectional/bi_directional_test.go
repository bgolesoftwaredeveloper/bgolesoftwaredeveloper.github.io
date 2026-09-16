// -----------------------------------------------------------------------------
// Package: bidirectional
// File: graph_test.go
//
// Description:
//
//		This file contains unit tests for the bidirectional graph implementation.
//		The tests validate correct behavior for:
//
//	  - Graph construction
//	  - Vertex existence checks
//	  - Undirected edge insertion and removal
//	  - Neighbor enumeration
//	  - Vertex removal
//	  - Breadth-first traversal
//
//		Each test follows a strict Arrange / Act / Assert structure to ensure
//		clarity, consistency, and debuggability.
//
// Notes:
//   - Map iteration order in Go is non-deterministic; tests account for this
//     by validating set membership rather than ordering where appropriate.
//   - All tests assume defensive behavior for nil receivers and invalid input.
//
// -----------------------------------------------------------------------------
package bidirectional

import "testing"

// TestNewGraphInitializesEmptyGraph verifies that NewGraph returns a
// non-nil graph with an initialized adjacency list.
func TestNewGraphInitializesEmptyGraph(test *testing.T) {
	// Arrange.
	graph := NewGraph()

	// Act.
	graphIsNil := graph == nil
	adjacencyListIsNil := graph.adjacencyList == nil
	adjacencyListLength := len(graph.adjacencyList)

	// Assert.
	if graphIsNil {
		test.Fatalf("expected graph to be non-nil")
	}

	if adjacencyListIsNil {
		test.Fatalf("expected adjacency list to be initialized")
	}

	if adjacencyListLength != 0 {
		test.Fatalf("expected empty adjacency list, got %d entries", adjacencyListLength)
	}
}

// TestHasVertexReportsCorrectExistence verifies that HasVertex correctly
// reports whether a vertex exists.
func TestHasVertexReportsCorrectExistence(test *testing.T) {
	// Arrange.
	graph := NewGraph()

	graph.AddUndirectedEdge("A", "B")

	// Act.
	hasVertexA := graph.HasVertex("A")
	hasVertexB := graph.HasVertex("B")
	hasVertexZ := graph.HasVertex("Z")

	// Assert.
	if !hasVertexA {
		test.Errorf("expected vertex A to exist")
	}

	if !hasVertexB {
		test.Errorf("expected vertex B to exist")
	}

	if hasVertexZ {
		test.Errorf("expected vertex Z to not exist")
	}
}

// TestAddUndirectedEdgeCreatesSymmetricAdjacency verifies that adding an
// undirected edge creates symmetric adjacency entries.
func TestAddUndirectedEdgeCreatesSymmetricAdjacency(test *testing.T) {
	// Arrange.
	graph := NewGraph()

	// Act.
	graph.AddUndirectedEdge("A", "B")

	edgeExistsAB := graph.HasUndirectedEdge("A", "B")
	edgeExistsBA := graph.HasUndirectedEdge("B", "A")

	// Assert.
	if !edgeExistsAB {
		test.Errorf("expected edge A <-> B to exist")
	}

	if !edgeExistsBA {
		test.Errorf("expected edge B <-> A to exist")
	}
}

// TestGetNeighborsReturnsAllNeighbors verifies that GetNeighbors returns
// the full neighbor set for a vertex.
func TestGetNeighborsReturnsAllNeighbors(test *testing.T) {
	// Arrange.
	graph := NewGraph()

	graph.AddUndirectedEdge("A", "B")
	graph.AddUndirectedEdge("A", "C")

	// Act.
	neighborsOfA := graph.GetNeighbors("A")

	// Assert.
	if len(neighborsOfA) != 2 {
		test.Fatalf("expected 2 neighbors for A, got %d", len(neighborsOfA))
	}

	neighborPresenceMap := make(map[string]bool)

	for neighborIndex := 0; neighborIndex < len(neighborsOfA); neighborIndex++ {
		neighborPresenceMap[neighborsOfA[neighborIndex]] = true
	}

	if !neighborPresenceMap["B"] || !neighborPresenceMap["C"] {
		test.Errorf("expected neighbors B and C for vertex A")
	}
}

// TestRemoveUndirectedEdgeRemovesSymmetricAdjacency verifies that removing
// an undirected edge removes both adjacency entries.
func TestRemoveUndirectedEdgeRemovesSymmetricAdjacency(test *testing.T) {
	// Arrange.
	graph := NewGraph()

	graph.AddUndirectedEdge("A", "B")

	// Act.
	graph.RemoveUndirectedEdge("A", "B")

	edgeExistsAB := graph.HasUndirectedEdge("A", "B")
	edgeExistsBA := graph.HasUndirectedEdge("B", "A")

	// Assert.
	if edgeExistsAB || edgeExistsBA {
		test.Errorf("expected edge A <-> B to be removed")
	}
}

// TestRemoveVertexRemovesIncidentEdges verifies that removing a vertex
// also removes all incident edges.
func TestRemoveVertexRemovesIncidentEdges(test *testing.T) {
	// Arrange.
	graph := NewGraph()

	graph.AddUndirectedEdge("A", "B")
	graph.AddUndirectedEdge("B", "C")

	// Act.
	graph.RemoveVertex("B")

	hasVertexB := graph.HasVertex("B")
	neighborsOfA := graph.GetNeighbors("A")
	neighborsOfC := graph.GetNeighbors("C")

	// Assert.
	if hasVertexB {
		test.Errorf("expected vertex B to be removed")
	}

	if len(neighborsOfA) != 0 {
		test.Errorf("expected vertex A to have no neighbors after removal")
	}

	if len(neighborsOfC) != 0 {
		test.Errorf("expected vertex C to have no neighbors after removal")
	}
}

// TestBreadthFirstTraversalVisitsReachableVertices verifies that BFS
// visits all reachable vertices exactly once.
func TestBreadthFirstTraversalVisitsReachableVertices(test *testing.T) {
	// Arrange.
	graph := NewGraph()

	graph.AddUndirectedEdge("A", "B")
	graph.AddUndirectedEdge("A", "C")
	graph.AddUndirectedEdge("B", "D")
	graph.AddUndirectedEdge("C", "E")

	// Act.
	traversalOrder := graph.BreadthFirstTraversal("A")

	// Assert.
	if len(traversalOrder) != 5 {
		test.Fatalf("expected BFS to visit 5 vertices, got %d", len(traversalOrder))
	}

	visitedVertexMap := make(map[string]bool)

	for traversalIndex := 0; traversalIndex < len(traversalOrder); traversalIndex++ {
		visitedVertexMap[traversalOrder[traversalIndex]] = true
	}

	expectedVertices := []string{"A", "B", "C", "D", "E"}

	for expectedIndex := 0; expectedIndex < len(expectedVertices); expectedIndex++ {
		if !visitedVertexMap[expectedVertices[expectedIndex]] {
			test.Errorf("expected BFS to visit vertex %s", expectedVertices[expectedIndex])
		}
	}
}

// TestHasUndirectedEdgeReturnsFalseForMissingVertices verifies that
// HasUndirectedEdge returns false when one or both vertices do not exist.
func TestHasUndirectedEdgeReturnsFalseForMissingVertices(test *testing.T) {
	// Arrange.
	graph := NewGraph()

	graph.AddUndirectedEdge("A", "B")

	// Act.
	edgeExistsWithMissingVertex := graph.HasUndirectedEdge("A", "Z")
	edgeExistsWithBothMissing := graph.HasUndirectedEdge("X", "Y")

	// Assert.
	if edgeExistsWithMissingVertex {
		test.Errorf("expected edge check to return false when one vertex is missing")
	}

	if edgeExistsWithBothMissing {
		test.Errorf("expected edge check to return false when both vertices are missing")
	}
}

// TestGetNeighborsReturnsEmptySliceForMissingVertex verifies that
// GetNeighbors returns an empty slice when the vertex does not exist.
func TestGetNeighborsReturnsEmptySliceForMissingVertex(test *testing.T) {
	// Arrange.
	graph := NewGraph()

	graph.AddUndirectedEdge("A", "B")

	// Act.
	neighborsOfMissingVertex := graph.GetNeighbors("Z")

	// Assert.
	if len(neighborsOfMissingVertex) != 0 {
		test.Errorf("expected empty neighbor slice for missing vertex")
	}
}

// TestRemoveUndirectedEdgeIsNoOpForMissingVertices verifies that
// removing an edge involving missing vertices does not affect the graph.
func TestRemoveUndirectedEdgeIsNoOpForMissingVertices(test *testing.T) {
	// Arrange.
	graph := NewGraph()

	graph.AddUndirectedEdge("A", "B")

	// Act.
	graph.RemoveUndirectedEdge("A", "Z")
	graph.RemoveUndirectedEdge("X", "Y")

	edgeStillExistsAB := graph.HasUndirectedEdge("A", "B")

	// Assert.
	if !edgeStillExistsAB {
		test.Errorf("expected existing edge A <-> B to remain unaffected")
	}
}

// TestRemoveVertexIsNoOpForMissingVertex verifies that removing a
// non-existent vertex does not alter the graph.
func TestRemoveVertexIsNoOpForMissingVertex(test *testing.T) {
	// Arrange.
	graph := NewGraph()

	graph.AddUndirectedEdge("A", "B")

	// Act.
	graph.RemoveVertex("Z")

	hasVertexA := graph.HasVertex("A")
	hasVertexB := graph.HasVertex("B")

	// Assert.
	if !hasVertexA || !hasVertexB {
		test.Errorf("expected existing vertices to remain after removing missing vertex")
	}
}

// TestBreadthFirstTraversalReturnsEmptyForMissingStartVertex verifies that
// BFS returns an empty slice when the start vertex does not exist.
func TestBreadthFirstTraversalReturnsEmptyForMissingStartVertex(test *testing.T) {
	// Arrange.
	graph := NewGraph()

	graph.AddUndirectedEdge("A", "B")

	// Act.
	traversalOrder := graph.BreadthFirstTraversal("Z")

	// Assert.
	if len(traversalOrder) != 0 {
		test.Errorf("expected empty BFS result for missing start vertex")
	}
}

// TestBreadthFirstTraversalDoesNotVisitDisconnectedVertices verifies that
// BFS only visits vertices reachable from the start vertex.
func TestBreadthFirstTraversalDoesNotVisitDisconnectedVertices(test *testing.T) {
	// Arrange.
	graph := NewGraph()

	graph.AddUndirectedEdge("A", "B")
	graph.AddUndirectedEdge("C", "D")

	// Act.
	traversalOrder := graph.BreadthFirstTraversal("A")

	// Assert.
	if len(traversalOrder) != 2 {
		test.Fatalf("expected BFS to visit 2 vertices, got %d", len(traversalOrder))
	}

	visitedVertexMap := make(map[string]bool)

	for traversalIndex := 0; traversalIndex < len(traversalOrder); traversalIndex++ {
		visitedVertexMap[traversalOrder[traversalIndex]] = true
	}

	if !visitedVertexMap["A"] || !visitedVertexMap["B"] {
		test.Errorf("expected BFS to visit vertices A and B")
	}

	if visitedVertexMap["C"] || visitedVertexMap["D"] {
		test.Errorf("expected BFS to not visit disconnected vertices C and D")
	}
}

// TestAddUndirectedEdgeIgnoresEmptyLabels verifies that edges with empty
// vertex labels are not added to the graph.
func TestAddUndirectedEdgeIgnoresEmptyLabels(test *testing.T) {
	// Arrange.
	graph := NewGraph()

	// Act.
	graph.AddUndirectedEdge("", "A")
	graph.AddUndirectedEdge("A", "")
	graph.AddUndirectedEdge("", "")

	hasVertexA := graph.HasVertex("A")

	totalVertexCount := len(graph.adjacencyList)

	// Assert.
	if hasVertexA {
		test.Errorf("expected vertex A to not be created from invalid edge insertion")
	}

	if totalVertexCount != 0 {
		test.Errorf("expected graph to remain empty after invalid edge insertions")
	}
}
