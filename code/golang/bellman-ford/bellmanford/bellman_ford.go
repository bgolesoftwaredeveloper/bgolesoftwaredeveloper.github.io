// -----------------------------------------------------------------------------
// Package: bellmanford
// File: bellman_ford.go
//
// Description:
//
//	This file implements the Bellman–Ford single-source shortest path
//	algorithm for directed, weighted graphs. The algorithm supports
//	negative edge weights and explicitly detects negative-weight cycles.
//
//	The core invariant maintained by the algorithm is that after the i-th
//	relaxation pass, all shortest paths consisting of at most i edges have
//	been correctly computed. After (V - 1) iterations, all shortest paths
//	in a graph without negative cycles must be fully resolved.
//
//	A final verification pass is performed to detect negative-weight cycles.
//	If any distance can still be reduced, the graph is declared invalid for
//	shortest-path computation.
//
// Notes:
//   - The graph is represented using an edge list.
//   - Distance values are initialized pessimistically to infinity.
//   - The algorithm tolerates unreachable vertices.
//   - A negative-weight cycle results in an error return.
//
// -----------------------------------------------------------------------------
package bellmanford

import (
	"errors"
	"math"
)

// Edge represents a directed, weighted edge in the graph.
// Each edge connects a source vertex to a destination vertex
// and contributes a signed weight to the path cost.
type Edge struct {
	Source      int
	Destination int
	Weight      int
}

// Graph represents a directed graph using an edge-list representation.
// The Count field specifies the number of vertices, while Edges contains
// all directed connections between vertices.
type Graph struct {
	Count int
	Edges []Edge
}

// BellmanFord computes the shortest path distances from a single source
// vertex to all other vertices in the graph.
// If the graph contains a negative-weight cycle reachable from the source,
// this function returns an error. Otherwise, it returns a slice of distances
// indexed by vertex identifier.
func BellmanFord(graph Graph, source int) ([]int, error) {
	// Guard against count.
	if graph.Count <= 0 {
		return nil, errors.New("graph must contain at least one vertex")
	}

	// Boundary checking.
	if source < 0 || source >= graph.Count {
		return nil, errors.New("source vertex out of range")
	}

	// Allocate and initialize the distance table.
	// All vertices are pessimistically assumed unreachable.
	distanceToVertex := make([]int, graph.Count)

	for index := 0; index < graph.Count; index++ {
		distanceToVertex[index] = math.MaxInt
	}

	// The distance from the source to itself is zero by definition.
	distanceToVertex[source] = 0

	// Perform (V - 1) relaxation passes over all edges.
	// Each pass allows paths with one additional edge to be resolved.
	for iteration := 1; iteration <= graph.Count-1; iteration++ {
		for _, edge := range graph.Edges {
			if distanceToVertex[source] == math.MaxInt {
				continue
			}

			// Compute the candidate distance via the current edge.
			newDistance := distanceToVertex[edge.Source] + edge.Weight

			// Relax the edge if a shorter path is discovered.
			if newDistance < distanceToVertex[edge.Destination] {
				distanceToVertex[edge.Destination] = newDistance
			}
		}
	}

	// Perform one additional pass to detect negative-weight cycles.
	// Any further relaxation indicates a cycle with net negative cost.
	for _, edge := range graph.Edges {
		if distanceToVertex[edge.Source] == math.MaxInt {
			continue
		}

		if distanceToVertex[edge.Source]+edge.Weight < distanceToVertex[edge.Destination] {
			return nil, errors.New("graph contains a negative-weight cycle")
		}
	}

	// All shortest paths have been successfully computed.
	return distanceToVertex, nil
}
