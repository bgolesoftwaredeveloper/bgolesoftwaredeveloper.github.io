"""
aes_gcm.py

An implementation of the Advanced Encryption Standard (AES) operating in
Galois/Counter Mode (GCM) with a 256-bit key length configuration.

This module provides authenticated encryption and decryption workflows conforming
to the NIST SP 800-38D architectural specifications. It explicitly combines standard
block transformations with Galois field multiplications over GF(2^128) to ensure both
confidentiality and payload data integrity.

Security Note:
    This implementation is designed for educational evaluation and reference.
    It does not incorporate explicit protections against hardware side-channel
    or execution timing analysis.
"""

class AESGCM:
    """
    An engine responsible for managing structural states, round keys, and executing
    authenticated data mutations through AES-256-GCM pipelines.
    """

    # AES Rijndael Substitution Box (S-box) used for non-linear byte substitution.
    SBOX = [
        0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
        0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
        0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
        0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
        0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
        0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
        0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
        0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
        0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
        0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
        0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
        0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
        0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
        0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
        0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
        0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
    ]

    # Round constant matrix used during key expansion schedules.
    RCON = [0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36]

    def __init__(self, key: bytes):
        """
        Initializes the cipher engine by processing raw keys and building authentication tags.

        Args:
            key (bytes): The raw, unhashed 32-byte (256-bit) secret key matrix.

        Raises:
            ValueError: If the byte string length configuration does not equal exactly 32.
        """
        if len(key) != 32:
            raise ValueError("This implementation requires a 256- bit (32-byte) key.")

        # Expand the raw 32-byte key into the 15 necessary internal round keys.
        self.round_keys = self._expand_key(key)

        # Calculate the authentication GHASH subkey 'H' by encrypting an all-zero block.
        self.hash_key = self.encrypt_block(b"\x00" * 16)

    def _expand_key(self, key: bytes) -> list[list[int]]:
        """
        Derives the complete set of structural round keys using Rijndael schedule algorithms.

        Args:
            key (bytes): The original 32-byte base secret input.

        Returns:
            list[list[int]]: A collection containing the 15 flattened 16-byte round keys.
        """
        words = []

        # Split the initial 32-byte key into 8 starting words (4 bytes each).
        for index in range(8):
            words.append(list(key[index * 4 : (index + 1) * 4]))

        # Generate words dynamically until we hit the 60 words required for AES-256 (15 rounds * 4 words).
        while len(words) < 60:
            temp = list(words[-1])
            index = len(words)

            if index % 8 == 0:
                # Rotate the 4-byte word left by 1 byte position.
                temp = temp[1:] + temp[:1]

                # Pass bytes through the SBOX.
                temp = [self.SBOX[box] for box in temp]

                # XOR the leftmost byte with the scheduled round constant.
                temp[0] ^= self.RCON[index // 8]
            elif index % 8 == 4:
                # For AES-256, an extra substitution step is required on index stride multiplies of 4.
                temp = [self.SBOX[box] for box in temp]

            # XOR the temporary word block with the word located 8 positions back.
            word_xor = [previous ^ temp for previous, temp in zip(words[-8], temp)]
            words.append(word_xor)

        round_keys = []

        # Chunk the 60 generated words back into 15 independent 16-byte round keys.
        for word_index in range(0, 60, 4):
            flat_round_key = []

            for word in words[word_index : word_index + 4]:
                flat_round_key.extend(word)

            round_keys.append(flat_round_key)

        return round_keys

    def _galois_double(self, byte_value: int) -> int:
        """
        Multiplies an individual byte by x (0x02) inside the Galois Field GF(2^8).

        Args:
            byte_value (int): An element scalar value between 0 and 255.

        Returns:
            int: The modulated resulting field primitive byte value.
        """
        # If the highest but is set, shift left and perform a modulo reduction against 0x1B.
        if byte_value & 0x80:
            return ((byte_value << 1) ^ 0x1B) & 0xFF

        # If the highest bit is not set, a simple bitwise left-shift is sufficient.
        return (byte_value << 1) & 0xFF

    def _gf_multiply(self, factor_x: bytes, factor_y: bytes) -> bytes:
        """
        Executes a carryless field multiplication over the polynomial grid space GF(2^128).

        Args:
            factor_x (bytes): The left-hand 16-byte polynomial string.
            factor_y (bytes): The right-hand 16-byte polynomial string.

        Returns:
            bytes: A newly synthesized 16-byte block product.
        """
        # Convert the 16-byte arrays into massive 128-bit unsigned integer registers.
        x = int.from_bytes(factor_x, byteorder='big')
        y = int.from_bytes(factor_y, byteorder='big')

        product_accumulator = 0
        shifting_operand = y

        # Standard GCM irreducible reduction polynomial shifted for right-to-left evaluation format.
        reduction_polynomial = 0xE1000000000000000000000000000000

        # Standard binary long multiplication loop evaluated inside a finite field.
        for bit_position in range(128):
            # If the current bit of x is active, add (XOR) the shifted operand to teh accumulator.
            if x & (1 << (127 - bit_position)):
                product_accumulator ^= shifting_operand

            # Reduce the operand value if it overflows the 128-bit boundary limits.
            if shifting_operand & 1:
                shifting_operand = (shifting_operand >> 1) ^ reduction_polynomial
            else:
                shifting_operand >>= 1

        return product_accumulator.to_bytes(16, byteorder='big')

    def _ghash(self, key: bytes, aad: bytes, ciphertext: bytes) -> bytes:
        """
        Generates the standard structural GHASH authenticity tag footprint over inputs.

        Args:
            key (bytes): The 16-byte internal subkey state H.
            aad (bytes): Additional Authenticated Data string parameters.
            ciphertext (bytes): The encrypted ciphertext byte array stream.

        Returns:
            bytes: A 16-byte computed GHASH block mapping output.
        """
        # Calculate padding targets to force 16-byte block alignments.
        pad_aad_length = (16 - len(aad) % 16) % 16
        pad_ciphertext_length = (16 - len(ciphertext) % 16) % 16

        # Assemble the continuous authentication stream string data structure.
        padded_stream = aad + (b"\x00" * pad_aad_length) + ciphertext + (b"\x00" * pad_ciphertext_length)

        # Append the explicit bit-lengths of both parts as trailing 64-bit big-endian integers.
        padded_stream += ((len(aad) * 8).to_bytes(8, byteorder='big') + (len(ciphertext) * 8)
            .to_bytes(8, byteorder='big'))

        hash_buffer = b'\x00' * 16

        # Iterate over the padding stream in 16-bbyte steps, multiplying blocks into the running field states.
        for block_offset in range(0, len(padded_stream), 16):
            current_block = padded_stream[block_offset: block_offset + 16]

            # Accumulate block states via XOR mix.
            xor_mix = bytes(hash_byte ^ block_byte for hash_byte, block_byte in zip(hash_buffer, current_block))

            # Perfect finite field multiplication against the authentication subkey 'H.'
            hash_buffer = self._gf_multiply(xor_mix, key)

        return hash_buffer

    def encrypt_block(self, block: bytes) -> bytes:
        """
        Executes raw single-block (16-byte) encryption via the 14 structural rounds of AES-256.

        Args:
            block (bytes): A single standalone 16-byte block structure.

        Returns:
            bytes: The resulting 16-byte encrypted block.
        """
        state = list(block)

        # Initial round: Mix the original data block state with the primary base key.
        state = [state_byte ^ key_byte for state_byte, key_byte in zip(state, self.round_keys[0])]

        # Main encryption rounds: Loop rounds 1 through 13.
        for round_index in range(1, 14):
            # Substitute every individual byte through the static SBOX.
            state = [self.SBOX[byte] for byte in state]

            # Permute byte spatial tracking layouts row-by-row.
            state = [
                state[0], state[5], state[10], state[15],
                state[4], state[9], state[14], state[3],
                state[8], state[13], state[2], state[7],
                state[12], state[1], state[6], state[11]
            ]

            # Transform column structures using matrix field multiplications.
            for column_offset in range(0, 16, 4):
                pivot_top = state[column_offset]
                anchor_middle = state[column_offset + 1]
                bridge_low = state[column_offset + 2]
                bound_base = state[column_offset + 3]

                state[column_offset] = self._galois_double(pivot_top) ^ (
                    self._galois_double(anchor_middle) ^ anchor_middle) ^ bridge_low ^ bound_base

                state[column_offset + 1] = pivot_top ^ self._galois_double(anchor_middle) ^ (
                    self._galois_double(bridge_low) ^ bridge_low) ^ bound_base

                state[column_offset + 2] = pivot_top ^ anchor_middle ^ self._galois_double(bridge_low) ^ (
                    self._galois_double(bound_base) ^ bound_base)

                # XOR the transformed column blocks with the current active round key.
                state[column_offset + 3] = ((self._galois_double(pivot_top) ^ pivot_top) ^
                    anchor_middle ^ bridge_low ^ self._galois_double(bound_base))

            state = [state_byte ^ key_byte for state_byte, key_byte in zip(state, self.round_keys[round_index])]

        # Final round (round 14): Omit the mix columns transformation step.
        state = [self.SBOX[byte] for byte in state]
        state = [
            state[0], state[5], state[10], state[15],
            state[4], state[9], state[14], state[3],
            state[8], state[13], state[2], state[7],
            state[12], state[1], state[6], state[11]
        ]

        state = [state_byte ^ key_byte for state_byte, key_byte in zip(state, self.round_keys[14])]

        return bytes(state)

    def encrypt(self, nonce: bytes, plaintext: bytes, aad: bytes = b""):
        """
        Encrypts a plaintext stream payload using counter loops and constructs a MAC tag.

        Args:
            nonce (bytes): A deterministic unique initialization string. Must be exactly 12 bytes.
            plaintext (bytes): The raw data byte sequence target requiring encryption.
            aad (bytes, optional): Auxiliary unencrypted contextual data parameters. Defaults to b"".

        Returns:
            tuple[bytes, bytes]: A pairing containing (ciphertext, authentication_tag).

        Raises:
            ValueError: If the nonce initialization buffer length does not equal exactly 12.
        """
        if len(nonce) != 12:
            raise ValueError("NIST recommended default 96-bit nonce execution enforced.")

        # Initialize the counter block (Y_0): Append a 4-byte big-endian starting integer value of 1.
        counter = nonce + b"\x00\x00\x00\x01"

        ciphertext = bytearray()
        accumulator = int.from_bytes(counter, byteorder='big')

        # Core CTR mode: Encryption loop.
        for data_offset in range(0, len(plaintext), 16):
            # Increment the counter value for each sequential text data block.
            accumulator += 1

            # Generate key stream bytes by passing the current counter through the AES engine.
            keystream_block = self.encrypt_block(accumulator.to_bytes(16, byteorder='big'))
            plaintext_chunk = plaintext[data_offset : data_offset + 16]

            # XOR the plaintext data block against the generated key stream block bytes.
            ciphertext.extend(bytes(plaintext_byte ^ key_byte
                for plaintext_byte, key_byte in zip(plaintext_chunk, keystream_block)))

        ciphertext_bytes = bytes(ciphertext)

        # Calculate the authentication block footprint using the collected ciphertext blocks and AAd data.
        hash_value = self._ghash(self.hash_key, aad, ciphertext_bytes)

        # Encrypt the original base counter initialization state (Y_0) to create a tag mask string.
        tag_mask = self.encrypt_block(counter)
        authentication_tag = bytes(hash_byte ^ mask_byte for hash_byte, mask_byte in zip(hash_value, tag_mask))

        return ciphertext_bytes, authentication_tag

    def decrypt(self, nonce: bytes, ciphertext: bytes, authentication_tag: bytes, aad: bytes = b""):
        """
        Validates payload structure tags and decrypts ciphertext blocks back to plaintext.

        Args:
            nonce (bytes): The exact 12-byte initialization string utilized during encryption.
            ciphertext (bytes): The protected ciphertext data stream array.
            authentication_tag (bytes): The 16-byte MAC verification tag signature.
            aad (bytes, optional): Auxiliary unencrypted metadata blocks. Defaults to b"".

        Returns:
            bytes: The verified, decrypted raw plaintext data string.

        Raises:
            ValueError: If the nonce size is invalid or tag parsing fails validation tests.
        """
        if len(nonce) != 12:
            raise ValueError("NIST recommended default 96-bit nonce execution enforced.")

        # Reconstruct teh starting counter (Y_0) using the incoming nonce string.
        counter = nonce + b"\x00\x00\x00\x01"

        # Independently calculate what the expected authentication tag should be over the data parameters.
        hash_value = self._ghash(self.hash_key, aad, ciphertext)
        tag_mask = self.encrypt_block(counter)
        expected_tag = bytes(hash_byte ^ mask_byte for hash_byte, mask_byte in zip(hash_value, tag_mask))

        # Integrity check: Reject processing outright if tags don't match exactly (Prevents oracle leakage).
        if expected_tag != authentication_tag:
            raise ValueError("Integrity failure: Tampered payload of invalid keys detected.")

        plaintext = bytearray()
        accumulator = int.from_bytes(counter, byteorder='big')

        # Core CTR mode decryption loop.
        for data_offset in range(0, len(ciphertext), 16):
            # Increment the counter value exactly matching the encryption loop cycle patterns.
            accumulator += 1

            # Generate the matching key stream block bytes.
            keystream_block = self.encrypt_block(accumulator.to_bytes(16, byteorder='big'))
            ciphertext_chunk = ciphertext[data_offset : data_offset + 16]

            # XOR the ciphertext block against the key stream block bytes to recover plaintext.
            plaintext.extend(bytes(ciphertext_byte ^ key_byte
                for ciphertext_byte, key_byte in zip(ciphertext_chunk, keystream_block)))

        return bytes(plaintext)