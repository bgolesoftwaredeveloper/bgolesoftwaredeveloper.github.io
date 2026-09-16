// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This program demonstrates the usage of the AKS primality test implemented
//	in the askprimality package. The AKS algorithm is a deterministic,
//	polynomial-time primality test that provides a mathematically rigorous
//	method for determining whether a given integer is prime.
//
//	The purpose of this file is educational and illustrative. It shows how to:
//
//	  - Construct large integers using math/big.
//	  - Invoke the AKS primality test.
//	  - Interpret and display results for known prime and composite values.
//
// Notes:
//   - AKS is significantly slower than probabilistic tests such as
//     Miller–Rabin and should not be used for production cryptography.
//   - This demonstration focuses on correctness and clarity.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"
	"math/big"

	askprimality "github.com/bgolesoftwaredeveloper/aks-primality/aksprimality"
)

func main() {
	// -------------------------------------------------------------------------
	// Demonstration Setup
	// -------------------------------------------------------------------------
	//
	// We define a set of known integers, including both primes and composites.
	// These values are intentionally small so that the AKS algorithm completes
	// in a reasonable amount of time while still exercising all major steps.
	//
	// Each value is represented using *big.Int to match the requirements of
	// the AKS implementation.
	//
	// -------------------------------------------------------------------------
	values := []*big.Int{
		big.NewInt(2),
		big.NewInt(3),
		big.NewInt(5),
		big.NewInt(7),
		big.NewInt(11),
		big.NewInt(15),
		big.NewInt(21),
		big.NewInt(27),
		big.NewInt(49),
	}

	// -------------------------------------------------------------------------
	// Execute AKS Primality Test
	// -------------------------------------------------------------------------
	//
	// For each value, we invoke askprimality.Aks and print a human-readable
	// result indicating whether the number is classified as prime or composite.
	//
	// This loop demonstrates how the AKS function can be integrated into
	// application code without exposing any internal algorithmic complexity.
	//
	// -------------------------------------------------------------------------
	for index := 0; index < len(values); index++ {
		current := values[index]

		isPrime := askprimality.Aks(current)

		if isPrime {
			fmt.Printf("AKS result: %s is PRIME\n", current.String())
		} else {
			fmt.Printf("AKS result: %s is COMPOSITE\n", current.String())
		}
	}
}
