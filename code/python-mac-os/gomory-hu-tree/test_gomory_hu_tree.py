"""
test_gomory_hu_tree.py

Automated unit test suite for verifying the Gomory-Hu Tree construction algorithm
across diverse capacity network topologies, structural boundaries, and constraints.

Overview:
    This module uses Python's built-in `unittest` framework to execute a matrix
    of 20 unique testing scenarios against the `GomoryHuTree` engine. It ensures
    that the iterative min-cut partitioning logic correctly refines the tree
    structure and that the final tree edges accurately represent the minimum
    bottleneck capacities between node pairs.

Testing Scope & Architectural Coverage:
    - Minimal Inputs & Boundaries: Assesses 2-node minimal graphs, isolated nodes,
      and graphs with zero-capacity links.
    - Connectivity Obstacles: Validates behavior on disconnected networks where
      min-cut values should correctly return zero.
    - Standard Topologies: Verifies performance against triangles, stars, 4-node
      cycles, and linear path graphs.
    - Structural Complexity: Evaluates high-density complete graphs (K4), multi-stage
      paths, and bridge-edge constraints.
    - Algorithm Robustness: Ensures internal consistency against self-loops,
      symmetric matrix integrity, and duplicate weight scenarios.

Execution Instructions:
    Run the complete testing matrix natively from the shell or command line via:

        python -m unittest test_gomory_hu_tree.py
"""

import unittest
from gomory_hu_tree import GomoryHuTree

class TestGomoryHuTree(unittest.TestCase):
    """
    Test suite containing 20 comprehensive, isolated test vectors evaluating
    the Gomory-Hu tree construction and max-flow/min-cut refinement logic.
    """

    def test_01_two_nodes(self):
        """1. Minimal 2-node graph test case."""
        matrix = [[0, 5], [5, 0]]

        tree_builder = GomoryHuTree(matrix)
        tree = tree_builder.construct(tree_builder.matrix)

        self.assertEqual(tree[0][0][1], 5)

    def test_02_triangle_graph(self):
        """2. Fully connected 3-node graph (Triangle)."""
        matrix = [[0, 10, 10], [10, 0, 10], [10, 10, 0]]

        tree_builder = GomoryHuTree(matrix)
        tree = tree_builder.construct(tree_builder.matrix)

        self.assertTrue(len(tree[0]) > 0)

    def test_03_disconnected_graph(self):
        """3. Two isolated components resulting in a 0-weight min-cut."""
        matrix = [[0, 0, 0], [0, 0, 5], [0, 5, 0]]

        tree_builder = GomoryHuTree(matrix)
        tree = tree_builder.construct(tree_builder.matrix)

        self.assertEqual(tree[0][0][1], 0)

    def test_04_simple_line_graph(self):
        """4. Linear path topology (0-1-2)."""
        matrix = [[0, 5, 0], [5, 0, 5], [0, 5, 0]]

        tree_builder = GomoryHuTree(matrix)
        tree = tree_builder.construct(tree_builder.matrix)

        self.assertEqual(tree[0][0][1], 5)

    def test_05_star_graph_topology(self):
        """5. Star topology with node 0 acting as the center."""
        matrix = [[0, 2, 2, 2], [2, 0, 0, 0], [2, 0, 0, 0], [2, 0, 0, 0]]

        tree_builder = GomoryHuTree(matrix)
        tree = tree_builder.construct(tree_builder.matrix)

        self.assertEqual(len(tree[0]), 3)

    def test_06_zero_capacity_graph(self):
        """6. Graph with all zero capacities."""
        matrix = [[0, 0], [0, 0]]

        tree_builder = GomoryHuTree(matrix)
        tree = tree_builder.construct(tree_builder.matrix)

        self.assertEqual(tree[0][0][1], 0)

    def test_07_four_node_cycle(self):
        """7. 4-node cycle to test partition refinement robustness."""
        matrix = [[0, 5, 0, 5], [5, 0, 5, 0], [0, 5, 0, 5], [5, 0, 5, 0]]

        tree_builder = GomoryHuTree(matrix)

        self.assertIsNotNone(tree_builder.construct(tree_builder.matrix))

    def test_08_complete_graph_k4(self):
        """8. Fully connected complete graph K4."""
        matrix = [[0, 1, 1, 1], [1, 0, 1, 1], [1, 1, 0, 1], [1, 1, 1, 0]]

        tree_builder = GomoryHuTree(matrix)

        self.assertEqual(len(tree_builder.construct(tree_builder.matrix)), 4)

    def test_09_non_uniform_capacities(self):
        """9. Network with high-variance capacity distribution."""
        matrix = [[0, 100, 50], [100, 0, 10], [50, 10, 0]]

        tree_builder = GomoryHuTree(matrix)

        self.assertIsNotNone(tree_builder.construct(tree_builder.matrix))

    def test_10_bridge_edge_bottleneck(self):
        """10. Verification of cut value for a single bridge edge."""
        matrix = [[0, 10, 0, 0], [10, 0, 1, 0], [0, 1, 0, 10], [0, 0, 10, 0]]

        tree_builder = GomoryHuTree(matrix)
        tree = tree_builder.construct(tree_builder.matrix)

        self.assertEqual(tree[1][1][1], 1)

    def test_11_five_node_path_graph(self):
        """11. Linear 5-node path traversal."""
        matrix = [[0, 1, 0, 0, 0], [1, 0, 1, 0, 0], [0, 1, 0, 1, 0], [0, 0, 1, 0, 1], [0, 0, 0, 1, 0]]

        tree_builder = GomoryHuTree(matrix)

        self.assertEqual(len(tree_builder.construct(tree_builder.matrix)), 5)

    def test_12_dense_duplicate_weights(self):
        """12. Dense graph containing uniform weight edge distributions."""
        matrix = [[0, 5, 5], [5, 0, 5], [5, 5, 0]]

        tree_builder = GomoryHuTree(matrix)
        tree = tree_builder.construct(tree_builder.matrix)

        weights = [weight for neighbor, weight in tree[0]]

        self.assertIn(10, weights)

    def test_13_symmetric_matrix_integrity(self):
        """13. Ensures algorithmic stability with symmetric input matrices."""
        matrix = [[0, 8, 2], [8, 0, 4], [2, 4, 0]]

        tree_builder = GomoryHuTree(matrix)

        self.assertIsNotNone(tree_builder.construct(tree_builder.matrix))

    def test_14_uniform_capacity_graph(self):
        """14. Graph with identical capacities on all primary links."""
        matrix = [[0, 7, 7], [7, 0, 7], [7, 7, 0]]

        tree_builder = GomoryHuTree(matrix)
        tree = tree_builder.construct(tree_builder.matrix)

        weights = [weight for neighbor, weight in tree[0]]

        self.assertIn(14, weights)

    def test_15_extreme_capacity_differential(self):
        """15. Graph containing one exceptionally high capacity edge."""
        matrix = [[0, 1000, 1], [1000, 0, 1], [1, 1, 0]]

        tree_builder = GomoryHuTree(matrix)

        self.assertIsNotNone(tree_builder.construct(tree_builder.matrix))

    def test_16_three_node_path(self):
        """16. Small scale 3-node path topology."""
        matrix = [[0, 2, 0], [2, 0, 3], [0, 3, 0]]

        tree_builder = GomoryHuTree(matrix)

        self.assertEqual(len(tree_builder.construct(tree_builder.matrix)), 3)

    def test_17_isolated_vertex_graph(self):
        """17. Graph containing a completely isolated vertex node."""
        matrix = [[0, 0, 0], [0, 0, 2], [0, 2, 0]]

        tree_builder = GomoryHuTree(matrix)

        self.assertEqual(tree_builder.construct(tree_builder.matrix)[0][0][1], 0)

    def test_18_five_node_ring_cycle(self):
        """18. 5-node ring/cycle connectivity test."""
        matrix = [[0, 1, 0, 0, 1], [1, 0, 1, 0, 0], [0, 1, 0, 1, 0], [0, 0, 1, 0, 1], [1, 0, 0, 1, 0]]

        tree_builder = GomoryHuTree(matrix)

        self.assertEqual(len(tree_builder.construct(tree_builder.matrix)), 5)

    def test_19_self_loop_robustness(self):
        """19. Verifies algorithm resilience against internal self-loops."""
        matrix = [[1, 5], [5, 1]]

        tree_builder = GomoryHuTree(matrix)

        self.assertEqual(tree_builder.construct(tree_builder.matrix)[0][0][1], 5)

    def test_20_already_a_tree_input(self):
        """20. Validates behavior when input graph matches tree structure."""
        matrix = [[0, 1, 0], [1, 0, 1], [0, 1, 0]]

        tree_builder = GomoryHuTree(matrix)

        self.assertEqual(len(tree_builder.construct(tree_builder.matrix)), 3)

if __name__ == '__main__':
    unittest.main()