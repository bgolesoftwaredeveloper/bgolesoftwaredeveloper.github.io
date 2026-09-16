// -----------------------------------------------------------------------------
// Package: graphalgorithms
// File: kruskal_minimum_spanning_tree.go
//
// Description:
//
//	This file provides a clear and defensive implementation of
//	Kruskal’s algorithm for computing a Minimum Spanning Tree (MST)
//	of an undirected, weighted graph.
//
//	The implementation emphasizes:
//
//	- Explicit data structures.
//	- Long, descriptive identifiers.
//	- Defensive bounds checking.
//	- Clear separation of responsibilities.
//	- Step-by-step commentary suitable for educational or systems work.
//
// -----------------------------------------------------------------------------
package kruskalminimumspanningtree

import "sort"

// WeightedUndirectedEdge represents a single undirected edge
// connecting two vertices with an associated non-negative weight.
type WeightedUndirectedEdge struct {
	start  int
	end    int
	weight int
}

// UndirectedWeightedGraph represents a graph defined by a fixed
// number of vertices and a collection of weighted edges.
type UndirectedWeightedGraph struct {
	count int
	edges []WeightedUndirectedEdge
}

// Start returns the starting vertex index of the undirected edge.
// If the receiver is nil, the function returns -1 to indicate
// an invalid or unavailable vertex index.
func (edge *WeightedUndirectedEdge) Start() int {
	// Guard: nil edge receiver.
	if edge == nil {
		return -1
	}

	return edge.start
}

// End returns the ending vertex index of the undirected edge.
// If the receiver is nil, the function returns -1 to indicate
// an invalid or unavailable vertex index.
func (edge *WeightedUndirectedEdge) End() int {
	// Guard: nil edge receiver.
	if edge == nil {
		return -1
	}

	return edge.end
}

// Weight returns the weight associated with the undirected edge.
// If the receiver is nil, the function returns -1 to indicate
// an invalid or unavailable weight value.
func (edge *WeightedUndirectedEdge) Weight() int {
	// Guard: nil edge receiver.
	if edge == nil {
		return -1
	}

	return edge.weight
}

// NewUndirectedWeightedGraph constructs a new graph with the
// specified number of vertices and no edges.
func NewUndirectedWeightedGraph(count int) *UndirectedWeightedGraph {
	// Guard: against count.
	if count <= 0 {
		return &UndirectedWeightedGraph{
			count: 0,
			edges: nil,
		}
	}

	return &UndirectedWeightedGraph{
		count: count,
		edges: make([]WeightedUndirectedEdge, 0),
	}
}

// AddEdge inserts a new undirected weighted edge into the graph.
// The function performs basic validation to prevent malformed input.
func (graph *UndirectedWeightedGraph) AddEdge(start, end, weight int) bool {
	// Validate vertex indices.
	if start < 0 || start >= graph.count {
		return false
	}

	if end < 0 || end >= graph.count {
		return false
	}

	// Disallow negative edge weights to keep semantics clear.
	if weight < 0 {
		return false
	}

	// Append the edge to the graph’s edge list.
	graph.edges = append(graph.edges, WeightedUndirectedEdge{
		start:  start,
		end:    end,
		weight: weight,
	})

	return true
}

// disjointSetUnion provides a union–find data structure with
// path compression and union by rank.
// It is used to detect cycles during Kruskal’s algorithm.
type disjointSetUnion struct {
	indices []int
	ranks   []int
}

// newDisjointSetUnion initializes a disjoint set structure
// with each element in its own set.
func newDisjointSetUnion(count int) *disjointSetUnion {
	// Guard: invlaid or empty count.
	if count <= 0 {
		return &disjointSetUnion{
			indices: nil,
			ranks:   nil,
		}
	}

	parentIndices := make([]int, count)
	treeRanks := make([]int, count)

	for index := 0; index < count; index++ {
		parentIndices[index] = index
		treeRanks[index] = 0
	}

	return &disjointSetUnion{
		indices: parentIndices,
		ranks:   treeRanks,
	}
}

// findRepresentative returns the root representative for the
// specified element using path compression.
func (set *disjointSetUnion) findRepresentative(element int) int {
	// Guard against invalid indices.
	if element < 0 || element >= len(set.indices) {
		return element
	}

	// If the element is not its own parent, recursively
	// compress the path to the root representative.
	if set.indices[element] != element {
		set.indices[element] = set.findRepresentative(set.indices[element])
	}

	return set.indices[element]
}

// unionSets merges the sets containing the two elements.
// The function returns true if a merge occurred.
func (set *disjointSetUnion) unionSets(item int, other int) bool {
	// Guard against invalid indices.
	numberOfIndices := len(set.indices)

	if item < 0 || item >= numberOfIndices {
		return false
	}

	if other < 0 || other >= numberOfIndices {
		return false
	}

	root := set.findRepresentative(item)
	rootRepresentative := set.findRepresentative(other)

	// If both elements already belong to the same set,
	// merging would create a cycle.
	if root == rootRepresentative {
		return false
	}

	// Attach the smaller-ranked tree under the larger-ranked tree.
	if set.ranks[root] < set.ranks[rootRepresentative] {
		set.indices[root] = rootRepresentative
	} else if set.ranks[root] > set.ranks[rootRepresentative] {
		set.indices[rootRepresentative] = root
	} else {
		set.indices[rootRepresentative] = root
		set.ranks[root]++
	}

	return true
}

// Kruskal executes Kruskal’s algorithm
// on the provided undirected weighted graph.
// It returns the list of edges included in the MST and the total weight.
func Kruskal(graph *UndirectedWeightedGraph) ([]WeightedUndirectedEdge, int) {
	// Guard: nil graph pointer.
	if graph == nil {
		return nil, 0
	}

	// Guard: a graph with no vertices cannot produce an MST.
	if graph.count <= 0 {
		return nil, 0
	}

	// Guard impossible MST case.
	if len(graph.edges) < graph.count-1 {
		return nil, 0
	}

	// Sort all edges by ascending weight.
	sort.Slice(graph.edges, func(compare int, against int) bool {
		return graph.edges[compare].weight < graph.edges[against].weight
	})

	// Initialize the disjoint set structure for cycle detection.
	disjointSets := newDisjointSetUnion(graph.count)

	minimumSpanningTreeEdges := make([]WeightedUndirectedEdge, 0, graph.count-1)
	totalWeight := 0

	// Iterate over edges in ascending weight order.
	for _, edge := range graph.edges {
		// Guard: validation of endpoints.
		if edge.start < 0 || edge.start >= graph.count {
			continue
		}

		if edge.end < 0 || edge.end >= graph.count {
			continue
		}

		// Attempt to merge the sets of the two endpoints.
		// If successful, the edge does not create a cycle.
		if disjointSets.unionSets(edge.start, edge.end) {
			minimumSpanningTreeEdges = append(minimumSpanningTreeEdges, edge)
			totalWeight += edge.weight

			// A spanning tree for V vertices contains exactly V - 1 edges.
			if len(minimumSpanningTreeEdges) == graph.count-1 {
				break
			}
		}
	}

	// Guard: incomplete spanning tree.
	// This occurs when the graph is disconnected.
	if len(minimumSpanningTreeEdges) != graph.count-1 {
		return nil, 0
	}

	return minimumSpanningTreeEdges, totalWeight
}
