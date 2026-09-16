// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file provides a concrete usage example of the QuadTree
//	spatial data structure defined in the quadtree package.
//
//	The example demonstrates:
//
//	1. Construction of a QuadTree with a fixed spatial boundary.
//	2. Insertion of multiple two-dimensional points.
//	3. Execution of an axis-aligned range query.
//	4. Iteration over and display of query results.
//
//	The intent of this file is instructional clarity rather than
//	performance optimization or minimalism.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/quad-tree/quadtree"
)

func main() {
	// -------------------------------------------------------------------------
	// Define the global spatial boundary for the QuadTree.
	//
	// All points inserted into the tree must lie within this region.
	// Any point outside this bounding box will be rejected during insertion.
	// -------------------------------------------------------------------------
	worldBoundary := quadtree.BoundingBox{
		MinimumX: 0.0,
		MinimumY: 0.0,
		MaximumX: 100.0,
		MaximumY: 100.0,
	}

	// -------------------------------------------------------------------------
	// Construct the QuadTree with a fixed node capacity.
	//
	// The capacity determines how many points a node may store before
	// it is subdivided into four child quadrants.
	// -------------------------------------------------------------------------
	treeNodeCapacity := 4

	quadTree := quadtree.NewQuadTree(worldBoundary, treeNodeCapacity)

	// -------------------------------------------------------------------------
	// Define a set of sample points to be inserted into the QuadTree.
	//
	// These points are intentionally distributed across the space
	// to exercise subdivision and spatial partitioning behavior.
	// -------------------------------------------------------------------------
	pointsToInsert := []quadtree.Point{
		{X: 10.0, Y: 10.0},
		{X: 15.0, Y: 15.0},
		{X: 25.0, Y: 30.0},
		{X: 60.0, Y: 20.0},
		{X: 70.0, Y: 80.0},
		{X: 90.0, Y: 90.0},
	}

	// -------------------------------------------------------------------------
	// Insert each point into the QuadTree.
	//
	// Failed insertions are reported, which would indicate a point
	// lying outside the defined spatial boundary.
	// -------------------------------------------------------------------------
	for _, point := range pointsToInsert {
		inserted := quadTree.InsertPoint(point)

		if !inserted {
			fmt.Printf("Failed to insert point: %+v\n", point)
		}
	}

	// -------------------------------------------------------------------------
	// Define a query region for spatial searching.
	//
	// This bounding box represents the area of interest for which
	// all contained points should be retrieved.
	// -------------------------------------------------------------------------
	queryRegion := quadtree.BoundingBox{
		MinimumX: 0.0,
		MinimumY: 0.0,
		MaximumX: 50.0,
		MaximumY: 50.0,
	}

	// -------------------------------------------------------------------------
	// Execute the range query against the QuadTree.
	//
	// The results slice is passed by reference and populated recursively
	// by the QuadTree traversal.
	// -------------------------------------------------------------------------
	var results []quadtree.Point

	quadTree.QueryRange(queryRegion, &results)

	// -------------------------------------------------------------------------
	// Output the results of the spatial query.
	//
	// Each reported point lies within the specified query region.
	// -------------------------------------------------------------------------
	fmt.Println("Points found within query region:")

	for _, point := range results {
		fmt.Printf("\tX = %.2f, Y = %.2f\n", point.X, point.Y)
	}
}
