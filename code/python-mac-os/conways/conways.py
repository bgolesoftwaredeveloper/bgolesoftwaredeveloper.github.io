"""
conways.py

An implementation of Conway's Game of Life, a zero-player cellular automaton
governed by simple neighborhood survival and reproduction rules.

This module provides an efficient matrix-based simulation environment that tracks
discrete cell states (alive or dead) over generations. It handles boundary
conditions seamlessly by modeling the grid as a toroidal surface (wrapping edges
around) and optimizes console rendering using native terminal control hooks.

Design Note:
    To prevent state mutation conflicts during a generation cycle, this module
    implements a double-buffering isolation strategy by reading from a frozen
    source matrix and writing modifications to a deep-copied secondary structure.
"""
import copy

def initialize_grid(rows: int, columns: int) -> list[list[int]]:
    """
    Allocates an empty cellular grid initialized to dead states and populates
    it with a localized 'Glider' kinematic seed pattern.

    Args:
        rows int: The vertical dimension of the computational grid boundary.
        columns int: The horizontal dimension of the computational grid boundary.

    Returns:
        list[list[int]]: A 2D matrix populated with dead spaces (0) and the initial seed (1).
    """
    grid = [[0 for _ in range(columns)] for _ in range(rows)]

    # Place the cursor near the top-left coordinate cluster.s
    grid[1][2] = 1
    grid[2][3] = 1
    grid[3][1] = 1
    grid[3][2] = 1
    grid[3][3] = 1

    return grid

def alive_neighbor_count(grid: list[list[int]],
                         row: int, column: int, number_of_rows: int, number_of_columns: int) -> int:
    """
    Evaluates the immediate 8-neighbor Moore neighborhood surrounding a specific
    coordinate, mapping indices modularly to achieve edge wrapping.

    Args:
        grid list[list[int]]: The current generation's structural baseline matrix.
        row int: The target vertical axis index to scan around.
        column int: The target horizontal axis index to scan around.
        number_of_rows int: Total vertical boundary limitation used for modular wrapping.
        number_of_columns int: Total horizontal boundary limitation used for modular wrapping.

    Returns:
        int: Total quantity of active living cells directly bordering the coordinate target.
    """
    count = 0

    # Traverse the 3x3 window bounded by offset differentials.
    for row_offset in [-1, 0, 1]:
        for column_offset in [-1, 0, 1]:
            # Exclude self-evaluation of the focal point center cell.
            if row_offset == 0 and column_offset == 0:
                continue

            # Apply toroidal modular arithmetic to enable cross-border neighbor discovery.
            neighbor_row = (row + row_offset) % number_of_rows
            neighbor_column = (column + column_offset) % number_of_columns

            if grid[neighbor_row][neighbor_column] == 1:
                count += 1

    return count

def compute_next_generation(grid: list[list[int]], number_of_rows: int, number_of_columns: int) -> list[list[int]]:
    """
    Processes a complete grid transition by filtering cell states through Conway's
    evolutionary logic loops based on neighboring densities.

    Args:
        grid list[list[int]]: The baseline grid configuration representing the current epoch.
        number_of_rows int: The precise vertical capacity of the game board.
        number_of_columns int: The precise horizontal capacity of the game board.

    Returns:
        list[list[int]]: An independent, fully updated 2D matrix representing the next generation.
    """
    # Clone the source structure completely to safeguard state processing from intermediate mutations.
    next_grid = copy.deepcopy(grid)

    for row_index in range(number_of_rows):
        for column_index in range(number_of_columns):
            current_state = grid[row_index][column_index]

            alive_neighbors = alive_neighbor_count(grid, row_index, column_index, number_of_rows, number_of_columns)

            # Apply transitions mechanics.
            if current_state == 1:
                # Underpopulated (neighbors < 2) ir Overpopulation (neighbors > 3) triggers mortality.
                if alive_neighbors < 2 or alive_neighbors > 3:
                    next_grid[row_index][column_index] = 0
            else:
                # Reproduction condition triggers birth on exactly 3 neighbors.
                if alive_neighbors == 3:
                    next_grid[row_index][column_index] = 1

    return next_grid

def display(grid: list[list[int]]) -> None:
    """
    Renders the game matrix directly into the standard console output stream,
    utilizing raw ANSI sequence operations to suppress artifact flickering.

    Args:
        grid list[list[int]]: The configured cell matrix scheduled to be visualised.

    Returns:
        None
    """
    print("\033[H\033[2J", end="")

    for row in grid:
        result = ""

        for cell in row:
            result += "█ " if cell == 1 else ". "

        print(result)
