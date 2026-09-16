"""
test_hungarian.py

Automated unit test suite for verifying the HungarianAssigner execution engine
across structural boundaries, dimensional variations, and exception constraints.

Overview:
    This module uses Python's built-in `unittest` framework to execute a matrix
    of 20 unique testing scenarios against the `HungarianAssigner`. It ensures
    that structural validation parameters run cleanly, data elements calculate
    accurately, and optimization deadlocks are broken safely.

Testing Scope & Architectural Coverage:
    - Standard Matrix Topologies: Validates fundamental matching problems, single-element
      matrices, and identity mapping sets.
    - Defensive & Boundary Validations: Assesses non-iterable inputs, empty sets, non-square
      shapes, and negative-weight edge matrices.
    - Floating-Point & Scale Variations: Targets floating-point arrays, extreme cost
      imbalances, and redundant multi-zero options.
    - Graph & Combinatorial Extremes: Tests complex path augmentations, fully dense matrices,
      and cyclical tracking safeguards.

Execution Instructions:
    Run the complete testing matrix natively from the shell or command line via:

        python -m unittest test_hungarian.py
"""
import unittest
import numpy

from hungarian import HungarianAssigner

class TestHungarianAssigner(unittest.TestCase):
    """
    Test suite containing 20 isolated test vectors evaluating assignment matching parameter rules,
    reduction steps, and defensive validations.
    """

    def test_01_single_element_matrix(self):
        """1. Verifies that a minimal 1x1 matrix resolves to the single logical assignment coordinate."""
        matrix = [[42.0]]

        assigner = HungarianAssigner(matrix)

        self.assertEqual(assigner.compute_assignment(), [(0, 0)])

    def test_02_basic_identity_matrix(self):
        """2. Validates a trivial identity matrix where diagonal entries are the sole unique options."""
        matrix = [[0.0, 5.0], [5.0, 0.0]]

        assigner = HungarianAssigner(matrix)

        self.assertEqual(assigner.compute_assignment(), [(0, 0), (1, 1)])

    def test_03_standard_assignment_matching(self):
        """3. Evaluates a known 3x3 combinatorial minimization problem with unique row-to-column pairs."""
        matrix = [[1.0, 2.0, 3.0], [3.0, 3.0, 2.0], [2.0, 1.0, 3.0]]

        assigner = HungarianAssigner(matrix)

        self.assertEqual(assigner.compute_assignment(), [(0, 0), (1, 2), (2, 1)])

    def test_04_floating_point_cost_resolution(self):
        """4. Verifies optimization operations execute accurately on delicate fractional floating-point weights."""
        matrix = [[0.1, 0.4, 0.9], [0.3, 0.2, 0.5], [0.8, 0.7, 0.1]]

        assigner = HungarianAssigner(matrix)

        self.assertEqual(assigner.compute_assignment(), [(0, 0), (1, 1), (2, 2)])

    def test_05_matrix_requiring_path_augmentation(self):
        """5. Forces execution of Step 5 (Zomega augmenting path algorithm) to resolve zero assignment locks."""
        matrix = [[2, 2, 1], [1, 3, 2], [2, 1, 3]]

        assigner = HungarianAssigner(matrix)

        raw_assignments = assigner.compute_assignment()
        cleaned_assignments = [(int(worker), int(task)) for worker, task in raw_assignments]

        self.assertEqual(cleaned_assignments, [(0, 2), (1, 0), (2, 1)])

    def test_06_independent_row_and_column_reduction(self):
        """6. Verifies reduction properties isolate zeroes across offset dimensional structures."""
        matrix = [[10.0, 20.0], [30.0, 40.0]]

        assigner = HungarianAssigner(matrix)

        self.assertEqual(assigner.compute_assignment(), [(0, 0), (1, 1)])

    def test_07_all_elements_identical(self):
        """7. Tests a uniform grid where every task is equal, validating arbitrary first-available matching stability."""
        matrix = [[5.0, 5.0], [5.0, 5.0]]

        assigner = HungarianAssigner(matrix)

        self.assertEqual(assigner.compute_assignment(), [(0, 0), (1, 1)])

    def test_08_large_scale_imbalance(self):
        """8. Validates processing limits when one extreme outlier row dwarfs remaining options."""
        matrix = [[1.0, 2.0, 3.0], [1000.0, 2000.0, 3000.0], [3.0, 1.0, 2.0]]

        assigner = HungarianAssigner(matrix)

        self.assertEqual(assigner.compute_assignment(), [(0, 2), (1, 0), (2, 1)])

    def test_09_pre_reduced_cost_matrix(self):
        """9. Processes a matrix already fully reduced to independent zeros to evaluate stable pass-through operations."""
        matrix = [[0.0, 1.0, 2.0], [1.0, 0.0, 2.0], [2.0, 1.0, 0.0]]

        assigner = HungarianAssigner(matrix)

        self.assertEqual(assigner.compute_assignment(), [(0, 0), (1, 1), (2, 2)])

    def test_10_linear_diagonal_dependencies(self):
        """10. Parses an inverted diagonal matrix layout to assess edge tracking profiles."""
        matrix = [[5.0, 5.0, 1.0], [5.0, 1.0, 5.0], [1.0, 5.0, 5.0]]

        assigner = HungarianAssigner(matrix)

        self.assertEqual(assigner.compute_assignment(), [(0, 2), (1, 1), (2, 0)])

    def test_11_rejection_of_non_iterable_input(self):
        """11. Defensive Check: Asserts TypeError when a flat primitive value is passed instead of an array."""
        invalid_input = 12345

        with self.assertRaises(TypeError):
            HungarianAssigner(invalid_input)

    def test_12_rejection_of_flat_list(self):
        """12. Defensive Check: Validates dimension logic rejects unnested single-dimensional collections."""
        invalid_input = [1.0, 2.0, 3.0]

        with self.assertRaises(ValueError):
            HungarianAssigner(invalid_input)

    def test_13_rejection_of_empty_matrix(self):
        """13. Defensive Check: Ensures instantiation drops out when missing inner data elements."""
        invalid_input = [[]]

        with self.assertRaises(ValueError):
            HungarianAssigner(invalid_input)

    def test_14_rejection_of_non_square_topology(self):
        """14. Defensive Check: Asserts mismatch failures when row counts deviate from column capacities."""
        invalid_input = [[1.0, 2.0, 3.0], [4.0, 5.0, 6.0]]

        with self.assertRaises(ValueError):
            HungarianAssigner(invalid_input)

    def test_15_rejection_of_negative_values(self):
        """15. Defensive Check: Rejects initialization states containing invalid negative costs."""
        invalid_input = [[1.0, -2.0], [3.0, 4.0]]

        with self.assertRaises(ValueError):
            HungarianAssigner(invalid_input)

    def test_16_rejection_of_non_numeric_elements(self):
        """16. Defensive Check: Flags instances when a nested item string literal bypasses layout casting."""
        invalid_input = [[1.0, "corrupted_text"], [3.0, 4.0]]

        with self.assertRaises(TypeError):
            HungarianAssigner(invalid_input)

    def test_17_numpy_ndarray_input_handling(self):
        """17. Assures native compatibility workflows parse pre-allocated NumPy multidimensional array blocks."""
        matrix = numpy.array([[4.0, 1.0], [2.0, 3.0]], dtype=float)

        assigner = HungarianAssigner(matrix)

        self.assertEqual(assigner.compute_assignment(), [(0, 1), (1, 0)])

    def test_18_completely_dense_cost_zeros(self):
        """18. Checks zero cost layouts to verify matching converges without getting lost in multiple empty options."""
        matrix = [[0.0, 0.0, 0.0], [0.0, 0.0, 0.0], [0.0, 0.0, 0.0]]

        assigner = HungarianAssigner(matrix)

        self.assertEqual(assigner.compute_assignment(), [(0, 0), (1, 1), (2, 2)])

    def test_19_extreme_matrix_scale_dimensions(self):
        """19. Tests system performance limits across an extended 5x5 layout matrix block."""
        matrix = [
            [10, 19, 8, 15, 19],
            [10, 18, 14, 15, 13],
            [13, 16, 16, 14, 10],
            [12, 11, 13, 19, 11],
            [14, 12, 11, 15, 13],
        ]

        assigner = HungarianAssigner(matrix)

        self.assertEqual(
            assigner.compute_assignment(), [(0, 2), (1, 0), (2, 4), (3, 1), (4, 3)]
        )

    def test_20_loop_guard_cutoff_safeguard(self):
        """20. Validates loop guard error threshold execution by enforcing a mock failure condition."""
        matrix = [[1.0, 2.0], [3.0, 4.0]]
        assigner = HungarianAssigner(matrix)

        assigner.number_of_rows = 0
        assigner.number_of_columns = 0

        with self.assertRaises(RuntimeError):
            assigner.compute_assignment()

if __name__ == "__main__":
    unittest.main()