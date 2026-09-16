// -----------------------------------------------------------------------------
// Package: spacesavings
// File: bitset.go
//
// Description:
//
//	This file provides two alternative implementations for storing and
//	manipulating collections of binary flags:
//
//	  1. NaiveBooleanFlags — a straightforward []bool-based representation.
//	  2. CompactBitset     — a memory-efficient bitset backed by []uint64.
//
//	The primary purpose of this package is to demonstrate *space savings*
//	and the practical trade-offs between clarity and memory efficiency.
//	While NaiveBooleanFlags prioritizes simplicity, CompactBitset packs
//	flags densely, using a single bit per value.
//
//	The CompactBitset implementation supports a rich set of operations,
//	including bulk updates, logical combination, population counting,
//	and efficient iteration over set bits.
//
// Defensive Design Principles:
//
//   - All methods explicitly reject nil receivers.
//   - All index-based operations enforce bounds checks.
//   - Invalid usage fails fast via panics.
//   - No silent behavior or implicit resizing is performed.
//
// Notes:
//   - CompactBitset is fixed-size after construction.
//   - Bit indexing uses integer division and modulo operations.
//   - Iteration over set bits skips zero regions efficiently.
//   - No concurrency or atomic guarantees are provided.
//
// Intended Use:
//
//	This package is intended for educational purposes, performance experiments,
//	and as a reference for memory-conscious data structure design in Go.
//
// -----------------------------------------------------------------------------
package spacesavings

import "math/bits"

// NaiveBooleanFlags stores binary flags using a plain boolean slice.
// Each flag consumes at least one byte of memory.
type NaiveBooleanFlags struct {
	flags []bool
}

// NewNaiveBooleanFlags allocates a boolean flag store with space for
// the specified number of flags.
func NewNaiveBooleanFlags(numberOfFlags int) *NaiveBooleanFlags {
	// Reject negative sizes to avoid invalid slice allocation.
	if numberOfFlags < 0 {
		panic("number of flags must be non-negative")
	}

	// Allocate and return the flag store.
	return &NaiveBooleanFlags{
		flags: make([]bool, numberOfFlags),
	}
}

// IsSet reports whether the flag at the given index is set to true.
func (store *NaiveBooleanFlags) IsSet(index int) bool {
	// Ensure the receiver is valid.
	if store == nil {
		panic("Naive boolean flags receiver is nil")
	}

	// Ensure the index lies within the slice bounds.
	if index < 0 || index >= len(store.flags) {
		panic("flag index out of bounds")
	}

	// Return the stored flag value.
	return store.flags[index]
}

// Set marks the flag at the given index as true.
func (store *NaiveBooleanFlags) Set(index int) {
	// Ensure the receiver is valid.
	if store == nil {
		panic("Naive boolean flags receiver is nil")
	}

	// Ensure the index lies within the slice bounds.
	if index < 0 || index >= len(store.flags) {
		panic("flag index out of bounds")
	}

	// Set the flag value.
	store.flags[index] = true
}

// Clear marks the flag at the given index as false.
func (store *NaiveBooleanFlags) Clear(index int) {
	// Ensure the receiver is valid.
	if store == nil {
		panic("Naive boolean flags receiver is nil")
	}

	// Ensure the index lies within the slice bounds.
	if index < 0 || index >= len(store.flags) {
		panic("flag index out of bounds")
	}

	// Clear the flag value.
	store.flags[index] = false
}

// CompactBitset stores binary flags packed into 64-bit machine words.
// Each flag occupies exactly one bit.
type CompactBitset struct {
	words []uint64
	size  int
}

// NewCompactBitset constructs a fixed-size bitset capable of storing
// the specified number of flags.
func NewCompactBitset(numberOfFlags int) *CompactBitset {
	// Reject negative sizes to prevent invalid allocation.
	if numberOfFlags < 0 {
		panic("number of flags must be non-negative")
	}

	// Compute the number of 64-bit words required.
	numberOfWords := (numberOfFlags + 63) / 64

	// Allocate and return the bitset.
	return &CompactBitset{
		words: make([]uint64, numberOfWords),
		size:  numberOfFlags,
	}
}

// IsSet reports whether the bit at the given index is set.
func (bitset *CompactBitset) IsSet(index int) bool {
	// Ensure the receiver is valid.
	if bitset == nil {
		panic("compact bitset receiver is nil")
	}

	// Ensure the index lies within the valid range.
	if index < 0 || index >= bitset.size {
		panic("bit index out of bounds")
	}

	// Determine the word and bit position.
	wordIndex := index / 64
	bitOffset := uint(index % 64)

	// Test the corresponding bit.
	return (bitset.words[wordIndex] & (1 << bitOffset)) != 0
}

// Size returns the number of addressable bits in the bitset.
func (bitset *CompactBitset) Size() int {
	// Ensure the receiver is valid.
	if bitset == nil {
		panic("compact bitset receiver is nil")
	}

	return bitset.size
}

// Count returns the number of bits currently set to true.
func (bitset *CompactBitset) Count() int {
	// Ensure the receiver is valid.
	if bitset == nil {
		panic("compact bitset receiver is nil")
	}

	count := 0

	// Count bits in each word using a hardware-accelerated operation.
	for _, word := range bitset.words {
		count += bits.OnesCount64(word)
	}

	return count
}

// And performs an in-place logical AND with another bitset of equal size.
func (bitset *CompactBitset) And(other *CompactBitset) {
	// Ensure both receivers are valid.
	if bitset == nil || other == nil {
		panic("compact bitset receiver is nil")
	}

	// Enforce equal-size invariant.
	if bitset.size != other.size {
		panic("bitsets must be the same size")
	}

	// Combine corresponding words.
	for index := range bitset.words {
		bitset.words[index] &= other.words[index]
	}
}

// Or performs an in-place logical OR with another bitset of equal size.
func (bitset *CompactBitset) Or(other *CompactBitset) {
	// Ensure both receivers are valid.
	if bitset == nil || other == nil {
		panic("compact bitset receiver is nil")
	}

	// Enforce equal-size invariant.
	if bitset.size != other.size {
		panic("bitsets must be the same size")
	}

	// Combine corresponding words.
	for index := range bitset.words {
		bitset.words[index] |= other.words[index]
	}
}

// Set marks the bit at the given index as true.
func (bitset *CompactBitset) Set(index int) {
	// Ensure the receiver is valid.
	if bitset == nil {
		panic("compact bitset receiver is nil")
	}

	// Ensure the index lies within the valid range.
	if index < 0 || index >= bitset.size {
		panic("bit index out of bounds")
	}

	// Determine the word and bit position.
	wordIndex := index / 64
	bitOffset := uint(index % 64)

	// Set the bit.
	bitset.words[wordIndex] |= 1 << bitOffset
}

// SetAll marks all valid bits in the bitset as true.
func (bitset *CompactBitset) SetAll() {
	// Ensure the receiver is valid.
	if bitset == nil {
		panic("compact bitset receiver is nil")
	}

	// Set all bits in all words.
	for index := range bitset.words {
		bitset.words[index] = ^uint64(0)
	}

	// Mask off unused bits in the final word.
	numberOfWords := len(bitset.words)
	excessBits := numberOfWords*64 - bitset.size

	if excessBits > 0 {
		bitset.words[numberOfWords-1] >>= excessBits
		bitset.words[numberOfWords-1] <<= excessBits
	}
}

// ForEach invokes the provided function for every set bit index.
func (bitset *CompactBitset) ForEach(visit func(index int)) {
	// Ensure the receiver and callback are valid.
	if bitset == nil {
		panic("compact bitset receiver is nil")
	}

	if visit == nil {
		panic("visit function is nil")
	}

	// Iterate over all words in the bitset.
	for wordIndex, word := range bitset.words {
		// Process only set bits using trailing-zero scans.
		for word != 0 {
			lowestBit := word & -word
			bitOffset := bits.TrailingZeros64(word)
			bitIndex := wordIndex*64 + bitOffset

			// Invoke the callback if the bit index is valid.
			if bitIndex < bitset.size {
				visit(bitIndex)
			}

			// Clear the lowest set bit.
			word ^= lowestBit
		}
	}
}

// Clear marks the bit at the given index as false.
func (bitset *CompactBitset) Clear(index int) {
	// Ensure the receiver is valid.
	if bitset == nil {
		panic("compact bitset receiver is nil")
	}

	// Ensure the index lies within the valid range.
	if index < 0 || index >= bitset.size {
		panic("bit index out of bounds")
	}

	// Determine the word and bit position.
	wordIndex := index / 64
	bitOffset := uint(index % 64)

	// Clear the bit.
	bitset.words[wordIndex] &^= 1 << bitOffset
}

// ClearAll clears all bits in the bitset.
func (bitset *CompactBitset) ClearAll() {
	// Ensure the receiver is valid.
	if bitset == nil {
		panic("compact bitset receiver is nil")
	}

	// Clear all words.
	for index := range bitset.words {
		bitset.words[index] = 0
	}
}
