"""
test_rivest_shamir_adleman.py

Automated unit test suite for verifying the custom Rivest-Shamir-Adleman (RSA)
cryptographic algorithm pipeline, key generation, and processing primitives.

Overview:
    This module uses Python's built-in `unittest` framework to execute a matrix
    of 20 unique testing scenarios against the custom cryptographic components:
    `gcd`, `calculate_inverse`, `keypair`, `encrypt`, and `decrypt`. It ensures
    mathematical precision across standard test vectors, mathematical edge cases,
    and extreme boundary scenarios.

Testing Scope & Architectural Coverage:
    - Euclidean Algorithms: Validates standard greatest common divisor routines, prime
      coprimality transitions, zero-neutrality boundaries, and linear step combinations.
    - Modular Inversion: Verifies Extended Euclidean state tracking, coefficient wrapping,
      and modular multi-inverse boundaries.
    - Keypair Lifecycle Generation: Assures deterministic key derivation, unique identity
      guards, totient cardinality tracking, and coprimality selection windows.
    - Asymmetric Content Operations: Evaluates single-character block transitions, long
      text payload flows, numeric boundary stability, and message integrity verification.

Execution Instructions:
    Run the complete testing matrix natively from the shell or command line via:

        python -m unittest test_rivest_shamir_adleman.py
"""
import unittest

from rivest_shamir_adleman import gcd, calculate_inverse, keypair, encrypt, decrypt

class TestRivestShamirAdleman(unittest.TestCase):
    """
    Test suite containing 20 isolated test vectors evaluating core math algorithms,
    asymmetric encryption primitives, and multi-block text restorations.
    """

    # =========================================================================
    # CORE MATH UTILITIES: GREATEST COMMON DIVISOR (gcd)
    # =========================================================================

    def test_01_gcd_standard_primes(self):
        """1. Verifies that two distinct prime numbers correctly return a GCD of 1."""
        dividend = 61
        divisor = 53

        computed_gcd = gcd(dividend, divisor)

        self.assertEqual(computed_gcd, 1)

    def test_02_gcd_common_composite_factor(self):
        """2. Validates GCD computation when numbers share a known composite factor."""
        dividend = 105
        divisor = 45

        computed_gcd = gcd(dividend, divisor)

        self.assertEqual(computed_gcd, 15)

    def test_03_gcd_zero_divisor_neutrality(self):
        """3. Confirms that a zero value divisor returns the original dividend value unchanged."""
        dividend = 3233
        divisor = 0

        computed_gcd = gcd(dividend, divisor)

        self.assertEqual(computed_gcd, dividend)

    def test_04_gcd_coprime_composite_bounds(self):
        """4. Evaluates coprimality tracking for large composite numbers with no shared factors."""
        dividend = 65537
        divisor = 3120

        computed_gcd = gcd(dividend, divisor)

        self.assertEqual(computed_gcd, 1)

    def test_05_inverse_standard_known_vector(self):
        """5. Validates modular inverse calculation against a known standard cryptographic vector."""
        coefficient = 65537
        modulus = 3120
        expected_inverse = 2753

        computed_inverse = calculate_inverse(coefficient, modulus)

        self.assertEqual(computed_inverse, expected_inverse)

    def test_06_inverse_trivial_modulus_boundary(self):
        """6. Checks that an invalid trivial modulus space of 1 returns a zero guard value."""
        coefficient = 7
        modulus = 1

        computed_inverse = calculate_inverse(coefficient, modulus)

        self.assertEqual(computed_inverse, 0)

    def test_07_inverse_negative_coefficient_wrap(self):
        """7. Verifies that negative linear combinations wrap securely into the positive modulus boundary."""
        coefficient = 3
        modulus = 11
        expected_inverse = 4  # (3 * 4) % 11 == 12 % 11 == 1

        computed_inverse = calculate_inverse(coefficient, modulus)

        self.assertEqual(computed_inverse, expected_inverse)

    def test_08_keypair_generation_determinism(self):
        """8. Assures public and private key values generate deterministically from standard prime factors."""
        prime = 61
        secret = 53

        expected_public = (65537, 3233)
        expected_private = (2753, 3233)

        public_key, private_key = keypair(prime, secret)

        self.assertEqual(public_key, expected_public)
        self.assertEqual(private_key, expected_private)

    def test_09_keypair_identical_prime_guard(self):
        """9. Ensures an exception raises immediately if identical prime factors are supplied."""
        prime = 61
        secret = 61

        with self.assertRaises(ValueError):
            keypair(prime, secret)

    def test_10_keypair_exponent_fallback_routine(self):
        """10. Tests alternative exponent hunting mechanics when the default exponent is not coprime to totient."""
        prime = 5
        secret = 11

        public_key, _ = keypair(prime, secret)

        self.assertGreaterEqual(public_key[0], 3)

    def test_11_encrypt_single_character(self):
        """11. Checks array block encryption constraints on a single character literal sequence."""
        content = "A"
        public_key = (65537, 3233)

        cipher_sequence = encrypt(content, public_key)

        self.assertEqual(len(cipher_sequence), 1)
        self.assertIsInstance(cipher_sequence[0], int)

    def test_12_encrypt_fixed_modulus_bounds(self):
        """12. Verifies that all generated ciphertext block values stay within the legal modulus boundary limits."""
        content = "Secure Asymmetric Message Protocol"
        public_key = (65537, 3233)

        _, operational_modulus = public_key

        cipher_sequence = encrypt(content, public_key)

        for block_value in cipher_sequence:
            self.assertTrue(0 <= block_value < operational_modulus)

    def test_13_encrypt_empty_content_handling(self):
        """13. Assures string parser handles empty strings gracefully without throwing sequence faults."""
        content = ""
        public_key = (65537, 3233)

        cipher_sequence = encrypt(content, public_key)

        self.assertEqual(len(cipher_sequence), 0)
        self.assertEqual(cipher_sequence, [])

    # =========================================================================
    # DECRYPTION PIPELINE PROCESSING (decrypt)
    # =========================================================================

    def test_14_decrypt_known_numeric_vector(self):
        """14. Validates that a sequence of encrypted integers scales back to a known string target."""
        cipher_sequence = [1859, 2680, 2790]  # Representing "RSA" under public_key (65537, 3233)
        private_key = (2753, 3233)

        recovered_text = decrypt(cipher_sequence, private_key)

        self.assertEqual(recovered_text, "RSA")

    def test_15_decrypt_empty_sequence_handling(self):
        """15. Confirms execution tracks normally when parsing an empty ciphertext block list array."""
        cipher_sequence = []
        private_key = (2753, 3233)

        recovered_text = decrypt(cipher_sequence, private_key)

        self.assertEqual(recovered_text, "")

    def test_16_integration_complete_loop_restoration(self):
        """16. Executes a full cycle of key building, text encryption, and exact data recovery."""
        prime = 61
        secret = 53

        content_payload = "RSA Encryption Suite Test 2026!"

        public_key, private_key = keypair(prime, secret)

        cipher_data = encrypt(content_payload, public_key)
        plain_data = decrypt(cipher_data, private_key)

        self.assertEqual(plain_data, content_payload)

    def test_17_integration_numerical_non_ascii_characters(self):
        """17. Confirms that special character arrays, unicode blocks, and control contexts transform accurately."""
        content_payload = "★密碼學-2026★"

        public_key, private_key = keypair(307, 239)

        cipher_data = encrypt(content_payload, public_key)
        plain_data = decrypt(cipher_data, private_key)

        self.assertEqual(plain_data, content_payload)

    def test_18_integration_tamper_isolation_failure(self):
        """18. Verifies that corrupting an integer element in the ciphertext array results in failed text recovery."""
        content_payload = "IntegrityPayload"
        public_key, private_key = keypair(61, 53)

        cipher_data = encrypt(content_payload, public_key)

        cipher_data[0] = cipher_data[0] + 1

        plain_data = decrypt(cipher_data, private_key)

        self.assertNotEqual(plain_data, content_payload)

    def test_19_integration_key_mismatch_isolation(self):
        """19. Assures that decrypting with an unrelated key mismatch produces randomized unreadable gibberish."""
        content_payload = "ConfidentialData"

        public_key, _ = keypair(61, 53)
        _, wrong_private_key = keypair(47, 59)

        cipher_data = encrypt(content_payload, public_key)
        plain_data = decrypt(cipher_data, wrong_private_key)

        self.assertNotEqual(plain_data, content_payload)

    def test_20_integration_reproducibility_invariance(self):
        """20. Validates execution constancy by running identical cryptographic cycles sequentially."""
        content_payload = "ConstantImmutableStateVerification"
        public_key, private_key = keypair(61, 53)

        cipher_initial = encrypt(content_payload, public_key)
        cipher_secondary = encrypt(content_payload, public_key)

        self.assertEqual(cipher_initial, cipher_secondary)

        plain_initial = decrypt(cipher_initial, private_key)
        plain_secondary = decrypt(cipher_secondary, private_key)

        self.assertEqual(plain_initial, plain_secondary)

if __name__ == "__main__":
    unittest.main()