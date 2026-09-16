// -----------------------------------------------------------------------------
// Package: convexhull
// File: graham_scan.go
//
// Description:
//
//	This file implements the Graham Scan algorithm for computing the
//	convex hull of a finite set of 2D points. The implementation is
//	defensive, explicit, and structured to make geometric invariants
//	and algorithmic intent obvious.
//
//	The algorithm runs in O(n log n) time due to the angular sort
//	and produces the convex hull in counter-clockwise order.
//
// -----------------------------------------------------------------------------
package grahamscan

import (
	"sort"
)

type Point struct {
	X float64
	Y float64
}

// crossProduct computes the signed area of the parallelogram formed by
// vectors (origin → first) and (origin → second).
//
// The result indicates orientation:
//
//	> 0 : counter-clockwise (left turn)
//	= 0 : collinear
//	< 0 : clockwise (right turn)
func crossProduct(origin, point, other Point) float64 {
	return (point.X-origin.X)*(other.Y-origin.Y) -
		(point.Y-origin.Y)*(other.X-origin.X)
}

// squaredDistance computes the squared Euclidean distance between
// two points. Squared distance avoids unnecessary square roots
// when only relative comparisons are required.
func squaredDistance(point, other Point) float64 {
	deltaX := point.X - other.X
	deltaY := point.Y - other.Y

	return deltaX*deltaX + deltaY*deltaY
}

// findLowestPointIndex identifies the index of the point with the
// smallest Y-coordinate, breaking ties by smallest X-coordinate.
func findLowestPointIndex(points []Point) int {
	// Guard against no points.
	if len(points) == 0 {
		return 0
	}

	lowestIndex := 0

	// Iterate over the remaining points to identify the
	// lowest-leftmost point in the set.
	for index := 1; index < len(points); index++ {
		current := points[index]
		lowest := points[lowestIndex]

		// Prefer points with a strictly smaller Y-coordinate.
		// If Y-coordinates are equal, prefer the point with
		// the smaller X-coordinate to ensure deterministic
		// pivot selection.
		if current.Y < lowest.Y || (current.Y == lowest.Y && current.X < lowest.X) {
			lowestIndex = index
		}
	}

	return lowestIndex
}

// GrahamScan computes the convex hull of the provided point set using
// the Graham Scan algorithm.
// The returned slice contains the hull vertices in counter-clockwise order.
// If fewer than three unique points are provided, the input is returned as-is.
func GrahamScan(points []Point) []Point {
	// Guard against nil input slices.
	if points == nil {
		return nil
	}

	// A convex hull is undefined for fewer than three points.
	if len(points) < 3 {
		return points
	}

	// Select the pivot point with the lowest Y-coordinate.
	// Ties are broken by choosing the leftmost (lowest X) point.
	pivotIndex := findLowestPointIndex(points)
	pivot := points[pivotIndex]

	// Move the pivot point to the front of the slice to simplify sorting.
	points[0], points[pivotIndex] = points[pivotIndex], points[0]

	// Sort the remaining points by polar angle relative to the pivot.
	// Collinear points are ordered by increasing distance from the pivot.
	sort.Slice(points[1:], func(candidate, competitor int) bool {
		first := points[candidate+1]
		second := points[competitor+1]

		orientation := crossProduct(pivot, first, second)

		if orientation == 0 {
			return squaredDistance(pivot, first) < squaredDistance(pivot, second)
		}

		return orientation > 0
	})

	// Initialize the hull stack with the first three points.
	// These are guaranteed to form a left turn after sorting.
	hull := make([]Point, 0, len(points))
	hull = append(hull, points[0], points[1], points[2])

	// Process each remaining point and maintain the convex hull invariant.
	for pointIndex := 3; pointIndex < len(points); pointIndex++ {
		currentPoint := points[pointIndex]

		// Remove points from the hull while they form a non-left turn.
		for len(hull) >= 2 {
			top := hull[len(hull)-1]
			nextToTop := hull[len(hull)-2]

			if crossProduct(nextToTop, top, currentPoint) > 0 {
				break
			}

			// Pop the top of the stack if it violates convexity.
			hull = hull[:len(hull)-1]
		}

		// Append the current point, restoring the hull invariant.
		hull = append(hull, currentPoint)
	}

	return hull
}
