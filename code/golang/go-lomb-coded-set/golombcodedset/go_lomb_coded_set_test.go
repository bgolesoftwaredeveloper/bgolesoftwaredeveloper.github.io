// -----------------------------------------------------------------------------
// Package: golombcodedset
// File: golomb_coded_set_test.go
//
// Description:
//
//	This file contains unit tests for the golombcodedset package.
//	The tests validate correctness, defensive behavior, and round-trip
//	integrity of Golomb-Coded Set construction, serialization, decoding,
//	and membership queries.
//
//	Each test follows a strict Arrange / Act / Assert structure to ensure
//	readability and consistency with the rest of the codebase.
//
//	The tests intentionally favor clarity and explicitness over compactness
//	and are designed to detect regressions in bit-level encoding logic,
//	range reduction behavior, and decoding safety.
//
// Notes:
//   - All tests use uniformly distributed 64-bit hash values.
//   - Test parameters use the name `test` instead of `t` by design.
//   - No test relies on undefined behavior or implementation details.
//
// -----------------------------------------------------------------------------
package golombcodedset

import (
	"bytes"
	"testing"
)

// TestMultiplyUint64ProducesCorrect128BitProduct verifies that the 128-bit
// multiplication utility produces correct high and low halves for known inputs.
func TestMultiplyUint64ProducesCorrect128BitProduct(test *testing.T) {
	// Arrange.
	var multiplicand uint64 = 0xFFFFFFFFFFFFFFFF
	var multiplier uint64 = 2

	// Act.
	upperHalf, lowerHalf := multiplyUint64(multiplicand, multiplier)

	// Assert.
	if lowerHalf != 0xFFFFFFFFFFFFFFFE {
		test.Fatalf("unexpected lower 64 bits %x", lowerHalf)
	}

	if upperHalf != 1 {
		test.Fatalf("unexpected upper 64 bits %x", upperHalf)
	}
}

// TestFastRangeReductionProducesValuesWithinRange verifies that reduced values
// always fall within the expected half-open interval [0, rangeLimit).
func TestFastRangeReductionProducesValuesWithinRange(test *testing.T) {
	// Arrange.
	var rangeLimit uint64 = 1000
	var inputValues = []uint64{
		0,
		1,
		42,
		123456789,
		0xFFFFFFFFFFFFFFFF,
	}

	// Act / Assert.
	for _, originalValue := range inputValues {
		reducedValue := fastRangeReduction(originalValue, rangeLimit)

		if reducedValue >= rangeLimit {
			test.Fatalf("reduced value %d exceeds range limit %d", reducedValue, rangeLimit)
		}
	}
}

// TestBuildGolombCodedSetAndMatch verifies successful construction and correct
// membership detection for present and absent values.
func TestBuildGolombCodedSetAndMatch(test *testing.T) {
	// Arrange.
	var riceParameter uint8 = 10
	var hashedInputValues = []uint64{
		0x0123456789ABCDEF,
		0x1111111111111111,
		0x2222222222222222,
		0x3333333333333333,
		0x4444444444444444,
	}

	// Act.
	golombSet, buildError := BuildGolombCodedSet(riceParameter, hashedInputValues)

	// Assert.
	if buildError != nil {
		test.Fatalf("unexpected build error %v", buildError)
	}

	for _, presentValue := range hashedInputValues {
		if !golombSet.Match(presentValue) {
			test.Fatalf("expected value %x to be present", presentValue)
		}
	}

	var absentValue uint64 = 0x9999999999999999

	if golombSet.Match(absentValue) {
		test.Fatalf("unexpected match for absent value %x", absentValue)
	}
}

// TestMarshalAndUnmarshalPreservesSet verifies that binary serialization and
// deserialization preserve all observable set behavior.
func TestMarshalAndUnmarshalPreservesSet(test *testing.T) {
	// Arrange.
	var riceParameter uint8 = 8
	var hashedInputValues = []uint64{
		0xAAAAAAAAAAAAAAAA,
		0xBBBBBBBBBBBBBBBB,
		0xCCCCCCCCCCCCCCCC,
	}

	originalSet, buildError := BuildGolombCodedSet(riceParameter, hashedInputValues)

	if buildError != nil {
		test.Fatalf("unexpected build error %v", buildError)
	}

	// Act.
	serializedBytes := originalSet.MarshalBinary()
	decodedSet, decodeError := UnmarshalGolombCodedSet(serializedBytes)

	// Assert.
	if decodeError != nil {
		test.Fatalf("unexpected decode error %v", decodeError)
	}

	for _, expectedValue := range hashedInputValues {
		if !decodedSet.Match(expectedValue) {
			test.Fatalf("expected value %x missing after decode", expectedValue)
		}
	}
}

// TestEmptyGolombCodedSetBehavior verifies that empty sets are valid,
// serializable, and never match any values.
func TestEmptyGolombCodedSetBehavior(test *testing.T) {
	// Arrange.
	var riceParameter uint8 = 5
	var emptyValueSlice []uint64

	// Act.
	emptySet, buildError := BuildGolombCodedSet(riceParameter, emptyValueSlice)

	// Assert.
	if buildError != nil {
		test.Fatalf("unexpected build error %v", buildError)
	}

	if emptySet.Match(0x1234567890ABCDEF) {
		test.Fatalf("empty set should not match any value")
	}

	serializedBytes := emptySet.MarshalBinary()

	if len(serializedBytes) == 0 {
		test.Fatalf("expected non-empty serialization for empty set")
	}
}

// TestBitStreamWriterAndReaderRoundTrip verifies that values written via the
// bit stream writer can be read back losslessly by the reader.
func TestBitStreamWriterAndReaderRoundTrip(test *testing.T) {
	// Arrange.
	var riceParameter uint8 = 6
	var originalValues = []uint64{0, 1, 5, 17, 64, 255}

	bitWriter := newBitStreamWriter()

	// Act.
	for _, value := range originalValues {
		bitWriter.writeGolombRiceEncodedValue(value, riceParameter)
	}

	encodedBuffer := bitWriter.finalize()
	bitReader := newBitStreamReader(encodedBuffer)

	// Assert.
	for _, expectedValue := range originalValues {
		decodedValue, ok := bitReader.readGolombRiceEncodedValue(riceParameter)

		if !ok {
			test.Fatalf("failed to decode value %d", expectedValue)
		}

		if decodedValue != expectedValue {
			test.Fatalf("decoded value %d does not match expected %d", decodedValue, expectedValue)
		}
	}
}

// TestUnmarshalRejectsInvalidHeader verifies that malformed serialized data
// is rejected during deserialization.
func TestUnmarshalRejectsInvalidHeader(test *testing.T) {
	// Arrange.
	var invalidSerializedData = []byte{0xFF}

	// Act.
	decodedSet, decodeError := UnmarshalGolombCodedSet(invalidSerializedData)

	// Assert.
	if decodeError == nil {
		test.Fatalf("expected decode error but got nil set %v", decodedSet)
	}
}

// TestSerializedOutputIsDeterministic verifies that serialization output
// is stable for identical inputs.
func TestSerializedOutputIsDeterministic(test *testing.T) {
	// Arrange.
	var riceParameter uint8 = 7
	var hashedInputValues = []uint64{
		0xDEADBEEFDEADBEEF,
		0xCAFEBABECAFEBABE,
	}

	firstSet, _ := BuildGolombCodedSet(riceParameter, hashedInputValues)
	secondSet, _ := BuildGolombCodedSet(riceParameter, hashedInputValues)

	// Act.
	firstBytes := firstSet.MarshalBinary()
	secondBytes := secondSet.MarshalBinary()

	// Assert.
	if !bytes.Equal(firstBytes, secondBytes) {
		test.Fatalf("serialization output is not deterministic")
	}
}
