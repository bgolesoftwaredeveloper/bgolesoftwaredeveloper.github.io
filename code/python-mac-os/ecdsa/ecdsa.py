"""
ecdsa.py

An implementation of the Elliptic Curve Digital Signature Algorithm (ECDSA)
configured for standard cryptographic signing and verification pipelines.

This module provides non-parametric asymmetric cryptographic workflows for key
generation, message hashing, signature synthesis, and verification topologies. It
explicitly combines elliptic curve scalar mathematics over prime fields with
one-way hashing primitives to achieve deterministic authentication criteria.

Design Note:
    This implementation is optimized for educational evaluation and structural reference.
    It natively handles large-integer math using pure Python primitives and lacks
    built-in side-channel mitigation techniques (such as constant-time execution)
    necessary for production-grade security architectures.
"""
import hashlib
import secrets

class EllipticCurve:
    """
    Represents an Elliptic Curve over a prime field (F_p).
    Equation: y^2 = x^3 + a*x + b (mod prime_modulus)
    """

    def __init__(self, prime_modulus: int, coefficient_linear: int, coefficient_constant: int):
        """
        Initializes an independent elliptic curve configuration.

        Args:
            prime_modulus (int): The large prime modulus defining the finite field boundaries.
            coefficient_linear (int): The 'a' parameter multiplying the linear term.
            coefficient_constant (int): The 'b' parameter defining the constant term.
        """
        # The prime number defining the finite boundaries and coordinate ceiling.
        self.prime_modulus = prime_modulus

        # The linear coefficient (a) affecting the slope properties of the curve.
        self.coefficient_linear = coefficient_linear

        # The constant coefficient (b) setting the structural baseline of the curve.
        self.coefficient_constant = coefficient_constant

class Point:
    """
    Represents a specific coordinate pair (x, y) resting on an active elliptic curve,
    or an abstract representation of the identity element (the Point at Infinity).
    """

    def __init__(self, curve: EllipticCurve, x: int = None, y: int = None):
        """
        Initializes an independent coordinate point or an identity state at infinity.

        Args:
            curve (EllipticCurve): The mathematical curve domain this point belongs to.
            x (int, optional): The horizontal field coordinate. None implies infinity.
            y (int, optional): The vertical field coordinate. None implies infinity.
        """
        # The underlying mathematical curve schema containing this coordinate.
        self.curve = curve

        # The numerical horizontal coordinate within the finite field.
        self.x = x

        # The numerical vertical coordinate within the finite field.
        self.y = y

        # A boolean flag indicating if this point acts as the identity element.
        self.is_infinity = x is None or y is None

    @property
    def coordinate_x(self):
        """Exposes the strict integer horizontal coordinate, raising an exception if it is undefined."""
        if self.is_infinity or self.x is None:
            raise ValueError("The point at Infinity does not posses a finite X-coordinate.")

        return self.x

    @property
    def coordinate_y(self):
        """Exposes the strict integer vertical coordinate, raising an exception if it is undefined."""
        if self.is_infinity or self.y is None:
            raise ValueError("The point at Infinity does not posses a finite Y-coordinate.")

        return self.y

    def __eq__(self, other: Point) -> bool:
        """Evaluates structural equivalence between two separate point nodes."""
        if self.is_infinity and other.is_infinity:
            return True
        if self.is_infinity != other.is_infinity:
            return False

        return (self.x == other.x and self.y == other.y)

    def __add__(self, other: Point) -> Point:
        """
        Executes point addition over a finite field to synthesize a combined coordinate point.

        Mathematical Formula:
            $$\\lambda = \\frac{y_2 - y_1}{x_2 - x_1} \\pmod p \\quad \\text{or} \\quad \\lambda = \\frac{3x_1^2 + a}{2y_1} \\pmod p$$
            $$x_3 = \\lambda^2 - x_1 - x_2 \\pmod p, \\quad y_3 = \\lambda(x_1 - x_3) - y_1 \\pmod p$$

        Args:
            other (Point): The secondary point configuration being added to the current point.

        Returns:
            Point: A newly instantiated point representing the chord-tangent field sum.
        """
        # Adding the identity element returns the other point unchanged.
        if self.is_infinity:
            return other

        if other.is_infinity:
            return self

        assert self.x is not None and self.y is not None
        assert other.x is not None and other.y is not None

        prime = self.curve.prime_modulus

        # Adding a point to its vertical reflection results in the Point at Infinity.
        if self.x == other.x and (self.y + other.y) % prime == 0:
            return Point(self.curve)

        # Points are distinct (Standard Chord-Intercept Addition).
        if self != other:
            numerator = (other.y - self.y) % prime
            denominator = (other.x - self.x) % prime
            slope = (numerator * pow(denominator, prime - 2, prime))
        else:
            # Points are identical (Point Doubling / Tangent Addition).
            numerator = (3 * pow(self.coordinate_x, 2, prime) + self.curve.coefficient_linear) % prime
            denominator = (2 * self.y) % prime
            slope = (numerator * pow(denominator, prime - 2, prime))

        new_x = (pow(slope, 2, prime) - self.x - other.x) % prime
        new_y = (slope * (self.x - new_x) - self.y) % prime

        return Point(self.curve, new_x, new_y)

    def __rmul__(self, scalar: int) -> Point:
        """
        Multiplies an existing point by an integer scalar via the Double-and-Add heuristic.

        Args:
            scalar (int): The integer multiplier scaling the target curve coordinate.

        Returns:
            Point: The terminal point node produced by cyclic addition loops.
        """
        result = Point(self.curve)
        addend = self

        bits = scalar

        while bits > 0:
            if bits & 1:
                result = result + addend

            addend = addend + addend
            bits >>= 1

        return result

class ECDSA:
    """
    An engine responsible for managing cryptographic keys, generating verifiable message
    signatures, and executing mathematical verification tests over a defined curve.
    """

    def __init__(self):
        """
        Initializes the cryptographic pipeline using standard secp256k1 base constraints.
        """
        prime_modulus = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
        coefficient_linear = 0
        coefficient_constant = 7

        self.curve = EllipticCurve(prime_modulus, coefficient_linear, coefficient_constant)

        generator_x = 0x79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798
        generator_y = 0x483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8

        self.generator_point = Point(self.curve, generator_x, generator_y)

        self.order = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141

    def _hash_message(self, message: bytes) -> int:
        """
        Transforms raw byte input vectors into uniform scalar integers using a SHA-256 digest.

        Args:
            message (bytes): The raw sequence of data bytes to evaluate.

        Returns:
            int: A large integer representation derived directly from the computed hash.
        """
        message_hash_bytes = hashlib.sha256(message).digest()

        return int.from_bytes(message_hash_bytes, byteorder='big')

    def generate_key_pair(self) -> tuple[int, Point]:
        """
        Generates an independent private integer scalar and its corresponding public point.

        Returns:
            tuple[int, Point]: A pair containing the private key and public key point.
        """
        private_key = secrets.randbelow(self.order - 1) + 1
        public_key = private_key * self.generator_point

        return private_key, public_key

    def sign_message(self, message: bytes, private_key: int) -> tuple[int, int]:
        """
        Computes a non-invertible digital signature (r, s) utilizing the private key state.

        Optimization Objective:
            $$r = (k \\cdot G)_x \\pmod n$$
            $$s = k^{-1} \\cdot (\\text{hash} + r \\cdot \\text{private\\_key}) \\pmod n$$

        Args:
            message (bytes): The raw data matrix passing through the signature pipeline.
            private_key (int): The secret key integer controlling identity verification.

        Returns:
            tuple[int, int]: The cryptographic signature components (r_value, s_value).
        """
        message_digest = self._hash_message(message)

        signature_proof_assertion = 0
        signature_witness = 0

        while signature_proof_assertion == 0 or signature_witness == 0:
            nonce = secrets.randbelow(self.order - 1) + 1
            ephemeral_point = nonce * self.generator_point
            signature_proof_assertion = ephemeral_point.x % self.order

            if signature_proof_assertion == 0:
                continue

            nonce_inverse = pow(nonce, self.order - 2, self.order)
            signature_witness = ((nonce_inverse * (message_digest + signature_proof_assertion * private_key))
                                 % self.order)

        return signature_proof_assertion, signature_witness

    def verify_signature(self, message: bytes, signature: tuple[int, int], public_key: Point) -> bool:
        """
        Evaluates an external signature pair against a known public key validation path.

        Verification Criteria:
            $$u_1 = \\text{hash} \\cdot s^{-1} \\pmod n, \\quad u_2 = r \\cdot s^{-1} \\pmod n$$
            $$\\text{Verify that: } (u_1 \\cdot G + u_2 \\cdot Q)_x \\equiv r \\pmod n$$

        Args:
            message (bytes): The data vector claiming validation authority.
            signature (tuple[int, int]): A cluster comprising the (r, s) values under review.
            public_key (Point): The unencrypted validation target point.

        Returns:
            bool: True if structural verification metrics match; False if the payload was altered.
        """

        signature_proof_assertion, signature_witness = signature

        if not (0 < signature_proof_assertion < self.order) or not (0 < signature_witness < self.order):
            return False

        message_digest = self._hash_message(message)
        witness_inverse = pow(signature_witness, self.order - 2, self.order)

        generator_scalar_multiplier = (message_digest * witness_inverse) % self.order
        public_key_scalar_multiplier = (signature_proof_assertion * witness_inverse) % self.order

        point = (generator_scalar_multiplier * self.generator_point) + (public_key_scalar_multiplier * public_key)

        if point.is_infinity:
            return False

        return (point.x % self.order) == signature_proof_assertion
