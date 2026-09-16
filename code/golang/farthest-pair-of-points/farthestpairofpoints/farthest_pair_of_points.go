// -----------------------------------------------------------------------------
// Package: farthestpairbruteforce
// File: farthest_pair.go
//
// Description:
//
//	This file implements the Farthest Pair of Points problem using a
//	defensive, invariant-preserving brute-force strategy.
//
//	The algorithm evaluates all unique point pairs and computes the
//	maximum Euclidean distance observed. While the asymptotic time
//	complexity is O(n²), the implementation mirrors the structural rigor
//	of divide-and-conquer geometric algorithms to ensure correctness,
//	numerical safety, and ease of future optimization.
//
//	The implementation is structured to expose a minimal public API
//	while encapsulating all pairwise evaluation and distance computation
//	within the package. Defensive checks are applied at package boundaries
//	to guarantee mathematically neutral behavior under degenerate or
//	invalid input conditions.
//
//	The core invariants enforced by this implementation are:
//	  - All distance comparisons are performed on finite numeric inputs.
//	  - Each unordered point pair is evaluated exactly once.
//	  - Invalid or insufficient input yields a neutral zero distance.
//	  - Internal helpers are intentionally unexported to preserve invariants.
//
// Notes:
//   - The public entry point performs all input validation.
//   - Returning zero cleanly composes with distance maximization.
//   - This structure can later be upgraded to a convex-hull-based
//     rotating-calipers implementation without API changes.
//
// -----------------------------------------------------------------------------
package farthestpairofpoints

import "math"

// Point represents a two-dimensional point in Cartesian space.
// The fields are exported to allow construction by external callers.
type Point struct {
	X float64
	Y float64
}

// computeEuclideanDistance returns the Euclidean distance between two points.
// This function assumes that both points contain finite numeric values.
func computeEuclideanDistance(point, other Point) float64 {
	// Validate numeric inputs defensively.
	if math.IsNaN(point.X) || math.IsNaN(point.Y) ||
		math.IsNaN(other.X) || math.IsNaN(other.Y) ||
		math.IsInf(point.X, 0) || math.IsInf(point.Y, 0) ||
		math.IsInf(other.X, 0) || math.IsInf(other.Y, 0) {
		return 0
	}

	// Compute the difference along each axis.
	xCoordinateDifference := point.X - other.X
	yCoordinateDifference := point.Y - other.Y

	distance := xCoordinateDifference*xCoordinateDifference +
		yCoordinateDifference*yCoordinateDifference

	// Guard against unexpected non-finite results.
	if math.IsNaN(distance) || math.IsInf(distance, 0) {
		return 0.0
	}

	// Apply the Euclidean distance formula.
	return math.Sqrt(xCoordinateDifference*xCoordinateDifference +
		yCoordinateDifference*yCoordinateDifference)
}

// findFarthestPairBruteForce computes the maximum distance between all pairs
// of points using a naive O(n²) approach.
// This function encapsulates all pairwise evaluation logic.
func findFarthestPairBruteForce(collection []Point) float64 {
	if collection == nil {
		return 0
	}

	collectionLength := len(collection)

	// Fewer than two points cannot form a pair.
	if collectionLength < 2 {
		return 0
	}

	// Initialize the maximum distance to a neutral lower bound.
	maximumObservedDistance := 0.0

	// Compare every unique unordered pair of points.
	for pointIndex := 0; pointIndex < collectionLength-1; pointIndex++ {
		for nextIndex := pointIndex + 1; nextIndex < collectionLength; nextIndex++ {
			currentDistance := computeEuclideanDistance(
				collection[pointIndex],
				collection[nextIndex],
			)

			// Update the maximum distance if a farther pair is found.
			if currentDistance > maximumObservedDistance {
				maximumObservedDistance = currentDistance
			}
		}
	}

	return maximumObservedDistance
}

// FindFarthestPairDistance computes the maximum Euclidean distance between any
// two points in the provided collection.
// This function validates inputs and invokes the internal brute-force routine.
// Invalid or insufficient input yields zero rather than panicking.
func FindFarthestPairDistance(collection []Point) float64 {
	// At least two points are required.
	if len(collection) < 2 {
		return 0
	}

	// Validate that all point coordinates are finite.
	for _, candidate := range collection {
		if math.IsNaN(candidate.X) || math.IsNaN(candidate.Y) ||
			math.IsInf(candidate.X, 0) || math.IsInf(candidate.Y, 0) {
			return 0
		}
	}

	// Delegate to the internal brute-force evaluator.
	return findFarthestPairBruteForce(collection)
}
