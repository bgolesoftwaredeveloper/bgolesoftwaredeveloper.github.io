"""
test_kargers_algorithm.py

Automated unit test suite for verifying Karger's randomized minimum cut algorithm
across diverse topological variations, structural boundaries, and data constraints.

Overview:
    This module uses Python's built-in `unittest` framework to execute a matrix
    of 20 unique testing scenarios against the `find_minimum_cut`, `kargers`, and
    `contract_edge` execution engines. It ensures that edge contractions run cleanly,
    self-loops filter out accurately, and deterministic or probabilistic graph cuts
    converge onto proper topological boundaries.

Testing Scope & Architectural Coverage:
    - Base Contraction Functions: Validates structural behavior during edge contraction,
      reference updating, and self-loop isolation.
    - Minimal Inputs & Boundaries: Assesses base-case topologies like 2-node simple links,
      3-node triangles, and disconnected or fully dense graphs.
    - Deterministic Structures: Targets specialized trees and linear chains where the
      minimum cut configuration has an absolute certainty of 1.
    - Complex Structural Graph Topologies: Evaluates dumbbell networks, complete cliques,
      bipartite configurations, and multi-edge clusters requiring higher test repetition.

Execution Instructions:
    Run the complete testing matrix natively from the shell or command line via:

        python -m unittest test_kargers_algorithm.py
"""
import unittest

from kargers import find_minimum_cut, kargers, contract_edge

class TestKargersAlgorithm(unittest.TestCase):
    """
    Test suite containing 20 isolated test vectors evaluating graph contractions,
    structural boundaries, and probabilistic minimum cut discovery parameters.
    """

    def test_01_contract_edge_merges_adjacency_lists(self):
        """1. Verifies that contract_edge combines neighbor structures of source and target."""
        graph = {1: [2, 3], 2: [1], 3: [1]}

        contract_edge(graph, source=1, target=2)

        # Vertex 2 should be deleted and its connection to 1 removed as a self-loop.
        self.assertNotIn(2, graph)
        self.assertEqual(graph[1], [3])

    def test_02_contract_edge_updates_neighbor_references(self):
        """2. Checks that contract_edge redirects external nodes to point to the source."""
        graph = {1: [2], 2: [1, 3], 3: [2]}

        contract_edge(graph, source=1, target=2)

        # Vertex 3 should now point to vertex 1 instead of vertex 2.
        self.assertEqual(graph[3], [1])

    def test_03_contract_edge_filters_self_loops(self):
        """3. Confirms that structural self-loops are purged during edge processing."""
        graph = {1: [2, 2, 3], 2: [1, 1, 3], 3: [1, 2]}

        contract_edge(graph, source=1, target=2)

        # Parallel edges to target turn into self-loops and must be deleted.
        self.assertNotIn(1, graph[1])

    def test_04_minimal_two_node_graph(self):
        """4. Evaluates the absolute base case consisting of two connected vertices."""
        graph = {1: [2], 2: [1]}

        # A 2-node graph has exactly one possible cut size.
        cut_size = find_minimum_cut(graph, number_of_trials=1)

        self.assertEqual(cut_size, 1)

    def test_05_minimal_two_node_parallel_edges(self):
        """5. Evaluates a two-node graph containing multiple parallel cross-edges."""
        graph = {1: [2, 2, 2], 2: [1, 1, 1]}

        cut_size = find_minimum_cut(graph, number_of_trials=1)

        self.assertEqual(cut_size, 3)

    def test_06_three_node_triangle_clique(self):
        """6. Parses a 3-node fully connected cycle where any cut must sever 2 edges."""
        graph = {1: [2, 3], 2: [1, 3], 3: [1, 2]}

        cut_size = find_minimum_cut(graph, number_of_trials=5)

        self.assertEqual(cut_size, 2)

    def test_07_four_node_linear_chain(self):
        """7. Parses a sequential path network where the minimum cut isolated endpoint is 1."""
        graph = {1: [2], 2: [1, 3], 3: [2, 4], 4: [3]}

        cut_size = find_minimum_cut(graph, number_of_trials=10)

        self.assertEqual(cut_size, 1)

    def test_08_four_node_star_topology(self):
        """8. Validates star networks where a single central node links independent outer leaf nodes."""
        graph = {1: [2, 3, 4], 2: [1], 3: [1], 4: [1]}

        cut_size = find_minimum_cut(graph, number_of_trials=10)

        self.assertEqual(cut_size, 1)

    def test_09_complete_four_node_clique(self):
        """9. Evaluates a fully connected K4 graph where the degree of each node is 3."""
        graph = {
            1: [2, 3, 4],
            2: [1, 3, 4],
            3: [1, 2, 4],
            4: [1, 2, 3]
        }

        cut_size = find_minimum_cut(graph, number_of_trials=20)

        self.assertEqual(cut_size, 3)

    def test_10_dumbbell_bottleneck_graph(self):
        """10. Verifies a structural bottleneck graph bridging two dense triangles."""
        graph = {
            1: [2, 3], 2: [1, 3], 3: [1, 2, 4],
            4: [3, 5, 6], 5: [4, 6], 6: [4, 5]
        }

        cut_size = find_minimum_cut(graph, number_of_trials=30)

        self.assertEqual(cut_size, 1)

    def test_11_disconnected_components(self):
        """11. Evaluates graph tracking when components are completely isolated (cut size 0)."""
        graph = {1: [2], 2: [1], 3: [4], 4: [3]}

        cut_size = find_minimum_cut(graph, number_of_trials=5)

        self.assertEqual(cut_size, 0)

    def test_12_single_run_returns_integer(self):
        """12. Verifies that an isolated call to kargers returns a primitive int."""
        graph = {1: [2, 3], 2: [1, 3], 3: [1, 2]}

        cut_size = kargers(graph)

        self.assertIsInstance(cut_size, int)

    def test_13_preservation_of_input_graph(self):
        """13. Confirms the orchestration function uses deep copies without mutating input data."""
        graph = {1: [2], 2: [1]}
        graph_backup = {1: [2], 2: [1]}

        find_minimum_cut(graph, number_of_trials=5)

        self.assertEqual(graph, graph_backup)

    def test_14_complete_bipartite_graph(self):
        """14. Evaluates a K2,2 bipartite layout validating matching cut boundaries."""
        graph = {
            1: [3, 4], 2: [3, 4],
            3: [1, 2], 4: [1, 2]
        }

        cut_size = find_minimum_cut(graph, number_of_trials=20)

        self.assertEqual(cut_size, 2)

    def test_15_asymmetric_degrees_graph(self):
        """15. Tests a graph containing highly skewed vertex degrees to protect edge picking logic."""
        graph = {
            1: [2, 3, 4, 5],
            2: [1], 3: [1], 4: [1], 5: [1]
        }

        cut_size = find_minimum_cut(graph, number_of_trials=15)

        self.assertEqual(cut_size, 1)

    def test_16_five_node_cycle_ring(self):
        """16. Checks a basic ring topology configuration where any valid cut size must equal 2."""
        graph = {
            1: [2, 5], 2: [1, 3], 3: [2, 4],
            4: [3, 5], 5: [4, 1]
        }

        cut_size = find_minimum_cut(graph, number_of_trials=25)

        self.assertEqual(cut_size, 2)

    def test_17_string_node_labels(self):
        """17. Verifies compatibility with string identifiers instead of standard integers."""
        graph = {"alpha": ["beta"], "beta": ["alpha"]}

        cut_size = find_minimum_cut(graph, number_of_trials=1)

        self.assertEqual(cut_size, 1)

    def test_18_large_parallel_bridge_dumbbell(self):
        """18. Validates a dumbbell network unified by a thick multi-edge bridge."""
        graph = {
            1: [2, 3], 2: [1, 3], 3: [1, 2, 4, 4, 4],
            4: [3, 3, 3, 5, 6], 5: [4, 6], 6: [4, 5]
        }

        cut_size = find_minimum_cut(graph, number_of_trials=40)

        self.assertEqual(cut_size, 2)  # Cut isolating node 1 or 2 (size 2) is smaller than bridge (size 3)

    def test_19_zero_trials_boundary(self):
        """19. Checks execution boundary configurations when zero trials are requested."""
        graph = {1: [2], 2: [1]}

        cut_size = find_minimum_cut(graph, number_of_trials=0)

        self.assertEqual(cut_size, float("inf"))

    def test_20_self_contained_loop_initialization(self):
        """20. Confirms tracking parameters can process graphs initialized with pre-existing self-loops."""
        graph = {1: [1, 2], 2: [2, 1]}

        cut_size = find_minimum_cut(graph, number_of_trials=5)

        self.assertEqual(cut_size, 1)

if __name__ == "__main__":
    unittest.main()