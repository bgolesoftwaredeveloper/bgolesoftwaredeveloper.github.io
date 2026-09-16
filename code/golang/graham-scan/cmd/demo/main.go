// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file provides a minimal executable entry point demonstrating
//	the use of the Graham Scan convex hull algorithm. It constructs a
//	sample point set, invokes the hull computation, and prints the
//	resulting hull vertices in order.
//
//	This file is intentionally simple and explicit. Its purpose is to
//	exercise the algorithm in a concrete, observable way rather than
//	to provide a full application or visualization.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/graham-scan/grahamscan"
)

func main() {
	// Define a sample set of 2D points.
	// The set intentionally includes interior points and
	// non-trivial geometry to exercise the hull logic.
	points := []grahamscan.Point{
		{X: 0, Y: 3},
		{X: 2, Y: 2},
		{X: 1, Y: 1},
		{X: 2, Y: 1},
		{X: 3, Y: 0},
		{X: 0, Y: 0},
		{X: 3, Y: 3},
	}

	// Compute the convex hull using the Graham Scan algorithm.
	// The returned slice contains the hull vertices in
	// counter-clockwise order.
	hull := grahamscan.GrahamScan(points)

	// Output the resulting hull to standard output.
	// This provides a simple verification mechanism without
	// requiring graphical rendering or external tooling.
	fmt.Println("Convex Hull Points (Counter-Clockwise):")

	for index, point := range hull {
		fmt.Printf("\t%d: (%.2f, %.2f)\n", index, point.X, point.Y)
	}
}
