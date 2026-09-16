"""
test_articulation_cut_vertex.py

Unit testing suite for the TopologicalNetwork graph analysis engine implementation.

This module provides exhaustive test coverage validation leveraging Python's
built-in unittest framework. It systematically verifies structural graph topologies,
edge cases (including lines, cycles, trees, stars, and completely disconnected elements),
defensive parameter input validation, and structural network bottleneck isolation metrics.
"""
import unittest
from articulation_cut_vertex import TopologicalNetwork

class TestTopologicalNetwork(unittest.TestCase):
    """
    Comprehensive unit testing suite enforcing topological accuracy, algorithmic correctness,
    and active defensive boundary validations for TopologicalNetwork.
    """

    def test_01_empty_graph_returns_no_articulation_points(self):
        """Verify an newly instantiated, completely empty graph returns an empty set."""
        network = TopologicalNetwork()

        self.assertEqual(network.retrieve_articulation_points(), set())

    def test_02_single_isolated_edge_has_no_articulation_points(self):
        """Verify a graph containing exactly two connected nodes contains no articulation points."""
        network = TopologicalNetwork()

        network.add_edge(0, 1)

        self.assertEqual(network.retrieve_articulation_points(), set())

    def test_03_type_error_on_non_integer_source_input(self):
        """Ensure TypeError is raised if the edge source parameter is not an integer."""
        network = TopologicalNetwork()

        with self.assertRaises(TypeError):
            network.add_edge("Node_A", 1)

    def test_04_type_error_on_non_integer_destination_input(self):
        """Ensure TypeError is raised if the edge destination parameter is not an integer."""
        network = TopologicalNetwork()

        with self.assertRaises(TypeError):
            network.add_edge(1, 2.5)

    def test_05_type_error_on_boolean_input_simulation(self):
        """Ensure TypeError is raised if a boolean value is passed, blocking integer casting tricks."""
        network = TopologicalNetwork()

        with self.assertRaises(TypeError):
            network.add_edge(True, 2)

    def test_06_value_error_on_self_loop_registration(self):
        """Ensure a ValueError is thrown if an edge points back to its own origin vertex."""
        network = TopologicalNetwork()

        with self.assertRaises(ValueError):
            network.add_edge(5, 5)

    def test_07_silents_duplicate_multi_edge_registrations(self):
        """Verify adding identical duplicate edges does not alter or corrupt the adjacency lists."""
        network = TopologicalNetwork()

        network.add_edge(0, 1)
        network.add_edge(0, 1)

        self.assertEqual(network.retrieve_articulation_points(), set())

    def test_08_linear_path_graph_identification(self):
        """Verify a simple line graph (0-1-2) correctly identifies internal transit nodes."""
        network = TopologicalNetwork()

        network.add_edge(0, 1)
        network.add_edge(1, 2)

        self.assertEqual(network.retrieve_articulation_points(), {1})

    def test_09_complete_cycle_graph_has_no_articulation_points(self):
        """Verify a closed ring topology can survive any single node failure without splitting."""
        network = TopologicalNetwork()

        network.add_edge(0, 1)
        network.add_edge(1, 2)
        network.add_edge(2, 0)

        self.assertEqual(network.retrieve_articulation_points(), set())

    def test_10_star_topology_root_isolation(self):
        """Verify a star topology isolates the central hub node as the exclusive bottleneck."""
        network = TopologicalNetwork()

        network.add_edge(0, 1)
        network.add_edge(0, 2)
        network.add_edge(0, 3)
        network.add_edge(0, 4)

        self.assertEqual(network.retrieve_articulation_points(), {0})

    def test_11_two_cycles_connected_by_a_bridge_edge(self):
        """Verify two distinct cycles linked by an explicit bridge identify both endpoints."""
        network = TopologicalNetwork()

        network.add_edge(0, 1)
        network.add_edge(1, 2)
        network.add_edge(2, 0)
        network.add_edge(2, 3)
        network.add_edge(3, 4)
        network.add_edge(4, 5)
        network.add_edge(5, 3)

        self.assertEqual(network.retrieve_articulation_points(), {2, 3})

    def test_12_two_cycles_sharing_a_single_vertex(self):
        """Verify two cycles intersecting at a single hourglass junction point isolates that hub."""
        network = TopologicalNetwork()

        network.add_edge(0, 1)
        network.add_edge(1, 2)
        network.add_edge(2, 0)
        network.add_edge(2, 3)
        network.add_edge(3, 4)
        network.add_edge(4, 2)

        self.assertEqual(network.retrieve_articulation_points(), {2})

    def test_13_disconnected_subgraphs_evaluated_systematically(self):
        """Ensure independent components with internal articulation points are parsed correctly."""
        network = TopologicalNetwork()

        network.add_edge(0, 1)
        network.add_edge(1, 2)
        network.add_edge(3, 4)
        network.add_edge(4, 5)

        self.assertEqual(network.retrieve_articulation_points(), {1, 4})

    def test_14_root_vertex_with_single_child_in_dfs_tree(self):
        """Verify a DFS root node with only one sequential tracking branch is not chosen as a cut point."""
        network = TopologicalNetwork()

        network.add_edge(0, 1)
        network.add_edge(1, 2)
        network.add_edge(2, 3)
        network.add_edge(3, 1)

        # Traversal originating at 0 views 0 as a root with 1 child. Nodes 1 is the true cut vertex.
        self.assertNotIn(0, network.retrieve_articulation_points())
        self.assertEqual(network.retrieve_articulation_points(), {1})

    def test_15_binary_tree_internal_node_isolation(self):
        """Verify a standard structured tree isolates all non-leaf elements."""
        network = TopologicalNetwork()

        # Root node 0.
        network.add_edge(0, 1)
        network.add_edge(0, 2)
        network.add_edge(1, 3)
        network.add_edge(1, 4)

        # Leaves 2, 3, 4 have no descendants. Nodes 0 and 1 are structural parents.
        self.assertEqual(network.retrieve_articulation_points(), {0, 1})

    def test_16_strongly_connected_clique_mesh_has_no_bottlenecks(self):
        """Verify a fully connected complete graph K5 returns an empty set due to high redundancy."""
        network = TopologicalNetwork()
        nodes = [0, 1, 2, 3, 4]
        
        for index in range(len(nodes)):
            for next in range(index + 1, len(nodes)):
                network.add_edge(nodes[index], nodes[next])

        self.assertEqual(network.retrieve_articulation_points(), set())

    def test_17_pendant_leaf_nodes_excluded_from_selection(self):
        """Ensure degree-1 terminal nodes (pendant edges) are never classified as cut vertices."""
        network = TopologicalNetwork()
        
        network.add_edge(0, 1)
        network.add_edge(1, 2)
        network.add_edge(2, 3)

        results = network.retrieve_articulation_points()

        self.assertNotIn(0, results)
        self.assertNotIn(3, results)

    def test_18_large_scale_unrolled_loop_intersections(self):
        """Validate complex geometric structures featuring loop paths intersecting structural lines."""
        network = TopologicalNetwork()

        network.add_edge(10, 20)
        network.add_edge(20, 30)
        network.add_edge(30, 10)
        network.add_edge(30, 40)
        network.add_edge(40, 50)

        self.assertEqual(network.retrieve_articulation_points(), {30, 40})

    def test_19_asymmetric_id_distribution_boundaries(self):
        """Ensure tracking arrays work seamlessly when node index identifiers are sparse or massive."""
        network = TopologicalNetwork()

        network.add_edge(999999, 55)
        network.add_edge(55, 1111)

        self.assertEqual(network.retrieve_articulation_points(), {55})

    def test_20_diamond_biconnected_envelope_exclusion(self):
        """Verify standard diamond configurations with an extra tail isolate only the tail anchor node."""
        network = TopologicalNetwork()

        # Diamond envelope.
        network.add_edge(0, 1)
        network.add_edge(0, 2)
        network.add_edge(1, 3)
        network.add_edge(2, 3)
        network.add_edge(3, 4)

        self.assertEqual(network.retrieve_articulation_points(), {3})

if __name__ == "__main__":
    unittest.main()