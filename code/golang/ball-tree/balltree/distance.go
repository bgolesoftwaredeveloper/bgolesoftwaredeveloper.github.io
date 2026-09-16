// -----------------------------------------------------------------------------
// Package: balltree
// File: distance.go
//
// Description:
//
//	This file provides Euclidean distance utility functions used throughout
//	the Ball Tree implementation. Both squared and non-squared distance
//	metrics are provided to support efficient geometric computations.
//
//	The squared distance function avoids the cost of a square root and is
//	preferred in situations where only relative distance comparisons are
//	required, such as during partitioning and pruning decisions.
//
//	The implementation is intentionally defensive and returns safe zero
//	values when invalid inputs are encountered.
//
// Notes:
//   - All distance computations assume Euclidean geometry.
//   - Input slices are expected to represent points in the same dimensional
//     space, but mismatches are handled gracefully.
//   - These helpers are unexported and intended for internal use only.
//
// -----------------------------------------------------------------------------
package balltree

import "math"

// squaredEuclideanDistance computes the squared Euclidean distance between
// two points represented as float64 slices.
// Squared distance is used to avoid the computational cost of square roots
// when only relative distance comparisons are required.
func squaredEuclideanDistance(point, other []float64) float64 {
	// Return a safe zero value if either point is nil.
	if point == nil || other == nil {
		return 0
	}

	// Ensure both points have the same dimensionality.
	if len(point) != len(other) {
		return 0
	}

	var squaredDistance float64

	// Accumulate squared coordinate differences across all dimensions.
	for index := 0; index < len(point); index++ {
		delta := point[index] - other[index]
		squaredDistance += delta * delta
	}

	return squaredDistance
}

// euclideanDistance computes the Euclidean distance between two points.
// This function delegates to squaredEuclideanDistance and applies a square root.
func euclideanDistance(point, other []float64) float64 {
	return math.Sqrt(squaredEuclideanDistance(point, other))
}
