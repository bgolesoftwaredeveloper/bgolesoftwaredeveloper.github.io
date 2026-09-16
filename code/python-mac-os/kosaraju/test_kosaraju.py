"""
test_kosaraju.py

Automated unit test suite for verifying Kosaraju's strongly connected components
(SCC) resolution engine across diverse topological structures and boundary states.

Overview:
    This module uses Python's built-in `unittest` framework to execute a matrix
    of 20 unique testing scenarios against the `Graph` and `KosarajuSolver`
    classes. It ensures that input constraints are enforced, topological sorting
    phases run cleanly, graph transpositions are symmetrical, and final component
    extractions match exact mathematical boundaries.

Testing Scope & Architectural Coverage:
    - Input & Boundary Conditions: Validates instantiation checks, type exceptions,
      out-of-bounds error captures, and trivial single-node graphs.
    - Basic Topologies: Assesses acyclic layouts (DAGs), linear chains, single
      isolated loops, and disconnected graph structures.
    - Complex Structural Graph Topologies: Evaluates dense cliques, interlocking loops,
      symmetric rings, bottleneck dumbbells, and giant components.
    - Component Formatting: Verifies proper state tracking, cleanup procedures,
      and consistency across repeated executions.

Execution Instructions:
    Run the complete testing matrix natively from the shell or command line via:

        python -m unittest test_kosaraju_scc.py
"""
import unittest

from kosaraju import Graph, KosarajuSolver

class TestKosarajuSolver(unittest.TestCase):
    """
    Test suite containing 20 isolated test vectors evaluating directed graph
    topologies, execution boundaries, and strongly connected components.
    """

    def test_01_invalid_graph_size_types(self):
        """1. Verifies that initializing a Graph with invalid types raises a TypeError."""
        with self.assertRaises(TypeError):
            Graph(size="five")

    def test_02_invalid_graph_size_value(self):
        """2. Checks that initializing a Graph with a size less than 1 raises a ValueError."""
        with self.assertRaises(ValueError):
            Graph(size=0)

    def test_03_add_edge_out_of_bounds_origin(self):
        """3. Confirms that adding an edge with an out-of-bounds origin index raises a ValueError."""
        network = Graph(3)

        with self.assertRaises(ValueError):
            network.add_edge(origin=3, target=1)

    def test_04_add_edge_out_of_bounds_target(self):
        """4. Confirms that adding an edge with an out-of-bounds target index raises a ValueError."""
        network = Graph(3)

        with self.assertRaises(ValueError):
            network.add_edge(0, -1)

    def test_05_solver_invalid_graph_type(self):
        """5. Assesses that initializing a KosarajuSolver with an invalid instance type raises a TypeError."""
        with self.assertRaises(TypeError):
            KosarajuSolver(graph="Not AGraphInstance")

    def test_06_minimal_single_node_graph(self):
        """6. Evaluates the base case graph consisting of an isolated single vertex."""
        network = Graph(size=1)

        solver = KosarajuSolver(network)

        components = solver.compute_strongly_connected_components()

        self.assertEqual(len(components), 1)
        self.assertEqual(sorted(components[0]), [0])

    def test_07_disconnected_nodes(self):
        """7. Parses a graph containing multiple vertices with zero connecting edge vectors."""
        network = Graph(4)

        solver = KosarajuSolver(network)

        components = solver.compute_strongly_connected_components()

        self.assertEqual(len(components), 4)
        flat_components = [item for sublist in components for item in sublist]
        self.assertEqual(sorted(flat_components), [0, 1, 2, 3])

    def test_08_directed_acyclic_graph_dag(self):
        """8. Validates a straight Directed Acyclic Graph (DAG) with no cycles."""
        network = Graph(3)

        network.add_edge(0, 1)
        network.add_edge(1, 2)

        solver = KosarajuSolver(network)

        components = solver.compute_strongly_connected_components()

        self.assertEqual(len(components), 3)

    def test_09_two_isolated_components(self):
        """9. Evaluates network tracking when two separate sub-graphs are completely isolated from each other."""
        network = Graph(4)

        network.add_edge(0, 1)
        network.add_edge(1, 0)
        network.add_edge(2, 3)
        network.add_edge(3, 2)

        solver = KosarajuSolver(network)

        components = solver.compute_strongly_connected_components()

        self.assertEqual(len(components), 2)
        sorted_sccs = sorted([sorted(scc) for scc in components])
        self.assertEqual(sorted_sccs, [[0, 1], [2, 3]])

    def test_10_simple_two_node_cycle(self):
        """10. Tests a basic reciprocal loop layout connecting two nodes."""
        network = Graph(2)

        network.add_edge(0, 1)
        network.add_edge(1, 0)

        solver = KosarajuSolver(network)

        components = solver.compute_strongly_connected_components()

        self.assertEqual(len(components), 1)
        self.assertEqual(sorted(components[0]), [0, 1])

    def test_11_three_node_cycle_ring(self):
        """11. Checks a closed ring topology layout tracking clockwise loop pathways."""
        network = Graph(3)

        network.add_edge(0, 1)
        network.add_edge(1, 2)
        network.add_edge(2, 0)

        solver = KosarajuSolver(network)

        components = solver.compute_strongly_connected_components()

        self.assertEqual(len(components), 1)
        self.assertEqual(sorted(components[0]), [0, 1, 2])

    def test_12_self_loop_node(self):
        """12. Confirms tracking parameters ignore reflexive self-loops without grouping extra components."""
        network = Graph(2)

        network.add_edge(0, 0)
        network.add_edge(0, 1)

        solver = KosarajuSolver(network)

        components = solver.compute_strongly_connected_components()

        self.assertEqual(len(components), 2)

    def test_13_complete_clique_k4(self):
        """13. Evaluates a fully connected K4 clique where every node links to every other node."""
        network = Graph(4)

        for origin in range(4):
            for target in range(4):
                if origin != target:
                    network.add_edge(origin, target)

        solver = KosarajuSolver(network)

        components = solver.compute_strongly_connected_components()

        self.assertEqual(len(components), 1)
        self.assertEqual(sorted(components[0]), [0, 1, 2, 3])

    def test_14_dumbbell_bottleneck_graph(self):
        """14. Verifies a structural bottleneck graph bridging two independent loop rings."""
        network = Graph(6)

        network.add_edge(0, 1)
        network.add_edge(1, 2)
        network.add_edge(2, 0)
        network.add_edge(2, 3)
        network.add_edge(3, 4)
        network.add_edge(4, 5)
        network.add_edge(5, 3)

        solver = KosarajuSolver(network)

        components = solver.compute_strongly_connected_components()

        self.assertEqual(len(components), 2)
        sorted_sccs = sorted([sorted(scc) for scc in components])
        self.assertEqual(sorted_sccs, [[0, 1, 2], [3, 4, 5]])

    def test_15_interlocking_sub_cycles(self):
        """15. Parses a highly interconnected arrangement containing nested sub-cycles."""
        network = Graph(4)

        network.add_edge(0, 1)
        network.add_edge(1, 2)
        network.add_edge(2, 0)
        network.add_edge(1, 3)
        network.add_edge(3, 1)

        solver = KosarajuSolver(network)

        components = solver.compute_strongly_connected_components()

        self.assertEqual(len(components), 1)
        self.assertEqual(sorted(components[0]), [0, 1, 2, 3])

    def test_16_long_linear_chain_scc(self):
        """16. Parses a giant sequence loop checking scaling depth limits during reconstruction passes."""
        size = 50

        network = Graph(size)

        for node in range(size):
            network.add_edge(node, (node + 1) % size)

        solver = KosarajuSolver(network)

        components = solver.compute_strongly_connected_components()

        self.assertEqual(len(components), 1)
        self.assertEqual(len(components[0]), size)

    def test_17_multiple_execution_idempotency(self):
        """17. Confirms the orchestration solver cleans internal state registers to maintain reproducibility across runs."""
        network = Graph(3)

        network.add_edge(0, 1)
        network.add_edge(1, 2)
        network.add_edge(2, 0)

        solver = KosarajuSolver(network)

        initial_run = solver.compute_strongly_connected_components()
        end_run = solver.compute_strongly_connected_components()

        self.assertEqual(initial_run, end_run)

    def test_18_graph_inversion_structural_symmetry(self):
        """18. Validates the integrity of the private graph transposition routine."""
        network = Graph(3)

        network.add_edge(0, 1)
        network.add_edge(1, 2)

        solver = KosarajuSolver(network)

        inverted = solver._invert_graph()

        self.assertIn(0, inverted[1])
        self.assertIn(1, inverted[2])
        self.assertNotIn(1, inverted[0])

    def test_19_asymmetric_tree_star_topology(self):
        """19. Tests star networks with paths radiating outward from a central hub vector node."""
        network = Graph(4)

        network.add_edge(0, 1)
        network.add_edge(0, 2)
        network.add_edge(0, 3)

        solver = KosarajuSolver(network)

        components = solver.compute_strongly_connected_components()

        self.assertEqual(len(components), 4)

    def test_20_head_to_head_convergent_edges(self):
        """20. Checks convergence processing boundaries where separate pathways terminate at a shared node."""
        network = Graph(3)

        network.add_edge(0, 2)
        network.add_edge(1, 2)  # 0 and 1 both point downstream to 2

        solver = KosarajuSolver(network)

        components = solver.compute_strongly_connected_components()

        self.assertEqual(len(components), 3)

if __name__ == "__main__":
    unittest.main()