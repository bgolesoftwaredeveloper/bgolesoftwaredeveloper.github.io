// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file provides a demonstration entry point for the
//	closestpairdivideandconquer package.
//
//	The program constructs a small set of two-dimensional points,
//	invokes the Closest Pair of Points algorithm, and prints the
//	computed minimum Euclidean distance between any two points.
//
//	This file is intentionally simple and focuses on illustrating
//	correct usage of the public API rather than algorithmic details.
//
// Notes:
//   - This program assumes the closestpairdivideandconquer package
//     has already validated all inputs defensively.
//   - The demonstration dataset includes points with varying spatial
//     separation to exercise both local and cross-partition behavior.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/closest-pair-divide-and-conquer/closestpairdivideandconquer"
)

func main() {
	// -------------------------------------------------------------------------
	// Demonstration Data Setup
	// -------------------------------------------------------------------------
	// Define a sample collection of points in two-dimensional space.
	// These points are intentionally distributed to ensure that the
	// closest pair is not trivially adjacent in the input ordering.
	points := []closestpairdivideandconquer.Point{
		{X: 2.0, Y: 3.0},
		{X: 12.0, Y: 30.0},
		{X: 40.0, Y: 50.0},
		{X: 5.0, Y: 1.0},
		{X: 12.0, Y: 10.0},
		{X: 3.0, Y: 4.0},
	}

	// -------------------------------------------------------------------------
	// Algorithm Invocation
	// -------------------------------------------------------------------------
	// Invoke the Closest Pair of Points algorithm.
	// The function returns the minimum Euclidean distance between any
	// two points in the collection, or positive infinity if no valid
	// pair exists.
	closestDistance := closestpairdivideandconquer.FindClosestPairDistance(points)

	// -------------------------------------------------------------------------
	// Result Reporting
	// -------------------------------------------------------------------------
	// Print the computed distance to standard output.
	// The formatting limits the output to four decimal places for clarity.
	fmt.Printf("Closest pair distance: %.4f\n", closestDistance)
}
