// -----------------------------------------------------------------------------
// Package: octant
// File:    octree_test.go
//
// Description:
//
//	This file contains a comprehensive unit test suite for the Octree
//	implementation. It validates core functionality including spatial
//	partitioning logic, recursive subdivision triggers, range queries,
//	and nearest-neighbor search accuracy.
//
//	The tests utilize a defensive Arrange-Act-Assert (AAA) pattern to
//	ensure the tree behaves predictably under both standard and edge-case
//	conditions, such as empty trees or nil pointers.
//
// Notes:
//   - Tests utilize a mock 'test' parameter (testing.T) for reporting.
//   - Floating point comparisons follow standard math logic.
//   - Subdivision is verified by checking internal node state transitions.
//
// -----------------------------------------------------------------------------
package octant

import (
	"math"
	"testing"
)

// TestNewOctreeNodeCapacityValidation ensures that the constructor enforces
// a minimum capacity of 1, even if provided with a negative or zero value.
func TestNewOctreeNodeCapacityValidation(test *testing.T) {
	// Arrange.
	boundary := BoundingBox{CenterX: 0, CenterY: 0, CenterZ: 0, HalfExtent: 10}
	invalidCapacity := 0

	// Act.
	node := NewOctreeNode(boundary, invalidCapacity)

	// Assert.
	if node.maximumPoints < 1 {
		test.Errorf("Expected minimum capacity of 1, got %d", node.maximumPoints)
	}
}

// TestPointContainmentStrictBoundary checks if the contains logic correctly
// identifies points on the inclusive edge of the bounding box.
func TestPointContainmentStrictBoundary(test *testing.T) {
	// Arrange.
	box := BoundingBox{CenterX: 0, CenterY: 0, CenterZ: 0, HalfExtent: 5}

	edgePoint := Point{X: 5, Y: 0, Z: 0}

	// Act.
	isInside := box.contains(edgePoint)

	// Assert.
	if !isInside {
		test.Error("Expected point on the boundary edge to be contained")
	}
}

// TestSubdivisionCreation ensures that the subdivide method correctly
// initializes the children array and sets the isSubdivided flag.
func TestSubdivisionCreation(test *testing.T) {
	// Arrange.
	node := NewOctreeNode(BoundingBox{0, 0, 0, 10}, 2)

	// Act.
	node.subdivide()

	// Assert.
	if !node.isSubdivided {
		test.Error("Node should be marked as subdivided")
	}
	if node.children == nil {
		test.Fatal("Children array should not be nil after subdivision")
	}
}

// TestInsertTriggersSubdivision verifies that inserting more points than
// the maximum capacity forces the node to partition into children.
func TestInsertTriggersSubdivision(test *testing.T) {
	// Arrange.
	node := NewOctreeNode(BoundingBox{0, 0, 0, 10}, 1)

	firstPoint := Point{1, 1, 1}
	secondPoint := Point{-1, -1, -1}

	// Act.
	node.Insert(firstPoint)
	node.Insert(secondPoint)

	// Assert.
	if !node.isSubdivided {
		test.Error("Node should have subdivided after exceeding capacity")
	}
}

// TestPointRedistributionOnSplit ensures that existing points are moved
// into children when a subdivision occurs.
func TestPointRedistributionOnSplit(test *testing.T) {
	// Arrange.
	node := NewOctreeNode(BoundingBox{0, 0, 0, 10}, 1)

	node.Insert(Point{5, 5, 5})

	// Act.
	node.Insert(Point{-5, -5, -5})

	// Assert.
	if len(node.points) != 0 {
		test.Error("Root node points should be empty after redistribution")
	}
}

// TestOutOfBoundsInsertion ensures points outside the world boundary are rejected.
func TestOutOfBoundsInsertion(test *testing.T) {
	// Arrange.
	node := NewOctreeNode(BoundingBox{0, 0, 0, 5}, 10)

	distantPoint := Point{10, 10, 10}

	// Act.
	success, _ := node.Insert(distantPoint)

	// Assert.
	if success {
		test.Error("Should not be able to insert point outside of boundary")
	}
}

// TestQueryRangeNoResults ensures empty results are returned when the search
// box does not intersect any points.
func TestQueryRangeNoResults(test *testing.T) {
	// Arrange.
	node := NewOctreeNode(BoundingBox{0, 0, 0, 10}, 5)

	node.Insert(Point{8, 8, 8})

	emptySearchBox := BoundingBox{CenterX: -5, CenterY: -5, CenterZ: -5, HalfExtent: 1}

	// Act.
	results := node.QueryRange(emptySearchBox)

	// Assert.
	if len(results) != 0 {
		test.Errorf("Expected 0 results, found %d", len(results))
	}
}

// TestQueryRangeMultiplePoints validates that range queries can correctly
// aggregate points from multiple child octants.
func TestQueryRangeMultiplePoints(test *testing.T) {
	// Arrange.
	node := NewOctreeNode(BoundingBox{0, 0, 0, 10}, 1)

	node.Insert(Point{2, 2, 2})
	node.Insert(Point{-2, -2, -2})

	wideSearchBox := BoundingBox{0, 0, 0, 5}

	// Act.
	results := node.QueryRange(wideSearchBox)

	// Assert.
	if len(results) != 2 {
		test.Errorf("Expected 2 points, got %d", len(results))
	}
}

// TestNearestNeighborInEmptyTree ensures that searching an empty tree
// returns a MaxFloat64 distance and an error/empty point.
func TestNearestNeighborInEmptyTree(test *testing.T) {
	// Arrange.
	node := NewOctreeNode(BoundingBox{0, 0, 0, 10}, 5)

	// Act.
	_, distance, _ := node.FindNearestNeighbor(Point{0, 0, 0})

	// Assert.
	if distance != math.MaxFloat64 {
		test.Error("Nearest distance in empty tree should be MaxFloat64")
	}
}

// TestNearestNeighborAccuracy checks if the search correctly finds the
// mathematically closest point among several candidates.
func TestNearestNeighborAccuracy(test *testing.T) {
	// Arrange.
	node := NewOctreeNode(BoundingBox{0, 0, 0, 100}, 2)

	node.Insert(Point{10, 10, 10})
	node.Insert(Point{80, 80, 80})

	target := Point{15, 15, 15}

	// Act.
	nearest, _, _ := node.FindNearestNeighbor(target)

	// Assert.
	if nearest.X != 10 {
		test.Errorf("Expected {10,10,10}, got %+v", nearest)
	}
}

// TestClearResetsTreeState verifies that calling Clear empties points and
// removes subdivision children.
func TestClearResetsTreeState(test *testing.T) {
	// Arrange.
	node := NewOctreeNode(BoundingBox{0, 0, 0, 10}, 1)

	node.Insert(Point{1, 1, 1})
	node.Insert(Point{-1, -1, -1})

	// Act.
	node.Clear()

	// Assert.
	if node.isSubdivided {
		test.Error("Tree should not be subdivided after Clear")
	}
	if len(node.points) != 0 {
		test.Error("Tree points should be empty after Clear")
	}
}

// TestBoxIntersectionMiss ensures that non-overlapping boxes are identified
// as such to prevent unnecessary recursion.
func TestBoxIntersectionMiss(test *testing.T) {
	// Arrange.
	boxA := BoundingBox{0, 0, 0, 5}
	boxB := BoundingBox{20, 20, 20, 5}

	// Act.
	intersects := boxA.intersects(boxB)

	// Assert.
	if intersects {
		test.Error("Boxes should not intersect")
	}
}

// TestIsWorthSearchingOptimization verifies the distance calculation
// used for pruning branches during nearest-neighbor search.
func TestIsWorthSearchingOptimization(test *testing.T) {
	// Arrange.
	node := NewOctreeNode(BoundingBox{50, 50, 50, 10}, 5)

	target := Point{0, 0, 0}
	currentBestDistance := 10.0

	// Act.
	worthIt := node.isWorthSearching(target, currentBestDistance)

	// Assert.
	if worthIt {
		test.Error("A node 40 units away should not be worth searching if we have a point 10 units away")
	}
}

// TestPointDistanceLogic validates the basic Euclidean distance calculation.
func TestPointDistanceLogic(test *testing.T) {
	// Arrange.
	point := Point{0, 0, 0}
	otherPoint := Point{3, 4, 0}

	// Act.
	distance := point.GetDistance(otherPoint)

	// Assert.
	if distance != 5.0 {
		test.Errorf("Expected distance 5.0, got %f", distance)
	}
}

// TestNilReceiverSafety ensures that public methods handle nil node pointers
// gracefully without panicking.
func TestNilReceiverSafety(test *testing.T) {
	// Arrange.
	var nilNode *OctreeNode = nil

	// Act & Assert.
	_, err := nilNode.Insert(Point{0, 0, 0})

	if err == nil {
		test.Error("Insert on nil node should return an error")
	}

	results := nilNode.QueryRange(BoundingBox{0, 0, 0, 10})

	if len(results) != 0 {
		test.Error("QueryRange on nil node should return empty slice")
	}
}
