// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This executable provides a demonstration entry point for the OPTICS
//	(Ordering Points To Identify the Clustering Structure) algorithm.
//
//	It constructs an N-dimensional spatial dataset containing two distinct point
//	clusters alongside an isolated noise/outlier vector. It then initializes and
//	executes the OPTICS algorithm, formatting and printing the resulting reachability
//	plot data (ordered sequence, core distances, and reachability distances) to stdout.
//
// Demonstrates:
//
//   - Dataset construction using the optics.Point slice abstraction.
//   - Parameter configuration (MaximumEpsilon radius and MinimumPoints core threshold).
//   - Execution via Optics.Run() to populate the density-ordered set.
//   - Pretty-printing infinite and numerical distances into a formatted output table.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"
	"math"

	"github.com/bgolesoftwaredeveloper/optics/optics"
)

func main() {
	// Define a representative 3D dataset containing two dense clusters.
	dataset := []optics.Point{
		{1.0, 1.0}, {1.1, 1.0}, {1.0, 1.2}, {1.2, 1.1},
		{8.0, 8.0}, {8.5, 8.0}, {8.0, 8.5},
		{25.0, 25.0},
	}

	// Instantiate the OPTICS engine with a maximum search radius of 10.0.
	optics := optics.NewOptics(dataset, 10.0, 3)

	// Execute density-based traversal to generate the ordered point set.
	optics.Run()

	// Output formatted table headers for the ordering plot results.
	fmt.Println("OPTICS Data:")
	fmt.Printf("%-8s | %-15s | %-15s\n", "Index", "Core Distance", "Reachability")
	fmt.Println("-------------------------------------------")

	// Iterate through the resulting ordered set to display node distances.
	for _, node := range optics.OrderedSet {
		// Format reachability distance, explicitly handling infinite values (noise / initial seed.
		reachRepresentation := fmt.Sprintf("%.3f", node.ReachabilityDistance)

		if math.IsInf(node.ReachabilityDistance, 1) {
			reachRepresentation = "∞ (UNREACHABLE)"
		}

		// Format core distance, handling infinite values (non-core points).
		coreRepresentation := fmt.Sprintf("%.3f", node.CoreDistance)

		if math.IsInf(node.CoreDistance, 1) {
			coreRepresentation = "∞"
		}

		// Print the formatted row representing the density profile of the current node.
		fmt.Printf("%-8d | %-15s | %-15s\n", node.Index, coreRepresentation, reachRepresentation)
	}
}
