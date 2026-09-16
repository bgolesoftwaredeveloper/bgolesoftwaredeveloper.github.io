"""
main.py

An execution script demonstrating the end-to-end pipeline of the RSA cryptographic
algorithm. This module acts as the entry point to orchestrate key generation,
string encryption, and data decryption workflows using custom modular arithmetic functions.
"""
from rivest_shamir_adleman import keypair, encrypt, decrypt

def main():
    # Establish initial distinct prime factors for the simulation.
    prime = 61
    secret = 53

    print("=" * 60)
    print("RSA Asymmetric Cryptography Demonstration")
    print("=" * 60)

    # Derive the cryptographic operational public and private key structural pairs.
    public_key, private_key = keypair(prime, secret)

    print(f"Generated Public Key: {public_key}.")
    print(f"Generated Private Key: {private_key}.")

    # Define the string content context to transform.
    encrypted_payload = "RSA Algorithm 2026"

    print(f"Original text: '{encrypted_payload}'.")

    # Encrypt the raw string using the public key.
    encrpyted_blocks = encrypt(encrypted_payload, public_key)

    print(f"Encrypted Integer Array: {encrpyted_blocks}.")

    # Decrypt the obscured integers back to text using the private key.
    decrypted_payload = decrypt(encrpyted_blocks, private_key)

    print(f"Decrypted Integer Array: {decrypted_payload}.\n")

    if encrypted_payload == decrypted_payload:
        print("Success.")
    else:
        print("Failure.")

if __name__ == "__main__":
    main()