// -----------------------------------------------------------------------------
// Package: main
// File: aks_primality.go
//
// Description:
//
//	This file provides a reference implementation of the AKS primality test
//	(Agrawal–Kayal–Saxena) in Go using math/big. The AKS test is a
//	deterministic, polynomial-time primality test.
//
//	High-level AKS outline:
//	  1) If n is a perfect power, return composite.
//	  2) Find the smallest r such that ord_r(n) > (log2 n)^2.
//	  3) For a = 2..r, if 1 < gcd(a, n) < n, return composite.
//	  4) If n <= r, return prime.
//	  5) For a = 1..floor(sqrt(phi(r)) * log2 n):
//	       Check (x + a)^n ≡ x^n + a (mod x^r - 1, n).
//	     If any check fails, return composite.
//	  6) Otherwise, return prime.
//
// Notes:
//   - This implementation prioritizes clarity over performance.
//   - For large cryptographic sizes, Miller–Rabin is usually preferred.
//
// -----------------------------------------------------------------------------
package askprimality

import (
	"math/big"
)

// integerSquareRootFloor returns ⌊√value⌋ using binary search.
// This function operates entirely on integers and avoids floating-point
// arithmetic. It is used when computing witness bounds in step (5) of AKS,
// ensuring conservative (never underestimated) limits.
func integerSquareRootFloor(value int) int {
	if value <= 0 {
		return 0
	}

	lower := 1
	upper := value
	result := 0

	// Binary search for the largest integer whose square does not exceed value.
	for lower <= upper {
		middle := lower + (upper-lower)/2

		if middle <= value/middle {
			result = middle

			lower = middle + 1
		} else {
			upper = middle - 1
		}
	}

	return result
}

// integerSquareRootCeil returns ⌈√value⌉.
// This is derived from the floor square root and ensures that witness bounds
// are not rounded downward, preserving correctness.
func integerSquareRootCeil(value int) int {
	floor := integerSquareRootFloor(value)

	if floor*floor == value {
		return floor
	}

	return floor + 1
}

// integerNthRootFloor computes ⌊candidate^(1/exponent)⌋.
// This is used in the perfect power check (AKS step 1). The function performs
// a binary search over integers to find the largest integer r such that
// r^exponent ≤ candidateValue.
func integerNthRootFloor(candidate *big.Int, exponent int) *big.Int {
	if exponent < 2 {
		return new(big.Int).Set(candidate)
	}

	lowerBound := big.NewInt(1)
	upperBound := new(big.Int).Set(candidate)
	middlePoint := new(big.Int)
	powerComparison := new(big.Int)

	for lowerBound.Cmp(upperBound) <= 0 {
		// mid = (lower + upper) / 2
		middlePoint.Add(lowerBound, upperBound).Rsh(middlePoint, 1)

		// Compute mid^exponent
		powerComparison.Exp(middlePoint, big.NewInt(int64(exponent)), nil)

		switch powerComparison.Cmp(candidate) {
		case 0:
			// Exact perfect power detected.
			return middlePoint
		case -1:
			// mid^exponent < candidate
			lowerBound.Add(middlePoint, big.NewInt(1))
		default:
			upperBound.Sub(middlePoint, big.NewInt(1))
		}
	}

	return upperBound
}

// isPerfectPower checks whether candidate can be written as a^k for integers
// a > 1 and k > 1.
// This implements AKS step (1). If n is a perfect power, it is composite
// (except for trivial cases already filtered).
func isPerfectPower(candidate *big.Int) bool {
	// For n > 1, it suffices to check exponents up to floor(log2(n)).
	maximumExponent := candidate.BitLen() - 1

	for exponent := 2; exponent <= maximumExponent; exponent++ {
		root := integerNthRootFloor(candidate, exponent)

		// Verify whether root^exponent == candidate.
		powerVerification := new(big.Int).Exp(root, big.NewInt(int64(exponent)), nil)

		if powerVerification.Cmp(candidate) == 0 {
			return true
		}
	}

	return false
}

// orderExceedsThreshold returns true if ord_r(n) > threshold.
// This is implemented by checking whether any k in [1..threshold] satisfies n^k ≡ 1 (mod r).
// If none do, then by definition the multiplicative order exceeds threshold.
func orderExceedsThreshold(candidate *big.Int, modulus int, threshold int) bool {
	modulusBig := big.NewInt(int64(modulus))

	// Reduce n modulo r.
	reducedCandidate := new(big.Int).Mod(candidate, modulusBig)

	// If n ≡ 0 or 1 (mod r), the multiplicative order is trivial.
	if reducedCandidate.Sign() == 0 || reducedCandidate.Cmp(big.NewInt(1)) == 0 {
		return false
	}

	currentPower := big.NewInt(1)

	// Iteratively compute n^k mod r for k = 1..threshold.
	for exponent := 1; exponent <= threshold; exponent++ {
		currentPower.Mul(currentPower, reducedCandidate)
		currentPower.Mod(currentPower, modulusBig)

		// If n^k ≡ 1 (mod r), order ≤ threshold.
		if currentPower.Cmp(big.NewInt(1)) == 0 {
			return false
		}
	}

	// No exponent ≤ threshold satisfied the congruence.
	return true
}

// minimumModulus finds the smallest integer r satisfying ord_r(n) > (log2 n)^2.
// This implements AKS step (2). The search bound r ≤ O((log n)^5) is derived
// from the AKS proof. An additional constant factor is used conservatively.
func minimumModulus(candidate *big.Int, orderThreshold int) int {
	log2Candidate := candidate.BitLen() - 1

	if log2Candidate < 1 {
		log2Candidate = 1
	}

	// Compute (log2 n)^5 as an integer.
	maximum := 1

	for index := 0; index < 5; index++ {
		maximum *= log2Candidate
	}

	maximum *= 10

	for modulus := 2; modulus <= maximum; modulus++ {
		if new(big.Int).GCD(nil, nil, candidate, big.NewInt(int64(modulus))).Cmp(big.NewInt(1)) != 0 {
			continue
		}

		if orderExceedsThreshold(candidate, modulus, orderThreshold) {
			return modulus
		}
	}

	panic("aks step (2) failed no suitable modulus 'r' found within theoretical search boundary")
}

// hasNonTrivial checks for a non-trivial common divisor between candidate and
// any integer in the range [2, modulus].
// This implements AKS step (3): if there exists an integer a such that
// 1 < gcd(a, n) < n, then n is composite.
func hasNonTrivial(candidate *big.Int, modulus int) bool {
	greatestCommonDivisor := new(big.Int)

	// Iterate over all potential small divisors up to r.
	for potentialDivisor := 2; potentialDivisor <= modulus; potentialDivisor++ {
		// Compute gcd(candidate, potentialDivisor).
		greatestCommonDivisor.GCD(nil, nil, candidate, big.NewInt(int64(potentialDivisor)))

		// If gcd is strictly between 1 and n, a non-trivial factor exists.
		if greatestCommonDivisor.Cmp(big.NewInt(1)) > 0 && greatestCommonDivisor.Cmp(candidate) < 0 {
			return true
		}
	}

	return false
}

// computeEulerTotient computes Euler's totient function φ(modulus).
// This counts the number of integers in [1, modulus] that are coprime to modulus.
// The value is used in AKS step (5) to determine the number of witnesses
// required for polynomial congruence testing.
func computeEulerTotient(modulus int) int {
	if modulus <= 0 {
		return 0
	}

	remaining := modulus
	totient := modulus

	// Perform prime factorization of modulus.
	for factor := 2; factor*factor <= remaining; factor++ {
		if remaining%factor == 0 {
			// Remove all occurrences of this prime factor.
			for remaining%factor == 0 {
				remaining /= factor
			}

			// Apply Euler totient update rule.
			totient -= totient / factor
		}
	}

	// If remaining > 1, it is a prime factor larger than sqrt(modulus).
	if remaining > 1 {
		totient -= totient / remaining
	}

	return totient
}

// multiplyPolynomialCyclicModulo multiplies two polynomials in the ring
// (ℤ / coefficientModulus ℤ)[x] / (x^modulus − 1).
// This corresponds to polynomial multiplication with coefficients reduced
// modulo candidate and degrees reduced modulo x^r − 1.
func multiplyPolynomialCyclicModulo(left, right []*big.Int, modulus int, coefficientModulus *big.Int) []*big.Int {
	// Initialize the result polynomial with zero coefficients.
	product := make([]*big.Int, modulus)

	for coefficientIndex := 0; coefficientIndex < modulus; coefficientIndex++ {
		product[coefficientIndex] = big.NewInt(0)
	}

	temporary := new(big.Int)

	// Perform convolution-style multiplication with cyclic degree reduction.
	for leftIndex := 0; leftIndex < modulus; leftIndex++ {
		if left[leftIndex].Sign() == 0 {
			continue
		}

		for rightIndex := 0; rightIndex < modulus; rightIndex++ {
			if right[rightIndex].Sign() == 0 {
				continue
			}

			// Degree reduction modulo x^r − 1.
			targetIndex := (leftIndex + rightIndex) % modulus

			// Multiply coefficients and reduce modulo n.
			temporary.Mul(left[leftIndex], right[rightIndex])
			temporary.Mod(temporary, coefficientModulus)

			product[targetIndex].Add(product[targetIndex], temporary)
			product[targetIndex].Mod(product[targetIndex], coefficientModulus)
		}
	}

	return product
}

// computeExpectedPolynomial constructs the polynomial x^n + a
// in the ring (ℤ / nℤ)[x] / (x^r − 1).
// This represents the right-hand side of the AKS polynomial congruence.
func computeExpectedPolynomial(candidate *big.Int, modulus int, witness int) []*big.Int {
	coefficients := make([]*big.Int, modulus)

	// Initialize all coefficients to zero.
	for index := 0; index < modulus; index++ {
		coefficients[index] = big.NewInt(0)
	}

	// Place coefficient 1 at position n mod r for x^n.
	indexForXPower := int(new(big.Int).Mod(candidate, big.NewInt(int64(modulus))).Int64())
	coefficients[indexForXPower].SetInt64(1)

	// Add constant term a.
	coefficients[0].Add(coefficients[0], big.NewInt(int64(witness)))
	coefficients[0].Mod(coefficients[0], candidate)

	return coefficients
}

// computePolynomialPowerModulo computes (x + a)^n modulo (x^r − 1, n).
// This is the left-hand side of the AKS polynomial congruence.
// Exponentiation is performed via binary exponentiation over polynomials.
func computePolynomialPowerModulo(candidate *big.Int, modulus int, witness int) []*big.Int {
	if modulus <= 1 {
		panic("invalid modulus for polynomial ring")
	}

	// Initialize base polynomial x + a.
	basePolynomial := make([]*big.Int, modulus)

	for coefficientIndex := 0; coefficientIndex < modulus; coefficientIndex++ {
		basePolynomial[coefficientIndex] = big.NewInt(0)
	}

	basePolynomial[0].SetInt64(int64(witness))
	basePolynomial[1%modulus].SetInt64(1)
	basePolynomial[0].Mod(basePolynomial[0], candidate)

	// Initialize result polynomial to 1.
	polynomial := make([]*big.Int, modulus)

	for coefficientIndex := 0; coefficientIndex < modulus; coefficientIndex++ {
		polynomial[coefficientIndex] = big.NewInt(0)
	}

	polynomial[0].SetInt64(1)

	// Perform binary exponentiation.
	exponentRemaining := new(big.Int).Set(candidate)

	for exponentRemaining.Sign() > 0 {
		if exponentRemaining.Bit(0) == 1 {
			polynomial = multiplyPolynomialCyclicModulo(polynomial, basePolynomial, modulus, candidate)
		}

		exponentRemaining.Rsh(exponentRemaining, 1)

		if exponentRemaining.Sign() > 0 {
			basePolynomial = multiplyPolynomialCyclicModulo(basePolynomial, basePolynomial, modulus, candidate)
		}
	}

	return polynomial
}

// polynomialCongruenceCheck verifies the AKS polynomial congruence
// (x + a)^n ≡ x^n + a (mod x^r − 1, n).
// This implements the core correctness test in AKS step (5).
func polynomialCongruenceCheck(candidate *big.Int, modulus int, witness int) bool {
	// Compute left-hand side: (x + a)^n mod (x^r − 1, n).
	leftPolynomial := computePolynomialPowerModulo(candidate, modulus, witness)

	// Compute right-hand side: x^n + a mod (x^r − 1, n).
	rightPolynomial := computeExpectedPolynomial(candidate, modulus, witness)

	// Compare coefficients term-by-term.
	for coefficientIndex := 0; coefficientIndex < modulus; coefficientIndex++ {
		if leftPolynomial[coefficientIndex].Cmp(rightPolynomial[coefficientIndex]) != 0 {
			return false
		}
	}

	return true
}

// Aks performs the AKS primality test on candidate.
// It returns true if candidate is prime and false otherwise.
// This function follows the AKS algorithm exactly as specified in the
// Agrawal–Kayal–Saxena paper, with conservative integer bounds.
func Aks(candidate *big.Int) bool {
	// Reject nil input.
	if candidate == nil {
		return false
	}

	// Reject n < 2 explicitly.
	if candidate.Cmp(big.NewInt(2)) < 0 {
		return false
	}

	// Handle small primes directly.
	if candidate.Cmp(big.NewInt(2)) == 0 || candidate.Cmp(big.NewInt(3)) == 0 {
		return true
	}

	// Eliminate even numbers greater than 2.
	if candidate.Bit(0) == 0 {
		return false
	}

	// AKS Step (1): perfect power test.
	if isPerfectPower(candidate) {
		return false
	}

	log2Candidate := candidate.BitLen() - 1
	orderThreshold := log2Candidate * log2Candidate

	// AKS Step (2): find suitable r.
	modulus := minimumModulus(candidate, orderThreshold)

	// AKS Step (3): gcd checks.
	if hasNonTrivial(candidate, modulus) {
		return false
	}

	// AKS Step (4): if n ≤ r, n is prime.
	if candidate.Cmp(big.NewInt(int64(modulus))) <= 0 {
		return true
	}

	// AKS Step (5): polynomial congruence checks.
	eulerTotient := computeEulerTotient(modulus)

	squareRootCeil := integerSquareRootCeil(eulerTotient)
	maximumWitness := squareRootCeil * log2Candidate

	for witness := 1; witness <= maximumWitness; witness++ {
		if !polynomialCongruenceCheck(candidate, modulus, witness) {
			return false
		}
	}

	// AKS Step (6): prime.
	return true
}
