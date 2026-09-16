// -----------------------------------------------------------------------------
// Package: main
// File:    main.go
//
// Description:
//
//	This is a demonstration entry point for the go-octant spatial engine.
//	It showcases the practical application of the Octree data structure by
//	initializing a 3D world, populating it with spatial data, and performing
//	complex queries.
//
//	Demonstrated Functionality:
//	- Initialization of a root OctreeNode with a defined BoundingBox.
//	- Batch insertion of points to trigger recursive subdivision.
//	- Range-based spatial queries to locate points within a specific volume.
//	- Nearest-Neighbor search to identify the single closest point to a target.
//	- Proper tree cleanup and memory management.
//
// Notes:
//   - The world boundary is set to 200 units total (-100 to 100).
//   - A small node capacity of 2 is used to force visible tree depth.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"
	"log"

	"github.com/bgolesoftwaredeveloper/go-octant/octant"
)

func main() {
	// 1. Define the world boundary (Center at 0,0,0 with a half-extent of 100).
	// This creates a box from -100 to +100 on all axes.
	worldBoundary := octant.BoundingBox{
		CenterX:    0,
		CenterY:    0,
		CenterZ:    0,
		HalfExtent: 100,
	}

	// 2. Initialize the Octree with a capacity of 2 points per node.
	// Low capacity triggers subdivision quickly so we can test the tree logic.
	tree := octant.NewOctreeNode(worldBoundary, 2)

	// 3. Insert several points.
	pointsToInsert := []octant.Point{
		{X: 10, Y: 10, Z: 10},
		{X: 12, Y: 10, Z: 10},
		{X: 50, Y: 50, Z: 50},
		{X: -20, Y: -20, Z: -20},
		{X: 80, Y: 80, Z: 80},
	}

	for _, point := range pointsToInsert {
		success, err := tree.Insert(point)

		if err != nil {
			log.Fatalf("Critical error: %v", err)
		}

		if success {
			fmt.Printf("Inserted Point: %+v\n", point)
		} else {
			fmt.Printf("Point out of bounds: %+v\n", point)
		}
	}

	fmt.Println("--------------------------------------------------")

	// 4. Test Range Query.
	// We'll search in a small box around the first two points.
	searchArea := octant.BoundingBox{
		CenterX:    11,
		CenterY:    10,
		CenterZ:    10,
		HalfExtent: 2,
	}

	foundPoints := tree.QueryRange(searchArea)

	fmt.Printf("Range Query found %d points in area %+v\n", len(foundPoints), searchArea)

	for _, point := range foundPoints {
		fmt.Printf(" - Found: %+v\n", point)
	}

	fmt.Println("--------------------------------------------------")

	// 5. Test Nearest Neighbor.
	// We want to find which point is closest to {X: 45, Y: 45, Z: 45}.
	target := octant.Point{X: 45, Y: 45, Z: 45}

	nearest, distance, err := tree.FindNearestNeighbor(target)

	if err != nil {
		fmt.Printf("Error searching: %v\n", err)
	} else {
		fmt.Printf("Target Coordinate: %+v\n", target)
		fmt.Printf("Nearest Neighbor:  %+v\n", nearest)
		fmt.Printf("Distance:          %.4f\n", distance)
	}

	// 6. Cleanup.
	tree.Clear()

	fmt.Println("--------------------------------------------------")
	fmt.Println("Tree cleared.")
}
