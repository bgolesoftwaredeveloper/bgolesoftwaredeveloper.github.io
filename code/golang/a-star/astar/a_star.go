// -----------------------------------------------------------------------------
// Package: astar
// File: astar.go
//
// Description:
//
//	This file provides a grid-based implementation of the A* (A-star)
//	pathfinding algorithm. The algorithm computes the shortest traversable
//	path between a start position and a goal position on a two-dimensional
//	grid using the Manhattan distance heuristic.
//
//	The grid is assumed to be axis-aligned and uniformly weighted. Movement
//	is restricted to the four cardinal directions (up, down, left, right).
//
// Notes:
//   - A grid value of 0 represents a walkable cell.
//   - A grid value of 1 represents an obstacle.
//   - The heuristic used is Manhattan distance.
//   - Returned paths are expressed as ordered grid coordinates.
//   - If no path exists, the algorithm returns nil.
//
// -----------------------------------------------------------------------------
package astar

import "container/heap"

// gridPosition represents a single coordinate within the grid.
type gridPosition struct {
	row    int
	column int
}

// pathNode represents a node evaluated by the A* algorithm.
// Each node stores cumulative cost information and a parent pointer
// used for path reconstruction.
type pathNode struct {
	position            gridPosition
	cost                int
	estimatedCostToGoal int
	totalEstimatedCost  int
	parentNode          *pathNode
	priorityQueueIndex  int
}

// PriorityQueue implements a min-heap of PathNode pointers ordered
// by lowest totalEstimatedCost.
type PriorityQueue []*pathNode

// abs returns the absolute value of an integer.
// This helper is used when computing Manhattan distances.
func abs(value int) int {
	// Convert negative values to their positive equivalent.
	if value < 0 {
		return -value
	}

	return value
}

// calculateManhattanDistance computes the Manhattan distance between
// two grid positions. This value serves as the heuristic estimate of
// the remaining distance from a node to the goal.
func calculateManhattanDistance(start gridPosition, goal gridPosition) int {
	// Guard against nonsensical coordinates.
	if start.row < 0 || start.column < 0 || goal.row < 0 || goal.column < 0 {
		return 0
	}

	// Compute vertical distance independently.
	rowDistance := abs(start.row - goal.row)

	// Compute horizontal distance independently.
	columnDistance := abs(start.column - goal.column)

	// Manhattan distance is the sum of orthogonal distances.
	return rowDistance + columnDistance
}

// reconstructPath rebuilds the final path by following parent pointers
// from the goal node back to the start node. The resulting path is
// reversed before being returned so that it is ordered from start
// to goal. If the goal node is nil, the function returns nil.
func reconstructPath(goal *pathNode) []gridPosition {
	// Guard against invalid goal nodes.
	if goal == nil {
		return nil
	}

	reversedPath := make([]gridPosition, 0)
	current := goal

	// Traverse backward through the parent chain.
	for current != nil {
		reversedPath = append(reversedPath, current.position)

		current = current.parentNode
	}

	// Reverse the collected positions in-place.
	for leftIndex, rightIndex := 0, len(reversedPath)-1; leftIndex < rightIndex; leftIndex, rightIndex = leftIndex+1, rightIndex-1 {
		reversedPath[leftIndex], reversedPath[rightIndex] =
			reversedPath[rightIndex], reversedPath[leftIndex]
	}

	return reversedPath
}

// NewGridPosition constructs a GridPosition using validated coordinates.
// The returned position is suitable for passing into AStar.
func NewGridPosition(row int, column int) gridPosition {
	return gridPosition{
		row:    row,
		column: column,
	}
}

// Row returns the row component of the position.
func (position gridPosition) Row() int {
	return position.row
}

// Column returns the column component of the position.
func (position gridPosition) Column() int {
	return position.column
}

// Len returns the number of elements currently stored in the priority queue.
// This method is required to satisfy heap.Interface. If the queue is nil,
// zero is returned.
func (queue PriorityQueue) Len() int {
	// Allow nil queues to behave as empty queues.
	if queue == nil {
		return 0
	}

	return len(queue)
}

// Less reports whether the element at leftIndex has a lower priority
// than the element at rightIndex. Nodes with lower totalEstimatedCost
// are treated as higher priority. If indices are invalid, false is returned conservatively.
func (queue PriorityQueue) Less(leftIndex, rightIndex int) bool {
	// Guard against invalid queue state or indices.
	if queue == nil ||
		leftIndex < 0 || rightIndex < 0 ||
		leftIndex >= len(queue) || rightIndex >= len(queue) {
		return false
	}

	return queue[leftIndex].totalEstimatedCost < queue[rightIndex].totalEstimatedCost
}

// Swap exchanges two elements in the priority queue and updates their
// internal heap indices. This method preserves heap invariants.
// If indices are invalid, the operation is ignored.
func (queue PriorityQueue) Swap(leftIndex, rightIndex int) {
	// Guard against invalid queue state or indices.
	if queue == nil ||
		leftIndex < 0 || rightIndex < 0 ||
		leftIndex >= len(queue) || rightIndex >= len(queue) {
		return
	}

	// Perform the swap.
	queue[leftIndex], queue[rightIndex] =
		queue[rightIndex], queue[leftIndex]

	// Update stored heap indices.
	queue[leftIndex].priorityQueueIndex = leftIndex
	queue[rightIndex].priorityQueueIndex = rightIndex
}

// Push inserts a new PathNode into the priority queue.
// This method is called internally by the heap package.
// Nil or incorrectly typed values are ignored defensively.
func (queue *PriorityQueue) Push(value interface{}) {
	// Guard against nil receivers or invalid input.
	if queue == nil || value == nil {
		return
	}

	node, ok := value.(*pathNode)

	if !ok {
		return
	}

	// Assign the node its heap position and append it.
	node.priorityQueueIndex = len(*queue)

	*queue = append(*queue, node)
}

// Pop removes and returns the PathNode with the lowest priority value.
// This method is called internally by the heap package.
// If the queue is empty or nil, nil is returned.
func (queue *PriorityQueue) Pop() interface{} {
	// Guard against empty or nil queues.
	if queue == nil || len(*queue) == 0 {
		return nil
	}

	oldQueue := *queue
	queueLength := len(oldQueue)

	// Remove the last element from the slice.
	node := oldQueue[queueLength-1]
	oldQueue[queueLength-1] = nil

	// Mark the node as removed from the heap.
	node.priorityQueueIndex = -1

	// Shrink the underlying slice.
	*queue = oldQueue[:queueLength-1]

	return node
}

// AStar computes the shortest traversable path between start and goal
// positions on the provided grid using the A* search algorithm.
// The function returns an ordered slice of grid positions representing
// the path from start to goal. If no path exists or the input is invalid,
// the function returns nil.
func AStar(grid [][]int, start gridPosition, goal gridPosition) []gridPosition {
	// Reject empty grids or grids with empty rows.
	if len(grid) == 0 || len(grid[0]) == 0 {
		return nil
	}

	// Validate start and goal coordinates.
	if start.row < 0 || start.column < 0 || goal.row < 0 || goal.column < 0 ||
		start.row >= len(grid) || start.column >= len(grid[0]) ||
		goal.row >= len(grid) || goal.column >= len(grid[0]) {
		return nil
	}

	// Reject blocked start or goal positions.
	if grid[start.row][start.column] == 1 || grid[goal.row][goal.column] == 1 {
		return nil
	}

	openSet := &PriorityQueue{}

	// Initialize the priority queue as a valid heap.
	heap.Init(openSet)

	// Create the initial node corresponding to the start position.
	node := &pathNode{
		position:            start,
		cost:                0,
		estimatedCostToGoal: calculateManhattanDistance(start, goal),
	}

	// Compute the node's total estimated cost.
	node.totalEstimatedCost = node.cost + node.estimatedCostToGoal

	// Insert the start node into the open set.
	heap.Push(openSet, node)

	// Track which positions have been fully evaluated.
	visitedPositions := make(map[gridPosition]bool)

	// Track the lowest known cost from the start to each position.
	costFromStartTable := make(map[gridPosition]int)
	costFromStartTable[start] = 0

	// Define the four cardinal movement directions.
	neighborDirections := []gridPosition{
		{row: -1, column: 0},
		{row: 1, column: 0},
		{row: 0, column: -1},
		{row: 0, column: 1},
	}

	// Continue processing nodes until no candidates remain.
	for openSet.Len() > 0 {
		current, ok := heap.Pop(openSet).(*pathNode)

		if !ok || current == nil {
			continue
		}

		// Terminate early if the goal has been reached.
		if current.position == goal {
			return reconstructPath(current)
		}

		// Mark the current position as closed.
		visitedPositions[current.position] = true

		// Explore each neighboring position.
		for _, direction := range neighborDirections {
			neighborPosition := gridPosition{
				row:    current.position.row + direction.row,
				column: current.position.column + direction.column,
			}

			// Skip positions outside grid bounds.
			if neighborPosition.row < 0 || neighborPosition.column < 0 ||
				neighborPosition.row >= len(grid) || neighborPosition.column >= len(grid[0]) {
				continue
			}

			// Skip blocked cells.
			if grid[neighborPosition.row][neighborPosition.column] == 1 {
				continue
			}

			// Compute the tentative cost to reach the neighbor.
			newCost := costFromStartTable[current.position]
			previousCost, hasRecorededCost := costFromStartTable[neighborPosition]

			// Update the neighbor if this path is cheaper.
			if !hasRecorededCost || newCost < previousCost {
				costFromStartTable[neighborPosition] = newCost

				estimatedCostToGoal := calculateManhattanDistance(neighborPosition, goal)

				totalEstimatedCost := newCost + estimatedCostToGoal

				neighborNode := &pathNode{
					position:            neighborPosition,
					cost:                newCost,
					estimatedCostToGoal: estimatedCostToGoal,
					totalEstimatedCost:  totalEstimatedCost,
					parentNode:          current,
				}

				// Add the neighbor to the open set if it has not been closed.
				if !visitedPositions[neighborPosition] {
					heap.Push(openSet, neighborNode)
				}
			}
		}
	}

	// No path to the goal exists.
	return nil
}
