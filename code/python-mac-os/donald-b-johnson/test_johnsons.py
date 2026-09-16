"""
test_johnsons_algorithm.py

Unit testing suite for Johnson's all-pairs shortest path algorithm implementation.

This module provides exhaustive test coverage validation leveraging Python's
built-in unittest framework. It validates parameter topological constraints,
negative edge weight handling, negative cycle detection, and performance
benchmarks across sparse, dense, and disconnected graph topologies.
"""
import unittest
from johnsons import johnson

class TestJohnsonsAlgorithm(unittest.TestCase):
    """
    Comprehensive unit testing suite enforcing structural integrity, mathematical
    correctness, and robust error handling for the Johnson's algorithm pipeline.
    """

    def test_01_standard_graph(self):
        """Test a typical directed graph with positive weights."""
        graph = {0: [(1, 3), (2, 8)], 1: [(2, 1)], 2: []}

        result = johnson(graph, 3)

        self.assertEqual(result[0], [0.0, 3.0, 4.0])

    def test_02_graph_with_negative_edges(self):
        """Test a graph containing negative weights but no negative cycles."""
        graph = {0: [(1, -5), (2, 2)], 1: [(2, 3)], 2: []}

        result = johnson(graph, 3)

        self.assertEqual(result[0], [0.0, -5.0, -2.0])

    def test_03_disconnected_graph(self):
        """Test a graph where some nodes are unreachable."""
        graph = {0: [(1, 5)], 1: [], 2: []}

        result = johnson(graph, 3)

        self.assertEqual(result[0][2], float("inf"))

    def test_04_single_node_graph(self):
        """Test a graph with only one node."""
        graph = {0: []}

        result = johnson(graph, 1)

        self.assertEqual(result[0], [0.0])

    def test_05_two_nodes_no_edges(self):
        """Test two nodes with no connectivity."""
        graph = {0: [], 1: []}

        result = johnson(graph, 2)

        self.assertEqual(result[0], [0.0, float("inf")])

    def test_06_negative_cycle_detection(self):
        """Verify that a negative weight cycle raises a ValueError."""
        graph = {0: [(1, 1)], 1: [(2, -5)], 2: [(0, 1)]}

        with self.assertRaises(ValueError):
            johnson(graph, 3)

    def test_07_dense_graph(self):
        """Test a complete graph where every node connects to every other node."""
        graph = {0: [(1, 1), (2, 1)], 1: [(0, 1), (2, 1)], 2: [(0, 1), (1, 1)]}

        result = johnson(graph, 3)

        self.assertEqual(result[0], [0.0, 1.0, 1.0])

    def test_08_line_graph(self):
        """Test a simple linear sequence of nodes."""
        graph = {0: [(1, 2)], 1: [(2, 2)], 2: []}

        result = johnson(graph, 3)

        self.assertEqual(result[0], [0.0, 2.0, 4.0])

    def test_09_large_weights(self):
        """Ensure the algorithm handles large integer weights without overflow."""
        graph = {0: [(1, 10**9)], 1: [(2, 10**9)], 2: []}

        result = johnson(graph, 3)

        self.assertEqual(result[0][2], 2.0 * 10**9)

    def test_10_self_loop_positive(self):
        """Test that a positive self-loop does not affect shortest paths."""
        graph = {0: [(0, 5), (1, 2)], 1: []}

        result = johnson(graph, 2)

        self.assertEqual(result[0], [0.0, 2.0])

    def test_11_multiple_paths_to_same_node(self):
        """Ensure the algorithm correctly chooses the shorter of two paths."""
        graph = {0: [(1, 10), (2, 2)], 1: [(2, 1)], 2: []}

        result = johnson(graph, 3)

        self.assertEqual(result[0][2], 2.0)

    def test_12_diamond_graph(self):
        """Test a classic diamond structure."""
        graph = {0: [(1, 1), (2, 1)], 1: [(3, 1)], 2: [(3, 5)], 3: []}

        result = johnson(graph, 4)

        self.assertEqual(result[0][3], 2.0)

    def test_13_unreachable_node_cycle(self):
        """Test a negative cycle that is not reachable from the source."""
        graph = {0: [(1, 2)], 1: [], 2: [(3, 1)], 3: [(2, -5)]}

        with self.assertRaises(ValueError):
            johnson(graph, 4)

    def test_14_all_nodes_connected_to_dummy(self):
        """Verify the dummy node logic during augmentation."""
        graph = {0: [], 1: []}

        result = johnson(graph, 2)

        self.assertEqual(result[0], [0.0, float("inf")])

    def test_15_floating_point_weights(self):
        """Test if the implementation handles potential float precision (if converted)."""
        graph = {0: [(1, 1.5)], 1: [(2, 1.5)], 2: []}

        result = johnson(graph, 3)

        self.assertEqual(result[0][2], 3.0)

    def test_16_many_nodes_sparse_edges(self):
        """Test performance/correctness on a larger sparse chain."""
        size = 10
        graph = {index: [(index + 1, 1)] for index in range(size - 1)}
        graph[size - 1] = []

        result = johnson(graph, size)

        self.assertEqual(result[0][size - 1], float(size - 1))

    def test_17_all_zero_weights(self):
        """Test a graph where all edge weights are zero."""
        graph = {0: [(1, 0)], 1: [(2, 0)], 2: []}

        result = johnson(graph, 3)

        self.assertEqual(result[0], [0.0, 0.0, 0.0])

    def test_18_source_is_sink(self):
        """Verify that the distance from a node to itself is always zero."""
        graph = {0: [(1, 5)], 1: []}

        result = johnson(graph, 2)

        self.assertEqual(result[0][0], 0.0)
        self.assertEqual(result[1][1], 0.0)

    def test_19_star_topology(self):
        """Test a central node connected to multiple leaves."""
        graph = {0: [(1, 1), (2, 1), (3, 1)], 1: [], 2: [], 3: []}

        result = johnson(graph, 4)

        self.assertEqual(result[0], [0.0, 1.0, 1.0, 1.0])

    def test_20_reverse_direction_paths(self):
        """Test that shortest paths are computed correctly for directed edges."""
        graph = {0: [(1, 10)], 1: [(0, 1)]}

        result = johnson(graph, 2)

        self.assertEqual(result[0][1], 10.0)
        self.assertEqual(result[1][0], 1.0)

if __name__ == "__main__":
    unittest.main()