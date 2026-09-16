"""
hungarian.py

An implementation of the Hungarian algorithm (Kuhn-Munkres algorithm), which
provides an optimal solution for the linear sum assignment problem in polynomial time.

This module resolves task allocation and matching problems by systematically
reducing a cost matrix to uncover a complete set of independent zeros. It tracks
tentative assignments via "starred" zeros and alternative path sequences via
"primed" zeros using augmenting path logic.

Design Note:
    The algorithm assumes a square cost matrix where rows represent entities (e.g., workers)
    and columns represent tasks. It is optimized to minimize the cumulative total cost
    of the final pairing.
"""
import numpy

class HungarianAssigner:
    """
    Encapsulates the construction and execution of the Hungarian algorithm to determine a minimum-cost matching
    for a given bipartite graph.
    """

    def __init__(self, matrix: list[list[float]]):
        """
        Initializes the HungarianAssigner with a cost matrix and performs defensive data checks.

        Args:
            matrix (list[list[float]]): A 2D nested list structure representing weights.

        Raises:
            TypeError: If the input matrix structure is invalid or elements are non-numeric.
            ValueError: If the matrix is empty, non-square, or contains negative costs.
        """
        # Verify matrix is valid iterable collection.
        if not isinstance(matrix, (list, tuple, numpy.ndarray)):
            raise TypeError("The cost matrix must be a valid 2D array like structure.")

        # Try parsing inputs into an internal numpy floating-point matrix array.
        try:
            convertex_matrix = numpy.array(matrix, dtype=float)
        except (ValueError, TypeError) as error:
            raise TypeError("All elements inside the cost matrix must be valid numeric values.") from error

        # Ensure the matrix contains data elements and is explicitly configured in two dimensions.
        if convertex_matrix.size == 0 or convertex_matrix.ndim != 2:
            raise ValueError("The cost matrix must be a non-empty, two-dimensional matrix.")

        # Keep a reference copy of the pristine configuration.
        self.original = convertex_matrix

        # Create a working copy of the matrix to safely mutate during reduction processing.
        self.matrix = self.original.copy()
        self.number_of_rows, self.number_of_columns = self.matrix.shape

        # Enforce the requirement that the assignment problem contains equal sets of workers and tasks.
        if self.number_of_rows != self.number_of_columns:
            raise ValueError("Hungarian algorithm requires square matrix.")

        # Verify that all provided cost values are positive metrics.
        if (self.matrix < 0).any():
            raise ValueError("The cost matrix cannot contain negative values.")

        # Initialize the state map: 0 = unflagged, 1 = starred assignment, 2 = primed candidate.
        self.marked_matrix = numpy.zeros_like(self.matrix, dtype=int)

        # Initialize boolean mask vectors to keep track of covered rows and columns.
        self.rows = numpy.zeros(self.number_of_rows, dtype=bool)
        self.columns = numpy.zeros(self.number_of_columns, dtype=bool)

    def _reduce_matrix(self):
        """
        Subtracts row minimums first, then subtracts column minimums from the updated matrix state.
        """
        # Subtract the minimum element of each row from its respective row.
        row_minimums = self.matrix.min(axis=1)[:, numpy.newaxis]
        self.matrix -= row_minimums

        # Subtract the minimum element of each column from its respective column.
        column_minimums = self.matrix.min(axis=0)
        self.matrix -= column_minimums

    def _find_uncovered_zero(self) -> tuple[int | None, int | None]:
        """
        Finds the first matrix zero not currently spanned by row or column lines.

        Returns:
            tuple[int | None, int | None]: Indices of the uncovered zero if found,
            otherwise (None, None).
        """
        # Linear sweep across rows and columns.
        for worker_index in range(self.number_of_rows):
            for task_index in range(self.number_of_columns):
                # An element is uncovered if both its row and column lines are false.
                if (
                    self.matrix[worker_index, task_index] == 0
                    and not self.rows[worker_index]
                    and not self.columns[task_index]
                ):
                    return worker_index, task_index

        return None, None

    def _apply_initial_stars(self):
        """
        Identifies preliminary independent zero positions and stars them inside the tracking matrix.
        """
        for worker_index in range(self.number_of_rows):
            for task_index in range(self.number_of_columns):
                # Look for cell candidates reduced exactly to zero.
                if self.matrix[worker_index, task_index] == 0:
                    # Star this position if no other zero in this row or column is already starred.
                    if not (self.marked_matrix[worker_index, :] == 1).any() and not (
                        self.marked_matrix[:, task_index] == 1
                    ).any():
                        self.marked_matrix[worker_index, task_index] = 1

    def _augment_path(self, worker_index: int, task_index: int):
        """
        Constructs alternating sequence paths of primes and stars to shift assignment paths.

        Args:
            worker_index (int): The row index of the starting primed zero.
            task_index (int): The column index of the starting primed zero.
        """
        path = [(worker_index, task_index)]

        while True:
            current_column = path[-1][1]

            # Find a starred zero in the column of the last added path step.
            starred_rows = numpy.where(self.marked_matrix[:, current_column] == 1)[0]

            # If there is no starred zero in this column, the sequence path is complete.
            if len(starred_rows) == 0:
                break

            starred_row_index = starred_rows[0]

            # Append the starred zero coordinates to our sequence trail.
            path.append((starred_row_index, current_column))

            current_row = path[-1][0]

            # Find a primed zero in the row of the starred zero just identified.
            primed_columns = numpy.where(self.marked_matrix[current_row, :] == 2)[0]
            primed_column_index = primed_columns[0]

            # Append the primed zero coordinates to our sequence trail.
            path.append((current_row, primed_column_index))

        # Update the state of zeros along the augmenting path.
        for row_index, column_index in path:
            if self.marked_matrix[row_index, column_index] == 1:
                self.marked_matrix[row_index, column_index] = 0  # Unstar
            elif self.marked_matrix[row_index, column_index] == 2:
                self.marked_matrix[row_index, column_index] = 1  # Star

        # Flush all transient prime tracking marks across the matrix layout.
        self.marked_matrix[self.marked_matrix == 2] = 0

    def _adjusted_matrix_weights(self):
        """Alters costs using the lowest exposed value to construct new option paths."""
        uncovered_values = []

        # Isolate all remaining elements not covered by drawing coverage lines.
        for worker_index in range(self.number_of_rows):
            for task_index in range(self.number_of_columns):
                if not self.rows[worker_index] and not self.columns[task_index]:
                    uncovered_values.append(self.matrix[worker_index, task_index])

        # If everything is covered, exit safely to prevent recalculation faults.
        if not uncovered_values:
            return

        # Find the smallest value among all remaining uncovered fields.
        minimum_uncovered_value = min(uncovered_values)

        # Mathematical adjustment: shift potential options to zero without changing baseline order.
        for worker_index in range(self.number_of_rows):
            if self.rows[worker_index]:
                self.matrix[worker_index, :] += minimum_uncovered_value

        for task_index in range(self.number_of_columns):
            # Subtrack the minimum value from any column NOT covered by a line.
            if not self.columns[task_index]:
                self.matrix[:, task_index] -= minimum_uncovered_value

    def compute_assignment(self) -> list[tuple[int, int]]:
        """
        Orchestrates structural processing steps until an optimal configuration state matches.

        Returns:
            list[tuple[int, int]]: A sequence of coordinate tuples matching row indices to column indices.
        """
        # Execute initial row and column value reduction adjustments.
        self._reduce_matrix()
        self._apply_initial_stars()

        # Set a safeguard variable to prevent infinite hanging executions.
        loop_guard = 0
        maximum_safe_iterations = self.number_of_rows * self.number_of_columns * 10

        while True:
            loop_guard += 1

            # Break process execution if an infinite loop sequence is caught.
            if loop_guard > maximum_safe_iterations:
                raise RuntimeError(
                    "Optimization matrix mapping timed out or reached an unstable cyclical pattern."
                )

            # Reset row and column cover masks before establishing current step definitions.
            self._clear_cover_lines()

            # Step 3: Draw coverage lines over any column containing a starred zero.
            for worker_index in range(self.number_of_rows):
                for task_index in range(self.number_of_columns):
                    if self.marked_matrix[worker_index, task_index] == 1:
                        self.columns[task_index] = True

            # If total drawn columns equal matrix size, a complete assignment matching is resolved.
            if self.columns.sum() == self.number_of_columns:
                break

            primed_worker, primed_task = None, None
            step_complete = False

            # Step 4: Scan for uncovered values to match up alternate assignments.
            while not step_complete:
                worker_index, task_index = self._find_uncovered_zero()

                if worker_index is None:
                    step_complete = True
                else:
                    self.marked_matrix[worker_index, task_index] = 2  # Prime it

                    # Search for an existing starred pairing within the same row.
                    starred_columns = numpy.where(self.marked_matrix[worker_index, :] == 1)[0]

                    # If a star exists in this row, cover the row and uncover the star's column.
                    if len(starred_columns) > 0:
                        starred_task_index = starred_columns[0]

                        self.rows[worker_index] = True
                        self.columns[starred_task_index] = False
                    else:
                        # No star found in this row; we've isolated a path.
                        primed_worker, primed_task = worker_index, task_index
                        step_complete = True

            # Step 5: If an independent primed zero is isolated, perform configuration shifts.
            if primed_worker is not None and primed_task is not None:
                self._augment_path(primed_worker, primed_task)
                continue

            # Step 6: Adjust global matrix cost weights to reveal hidden zeroes.
            self._adjusted_matrix_weights()

        assignments = []

        # Parse tracking structures out into concrete solution arrays.
        for worker_index in range(self.number_of_rows):
            starred_indices = numpy.where(self.marked_matrix[worker_index, :] == 1)[0]

            # Enforce validation verification before concluding output mapping.
            if len(starred_indices) == 0:
                raise RuntimeError(
                    f"Algorithmic error: Worker {worker_index} failed to obtain a valid target matching."
                )

            task_index = starred_indices[0]
            assignments.append((worker_index, task_index))

        return assignments

    def _clear_cover_lines(self):
        """Flushes structural cross-out tracking states."""
        self.rows.fill(False)
        self.columns.fill(False)