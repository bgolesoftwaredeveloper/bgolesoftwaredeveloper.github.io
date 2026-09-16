// -----------------------------------------------------------------------------
// Package: astar
// File: astar_test.go
//
// Description:
//
//	This file contains unit tests for the A* (A-star) pathfinding algorithm
//	implemented in the astar package. The tests verify correct behavior
//	across common scenarios, including valid paths, blocked paths, and
//	invalid input conditions.
//
//	The tests are designed to validate both algorithm correctness and
//	defensive behavior, ensuring that the implementation behaves
//	predictably under normal and edge-case conditions.
//
// Notes:
//   - All tests use Arrange / Act / Assert structure.
//   - The testing.T parameter is named "test" for clarity.
//   - Grids are intentionally small to keep test intent explicit.
//   - Tests focus on observable behavior, not internal state.
//
// -----------------------------------------------------------------------------
package astar

import "testing"

// TestAStar_FindsValidPath verifies that AStar returns a valid shortest
// path when a traversable route exists between the start and goal.
func TestAStar_FindsValidPath(test *testing.T) {
	// Arrange.
	grid := [][]int{
		{0, 0, 0, 0, 0},
		{1, 1, 0, 1, 0},
		{0, 0, 0, 1, 0},
		{0, 1, 1, 0, 0},
		{0, 0, 0, 0, 0},
	}

	start := NewGridPosition(0, 0)
	goal := NewGridPosition(4, 4)

	// Act.
	path := AStar(grid, start, goal)

	// Assert.
	if path == nil {
		test.Fatalf("expected a valid path, but received nil")
	}

	if len(path) == 0 {
		test.Fatalf("expected non-empty path, but received empty path")
	}

	first := path[0]
	last := path[len(path)-1]

	if first != start {
		test.Errorf("expected path to start at %v, got %v", start, first)
	}

	if last != goal {
		test.Errorf("expected path to end at %v, got %v", goal, last)
	}
}

// TestAStar_ReturnsNilWhenNoPathExists verifies that AStar returns nil
// when the goal cannot be reached due to obstacles.
func TestAStar_ReturnsNilWhenNoPathExists(test *testing.T) {
	// Arrange.
	grid := [][]int{
		{0, 1, 0},
		{1, 1, 1},
		{0, 1, 0},
	}

	start := NewGridPosition(0, 0)
	goal := NewGridPosition(2, 2)

	// Act.
	path := AStar(grid, start, goal)

	// Assert.
	if path != nil {
		test.Fatalf("expected nil path when no route exists, got %v", path)
	}
}

// TestAStar_ReturnsNilForBlockedStart verifies that AStar returns nil
// when the start position itself is blocked.
func TestAStar_ReturnsNilForBlockedStart(test *testing.T) {
	// Arrange.
	grid := [][]int{
		{1, 0},
		{0, 0},
	}

	start := NewGridPosition(0, 0)
	goal := NewGridPosition(1, 1)

	// Act.
	path := AStar(grid, start, goal)

	// Assert.
	if path != nil {
		test.Fatalf("expected nil path for blocked start position, got %v", path)
	}
}

// TestAStar_ReturnsNilForBlockedGoal verifies that AStar returns nil
// when the goal position itself is blocked.
func TestAStar_ReturnsNilForBlockedGoal(test *testing.T) {
	// Arrange.
	grid := [][]int{
		{0, 0},
		{0, 1},
	}

	start := NewGridPosition(0, 0)
	goal := NewGridPosition(1, 1)

	// Act.
	path := AStar(grid, start, goal)

	// Assert.
	if path != nil {
		test.Fatalf("expected nil path for blocked goal position, got %v", path)
	}
}

// TestAStar_ReturnsNilForOutOfBoundsStart verifies that AStar returns nil
// when the start position is outside the grid bounds.
func TestAStar_ReturnsNilForOutOfBoundsStart(test *testing.T) {
	// Arrange.
	grid := [][]int{
		{0, 0},
		{0, 0},
	}

	start := NewGridPosition(-1, 0)
	goal := NewGridPosition(1, 1)

	// Act.
	path := AStar(grid, start, goal)

	// Assert.
	if path != nil {
		test.Fatalf("expected nil path for out-of-bounds start, got %v", path)
	}
}

// TestAStar_ReturnsNilForEmptyGrid verifies that AStar returns nil
// when provided with an empty grid.
func TestAStar_ReturnsNilForEmptyGrid(test *testing.T) {
	// Arrange.
	var grid [][]int

	start := NewGridPosition(0, 0)
	goal := NewGridPosition(0, 0)

	// Act.
	path := AStar(grid, start, goal)

	// Assert.
	if path != nil {
		test.Fatalf("expected nil path for empty grid, got %v", path)
	}
}

// TestAStar_StartEqualsGoal verifies that AStar returns a path containing
// exactly one position when the start and goal positions are identical.
func TestAStar_StartEqualsGoal(test *testing.T) {
	// Arrange.
	grid := [][]int{
		{0, 0},
		{0, 0},
	}

	start := NewGridPosition(1, 1)
	goal := NewGridPosition(1, 1)

	// Act.
	path := AStar(grid, start, goal)

	// Assert.
	if path == nil {
		test.Fatalf("expected non-nil path when start equals goal")
	}

	if len(path) != 1 {
		test.Fatalf("expected path length of 1, got %d", len(path))
	}

	if path[0] != start {
		test.Errorf("expected path to contain start position %v, got %v", start, path[0])
	}
}

// TestAStar_FindsStraightHorizontalPath verifies that AStar correctly
// finds a straight horizontal path with no obstacles.
func TestAStar_FindsStraightHorizontalPath(test *testing.T) {
	// Arrange.
	grid := [][]int{
		{0, 0, 0, 0},
	}

	start := NewGridPosition(0, 0)
	goal := NewGridPosition(0, 3)

	// Act.
	path := AStar(grid, start, goal)

	// Assert.
	if path == nil {
		test.Fatalf("expected valid path, got nil")
	}

	expectedLength := 4

	if len(path) != expectedLength {
		test.Fatalf("expected path length %d, got %d", expectedLength, len(path))
	}

	if path[0] != start || path[len(path)-1] != goal {
		test.Errorf("path endpoints incorrect: got %v -> %v", path[0], path[len(path)-1])
	}
}

// TestAStar_FindsStraightVerticalPath verifies that AStar correctly
// finds a straight vertical path with no obstacles.
func TestAStar_FindsStraightVerticalPath(test *testing.T) {
	// Arrange.
	grid := [][]int{
		{0},
		{0},
		{0},
		{0},
	}

	start := NewGridPosition(0, 0)
	goal := NewGridPosition(3, 0)

	// Act.
	path := AStar(grid, start, goal)

	// Assert.
	if path == nil {
		test.Fatalf("expected valid path, got nil")
	}

	expectedLength := 4

	if len(path) != expectedLength {
		test.Fatalf("expected path length %d, got %d", expectedLength, len(path))
	}
}

// TestAStar_DoesNotTraverseObstacles verifies that every position
// in the returned path corresponds to a walkable grid cell.
func TestAStar_DoesNotTraverseObstacles(test *testing.T) {
	// Arrange.
	grid := [][]int{
		{0, 0, 0},
		{1, 1, 0},
		{0, 0, 0},
	}

	start := NewGridPosition(0, 0)
	goal := NewGridPosition(2, 2)

	// Act.
	path := AStar(grid, start, goal)

	// Assert.
	if path == nil {
		test.Fatalf("expected valid path, got nil")
	}

	for _, position := range path {
		if grid[position.Row()][position.Column()] == 1 {
			test.Fatalf("path traverses obstacle at %v", position)
		}
	}
}

// TestAStar_PathStepsAreAdjacent verifies that consecutive positions
// in the returned path differ by exactly one orthogonal move.
func TestAStar_PathStepsAreAdjacent(test *testing.T) {
	// Arrange.
	grid := [][]int{
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0},
	}

	start := NewGridPosition(0, 0)
	goal := NewGridPosition(2, 2)

	// Act.
	path := AStar(grid, start, goal)

	// Assert.
	if path == nil {
		test.Fatalf("expected valid path, got nil")
	}

	for index := 1; index < len(path); index++ {
		previous := path[index-1]
		current := path[index]

		rowDelta := abs(previous.Row() - current.Row())
		columnDelta := abs(previous.Column() - current.Column())

		if rowDelta+columnDelta != 1 {
			test.Fatalf("non-adjacent step detected between %v and %v",
				previous,
				current,
			)
		}
	}
}

// TestAStar_ReturnsNilForOutOfBoundsGoal verifies that AStar returns nil
// when the goal position lies outside the grid bounds.
func TestAStar_ReturnsNilForOutOfBoundsGoal(test *testing.T) {
	// Arrange.
	grid := [][]int{
		{0, 0},
		{0, 0},
	}

	start := NewGridPosition(0, 0)
	goal := NewGridPosition(2, 0)

	// Act.
	path := AStar(grid, start, goal)

	// Assert.
	if path != nil {
		test.Fatalf("expected nil path for out-of-bounds goal, got %v", path)
	}
}
