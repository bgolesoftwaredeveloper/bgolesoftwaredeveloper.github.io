// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file provides a simple executable demonstration of the A* (A-star)
//	pathfinding algorithm implemented in the astar package. The program
//	constructs a small two-dimensional grid with obstacles, defines a start
//	and goal position, and computes the shortest traversable path between
//	them.
//
//	The resulting path is printed to standard output as an ordered sequence
//	of grid coordinates, allowing the behavior of the algorithm to be
//	observed and verified.
//
// Notes:
//   - This file serves as a usage example, not a library component.
//   - The grid is hard-coded for demonstration purposes.
//   - Output coordinates correspond to grid row and column indices.
//   - If no path exists, a message is printed instead of a path.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/a-star/astar"
)

func main() {
	// Define a simple grid where:
	// 0 represents a walkable cell.
	// 1 represents an obstacle.
	grid := [][]int{
		{0, 0, 0, 0, 0},
		{1, 1, 0, 1, 0},
		{0, 0, 0, 1, 0},
		{0, 1, 1, 0, 0},
		{0, 0, 0, 0, 0},
	}

	// Construct start and goal positions without touching unexported fields.
	startPosition := astar.NewGridPosition(0, 0)
	goalPosition := astar.NewGridPosition(4, 4)

	// Execute the A* pathfinding algorithm.
	path := astar.AStar(grid, startPosition, goalPosition)

	// Handle the case where no path exists.
	if path == nil {
		fmt.Println("No path could be found.")
		return
	}

	// Print the resulting path in order.
	fmt.Println("Path found:")

	for stepIndex, position := range path {
		fmt.Printf("\tStep %d -> row: %d, column: %d\n",
			stepIndex,
			position.Row(),
			position.Column(),
		)
	}
}
