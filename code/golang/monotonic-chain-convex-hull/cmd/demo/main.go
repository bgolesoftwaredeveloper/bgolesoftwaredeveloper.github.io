// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file demonstrates usage of the Monotonic Chain convex hull
//	implementation by constructing a small set of two-dimensional
//	points, computing their convex hull, and inspecting the result.
//
//	The example is intentionally minimal and self-contained, focusing
//	on clarity of control flow and data transformation rather than
//	user input or visualization.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/monotonic-chain-convex-hull/monotonicchainconvexhull"
)

func main() {
	// Define a set of input points in the plane.
	// This set intentionally includes interior points and points
	// that lie on the boundary of the convex hull.
	inputPoints := []monotonicchainconvexhull.Point{
		{X: 0, Y: 0},
		{X: 2, Y: 0},
		{X: 2, Y: 2},
		{X: 0, Y: 2},
		{X: 1, Y: 1}, // Interior point.
		{X: 1, Y: 0}, // Boundary collinear point.
	}

	// Specify whether collinear boundary points should be retained
	// on the convex hull. Setting this to false excludes interior
	// collinear points and preserves only extreme endpoints.
	includeCollinearBoundaryPoints := false

	// Compute the convex hull of the input point set using the
	// Monotonic Chain algorithm.
	convexHullVertices := monotonicchainconvexhull.MonotonicChainConvexHull(
		inputPoints,
		includeCollinearBoundaryPoints,
	)

	// Output the resulting hull vertices in order.
	// The vertices are returned in counter-clockwise order and
	// do not repeat the starting point.
	fmt.Println("Convex hull vertices:")

	for index, vertex := range convexHullVertices {
		fmt.Printf("\t%d: (%.2f, %.2f)\n",
			index,
			vertex.X,
			vertex.Y,
		)
	}
}
