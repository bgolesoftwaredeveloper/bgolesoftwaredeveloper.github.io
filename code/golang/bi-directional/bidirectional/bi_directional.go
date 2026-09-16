// -----------------------------------------------------------------------------
// Package: bidirectional
// File: graph.go
//
// Description:
//
//		This file provides a defensive implementation of an undirected
//		(bi-directional) graph using an adjacency-list representation.
//
//		Each vertex is identified by a string label. Edges are stored symmetrically
//		such that if an edge exists between two vertices, both adjacency lists
//		reflect that relationship.
//
//		The implementation is intentionally defensive:
//	  - All exported methods are safe to call on a nil *Graph receiver.
//	  - Empty vertex labels are treated as invalid input.
//	  - Internal maps are lazily initialized as needed.
//	  - All read operations return safe zero values rather than panicking.
//
//		Data Structure:
//
//		adjacencyList[vertexLabel][neighborLabel] = true
//
//		This structure models a set of neighbors for each vertex, enabling
//		O(1)-average-time edge existence checks, insertions, and removals.
//
// Notes:
//   - The graph is undirected; no edge directionality is implied.
//   - Vertex iteration order is non-deterministic due to Go map semantics.
//   - Traversals are resilient to partially malformed internal state.
//
// -----------------------------------------------------------------------------
package bidirectional

// Graph represents an undirected graph backed by an adjacency list.
type Graph struct {
	adjacencyList map[string]map[string]bool
}

// NewGraph constructs and returns an empty graph with an initialized
// adjacency list.
func NewGraph() *Graph {
	return &Graph{
		adjacencyList: make(map[string]map[string]bool),
	}
}

// ensureVertexExists guarantees that a vertex entry exists in the adjacency
// list and that its neighbor set has been initialized.
//
// This method is unexported because it enforces an internal invariant rather
// than representing a meaningful graph operation.
//
// Defensive behavior:
//   - No-op if the graph receiver is nil.
//   - No-op if the provided label is empty.
func (graph *Graph) ensureVertexExists(label string) {
	// Guard against nil receiver or invalid label.
	if graph == nil || label == "" {
		return
	}

	// Lazily initialize the top-level adjacency list if needed.
	if graph.adjacencyList == nil {
		graph.adjacencyList = make(map[string]map[string]bool)
	}

	// Initialize the vertex's adjacency set if it does not already exist.
	if _, exists := graph.adjacencyList[label]; !exists {
		graph.adjacencyList[label] = make(map[string]bool)
	}
}

// HasVertex reports whether a vertex with the given label exists in the graph.
func (graph *Graph) HasVertex(label string) bool {
	// Guard against nil graph, uninitialized adjacency list, or invalid label.
	if graph == nil || graph.adjacencyList == nil || label == "" {
		return false
	}

	_, exists := graph.adjacencyList[label]

	return exists
}

// HasUndirectedEdge reports whether an undirected edge exists between two
// vertices. The order of the labels does not matter.
func (graph *Graph) HasUndirectedEdge(label, other string) bool {
	// Guard against nil graph or uninitialized adjacency list.
	if graph == nil || graph.adjacencyList == nil {
		return false
	}

	// Retrieve the adjacency adjacency list for the first vertex.
	adjacencyList, exists := graph.adjacencyList[label]

	if !exists {
		return false
	}

	// Check whether the second vertex appears in the adjacency set.
	_, edgeExists := adjacencyList[other]

	return edgeExists
}

// AddUndirectedEdge adds an undirected edge between two vertices.
// If either vertex does not already exist, it is created implicitly.
func (graph *Graph) AddUndirectedEdge(label, other string) {
	// Guard against nil receiver or invalid labels.
	if graph == nil || label == "" || other == "" {
		return
	}

	// Ensure both endpoints exist and have initialized adjacency sets.
	graph.ensureVertexExists(label)
	graph.ensureVertexExists(other)

	// Add symmetric adjacency entries to represent an undirected edge.
	graph.adjacencyList[label][other] = true
	graph.adjacencyList[other][label] = true
}

// GetNeighbors returns a slice containing all neighboring vertex labels
// of the specified vertex. If the vertex does not exist, an empty slice is returned.
func (graph *Graph) GetNeighbors(label string) []string {
	// Guard against nil graph, uninitialized adjacency list, or invalid label.
	if graph == nil || graph.adjacencyList == nil || label == "" {
		return []string{}
	}

	// Retrieve the adjacency set for the vertex.
	adjacencySet, exists := graph.adjacencyList[label]

	if !exists {
		return []string{}
	}

	// Collect neighbor labels into a slice.
	neighbors := make([]string, 0, len(adjacencySet))

	for neighborLabel := range adjacencySet {
		neighbors = append(neighbors, neighborLabel)
	}

	return neighbors
}

// RemoveUndirectedEdge removes the undirected edge between two vertices,
// if such an edge exists. If either vertex does not exist, the operation is a no-op.
func (graph *Graph) RemoveUndirectedEdge(label, other string) {
	// Guard against nil graph or uninitialized adjacency list.
	if graph == nil || graph.adjacencyList == nil {
		return
	}

	// Retrieve adjacency sets for both endpoints.
	adjacencySet, exists := graph.adjacencyList[label]
	otherAdjacencySet, endpointExists := graph.adjacencyList[other]

	if !exists || !endpointExists {
		return
	}

	// Remove the symmetric adjacency entries.
	delete(adjacencySet, other)
	delete(otherAdjacencySet, label)
}

// RemoveVertex removes a vertex from the graph along with all incident edges.
func (graph *Graph) RemoveVertex(label string) {
	// Guard against nil graph, uninitialized adjacency list, or invalid label.
	if graph == nil || graph.adjacencyList == nil || label == "" {
		return
	}

	// Retrieve the adjacency set for the vertex to be removed.
	adjacencySet, exists := graph.adjacencyList[label]

	if !exists {
		return
	}

	// Remove this vertex from all neighbor adjacency sets.
	for neighborLabel := range adjacencySet {
		neighborAdjacencySet, neighborExists := graph.adjacencyList[neighborLabel]

		if neighborExists {
			delete(neighborAdjacencySet, label)
		}
	}

	// Remove the vertex entry itself.
	delete(graph.adjacencyList, label)
}

// BreadthFirstTraversal performs a breadth-first traversal starting at the
// specified vertex label. The function returns the visitation order as a slice of vertex labels.
func (graph *Graph) BreadthFirstTraversal(label string) []string {
	// Guard against nil graph, uninitialized adjacency list, or invalid label.
	if graph == nil || graph.adjacencyList == nil || label == "" {
		return []string{}
	}

	// The start vertex must exist to perform a traversal.
	if !graph.HasVertex(label) {
		return []string{}
	}

	// Track which vertices have been visited.
	visitedVertices := make(map[string]bool, len(graph.adjacencyList))

	// Queue used to manage BFS frontier.
	vertexQueue := make([]string, 0)

	// Slice capturing the visitation order.
	visitOrder := make([]string, 0)

	// Initialize traversal with the starting vertex.
	visitedVertices[label] = true
	vertexQueue = append(vertexQueue, label)

	for len(vertexQueue) > 0 {
		// Dequeue the next vertex.
		currentLabel := vertexQueue[0]
		vertexQueue = vertexQueue[1:]

		// Record visitation of the current vertex.
		visitOrder = append(visitOrder, currentLabel)

		// Retrieve adjacency set for the current vertex.
		adjacencyList, exists := graph.adjacencyList[currentLabel]

		if !exists {
			continue
		}

		// Enqueue all unvisited neighbors.
		for neighborLabel := range adjacencyList {
			if visitedVertices[neighborLabel] {
				continue
			}

			visitedVertices[neighborLabel] = true
			vertexQueue = append(vertexQueue, neighborLabel)
		}
	}

	return visitOrder
}
