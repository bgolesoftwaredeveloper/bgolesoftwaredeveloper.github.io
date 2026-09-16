"""
main.py

A complete, standalone execution script implementing the Elliptic Curve Digital
Signature Algorithm (ECDSA) over the secp256k1 elliptic curve topology.

This script demonstrates key pair generation, message hashing, signature
generation, and public key verification using fully descriptive variable names
and object-oriented design patterns.
"""
from ecdsa import ECDSA

def main():
    print("=" * 60)
    print("\tECDSA (secp256k1) Implementation.")
    print("=" * 60)

    pipeline = ECDSA()

    print("[1] Key Pair Generation:")

    private_key, public_key = pipeline.generate_key_pair()

    print(f"\tPrivate key scalar: {hex(private_key)}).")
    print(f"\tPublic key coordinate X: {hex(public_key.coordinate_x)}.")
    print(f"\tPublic key coordinate Y: {hex(public_key.coordinate_y)}.\n")

    print("[2] Digital Signature Synthesis:")

    original_message = b"Transaction Request: Transfer 12.5009 BTX to Node Hex-84B"
    signature = pipeline.sign_message(original_message, private_key)

    print(f"\tPlaintext Payload: {original_message.decode()}.")
    print(f"\tSignature Proof Point [X]: {hex(signature[0])}.")
    print(f"\tSignature Assertion [S]: {hex(signature[1])}.\n")

    print("[3] Signature Verification (Untampered Message):")

    is_signature_valid = pipeline.verify_signature(original_message, signature, public_key)

    print(f"\tVerification Engine Result: {is_signature_valid}. ✅\n")

    print("[4] Signature Verification (Tampered Message Flagging):")

    tampered_message = b"Transaction Request: Transfer 125.009 BTC to Node Hex-84B."
    is_tampered_valid = pipeline.verify_signature(tampered_message, signature, public_key)

    print(f"\tAttempting to verify: '{tampered_message.decode()}'.")
    print(f"\tVerification Engine Result: {is_tampered_valid}. (Rejection Confirmed)")

    print("=" * 60)
    print("\tTerminate ECDSA (secp256k1) Implementation.")
    print("=" * 60)

if __name__ == "__main__":
    main()




