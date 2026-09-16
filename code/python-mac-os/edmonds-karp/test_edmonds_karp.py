"""
test_edmonds_karp.py

A validation suite containing 20 unique unit tests designed to evaluate the
structural integrity, mathematical purity, and execution edge cases of the
custom Edmonds-Karp Maximum Flow pipeline.

Security Note:
    This test suite runs completely within volatile memory space using clean math
    primitives. It does not read or write persistent states to the host environment.
"""
import unittest

from edmonds_karp import edmonds_karp

class TestMaximumFlowPipeline(unittest.TestCase):
    """
    An execution matrix responsible for validating empty states, path extensions,
    boundary limits, structural topologies, and residual flow modification loops.
    """

    def test_01_empty_graph_matrix_initialization(self):
        """1. Verifies that an empty graph system registers parameters matching null boundaries."""
        matrix = []

        with self.assertRaises(ValueError):
            edmonds_karp(matrix, 0, 0)

    def test_02_isolated_unconnected_vertices_initialization(self):
        """2. Validates that default disconnected configurations correctly represent empty flow matrices."""
        matrix = [
            [0, 0, 0],
            [0, 0, 0],
            [0, 0, 0],
        ]

        maximum_flow = edmonds_karp(matrix, 0, 2)

        self.assertEqual(maximum_flow, 0)

    def test_03_strict_single_edge_matching_flows(self):
        """3. Confirms that minimal valid edge setups generate correct flow across dual components."""
        matrix = [
            [0, 10],
            [0, 0],
        ]

        maximum_flow = edmonds_karp(matrix, 0, 1)

        self.assertEqual(maximum_flow, 10)

    def test_04_valid_disjoint_pairing_matrices_retrieval(self):
        """4. Confirms that parallel independent paths correctly aggregate flow limits."""
        matrix = [
            [0, 10, 20, 0],
            [0, 0, 0, 10],
            [0, 0, 0, 20],
            [0, 0, 0, 0],
        ]

        maximum_flow = edmonds_karp(matrix, 0, 3)

        self.assertEqual(maximum_flow, 30)

    def test_05_bipartite_even_path_structural_equivalence(self):
        """5. Evaluates equality evaluations across standard alternating unbranched path layouts."""
        matrix = [
            [0, 15, 0, 0],
            [0, 0, 15, 0],
            [0, 0, 0, 15],
            [0, 0, 0, 0],
        ]

        maximum_flow = edmonds_karp(matrix, 0, 3)

        self.assertEqual(maximum_flow, 15)

    def test_06_bipartite_odd_path_extension_limits(self):
        """6. Verifies that extending pathways by odd lengths preserves core capacities uniformly."""
        matrix = [
            [0, 12, 0, 0, 0],
            [0, 0, 12, 0, 0],
            [0, 0, 0, 12, 0],
            [0, 0, 0, 0, 12],
            [0, 0, 0, 0, 0],
        ]

        maximum_flow = edmonds_karp(matrix, 0, 4)

        self.assertEqual(maximum_flow, 12)

    def test_07_bipartite_even_cycle_preservation_metrics(self):
        """7. Ensures path exploration across structural loops preserves localized forward boundaries."""
        matrix = [
            [0, 10, 0, 10],
            [0, 0, 10, 0],
            [0, 0, 0, 0],
            [0, 0, 10, 0],
        ]

        maximum_flow = edmonds_karp(matrix, 0, 2)

        self.assertEqual(maximum_flow, 20)

    def test_08_chord_intercept_prime_blossom_contraction(self):
        """8. Validates bottleneck constraints across tightly packed 3-node triangular networks."""
        matrix = [
            [0, 10, 10],
            [0, 0, 5],
            [0, 0, 0],
        ]

        maximum_flow = edmonds_karp(matrix, 0, 2)

        self.assertEqual(maximum_flow, 15)

    def test_09_tangent_intercept_five_node_blossom_resolution(self):
        """9. Confirms the baseline augment logic handles standard 5-node linear network routing."""
        matrix = [
            [0, 20, 20, 0, 0],
            [0, 0, 0, 10, 0],
            [0, 0, 0, 10, 0],
            [0, 0, 0, 0, 20],
            [0, 0, 0, 0, 0],
        ]

        maximum_flow = edmonds_karp(matrix, 0, 4)

        self.assertEqual(maximum_flow, 20)

    def test_10_scalar_multiplication_blossom_cross_chord_annihilation(self):
        """10. Checks that inserting internal cross chords dynamically resolves alternative tracks."""
        matrix = [
            [0, 15, 15, 0, 0],
            [0, 0, 5, 10, 0],
            [0, 0, 0, 0, 15],
            [0, 0, 0, 0, 15],
            [0, 0, 0, 0, 0],
        ]

        maximum_flow = edmonds_karp(matrix, 0, 4)

        self.assertEqual(maximum_flow, 25)

    def test_11_star_topology_hub_associativity(self):
        """11. Evaluates central hub node properties across multiple star layout vertex distributions."""
        matrix = [
            [0, 30, 0, 0],
            [0, 0, 15, 15],
            [0, 0, 0, 0],
            [0, 0, 0, 0],
        ]

        maximum_flow = edmonds_karp(matrix, 0, 2)

        self.assertEqual(maximum_flow, 15)

    def test_12_complete_bipartite_grid_subgroup_wrap(self):
        """12. Boundary Test: Verifies fully connected bipartite structures yield stable maximum bounds."""
        matrix = [
            [0, 0, 0, 10, 10, 10],
            [0, 0, 0, 10, 10, 10],
            [0, 0, 0, 10, 10, 10],
            [0, 0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0, 0],
        ]

        maximum_flow = edmonds_karp(matrix, 0, 5)

        self.assertEqual(maximum_flow, 10)

    def test_13_complete_clique_four_distribution(self):
        """13. Confirms fully meshed K4 systems isolate flow to optimal terminal constraints."""
        matrix = [
            [0, 10, 10, 10],
            [10, 0, 10, 10],
            [10, 10, 0, 10],
            [10, 10, 10, 0],
        ]

        maximum_flow = edmonds_karp(matrix, 0, 3)

        self.assertEqual(maximum_flow, 30)

    def test_14_complete_clique_five_bounds_generation(self):
        """14. Validates that dense K5 networks generate expected maximum limits under total connectivity."""
        matrix = [
            [0, 5, 5, 5, 5],
            [5, 0, 5, 5, 5],
            [5, 5, 0, 5, 5],
            [5, 5, 5, 0, 5],
            [5, 5, 5, 5, 0],
        ]

        maximum_flow = edmonds_karp(matrix, 0, 4)

        self.assertEqual(maximum_flow, 20)

    def test_15_e2e_blossom_with_pendant_stem_loops(self):
        """15. Core Pipeline: Tests textbook network pipelines with structured routing constraints."""
        matrix = [
            [0, 16, 13, 0, 0, 0],
            [0, 0, 10, 12, 0, 0],
            [0, 4, 0, 0, 14, 0],
            [0, 0, 9, 0, 0, 20],
            [0, 0, 0, 7, 0, 4],
            [0, 0, 0, 0, 0, 0],
        ]

        maximum_flow = edmonds_karp(matrix, 0, 5)

        self.assertEqual(maximum_flow, 23)

    def test_16_bridged_double_blossom_rejection(self):
        """16. Security Test: Assures independent interior routing lanes preserve base sets."""
        matrix = [
            [0, 10, 0, 0, 0],
            [0, 0, 5, 0, 0],
            [0, 0, 0, 5, 0],
            [0, 0, 0, 0, 10],
            [0, 0, 0, 0, 0],
        ]

        maximum_flow = edmonds_karp(matrix, 0, 4)

        self.assertEqual(maximum_flow, 5)

    def test_17_nested_overlapping_blossoms_rejection(self):
        """17. Security Test: Checks that cyclic loops do not lock or corrupt component lookups."""
        matrix = [
            [0, 10, 0, 0],
            [0, 0, 10, 5],
            [0, 5, 0, 5],
            [0, 0, 0, 0],
        ]

        maximum_flow = edmonds_karp(matrix, 0, 3)

        self.assertEqual(maximum_flow, 10)

    def test_18_disconnected_heterogeneous_subgraphs_rejection(self):
        """18. Security Test: Validates independent subgraphs compile safely without parameter collisions."""
        matrix = [
            [0, 10, 0, 0, 0],
            [0, 0, 0, 0, 0],
            [0, 0, 0, 10, 10],
            [0, 0, 0, 0, 10],
            [0, 0, 0, 0, 0],
        ]

        maximum_flow = edmonds_karp(matrix, 0, 4)

        self.assertEqual(maximum_flow, 0)

    def test_19_suboptimal_path_reversal_rejection(self):
        """19. Boundary Test: Forces the search schema to successfully back-augment flow via reverse tracks."""
        matrix = [
            [0, 10, 10, 0],
            [0, 0, 10, 10],
            [0, 0, 0, 10],
            [0, 0, 0, 0],
        ]

        maximum_flow = edmonds_karp(matrix, 0, 3)

        self.assertEqual(maximum_flow, 20)

    def test_20_butterfly_focal_point_rejection(self):
        """20. Boundary Test: Assures flow accurately scales across pinched single-vertex intersection points."""
        matrix = [
            [0, 10, 10, 0, 0],
            [0, 0, 0, 5, 0],
            [0, 0, 0, 5, 0],
            [0, 0, 0, 0, 20],
            [0, 0, 0, 0, 0],
        ]

        maximum_flow = edmonds_karp(matrix, 0, 4)

        self.assertEqual(maximum_flow, 10)

if __name__ == "__main__":
    unittest.main()