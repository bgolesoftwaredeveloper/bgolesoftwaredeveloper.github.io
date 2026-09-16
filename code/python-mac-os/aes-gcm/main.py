"""
main.py

An execution harness designed to validate and test the manual AES-256-GCM
cryptographic engine implementation.

This script verifies standard text mutation pipelines, ensures compatibility with
Associated Data strings (AAD), and tests security boundaries against ciphertext
manipulation attempt scenarios.
"""
import os
from aes_gcm import AESGCM

def main():
    print("=" * 60)
    print("\tINITIALIZING AES-256-GCM PIPELINES")
    print("=" * 60)

    secret_key = os.urandom(32)
    nonce = os.urandom(12)

    plaintext = b"Secure communication verification message: Transmission vector active."
    associated_metadata = b"Protocol: TLSv1.3 | Origin IP: 192.168.1.50"

    print(f"[+] Plaintext String: {plaintext.decode()}")
    print(f"[+] Associated Data: {associated_metadata.decode()}")
    print(f"[+] Secret Key (Hex): {secret_key.hex()[:32]}...")
    print(f"[+] IV Nonce (Hex): {nonce.hex()}\n")

    try:
        cipher = AESGCM(secret_key)
        ciphertext, authentication_tag = cipher.encrypt(nonce, plaintext, associated_metadata)

        print(f"[✔] Ciphertext (Hex):  {ciphertext.hex()[:40]}...")
        print(f"[✔] Authentication Tag:{authentication_tag.hex()}\n")

        # 4. Perform Symmetric Decryption (Happy Path).
        print("--- Executing Inbound Decryption Pipeline ---")

        recovered_plaintext = cipher.decrypt(
            nonce=nonce,
            ciphertext=ciphertext,
            authentication_tag=authentication_tag,
            aad=associated_metadata
        )

        print(f"[✔] Plaintext Restored: {recovered_plaintext.decode()}\n")

        # 5. Adversarial Testing Phase: Alter ciphertext array to test MAC validation.
        print("--- Executing Adversarial Integrity Testing ---")

        tampered_ciphertext = bytearray(ciphertext)

        # Flip a single random tracking bit down in the final byte slice.
        tampered_ciphertext[-1] ^= 0x01
        tampered_ciphertext = bytes(tampered_ciphertext)

        print("[!] Injecting bitwise distortion into active ciphertext stream...")

        # This execution must drop out into the verification block exception pool.
        cipher.decrypt(
            nonce=nonce,
            ciphertext=tampered_ciphertext,
            authentication_tag=authentication_tag,
            aad=associated_metadata
        )
        print("[X] Critical Safety Warning: Processing failed to detect payload tampering.")
    except ValueError as error:
        if "integrity failure" in str(error):
            print(f"[✔] Active Mitigation Success: {error}")
            print("[✔] Payload drop sequence executed. Tampered block was isolated cleanly.")
        else:
            print(f"[X] Unexpected structural engine error dropped: {error}")

    print("=" * 60)
    print("\tCRYPTOGRAPHIC TESTING CONCLUDED")
    print("=" * 60)

if __name__ == "__main__":
    main()