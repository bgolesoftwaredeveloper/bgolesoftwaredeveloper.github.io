"""
test_aes_gcm.py

A comprehensive unit test suite containing 20 unique functional, boundary,
and adversarial security verification scenarios for the custom AESGCM class.
"""
import unittest
from aes_gcm import AESGCM

class TestAESGCM(unittest.TestCase):
    def setUp(self):
        """Set up standard keys, nonces, and inputs used across tests."""
        self.valid_key = b"\x01" * 32
        self.valid_nonce = b"\x02" * 12
        self.standard_plaintext = b"Testing cryptographic engine boundaries."
        self.standard_aad = b"Version: 1.0.0 | Context: Unit Testing"
        self.cipher = AESGCM(self.valid_key)

    def test_01_initialization_valid_key(self):
        """Ensure class instantiates cleanly with a proper 32-byte key."""
        try:
            engine = AESGCM(self.valid_key)
            
            self.assertIsInstance(engine, AESGCM)
        except Exception as exception:
            self.fail(f"Initialization failed with valid key: {exception}")

    def test_02_initialization_invalid_key_length_short(self):
        """Ensure constructor rejects a key that is too short (e.g., 16 bytes)."""
        short_key = b"\x01" * 16

        with self.assertRaises(ValueError):
            AESGCM(short_key)

    def test_03_initialization_invalid_key_length_long(self):
        """Ensure constructor rejects a key that is too long (e.g., 64 bytes)."""
        long_key = b"\x01" * 64

        with self.assertRaises(ValueError):
            AESGCM(long_key)

    def test_04_key_expansion_structure(self):
        """Verify key schedule derives exactly 15 flat round keys, each 16 bytes long."""
        self.assertEqual(len(self.cipher.round_keys), 15)

        for r_key in self.cipher.round_keys:
            self.assertEqual(len(r_key), 16)

    def test_05_galois_double_basic(self):
        """Test internal GF(2^8) byte doubling without conditional reduction."""
        # 0x03 << 1 = 0x06
        self.assertEqual(self.cipher._galois_double(0x03), 0x06)

    def test_06_galois_double_with_reduction(self):
        """Test internal GF(2^8) byte doubling with irreducible polynomial reduction active."""
        # 0x80 has MSB set -> (0x80 << 1) ^ 0x1B = 0x1B (after masking to 0xFF)
        self.assertEqual(self.cipher._galois_double(0x80), 0x1B)

    def test_07_gf_multiply_identity(self):
        """Verify finite field multiplication against a zero element yields zero block."""
        zero_block = b"\x00" * 16
        any_block = b"\x42" * 16

        result = self.cipher._gf_multiply(any_block, zero_block)

        self.assertEqual(result, zero_block)

    def test_08_encrypt_block_preserves_length(self):
        """Verify raw single-block transformation takes 16 bytes and yields exactly 16 bytes."""
        block = b"\x00" * 16

        encrypted = self.cipher.encrypt_block(block)

        self.assertEqual(len(encrypted), 16)
        self.assertNotEqual(encrypted, block)

    def test_09_encrypt_invalid_nonce_length(self):
        """Ensure encryption rejects non-standard nonce configurations (e.g., 16 bytes)."""
        bad_nonce = b"\x02" * 16

        with self.assertRaises(ValueError):
            self.cipher.encrypt(bad_nonce, self.standard_plaintext)

    def test_10_decrypt_invalid_nonce_length(self):
        """Ensure decryption rejects non-standard nonce configurations outright."""
        bad_nonce = b"\x02" * 8

        with self.assertRaises(ValueError):
            self.cipher.decrypt(bad_nonce, b"raw_ct", b"raw_tag")

    def test_11_empty_plaintext_loop(self):
        """Verify engine safely handles and cycles a completely empty plaintext payload."""
        ciphertext, tag = self.cipher.encrypt(self.valid_nonce, b"")
        
        self.assertEqual(len(ciphertext), 0)
        self.assertEqual(len(tag), 16)

        plaintext = self.cipher.decrypt(self.valid_nonce, ciphertext, tag)

        self.assertEqual(plaintext, b"")

    def test_12_empty_aad_handling(self):
        """Verify successful round-trip encryption when no Associated Data (AAD) is supplied."""
        ciphertext, tag = self.cipher.encrypt(self.valid_nonce, self.standard_plaintext, aad=b"")
        plaintext = self.cipher.decrypt(self.valid_nonce, ciphertext, tag, aad=b"")

        self.assertEqual(plaintext, self.standard_plaintext)

    def test_13_exact_block_size_alignment(self):
        """Verify code processes plaintexts that align perfectly with the 16-byte block boundary."""
        exact_plaintext = b"A" * 16

        ciphertext, tag = self.cipher.encrypt(self.valid_nonce, exact_plaintext)

        self.assertEqual(len(ciphertext), 16)

        plaintext = self.cipher.decrypt(self.valid_nonce, ciphertext, tag)

        self.assertEqual(plaintext, exact_plaintext)

    def test_14_partial_block_stride_alignment(self):
        """Verify code processes plaintexts that do not align with 16-byte boundaries."""
        ragged_plaintext = b"A" * 25

        ciphertext, tag = self.cipher.encrypt(self.valid_nonce, ragged_plaintext)

        self.assertEqual(len(ciphertext), 25)

        plaintext = self.cipher.decrypt(self.valid_nonce, ciphertext, tag)

        self.assertEqual(plaintext, ragged_plaintext)

    def test_15_massive_payload_strides(self):
        """Verify pipeline execution over larger continuous data tracking streams."""
        large_plaintext = b"DataStreamTesting" * 200

        ciphertext, tag = self.cipher.encrypt(self.valid_nonce, large_plaintext, self.standard_aad)
        plaintext = self.cipher.decrypt(self.valid_nonce, ciphertext, tag, self.standard_aad)

        self.assertEqual(plaintext, large_plaintext)

    def test_16_ciphertext_tampering_mitigation(self):
        """Verify that altering a single bit in the ciphertext causes a decryption failure."""
        ciphertext, tag = self.cipher.encrypt(self.valid_nonce, self.standard_plaintext, self.standard_aad)

        tampered_ct = bytearray(ciphertext)
        tampered_ct[0] ^= 0x01

        with self.assertRaises(ValueError):
            self.cipher.decrypt(self.valid_nonce, bytes(tampered_ct), tag, self.standard_aad)

    def test_17_tag_tampering_mitigation(self):
        """Verify that altering the authentication tag causes instant payload rejection."""
        ciphertext, tag = self.cipher.encrypt(self.valid_nonce, self.standard_plaintext, self.standard_aad)

        tampered_tag = bytearray(tag)
        tampered_tag[-1] ^= 0x01

        with self.assertRaises(ValueError):
            self.cipher.decrypt(self.valid_nonce, ciphertext, bytes(tampered_tag), self.standard_aad)

    def test_18_aad_tampering_mitigation(self):
        """Verify that manipulating the unencrypted Associated Data context causes a MAC failure."""
        ciphertext, tag = self.cipher.encrypt(self.valid_nonce, self.standard_plaintext, self.standard_aad)

        tampered_aad = self.standard_aad + b" "  # Modify contextual metadata

        with self.assertRaises(ValueError):
            self.cipher.decrypt(self.valid_nonce, ciphertext, tag, tampered_aad)

    def test_19_nonce_mismatch_mitigation(self):
        """Verify that decrypting with a different nonce drops processing pipelines."""
        ciphertext, tag = self.cipher.encrypt(self.valid_nonce, self.standard_plaintext, self.standard_aad)

        different_nonce = b"\x03" * 12

        with self.assertRaises(ValueError):
            self.cipher.decrypt(different_nonce, ciphertext, tag, self.standard_aad)

    def test_20_key_mismatch_mitigation(self):
        """Verify that decrypting with an incorrect key matrix drops processing pipelines."""
        ciphertext, tag = self.cipher.encrypt(self.valid_nonce, self.standard_plaintext, self.standard_aad)

        wrong_key = b"\x99" * 32
        wrong_cipher = AESGCM(wrong_key)

        with self.assertRaises(ValueError):
            wrong_cipher.decrypt(self.valid_nonce, ciphertext, tag, self.standard_aad)

if __name__ == "__main__":
    unittest.main()