// -----------------------------------------------------------------------------
// Package: closestpairdivideandconquer
// File: closest_pair_test.go
//
// Description:
//
//	This file contains unit tests for the Closest Pair of Points
//	divide-and-conquer implementation.
//
//	The tests validate correct behavior across typical inputs,
//	degenerate cases, and defensive boundary conditions, including
//	empty collections, singleton collections, invalid numeric values,
//	and known geometric configurations with deterministic outcomes.
//
//	Each test follows a clear Arrange / Act / Assert structure to
//	emphasize intent and correctness over incidental implementation
//	details.
//
// Notes:
//   - Tests rely exclusively on the public API.
//   - Floating-point comparisons account for numerical precision.
//   - Positive infinity is treated as a valid sentinel result.
//
// -----------------------------------------------------------------------------
package closestpairdivideandconquer

import (
	"math"
	"testing"
)

// TestFindClosestPairDistance verifies correctness of the public API
// across a range of valid, degenerate, and invalid input scenarios.
func TestFindClosestPairDistance(test *testing.T) {
	// Arrange.
	type testCase struct {
		name            string
		collection      []Point
		minimumDistance float64
		tolerant        bool
	}

	testCollection := []testCase{
		{
			name: "two_points_basic_distance",
			collection: []Point{
				{X: 0.0, Y: 0.0},
				{X: 3.0, Y: 4.0},
			},
			minimumDistance: 5.0,
		},
		{
			name: "known_closest_pair",
			collection: []Point{
				{X: 2.0, Y: 3.0},
				{X: 12.0, Y: 30.0},
				{X: 40.0, Y: 50.0},
				{X: 5.0, Y: 1.0},
				{X: 12.0, Y: 10.0},
				{X: 3.0, Y: 4.0},
			},
			minimumDistance: math.Sqrt(2),
			tolerant:        true,
		},
		{
			name:            "empty_collection",
			collection:      []Point{},
			minimumDistance: math.Inf(1),
		},
		{
			name: "single_point_collection",
			collection: []Point{
				{X: 1.0, Y: 1.0},
			},
			minimumDistance: math.Inf(1),
		},
		{
			name: "collection_with_nan",
			collection: []Point{
				{X: math.NaN(), Y: 0.0},
				{X: 1.0, Y: 1.0},
			},
			minimumDistance: math.Inf(1),
		},
		{
			name: "collection_with_infinity",
			collection: []Point{
				{X: 0.0, Y: math.Inf(1)},
				{X: 1.0, Y: 1.0},
			},
			minimumDistance: math.Inf(1),
		},
	}

	for index := 0; index < len(testCollection); index++ {
		currentCase := testCollection[index]

		test.Run(currentCase.name, func(test *testing.T) {
			// Act.
			computedMinimumDistance := FindClosestPairDistance(currentCase.collection)

			// Assert.
			if currentCase.tolerant {
				floatingPointTolerance := 1e-9

				if math.Abs(computedMinimumDistance-currentCase.minimumDistance) > floatingPointTolerance {
					test.Fatalf("expected distance approximately %f, but got %f",
						currentCase.minimumDistance,
						computedMinimumDistance)
				}
			} else {
				if computedMinimumDistance != currentCase.minimumDistance {
					test.Fatalf("expected distance %f, but got %f",
						currentCase.minimumDistance,
						computedMinimumDistance)
				}
			}
		})
	}
}
