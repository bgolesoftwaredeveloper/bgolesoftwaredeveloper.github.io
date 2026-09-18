// -----------------------------------------------------------------------------
// Package: optics
// File: optics.go
//
// Description:
//
//	This file provides a clean, dependency-free implementation of the OPTICS
//	(Ordering Points To Identify the Clustering Structure) density-based
//	clustering algorithm for N-dimensional vector data.
//
//	Unlike traditional partitioning algorithms (e.g., K-Means), OPTICS does not
//	force points into rigid clusters upfront. Instead, it processes data points
//	to produce a linear ordering based on spatial density structures. This ordering
//	encapsulates core distances and reachability distances, enabling the extraction
//	of arbitrary-shaped clusters and variable-density structures without specifying K.
//
//	The algorithm operates using the following process:
//
//	  1. Initialization — wraps spatial points into Graph Nodes with initial +Inf distances.
//	  2. Core Identification — determines if a point has at least MinPts within MaximumEpsilon.
//	  3. Priority Expansion — uses a priority seed queue ordered by Reachability Distance
//	     to continuously process the densest reachable neighbors next.
//
// Key Features & Design Principles:
//
//   - Dimension-Agnostic: Point abstractions support arbitrary N-dimensional real-valued spaces.
//   - Self-Contained Math: Implements Euclidean distance calculations using standard primitives.
//   - Density-Based Traversal: Handles noise identification and arbitrary cluster geometries.
//   - Invariant Preservation: Preserves input vector integrity while tracking traversal state.
//
// Operational Limits & Bounds:
//
//   - MinPts Condition: Core distance evaluates to +Inf if neighborhood count < MinimumPoints.
//   - Epsilon Cutoff: MaximumEpsilon bounds neighbor searches to optimize execution complexity.
//
// Intended Use:
//
//	This package is intended as a core machine learning building block for zero-dependency
//	Go applications requiring density-based cluster analysis, outlier/noise detection, or
//	hierarchical structure discovery.
//
// -----------------------------------------------------------------------------
package optics

import (
	"math"
	"sort"
)

// Point represents an N-dimensional numerical vector.
type Point []float64

// Node encapsulates a spatial point alongside tracking parameters necessary
// for density-based traversal, including core and reachability distances.
type Node struct {
	Index                int
	Point                Point
	CoreDistance         float64
	ReachabilityDistance float64
	Visited              bool
}

// Optics manages execution state, parameter constraints, and the output
// ordering sequence for the clustering algorithm.
type Optics struct {
	MinimumPoints  int
	MaximumEpsilon float64
	Nodes          []*Node
	OrderedSet     []*Node
}

// getNeighbors locates all nodes within MaximumEpsilon distance of the target junction node,
// returning the matched nodes alongside their computed Euclidean distances.
func (optics *Optics) getNeighbors(junction *Node) ([]*Node, []float64) {
	var neighbors []*Node
	var distances []float64

	// Evaluate distance against every node in the dataset.
	for _, node := range optics.Nodes {
		distance := junction.Point.Distance(node.Point)

		// Retain nodes that fall within the specified maximum search radius.
		if distance <= optics.MaximumEpsilon {
			neighbors = append(neighbors, node)
			distances = append(distances, distance)
		}
	}

	return neighbors, distances
}

// calculateCoreDistance determines the minimum distance required for a point to qualify
// as a core point. Returns +Inf if neighbor count falls below MinimumPoints.
func (optics *Optics) calculateCoreDistance(distances []float64) float64 {
	// Reject core qualification if fewer neighbors exist than the minimum threshold.
	if len(distances) < optics.MinimumPoints {
		return math.Inf(1)
	}

	sorted := make([]float64, len(distances))

	copy(sorted, distances)

	// Sort neighbor distances in ascending order to find the distance to the K-th neighbor.
	sort.Float64s(sorted)

	return sorted[optics.MinimumPoints-1]
}

// update updates the reachability distances of unvisited neighbors relative to the center node
// and inserts or adjusts their positioning within the priority seed queue.
func (optics *Optics) update(neighbors []*Node, center *Node, seeds []*Node) []*Node {
	for _, neighbor := range neighbors {
		// Skip nodes that have already been finalized in the ordered output set.
		if neighbor.Visited {
			continue
		}

		distance := center.Point.Distance(neighbor.Point)
		newReachabilityDistance := math.Max(center.CoreDistance, distance)

		// Unqueued neighbor: assign reachability distance and add to seed queue.
		if math.IsInf(neighbor.ReachabilityDistance, 1) {
			neighbor.ReachabilityDistance = newReachabilityDistance
			seeds = append(seeds, neighbor)
		} else if newReachabilityDistance < neighbor.ReachabilityDistance {
			// Existing seed neighbor: update with newly discovered shorter reachability distance.
			neighbor.ReachabilityDistance = newReachabilityDistance
		}
	}

	return seeds
}

// Distance computes the Euclidean distance (L2 norm) between two N-dimensional points.
func (point Point) Distance(other Point) float64 {
	var sum float64

	for index := range point {
		difference := point[index] - other[index]

		sum += difference * difference
	}

	return math.Sqrt(sum)
}

// NewOptics initializes and constructs a new Optics instance with formatted nodes
// setting initial core and reachability values to +Inf.
func NewOptics(data []Point, maximumEpsilon float64, minimumPoints int) *Optics {
	nodes := make([]*Node, len(data))

	// Instantiate tracking nodes for all provided spatial vectors.
	for index, point := range data {
		nodes[index] = &Node{
			Index:                index,
			Point:                point,
			CoreDistance:         math.Inf(1),
			ReachabilityDistance: math.Inf(1),
		}
	}

	return &Optics{
		MinimumPoints:  minimumPoints,
		MaximumEpsilon: maximumEpsilon,
		Nodes:          nodes,
	}
}

// Run executes the density-based ordering traversal across all dataset nodes,
// populating OrderedSet with sequentially processed nodes and calculated distances.
func (optics *Optics) Run() {
	for _, node := range optics.Nodes {
		// Skip node if processed during a prior density expansion sequence.
		if node.Visited {
			continue
		}

		// Query local neighborhood within maximum epsilon radius.
		neighbors, distances := optics.getNeighbors(node)

		node.Visited = true

		// Append unvisited base node to the final cluster ordering output.
		optics.OrderedSet = append(optics.OrderedSet, node)

		node.CoreDistance = optics.calculateCoreDistance(distances)

		// Expand density neighborhood if current node meets core distance requirements.
		if !math.IsInf(node.CoreDistance, 1) {
			var seeds []*Node

			seeds = optics.update(neighbors, node, seeds)

			// Process priority seed queue prioritized by shortest reachability distance.
			for len(seeds) > 0 {
				sort.Slice(seeds, func(compare, against int) bool {
					return seeds[compare].ReachabilityDistance < seeds[against].ReachabilityDistance
				})

				// Extract node with smallest reachability distance from seed queue.
				current := seeds[0]
				seeds = seeds[1:]

				if current.Visited {
					continue
				}

				current.Visited = true

				optics.OrderedSet = append(optics.OrderedSet, current)

				currentNeighbors, currentDistances := optics.getNeighbors(current)

				current.CoreDistance = optics.calculateCoreDistance(currentDistances)

				// Recursively update and expand seeds if candidate is also a core point.
				if !math.IsInf(current.CoreDistance, 1) {
					seeds = optics.update(currentNeighbors, current, seeds)
				}
			}
		}
	}
}
