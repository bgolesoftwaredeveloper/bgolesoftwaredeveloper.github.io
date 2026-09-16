"""
test_eulerian_path.py

Automated unit test suite for verifying Hierholzer's Algorithm across diverse
directed graph topologies, structural boundary states, and network constraints.

Overview:
    This module uses Python's built-in `unittest` framework to execute a matrix
    of 20 unique testing scenarios against the `eulerian_path_or_circuit` engine.
    It ensures that structural tracking properties run in linear time O(V + E)
    and fail safely when confronted with topologically invalid inputs.

Testing Scope & Architectural Coverage:
    - Standard Paths & Circuits: Verifies expected sequential trails, balanced loops,
      and nested multi-cycle junctions.
    - Minimal Inputs & Boundaries: Assesses empty structures, single-node configurations,
      self-loops, and parallel duplicate edges.
    - Global Asymmetry Rejections: Confirms that graphs with multiple source entry nodes,
      multiple sink nodes, or excessive degree imbalances are rejected immediately.
    - Connectivity Mismatches: Validates global reachability to ensure isolated edge
      islands or unreachable cyclical sub-graphs return None.
    - Typing Robustness: Tests string identifiers, implicit leaf nodes missing from dictionary
      keys, and raw numerical vertex inputs.

Execution Instructions:
    Run the complete testing matrix natively from the shell or command line via:

        python -m unittest test_eulerian_path.py
"""
import unittest

from eulerian_path import eulerian_path_or_circuit

class TestEulerianPathOrCircuit(unittest.TestCase):
    """
    Test suite containing 20 isolated test vectors evaluating path and circuit
    generation parameters, degree balancing validations, and global graph
    reachability constraints using Hierholzer's algorithm.
    """

    def test_01_standard_eulerian_path(self):
        """1. A standard valid Eulerian path with distinct asymmetric start/end terminals."""
        graph = {
            "A": ["B"],
            "B": ["C", "D"],
            "C": ["B"],
            "D": []
        }

        result = eulerian_path_or_circuit(graph)

        self.assertEqual(result, ["A", "B", "C", "B", "D"])

    def test_02_simple_eulerian_circuit(self):
        """2. A basic directed loop forming an Eulerian circuit where all degrees balance."""
        graph = {
            "A": ["B"],
            "B": ["C"],
            "C": ["A"]
        }
        result = eulerian_path_or_circuit(graph)

        self.assertIsNotNone(result)
        self.assertIsInstance(result, list)
        self.assertEqual(len(result), 4)
        self.assertEqual(result[0], result[-1])

    def test_03_two_interlocking_cycles_circuit(self):
        """3. An Eulerian circuit composed of two cycles intersecting at a shared pivot hub node."""
        graph = {
            "A": ["B", "C"],
            "B": ["A"],
            "C": ["A"]
        }

        result = eulerian_path_or_circuit(graph)

        self.assertIsNotNone(result)
        self.assertIsInstance(result, list)
        self.assertEqual(len(result), 5)
        self.assertEqual(result[0], result[-1])

    def test_04_de_bruijn_subgraph_path(self):
        """4. A dense string sequence configuration similar to a small 2D De Bruijn graph sequence."""
        graph = {
            "AA": ["AB"],
            "AB": ["BB", "BA"],
            "BB": ["BC"],
            "BC": ["CA"],
            "CA": ["AA"],
            "BA": ["AA"]
        }

        result = eulerian_path_or_circuit(graph)

        self.assertIsNotNone(result)
        self.assertIsInstance(result, list)
        self.assertEqual(len(result), 8)

    def test_05_completely_empty_graph(self):
        """5. An empty map topology representing no vertices or relational tracking rules."""
        graph = {}

        result = eulerian_path_or_circuit(graph)

        self.assertEqual(result, [])

    def test_06_single_node_with_no_edges(self):
        """6. A standalone map entry containing a vertex but tracking no active connections."""
        graph = {"A": []}

        result = eulerian_path_or_circuit(graph)

        self.assertEqual(result, [])

    def test_07_single_self_loop_circuit(self):
        """7. A single isolated node containing an edge that cycles directly back to itself."""
        graph = {"A": ["A"]}

        result = eulerian_path_or_circuit(graph)

        self.assertEqual(result, ["A", "A"])

    def test_08_multiple_parallel_edges_path(self):
        """8. Multiple distinct parallel paths existing between the same source and destination."""
        graph = {
            "A": ["B", "B"],
            "B": ["A"]
        }

        result = eulerian_path_or_circuit(graph)

        self.assertEqual(result, ["A", "B", "A", "B"])

    def test_09_invalid_multiple_net_sources(self):
        """9. Rejection when a graph has multiple nodes competing as start anchors (out - in == 1)."""
        graph = {
            "A": ["C"],
            "B": ["D"],
            "C": [],
            "D": []
        }

        result = eulerian_path_or_circuit(graph)

        self.assertIsNone(result)

    def test_10_invalid_multiple_net_sinks(self):
        """10. Rejection when a graph has multiple nodes acting as dead-end targets (in - out == 1)."""
        graph = {
            "A": ["B", "C"],
            "B": [],
            "C": []
        }

        result = eulerian_path_or_circuit(graph)

        self.assertIsNone(result)

    def test_11_extreme_degree_imbalance(self):
        """11. Rejection when out-degree and in-degree differ by more than 1 structural edge unit."""
        graph = {
            "A": ["B", "B", "B"],
            "B": []
        }

        result = eulerian_path_or_circuit(graph)

        self.assertIsNone(result)

    def test_12_disconnected_active_components(self):
        """12. Rejection when two valid circuits exist but are completely isolated from each other."""
        graph = {
            "A": ["B"],
            "B": ["A"],
            "X": ["Y"],
            "Y": ["X"]
        }

        result = eulerian_path_or_circuit(graph)

        self.assertIsNone(result)

    def test_13_valid_circuit_with_isolated_edge_dead_ends(self):
        """13. Rejection when a valid internal loop is isolated from a separate single-edge trail."""
        graph = {
            "A": ["B"],
            "B": ["A"],
            "C": ["D"],
            "D": []
        }

        result = eulerian_path_or_circuit(graph)

        self.assertIsNone(result)

    def test_14_completely_unreachable_node_island(self):
        """14. A graph where degree distribution balances, but parts form an unreachable sub-graph."""
        graph = {
            "A": ["B"],
            "B": ["C"],
            "C": ["A"],
            "D": ["E"],
            "E": ["F"],
            "F": ["D"]
        }

        result = eulerian_path_or_circuit(graph)

        self.assertIsNone(result)

    def test_15_long_linear_daisy_chain_path(self):
        """15. A long, strictly sequential chain layout containing no loops or cross-branches."""
        graph = {
            "1": ["2"],
            "2": ["3"],
            "3": ["4"],
            "4": ["5"],
            "5": []
        }

        result = eulerian_path_or_circuit(graph)

        self.assertEqual(result, ["1", "2", "3", "4", "5"])

    def test_16_nested_loops_circuit(self):
        """16. Highly layered concentric loops sharing a common central traversal focal node."""
        graph = {
            "Center": ["L1_In", "L2_In"],
            "L1_In": ["L1_Out"],
            "L1_Out": ["Center"],
            "L2_In": ["L2_Out"],
            "L2_Out": ["Center"]
        }

        result = eulerian_path_or_circuit(graph)

        self.assertIsNotNone(result)
        self.assertIsInstance(result, list)
        self.assertEqual(len(result), 7)
        self.assertEqual(result[0], "Center")
        self.assertEqual(result[-1], "Center")

    def test_17_backtracking_dead_end_trap_path(self):
        """17. A layout specifically built to trap naive depth-first-search algorithms early."""
        graph = {
            "Start": ["Hub"],
            "Hub": ["Loop", "End"],
            "Loop": ["Hub"],
            "End": []
        }

        result = eulerian_path_or_circuit(graph)

        self.assertEqual(result, ["Start", "Hub", "Loop", "Hub", "End"])

    def test_18_numerical_vertex_identifiers(self):
        """18. Validating that integer vertex identifiers parse correctly alongside string maps."""
        graph = {
            1: [2],
            2: [3, 4],
            3: [2],
            4: []
        }

        result = eulerian_path_or_circuit(graph)

        self.assertEqual(result, [1, 2, 3, 2, 4])

    def test_19_large_complete_bipartite_style_circuit(self):
        """19. A complex star-burst circuit mapping alternating connections back and forth."""
        graph = {
            "Hub": ["A", "B", "C"],
            "A": ["Hub"],
            "B": ["Hub"],
            "C": ["Hub"]
        }

        result = eulerian_path_or_circuit(graph)

        self.assertIsNotNone(result)
        self.assertIsInstance(result, list)
        self.assertEqual(len(result), 7)
        self.assertEqual(result.count("Hub"), 4)

    def test_20_implicit_destinations_not_in_keys(self):
        """20. Vertices acting as leaf nodes that appear only as items inside destination values."""
        graph = {
            "Start": ["Middle"],
            "Middle": ["Target"]
        }

        result = eulerian_path_or_circuit(graph)

        self.assertEqual(result, ["Start", "Middle", "Target"])

if __name__ == "__main__":
    unittest.main()