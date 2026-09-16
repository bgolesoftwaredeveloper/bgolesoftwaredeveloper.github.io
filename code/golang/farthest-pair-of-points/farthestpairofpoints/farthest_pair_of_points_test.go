// -----------------------------------------------------------------------------
// Package: farthestpairofpoints
// File: farthest_pair_test.go
//
// Description:
//
//	This file contains unit tests for the Farthest Pair of Points
//	brute-force implementation.
//
//	The tests verify correct distance computation under valid input
//	conditions and confirm that defensive behavior is preserved under
//	degenerate, insufficient, or invalid inputs.
//
//	The test suite is intentionally explicit and invariant-driven,
//	favoring clarity and correctness over compactness.
//
// Notes:
//   - All tests use Arrange / Act / Assert structure.
//   - The testing parameter is named `test` by convention.
//   - Fatalf formatting follows project-specific readability rules.
//   - No tests rely on internal helpers or unexported functions.
//
// -----------------------------------------------------------------------------
package farthestpairofpoints

import (
	"math"
	"testing"
)

// TestFindFarthestPairDistance_ValidPoints verifies that the function
// correctly computes the maximum distance for a well-formed point set.
func TestFindFarthestPairDistance_ValidPoints(test *testing.T) {
	// Arrange.
	pointCollection := []Point{
		{X: 0.0, Y: 0.0},
		{X: 1.0, Y: 1.0},
		{X: -2.0, Y: 3.0},
		{X: 4.0, Y: -1.0},
		{X: -5.0, Y: -4.0},
	}

	expectedDistance := math.Sqrt(90.0)

	// Act.
	actualDistance := FindFarthestPairDistance(pointCollection)

	// Assert.
	if math.Abs(actualDistance-expectedDistance) > 1e-9 {
		test.Fatalf("expected farthest distance %.9f, but received %.9f",
			expectedDistance,
			actualDistance,
		)
	}
}

// TestFindFarthestPairDistance_InsufficientPoints verifies that fewer
// than two points yields a neutral zero distance.
func TestFindFarthestPairDistance_InsufficientPoints(test *testing.T) {
	// Arrange.
	pointCollection := []Point{
		{X: 1.0, Y: 2.0},
	}

	// Act.
	actualDistance := FindFarthestPairDistance(pointCollection)

	// Assert.
	if actualDistance != 0 {
		test.Fatalf("expected zero distance for insufficient input, but received %.9f",
			actualDistance,
		)
	}
}

// TestFindFarthestPairDistance_NilSlice verifies that a nil slice
// yields a neutral zero distance without panicking.
func TestFindFarthestPairDistance_NilSlice(test *testing.T) {
	// Arrange.
	var pointCollection []Point

	// Act.
	actualDistance := FindFarthestPairDistance(pointCollection)

	// Assert.
	if actualDistance != 0 {
		test.Fatalf("expected zero distance for nil input, but received %.9f",
			actualDistance,
		)
	}
}

// TestFindFarthestPairDistance_NonFiniteCoordinates verifies that
// NaN or infinite coordinates cause the function to return zero.
func TestFindFarthestPairDistance_NonFiniteCoordinates(test *testing.T) {
	// Arrange.
	pointCollection := []Point{
		{X: 0.0, Y: 0.0},
		{X: math.NaN(), Y: 1.0},
		{X: 2.0, Y: math.Inf(1)},
	}

	// Act.
	actualDistance := FindFarthestPairDistance(pointCollection)

	// Assert.
	if actualDistance != 0 {
		test.Fatalf("expected zero distance for non-finite input, but received %.9f",
			actualDistance,
		)
	}
}

// TestFindFarthestPairDistance_DuplicatePoints verifies that identical
// points do not inflate the computed distance.
func TestFindFarthestPairDistance_DuplicatePoints(test *testing.T) {
	// Arrange.
	pointCollection := []Point{
		{X: 2.0, Y: 2.0},
		{X: 2.0, Y: 2.0},
		{X: 2.0, Y: 2.0},
	}

	expectedDistance := 0.0

	// Act.
	actualDistance := FindFarthestPairDistance(pointCollection)

	// Assert.
	if actualDistance != expectedDistance {
		test.Fatalf("expected distance %.1f for duplicate points, but received %.9f",
			expectedDistance,
			actualDistance,
		)
	}
}

// TestFindFarthestPairDistance_TwoPoints verifies that exactly two points
// returns the distance between them.
func TestFindFarthestPairDistance_TwoPoints(test *testing.T) {
	// Arrange.
	pointCollection := []Point{
		{X: -3.0, Y: -4.0},
		{X: 3.0, Y: 4.0},
	}

	expectedDistance := 10.0

	// Act.
	actualDistance := FindFarthestPairDistance(pointCollection)

	// Assert.
	if math.Abs(actualDistance-expectedDistance) > 1e-9 {
		test.Fatalf("expected distance %.9f, but received %.9f",
			expectedDistance,
			actualDistance,
		)
	}
}

// TestFindFarthestPairDistance_CollinearPoints verifies that the farthest
// distance is computed correctly for collinear input.
func TestFindFarthestPairDistance_CollinearPoints(test *testing.T) {
	// Arrange.
	pointCollection := []Point{
		{X: -10.0, Y: 0.0},
		{X: -5.0, Y: 0.0},
		{X: 0.0, Y: 0.0},
		{X: 7.0, Y: 0.0},
		{X: 12.0, Y: 0.0},
	}

	expectedDistance := 22.0

	// Act.
	actualDistance := FindFarthestPairDistance(pointCollection)

	// Assert.
	if math.Abs(actualDistance-expectedDistance) > 1e-9 {
		test.Fatalf("expected distance %.9f, but received %.9f",
			expectedDistance,
			actualDistance,
		)
	}
}

// TestFindFarthestPairDistance_SymmetricSquare verifies that diagonally
// opposite points produce the maximum distance.
func TestFindFarthestPairDistance_SymmetricSquare(test *testing.T) {
	// Arrange.
	pointCollection := []Point{
		{X: -1.0, Y: -1.0},
		{X: -1.0, Y: 1.0},
		{X: 1.0, Y: -1.0},
		{X: 1.0, Y: 1.0},
	}

	expectedDistance := math.Sqrt(8.0)

	// Act.
	actualDistance := FindFarthestPairDistance(pointCollection)

	// Assert.
	if math.Abs(actualDistance-expectedDistance) > 1e-9 {
		test.Fatalf("expected distance %.9f, but received %.9f",
			expectedDistance,
			actualDistance,
		)
	}
}

// TestFindFarthestPairDistance_LargeCoordinates verifies that large
// coordinate magnitudes do not overflow or degrade accuracy.
func TestFindFarthestPairDistance_LargeCoordinates(test *testing.T) {
	// Arrange.
	pointCollection := []Point{
		{X: -1e9, Y: -1e9},
		{X: 1e9, Y: 1e9},
		{X: 0.0, Y: 0.0},
	}

	expectedDistance := math.Sqrt(8e18)

	// Act.
	actualDistance := FindFarthestPairDistance(pointCollection)

	// Assert.
	if math.Abs(actualDistance-expectedDistance) > 1e3 {
		test.Fatalf("expected distance %.3f, but received %.3f",
			expectedDistance,
			actualDistance,
		)
	}
}

// TestFindFarthestPairDistance_MixedValidAndInvalid verifies that the
// presence of any non-finite point invalidates the entire computation.
func TestFindFarthestPairDistance_MixedValidAndInvalid(test *testing.T) {
	// Arrange.
	pointCollection := []Point{
		{X: 0.0, Y: 0.0},
		{X: 10.0, Y: 10.0},
		{X: math.Inf(-1), Y: 5.0},
	}

	// Act.
	actualDistance := FindFarthestPairDistance(pointCollection)

	// Assert.
	if actualDistance != 0 {
		test.Fatalf("expected zero distance due to invalid input, but received %.9f",
			actualDistance,
		)
	}
}

// TestFindFarthestPairDistance_RepeatedExtremePoints verifies that
// repeated extreme points do not affect the maximum distance result.
func TestFindFarthestPairDistance_RepeatedExtremePoints(test *testing.T) {
	// Arrange.
	pointCollection := []Point{
		{X: -100.0, Y: 0.0},
		{X: -100.0, Y: 0.0},
		{X: 100.0, Y: 0.0},
		{X: 100.0, Y: 0.0},
	}

	expectedDistance := 200.0

	// Act.
	actualDistance := FindFarthestPairDistance(pointCollection)

	// Assert.
	if math.Abs(actualDistance-expectedDistance) > 1e-9 {
		test.Fatalf("expected distance %.9f, but received %.9f",
			expectedDistance,
			actualDistance,
		)
	}
}
