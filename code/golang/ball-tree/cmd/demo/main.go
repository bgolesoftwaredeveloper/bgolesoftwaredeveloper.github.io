// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file serves as the entry point for a program that demonstrates
//	usage of the Ball Tree data structure implemented in the balltree
//	package.
//
//	The Ball Tree is used to efficiently perform spatial queries such as
//	k-nearest neighbor (k-NN) searches and radius-based searches over a
//	fixed set of multi-dimensional points.
//
//	This main function is intentionally minimal and is intended as a
//	usage example rather than a comprehensive test harness.
//
// Notes:
//   - All Ball Tree validation occurs during construction and query execution.
//   - Errors returned by Ball Tree operations should be handled explicitly
//     by the caller.
//   - This file may be extended to include benchmarking or interactive input.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"
	"log"

	"github.com/bgolesoftwaredeveloper/ball-tree/balltree"
)

func main() {
	// Define a small set of two-dimensional points.
	// Each inner slice represents a single point in R².
	points := [][]float64{
		{0.0, 0.0},
		{1.0, 1.0},
		{2.0, 2.0},
		{3.0, 3.0},
		{5.0, 5.0},
		{8.0, 8.0},
	}

	// Specify the maximum number of points allowed in a leaf node.
	// Smaller values increase tree depth; larger values reduce recursion.
	leafSizeLimit := 2

	// Construct the Ball Tree from the provided point set.
	tree, err := balltree.New(points, leafSizeLimit)

	if err != nil {
		log.Fatalf("failed to construct ball tree: %v", err)
	}

	// Define a query point for spatial searches.
	queryPoint := []float64{2.5, 2.5}

	// -------------------------------------------------------------------------
	// Radius Search Demonstration
	// -------------------------------------------------------------------------
	// Specify a radius within which neighbors should be returned.
	searchRadius := 2.0

	// Perform a radius-based query.
	radiusResults, err := tree.RadiusSearch(queryPoint, searchRadius)

	if err != nil {
		log.Fatalf("radius search failed: %v", err)
	}

	fmt.Println("Radius Search Results:")

	for _, neighbor := range radiusResults {
		fmt.Printf("\tIndex: %d, Point: %v, Distance: %.3f\n",
			neighbor.Index,
			points[neighbor.Index],
			neighbor.Distance,
		)
	}

	// -------------------------------------------------------------------------
	// k-Nearest Neighbors Demonstration
	// -------------------------------------------------------------------------
	// Specify the number of nearest neighbors to retrieve.
	k := 3

	// Perform a k-nearest neighbors query.
	knnResults, err := tree.KNearestNeighbors(queryPoint, k)

	if err != nil {
		log.Fatalf("k-nearest neighbors search failed: %v", err)
	}

	fmt.Println("\nk-Nearest Neighbors Results:")

	for _, neighbor := range knnResults {
		fmt.Printf("\tIndex: %d, Point: %v, Distance: %.3f\n",
			neighbor.Index,
			points[neighbor.Index],
			neighbor.Distance,
		)
	}
}
