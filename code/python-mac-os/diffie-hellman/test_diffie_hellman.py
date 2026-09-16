"""
test_diffie_hellman.py

Unit testing suite for the Diffie-Hellman key exchange engine implementation.

This module provides exhaustive test coverage validation leveraging Python's
built-in unittest framework. It validates parameter edge cases, state mutations,
cryptographic bounds, end-to-end multi-party agreement simulation, and explicit
security mitigations against public key malicious injection vectors (0, 1, p-1).
"""
import unittest
from diffie_hellman import DiffieHellman

class TestDiffieHellman(unittest.TestCase):
    """
    Comprehensive unit testing suite enforcing structural integrity, mathematical
    correctness, and active security boundary validations for DiffieHellman.
    """

    def test_01_successful_initialization(self):
        """Verify standard instantiation with custom small valid parameters works."""
        diffie_hellman = DiffieHellman(prime_modulus=23, generator=5)

        self.assertEqual(diffie_hellman.prime_modulus, 23)
        self.assertEqual(diffie_hellman.generator, 5)
        self.assertIsNone(diffie_hellman.private_key)
        self.assertIsNone(diffie_hellman.public_key)

    def test_02_group14_factory_method(self):
        """Verify group14 factory properly initializes standard RFC 3526 parameters."""
        diffie_hellman = DiffieHellman.group14()

        self.assertEqual(diffie_hellman.generator, 2)

        # Check that the modulus is a massive 2048-bit integer.
        self.assertGreater(diffie_hellman.prime_modulus, 2 ** 2047)

    def test_03_type_error_on_non_integer_modulus(self):
        """Ensure TypeError is raised if prime_modulus is passed as a string/float."""
        with self.assertRaises(TypeError):
            DiffieHellman(prime_modulus="23", generator=5)

    def test_04_type_error_on_non_integer_generator(self):
        """Ensure TypeError is raised if generator is passed as a string/float."""
        with self.assertRaises(TypeError):
            DiffieHellman(prime_modulus=23, generator=5.0)

    def test_05_value_error_on_invalid_low_modulus(self):
        """Ensure a prime modulus less than 2 drops a ValueError constraint exception."""
        with self.assertRaises(ValueError):
            DiffieHellman(prime_modulus=1, generator=2)

    def test_06_value_error_on_generator_out_of_bounds_low(self):
        """Ensure a generator equal to or less than 1 raises a ValueError."""
        with self.assertRaises(ValueError):
            DiffieHellman(prime_modulus=23, generator=1)

    def test_07_value_error_on_generator_out_of_bounds_high(self):
        """Ensure a generator equal to or greater than prime_modulus raises a ValueError."""
        with self.assertRaises(ValueError):
            DiffieHellman(prime_modulus=23, generator=23)

    def test_08_private_key_generation_mutates_state(self):
        """Verify generating a private key sets internal attributes and updates the public key."""
        diffie_hellman = DiffieHellman.group14()
        returned_private = diffie_hellman.generate_private_key()

        self.assertIsNotNone(diffie_hellman.private_key)
        self.assertIsNotNone(diffie_hellman.public_key)
        self.assertEqual(diffie_hellman.private_key, returned_private)

    def test_09_private_key_within_default_bounds(self):
        """Ensure generated private key adiffie_hellmaneres to default bounds [2, p - 2]."""
        diffie_hellman = DiffieHellman(prime_modulus=23, generator=5)

        diffie_hellman.generate_private_key()

        # Bounds must be 2 <= private_key <= 21.
        self.assertTrue(2 <= diffie_hellman.private_key <= 21)

    def test_10_private_key_custom_range_enforcement(self):
        """Verify custom minimum and maximum private key ranges are strictly respected."""
        diffie_hellman = DiffieHellman.group14()

        # Force a highly restricted range window.
        diffie_hellman.generate_private_key(minimum_value=500, maximum_value=505)

        self.assertTrue(500 <= diffie_hellman.private_key <= 505)

    def test_11_private_key_invalid_custom_range_bounds(self):
        """Ensure illegal custom range profiles throw a ValueError constraint exception."""
        diffie_hellman = DiffieHellman(prime_modulus=23, generator=5)

        with self.assertRaises(ValueError):
            # minimum cannot be larger than maximum.
            diffie_hellman.generate_private_key(minimum_value=15, maximum_value=10)

    def test_12_public_key_mathematically_correct(self):
        """Validate public key output maps accurately against manual small evaluation arithmetic."""
        diffie_hellman = DiffieHellman(prime_modulus=23, generator=5)
        diffie_hellman.generate_private_key(minimum_value=3, maximum_value=3)

        self.assertEqual(diffie_hellman.public_key, 10)

    def test_13_compute_public_key_before_private_fails(self):
        """Ensure running compute_public_key raw before private key initialization triggers an asset error."""
        diffie_hellman = DiffieHellman.group14()

        with self.assertRaises(ValueError):
            diffie_hellman.compute_public_key()

    def test_14_successful_key_exchange_simulation(self):
        """Simulate an end-to-end exchange between Braiden and Brant to confirm shared alignment."""
        braiden = DiffieHellman.group14()
        brant = DiffieHellman.group14()

        braiden.generate_private_key()
        brant.generate_private_key()

        secret_braiden = braiden.compute_shared_secret(brant.public_key)
        secret_brant = brant.compute_shared_secret(braiden.public_key)

        self.assertEqual(secret_braiden, secret_brant)

    def test_15_compute_secret_before_private_fails(self):
        """Ensure attempting to compute a secret without an existing local private key fails."""
        diffie_hellman = DiffieHellman.group14()

        with self.assertRaises(ValueError):
            diffie_hellman.compute_shared_secret(123456789)

    def test_16_compute_secret_type_error_on_public_key(self):
        """Verify string/float formats passed as a foreign public key raise a TypeError."""
        diffie_hellman = DiffieHellman.group14()
        diffie_hellman.generate_private_key()

        with self.assertRaises(TypeError):
            diffie_hellman.compute_shared_secret("invalid_public_key_string")

    def test_17_security_mitigation_reject_public_key_zero(self):
        """Mitigation check: Verify an inbound public key value of 0 is rejected."""
        diffie_hellman = DiffieHellman.group14()
        diffie_hellman.generate_private_key()

        with self.assertRaises(ValueError):
            diffie_hellman.compute_shared_secret(0)

    def test_18_security_mitigation_reject_public_key_one(self):
        """Mitigation check: Verify an inbound public key value of 1 is rejected to stop trivial subgroup collapse."""
        diffie_hellman = DiffieHellman.group14()
        diffie_hellman.generate_private_key()

        with self.assertRaises(ValueError):
            diffie_hellman.compute_shared_secret(1)

    def test_19_security_mitigation_reject_public_key_modulus_minus_one(self):
        """Mitigation check: Verify an inbound public key value equal to p-1 is actively blocked."""
        diffie_hellman = DiffieHellman.group14()
        diffie_hellman.generate_private_key()

        toxic_public_key = diffie_hellman.prime_modulus - 1

        with self.assertRaises(ValueError):
            diffie_hellman.compute_shared_secret(toxic_public_key)

    def test_20_security_mitigation_reject_public_key_out_of_bounds_high(self):
        """Mitigation check: Verify an inbound public key greater than or equal to the modulus is rejected."""
        diffie_hellman = DiffieHellman.group14()
        diffie_hellman.generate_private_key()

        with self.assertRaises(ValueError):
            diffie_hellman.compute_shared_secret(diffie_hellman.prime_modulus + 5)

if __name__ == "__main__":
    unittest.main()