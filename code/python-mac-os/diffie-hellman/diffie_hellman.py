"""
diffie_hellman.py

An implementation of the Diffie-Hellman (DH) key exchange protocol configured
for secure multi-party shared cryptographic secret synthesis.

This module provides an object-oriented framework for executing multi-party key
negotiations over insecure communication channels. It explicitly relies on the
intractability of the discrete logarithm problem inside multiplicative groups of
integers modulo a large prime, integrating cryptographically secure pseudo-random
number generation (CSPRNG) alongside mathematical subgroup confinement mitigations.

Design Note:
    This implementation natively defaults to standard 2048-bit MODP Group 14 parameters
    (RFC 3526) to enforce high-security performance boundaries. It includes active input
    validation on public metrics to suppress small-subgroup confinement vulnerabilities.
"""
from __future__ import annotations

import secrets

class DiffieHellman:
    """
    An engine responsible for managing operational key states, generating secure
    private exponents, and calculating public asymmetric keys and shared cryptographic secrets.
    """

    def __init__(self, prime_modulus: int, generator: int):
        """
        Initializes an independent participant instance anchored to fixed algebraic group parameters.

        Args:
            prime_modulus (int): A large prime number defining the finite field boundary (p).
            generator (int): A primitive root modulo prime_modulus anchoring cyclic operations (g).

        Raises:
            TypeError: If the modulus or generator parameters fail type validation checks.
            ValueError: If the modulus or generator fail foundational cryptographic safety bounds.
        """
        # Enforce strict primitive type assertions across the domain variables.
        if not isinstance(prime_modulus, int):
            raise TypeError("Prime modulus must be an integer")

        if not isinstance(generator, int):
            raise TypeError("Generator must be an integer")

        # Validate lower limits to block degenerate or trivial field configurations.
        if prime_modulus < 2:
            raise ValueError("Prime modulus must be greater than 1.")

        # Enforce generator boundaries relative to the active modular order.
        if not (1 < generator < prime_modulus):
            raise ValueError("Generator must be between 2 and prime_modulus - 1.")

        # The prime order scaling the finite field space.
        self.prime_modulus = prime_modulus

        # The base generator used to evaluate cyclic group trajectories.
        self.generator = generator

        # The secret private exponent scalar; restricted from public visibility.
        self.private_key: int | None = None

        # The asymmetric public coordinate distributed to remote session nodes.
        self.public_key: int | None = None

    @classmethod
    def group14(cls):
        """
        Factory method providing standard RFC 3526 Group 14 (2048-bit MODP Group)
        parameters. Safe for modern cryptographic implementations.

        Returns:
            DiffieHellman: An instantiated engine pre-configured with a 2048-bit safe prime field.
        """
        # Define the standardized 2048-bit prime bitstring literal.
        prime_hex = (
            "FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD1"
            "29024E088A67CC74020BBEA63B139B22514A08798E3404DD"
            "EF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245"
            "E485B576625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7ED"
            "EE386BFB5A899FA5AE9F24117C4B1FE649286651ECE45B3D"
            "C2007CB8A163BF0598DA48361C55D39A69163FA8FD24CF5F"
            "83655D23DCA3AD961C62F356208552BB9ED529077096966D"
            "670C354E4ABC9804F1746C08CA18217C32905E462E36CE3B"
            "E39E772C180E86039B2783A2EC07A28FB5C55DF06F4C52C9"
            "DE2BCBF6955817183995497CEA956AE515D2261898FA0510"
            "15728E5A8AACAA68FFFFFFFFFFFFFFFF"
        )

        # Parse the base-16 string representation into a Python integer.
        return cls(prime_modulus=int(prime_hex, 16), generator=2)

    def generate_private_key(self, minimum_value: int = 2, maximum_value: int | None = None) -> int | None:
        """
        Generates a cryptographically secure random private key and executes an automatic public key update.

        Args:
            minimum_value (int): Lower bound of the private key range (inclusive).
            maximum_value (int, optional): Upper bound of the private key range (inclusive). Defaults to p - 2.

        Returns:
            int: The freshly instantiated private key exponent.

        Raises:
            ValueError: If the requested range properties conflict with field safety rules.
        """
        # Assign the default upper boundary constraint if none was explicitly passed.
        if maximum_value is None:
            maximum_value = self.prime_modulus - 2

        # Enforce boundary properties to guarantee the secret key sits inside valid group order limits.
        if not (2 <= minimum_value <= maximum_value <= self.prime_modulus - 2):
            raise ValueError("Private key range must be within [2, prime_modulus - 2].")

        # Use secrets (CSPRNG) to pick a secure random value within the calculated index span.
        self.private_key = secrets.randbelow(maximum_value - minimum_value + 1) + minimum_value

        # Trigger an automatic update to derive the matching public key coordinate immediately.
        self.compute_public_key()

        return self.private_key

    def compute_public_key(self) -> int | None:
        """
        Derives the public intermediate key coordinate via modular exponentiation.

        Mathematical Formula:
            $$A = g^a \\pmod p$$

        Returns:
            int: The calculated public key coordinate.

        Raises:
            ValueError: If execution is triggered before an underlying private key has been generated.
        """
        # Block computation if the necessary secret private key exponent asset is unpopulated.
        if self.private_key is None:
            raise ValueError("Private key has not been generated yet.")

        # Compute the public asymmetric coordinate using built-in three-argument modular pow().
        self.public_key = pow(self.generator, self.private_key, self.prime_modulus)

        return self.public_key

    def compute_shared_secret(self, public_key: int | None) -> int | None:
        """
        Computes the shared secret value from a foreign public key coordinate.

        Mathematical Formula:
            $$K = B^a \\pmod p$$

        Args:
            other_party_public_key (int): The inbound public vector issued by the remote participant.

        Returns:
            int: The deterministic shared secret key scalar.

        Raises:
            TypeError: If the incoming public key coordinate is not an integer.
            ValueError: If internal dependencies are unpopulated or the foreign key fails validation.
        """
        # Verify localized private state key exists before managing.
        if self.private_key is None:
            raise ValueError("Private key has not been generated yet.")

        # Validate primitive type of the incoming remote public coordinate parameter.
        if not isinstance(public_key, int):
            raise TypeError("Public key must be an integer.")

        # Mitigate small-subgroup attacks by verifying the foreign coordinate avoids identity bounds.
        if not (1 < public_key < self.prime_modulus - 1):
            raise ValueError("Insecure or invalid public key provided by the other party.")

        # Calculate the finalized matching shared secret integer via modular exponentiation.
        return pow(public_key, self.private_key, self.prime_modulus)