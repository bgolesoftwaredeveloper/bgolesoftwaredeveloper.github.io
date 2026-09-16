// -----------------------------------------------------------------------------
// Package: balltree
// File: ball_tree.go
//
// Description:
//
//		This file provides a defensive implementation of a Ball Tree data structure
//		for efficient nearest-neighbor and range queries in metric spaces.
//
//		A Ball Tree recursively partitions points into hyperspherical regions
//		("balls"), each defined by a center point and a radius. This enables
//		pruning of large portions of the search space during queries such as
//		k-nearest neighbors (k-NN) and radius searches.
//
//		The construction strategy used here approximates cluster diameters by
//		selecting two farthest points and partitioning remaining points based on
//		proximity to those endpoints.
//
//		The implementation is intentionally defensive:
//	  - All methods are safe to call on a nil *BallTree receiver.
//	  - Degenerate inputs (empty sets, zero-dimensional points) are handled
//	    gracefully without panics.
//	  - Internal helpers return safe zero values when invariants are violated.
//	  - Search routines are resilient to partially malformed internal state.
//
// Notes:
//   - Distance calculations assume Euclidean geometry.
//   - Tree shape depends on point distribution and split heuristics.
//   - Leaf nodes store point indices directly; internal nodes recurse.
//   - Query results are deterministic for fixed input ordering.
//
// -----------------------------------------------------------------------------
package balltree

import (
	"errors"
	"math"
	"sort"
)

// BallTree represents a spatial index over a fixed set of points.
// The tree supports k-nearest neighbor and radius-based queries.
type BallTree struct {
	points        [][]float64
	root          *Node
	leafSizeLimit int
}

// computeMeanCenter computes the centroid of the points referenced by the
// provided indices. The center is computed as the arithmetic mean across
// each dimension.
//
// Defensive behavior:
//   - Returns nil if the receiver is nil.
//   - Returns nil if no indices are provided.
//   - Returns nil if the point set is empty or zero-dimensional.
func (tree *BallTree) computeMeanCenter(indices []int) []float64 {
	// Ensure the tree exists, indices are provided, and points are available.
	if tree == nil || len(indices) == 0 || len(tree.points) == 0 {
		return nil
	}

	// Determine dimensionality from the first point.
	dimensions := len(tree.points[0])

	// Reject zero-dimensional points.
	if dimensions == 0 {
		return nil
	}

	centerPoint := make([]float64, dimensions)

	// Accumulate coordinate values across all indexed points.
	for _, pointIndex := range indices {
		point := tree.points[pointIndex]

		for dimensionIndex := 0; dimensionIndex < dimensions; dimensionIndex++ {
			centerPoint[dimensionIndex] += point[dimensionIndex]
		}
	}

	// Convert accumulated sums into an arithmetic mean.
	inverseCount := 1.0 / float64(len(indices))

	for dimensionIndex := 0; dimensionIndex < dimensions; dimensionIndex++ {
		centerPoint[dimensionIndex] *= inverseCount
	}

	return centerPoint
}

// computeRadius computes the maximum Euclidean distance from the given center
// point to any point referenced by the provided indices.
//
// Defensive behavior:
//   - Returns 0 if the receiver is nil.
//   - Returns 0 if the center point is nil.
//   - Returns 0 if no indices are provided.
func (tree *BallTree) computeRadius(centerPoint []float64, indices []int) float64 {
	// Ensure the tree exists, a center point is provided, and indices are available.
	if tree == nil || centerPoint == nil || len(indices) == 0 {
		return 0.0
	}

	var maximumDistance float64

	// Compute the maximum distance from the center to any indexed point.
	for _, pointIndex := range indices {
		distance := euclideanDistance(centerPoint, tree.points[pointIndex])

		if distance > maximumDistance {
			maximumDistance = distance
		}
	}

	return maximumDistance
}

// findFarthestPointIndex returns the index of the point farthest from the
// reference point identified by the given index.
//
// Defensive behavior:
//   - Returns -1 if the receiver is nil.
//   - Returns -1 if the point set or indices slice is empty.
//   - Returns -1 if the reference index is invalid.
//   - Skips invalid candidate indices silently.
func (tree *BallTree) findFarthestPointIndex(index int, indices []int) int {
	// Ensure the tree exists and valid data is available.
	if tree == nil || len(indices) == 0 || len(tree.points) == 0 {
		return -1
	}

	// Ensure the reference index is within bounds.
	if index < 0 || index >= len(tree.points) {
		return -1
	}

	referencePoint := tree.points[index]
	farthestPointIndex := -1

	var farthestSquaredDistance float64

	// Scan all candidate indices to find the farthest point.
	for _, candidateIndex := range indices {
		// Skip indices that fall outside the point slice.
		if candidateIndex < 0 || candidateIndex >= len(tree.points) {
			continue
		}

		candidatePoint := tree.points[candidateIndex]
		squaredDistance := squaredEuclideanDistance(referencePoint, candidatePoint)

		if farthestPointIndex < 0 || squaredDistance > farthestSquaredDistance {
			farthestSquaredDistance = squaredDistance
			farthestPointIndex = candidateIndex
		}
	}

	return farthestPointIndex
}

// partitionByApproximateDiameter partitions a set of point indices into two
// subsets based on proximity to two approximately diametrical endpoints.
//
// The method approximates the diameter by:
//  1. Selecting an arbitrary anchor point.
//  2. Finding the farthest point from that anchor.
//  3. Finding the farthest point from the first endpoint.
//
// Points are then assigned to the closer of the two endpoints.
//
// Defensive behavior:
//   - Returns nil partitions for invalid input.
//   - Falls back to midpoint splitting if a valid diameter cannot be formed.
func (tree *BallTree) partitionByApproximateDiameter(indices []int) ([]int, []int) {
	numberOfIndices := len(indices)

	// Ensure the tree exists and indices are provided.
	if tree == nil || numberOfIndices == 0 {
		return nil, nil
	}

	// A single point cannot be meaningfully partitioned.
	if numberOfIndices == 1 {
		return append([]int(nil), indices...), nil
	}

	// Select an arbitrary anchor point.
	anchorIndex := indices[0]

	// Find an approximate diameter endpoint.
	endpointAIndex := tree.findFarthestPointIndex(anchorIndex, indices)

	if endpointAIndex < 0 {
		return nil, nil
	}

	// Find the opposite diameter endpoint.
	endpointBIndex := tree.findFarthestPointIndex(endpointAIndex, indices)

	// Fall back to midpoint splitting if a valid diameter cannot be formed.
	if endpointBIndex < 0 || endpointAIndex == endpointBIndex {
		midpoint := numberOfIndices / 2

		return append([]int(nil), indices[:midpoint]...),
			append([]int(nil), indices[midpoint:]...)
	}

	endpointAPoint := tree.points[endpointAIndex]
	endpointBPoint := tree.points[endpointBIndex]

	left := make([]int, 0, numberOfIndices/2)
	right := make([]int, 0, numberOfIndices/2)

	// Assign points based on proximity to the two diameter endpoints.
	for _, pointIndex := range indices {
		point := tree.points[pointIndex]

		distanceToEndpointA := squaredEuclideanDistance(point, endpointAPoint)
		distanceToEndpointB := squaredEuclideanDistance(point, endpointBPoint)

		if distanceToEndpointA <= distanceToEndpointB {
			left = append(left, pointIndex)
		} else {
			right = append(right, pointIndex)
		}
	}

	// If partitioning collapses, fall back to a midpoint split.
	if len(left) == 0 || len(right) == 0 {
		midpoint := len(indices) / 2
		leftCopy := append([]int(nil), indices[:midpoint]...)
		rightCopy := append([]int(nil), indices[midpoint:]...)

		return leftCopy, rightCopy
	}

	return left, right
}

// buildNode recursively constructs a Ball Tree node for the given point indices.
//
// Leaf nodes store indices directly. Internal nodes split the index set and
// recursively build child subtrees.
//
// Defensive behavior:
//   - Returns nil if the receiver is nil.
//   - Returns nil if no indices are provided.
func (tree *BallTree) buildNode(indices []int) *Node {
	// Ensure the tree exists and indices are provided.
	if tree == nil || len(indices) == 0 {
		return nil
	}

	// Compute geometric bounds for this node.
	centerPoint := tree.computeMeanCenter(indices)
	radius := tree.computeRadius(centerPoint, indices)

	node := &Node{
		center: centerPoint,
		radius: radius,
	}

	// Stop recursion once the leaf size limit is reached.
	if len(indices) <= tree.leafSizeLimit {
		node.indices = append([]int(nil), indices...)

		return node
	}

	// Partition the point set into two subsets.
	leftIndices, rightIndices := tree.partitionByApproximateDiameter(indices)

	// If partitioning fails, treat this node as a leaf.
	if len(leftIndices) == 0 || len(rightIndices) == 0 {
		node.indices = append([]int(nil), indices...)

		return node
	}

	// Recursively construct child subtrees.
	node.left = tree.buildNode(leftIndices)
	node.right = tree.buildNode(rightIndices)

	return node
}

// New constructs and returns a Ball Tree over the provided point set.
//
// Validation:
//   - Points must be non-empty.
//   - All points must have identical dimensionality.
//   - Points must have at least one dimension.
//   - Leaf size limit must be greater than zero.
func New(points [][]float64, leafSizeLimit int) (*BallTree, error) {
	numberOfPoints := len(points)

	// Ensure a non-empty point set.
	if numberOfPoints == 0 {
		return nil, errors.New("ball-tree points must be non-empty")
	}

	// Ensure the leaf size limit is valid.
	if leafSizeLimit <= 0 {
		return nil, errors.New("ball-tree leaf size limit must be > 0")
	}

	dimensions := len(points[0])

	// Ensure points have at least one dimension.
	if dimensions == 0 {
		return nil, errors.New("ball-tree points must have at least one dimension")
	}

	// Ensure all points share the same dimensionality.
	for index := 0; index < numberOfPoints; index++ {
		if len(points[index]) != dimensions {
			return nil, errors.New("ball-tree all points must have the same dimensionality")
		}
	}

	indices := make([]int, len(points))

	for index := range indices {
		indices[index] = index
	}

	tree := &BallTree{
		points:        points,
		leafSizeLimit: leafSizeLimit,
	}

	// Build the tree recursively from all points.
	tree.root = tree.buildNode(indices)

	return tree, nil
}

// Neighbor represents a single nearest-neighbor result.
type Neighbor struct {
	Index    int
	Distance float64
}

// searchRadius performs a recursive radius-based search starting at the given node.
// Points whose distance from the query point is less than or equal to the
// specified radius are appended to the results slice.
func (tree *BallTree) searchRadius(node *Node, queryPoint []float64, radius float64, results *[]Neighbor) {
	// Stop recursion if the node does not exist.
	if node == nil {
		return
	}

	// Compute the minimum possible distance from the query to this node.
	distanceToCenter := euclideanDistance(queryPoint, node.center)

	// Prune this subtree if it cannot intersect the query radius.
	if distanceToCenter-node.radius > radius {
		return
	}

	// If this is a leaf, evaluate all contained points.
	if node.isLeaf() {
		for _, pointIndex := range node.indices {
			distance := euclideanDistance(queryPoint, tree.points[pointIndex])

			if distance <= radius {
				*results = append(*results, Neighbor{
					Index:    pointIndex,
					Distance: distance,
				})
			}
		}

		return
	}

	// Recurse into both children.
	tree.searchRadius(node.left, queryPoint, radius, results)
	tree.searchRadius(node.right, queryPoint, radius, results)
}

// searchKNearestNeighbors performs a recursive k-nearest neighbor search
// starting at the given node, using a bounded max-heap to track candidates.
func (tree *BallTree) searchKNearestNeighbors(node *Node, queryPoint []float64, neighborHeap *maximumNeighborHeap) {
	// Stop recursion if the tree or node does not exist.
	if tree == nil || node == nil {
		return
	}

	// Compute the best possible distance achievable within this node.
	distanceToCenter := euclideanDistance(queryPoint, node.center)
	minimumPossibleDistance := math.Max(0, distanceToCenter-node.radius)

	// Prune the subtree if it cannot improve the current neighbor set.
	if neighborHeap.isFull() && minimumPossibleDistance >= neighborHeap.maximumDistance() {
		return
	}

	// If this is a leaf, evaluate all contained points.
	if node.isLeaf() {
		for _, pointIndex := range node.indices {
			distance := euclideanDistance(queryPoint, tree.points[pointIndex])
			neighborHeap.tryAdd(Neighbor{Index: pointIndex, Distance: distance})
		}

		return
	}

	// Recurse into the only available child if the tree is unbalanced.
	if node.left == nil {
		tree.searchKNearestNeighbors(node.right, queryPoint, neighborHeap)

		return
	}

	if node.right == nil {
		tree.searchKNearestNeighbors(node.left, queryPoint, neighborHeap)

		return
	}

	// Visit the closer child first to maximize pruning opportunities.
	leftDistance := euclideanDistance(queryPoint, node.left.center)
	rightDistance := euclideanDistance(queryPoint, node.right.center)

	if leftDistance <= rightDistance {
		tree.searchKNearestNeighbors(node.left, queryPoint, neighborHeap)
		tree.searchKNearestNeighbors(node.right, queryPoint, neighborHeap)
	} else {
		tree.searchKNearestNeighbors(node.right, queryPoint, neighborHeap)
		tree.searchKNearestNeighbors(node.left, queryPoint, neighborHeap)
	}
}

// RadiusSearch returns all points within the specified radius of the query point.
// Results are returned in ascending order of distance.
func (tree *BallTree) RadiusSearch(queryPoint []float64, radius float64) ([]Neighbor, error) {
	// Ensure the tree exists.
	if tree == nil {
		return nil, errors.New("ball-tree is uninitialized")
	}

	// Ensure the tree contains points.
	if len(tree.points) == 0 {
		return nil, errors.New("ball-tree has no points")
	}

	// Reject negative radius queries.
	if radius < 0 {
		return nil, errors.New("ball-tree radius must be >= 0")
	}

	// Ensure the query dimensionality matches the tree.
	if len(queryPoint) != len(tree.points[0]) {
		return nil, errors.New("ball-tree query point has wrong dimensionality")
	}

	var results []Neighbor

	// Perform the recursive search.
	tree.searchRadius(tree.root, queryPoint, radius, &results)

	// Sort results by increasing distance.
	sort.Slice(results, func(compare, against int) bool {
		return results[compare].Distance < results[against].Distance
	})

	return results, nil
}

// KNearestNeighbors returns the k nearest neighbors to the query point,
// ordered by increasing distance.
func (tree *BallTree) KNearestNeighbors(queryPoint []float64, k int) ([]Neighbor, error) {
	// Ensure the tree exists.
	if tree == nil {
		return nil, errors.New("ball-tree is uninitialized")
	}

	// Reject invalid k values.
	if k <= 0 {
		return nil, errors.New("ball-tree k must be > 0")
	}

	// Ensure the query dimensionality matches the tree.
	if len(queryPoint) != len(tree.points[0]) {
		return nil, errors.New("ball-tree query point has wrong dimensionality")
	}

	neighborHeap := newMaximumNeighborHeap(k)

	// Perform the recursive k-NN search.
	tree.searchKNearestNeighbors(tree.root, queryPoint, neighborHeap)

	return neighborHeap.sortedAscending(), nil
}
