// -----------------------------------------------------------------------------
// Package: grahamscan
// File: graham_scan_test.go
//
// Description:
//
//	This file contains unit tests for the Graham Scan convex hull
//	implementation. The tests validate correctness across common,
//	degenerate, and edge-case point configurations.
//
//	Each test is structured using explicit Arrange / Act / Assert
//	sections to make intent and expectations unambiguous.
//
// -----------------------------------------------------------------------------
package grahamscan

import "testing"

// TestGrahamScan_SquareWithInteriorPoints verifies that interior points
// are excluded and the correct convex hull is produced for a square.
func TestGrahamScan_SquareWithInteriorPoints(test *testing.T) {
	// Arrange.
	points := []Point{
		{X: 0, Y: 0},
		{X: 0, Y: 3},
		{X: 3, Y: 0},
		{X: 3, Y: 3},
		{X: 1, Y: 1},
		{X: 2, Y: 1},
		{X: 2, Y: 2},
	}

	expectedHull := []Point{
		{X: 0, Y: 0},
		{X: 3, Y: 0},
		{X: 3, Y: 3},
		{X: 0, Y: 3},
	}

	// Act.
	actualHull := GrahamScan(points)

	// Assert.
	if len(actualHull) != len(expectedHull) {
		test.Fatalf("unexpected hull size: expected %d, got %d",
			len(expectedHull),
			len(actualHull),
		)
	}

	for index := range expectedHull {
		if actualHull[index] != expectedHull[index] {
			test.Fatalf("unexpected hull point at index %d: expected %+v, got %+v",
				index,
				expectedHull[index],
				actualHull[index],
			)
		}
	}
}

// TestGrahamScan_Triangle verifies that a minimal convex hull
// remains unchanged for a simple triangle.
func TestGrahamScan_Triangle(test *testing.T) {
	// Arrange.
	points := []Point{
		{X: 0, Y: 0},
		{X: 2, Y: 0},
		{X: 1, Y: 2},
	}

	// Act.
	hull := GrahamScan(points)

	// Assert.
	if len(hull) != 3 {
		test.Fatalf("expected hull size 3, got %d", len(hull))
	}
}

// TestGrahamScan_AllCollinear verifies that collinear points
// collapse to a valid hull without panic or corruption.
func TestGrahamScan_AllCollinear(test *testing.T) {
	// Arrange.
	points := []Point{
		{X: 0, Y: 0},
		{X: 1, Y: 1},
		{X: 2, Y: 2},
		{X: 3, Y: 3},
	}

	// Act.
	hull := GrahamScan(points)

	// Assert.
	if len(hull) < 2 {
		test.Fatalf("expected at least 2 hull points, got %d", len(hull))
	}
}

// TestGrahamScan_DuplicatePoints verifies that duplicate points
// do not cause incorrect hull construction or runtime errors.
func TestGrahamScan_DuplicatePoints(test *testing.T) {
	// Arrange.
	points := []Point{
		{X: 0, Y: 0},
		{X: 0, Y: 0},
		{X: 3, Y: 0},
		{X: 3, Y: 3},
		{X: 0, Y: 3},
		{X: 0, Y: 3},
	}

	// Act.
	hull := GrahamScan(points)

	// Assert.
	if len(hull) < 3 {
		test.Fatalf("expected hull size >= 3, got %d", len(hull))
	}
}

// TestGrahamScan_NilInput verifies that a nil slice
// is handled defensively and returns nil.
func TestGrahamScan_NilInput(test *testing.T) {
	// Arrange.
	var points []Point

	// Act.
	hull := GrahamScan(points)

	// Assert.
	if hull != nil {
		test.Fatalf("expected nil hull for nil input, got %+v", hull)
	}
}

// TestGrahamScan_SmallInput verifies that inputs with fewer than
// three points are returned unchanged.
func TestGrahamScan_SmallInput(test *testing.T) {
	// Arrange.
	points := []Point{
		{X: 1, Y: 1},
		{X: 2, Y: 2},
	}

	// Act.
	hull := GrahamScan(points)

	// Assert.
	if len(hull) != len(points) {
		test.Fatalf("expected hull size %d, got %d", len(points), len(hull))
	}
}

// TestGrahamScan_ConcaveShape verifies that concave points
// are excluded from the convex hull.
func TestGrahamScan_ConcaveShape(test *testing.T) {
	// Arrange.
	points := []Point{
		{X: 0, Y: 0},
		{X: 4, Y: 0},
		{X: 4, Y: 4},
		{X: 0, Y: 4},
		{X: 2, Y: 1},
		{X: 2, Y: 2},
		{X: 2, Y: 3},
	}

	// Act.
	hull := GrahamScan(points)

	// Assert.
	if len(hull) != 4 {
		test.Fatalf("expected hull size 4, got %d", len(hull))
	}
}

// TestGrahamScan_PointsAlreadyOnHull verifies that when all points
// lie on the convex hull, none are incorrectly removed.
func TestGrahamScan_PointsAlreadyOnHull(test *testing.T) {
	// Arrange.
	points := []Point{
		{X: 0, Y: 0},
		{X: 2, Y: 0},
		{X: 3, Y: 1},
		{X: 2, Y: 3},
		{X: 0, Y: 2},
	}

	// Act.
	hull := GrahamScan(points)

	// Assert.
	if len(hull) != len(points) {
		test.Fatalf("expected hull size %d, got %d", len(points), len(hull))
	}
}

// TestGrahamScan_PivotSelection verifies that the lowest-leftmost
// point is always the first point in the hull.
func TestGrahamScan_PivotSelection(test *testing.T) {
	// Arrange.
	points := []Point{
		{X: 2, Y: 2},
		{X: 1, Y: 1},
		{X: 3, Y: 3},
		{X: 0, Y: 0},
		{X: 2, Y: 0},
	}

	// Act.
	hull := GrahamScan(points)

	// Assert.
	if hull[0] != (Point{X: 0, Y: 0}) {
		test.Fatalf("expected pivot (0,0) as first hull point, got %+v", hull[0])
	}
}

// TestGrahamScan_CounterClockwiseOrder verifies that consecutive
// hull points form only left turns.
func TestGrahamScan_CounterClockwiseOrder(test *testing.T) {
	// Arrange.
	points := []Point{
		{X: 0, Y: 0},
		{X: 3, Y: 0},
		{X: 3, Y: 3},
		{X: 0, Y: 3},
		{X: 1, Y: 1},
	}

	// Act.
	hull := GrahamScan(points)

	// Assert.
	for index := 0; index < len(hull)-2; index++ {
		orientation := crossProduct(hull[index], hull[index+1], hull[index+2])

		if orientation <= 0 {
			test.Fatalf("expected left turn at indices %d,%d,%d, got orientation %.2f",
				index,
				index+1,
				index+2,
				orientation,
			)
		}
	}
}

// TestGrahamScan_ReversedInputOrder verifies that input ordering
// does not affect hull correctness.
func TestGrahamScan_ReversedInputOrder(test *testing.T) {
	// Arrange.
	points := []Point{
		{X: 0, Y: 3},
		{X: 3, Y: 3},
		{X: 3, Y: 0},
		{X: 0, Y: 0},
		{X: 1, Y: 1},
	}

	// Act.
	hull := GrahamScan(points)

	// Assert.
	if len(hull) != 4 {
		test.Fatalf("expected hull size 4, got %d", len(hull))
	}
}

// TestGrahamScan_LargeCoordinateValues verifies that large coordinate
// magnitudes do not break orientation or distance calculations.
func TestGrahamScan_LargeCoordinateValues(test *testing.T) {
	// Arrange.
	points := []Point{
		{X: -1_000_000, Y: -1_000_000},
		{X: 1_000_000, Y: -1_000_000},
		{X: 1_000_000, Y: 1_000_000},
		{X: -1_000_000, Y: 1_000_000},
		{X: 0, Y: 0},
	}

	// Act.
	hull := GrahamScan(points)

	// Assert.
	if len(hull) != 4 {
		test.Fatalf("expected hull size 4, got %d", len(hull))
	}
}

// TestGrahamScan_SinglePoint verifies that a single-point input
// is returned unchanged.
func TestGrahamScan_SinglePoint(test *testing.T) {
	// Arrange.
	points := []Point{
		{X: 5, Y: 5},
	}

	// Act.
	hull := GrahamScan(points)

	// Assert.
	if len(hull) != 1 {
		test.Fatalf("expected hull size 1, got %d", len(hull))
	}

	if hull[0] != points[0] {
		test.Fatalf("expected hull point %+v, got %+v", points[0], hull[0])
	}
}
