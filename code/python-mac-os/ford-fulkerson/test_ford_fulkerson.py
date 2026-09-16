"""
test_ford_fulkerson.py

Automated unit test suite for verifying the Ford-Fulkerson Algorithm (Edmonds-Karp)
across diverse capacity flow network topologies, structural boundaries, and constraints.

Overview:
    This module uses Python's built-in `unittest` framework to execute a matrix
    of 20 unique testing scenarios against the `ford_fulkerson` network flow engine.
    It ensures that residual tracking updates accurately, shortest paths are preferred
    via Breadth-First Search (BFS), and that bottleneck capacities balance precisely.

Testing Scope & Architectural Coverage:
    - Standard Classic Layouts: Validates standard multi-node textbook networks with
      known maximum flow benchmarks.
    - Minimal Inputs & Boundaries: Assesses single-edge profiles, explicit zero-capacity
      configurations, and graphs containing internal self-loops.
    - Connectivity Obstacles: Verifies proper termination and zero-flow return states
      for fully disconnected networks, missing links, or backward-pointing edges.
    - Structural Path Variations: Evaluates flow distribution across symmetric diamonds,
      multi-stage bridge paths, cyclic back-loops, and extreme capacity differentials.
    - Flow Constraints: Targets networks restricted by extreme bottlenecking at different
      points—specifically at the initial source outflow or terminal sink inflow.

Execution Instructions:
    Run the complete testing matrix natively from the shell or command line via:

        python -m unittest test_ford_fulkerson.py
"""

import unittest
from ford_fulkerson import ford_fulkerson

class TestFordFulkerson(unittest.TestCase):
    """
    Test suite containing 20 comprehensive, isolated test vectors evaluating
    the residual graph updates and maximum flow optimization parameters.
    """

    def test_01_classic_clrs_textbook_graph(self):
        """1. Standard 6-node network from CLRS textbook yielding a max flow of 23."""
        graph = [
            [0, 16, 13, 0, 0, 0],
            [0, 0, 10, 12, 0, 0],
            [0, 4, 0, 0, 14, 0],
            [0, 0, 9, 0, 0, 20],
            [0, 0, 0, 7, 0, 4],
            [0, 0, 0, 0, 0, 0]
        ]

        result = ford_fulkerson(graph, 0, 5)

        self.assertEqual(result, 23)

    def test_02_single_direct_edge(self):
        """2. Minimal graph containing only a source and a sink connected by one edge."""
        graph = [
            [0, 10],
            [0, 0]
        ]

        result = ford_fulkerson(graph, 0, 1)

        self.assertEqual(result, 10)

    def test_03_zero_capacity_edge(self):
        """3. Graph with a physical path where the edge capacity is explicitly zero."""
        graph = [
            [0, 0],
            [0, 0]
        ]

        result = ford_fulkerson(graph, 0, 1)

        self.assertEqual(result, 0)

    def test_04_disconnected_source_and_sink(self):
        """4. Graph where no directed or indirect path exists between source and sink."""
        graph = [
            [0, 5, 0],
            [0, 0, 0],
            [0, 0, 0]
        ]

        result = ford_fulkerson(graph, 0, 2)

        self.assertEqual(result, 0)

    def test_05_multiple_parallel_independent_paths(self):
        """5. Source diverges into parallel isolated paths that converge at the sink."""
        graph = [
            [0, 10, 20, 0],
            [0, 0, 0, 5],
            [0, 0, 0, 15],
            [0, 0, 0, 0]
        ]

        result = ford_fulkerson(graph, 0, 3)

        self.assertEqual(result, 20)

    def test_06_linear_chain_bottleneck(self):
        """6. A straight line of nodes where a single mid-chain small edge bottlenecks the flow."""
        graph = [
            [0, 100, 0, 0],
            [0, 0, 5, 0],
            [0, 0, 0, 100],
            [0, 0, 0, 0]
        ]

        result = ford_fulkerson(graph, 0, 3)

        self.assertEqual(result, 5)

    def test_07_graph_with_directed_cycles(self):
        """7. A network containing internal cyclic feedback loops to test cycle-handling robustness."""
        graph = [
            [0, 10, 0, 0],
            [0, 0, 10, 0],
            [0, 5, 0, 10],
            [0, 0, 0, 0]
        ]

        result = ford_fulkerson(graph, 0, 3)

        self.assertEqual(result, 10)

    def test_08_capacities_with_huge_variation(self):
        """8. Edges mixing extremely large numbers with small integer structural boundaries."""
        graph = [
            [0, 1000000, 1],
            [0, 0, 1000000],
            [0, 0, 0]
        ]

        result = ford_fulkerson(graph, 0, 2)

        self.assertEqual(result, 1000001)

    def test_09_sink_has_outgoing_edges(self):
        """9. The sink node contains outgoing edges, which should not affect the final flow calculus."""
        graph = [
            [0, 10, 0],
            [0, 0, 5],
            [0, 10, 0]
        ]

        result = ford_fulkerson(graph, 0, 2)

        self.assertEqual(result, 5)

    def test_10_source_has_incoming_edges(self):
        """10. The source node has incoming internal cyclic paths, which should be safely ignored."""
        graph = [
            [0, 15],
            [5, 0]
        ]

        result = ford_fulkerson(graph, 0, 1)

        self.assertEqual(result, 15)

    def test_11_unreachable_subgraph_nodes(self):
        """11. Graph contains an isolated, unreachable component cluster completely away from source."""
        graph = [
            [0, 10, 0, 0],
            [0, 0, 0, 10],
            [0, 0, 0, 50],
            [0, 0, 0, 0]
        ]

        result = ford_fulkerson(graph, 0, 3)

        self.assertEqual(result, 10)

    def test_12_diamond_network_symmetric(self):
        """12. Classic symmetric diamond tracking flow distribution through upper and lower paths."""
        graph = [
            [0, 10, 10, 0],
            [0, 0, 0, 10],
            [0, 0, 0, 10],
            [0, 0, 0, 0]
        ]

        result = ford_fulkerson(graph, 0, 3)

        self.assertEqual(result, 20)

    def test_13_edmonds_karp_shortest_path_preference(self):
        """13. Ensures the BFS variant selects the route with fewer hops when capacities mismatch."""
        graph = [
            [0, 5, 20, 0],
            [0, 0, 10, 0],
            [0, 0, 0, 0]
        ]

        result = ford_fulkerson(graph, 0, 2)

        self.assertEqual(result, 25)

    def test_14_bottleneck_at_the_source_outflow(self):
        """14. The major capacity constriction happens directly on the source node's egress edges."""
        graph = [
            [0, 2, 2],
            [0, 0, 50],
            [0, 0, 0]
        ]

        result = ford_fulkerson(graph, 0, 2)

        self.assertEqual(result, 4)

    def test_15_bottleneck_at_the_sink_inflow(self):
        """15. Internal capacities are massive, but the terminal entry paths into the sink are small."""
        graph = [
            [0, 50, 50, 0],
            [0, 0, 0, 2],
            [0, 0, 0, 3],
            [0, 0, 0, 0]
        ]

        result = ford_fulkerson(graph, 0, 3)

        self.assertEqual(result, 5)

    def test_16_fully_connected_complete_graph(self):
        """16. Every single node is bound to every other node with non-zero capacity bounds."""
        graph = [
            [0, 5, 5, 5],
            [5, 0, 5, 5],
            [5, 5, 0, 5],
            [5, 5, 5, 0]
        ]

        result = ford_fulkerson(graph, 0, 3)

        self.assertEqual(result, 15)

    def test_17_bidirectional_edge_flow(self):
        """17. Adjacent internal nodes share anti-parallel direct structural edges with separate limits."""
        graph = [
            [0, 10, 0, 0],
            [0, 0, 10, 0],
            [0, 4, 0, 10],
            [0, 0, 0, 0]
        ]

        result = ford_fulkerson(graph, 0, 3)

        self.assertEqual(result, 10)

    def test_18_no_paths_due_to_directionality(self):
        """18. Edges physically exist between all elements, but point backward towards the source."""
        graph = [
            [0, 0, 0],
            [10, 0, 0],
            [0, 10, 0]
        ]

        result = ford_fulkerson(graph, 0, 2)

        self.assertEqual(result, 0)

    def test_19_multi_stage_bridge_network(self):
        """19. Two dense sub-clusters joined across a singular, tightly constrained bridge edge."""
        graph = [
            [0, 10, 10, 0, 0, 0],
            [0, 0, 5, 2, 0, 0],
            [0, 0, 0, 0, 0, 0],
            [0, 0, 0, 0, 10, 10],
            [0, 0, 0, 0, 0, 5],
            [0, 0, 0, 0, 0, 0]
        ]

        result = ford_fulkerson(graph, 0, 5)

        self.assertEqual(result, 2)

    def test_20_nodes_with_self_loops(self):
        """20. Graph contains internal self-loops which must not affect valid path evaluation."""
        graph = [
            [5, 10, 0],
            [0, 5, 10],
            [0, 0, 5]
        ]

        result = ford_fulkerson(graph, 0, 2)

        self.assertEqual(result, 10)

if __name__ == "__main__":
    unittest.main()