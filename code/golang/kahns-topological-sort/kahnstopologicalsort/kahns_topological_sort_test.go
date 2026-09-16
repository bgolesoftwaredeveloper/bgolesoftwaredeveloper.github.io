// -----------------------------------------------------------------------------
// Package: kahnstopologicalsort
// File: kahn_topological_sort_test.go
//
// Description:
//
//	This file contains unit tests for the Kahn topological sorting
//	algorithm implementation.
//
//	The tests construct small directed graphs with known dependency
//	structures and verify that the computed topological ordering
//	respects all precedence constraints.
//
//	Each test follows a clear Arrange / Act / Assert structure to
//	improve readability and diagnostic clarity.
//
// Notes:
//   - Tests do not rely on map iteration order.
//   - Orderings are validated by dependency constraints, not position.
//   - Cyclic graphs are expected to produce failure results.
//
// -----------------------------------------------------------------------------
package kahnstopologicalsort

import "testing"

// assertPrecedes verifies that predecessor appears before successor
// in the computed topological ordering.
func assertPrecedes(test *testing.T,
	position map[string]int, predecessor string, successor string) {
	test.Helper()

	predecessorPosition, predecessorExists := position[predecessor]
	successorPosition, successorExists := position[successor]

	if !predecessorExists {
		test.Fatalf("Expected node %q to appear in ordering.", predecessor)
	}

	if !successorExists {
		test.Fatalf("Expected node %q to appear in ordering.", successor)
	}

	if predecessorPosition >= successorPosition {
		test.Fatalf("Expected %q to precede %q in ordering.",
			predecessor,
			successor,
		)
	}
}

// TestTopologicalSortKahnHandlesEmptyGraph verifies that an empty
// graph yields a valid empty ordering.
func TestTopologicalSortKahnHandlesEmptyGraph(test *testing.T) {
	// Arrange.
	graph := Graph{}

	// Act.
	ordering, ok := TopologicalSortKahn(graph)

	// Assert.
	if !ok {
		test.Fatalf("Expected empty graph to produce valid ordering.")
	}

	if ordering == nil {
		test.Fatalf("Expected non-nil ordering slice.")
	}

	if len(ordering) != 0 {
		test.Fatalf("Expected empty ordering, got %d elements.",
			len(ordering),
		)
	}
}

// TestTopologicalSortKahnComputesValidOrdering verifies that the
// algorithm produces an ordering that satisfies all dependencies.
func TestTopologicalSortKahnComputesValidOrdering(test *testing.T) {
	// Arrange.
	graph := Graph{
		"design":  {"compile"},
		"compile": {"link"},
		"link":    {"package"},
		"test":    {"package"},
		"package": {},
	}

	// Act.
	ordering, ok := TopologicalSortKahn(graph)

	// Assert.
	if !ok {
		test.Fatalf("Expected valid ordering for acyclic graph.")
	}

	if ordering == nil {
		test.Fatalf("Expected non-nil ordering slice.")
	}

	// Build index map for order verification.
	positionByNode := make(map[string]int)

	for index, node := range ordering {
		positionByNode[node] = index
	}

	// Verify dependency constraints.
	assertPrecedes(test, positionByNode, "design", "compile")
	assertPrecedes(test, positionByNode, "compile", "link")
	assertPrecedes(test, positionByNode, "link", "package")
	assertPrecedes(test, positionByNode, "test", "package")
}

// TestTopologicalSortKahnIncludesDependencyOnlyNodes verifies that
// nodes which appear only as dependencies are still included.
func TestTopologicalSortKahnIncludesDependencyOnlyNodes(test *testing.T) {
	// Arrange.
	graph := Graph{
		"a": {"b"},
	}

	// Act.
	ordering, ok := TopologicalSortKahn(graph)

	// Assert.
	if !ok {
		test.Fatalf("Expected valid ordering.")
	}

	if len(ordering) != 2 {
		test.Fatalf("Expected 2 nodes in ordering, got %d.",
			len(ordering),
		)
	}

	foundA := false
	foundB := false

	for _, node := range ordering {
		if node == "a" {
			foundA = true
		}
		if node == "b" {
			foundB = true
		}
	}

	if !foundA || !foundB {
		test.Fatalf("Expected both 'a' and 'b' to appear in ordering.")
	}
}

// TestTopologicalSortKahnDetectsCycle verifies that the algorithm
// correctly reports failure when the graph contains a cycle.
func TestTopologicalSortKahnDetectsCycle(test *testing.T) {
	// Arrange.
	graph := Graph{
		"a": {"b"},
		"b": {"c"},
		"c": {"a"},
	}

	// Act.
	ordering, ok := TopologicalSortKahn(graph)

	// Assert.
	if ok {
		test.Fatalf("Expected cycle detection failure, got success.")
	}

	if ordering != nil {
		test.Fatalf("Expected nil ordering for cyclic graph.")
	}
}

// TestTopologicalSortKahnHandlesSelfLoop verifies that a self-loop
// is treated as a cycle.
func TestTopologicalSortKahnHandlesSelfLoop(test *testing.T) {
	// Arrange.
	graph := Graph{
		"a": {"a"},
	}

	// Act.
	ordering, ok := TopologicalSortKahn(graph)

	// Assert.
	if ok {
		test.Fatalf("Expected self-loop to be detected as a cycle.")
	}

	if ordering != nil {
		test.Fatalf("Expected nil ordering for self-loop graph.")
	}
}

// TestTopologicalSortKahnHandlesDisconnectedGraph verifies that
// disconnected components are all included in the ordering.
func TestTopologicalSortKahnHandlesDisconnectedGraph(test *testing.T) {
	// Arrange.
	graph := Graph{
		"a": {"b"},
		"c": {"d"},
		"b": {},
		"d": {},
	}

	// Act.
	ordering, ok := TopologicalSortKahn(graph)

	// Assert.
	if !ok {
		test.Fatalf("Expected valid ordering for disconnected graph.")
	}

	if len(ordering) != 4 {
		test.Fatalf("Expected 4 nodes in ordering, got %d.",
			len(ordering),
		)
	}

	positionByNode := make(map[string]int)

	for index, node := range ordering {
		positionByNode[node] = index
	}

	assertPrecedes(test, positionByNode, "a", "b")
	assertPrecedes(test, positionByNode, "c", "d")
}

// TestTopologicalSortKahnHandlesSingleNode verifies that a graph
// containing a single node with no edges produces a valid ordering.
func TestTopologicalSortKahnHandlesSingleNode(test *testing.T) {
	// Arrange.
	graph := Graph{
		"a": {},
	}

	// Act.
	ordering, ok := TopologicalSortKahn(graph)

	// Assert.
	if !ok {
		test.Fatalf("Expected valid ordering for single-node graph.")
	}

	if ordering == nil {
		test.Fatalf("Expected non-nil ordering slice.")
	}

	if len(ordering) != 1 {
		test.Fatalf("Expected ordering of length 1, got %d.",
			len(ordering),
		)
	}

	if ordering[0] != "a" {
		test.Fatalf("Expected only node 'a' in ordering, got %q.",
			ordering[0],
		)
	}
}

// TestTopologicalSortKahnHandlesMultipleRoots verifies that multiple
// zero in-degree nodes are all included in the ordering.
func TestTopologicalSortKahnHandlesMultipleRoots(test *testing.T) {
	// Arrange.
	graph := Graph{
		"a": {"c"},
		"b": {"c"},
		"c": {},
	}

	// Act.
	ordering, ok := TopologicalSortKahn(graph)

	// Assert.
	if !ok {
		test.Fatalf("Expected valid ordering for graph with multiple roots.")
	}

	if len(ordering) != 3 {
		test.Fatalf("Expected 3 nodes in ordering, got %d.",
			len(ordering),
		)
	}

	positionByNode := make(map[string]int)

	for index, node := range ordering {
		positionByNode[node] = index
	}

	assertPrecedes(test, positionByNode, "a", "c")
	assertPrecedes(test, positionByNode, "b", "c")
}

// TestTopologicalSortKahnHandlesDiamondDependency verifies correct
// ordering in a diamond-shaped dependency graph.
func TestTopologicalSortKahnHandlesDiamondDependency(test *testing.T) {
	// Arrange.
	graph := Graph{
		"a": {"b", "c"},
		"b": {"d"},
		"c": {"d"},
		"d": {},
	}

	// Act.
	ordering, ok := TopologicalSortKahn(graph)

	// Assert.
	if !ok {
		test.Fatalf("Expected valid ordering for diamond dependency graph.")
	}

	positionByNode := make(map[string]int)

	for index, node := range ordering {
		positionByNode[node] = index
	}

	assertPrecedes(test, positionByNode, "a", "b")
	assertPrecedes(test, positionByNode, "a", "c")
	assertPrecedes(test, positionByNode, "b", "d")
	assertPrecedes(test, positionByNode, "c", "d")
}

// TestTopologicalSortKahnHandlesLongLinearChain verifies correct
// behavior for a deep linear dependency chain.
func TestTopologicalSortKahnHandlesLongLinearChain(test *testing.T) {
	// Arrange.
	graph := Graph{
		"a": {"b"},
		"b": {"c"},
		"c": {"d"},
		"d": {"e"},
		"e": {},
	}

	// Act.
	ordering, ok := TopologicalSortKahn(graph)

	// Assert.
	if !ok {
		test.Fatalf("Expected valid ordering for linear dependency chain.")
	}

	positionByNode := make(map[string]int)

	for index, node := range ordering {
		positionByNode[node] = index
	}

	assertPrecedes(test, positionByNode, "a", "b")
	assertPrecedes(test, positionByNode, "b", "c")
	assertPrecedes(test, positionByNode, "c", "d")
	assertPrecedes(test, positionByNode, "d", "e")
}

// TestTopologicalSortKahnDetectsPartialCycle verifies that the
// algorithm reports failure when a cycle exists in only part
// of the graph.
func TestTopologicalSortKahnDetectsPartialCycle(test *testing.T) {
	// Arrange.
	graph := Graph{
		"a": {"b"},
		"b": {"a"},
		"c": {"d"},
		"d": {},
	}

	// Act.
	ordering, ok := TopologicalSortKahn(graph)

	// Assert.
	if ok {
		test.Fatalf("Expected failure due to partial cycle in graph.")
	}

	if ordering != nil {
		test.Fatalf("Expected nil ordering for graph containing a cycle.")
	}
}

// TestTopologicalSortKahnHandlesDuplicateEdges verifies that
// duplicate edges do not break dependency resolution.
func TestTopologicalSortKahnHandlesDuplicateEdges(test *testing.T) {
	// Arrange.
	graph := Graph{
		"a": {"b", "b"},
		"b": {},
	}

	// Act.
	ordering, ok := TopologicalSortKahn(graph)

	// Assert.
	if !ok {
		test.Fatalf("Expected valid ordering despite duplicate edges.")
	}

	positionByNode := make(map[string]int)

	for index, node := range ordering {
		positionByNode[node] = index
	}

	assertPrecedes(test, positionByNode, "a", "b")
}

// TestTopologicalSortKahnDoesNotMutateInput verifies that the
// input graph is not modified during execution.
func TestTopologicalSortKahnDoesNotMutateInput(test *testing.T) {
	// Arrange.
	graph := Graph{
		"a": {"b"},
		"b": {},
	}

	originalEdgeCount := len(graph["a"])

	// Act.
	_, ok := TopologicalSortKahn(graph)

	// Assert.
	if !ok {
		test.Fatalf("Expected valid ordering.")
	}

	if len(graph["a"]) != originalEdgeCount {
		test.Fatalf("Expected input graph to remain unmodified.")
	}
}
