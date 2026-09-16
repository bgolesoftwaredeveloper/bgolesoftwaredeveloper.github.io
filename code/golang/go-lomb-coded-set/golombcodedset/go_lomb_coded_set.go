// -----------------------------------------------------------------------------
// Package: golombcodedset
// File: golomb_coded_set.go
//
// Description:
//
//	This file defines the core Golomb-Coded Set (GCS) data structure and its
//	public API. It is responsible for set construction, serialization,
//	deserialization, and membership queries over a static collection of
//	uniformly distributed 64-bit hash values.
//
//	Specifically, this file provides:
//	  - The GolombCodedSet type and its invariants
//	  - Binary marshaling and unmarshaling logic
//	  - Set construction via delta encoding and Golomb–Rice coding
//	  - Probabilistic membership testing logic
//
//	Low-level implementation details such as bit-level encoding/decoding and
//	arithmetic utilities are delegated to companion files within the same
//	package to maintain clear separation of responsibilities.
//
//	The Golomb-Coded Set supports false positives but guarantees no false
//	negatives, assuming uniformly distributed input hashes.
//
// Notes:
//   - Input values must already be uniformly distributed 64-bit hashes.
//   - This implementation favors correctness, clarity, and defensive decoding
//     over micro-optimizations.
//   - Malformed or truncated encodings are handled safely and cause lookups
//     to fail closed.
//
// -----------------------------------------------------------------------------
package golombcodedset

import (
	"encoding/binary"
	"errors"
	"sort"
)

// GolombCodedSet represents a space-efficient, probabilistic set supporting
// membership queries over a static collection of hashed values.
// The set is parameterized by a Rice parameter and stores delta-encoded,
// Golomb–Rice encoded values in a compact bitstream.
type GolombCodedSet struct {
	riceParameter uint8
	elementCount  uint32
	encodedBits   []byte
}

// MarshalBinary serializes the Golomb-Coded Set into a binary representation
// consisting of a header and the encoded bitstream.
func (set *GolombCodedSet) MarshalBinary() []byte {
	// Allocate space for the header and encoded payload.
	output := make([]byte, 0, 5+len(set.encodedBits))

	// Write the Rice parameter as the first byte.
	output = append(output, set.riceParameter)

	// Encode the element count using little-endian format.
	var countBuffer [4]byte

	binary.LittleEndian.PutUint32(countBuffer[:], set.elementCount)

	// Append the header and encoded bitstream.
	output = append(output, countBuffer[:]...)
	output = append(output, set.encodedBits...)

	return output
}

// UnmarshalGolombCodedSet reconstructs a Golomb-Coded Set from its binary
// representation, performing basic validation of the header fields.
func UnmarshalGolombCodedSet(data []byte) (*GolombCodedSet, error) {
	// A valid encoding must contain at least the header.
	if len(data) < 5 {
		return nil, errors.New("golomb coded set invalid encoding")
	}

	// Extract and validate the Rice parameter.
	riceParameter := data[0]

	if riceParameter > 63 {
		return nil, errors.New("golomb coded set invalid rice parameter")
	}

	// Decode the element count and copy the encoded payload.
	return &GolombCodedSet{
		riceParameter: riceParameter,
		elementCount:  binary.LittleEndian.Uint32(data[1:5]),
		encodedBits:   append([]byte(nil), data[5:]...),
	}, nil
}

// BuildGolombCodedSet constructs a new Golomb-Coded Set from the provided
// uniformly distributed hash values and Rice parameter.
// The function sorts and delta-encodes the mapped values before applying
// Golomb–Rice encoding. An error is returned if parameters are invalid or
// derived ranges overflow.
func BuildGolombCodedSet(riceParameter uint8, hashedValues []uint64) (*GolombCodedSet, error) {
	// Validate the Rice parameter. Only values in [0, 63] are valid
	// for 64-bit Golomb–Rice encoding.
	if riceParameter > 63 {
		return nil, errors.New("golomb coded set invalid rice parameter")
	}

	elementCount := uint32(len(hashedValues))

	// An empty input produces a valid but empty set.
	if elementCount == 0 {
		return &GolombCodedSet{
			riceParameter: riceParameter,
			elementCount:  0,
			encodedBits:   nil,
		}, nil
	}

	// Compute the mapping range as N * 2^P, where N is the number
	// of elements and P is the Rice parameter.
	rangeMultiplier := uint64(1) << riceParameter
	valueRangeLimit := uint64(elementCount) * rangeMultiplier

	// Detect overflow or degenerate ranges defensively.
	if valueRangeLimit == 0 {
		return nil, errors.New("golomb coded set invalid value range limit")
	}

	// Map each hash value into the target range using fast
	// multiplication-based range reduction.
	mappedValues := make([]uint64, 0, elementCount)

	for _, hashValue := range hashedValues {
		reducedValue := fastRangeReduction(hashValue, valueRangeLimit)
		mappedValues = append(mappedValues, reducedValue)
	}

	// Sort mapped values so that delta encoding produces
	// non-negative, monotonic differences.
	sort.Slice(mappedValues, func(index, next int) bool {
		return mappedValues[index] < mappedValues[next]
	})

	// Convert sorted values into a delta-encoded sequence.
	deltaValues := make([]uint64, 0, elementCount)

	var previousValue uint64

	for index, currentValue := range mappedValues {
		if index == 0 {
			deltaValues = append(deltaValues, currentValue)
		} else {
			deltaValues = append(deltaValues, currentValue-previousValue)
		}

		previousValue = currentValue
	}

	// Encode each delta value using Golomb–Rice coding.
	bitWriter := newBitStreamWriter()

	for _, delta := range deltaValues {
		bitWriter.writeGolombRiceEncodedValue(delta, riceParameter)
	}

	return &GolombCodedSet{
		riceParameter: riceParameter,
		elementCount:  elementCount,
		encodedBits:   bitWriter.finalize(),
	}, nil
}

// Match tests whether the provided hashed value is a member of the set.
// The function returns false if the value is not present or if decoding fails
// due to malformed or truncated encoded data.
func (set *GolombCodedSet) Match(hashedValue uint64) bool {
	// Reject invalid or empty sets defensively.
	if set == nil || set.elementCount == 0 {
		return false
	}

	if set.riceParameter > 63 || len(set.encodedBits) == 0 {
		return false
	}

	// Recompute the mapping range used during construction.
	rangeMultiplier := uint64(1) << set.riceParameter
	valueRangeLimit := uint64(set.elementCount) * rangeMultiplier

	if valueRangeLimit == 0 {
		return false
	}

	// Map the queried hash value into the same reduced domain.
	targetValue := fastRangeReduction(hashedValue, valueRangeLimit)
	bitReader := newBitStreamReader(set.encodedBits)

	var accumulatedValue uint64

	// Decode delta values sequentially and reconstruct the original
	// mapped values until a match is found or the search can terminate.
	for decodedCount := uint32(0); decodedCount < set.elementCount; decodedCount++ {
		if !bitReader.hasRemainingBits() {
			return false
		}

		delta, ok := bitReader.readGolombRiceEncodedValue(set.riceParameter)

		if !ok {
			return false
		}

		// Reconstruct the next mapped value.
		accumulatedValue += delta

		// Any value outside the valid range indicates corruption.
		if accumulatedValue >= valueRangeLimit {
			return false
		}

		// Exact match indicates probable membership.
		if accumulatedValue == targetValue {
			return true
		}

		// Since values are sorted, exceeding the target means
		// it cannot appear later.
		if accumulatedValue > targetValue {
			return false
		}
	}

	return false
}
