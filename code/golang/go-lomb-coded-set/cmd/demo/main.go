// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file contains a runnable demonstration of the golombcodedset package.
//	The program constructs a Golomb-Coded Set from a small collection of
//	pre-hashed 64-bit values, serializes the set to its binary representation,
//	deserializes it back into memory, and performs membership queries against
//	the reconstructed set.
//
//	The demo exercises the full lifecycle of a Golomb-Coded Set, including
//	construction, encoding, decoding, and lookup, while printing intermediate
//	results to make internal behavior and data flow explicit.
//
//	The demonstration is intended to provide a clear, end-to-end example of
//	correct usage and to serve as a manual verification tool for correctness,
//	defensive decoding, and edge-case handling.
//
// Usage:
//
//	go run ./cmd/demo
//
// Related Package:
//
//	github.com/bgolesoftwaredeveloper/golomb-coded-set/golombcodedset
//
// Notes:
//   - This file is not part of the golombcodedset library API.
//   - Input values are assumed to be uniformly distributed 64-bit hashes.
//   - Output is intentionally verbose to make encoding and lookup behavior clear.
//   - The demo favors clarity and correctness over conciseness or performance.
//
// -----------------------------------------------------------------------------
package main

import (
	"encoding/hex"
	"fmt"
	"log"

	"github.com/bgolesoftwaredeveloper/golombcodedset/golombcodedset"
)

func main() {
	// -------------------------------------------------------------------------
	// Step 1: Choose Golomb–Rice parameter.
	// -------------------------------------------------------------------------
	// Typical values are in the range [10..20] depending on false-positive rate.
	riceParameter := uint8(12)

	// -------------------------------------------------------------------------
	// Step 2: Prepare hashed values.
	// -------------------------------------------------------------------------
	// In real usage, these would already be cryptographic hashes
	// (e.g., SipHash, SHA-256 truncated to uint64, etc.).
	hashedValues := []uint64{
		0x0123456789abcdef,
		0x1111111111111111,
		0x2222222222222222,
		0xdeadbeefdeadbeef,
		0xffffffffffffffff,
	}

	// -------------------------------------------------------------------------
	// Step 3: Build the Golomb-Coded Set.
	// -------------------------------------------------------------------------
	set, err := golombcodedset.BuildGolombCodedSet(riceParameter, hashedValues)

	if err != nil {
		log.Fatalf("build failed: %v", err)
	}

	fmt.Println("golomb coded set built")
	fmt.Printf("\telements: %d\n", len(hashedValues))
	fmt.Printf("\trice parameter: %d\n", riceParameter)
	fmt.Printf("\tencoded size: %d bytes\n", len(set.MarshalBinary()))

	// -------------------------------------------------------------------------
	// Step 4: Serialize the set.
	// -------------------------------------------------------------------------
	serialized := set.MarshalBinary()

	fmt.Println("serialized bytes:", hex.EncodeToString(serialized))

	// -------------------------------------------------------------------------
	// Step 5: Deserialize the set.
	// -------------------------------------------------------------------------
	decodedSet, err := golombcodedset.UnmarshalGolombCodedSet(serialized)

	if err != nil {
		log.Fatalf("unmarshal failed: %v", err)
	}

	fmt.Println("golomb coded set deserialized")

	// -------------------------------------------------------------------------
	// Step 6: Membership queries.
	// -------------------------------------------------------------------------
	tests := []uint64{
		0x0123456789abcdef,
		0xdeadbeefdeadbeef,
		0x0000000000000000,
		0x9999999999999999,
	}

	for _, value := range tests {
		match := decodedSet.Match(value)

		fmt.Printf("match 0x%016x -> %v\n", value, match)
	}

	// -------------------------------------------------------------------------
	// Step 7: Demonstrate empty set behavior.
	// -------------------------------------------------------------------------
	emptySet, err := golombcodedset.BuildGolombCodedSet(riceParameter, nil)

	if err != nil {
		log.Fatalf("empty build failed: %v", err)
	}

	fmt.Println("empty set match ->", emptySet.Match(0x1234))
}
