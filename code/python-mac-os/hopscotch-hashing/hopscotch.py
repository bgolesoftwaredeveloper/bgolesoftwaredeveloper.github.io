"""
hopscotch_hashing.py

A complete, self-contained implementation of a Hopscotch Hash Table built
using fundamental primitive types and fixed-size arrays in Python.

This module provides an optimized open-addressing dictionary alternative that
guarantees strict localized neighborhoods for bucket collisions. It maintains
a structural bitmap for every bucket slot, ensuring that any item hashing to a
given bucket is guaranteed to reside within a small, predefined neighborhood
window neighborhood capacity. This bounds worst-case lookup performance to O(neighborhood capacity).

Design Note:
    This implementation prioritizes physical array slot swaps and strict bitwise
    coordination over abstract chaining. It manages collisions dynamically by
    cascading empty slots backward via an iterative displacement chain, matching
    cache-friendly spatial requirements and clean null-safety boundaries.
"""
from __future__ import annotations

class KeyValuePair:
    """
    Represents an isolated key-value payload container bound to a physical
    slot within the linear hash table array layout.
    """

    def __init__(self, key: int, value: str):
        """
        Initializes an entry instance pairing an explicit integer lookup key
        with its mapped string payload data.

        Args:
            key (int): The unique identification integer key.
            value (str): The primary payload data associated with this key.
        """
        # Validate that incoming types match structure invariants.
        if not isinstance(key, int):
            raise TypeError("Key must be an integer")

        if not isinstance(value, str):
            raise TypeError("Value must be an string")

        # Map fields to internal properties.
        self.key = key
        self.value = value

class HopscotchTable:
    """
    Manages array bucket sizing, key displacement cascading, and bitwise structural
    neighborhood tracking to maintain Hopscotch Hashing invariants across elements.
    """

    def __init__(self, capacity: int = 16, neighborhood_size: int = 4):
        """
        Instantiates a fixed-capacity Hopscotch Hash Table configured with
        localized neighborhood window boundaries.

        Args:
            capacity (int): The baseline total size of the internal table array.
            neighborhood_size (int): The maximum scanning radius allowed for
                                     displaced items.
        """
        self.capacity = capacity
        self.neighborhood_radius = neighborhood_size

        # Pre-allocate underlying parallel array infrastructure.
        self.table: list[KeyValuePair | None] = [None] * self.capacity
        self.hop_masks = [0] * self.capacity

    def _hash(self, key: int) -> int:
        """
        Computes the primary entry index within the table allocation array bounds.

        Args:
            key (int): The integer target key to hash.

        Returns:
            int: A clean array index mapping inside the active physical footprint.
        """
        # Absorb negative bounds and map to internal index capacity space.
        return abs(key) % self.capacity

    def _retrieve_hop_bit(self, index: int, offset: int) -> bool:
        """
        Queries the localized tracking bitmask to verify if an item belonging
        to index exists at the specified offset position.

        Args:
            index (int): The original absolute anchor bucket index.
            offset (int): The relative displacement offset to query.

        Returns:
            bool: True if the bitwise flag at that offset position is active.
        """
        # Shift bit flag left by offset and cross-reference via bitwise AND.
        return bool(self.hop_masks[index] & (1 << offset))

    def _set_hop_bit(self, index: int, offset: int) -> None:
        """
        Enables the bitwise tracking flag indicating that a key belonging to
        index has been safely positioned at the offset relative location.

        Args:
            index (int): The absolute anchor bucket index being modified.
            offset (int): The relative displacement offset position to flag.
        """
        # Apply bitwise OR operation to store the relative offset location flag.
        self.hop_masks[index] |= (1 << offset)

    def _clear_hop_bit(self, index: int, offset: int) -> None:
        """
        Disables the bitwise tracking flag indicating that a key belonging to
        index has been removed or displaced from the offset position.

        Args:
            index (int): The absolute anchor bucket index being modified.
            offset (int): The relative displacement offset position to clear.
        """
        # Flip target mask bit using bitwise NOT, then apply bitwise AND.
        self.hop_masks[index] &= ~(1 << offset)

    def insert(self, key: int, value: str) -> None:
        """
        Inserts a unique key-value pair into the table, executing cascading slot
        displacements if the primary neighborhood window is full.

        Args:
            key (int): The primitive integer key to register.
            value (str): The accompanying string metadata payload.
        """
        # Step 1: Compute home base location index.
        hash_index = self._hash(key)

        # Step 2: Search active neighborhood window for matching extant keys.
        for offset in range(self.neighborhood_radius):
            if self._retrieve_hop_bit(hash_index, offset):
                check_index = (hash_index + offset) % self.capacity
                entry = self.table[check_index]

                # If found, update payload in-place and terminate.
                if entry and entry.key == key:
                    entry.value = value

                    return

        # Step 3: Track down globally nearest open slot using linear probing.
        empty_index = -1

        for index in range(self.capacity):
            scan_index = (hash_index + index) % self.capacity

            if self.table[scan_index] is None:
                empty_index = scan_index

                break

        # Panic if table is thoroughly packed out.
        if empty_index == -1:
            raise OverflowError("Hash table is completely full.")

        cycle = True

        # Step 4: Cascade displacements if open spot sits outside home window radius.
        while cycle:
            # Measure loop distance between home anchor and free index slot.
            distance = (empty_index - hash_index) % self.capacity

            # Break loop when open slot falls inside the neighborhood radius limit.
            if distance < self.neighborhood_radius:
                break

            swapped = False

            # Scan backward sequentially from the empty position slot.
            for index in range(self.neighborhood_radius - 1, 0, -1):
                check_index = (empty_index - index) % self.capacity

                # Look for an alternate anchor that owns the node at check index.
                for anchor_index in range(self.neighborhood_radius - 1, -1, -1):
                    possible_anchor = (check_index - anchor_index) % self.capacity

                    # Check if alternative anchor tracks ownership at check index.
                    if self._retrieve_hop_bit(possible_anchor, anchor_index):
                        # Measure if shift keeps target item within its own home radius.
                        new_distance = (empty_index - possible_anchor) % self.capacity

                        if new_distance < self.neighborhood_radius:
                            # Transfer content downstream into current free slot.
                            self.table[empty_index] = self.table[check_index]

                            # Calculate true position offset to clear tracking flag.
                            old_distnace = (check_index - possible_anchor) % self.capacity

                            self._clear_hop_bit(possible_anchor, old_distnace)

                            # Activate new tracking flag at updated slot offset.
                            self._set_hop_bit(possible_anchor, new_distance)

                            # Evacuate original slot, moving free space target upstream.
                            self.table[check_index] = None

                            empty_index = check_index

                            swapped = True

                            break

                # Break out of loop layer if swap chain registers successfully.
                if swapped:
                    break

            # If all backward attempts fail to relocate empty space, we are stuck.
            if not swapped:
                raise RuntimeError("Hopscotch neighborhood deadlock encountered.")

        # Step 5: Finalize record insertion sequence inside optimized window position.
        final_offset = (empty_index - hash_index) % self.capacity

        self.table[empty_index] = KeyValuePair(key, value)

        self._set_hop_bit(hash_index, final_offset)

    def lookup(self, key: int) -> str | None:
        """
        Traverses a strict localized neighborhood window to retrieve the string
        payload bound to an integer key.

        Args:
            key (int): The target lookup key to isolate.

        Returns:
            str | None: The matching string payload, or None if missing.
        """
        # Resolve target anchor search position index.
        hash_index = self._hash(key)

        # Iterate strictly over localized neighborhood footprint offsets.
        for offset in range(self.neighborhood_radius):
            # Parse index location exclusively if tracking bit flag is turned on.
            if self._retrieve_hop_bit(hash_index, offset):
                check_index = (hash_index + offset) % self.capacity
                entry = self.table[check_index]

                # Match found: Return bounded payload string.
                if entry and entry.key == key:
                    return entry.value

        return None

    def erase(self, key: int) -> None:
        """
        Removes a key-value record from the table layout and cleans up its
        associated bitmask tracking flags.

        Args:
            key (int): The primitive integer key target to clear.
        """
        # Resolve target anchor deletion position index.
        hash_index = self._hash(key)

        # Scan neighborhood frame layout using bitmask cues.
        for offset in range(self.neighborhood_radius):
            if self._retrieve_hop_bit(hash_index, offset):
                check_index = (hash_index + offset) % self.capacity
                entry = self.table[check_index]

                # If key matches, scrub data and lower the tracking flag.
                if entry and entry.key == key:
                    self.table[check_index] = None
                    self._clear_hop_bit(hash_index, offset)

                    return

        # Throw exception if key is missing from table architecture completely.
        raise ValueError(f"Erase error: key '{key}' does not exist.")

    def display(self) -> None:
        """
        Renders a structured layout visualization of the table array nodes
        and parallel bit vectors using formatted table borders.
        """
        print("=" * 60)
        print("-- Hopscotch Hash Table --")
        print(f"\t{'Index':<7} | {'Hop Mask':<10}| {'Key:Value Payload':<1}")

        # Render array map nodes line-by-line sequentially.
        for index in range(self.capacity):
            # Convert integer masks into readable padded binary segments.
            mask_bin = f"{self.hop_masks[index]:08b}"[-self.neighborhood_radius:]
            entry = self.table[index]
            payload = f"[{entry.key}:{entry.value}]" if entry else "NIL"

            # Print current data row layout.
            print(f"\t[{index:^3}]\t| {mask_bin:^10}|\t{payload}")

        print("=" * 60)