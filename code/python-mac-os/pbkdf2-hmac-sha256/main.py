"""
main.py

The primary execution entry point and verification sandbox for the custom
PBKDF2-HMAC-SHA256 cryptographic suite.

This script coordinates a live execution demonstration of the underlying from-scratch
cryptographic engine. It takes a simulated user credential and unique salt configuration,
passes them through the manual hashing pipeline, and prints the resulting high-entropy
derived key as a hex-encoded string.

Usage:
    python main.py
"""

# Import the cryptographic components from our custom suite implementation
from pbkdf2_hmac_sha256 import PBKDF2KeyDeriver


def main() -> None:
    """
    Coordinates and executes a baseline functional demonstration of the manual
    PBKDF2-HMAC-SHA256 key derivation workflow.

    This function simulates a standard key stretching scenario:
    1. Defines a low-entropy user secret (passphrase).
    2. Defines a unique cryptographic salt value to protect against rainbow table attacks.
    3. Configures an iteration work factor (computational complexity loop count).
    4. Requests a specific target output length for the resulting key block.
    5. Dispatches these parameters to the manual execution stack and reports the results.
    """
    print("=" * 70)
    print("RUNNING PBKDF2-HMAC-SHA256".center(70))
    print("=" * 70)

    # 1. Initialize input parameters using raw bytes.
    secret = b"SecureMasterPassword123!"
    salt = b"ApplicationSpecificUniqueSalt"
    iterations = 2000
    length = 32

    print(f"[*] Secret: {secret.decode('utf-8')}")
    print(f"[*] Salt: {salt.decode('utf-8')}")
    print(f"[*] Iterations: {iterations:,}")
    print(f"[*] Length: {length} bytes (256 bits)")

    # 2. Compute the derived key using our manual, class-based architecture.
    derived_cryptographic_key = PBKDF2KeyDeriver.derive_key(
        secret,
        salt,
        iterations,
        length
    )

    # 3. Present the diagnostic verification output.
    print("-" * 70)
    print("SUCCESS: Key Generation Complete")
    print(f"Derived Key (Hex): {derived_cryptographic_key.hex()}")
    print(f"Total Byte Length: {len(derived_cryptographic_key)}")
    print("=" * 70)

if __name__ == "__main__":
    main()