// -----------------------------------------------------------------------------
// Package: toposort
// File: kahns_topological_sort.go
//
// Description:
//
//	This file contains an implementation of Kahn's Algorithm for
//	topological sorting of a directed acyclic graph (DAG).
//
//	Kahn's Algorithm works by iteratively removing nodes with
//	no incoming edges (in-degree zero) and appending them to
//	the topological ordering. As nodes are removed, the in-degrees
//	of their outgoing neighbors are reduced. Newly freed nodes
//	are processed next.
//
//	If the algorithm is unable to process all nodes, the graph
//	contains at least one cycle and therefore has no valid
//	topological ordering.
//
// Design Notes:
//   - The implementation is fully deterministic given input order.
//   - No recursion is used; the algorithm is iterative and queue-based.
//   - The graph is treated as immutable during execution.
//   - Cycles are detected by comparing processed node count.
//
// Intended Use:
//
//	This package is suitable for dependency resolution, build systems,
//	scheduling problems, and as a reference implementation of
//	topological sorting using in-degree elimination.
//
// -----------------------------------------------------------------------------
package kahnstopologicalsort

// Graph represents a directed graph using an adjacency list.
// Each node maps to a slice of nodes it has outgoing edges to.
type Graph map[string][]string

// TopologicalSortKahn performs a topological sort of the given graph
// using Kahn's Algorithm.
//
// If the graph is acyclic, the function returns a slice containing
// all nodes in topologically sorted order and true.
//
// If the graph contains a cycle, the function returns nil and false.
//
// This function is defensive:
//   - An empty graph yields an empty ordering and true.
//   - Nodes appearing only as dependencies are still included.
//   - Internal maps are validated before use.
func TopologicalSortKahn(graph Graph) ([]string, bool) {
	// Handle a nil or empty graph.
	if len(graph) == 0 {
		return []string{}, true
	}

	// Compute the in-degree for every node in the graph.
	inDegreeByNode := make(map[string]int)

	// Initialize in-degrees for all explicitly declared nodes.
	for node := range graph {
		inDegreeByNode[node] = 0
	}

	// Populate in-degree counts by scanning outgoing edges.
	for _, outgoingNeighbors := range graph {
		for _, neighbor := range outgoingNeighbors {
			// Ensure nodes that only appear as neighbors are tracked.
			if _, exists := inDegreeByNode[neighbor]; !exists {
				inDegreeByNode[neighbor] = 0
			}

			// Increment the neighbor's in-degree.
			inDegreeByNode[neighbor]++
		}
	}

	// Initialize a queue containing all nodes with zero in-degree.
	zeroInDegreeQueue := make([]string, 0)

	for node, degree := range inDegreeByNode {
		if degree == 0 {
			zeroInDegreeQueue = append(zeroInDegreeQueue, node)
		}
	}

	// This slice will store the final topological ordering.
	topologicalOrder := make([]string, 0, len(inDegreeByNode))

	// Process nodes until no zero in-degree nodes remain.
	for len(zeroInDegreeQueue) > 0 {
		// Dequeue the next node with zero in-degree.
		currentNode := zeroInDegreeQueue[0]
		zeroInDegreeQueue = zeroInDegreeQueue[1:]

		// Append the node to the topological ordering.
		topologicalOrder = append(topologicalOrder, currentNode)

		// Retrieve the outgoing neighbors of the current node.
		outgoingNeighbors, exists := graph[currentNode]

		// Nodes with no outgoing edges may not exist in the adjacency list.
		if !exists {
			continue
		}

		// Reduce the in-degree of each neighbor.
		for _, neighbor := range outgoingNeighbors {
			inDegreeByNode[neighbor]--

			// If a neighbor now has zero in-degree, enqueue it.
			if inDegreeByNode[neighbor] == 0 {
				zeroInDegreeQueue = append(zeroInDegreeQueue, neighbor)
			}
		}
	}

	// If not all nodes were processed, the graph contains a cycle.
	if len(topologicalOrder) != len(inDegreeByNode) {
		return nil, false
	}

	// Return the valid topological ordering.
	return topologicalOrder, true
}
