// -----------------------------------------------------------------------------
// Package: quadtree
// File: quadtree_test.go
//
// Description:
//
//	This file contains unit tests for the QuadTree spatial data structure.
//	The tests validate correct behavior of point insertion, spatial
//	subdivision, and axis-aligned range queries.
//
//	The tests are written in a clear Arrange / Act / Assert style and favor
//	explicit variable naming and defensive checks over brevity.
//
// Intended Coverage:
//
//   - QuadTree initialization.
//   - Point insertion within and outside boundaries.
//   - Automatic subdivision when capacity is exceeded.
//   - Correct range query results.
//
// -----------------------------------------------------------------------------
package quadtree

import "testing"

// TestNewQuadTree_EmptyTree verifies that a newly constructed QuadTree
// starts in a valid and empty state.
func TestNewQuadTree_EmptyTree(test *testing.T) {
	// Arrange.
	worldBoundary := BoundingBox{
		MinimumX: 0.0,
		MinimumY: 0.0,
		MaximumX: 100.0,
		MaximumY: 100.0,
	}

	// Act.
	quadTree := NewQuadTree(worldBoundary, 4)

	// Assert.
	if quadTree == nil {
		test.Fatal("expected QuadTree instance to be non-nil")
	}

	if len(quadTree.points) != 0 {
		test.Fatalf("expected QuadTree to contain 0 points, got %d", len(quadTree.points))
	}

	if quadTree.isSubdivided {
		test.Fatal("expected QuadTree to not be subdivided initially")
	}
}

// TestInsertPoint_WithinBoundary verifies that points inside the
// QuadTree boundary are successfully inserted.
func TestInsertPoint_WithinBoundary(test *testing.T) {
	// Arrange.
	worldBoundary := BoundingBox{
		MinimumX: 0.0,
		MinimumY: 0.0,
		MaximumX: 50.0,
		MaximumY: 50.0,
	}

	quadTree := NewQuadTree(worldBoundary, 4)

	pointToInsert := Point{
		X: 25.0,
		Y: 25.0,
	}

	// Act.
	insertionSucceeded := quadTree.InsertPoint(pointToInsert)

	// Assert.
	if !insertionSucceeded {
		test.Fatalf("expected insertion to succeed for point %+v", pointToInsert)
	}
}

// TestInsertPoint_OutsideBoundary verifies that points outside the
// QuadTree boundary are rejected.
func TestInsertPoint_OutsideBoundary(test *testing.T) {
	// Arrange.
	worldBoundary := BoundingBox{
		MinimumX: 0.0,
		MinimumY: 0.0,
		MaximumX: 50.0,
		MaximumY: 50.0,
	}

	quadTree := NewQuadTree(worldBoundary, 4)

	outsidePoint := Point{
		X: 75.0,
		Y: 75.0,
	}

	// Act.
	insertionSucceeded := quadTree.InsertPoint(outsidePoint)

	// Assert.
	if insertionSucceeded {
		test.Fatalf("expected insertion to fail for out-of-bounds point %+v", outsidePoint)
	}
}

// TestInsertPoint_TriggersSubdivision verifies that the QuadTree
// subdivides once its node capacity is exceeded.
func TestInsertPoint_TriggersSubdivision(test *testing.T) {
	// Arrange.
	worldBoundary := BoundingBox{
		MinimumX: 0.0,
		MinimumY: 0.0,
		MaximumX: 100.0,
		MaximumY: 100.0,
	}

	quadTree := NewQuadTree(worldBoundary, 1)

	firstPoint := Point{X: 10.0, Y: 10.0}
	secondPoint := Point{X: 90.0, Y: 90.0}

	// Act.
	firstInsertSucceeded := quadTree.InsertPoint(firstPoint)
	secondInsertSucceeded := quadTree.InsertPoint(secondPoint)

	// Assert.
	if !firstInsertSucceeded || !secondInsertSucceeded {
		test.Fatal("expected both point insertions to succeed")
	}

	if !quadTree.isSubdivided {
		test.Fatal("expected QuadTree to be subdivided after capacity was exceeded")
	}
}

// TestQueryRange_ReturnsExpectedPoints verifies that a range query
// returns only points contained within the query region.
func TestQueryRange_ReturnsExpectedPoints(test *testing.T) {
	// Arrange.
	worldBoundary := BoundingBox{
		MinimumX: 0.0,
		MinimumY: 0.0,
		MaximumX: 100.0,
		MaximumY: 100.0,
	}

	quadTree := NewQuadTree(worldBoundary, 2)

	pointsToInsert := []Point{
		{X: 10.0, Y: 10.0},
		{X: 20.0, Y: 20.0},
		{X: 60.0, Y: 60.0},
	}

	for _, point := range pointsToInsert {
		if !quadTree.InsertPoint(point) {
			test.Fatalf("failed to insert point during test setup: %+v", point)
		}
	}

	queryRegion := BoundingBox{
		MinimumX: 0.0,
		MinimumY: 0.0,
		MaximumX: 50.0,
		MaximumY: 50.0,
	}

	// Act.
	var queryResults []Point

	quadTree.QueryRange(queryRegion, &queryResults)

	// Assert.
	expectedResultCount := 2

	if len(queryResults) != expectedResultCount {
		test.Fatalf("expected %d points in query result, got %d", expectedResultCount, len(queryResults))
	}

	for _, resultPoint := range queryResults {
		if !queryRegion.containsPoint(resultPoint) {
			test.Fatalf("query returned point outside expected region: %+v", resultPoint)
		}
	}
}

// TestQueryRange_EmptyTree verifies that querying an empty QuadTree
// returns no results.
func TestQueryRange_EmptyTree(test *testing.T) {
	// Arrange.
	worldBoundary := BoundingBox{
		MinimumX: 0.0,
		MinimumY: 0.0,
		MaximumX: 100.0,
		MaximumY: 100.0,
	}

	quadTree := NewQuadTree(worldBoundary, 4)

	queryRegion := BoundingBox{
		MinimumX: 25.0,
		MinimumY: 25.0,
		MaximumX: 75.0,
		MaximumY: 75.0,
	}

	// Act.
	var queryResults []Point

	quadTree.QueryRange(queryRegion, &queryResults)

	// Assert.
	if len(queryResults) != 0 {
		test.Fatalf("expected no points in query result, got %d", len(queryResults))
	}
}

// TestQueryRange_NoMatchingPoints verifies that a query region
// containing no points returns an empty result.
func TestQueryRange_NoMatchingPoints(test *testing.T) {
	// Arrange.
	worldBoundary := BoundingBox{
		MinimumX: 0.0,
		MinimumY: 0.0,
		MaximumX: 100.0,
		MaximumY: 100.0,
	}

	quadTree := NewQuadTree(worldBoundary, 2)

	pointsToInsert := []Point{
		{X: 10.0, Y: 10.0},
		{X: 90.0, Y: 90.0},
	}

	for _, point := range pointsToInsert {
		if !quadTree.InsertPoint(point) {
			test.Fatalf("failed to insert point during test setup: %+v", point)
		}
	}

	queryRegion := BoundingBox{
		MinimumX: 40.0,
		MinimumY: 40.0,
		MaximumX: 60.0,
		MaximumY: 60.0,
	}

	// Act.
	var queryResults []Point

	quadTree.QueryRange(queryRegion, &queryResults)

	// Assert.
	if len(queryResults) != 0 {
		test.Fatalf("expected no points in query result, got %d", len(queryResults))
	}
}

// TestInsertPoint_OnBoundaryEdge verifies that points lying exactly
// on the QuadTree boundary edges are accepted.
func TestInsertPoint_OnBoundaryEdge(test *testing.T) {
	// Arrange.
	worldBoundary := BoundingBox{
		MinimumX: 0.0,
		MinimumY: 0.0,
		MaximumX: 50.0,
		MaximumY: 50.0,
	}

	quadTree := NewQuadTree(worldBoundary, 4)

	boundaryPoint := Point{
		X: 50.0,
		Y: 25.0,
	}

	// Act.
	insertionSucceeded := quadTree.InsertPoint(boundaryPoint)

	// Assert.
	if !insertionSucceeded {
		test.Fatalf("expected insertion to succeed for boundary point %+v", boundaryPoint)
	}
}

// TestQueryRange_OnBoundaryEdge verifies that points lying on the
// query region boundary are included in results.
func TestQueryRange_OnBoundaryEdge(test *testing.T) {
	// Arrange.
	worldBoundary := BoundingBox{
		MinimumX: 0.0,
		MinimumY: 0.0,
		MaximumX: 100.0,
		MaximumY: 100.0,
	}

	quadTree := NewQuadTree(worldBoundary, 4)

	boundaryPoint := Point{
		X: 50.0,
		Y: 50.0,
	}

	if !quadTree.InsertPoint(boundaryPoint) {
		test.Fatalf("failed to insert boundary point during test setup: %+v", boundaryPoint)
	}

	queryRegion := BoundingBox{
		MinimumX: 25.0,
		MinimumY: 25.0,
		MaximumX: 50.0,
		MaximumY: 50.0,
	}

	// Act.
	var queryResults []Point

	quadTree.QueryRange(queryRegion, &queryResults)

	// Assert.
	if len(queryResults) != 1 {
		test.Fatalf("expected 1 point in query result, got %d", len(queryResults))
	}

	if queryResults[0] != boundaryPoint {
		test.Fatalf("expected boundary point %+v, got %+v", boundaryPoint, queryResults[0])
	}
}

// TestQueryRange_SpansMultipleQuadrants verifies that a query region
// spanning multiple child quadrants returns all matching points.
func TestQueryRange_SpansMultipleQuadrants(test *testing.T) {
	// Arrange.
	worldBoundary := BoundingBox{
		MinimumX: 0.0,
		MinimumY: 0.0,
		MaximumX: 100.0,
		MaximumY: 100.0,
	}

	quadTree := NewQuadTree(worldBoundary, 1)

	pointsToInsert := []Point{
		{X: 10.0, Y: 10.0},
		{X: 10.0, Y: 90.0},
		{X: 90.0, Y: 10.0},
		{X: 90.0, Y: 90.0},
	}

	for _, point := range pointsToInsert {
		if !quadTree.InsertPoint(point) {
			test.Fatalf("failed to insert point during test setup: %+v", point)
		}
	}

	queryRegion := BoundingBox{
		MinimumX: 0.0,
		MinimumY: 0.0,
		MaximumX: 100.0,
		MaximumY: 100.0,
	}

	// Act.
	var queryResults []Point

	quadTree.QueryRange(queryRegion, &queryResults)

	// Assert.
	if len(queryResults) != len(pointsToInsert) {
		test.Fatalf("expected %d points in query result, got %d", len(pointsToInsert), len(queryResults))
	}
}
