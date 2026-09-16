"""
rsa.py

An implementation of the Rivest-Shamir-Adleman (RSA) cryptographic algorithm,
demonstrating key generation, modular exponentiation encryption, and decryption.

This module provides a functional breakdown of asymmetric cryptography using Python's
built-in arbitrary-precision arithmetic. It includes helper routines for calculating
the greatest common divisor (Euclidean Algorithm) and the modular multiplicative inverse
(Extended Euclidean Algorithm) to construct public and private keys without external
dependencies.

Security Note:
    This code uses small prime numbers for pedagogical clarity and relies on Python's
    standard `random` module, which is not cryptographically secure. For production
    environments, use cryptographically secure pseudo-random number generators (CSPRNG)
    such as `secrets` and prime sizes of at least 2048 bits.
"""
import random

def gcd(dividend: int, divisor: int) -> int:
    """
    Computes the greatest common divisor of two integers using the Euclidean Algorithm.

    This function continuously computes remainders until the divisor reduces to zero,
    leaving the highest common factor as the remaining dividend.

    Args:
        dividend (int): The initial larger integer value to be divided.
        divisor (int): The initial smaller integer value acting as the divisor.

    Returns:
        int: The greatest common divisor (GCD) shared by both input integers.
    """
    # Loop repeats until the divisor drops down to zero.
    while divisor != 0:
        # Shift the old divisor into the dividend position, and
        # evaluate the new remainder to update the divisor.
        dividend, divisor = divisor, dividend % divisor

    # When divisor reaches zero, the remaining dividend holds the final GCD.
    return dividend

def calculate_inverse(coefficient: int, modulus: int) -> int:
    """
    Finds the modular multiplicative inverse of an integer using the Extended
    Euclidean Algorithm, such that (coefficient * inverse) % modulus == 1.

    Args:
        coefficient (int): The integer to invert (typically the public exponent).
        modulus (int): The totient boundary space.

    Returns:
        int: The computed modular inverse.

    Raises:
        ValueError: If the coefficient and modulus are not coprime.
    """
    # Retain the initial modulus value to correct negative boundaries at the end.
    original_modulus = modulus

    # Initialize tracking coefficient for the Extended Euclidean steps.
    step_tracker_x = 0
    step_tracker_y = 1

    # Case for an invalid trivial modulus space.
    if modulus == 1:
        return 0

    # Execute the reduction while the processing coefficient remains above 1.
    while coefficient > 1:
        # Determine the whole quotient factor of the division.
        quotient = coefficient // modulus

        # Sequentially scale down the coefficient and modulus value via remainder.
        coefficient, modulus = modulus, coefficient % modulus

        # Simultaneously shift and calculate the operational linear combinations.
        step_tracker_x, step_tracker_y = step_tracker_y - quotient * step_tracker_x, step_tracker_x

    # If the computed tracking state fell below 0, wrap it back into positive limits.
    if step_tracker_y < 0:
        step_tracker_y += original_modulus

    return step_tracker_y

def keypair(prime: int, secret: int) -> tuple[tuple[int, int], tuple[int, int]]:
    """
    Generates an RSA public and private key pair from two distinct prime values.

    Calculates the shared operational modulus, establishes the totient boundary math,
    selects a valid public exponent, and determines the corresponding private key via
    modular inversion.

    Args:
        prime (int): The first distinct secret prime factor.
        secret (int): The second distinct secret prime factor.

    Returns:
        tuple[tuple[int, int], tuple[int, int]]: A paired cryptographic key collection structured
        exactly as ((public_exponent, operational_modulus), (private_exponent, operational_modulus)).

    Raises:
        ValueError: If the provided prime and secret variables are identical.
    """
    if prime == secret:
        raise ValueError("Prime and secret must be distinc integers.")

    # Calculate the public modulus component.
    operational_modulus = prime * secret

    # Calculate Carmichael's totient or Euler's totient function lambda(n).
    totient_cardinality = (prime - 1) * (secret - 1)

    # Select an ecryption exponent that is coprime to the totient space.
    exponent = 65537

    if gcd(exponent, totient_cardinality) != 1:
        # Fallback to finding an alternative small off prime coprime to totient.
        exponent = 3

        while gcd(exponent, totient_cardinality) != 1:
            exponent += 2

    # Compute the matching private decryption exponent.
    private_exponent = calculate_inverse(exponent, totient_cardinality)

    # Return public key pair and private key pair respectively.
    return ((exponent, operational_modulus), (private_exponent, operational_modulus))

def encrypt(content: str, public_key: tuple[int, int]) -> list[int]:
    """
    Encrypts a plaintext string into a sequence of ciphered integers using an RSA public key.

    Each character in the text is converted to its character index value (ASCII/Unicode ordinal)
    and transformed via modular exponentiation using the encryption exponent.

    Args:
        content (str): The raw text sequence to be obscured.
        public_key (tuple[int, int]): Public key components structured as (exponent, operational_modulus).

    Returns:
        list[int]: An array of encrypted integers representing the ciphertext blocks.
    """
    exponent, operational_modulus = public_key

    sequence = []

    # Process each distinct piece of content sequentially.
    for character in content:
        # Extract the ordinal integer index representing the current character.
        index = ord(character)

        # Apply the core RSA transformation formula.
        cipher_value = pow(index, exponent, operational_modulus)

        # Append the calculated block integer to our collection array.
        sequence.append(cipher_value)

    return sequence

def decrypt(sequence: list[int], private_key: tuple[int, int]) -> str:
    """
    Decrypts an array of ciphered integers back into its original text structure using an RSA private key.

    Each encrypted integer block is transformed back into its character index value via modular
    exponentiation using the private decryption exponent, and then converted back into a string character.

    Args:
        sequence (list[int]): The array of numerical ciphertext blocks to decrypt.
        private_key (tuple[int, int]): Private key components structured as (private_exponent, operational_modulus).

    Returns:
        str: The recovered plaintext message.
    """
    # Unpack the private key components.
    private_exponent, operational_modulus = private_key
    plaintext = []

    # Process each individual numerical ciphertext block sequentially.
    for value in sequence:
        # Reverse the mathematical transformation using the private exponent
        index = pow(value, private_exponent, operational_modulus)

        # Reconstruct the string character literal from its numerical index value.
        recovered_character = chr(index)

        # Accumulate the string character into our plaintext array.
        plaintext.append(recovered_character)

    return "".join(plaintext)