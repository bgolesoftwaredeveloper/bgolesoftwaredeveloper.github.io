// -----------------------------------------------------------------------------
// Package: closestpairdivideandconquer
// File: closest_pair.go
//
// Description:
//
//	This file implements the Closest Pair of Points problem using a
//	divide-and-conquer strategy with O(n log n) time complexity.
//
//	The algorithm operates by recursively partitioning a set of points
//	sorted by X-coordinate, computing the minimum pairwise distance in
//	each half, and then resolving potential cross-boundary pairs within
//	a bounded vertical strip centered at the partition line.
//
//	The implementation is structured to expose a minimal public API
//	while encapsulating all recursive logic, distance computation, and
//	invariant management within the package. Defensive checks are applied
//	at package boundaries to ensure numerical safety and algorithmic
//	correctness under degenerate or invalid input conditions.
//
//	The core invariants enforced by this implementation are:
//	  - The recursive routine always operates on points pre-sorted by
//	    X-coordinate.
//	  - Brute-force evaluation is used for subsets of size three or fewer.
//	  - Strip evaluation considers only points within the current minimum
//	    distance of the partition line and is ordered by Y-coordinate to
//	    bound comparisons.
//	  - Invalid, insufficient, or non-finite inputs yield a mathematically
//	    neutral infinite distance rather than panicking.
//
// Notes:
//   - The public entry point performs all input validation and sorting.
//   - Recursive helpers assume validated numeric input and sorted order.
//   - Returning positive infinity cleanly composes with distance minimization.
//   - Internal helpers and recursion are intentionally unexported to
//     prevent misuse and preserve algorithmic invariants.
//
// -----------------------------------------------------------------------------
package closestpairdivideandconquer

import (
	"math"
	"sort"
)

// Point represents a two-dimensional point in Cartesian space.
// The fields are exported to allow construction by external callers.
type Point struct {
	X float64
	Y float64
}

// computeEuclideanDistance returns the Euclidean distance between two points.
// This function assumes that both points contain finite numeric values.
func computeEuclideanDistance(point, other Point) float64 {
	// Compute the difference along each axis.
	xCoordinateDifference := point.X - other.X
	yCoordinateDifference := point.Y - other.Y

	// Apply the Euclidean distance formula.
	return math.Sqrt(xCoordinateDifference*xCoordinateDifference +
		yCoordinateDifference*yCoordinateDifference)
}

// findClosestPairBruteForce computes the minimum distance between all pairs
// of points using a naive O(n²) approach.
// This function is used as the base case for recursion when the input
// size is sufficiently small.
func findClosestPairBruteForce(collection []Point) float64 {
	collectionLength := len(collection)

	// Fewer than two points cannot form a pair.
	if collectionLength < 2 {
		return math.Inf(1)
	}

	// Initialize the minimum distance to positive infinity.
	minimumObservedDistance := math.Inf(1)

	// Compare every unique pair of points.
	for pointIndex := 0; pointIndex < collectionLength-1; pointIndex++ {
		for nextIndex := pointIndex + 1; nextIndex < collectionLength; nextIndex++ {
			currentDistance := computeEuclideanDistance(collection[pointIndex],
				collection[nextIndex])

			// Update the minimum distance if a closer pair is found.
			if currentDistance < minimumObservedDistance {
				minimumObservedDistance = currentDistance
			}
		}
	}

	return minimumObservedDistance
}

// findClosestDistanceInStrip computes the closest pair distance within a
// vertical strip of points.
// The input slice must be sorted by Y-coordinate. Only pairs whose vertical
// separation is less than the current minimum distance are evaluated.
func findClosestDistanceInStrip(collection []Point, minimumDistance float64) float64 {
	// A strip with fewer than two points cannot yield.
	if len(collection) < 2 {
		return minimumDistance
	}

	// Non-positive or infinite distances cannot be improved.
	if minimumDistance <= 0 || math.IsInf(minimumDistance, 1) {
		return minimumDistance
	}

	bestStripDistance := minimumDistance
	collectionLength := len(collection)

	// Compare each point to subsequent points within the Y-distance bound.
	for pointIndex := 0; pointIndex < collectionLength; pointIndex++ {
		for nextIndex := pointIndex + 1; nextIndex < collectionLength &&
			(collection[nextIndex].Y-collection[pointIndex].Y) < bestStripDistance; nextIndex++ {
			currentDistance := computeEuclideanDistance(collection[pointIndex], collection[nextIndex])

			// Update the strip minimum if a closer pair is found.
			if currentDistance < bestStripDistance {
				bestStripDistance = currentDistance
			}
		}
	}

	return bestStripDistance
}

// findClosestPairRecursive computes the closest pair distance using a
// divide-and-conquer strategy.
// The input slice must be pre-sorted by X-coordinate.
func findClosestPairRecursive(points []Point) float64 {
	numberOfPoints := len(points)

	// Fewer than two points cannot form a pair.
	if numberOfPoints < 2 {
		return math.Inf(1)
	}

	// Base case: use brute force for small subsets.
	if numberOfPoints <= 3 {
		return findClosestPairBruteForce(points)
	}

	// Partition the points around the midpoint.
	middleIndex := numberOfPoints / 2
	middlePoint := points[middleIndex]

	leftSubset := points[:middleIndex]
	rightSubset := points[middleIndex:]

	// Recursively compute the minimum distance in each half.
	leftMinimumDistance := findClosestPairRecursive(leftSubset)
	rightMinimumDistance := findClosestPairRecursive(rightSubset)

	// Determine the smaller of the two distances.
	currentMinimumDistance := math.Min(leftMinimumDistance, rightMinimumDistance)

	// If no finite distance exists, further evaluation is pointless.
	if math.IsInf(currentMinimumDistance, 1) {
		return currentMinimumDistance
	}

	var stripCollection []Point

	// Build the vertical strip of candidate points.
	for _, candidate := range points {
		if math.Abs(candidate.X-middlePoint.X) < currentMinimumDistance {
			stripCollection = append(stripCollection, candidate)
		}
	}

	// If the strip is too small, return the current minimum.
	if len(stripCollection) < 2 {
		return currentMinimumDistance
	}

	// Sort the strip by Y-coordinate to bound comparisons.
	sort.Slice(stripCollection, func(compare, against int) bool {
		return stripCollection[compare].Y < stripCollection[against].Y
	})

	// Compute the minimum distance within the strip and return the best result.
	return math.Min(currentMinimumDistance,
		findClosestDistanceInStrip(stripCollection, currentMinimumDistance))
}

// FindClosestPairDistance computes the minimum Euclidean distance between any
// two points in the provided collection.
// This function validates inputs, sorts points by X-coordinate, and initiates
// the divide-and-conquer recursion. Invalid or insufficient input yields
// positive infinity rather than panicking.
func FindClosestPairDistance(collection []Point) float64 {
	// At least two points are required.
	if len(collection) < 2 {
		return math.Inf(1)
	}

	// Validate that all point coordinates are finite.
	for _, candidate := range collection {
		if math.IsNaN(candidate.X) || math.IsNaN(candidate.Y) ||
			math.IsInf(candidate.X, 0) || math.IsInf(candidate.Y, 0) {
			return math.Inf(1)
		}
	}

	// Create a defensive copy to avoid mutating caller-owned data.
	pointCollection := make([]Point, len(collection))

	copy(pointCollection, collection)

	// Sort the points by X-coordinate as required by the algorithm.
	sort.Slice(pointCollection, func(compare, against int) bool {
		return pointCollection[compare].X < pointCollection[against].X
	})

	// Invoke the recursive divide-and-conquer routine.
	return findClosestPairRecursive(pointCollection)
}
