// -----------------------------------------------------------------------------
// Package: monotonicchainconvexhull
// File: monotonic_chain_convex_hull.go
//
// Description:
//
//	This file implements the Monotonic Chain convex hull algorithm
//	(also known as Andrew’s algorithm) for computing the convex hull
//	of a set of two-dimensional points.
//
//	The implementation is defensive and deterministic. Input points
//	are lexicographically sorted (X, then Y) using a stable sort to
//	ensure consistent hull construction. Duplicate points are removed
//	prior to hull generation.
//
//	The resulting hull is returned in counter-clockwise order without
//	repeating the starting vertex.
//
// -----------------------------------------------------------------------------
package monotonicchainconvexhull

import "sort"

// Point represents a two-dimensional Cartesian coordinate.
// Coordinates are expressed using float64 values.
type Point struct {
	X float64
	Y float64
}

// removeDuplicateSortedPoints removes exact duplicate points from a slice
// that is already sorted lexicographically by X coordinate, then Y coordinate.
// The relative ordering of unique points is preserved.
func removeDuplicateSortedPoints(points []Point) []Point {
	numberOfPoints := len(points)

	// Guard against empty input.
	if numberOfPoints == 0 {
		return nil
	}

	// Allocate a result slice sized for the worst case
	// where no duplicates are present.
	uniquePoints := make([]Point, 0, numberOfPoints)

	// The first point is always unique in a non-empty sorted slice.
	uniquePoints = append(uniquePoints, points[0])

	// Scan the remaining points and append only when the point
	// differs from the most recently accepted unique point.
	for index := 1; index < numberOfPoints; index++ {
		currentPoint := points[index]
		lastUniquePoint := uniquePoints[len(uniquePoints)-1]

		if currentPoint.X == lastUniquePoint.X && currentPoint.Y == lastUniquePoint.Y {
			continue
		}

		uniquePoints = append(uniquePoints, currentPoint)
	}

	return uniquePoints
}

// computeTurnOrientation computes the signed turn orientation formed by
// three points taken in sequence.
// The orientation is determined by examining the direction of the turn
// from the first point to the second point and then to the third point.
// The returned value indicates whether the turn is counter-clockwise,
// clockwise, or collinear.
func computeTurnOrientation(point, intermediate, candidate Point) float64 {
	vectorBaseToIntermediateX := intermediate.X - point.X
	vectorBaseToIntermediateY := intermediate.Y - point.Y

	vectorBaseToCandidateX := candidate.X - point.X
	vectorBaseToCandidateY := candidate.Y - point.Y

	return (vectorBaseToIntermediateX * vectorBaseToCandidateY) -
		(vectorBaseToIntermediateY * vectorBaseToCandidateX)
}

// shouldDiscardLastHullVertex determines whether the most recently added
// vertex in the current hull should be removed based on turn orientation.
// If flag is false, both clockwise turns and collinear points are discarded,
// preserving only extreme hull endpoints.
// If flag is true, only clockwise turns are discarded, allowing collinear
// boundary points to remain on the hull.
func shouldDiscardLastHullVertex(orientation float64, flag bool) bool {
	if flag {
		return orientation < 0
	}

	return orientation <= 0
}

// removeConsecutiveDuplicateVertices removes consecutive duplicate vertices
// from a hull vertex list as a defensive cleanup step. This ensures the
// final hull contains no immediately repeated points.
func removeConsecutiveDuplicateVertices(vertices []Point) []Point {
	// Guard against empty input.
	if len(vertices) == 0 {
		return nil
	}

	numberOfVertices := len(vertices)

	// Allocate a result slice sized for the worst case
	// where no duplicates are removed.
	cleanedVertices := make([]Point, 0, numberOfVertices)

	// The first vertex is always retained.
	cleanedVertices = append(cleanedVertices, vertices[0])

	// Append vertices only when they differ from the previously
	// accepted vertex.
	for index := 1; index < numberOfVertices; index++ {
		vertex := vertices[index]
		lastVertex := cleanedVertices[len(cleanedVertices)-1]

		if vertex.X == lastVertex.X && vertex.Y == lastVertex.Y {
			continue
		}

		cleanedVertices = append(cleanedVertices, vertex)
	}

	return cleanedVertices
}

// MonotonicChainConvexHull computes the convex hull of the provided
// two-dimensional point set using the Monotonic Chain algorithm.
// The returned hull is ordered counter-clockwise and does not repeat
// the starting vertex.
// If the input contains fewer than three unique points, the unique
// points are returned directly.
func MonotonicChainConvexHull(points []Point, flag bool) []Point {
	// Guard against empty input.
	if len(points) == 0 {
		return nil
	}

	// Copy the input slice to avoid mutating caller-owned data.
	sortedWorkingPoints := make([]Point, len(points))

	copy(sortedWorkingPoints, points)

	// Sort points lexicographically by X coordinate, then Y coordinate.
	sort.SliceStable(sortedWorkingPoints, func(left, right int) bool {
		leftPoint := sortedWorkingPoints[left]
		rightPoint := sortedWorkingPoints[right]

		if leftPoint.X < rightPoint.X {
			return true
		}

		if leftPoint.X > rightPoint.X {
			return false
		}

		return leftPoint.Y < rightPoint.Y
	})

	// Remove duplicate points after sorting.
	uniqueSortedPoints := removeDuplicateSortedPoints(sortedWorkingPoints)

	// Trivial hull cases: zero, one, or two unique points.
	if len(uniqueSortedPoints) <= 2 {
		return append([]Point(nil), uniqueSortedPoints...)
	}

	numberOfUniquePoints := len(uniqueSortedPoints)
	lowerHullVertices := make([]Point, 0, numberOfUniquePoints)

	// Construct the lower hull.
	for traversalIndex := 0; traversalIndex < numberOfUniquePoints; traversalIndex++ {
		candidateVertex := uniqueSortedPoints[traversalIndex]

		for len(lowerHullVertices) >= 2 {
			hullVertex := lowerHullVertices[len(lowerHullVertices)-2]
			lastHullVertex := lowerHullVertices[len(lowerHullVertices)-1]

			turnOrientation := computeTurnOrientation(hullVertex, lastHullVertex, candidateVertex)

			if shouldDiscardLastHullVertex(turnOrientation, flag) {
				lowerHullVertices = lowerHullVertices[:len(lowerHullVertices)-1]

				continue
			}

			break
		}

		lowerHullVertices = append(lowerHullVertices, candidateVertex)
	}

	// Construct the upper hull.
	upperHullVertices := make([]Point, 0, numberOfUniquePoints)

	for reverseTraversalIndex := numberOfUniquePoints - 1; reverseTraversalIndex >= 0; reverseTraversalIndex-- {
		candidateVertex := uniqueSortedPoints[reverseTraversalIndex]

		for len(upperHullVertices) >= 2 {
			hullVertex := upperHullVertices[len(upperHullVertices)-2]
			lastUpperHullVertex := upperHullVertices[len(upperHullVertices)-1]

			turnOrientation := computeTurnOrientation(hullVertex, lastUpperHullVertex, candidateVertex)

			if shouldDiscardLastHullVertex(turnOrientation, flag) {
				upperHullVertices = upperHullVertices[:len(upperHullVertices)-1]

				continue
			}

			break
		}

		upperHullVertices = append(upperHullVertices, candidateVertex)
	}

	// Remove the last vertex from each hull to avoid duplicating endpoints.
	lowerHullVertices = lowerHullVertices[:len(lowerHullVertices)-1]
	upperHullVertices = upperHullVertices[:len(upperHullVertices)-1]

	// Concatenate the lower and upper hulls to form the complete hull.
	completedHullVertices := make([]Point, 0, len(lowerHullVertices)+len(upperHullVertices))

	completedHullVertices = append(completedHullVertices, lowerHullVertices...)
	completedHullVertices = append(completedHullVertices, upperHullVertices...)

	// Remove any remaining consecutive duplicates as a final safety measure.
	return removeConsecutiveDuplicateVertices(completedHullVertices)
}
