"""
main.py

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
from diffie_hellman import DiffieHellman

def main():
    # Initialize independent cryptographic engine instances for both participants using RCF 3526 Group 14.
    braiden = DiffieHellman.group14()
    brant = DiffieHellman.group14()

    # Generate secure random private exponents and automatically calculate corresponding public keys.
    braiden.generate_private_key()
    brant.generate_private_key()

    # Output the generated public keys (truncated for display purposes due to massive 2048-bit length).
    print(f"Braiden Public Key (truncated): {str(braiden.public_key)[:30]}...")
    print(f"Brant Public Key (truncated):   {str(brant.public_key)[:30]}...\n")

    # Exchange public keys over the unsecure channel and compute the localized shared secret values.
    braiden_derived_secret = braiden.compute_shared_secret(brant.public_key)
    brant_derived_secret = brant.compute_shared_secret(braiden.public_key)

    # Output the calculated shared secrets to inspect mathematical convergence.
    print(f"Braiden Shared Secret (truncated): {str(braiden_derived_secret)[:30]}...")
    print(f"Brant Shared Secret (truncated):   {str(brant_derived_secret)[:30]}...\n")

    # Evaluate if both parties successfully established the exact same symmetric key value.
    are_equivalent = braiden_derived_secret == brant_derived_secret

    # Print the final truth status verifying successful key alignment.
    print(f"Symmetric key synchronicity verification status: {are_equivalent}.")

if __name__ == "__main__":
    main()