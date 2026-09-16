// -----------------------------------------------------------------------------
// Package: spacesavings
// File: bitset_test.go
//
// Description:
//
//	This file contains unit tests for the NaiveBooleanFlags and CompactBitset
//	data structures. The tests validate correctness, defensive behavior,
//	and logical equivalence between the naive and compact representations.
//
//	The test suite focuses on:
//
//	  - Correct setting and clearing of individual flags.
//	  - Accurate reporting of flag state.
//	  - Proper population counting.
//	  - Correct iteration over set bits.
//	  - Bulk operations such as SetAll and ClearAll.
//
// Notes:
//   - All tests use explicit Arrange / Act / Assert structure.
//   - The testing parameter is intentionally named "test" for clarity.
//   - Descriptive variable names are used throughout.
//   - Tests assume fixed-size, non-concurrent usage.
//
// -----------------------------------------------------------------------------
package spacesavings

import "testing"

// TestNaiveBooleanFlags_SetAndIsSet verifies that setting flags
// correctly updates their observable state.
func TestNaiveBooleanFlags_SetAndIsSet(test *testing.T) {
	// Arrange.
	numberOfFlags := 32

	flagStore := NewNaiveBooleanFlags(numberOfFlags)

	indicesToSet := []int{1, 5, 17, 31}

	// Act.
	for _, flagIndex := range indicesToSet {
		flagStore.Set(flagIndex)
	}

	// Assert.
	for _, flagIndex := range indicesToSet {
		if !flagStore.IsSet(flagIndex) {
			test.Fatalf("expected flag at index %d to be set",
				flagIndex,
			)
		}
	}
}

// TestNaiveBooleanFlags_Clear verifies that clearing a flag
// correctly resets its value.
func TestNaiveBooleanFlags_Clear(test *testing.T) {
	// Arrange.
	numberOfFlags := 16

	flagStore := NewNaiveBooleanFlags(numberOfFlags)

	flagIndex := 7

	flagStore.Set(flagIndex)

	// Act.
	flagStore.Clear(flagIndex)

	// Assert.
	if flagStore.IsSet(flagIndex) {
		test.Fatalf("expected flag at index %d to be cleared",
			flagIndex,
		)
	}
}

// TestCompactBitset_SetAndIsSet verifies that setting bits
// correctly updates their observable state.
func TestCompactBitset_SetAndIsSet(test *testing.T) {
	// Arrange.
	numberOfFlags := 128

	bitset := NewCompactBitset(numberOfFlags)

	indicesToSet := []int{0, 3, 64, 79, 127}

	// Act.
	for _, bitIndex := range indicesToSet {
		bitset.Set(bitIndex)
	}

	// Assert.
	for _, bitIndex := range indicesToSet {
		if !bitset.IsSet(bitIndex) {
			test.Fatalf("expected bit at index %d to be set",
				bitIndex,
			)
		}
	}
}

// TestCompactBitset_Clear verifies that clearing a bit
// correctly resets its value.
func TestCompactBitset_Clear(test *testing.T) {
	// Arrange.
	numberOfFlags := 64

	bitset := NewCompactBitset(numberOfFlags)

	bitIndex := 42

	bitset.Set(bitIndex)

	// Act.
	bitset.Clear(bitIndex)

	// Assert.
	if bitset.IsSet(bitIndex) {
		test.Fatalf("expected bit at index %d to be cleared",
			bitIndex,
		)
	}
}

// TestCompactBitset_Count verifies that the population count
// accurately reflects the number of set bits.
func TestCompactBitset_Count(test *testing.T) {
	// Arrange.
	numberOfFlags := 100

	bitset := NewCompactBitset(numberOfFlags)

	indicesToSet := []int{2, 4, 6, 8, 10}

	// Act.
	for _, bitIndex := range indicesToSet {
		bitset.Set(bitIndex)
	}

	// Assert.
	expectedCount := len(indicesToSet)
	actualCount := bitset.Count()

	if actualCount != expectedCount {
		test.Fatalf("expected bit count %d, got %d",
			expectedCount,
			actualCount,
		)
	}
}

// TestCompactBitset_SetAll verifies that SetAll marks
// all valid bits as set.
func TestCompactBitset_SetAll(test *testing.T) {
	// Arrange.
	numberOfFlags := 73

	bitset := NewCompactBitset(numberOfFlags)

	// Act.
	bitset.SetAll()

	// Assert.
	if bitset.Count() != numberOfFlags {
		test.Fatalf("expected %d set bits after SetAll, got %d",
			numberOfFlags,
			bitset.Count(),
		)
	}
}

// TestCompactBitset_ClearAll verifies that ClearAll
// clears all bits in the bitset.
func TestCompactBitset_ClearAll(test *testing.T) {
	// Arrange.
	numberOfFlags := 50

	bitset := NewCompactBitset(numberOfFlags)

	bitset.SetAll()

	// Act.
	bitset.ClearAll()

	// Assert.
	if bitset.Count() != 0 {
		test.Fatalf("expected bit count to be 0 after ClearAll")
	}
}

// TestCompactBitset_ForEach verifies that iteration visits
// exactly the set bits and no others.
func TestCompactBitset_ForEach(test *testing.T) {
	// Arrange.
	numberOfFlags := 128

	bitset := NewCompactBitset(numberOfFlags)

	expectedSetBits := map[int]bool{
		1:  true,
		7:  true,
		42: true,
		99: true,
	}

	for bitIndex := range expectedSetBits {
		bitset.Set(bitIndex)
	}

	visitedBits := make(map[int]bool)

	// Act.
	bitset.ForEach(func(bitIndex int) {
		visitedBits[bitIndex] = true
	})

	// Assert.
	if len(visitedBits) != len(expectedSetBits) {
		test.Fatalf("expected %d visited bits, got %d",
			len(expectedSetBits),
			len(visitedBits),
		)
	}

	for expectedBitIndex := range expectedSetBits {
		if !visitedBits[expectedBitIndex] {
			test.Fatalf("expected bit index %d to be visited",
				expectedBitIndex,
			)
		}
	}
}

// TestCompactBitset_Size verifies that Size reports the
// correct number of addressable bits.
func TestCompactBitset_Size(test *testing.T) {
	// Arrange.
	numberOfFlags := 256

	bitset := NewCompactBitset(numberOfFlags)

	// Act.
	reportedSize := bitset.Size()

	// Assert.
	if reportedSize != numberOfFlags {
		test.Fatalf("expected size %d, got %d",
			numberOfFlags,
			reportedSize,
		)
	}
}

// TestCompactBitset_SetIdempotence verifies that setting
// the same bit multiple times does not affect correctness.
func TestCompactBitset_SetIdempotence(test *testing.T) {
	// Arrange.
	numberOfFlags := 64

	bitset := NewCompactBitset(numberOfFlags)

	bitIndex := 13

	// Act.
	bitset.Set(bitIndex)
	bitset.Set(bitIndex)
	bitset.Set(bitIndex)

	// Assert.
	if !bitset.IsSet(bitIndex) {
		test.Fatalf("expected bit %d to remain set after repeated Set calls",
			bitIndex,
		)
	}

	if bitset.Count() != 1 {
		test.Fatalf("expected bit count to be 1, got %d",
			bitset.Count(),
		)
	}
}

// TestCompactBitset_ClearIdempotence verifies that clearing
// an already-cleared bit does not affect other bits.
func TestCompactBitset_ClearIdempotence(test *testing.T) {
	// Arrange.
	numberOfFlags := 64

	bitset := NewCompactBitset(numberOfFlags)

	firstBitIndex := 10
	secondBitIndex := 20

	bitset.Set(firstBitIndex)
	bitset.Set(secondBitIndex)

	// Act.
	bitset.Clear(firstBitIndex)
	bitset.Clear(firstBitIndex)

	// Assert.
	if bitset.IsSet(firstBitIndex) {
		test.Fatalf("expected bit %d to remain cleared",
			firstBitIndex,
		)
	}

	if !bitset.IsSet(secondBitIndex) {
		test.Fatalf("expected bit %d to remain set",
			secondBitIndex,
		)
	}

	if bitset.Count() != 1 {
		test.Fatalf("expected bit count to be 1, got %d",
			bitset.Count(),
		)
	}
}

// TestCompactBitset_AndWithSelf verifies that AND-ing
// a bitset with itself preserves all set bits.
func TestCompactBitset_AndWithSelf(test *testing.T) {
	// Arrange.
	numberOfFlags := 128

	bitset := NewCompactBitset(numberOfFlags)

	indicesToSet := []int{4, 9, 27, 63, 100}

	for _, bitIndex := range indicesToSet {
		bitset.Set(bitIndex)
	}

	// Act.
	bitset.And(bitset)

	// Assert.
	if bitset.Count() != len(indicesToSet) {
		test.Fatalf("expected bit count %d, got %d",
			len(indicesToSet),
			bitset.Count(),
		)
	}

	for _, bitIndex := range indicesToSet {
		if !bitset.IsSet(bitIndex) {
			test.Fatalf("expected bit %d to remain set after self-AND",
				bitIndex,
			)
		}
	}
}

// TestCompactBitset_OrWithEmpty verifies that OR-ing
// with an empty bitset preserves existing bits.
func TestCompactBitset_OrWithEmpty(test *testing.T) {
	// Arrange.
	numberOfFlags := 64

	leftBitset := NewCompactBitset(numberOfFlags)
	rightBitset := NewCompactBitset(numberOfFlags)

	indicesToSet := []int{2, 8, 16, 32}

	for _, bitIndex := range indicesToSet {
		leftBitset.Set(bitIndex)
	}

	// Act.
	leftBitset.Or(rightBitset)

	// Assert.
	if leftBitset.Count() != len(indicesToSet) {
		test.Fatalf("expected bit count %d, got %d",
			len(indicesToSet),
			leftBitset.Count(),
		)
	}

	for _, bitIndex := range indicesToSet {
		if !leftBitset.IsSet(bitIndex) {
			test.Fatalf("expected bit %d to remain set after OR with empty bitset",
				bitIndex,
			)
		}
	}
}

// TestNaiveAndCompactBitset_LogicalEquivalence verifies
// that naive and compact representations behave identically
// under the same sequence of operations.
func TestNaiveAndCompactBitset_LogicalEquivalence(test *testing.T) {
	// Arrange.
	numberOfFlags := 128

	naiveFlags := NewNaiveBooleanFlags(numberOfFlags)
	compactBitset := NewCompactBitset(numberOfFlags)

	indicesToSet := []int{1, 6, 25, 64, 90}
	indicesToClear := []int{6, 64}

	// Act.
	for _, index := range indicesToSet {
		naiveFlags.Set(index)
		compactBitset.Set(index)
	}

	for _, index := range indicesToClear {
		naiveFlags.Clear(index)
		compactBitset.Clear(index)
	}

	// Assert.
	for index := 0; index < numberOfFlags; index++ {
		naiveValue := naiveFlags.IsSet(index)
		compactValue := compactBitset.IsSet(index)

		if naiveValue != compactValue {
			test.Fatalf("mismatch at index %d: naive=%v, bitset=%v",
				index,
				naiveValue,
				compactValue,
			)
		}
	}
}
