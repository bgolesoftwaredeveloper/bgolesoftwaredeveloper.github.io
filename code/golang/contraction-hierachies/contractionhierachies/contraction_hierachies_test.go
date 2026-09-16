// -----------------------------------------------------------------------------
// Package: contractionhierachies
// File:    contraction_hierarchy_test.go
//
// Description:
//
//	This file contains a comprehensive unit test suite for the Contraction
//	Hierarchy (CH) implementation. It validates the structural integrity
//	of the graph, defensive error handling, and the accuracy of the
//	shortcut generation logic.
//
//	The suite covers:
//	- Graph initialization and capacity constraints.
//	- Directed edge insertion and weight validation.
//	- Correct identification of incoming and outgoing neighbors.
//	- Shortcut weight calculation accuracy.
//	- Defensive checks for nil receivers and out-of-bounds indices.
//
// -----------------------------------------------------------------------------
package contractionhierachies

import "testing"

// TestNewGraphValidation ensures the constructor rejects non-positive node counts.
func TestNewGraphValidation(test *testing.T) {
	// Arrange.
	invalidSize := 0

	// Act.
	graph, err := NewGraph(invalidSize)

	// Assert.
	if err == nil || graph != nil {
		test.Error("Expected error when creating a graph with 0 nodes")
	}
}

// TestAddEdgeBoundsCheck verifies that the graph rejects indices outside its range.
func TestAddEdgeBoundsCheck(test *testing.T) {
	// Arrange.
	graph, _ := NewGraph(5)

	// Act.
	err := graph.AddDirectedEdge(0, 10, 1.0)

	// Assert.
	if err == nil {
		test.Error("Expected error when adding edge to non-existent node index")
	}
}

// TestNegativeWeightRejection ensures the graph enforces non-negative edge costs.
func TestNegativeWeightRejection(test *testing.T) {
	// Arrange.
	graph, _ := NewGraph(2)

	// Act.
	err := graph.AddDirectedEdge(0, 1, -5.0)

	// Assert.
	if err == nil {
		test.Error("Graph should not accept negative weights for edges")
	}
}

// TestRetrieveEdgesEmptyNode ensures a node with no outgoing edges returns an empty slice.
func TestRetrieveEdgesEmptyNode(test *testing.T) {
	// Arrange.
	graph, _ := NewGraph(2)

	// Act.
	edges, err := graph.RetrieveEdges(0)

	// Assert.
	if err != nil || len(edges) != 0 {
		test.Errorf("Expected empty edge slice, got %v", edges)
	}
}

// TestContractShortcutWeight ensures the shortcut weight is exactly the sum of its parts.
func TestContractShortcutWeight(test *testing.T) {
	// Arrange.
	graph, _ := NewGraph(3)

	graph.AddDirectedEdge(0, 1, 10.5)
	graph.AddDirectedEdge(1, 2, 4.5)

	// Act.
	shortcuts, _ := graph.Contract(1)

	// Assert.
	if len(shortcuts) != 1 {
		test.Fatalf("Expected 1 shortcut, got %d", len(shortcuts))
	}

	if shortcuts[0].Weight != 15.0 {
		test.Errorf("Expected weight 15.0, got %f", shortcuts[0].Weight)
	}
}

// TestContractSelfLoopPrevention ensures shortcuts are not created that loop back to the source.
func TestContractSelfLoopPrevention(test *testing.T) {
	// Arrange.
	graph, _ := NewGraph(2)

	graph.AddDirectedEdge(0, 1, 5.0)
	graph.AddDirectedEdge(1, 0, 5.0)

	// Act.
	shortcuts, _ := graph.Contract(1)

	// Assert.
	if len(shortcuts) != 0 {
		test.Error("Shortcut should not be created for a self-loop (0->1->0)")
	}
}

// TestMultiPathContraction verifies that contracting a node with multiple neighbors
// creates the correct number of combinations (In x Out).
func TestMultiPathContraction(test *testing.T) {
	// Arrange.
	graph, _ := NewGraph(5)

	graph.AddDirectedEdge(0, 2, 1.0)
	graph.AddDirectedEdge(1, 2, 1.0)
	graph.AddDirectedEdge(2, 3, 1.0)
	graph.AddDirectedEdge(2, 4, 1.0)

	// Act.
	shortcuts, _ := graph.Contract(2)

	// Assert.
	if len(shortcuts) != 4 {
		test.Errorf("Expected 4 shortcuts, got %d", len(shortcuts))
	}
}

// TestNilGraphContractSafety ensures calling Contract on a nil pointer returns an error.
func TestNilGraphContractSafety(test *testing.T) {
	// Arrange.
	var nilGraph *Graph = nil

	// Act.
	_, err := nilGraph.Contract(0)

	// Assert.
	if err == nil {
		test.Error("Contract on nil graph should return error")
	}
}

// TestShortcutIsFlaggedCorrectly verifies that the IsShortcut field is true for generated edges.
func TestShortcutIsFlaggedCorrectly(test *testing.T) {
	// Arrange.
	graph, _ := NewGraph(3)

	graph.AddDirectedEdge(0, 1, 1)
	graph.AddDirectedEdge(1, 2, 1)

	// Act.
	shortcuts, _ := graph.Contract(1)

	// Assert.
	if !shortcuts[0].IsShortcut {
		test.Error("Generated edge must be flagged as a shortcut")
	}
}

// TestGraphStringFormatting validates the custom String() output for nil graphs.
func TestGraphStringFormatting(test *testing.T) {
	// Arrange.
	var nilGraph *Graph = nil

	// Act.
	str := nilGraph.String()

	// Assert.
	if str != "Graph: <nil>" {
		test.Errorf("Unexpected string for nil graph: %s", str)
	}
}

// TestContractInvalidIndex checks for error when contracting a node index out of bounds.
func TestContractInvalidIndex(test *testing.T) {
	// Arrange.
	graph, _ := NewGraph(2)

	// Act.
	_, err := graph.Contract(99)

	// Assert.
	if err == nil {
		test.Error("Expected error for out-of-bounds contraction index")
	}
}

// TestShortcutPersistence ensures shortcuts are actually stored in the internal
// adjacency list after contraction.
func TestShortcutPersistence(test *testing.T) {
	// Arrange.
	graph, _ := NewGraph(3)

	graph.AddDirectedEdge(0, 1, 5)
	graph.AddDirectedEdge(1, 2, 5)

	// Act.
	graph.Contract(1)

	edges, _ := graph.RetrieveEdges(0)

	// Assert.
	foundShortcut := false

	for _, edge := range edges {
		if edge.IsShortcut && edge.Destination == 2 {
			foundShortcut = true
		}
	}

	if !foundShortcut {
		test.Error("Shortcut was not found in Node 0's edge list")
	}
}

// TestAddEdgeNilSafety checks that adding an edge to a nil graph returns an error.
func TestAddEdgeNilSafety(test *testing.T) {
	// Arrange.
	var graph *Graph = nil

	// Act.
	err := graph.AddDirectedEdge(0, 1, 10)

	// Assert.
	if err == nil {
		test.Error("Adding edge to nil graph should fail")
	}
}

// TestDuplicateEdgesInContraction ensures that multiple standard edges
// results in multiple possible shortcuts.
func TestDuplicateEdgesInContraction(test *testing.T) {
	// Arrange.
	graph, _ := NewGraph(3)

	graph.AddDirectedEdge(0, 1, 5)
	graph.AddDirectedEdge(0, 1, 10)
	graph.AddDirectedEdge(1, 2, 5)

	// Act.
	shortcuts, _ := graph.Contract(1)

	// Assert.
	if len(shortcuts) != 2 {
		test.Errorf("Expected 2 shortcuts from dual incoming edges, got %d", len(shortcuts))
	}
}

// TestRetrieveEdgesNilSafety checks the behavior of RetrieveEdges on a nil receiver.
func TestRetrieveEdgesNilSafety(test *testing.T) {
	// Arrange.
	var nilGraph *Graph = nil

	// Act.
	_, err := nilGraph.RetrieveEdges(0)

	// Assert.
	if err == nil {
		test.Error("RetrieveEdges on nil graph should return an error")
	}
}
