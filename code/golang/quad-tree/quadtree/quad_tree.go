// -----------------------------------------------------------------------------
// Package: quadtree
// File: quadtree.go
//
// Description:
//
//	This file implements a two-dimensional point QuadTree for spatial
//	indexing and axis-aligned range queries.
//
//	The implementation is defensive by design and includes explicit
//	validation of bounding boxes, subdivision geometry, and recursion
//	preconditions to prevent silent corruption or infinite subdivision.
//
//	The QuadTree stores points in leaf nodes up to a fixed capacity.
//	When capacity is exceeded, the node subdivides into four child
//	quadrants covering non-overlapping regions of space.
//
// -----------------------------------------------------------------------------
package quadtree

// Point represents a single location in two-dimensional Cartesian space.
type Point struct {
	X float64
	Y float64
}

// BoundingBox represents an axis-aligned rectangular region.
type BoundingBox struct {
	MinimumX float64
	MinimumY float64
	MaximumX float64
	MaximumY float64
}

// isValid reports whether the bounding box has a valid coordinate ordering.
// A box is considered valid if its minimum coordinates do not exceed its
// maximum coordinates.
func (box BoundingBox) isValid() bool {
	return box.MinimumX <= box.MaximumX && box.MinimumY <= box.MaximumY
}

// hasNonZeroArea reports whether the bounding box encloses a non-zero area.
// This is required for safe subdivision, as zero-area boxes cannot be split.
func (box BoundingBox) hasNonZeroArea() bool {
	return box.MinimumX < box.MaximumX &&
		box.MinimumY < box.MaximumY
}

// containsPoint reports whether the given point lies within the bounding box.
// The check is inclusive of boundary edges. Invalid bounding boxes never
// report containment.
func (box BoundingBox) containsPoint(point Point) bool {
	// Reject containment checks against invalid geometry.
	if !box.isValid() {
		return false
	}

	// Perform inclusive axis-aligned containment test.
	return point.X >= box.MinimumX && point.X <= box.MaximumX &&
		point.Y >= box.MinimumY && point.Y <= box.MaximumY
}

// intersectsBoundingBox reports whether this bounding box intersects another.
// Invalid bounding boxes are treated as non-intersecting.
func (box BoundingBox) intersectsBoundingBox(other BoundingBox) bool {
	// Reject intersection checks involving invalid geometry.
	if !box.isValid() || !other.isValid() {
		return false
	}

	// Two axis-aligned boxes do not intersect if one lies entirely
	// to one side of the other along either axis.
	return !(other.MinimumX > box.MaximumX ||
		other.MaximumX < box.MinimumX ||
		other.MinimumY > box.MaximumY ||
		other.MaximumY < box.MinimumY)
}

// QuadTree represents a recursive spatial partitioning structure
// used to store and query points in two-dimensional space.
type QuadTree struct {
	boundary     BoundingBox
	capacity     int
	points       []Point
	isSubdivided bool

	northWestChild *QuadTree
	northEastChild *QuadTree
	southWestChild *QuadTree
	southEastChild *QuadTree
}

// NewQuadTree constructs a new QuadTree node with the given spatial boundary
// and maximum point capacity. Invalid boundaries or non-positive capacities
// result in a nil return value.
func NewQuadTree(boundary BoundingBox, capacity int) *QuadTree {
	// Reject invalid bounding boxes.
	if !boundary.isValid() {
		return nil
	}

	// Reject non-positive capacities to avoid degenerate trees.
	if capacity <= 0 {
		return nil
	}

	// Initialize an empty QuadTree node.
	return &QuadTree{
		boundary: boundary,
		capacity: capacity,
		points:   make([]Point, 0, capacity),
	}
}

// subdivide splits the current QuadTree node into four child quadrants.
// Subdivision is aborted if the node is already subdivided, has invalid
// geometry, or cannot be safely split due to precision limits.
func (tree *QuadTree) subdivide() {
	// Reject subdivision on nil receivers.
	if tree == nil {
		return
	}

	// Prevent duplicate subdivision.
	if tree.isSubdivided {
		return
	}

	// Ensure the boundary has non-zero area.
	if !tree.boundary.hasNonZeroArea() {
		return
	}

	minimumX := tree.boundary.MinimumX
	minimumY := tree.boundary.MinimumY
	maximumX := tree.boundary.MaximumX
	maximumY := tree.boundary.MaximumY

	// Compute midpoint coordinates.
	middleX := (minimumX + maximumX) / 2.0
	middleY := (minimumY + maximumY) / 2.0

	// Guard against floating-point collapse that would prevent progress.
	if middleX == minimumX || middleX == maximumX ||
		middleY == minimumX || middleY == maximumY {
		return
	}

	// Define non-overlapping child quadrant boundaries.
	northWestBoundary := BoundingBox{
		MinimumX: minimumX,
		MinimumY: middleY,
		MaximumX: middleX,
		MaximumY: maximumY,
	}

	northEastBoundary := BoundingBox{
		MinimumX: middleX,
		MinimumY: middleY,
		MaximumX: maximumX,
		MaximumY: maximumY,
	}

	southWestBoundary := BoundingBox{
		MinimumX: minimumX,
		MinimumY: minimumY,
		MaximumX: middleX,
		MaximumY: middleY,
	}

	southEastBoundary := BoundingBox{
		MinimumX: middleX,
		MinimumY: minimumY,
		MaximumX: maximumX,
		MaximumY: middleY,
	}

	// Construct child QuadTree nodes.
	tree.northWestChild = NewQuadTree(northWestBoundary, tree.capacity)
	tree.northEastChild = NewQuadTree(northEastBoundary, tree.capacity)
	tree.southWestChild = NewQuadTree(southWestBoundary, tree.capacity)
	tree.southEastChild = NewQuadTree(southEastBoundary, tree.capacity)

	// Abort subdivision if any child failed to initialize.
	if tree.northWestChild == nil || tree.northEastChild == nil ||
		tree.southWestChild == nil || tree.southEastChild == nil {
		return
	}

	// Mark the node as successfully subdivided.
	tree.isSubdivided = true
}

// InsertPoint attempts to insert a point into the QuadTree.
// The function returns false if the point lies outside the tree boundary
// or if insertion fails due to subdivision constraints.
func (tree *QuadTree) InsertPoint(point Point) bool {
	// Reject insertion on nil receivers.
	if tree == nil {
		return false
	}

	// Reject points outside the node boundary.
	if !tree.boundary.containsPoint(point) {
		return false
	}

	// Store the point locally if capacity allows.
	if len(tree.points) < tree.capacity {
		tree.points = append(tree.points, point)

		return true
	}

	// Subdivide the node if necessary.
	if !tree.isSubdivided {
		tree.subdivide()
	}

	// Abort insertion if subdivision failed.
	if !tree.isSubdivided {
		return false
	}

	// Delegate insertion to child quadrants.
	if tree.northWestChild.InsertPoint(point) {
		return true
	}

	if tree.northEastChild.InsertPoint(point) {
		return true
	}

	if tree.southWestChild.InsertPoint(point) {
		return true
	}

	if tree.southEastChild.InsertPoint(point) {
		return true
	}

	// Point did not fit into any child quadrant.
	return false
}

// QueryRange appends all points contained within the given bounding box
// to the provided result slice. The function safely aborts on invalid
// geometry or non-intersecting regions.
func (tree *QuadTree) QueryRange(region BoundingBox, results *[]Point) {
	// Reject invalid receivers or result destinations.
	if tree == nil || results == nil {
		return
	}

	// Reject invalid query regions.
	if !region.isValid() {
		return
	}

	// Abort traversal if the query region does not intersect this node.
	if !tree.boundary.intersectsBoundingBox(region) {
		return
	}

	// Collect points stored directly in this node.
	for _, point := range tree.points {
		if region.containsPoint(point) {
			*results = append(*results, point)
		}
	}

	// Terminate recursion if the node has not been subdivided.
	if !tree.isSubdivided {
		return
	}

	// Recursively query child quadrants.
	if tree.northWestChild != nil {
		tree.northWestChild.QueryRange(region, results)
	}

	if tree.northEastChild != nil {
		tree.northEastChild.QueryRange(region, results)
	}

	if tree.southWestChild != nil {
		tree.southWestChild.QueryRange(region, results)
	}

	if tree.southEastChild != nil {
		tree.southEastChild.QueryRange(region, results)
	}
}
