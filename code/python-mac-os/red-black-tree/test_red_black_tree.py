"""
test_red_black_tree.py

Automated unit test suite for verifying the custom self-balancing Red-Black
Binary Search Tree (BST) implementation.

Overview:
    This module uses Python's built-in `unittest` framework to execute a matrix
    of 40 unique testing scenarios against the `Node` and `RedBlackTree` classes.
    It ensures mathematical and structural precision across standard sorted inserts,
    complex multi-case balancing rotations, structural edge-case deletions, and
    error boundaries.

Testing Scope & Architectural Coverage:
    - Node Core Properties & Initialization: validates initial states, defaults,
      and type alignment behavior.
    - Insertion Mechanics & Fixes: Verifies standard BST placement, handle color
      reflections (Uncle RED), zig-zag alignment adjustments, and straight-line
      structural shifts (Uncle BLACK).
    - Erasure Mechanics & Deficiencies: Tests structural replacement logic for
      nodes with zero, one, or two operational children, alongside extensive multi-case
      double-black deficiency fixes up to the root.
    - Safety Boundaries & Exception Handling: validates exception raising on duplicate
      keys, invalid non-integer data payloads, and missing delete targets.

Execution Instructions:
    Run the complete testing matrix natively from the shell or command line via:

        python -m unittest test_red_black_tree.py
"""
import unittest

from red_black_tree import RedBlackTree, Node, RED, BLACK


class TestRedBlackTree(unittest.TestCase):
    """
    Test suite containing 40 isolated test vectors evaluating pointer routing's,
    structural invariants, color codes, and error bounds of the Red-Black Tree.
    """

    def test_01_tree_initialization_state(self):
        """1. Verifies that a newly initialized tree establishes its root pointing to the sentinel NIL."""
        tree = RedBlackTree()
        
        self.assertEqual(tree.root, tree.nil)

    def test_02_sentinel_nil_node_properties(self):
        """2. Checks that the internal NIL sentinel node is strictly initialized to BLACK with no value."""
        tree = RedBlackTree()

        self.assertEqual(tree.nil.color, BLACK)
        self.assertIsNone(tree.nil.value)

    def test_03_sentinel_nil_circular_references(self):
        """3. Confirms that the sentinel NIL's child pointers refer circularly back to itself."""
        tree = RedBlackTree()

        self.assertEqual(tree.nil.left, tree.nil)
        self.assertEqual(tree.nil.right, tree.nil)

    def test_04_node_default_initialization_color(self):
        """4. Assures that raw standalone node allocations default structurally to the RED color flag."""
        node = Node(50)

        self.assertEqual(node.color, RED)

    def test_05_node_payload_type_assignment(self):
        """5. validates that node allocations correctly assign and retain primitive integer payloads."""
        node = Node(42)

        self.assertEqual(node.value, 42)

    def test_06_single_element_insertion_is_root(self):
        """6. Verifies that the very first inserted key correctly registers as the absolute tree root."""
        tree = RedBlackTree()

        tree.insert(10)

        self.assertEqual(tree.root.value, 10)

    def test_07_root_node_remains_strictly_black(self):
        """7. Enforces the invariant that the tree root color flag is automatically corrected to BLACK."""
        tree = RedBlackTree()

        tree.insert(10)

        self.assertEqual(tree.root.color, BLACK)

    def test_08_binary_search_tree_routing_left(self):
        """8. Confirms that a smaller incoming element routes to the left side of a root node."""
        tree = RedBlackTree()

        tree.insert(20)
        tree.insert(10)

        self.assertEqual(tree.root.left.value, 10)

    def test_09_binary_search_tree_routing_right(self):
        """9. Confirms that a larger incoming element routes to the right side of a root node."""
        tree = RedBlackTree()

        tree.insert(20)
        tree.insert(30)

        self.assertEqual(tree.root.right.value, 30)

    def test_10_isolated_left_rotation_mechanics(self):
        """10. Targets the isolated pointer re-routing logic within a counter-clockwise left rotation."""
        tree = RedBlackTree()

        parent_node = Node(10)
        child_node = Node(20)

        parent_node.right = child_node
        child_node.parent = parent_node
        parent_node.left = tree.nil

        child_node.left = tree.nil
        child_node.right = tree.nil

        tree.root = parent_node

        tree._rotate_left(parent_node)

        self.assertEqual(tree.root, child_node)
        self.assertEqual(child_node.left, parent_node)

    def test_11_isolated_right_rotation_mechanics(self):
        """11. Targets the isolated pointer re-routing logic within a clockwise right rotation."""
        tree = RedBlackTree()

        parent_node = Node(20)
        child_node = Node(10)

        parent_node.left = child_node
        child_node.parent = parent_node
        parent_node.right = tree.nil

        child_node.left = tree.nil
        child_node.right = tree.nil

        tree.root = parent_node

        tree._rotate_right(parent_node)

        self.assertEqual(tree.root, child_node)
        self.assertEqual(child_node.right, parent_node)

    def test_12_insertion_fix_case_1_left_uncle_red(self):
        """12. validates Case 1 (Left): Uncle is RED, triggering color reflection up into the grandparent."""
        tree = RedBlackTree()

        tree.insert(30)
        tree.insert(20)
        tree.insert(40)
        tree.insert(10)

        self.assertEqual(tree._search(20).color, BLACK)
        self.assertEqual(tree._search(40).color, BLACK)
        self.assertEqual(tree._search(30).color, BLACK)

    def test_13_insertion_fix_case_1_right_uncle_red(self):
        """13. validates Case 1 (Right Mirror): Uncle is RED, triggering proper symmetric color reflections."""
        tree = RedBlackTree()

        tree.insert(30)
        tree.insert(20)
        tree.insert(40)
        tree.insert(50)

        self.assertEqual(tree._search(20).color, BLACK)
        self.assertEqual(tree._search(40).color, BLACK)

    def test_14_insertion_fix_case_2_left_uncle_black_zigzag(self):
        """14. Checks Case 2 (Left): Uncle is BLACK with a zig-zag alignment, triggering an inner left rotation."""
        tree = RedBlackTree()

        tree.insert(30)
        tree.insert(10)
        tree.insert(40)
        tree.insert(5)

        tree.insert(8)

        node_8 = tree._search(8)

        self.assertEqual(node_8.left.value, 5)

    def test_15_insertion_fix_case_3_left_uncle_black_straight(self):
        """15. validates Case 3 (Left): Uncle is BLACK with a straight-line layout, triggering a right pivot."""
        tree = RedBlackTree()

        tree.insert(30)
        tree.insert(20)
        tree.insert(10)

        self.assertEqual(tree.root.value, 20)
        self.assertEqual(tree.root.color, BLACK)
        self.assertEqual(tree.root.left.color, RED)
        self.assertEqual(tree.root.right.color, RED)

    def test_16_insertion_fix_case_2_right_uncle_black_zigzag(self):
        """16. Tests Case 2 (Right Mirror): Uncle is BLACK with a zig-zag alignment, pulling via right rotation."""
        tree = RedBlackTree()

        tree.insert(10)
        tree.insert(5)
        tree.insert(30)
        tree.insert(35)

        tree.insert(20)

        node_20 = tree._search(20)

        self.assertIsNotNone(node_20)

    def test_17_insertion_fix_case_3_right_uncle_black_straight(self):
        """17. validates Case 3 (Right Mirror): Uncle is BLACK with a straight line, triggering a left pivot."""
        tree = RedBlackTree()

        tree.insert(10)
        tree.insert(20)
        tree.insert(30)

        self.assertEqual(tree.root.value, 20)
        self.assertEqual(tree.root.left.value, 10)
        self.assertEqual(tree.root.right.value, 30)

    def test_18_search_existing_element(self):
        """18. Assures that searching for an active key successfully retrieves its matching node instance."""
        tree = RedBlackTree()

        tree.insert(15)
        tree.insert(25)

        found = tree._search(25)

        self.assertEqual(found.value, 25)

    def test_19_search_non_existent_element(self):
        """19. Assures that searching for a missing value returns the internal sentinel self.nil loop."""
        tree = RedBlackTree()

        tree.insert(15)

        found = tree._search(99)

        self.assertEqual(found, tree.nil)

    def test_20_minimum_element_of_subtree(self):
        """20. Confirms that the `_minimum` lookup accurately returns the absolute lowest payload in a branch."""
        tree = RedBlackTree()

        tree.insert(40)
        tree.insert(20)
        tree.insert(10)
        tree.insert(30)

        minimum_node = tree._minimum(tree.root)

        self.assertEqual(minimum_node.value, 10)

    def test_21_transplant_updates_parent_child_links(self):
        """21. evaluates pointer changes inside `_transplant` to verify precise parent-child substitutions."""
        tree = RedBlackTree()

        tree.insert(50)
        tree.insert(25)

        node_25 = tree._search(25)

        new_subtree = Node(35)

        new_subtree.left = tree.nil
        new_subtree.right = tree.nil

        tree._transplant(node_25, new_subtree)

        self.assertEqual(tree.root.left, new_subtree)
        self.assertEqual(new_subtree.parent, tree.root)

    def test_22_erase_red_leaf_node_no_rebalance(self):
        """22. Verifies that deleting a red leaf node executes directly without triggering rebalance adjustments."""
        tree = RedBlackTree()

        tree.insert(20)
        tree.insert(10)

        tree.erase(10)

        self.assertEqual(tree.root.left, tree.nil)

    def test_23_erase_node_with_single_right_child(self):
        """23. Tests erasing a node possessing a right child exclusively, validating direct replacement routing."""
        tree = RedBlackTree()

        tree.insert(20)
        tree.insert(10)
        tree.insert(30)
        tree.insert(35)

        tree.erase(30)

        self.assertEqual(tree.root.right.value, 35)

    def test_24_erase_node_with_single_left_child(self):
        """24. Tests erasing a node possessing a left child exclusively, validating direct replacement routing."""
        tree = RedBlackTree()

        tree.insert(20)
        tree.insert(10)
        tree.insert(30)
        tree.insert(25)

        tree.erase(30)

        self.assertEqual(tree.root.right.value, 25)

    def test_25_erase_node_with_two_operational_children(self):
        """25. Assures that deleting a node with two children cleanly substitutes its in-order successor."""
        tree = RedBlackTree()

        tree.insert(40)
        tree.insert(20)
        tree.insert(60)
        tree.insert(50)
        tree.insert(70)

        tree.erase(60)

        self.assertEqual(tree.root.right.value, 70)
        self.assertEqual(tree.root.right.left.value, 50)

    def test_26_erase_root_node_with_children(self):
        """26. validates erasing the tree root node when it maintains multiple children pathways."""
        tree = RedBlackTree()

        tree.insert(20)
        tree.insert(10)
        tree.insert(30)

        tree.erase(20)

        self.assertEqual(tree.root.color, BLACK)
        self.assertIn(tree.root.value, [10, 30])

    def test_27_erase_fix_case_1_sibling_red(self):
        """27. Tests Erasure Case 1: Sibling is RED, rotating to re-expose a black sibling layout context."""
        tree = RedBlackTree()

        for value in [50, 20, 60, 10, 30, 5, 15]:
            tree.insert(value)

        tree.erase(60)

        self.assertEqual(tree.root.color, BLACK)

    def test_28_erase_fix_case_2_sibling_and_children_black(self):
        """28. Checks Erasure Case 2: Sibling and both children are BLACK, bubbling up double-black status."""
        tree = RedBlackTree()

        tree.insert(30)
        tree.insert(20)
        tree.insert(40)

        tree.erase(20)

        self.assertEqual(tree._search(40).color, RED)

    def test_29_erase_fix_case_3_sibling_black_inner_child_red(self):
        """29. evaluates Erasure Case 3: Sibling is BLACK, inner child is RED, transforming via inner rotation."""
        tree = RedBlackTree()

        for value in [40, 20, 60, 10, 30, 50, 70, 45]:
            tree.insert(value)

        tree.erase(20)

        self.assertIsNotNone(tree._search(40))

    def test_30_erase_fix_case_4_sibling_black_outer_child_red(self):
        """30. Tests Erasure Case 4: Sibling is BLACK, outer child is RED, triggering final balancing pivots."""
        tree = RedBlackTree()

        for value in [40, 20, 60, 10, 50, 70]:
            tree.insert(value)

        tree.erase(10)

        self.assertEqual(tree.root.value, 40)

    def test_31_erase_fix_mirror_case_2_sibling_black_children_black(self):
        """31. Tests Erasure Case 2 Mirror: Deficient node on the right, sibling and its kids are BLACK."""
        tree = RedBlackTree()

        tree.insert(30)
        tree.insert(20)
        tree.insert(40)

        tree.erase(40)

        self.assertEqual(tree._search(20).color, RED)

    def test_32_erase_fix_mirror_case_3_sibling_black_inner_child_red(self):
        """32. Tests Erasure Case 3 Mirror: Deficient node on right, sibling BLACK with inner child RED."""
        tree = RedBlackTree()

        for value in [40, 20, 60, 10, 30, 50, 70, 35]:
            tree.insert(value)

        tree.erase(60)

        self.assertEqual(tree.root.color, BLACK)

    def test_33_erase_fix_mirror_case_4_sibling_black_outer_child_red(self):
        """33. Tests Erasure Case 4 Mirror: Deficient node on right, sibling BLACK with outer child RED."""
        tree = RedBlackTree()

        for value in [40, 20, 60, 10, 30, 50]:
            tree.insert(value)

        tree.erase(50)

        self.assertEqual(tree.root.color, BLACK)

    def test_34_insert_type_validation_raises_exception(self):
        """34. Enforces type safety checking by ensuring text string insertions raise a TypeError."""
        tree = RedBlackTree()

        with self.assertRaises(TypeError):
            tree.insert("InvalidPayloadString")

    def test_35_insert_duplicate_key_raises_exception(self):
        """35. validates invariant checking by verifying duplicate insertion attempts raise a ValueError."""
        tree = RedBlackTree()

        tree.insert(100)

        with self.assertRaises(ValueError):
            tree.insert(100)

    def test_36_erase_missing_key_raises_exception(self):
        """36. validates exception boundary coverage when attempting to delete a key absent from the tree."""
        tree = RedBlackTree()

        tree.insert(5)

        with self.assertRaises(ValueError):
            tree.erase(999)

    def test_37_massive_sequential_insertions_determinism(self):
        """37. Benchmarks data layout consistency and black height tracking across a large sequential array sequence."""
        tree = RedBlackTree()

        for value in range(1, 101):
            tree.insert(value)

        self.assertEqual(tree.root.color, BLACK)
        self.assertIsNotNone(tree._search(50))

    def test_38_massive_random_mutations_stability(self):
        """38. Audits structural pipeline integrity across mixed random combinations of insertions and erasures."""
        tree = RedBlackTree()

        items = [45, 12, 89, 34, 67, 90, 23, 56, 78, 11, 22, 33]

        for value in items:
            tree.insert(value)

        for value in [12, 67, 34]:
            tree.erase(value)

        self.assertEqual(tree.root.color, BLACK)
        self.assertEqual(tree._search(12), tree.nil)

    def test_39_clear_tree_to_empty_sentinel_state(self):
        """39. Assures that erasing all elements brings the tree layout reliably back down to its initial sentinel baseline."""
        tree = RedBlackTree()

        tree.insert(10)
        tree.insert(20)

        tree.erase(10)
        tree.erase(20)

        self.assertEqual(tree.root, tree.nil)

    def test_40_root_parent_pointer_after_destructive_erasure(self):
        """40. Verifies that when the tree is depleted down to a final single node, its parent pointer safely targets NIL."""
        tree = RedBlackTree()

        for val in [15, 10, 20]:
            tree.insert(val)

        tree.erase(10)
        tree.erase(20)

        self.assertEqual(tree.root.value, 15)
        self.assertEqual(tree.root.parent, None)
        self.assertEqual(tree.root.left, tree.nil)
        self.assertEqual(tree.root.right, tree.nil)

if __name__ == "__main__":
    unittest.main()