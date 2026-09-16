// ---------------------------------------------------------------------------
// Package: octant
// File:    octree.go
//
// Description:
//
//	This file implements a high-performance, 3D Octree spatial partitioning
//	structure. It is designed to manage large sets of 3D coordinates by
//	recursively subdividing space into eight octants (nodes).
//
//	Key Features:
//	- Axis-Aligned Bounding Box (AABB) collision detection.
//	- Recursive subdivision with configurable point capacity.
//	- Defensive programming with nil-receiver checks.
//	- Optimized Nearest-Neighbor search with branch pruning.
//	- Encapsulated internal state to ensure data integrity.
//
// Usage:
//
//	Initialize a new tree using NewOctreeNode, then use Insert, QueryRange,
//	and FindNearestNeighbor to manage and search your 3D data.
//
// Notes:
//   - Coordinates are represented using 64-bit floating point numbers.
//   - The tree is axis-aligned and uses a center/half-extent boundary model.
//   - Points on the exact boundary edge are treated as inclusive.
//   - Memory is managed via manual clearing or standard garbage collection.
//
// -----------------------------------------------------------------------------
package octant

import (
	"errors"
	"math"
)

// Point represents a single coordinate in 3D space.
type Point struct {
	X float64
	Y float64
	Z float64
}

// BoundingBox defines a cubic region in 3D space using a center point
// and a half-extent (the distance from the center to any face).
type BoundingBox struct {
	CenterX    float64
	CenterY    float64
	CenterZ    float64
	HalfExtent float64
}

// OctreeNode represents a single cell within the spatial tree.
// It either holds a list of points or is subdivided into eight child octants.
type OctreeNode struct {
	boundary      BoundingBox
	points        []Point
	children      *[8]*OctreeNode
	isSubdivided  bool
	maximumPoints int
}

// contains reports whether a specific Point resides within the boundaries
// of the BoundingBox.
func (box BoundingBox) contains(point Point) bool {
	// Check if the point's coordinates fall within the min/max range of each axis.
	return point.X >= box.CenterX-box.HalfExtent &&
		point.X <= box.CenterX+box.HalfExtent &&
		point.Y >= box.CenterY-box.HalfExtent &&
		point.Y <= box.CenterY+box.HalfExtent &&
		point.Z >= box.CenterZ-box.HalfExtent &&
		point.Z <= box.CenterZ+box.HalfExtent
}

// intersects reports whether two BoundingBoxes overlap in 3D space using
// Axis-Aligned Bounding Box (AABB) collision logic.
func (box BoundingBox) intersects(other BoundingBox) bool {
	// The boxes intersect if they are not separated on any of the three axes.
	return !(other.CenterX-other.HalfExtent > box.CenterX+box.HalfExtent ||
		other.CenterX+other.HalfExtent < box.CenterX-box.HalfExtent ||
		other.CenterY-other.HalfExtent > box.CenterY+box.HalfExtent ||
		other.CenterY+other.HalfExtent < box.CenterY-box.HalfExtent ||
		other.CenterZ-other.HalfExtent > box.CenterZ+box.HalfExtent ||
		other.CenterZ+other.HalfExtent < box.CenterZ-box.HalfExtent)
}

// subdivide partitions the current node into eight equal octants,
// effectively increasing the spatial resolution of the tree.
func (node *OctreeNode) subdivide() {
	// Ensure we do not subdivide an already partitioned node or a nil receiver.
	if node == nil || node.isSubdivided {
		return
	}

	x := node.boundary.CenterX
	y := node.boundary.CenterY
	z := node.boundary.CenterZ

	// The children will have half the extent of the parent.
	quarterExtent := node.boundary.HalfExtent / 2

	// Initialize the children array.
	node.children = &[8]*OctreeNode{}

	// Generate 8 children using bit-masking to represent axis combinations.
	// octantIndex bits represent: [Z][Y][X].
	for octantIndex := 0; octantIndex < 8; octantIndex++ {
		offsetX, offsetY, offsetZ := -quarterExtent, -quarterExtent, -quarterExtent

		// Shift offsets based on the bits of the current index.
		if (octantIndex & 1) != 0 {
			offsetX = quarterExtent
		}

		if (octantIndex & 2) != 0 {
			offsetY = quarterExtent
		}

		if (octantIndex & 4) != 0 {
			offsetZ = quarterExtent
		}

		childBoundary := BoundingBox{
			CenterX:    x + offsetX,
			CenterY:    y + offsetY,
			CenterZ:    z + offsetZ,
			HalfExtent: quarterExtent,
		}

		node.children[octantIndex] = NewOctreeNode(childBoundary, node.maximumPoints)
	}

	// Mark the node as subdivided to redirect future operations to children.
	node.isSubdivided = true
}

// NewOctreeNode constructs a new spatial node with the specified boundary
// and point capacity.
func NewOctreeNode(spatialBoundary BoundingBox, capacity int) *OctreeNode {
	// Guard against non-positive capacities to prevent potential logic issues.
	if capacity < 1 {
		capacity = 1
	}

	return &OctreeNode{
		boundary:      spatialBoundary,
		points:        make([]Point, 0),
		isSubdivided:  false,
		maximumPoints: capacity,
	}
}

// isWorthSearching calculates the minimum distance from a target point to
// the boundary of the node to determine if a closer point could exist within.
func (node *OctreeNode) isWorthSearching(target Point, currentBestDistance float64) bool {
	if node == nil {
		return false
	}

	// Calculate the distance from the target to the nearest point on the AABB boundary.
	deltaX := math.Max(0, math.Abs(target.X-node.boundary.CenterX)-node.boundary.HalfExtent)
	deltaY := math.Max(0, math.Abs(target.Y-node.boundary.CenterY)-node.boundary.HalfExtent)
	deltaZ := math.Max(0, math.Abs(target.Z-node.boundary.CenterZ)-node.boundary.HalfExtent)

	closestDistance := math.Sqrt(deltaX*deltaX + deltaY*deltaY + deltaZ*deltaZ)

	// If the closest possible point in this node is further than our best, skip it.
	return closestDistance < currentBestDistance
}

// searchNearest recursively traverses the tree to find the point closest to
// the target coordinate.
func (node *OctreeNode) searchNearest(target Point, bestDistance float64, bestPoint Point) (Point, float64, error) {
	// If at a leaf node, check all points stored locally.
	if !node.isSubdivided {
		for _, point := range node.points {
			currentDistance := point.GetDistance(target)

			if currentDistance < bestDistance {
				bestDistance = currentDistance
				bestPoint = point
			}
		}

		return bestPoint, bestDistance, nil
	}

	// Recursively search children, pruning branches that cannot beat the current best distance.
	for childIndex := 0; childIndex < 8; childIndex++ {
		if node.children[childIndex].isWorthSearching(target, bestDistance) {
			point, distance, err := node.children[childIndex].searchNearest(target, bestDistance, bestPoint)

			if err == nil && distance < bestDistance {
				bestDistance = distance
				bestPoint = point
			}
		}
	}

	return bestPoint, bestDistance, nil
}

// GetDistance calculates the Euclidean distance between two 3D points.
func (point Point) GetDistance(other Point) float64 {
	differenceX := point.X - other.X
	differenceY := point.Y - other.Y
	differenceZ := point.Z - other.Z

	return math.Sqrt(differenceX*differenceX + differenceY*differenceY + differenceZ*differenceZ)
}

// Insert adds a point to the Octree, subdividing the node if its capacity
// is exceeded.
func (node *OctreeNode) Insert(point Point) (bool, error) {
	// Defensive check for nil receiver.
	if node == nil {
		return false, errors.New("cannot insert into a nil Octree node")
	}

	// Ignore points that do not fall within this node's spatial boundary.
	if !node.boundary.contains(point) {
		return false, nil
	}

	// If the node has room and isn't subdivided, store the point here.
	if len(node.points) < node.maximumPoints && !node.isSubdivided {
		node.points = append(node.points, point)

		return true, nil
	}

	// If capacity is reached and we haven't split yet, subdivide the node.
	if !node.isSubdivided {
		node.subdivide()

		// Re-distribute existing points into the newly created child octants.
		for _, existingPoint := range node.points {
			for childIndex := 0; childIndex < 8; childIndex++ {
				if accepted, _ := node.children[childIndex].Insert(existingPoint); accepted {
					break
				}
			}
		}

		// Clear local points as they are now managed by children.
		node.points = nil
	}

	// Attempt to insert the new point into the appropriate child octant.
	for childIndex := 0; childIndex < 8; childIndex++ {
		if accepted, _ := node.children[childIndex].Insert(point); accepted {
			return true, nil
		}
	}

	return false, nil
}

// FindNearestNeighbor provides a public entry point for the nearest-neighbor search.
func (node *OctreeNode) FindNearestNeighbor(target Point) (Point, float64, error) {
	// Defensive check for nil receiver.
	if node == nil {
		return Point{}, 0, errors.New("cannot search a nil node")
	}

	// Initialize the search with the maximum possible distance.
	return node.searchNearest(target, math.MaxFloat64, Point{})
}

// QueryRange identifies all points within the tree that fall within the
// specified search bounding box.
func (node *OctreeNode) QueryRange(searchBox BoundingBox) []Point {
	query := make([]Point, 0)

	// Defensive check for nil receiver.
	if node == nil {
		return query
	}

	// If the search box does not intersect this node, return early.
	if !node.boundary.intersects(searchBox) {
		return query
	}

	// If at a leaf node, check contained points against the search criteria.
	if !node.isSubdivided {
		for _, candidatePoint := range node.points {
			if searchBox.contains(candidatePoint) {
				query = append(query, candidatePoint)
			}
		}
	} else {
		// If subdivided, aggregate results from intersecting children.
		for childIndex := 0; childIndex < 8; childIndex++ {
			results := node.children[childIndex].QueryRange(searchBox)
			query = append(query, results...)
		}
	}

	return query
}

// Clear recursively removes all points and child nodes from the tree,
// resetting it to an empty state.
func (node *OctreeNode) Clear() {
	// Defensive check for nil receiver.
	if node == nil {
		return
	}

	// If subdivided, clear and nil-out children to facilitate garbage collection.
	if node.isSubdivided {
		for childIndex := 0; childIndex < 8; childIndex++ {
			node.children[childIndex].Clear()
			node.children[childIndex] = nil
		}
	}

	// Reset node state.
	node.points = nil
	node.isSubdivided = false
}
