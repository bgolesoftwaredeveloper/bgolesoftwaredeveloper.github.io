"""
test_conways.py

Unit testing suite for Conway's Game of Life algorithm implementation.

This module provides comprehensive test coverage verification leveraging Python's
built-in unittest framework. It validates parameter initialization edge bounds,
individual cell status state transitions, toroidal wrapping logic, population
neighborhood count limits, and structural mutation rules.
"""
import unittest

from conways import initialize_grid, alive_neighbor_count, compute_next_generation

class TestConwaysGameOfLife(unittest.TestCase):
    """
    Comprehensive unit testing suite enforcing strict verification metrics, cellular
    state transitions, and mathematical border wrapping logic across cellular grids.
    """

    def test_01_grid_dimensions(self):
        """Verify initialization constructs correct dimension allocations."""
        rows, columns = 15, 25

        grid = initialize_grid(rows, columns)

        self.assertEqual(len(grid), rows)
        self.assertEqual(len(grid[0]), columns)

    def test_02_glider_initialization(self):
        """Confirm the glider seed pattern is injected accurately into the default grid."""
        grid = initialize_grid(10, 10)

        self.assertEqual(grid[1][2], 1)
        self.assertEqual(grid[2][3], 1)
        self.assertEqual(grid[3][1], 1)
        self.assertEqual(grid[3][2], 1)
        self.assertEqual(grid[3][3], 1)

    def test_03_empty_neighborhood_count(self):
        """Verify neighbor counting reads zero across isolated dead spaces."""
        grid = [[0 for _ in range(5)] for _ in range(5)]

        count = alive_neighbor_count(grid, 2, 2, 5, 5)

        self.assertEqual(count, 0)

    def test_04_full_neighborhood_count(self):
        """Verify neighbor counting maxes out at 8 when completely surrounded."""
        grid = [[1 for _ in range(3)] for _ in range(3)]

        count = alive_neighbor_count(grid, 1, 1, 3, 3)

        self.assertEqual(count, 8)

    def test_05_toroidal_wrapping_horizontal(self):
        """Test horizontal wrapping across left and right border horizons."""
        grid = [[0 for _ in range(5)] for _ in range(5)]

        grid[2][0] = 1

        count = alive_neighbor_count(grid, 2, 4, 5, 5)

        self.assertEqual(count, 1)

    def test_06_toroidal_wrapping_vertical(self):
        """Test vertical wrapping across top and bottom border horizons."""
        grid = [[0 for _ in range(5)] for _ in range(5)]

        grid[0][2] = 1

        count = alive_neighbor_count(grid, 4, 2, 5, 5)

        self.assertEqual(count, 1)

    def test_07_toroidal_wrapping_corners(self):
        """Verify corner diagnostics accurately wrap around to alternate diagonal corners."""
        grid = [[0 for _ in range(4)] for _ in range(4)]

        grid[0][0] = 1

        count = alive_neighbor_count(grid, 3, 3, 4, 4)

        self.assertEqual(count, 1)

    def test_08_rule_1_underpopulation(self):
        """Verify a live cell with fewer than 2 neighbors dies in the next turn."""
        grid = [[0 for _ in range(3)] for _ in range(3)]

        grid[1][1] = 1

        next_grid = compute_next_generation(grid, 3, 3)

        self.assertEqual(next_grid[1][1], 0)

    def test_09_rule_2_survival_two_neighbors(self):
        """Verify a live cell with exactly 2 neighbors stays alive."""
        grid = [
            [1, 0, 0],
            [0, 1, 0],
            [0, 0, 1]
        ]

        next_grid = compute_next_generation(grid, 3, 3)

        self.assertEqual(next_grid[1][1], 1)

    def test_10_rule_2_survival_three_neighbors(self):
        """Verify a live cell with exactly 3 neighbors stays alive."""
        grid = [
            [1, 0, 1],
            [0, 1, 0],
            [0, 0, 1]
        ]

        next_grid = compute_next_generation(grid, 3, 3)

        self.assertEqual(next_grid[1][1], 1)

    def test_11_rule_3_overpopulation(self):
        """Verify a live cell with more than 3 neighbors dies due to crowding."""
        grid = [
            [1, 0, 1],
            [0, 1, 0],
            [1, 0, 1]
        ]

        next_grid = compute_next_generation(grid, 3, 3)

        self.assertEqual(next_grid[1][1], 0)

    def test_12_rule_4_reproduction(self):
        """Verify a dead cell with exactly 3 neighbors comes to life."""
        grid = [
            [1, 0, 1],
            [0, 0, 0],
            [0, 0, 1]
        ]

        next_grid = compute_next_generation(grid, 3, 3)

        self.assertEqual(next_grid[1][1], 1)

    def test_13_dead_cell_stasis(self):
        """Verify a dead cell with only 2 neighbors remains dead."""
        grid = [
            [1, 0, 1],
            [0, 0, 0],
            [0, 0, 0]
        ]

        next_grid = compute_next_generation(grid, 3, 3)

        self.assertEqual(next_grid[1][1], 0)

    def test_14_still_life_block(self):
        """Verify a static 2x2 'Block' structure remains perfectly stable."""
        grid = [
            [0, 0, 0, 0],
            [0, 1, 1, 0],
            [0, 1, 1, 0],
            [0, 0, 0, 0]
        ]

        next_grid = compute_next_generation(grid, 4, 4)

        self.assertEqual(next_grid, grid)

    def test_15_oscillator_blinker(self):
        """Verify a 3-cell linear 'Blinker' alternates orientation each turn."""
        horizontal_blinker = [
            [0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0],
            [0, 1, 1, 1, 0],
            [0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0]
        ]

        expected_vertical_blinker = [
            [0, 0, 0, 0, 0],
            [0, 0, 1, 0, 0],
            [0, 0, 1, 0, 0],
            [0, 0, 1, 0, 0],
            [0, 0, 0, 0, 0]
        ]

        next_grid = compute_next_generation(horizontal_blinker, 5, 5)

        self.assertEqual(next_grid, expected_vertical_blinker)

    def test_16_all_dead_stasis(self):
        """Verify an empty grid remains empty without spontaneously spawning life."""
        grid = [[0 for _ in range(4)] for _ in range(4)]

        next_grid = compute_next_generation(grid, 4, 4)

        self.assertEqual(next_grid, grid)

    def test_17_all_alive_collapse(self):
        """Verify a completely filled grid collapses into massive die-offs due to crowding."""
        grid = [[1 for _ in range(4)] for _ in range(4)]

        next_grid = compute_next_generation(grid, 4, 4)

        self.assertEqual(next_grid[1][1], 0)
        self.assertEqual(next_grid[2][2], 0)

    def test_18_minimal_grid_size(self):
        """Verify calculations hold up cleanly on a minimal 1x1 map structure."""
        grid = [[1]]

        count = alive_neighbor_count(grid, 0, 0, 1, 1)
        next_grid = compute_next_generation(grid, 1, 1)

        self.assertEqual(count, 8)
        self.assertEqual(next_grid[0][0], 0)  # Overpopulation death

    def test_19_mutation_isolation(self):
        """Verify the computation process uses isolated buffering and avoids in-place edits."""
        grid = [
            [0, 1, 0],
            [0, 1, 0],
            [0, 1, 0]
        ]

        next_grid = compute_next_generation(grid, 3, 3)

        self.assertEqual(grid[0][1], 1)
        self.assertNotEqual(grid, next_grid)

    def test_20_large_scale_empty_matrix(self):
        """Verify structural parsing handles large matrices without processing drops."""
        large_rows, large_columns = 100, 100

        grid = [[0 for _ in range(large_columns)] for _ in range(large_rows)]

        next_grid = compute_next_generation(grid, large_rows, large_columns)

        self.assertEqual(len(next_grid), 100)
        self.assertEqual(next_grid[50][50], 0)

if __name__ == "__main__":
    unittest.main()