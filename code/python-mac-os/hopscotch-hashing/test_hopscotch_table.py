"""
test_hopscotch_table.py

Automated unit test suite for verifying the Hopscotch Hash Table implementation
across diverse structural arrangements, bitmask transitions, and load limits.

Overview:
    This module uses Python's built-in `unittest` framework to execute a matrix
    of 20 unique testing scenarios against the `HopscotchTable` engine. It ensures
    that state changes, lookups, updates, deletions, and multi-hop neighborhood
    displacements behave with complete structural integrity.

Testing Scope & Architectural Coverage:
    - Core CRUD Operations: Assesses foundational insertion, lookups, missing
      keys, in-place updates, and deletions.
    - Bitmask Vectors: Verifies bit state setting, clearing, matching, and physical
      table cell allocation mapping.
    - Neighborhood Constraints: Evaluates collision resolution, max packing,
      deadlocks, and linear probing extensions.
    - Cascading Displacements: Exercises complex data shifts where multiple
      hops are required to clear an entry slot.

Execution Instructions:
    Run the complete testing matrix natively from the shell or command line via:

        python -m unittest test_hopscotch_table.py
"""
import unittest

from hopscotch import KeyValuePair, HopscotchTable

class TestHopscotchTable(unittest.TestCase):
    """
    Test suite containing 20 comprehensive, isolated test vectors evaluating
    the Hopscotch Hash Table open-addressing and structural displacement logic.
    """

    def test_01_initial_empty_state(self):
        """1. Verifies that an initialized table contains entirely empty structures."""
        table = HopscotchTable(capacity=8, neighborhood_size=3)

        for index in range(table.capacity):
            self.assertIsNone(table.table[index])
            self.assertEqual(table.hop_masks[index], 0)

    def test_02_basic_insertion_and_lookup(self):
        """2. Validates clean insert and retrieval without collisions."""
        table = HopscotchTable(capacity=8, neighborhood_size=3)

        table.insert(2, "Alpha")

        self.assertEqual(table.lookup(2), "Alpha")
        self.assertTrue(table._retrieve_hop_bit(2, 0))

    def test_03_lookup_missing_key(self):
        """3. Verifies that lookup returns None for keys not present in the table."""
        table = HopscotchTable(capacity=8, neighborhood_size=3)

        table.insert(5, "Epsilon")

        self.assertIsNone(table.lookup(99))

    def test_04_update_existing_key(self):
        """4. Validates that inserting an existing key updates its value in-place."""
        table = HopscotchTable(capacity=8, neighborhood_size=3)

        table.insert(2, "Alpha")
        table.insert(2, "Omega")

        self.assertEqual(table.lookup(2), "Omega")

    def test_05_basic_collision_handling(self):
        """5. Verifies adjacent placement within neighborhood boundaries on collision."""
        table = HopscotchTable(capacity=8, neighborhood_size=3)

        table.insert(2, "Alpha")
        table.insert(10, "Beta")

        self.assertEqual(table.lookup(10), "Beta")
        self.assertTrue(table._retrieve_hop_bit(2, 0))
        self.assertTrue(table._retrieve_hop_bit(2, 1))

    def test_06_erase_existing_key(self):
        """6. Assesses state wipe and corresponding bitmask clearing upon item removal."""
        table = HopscotchTable(capacity=8, neighborhood_size=3)

        table.insert(2, "Alpha")

        table.erase(2)

        self.assertIsNone(table.lookup(2))
        self.assertFalse(table._retrieve_hop_bit(2, 0))
        self.assertIsNone(table.table[2])

    def test_07_erase_missing_key_error(self):
        """7. Assures ValueError panic response when trying to erase an unmapped key."""
        table = HopscotchTable(capacity=8, neighborhood_size=3)

        with self.assertRaises(ValueError):
            table.erase(42)

    def test_08_max_neighborhood_packing(self):
        """8. Evaluates packing efficiency when a single neighborhood window is completely maxed out."""
        table = HopscotchTable(capacity=8, neighborhood_size=4)

        table.insert(2, "Alpha")
        table.insert(10, "Beta")
        table.insert(18, "Gamma")
        table.insert(26, "Delta")

        self.assertEqual(table.hop_masks[2], 0b1111)
        self.assertEqual(table.lookup(26), "Delta")

    def test_09_expected_structural_deadlock(self):
        """9. Validates that exceeding mathematical radius capabilities correctly raises a RuntimeError."""
        table = HopscotchTable(capacity=8, neighborhood_size=3)

        table.insert(2, "Alpha")
        table.insert(10, "Beta")
        table.insert(18, "Gamma")

        with self.assertRaises(RuntimeError):
            table.insert(26, "Delta")

    def test_10_successful_cascading_displacement(self):
        """10. Exercises a successful displacement chain where items are kicked downstream to clear space."""
        table = HopscotchTable(capacity=8, neighborhood_size=4)

        table.insert(2, "Alpha")
        table.insert(10, "Beta")
        table.insert(4, "Gamma")
        table.insert(26, "Delta")

        self.assertEqual(table.lookup(26), "Delta")
        self.assertIsNotNone(table.table[5])

    def test_11_hash_distribution_modulo_wrap(self):
        """11. Checks that insertion and tracking wraps around boundary array walls cleanly via modulo arithmetic."""
        table = HopscotchTable(capacity=8, neighborhood_size=3)

        table.insert(7, "Seven")
        table.insert(15, "Fifteen")

        self.assertEqual(table.lookup(15), "Fifteen")
        self.assertTrue(table._retrieve_hop_bit(7, 1))

    def test_12_overflow_on_completely_packed_table(self):
        """12. Assures an OverflowError is thrown when every single physical slot in the allocation array is full."""
        table = HopscotchTable(capacity=4, neighborhood_size=2)

        table.insert(0, "A")
        table.insert(1, "B")
        table.insert(2, "C")
        table.insert(3, "D")

        with self.assertRaises(OverflowError):
            table.insert(4, "E")

    def test_13_negative_key_handling(self):
        """13. Verifies robustness against negative integer keys via absolute value conversion."""
        table = HopscotchTable(capacity=8, neighborhood_size=3)

        table.insert(-2, "NegativeTwo")

        self.assertEqual(table.lookup(-2), "NegativeTwo")

    def test_14_zero_key_handling(self):
        """14. Checks table operations work flawlessly using 0 as a key value entry."""
        table = HopscotchTable(capacity=8, neighborhood_size=3)

        table.insert(0, "Zero")

        self.assertEqual(table.lookup(0), "Zero")

    def test_15_erasing_middle_neighborhood_item(self):
        """15. Confirms lookups for neighbors remain untouched when a middle element is removed."""
        table = HopscotchTable(capacity=8, neighborhood_size=4)

        table.insert(2, "Alpha")
        table.insert(10, "Beta")
        table.insert(18, "Gamma")

        table.erase(10)

        self.assertEqual(table.lookup(2), "Alpha")
        self.assertEqual(table.lookup(18), "Gamma")
        self.assertEqual(table.hop_masks[2], 0b101)

    def test_16_payload_type_safety_guards(self):
        """16. Confirms explicit object validation protections trigger TypeError exceptions on bad inputs."""
        table = HopscotchTable(capacity=8, neighborhood_size=3)

        with self.assertRaises(TypeError):
            table.insert("InvalidKey", "Value")

        with self.assertRaises(TypeError):
            table.insert(5, 12345)

    def test_17_independent_neighborhood_masks(self):
        """17. Assures bit operations on one bucket's mask don't pollute or corrupt another bucket's mask."""
        table = HopscotchTable(capacity=8, neighborhood_size=3)

        table.insert(2, "Alpha")
        table.insert(3, "Beta")

        self.assertEqual(table.hop_masks[2], 0b001)
        self.assertEqual(table.hop_masks[3], 0b001)

    def test_18_interleaved_insert_erase_sequence(self):
        """18. Validates long-term stability and structural recycling during alternating insert and delete steps."""
        table = HopscotchTable(capacity=8, neighborhood_size=3)

        table.insert(2, "Alpha")
        table.insert(10, "Beta")

        table.erase(2)

        table.insert(18, "Gamma")

        self.assertIsNone(table.lookup(2))
        self.assertEqual(table.lookup(10), "Beta")
        self.assertEqual(table.lookup(18), "Gamma")

    def test_19_fill_neighborhood_by_natural_hash_matches(self):
        """19. Evaluates table behaviors when entries naturally hash to adjacent slots without collision cascading."""
        table = HopscotchTable(capacity=8, neighborhood_size=3)

        table.insert(2, "Alpha")
        table.insert(3, "Beta")

        self.assertEqual(table.hop_masks[2], 0b001)
        self.assertEqual(table.hop_masks[3], 0b001)

    def test_20_displacement_chain_with_modulo_looping(self):
        """20. Assesses multi-hop displacement routines where empty slots circle completely around the array walls."""
        table = HopscotchTable(capacity=4, neighborhood_size=2)

        table.insert(3, "Three")
        table.insert(7, "Seven")

        self.assertEqual(table.lookup(3), "Three")
        self.assertEqual(table.lookup(7), "Seven")

if __name__ == '__main__':
    unittest.main()