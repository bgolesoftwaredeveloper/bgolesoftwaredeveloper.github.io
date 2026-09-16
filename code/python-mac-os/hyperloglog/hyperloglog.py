"""
hyperloglog.py

A sub-linear memory implementation of the HyperLogLog cardinality estimation algorithm.

Pipeline:
    1. Hash input via truncated SHA-256 to a uniform 64-bit space.
    2. Use the first 'p' bits as a register index to partition elements ($m = 2^p$).
    3. Count the trailing/leading zeros in the remaining bits to get the item's rank.
    4. Update the register if the new rank exceeds its historical high-water mark.
    5. Estimate using the harmonic mean corrected by alpha, linear counting, or saturation rules.
"""
from __future__ import annotations

import hashlib
import math

class HyperLogLog:
    """
    Manages register bucket sizing, stochastic bit-pattern tracking, and mathematical
    bias corrections to maintain HyperLogLog cardinality estimation invariants.
    """

    def __init__(self, error_rate: float = 0.01):
        """
        Instantiates a highly configurable HyperLogLog estimator calculated
        against a specific targeted standard error rate boundary.

        Args:
            error_rate (float): The desired standard error threshold (e.g., 0.01 for 1%).
        """
        if not isinstance(error_rate, (float, int)):
            raise TypeError("Error rate must be a float or integer.")

        if not (0.0001 <= error_rate < 1.0):
            raise ValueError("Error rate must reside within the range [0.0001, 1.0).")

        # Step 1: Compute correct logarithmic precision bit scale.
        precision = math.ceil(math.log2((1.04 / error_rate) ** 2))

        self.precision_boundary = max(4, min(precision, 16))

        # Step 2: Establish the actual total number of physical register buckets.
        self.register_count = 1 << self.precision_boundary

        # Step 3: Instantiate empty register tracking array.
        self.registers = [0] * self.register_count

        self.alpha = self._compute_alpha(self.register_count)

    def _compute_alpha(self, number_of_registers: int) -> float:
        """
        Calculates the theoretical scaling factor constant required to offset
        stochastic multi-hash collisions depending on the register count.

        Args:
            number_of_registers (int): Total number of discrete registers assigned
                                       to the instance.
        Returns:
            float: A specialized bias correction factor.
        """
        if number_of_registers == 16:
            return 0.673
        elif number_of_registers == 32:
            return 0.697
        elif number_of_registers == 64:
            return 0.709
        else:
            return 0.7213 / (1.0 + 1.079 / number_of_registers)

    def _hash(self, element: str) -> int:
        """
        Generates an evenly distributed, deterministic 64-bit unsigned integer
        representation of a string payload using a secure cryptographic hash.

        Args:
            element (str): The target item to process.

        Returns:
            int: A uniform 64-bit unsigned integer value.
        """
        if not isinstance(element, str):
            raise TypeError("Element must be presented explicitly as a string type.")

        hasher = hashlib.sha256(element.encode('utf-8'))
        digest = hasher.hexdigest()

        return int(digest[:16], 16)

    def _retrieve_leading_zeros(self, bits: int, maximum_bits: int) -> int:
        """
        Calculates the count of consecutive zeros preceding the first non-zero
        bit within a designated binary bit-space sequence.

        Args:
            bits (int): The residual integer bit segment containing the sequence.
            maximum_bits (int): Total length of the remaining binary bit string to check.

        Returns:
            int: The calculated count of leading zeros plus one.
        """
        if bits == 0:
            return maximum_bits + 1

        count = 0

        for index in range(maximum_bits - 1, -1, -1):
            if (bits >> index) & 1:
                break

            count += 1

        return count + 1

    def add(self, element: str) -> None:
        """
        Inscribes a new string element into the data structure by mapping it
        to an indexed register and tracking its observed binary signature.

        Args:
            element (str): The target string item to register.
        """
        hash_value = self._hash(element)

        register_index = hash_value >> (64 - self.precision_boundary)
        remainder_bits = hash_value & ((1 << (64 - self.precision_boundary)) - 1)

        rank = self._retrieve_leading_zeros(remainder_bits, 64 - self.precision_boundary)

        if rank > self.registers[register_index]:
            self.registers[register_index] = rank

    def estimate(self) -> int:
        """
        Synthesizes the complete internal register state array via a corrected
        harmonic mean calculation to estimate set cardinality.

        Returns:
            int: The estimated count of unique elements recorded.
        """
        raw_sum = 0.0

        for register_value in self.registers:
            raw_sum += 1.0 / (2.0 ** register_value)

        raw_estimate = self.alpha * (self.register_count ** 2) * (1.0 / raw_sum)

        # Small Range Correction (Linear Counting Regime).
        if raw_estimate <= 2.5 * self.register_count:
            empty_registers = self.registers.count(0)

            if empty_registers > 0:
                return round(self.register_count * math.log(self.register_count / empty_registers))

            return round(raw_estimate)
        elif raw_estimate > (1.0 / 30.0) * (2.0 ** 64):
            overflow_threshold = (2.0 ** 64) - raw_estimate

            if overflow_threshold <= 0:
                raise OverflowError("Stochastic estimate has fully saturated limits.")

            return round(-(2.0 ** 64) * math.log(overflow_threshold / (2.0 ** 64)))

        return round(raw_estimate)

    def display(self) -> None:
        print("=" * 60)
        print("-- HyperLogLog Internal State Summary --")
        print(f"\tConfigured Precision Bits : {self.precision_boundary}")
        print(f"\tTotal Register Buckets   : {self.register_count}")
        print(f"\tAlpha Correction Factor  : {self.alpha:.4f}")
        print("-" * 60)

        preview_limit = min(self.register_count, 16)
        preview_data = [str(value) for value in self.registers[:preview_limit]]
        trailing_dots = "..." if self.register_count > 16 else ""

        print(f"\tRegister State Preview   : [{', '.join(preview_data)}]{trailing_dots}")
        print(f"\tCurrent Unique Estimate  : {self.estimate()}")
        print("=" * 60)