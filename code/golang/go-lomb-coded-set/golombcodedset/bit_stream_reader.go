// -----------------------------------------------------------------------------
// Package: golombcodedset
// File: bit_stream_reader.go
//
// Description:
//
//	This file defines a bit-level stream reader used for decoding
//	Golomb–Rice encoded values in a defensive and bounds-checked manner.
//
// -----------------------------------------------------------------------------
package golombcodedset

// bitStreamReader provides bit-level, least-significant-bit-first reading
// over a byte slice.
// It is used to decode Golomb–Rice encoded values in a defensive manner,
// ensuring bounds checks and safe termination on malformed input.
type bitStreamReader struct {
	buffer      []byte
	byteIndex   int
	currentByte uint8
	bitOffset   uint8
}

// newBitStreamReader constructs a new bit stream reader for the provided
// encoded bit buffer. The reader is initialized in a state that forces
// a byte fetch on the first bit read.
func newBitStreamReader(encodedBits []byte) *bitStreamReader {
	// Initialize the reader with the provided encoded bit buffer.
	// Setting bitOffset to 8 forces the first call to readSingleBit
	// to load a fresh byte from the buffer.
	return &bitStreamReader{
		buffer:    encodedBits,
		bitOffset: 8,
	}
}

// hasRemainingBits reports whether additional bits are available for reading
// from the underlying buffer.
func (reader *bitStreamReader) hasRemainingBits() bool {
	// If there are unread bytes remaining, bits are available.
	if reader.byteIndex < len(reader.buffer) {
		return true
	}

	// If all bytes have been consumed, bits may still remain
	// in the currently loaded byte.
	return reader.byteIndex == len(reader.buffer) && reader.bitOffset < 8
}

// remainingBits returns the total number of unread bits remaining in the
// underlying buffer, accounting for partially consumed bytes.
func (reader *bitStreamReader) remainingBits() uint64 {
	// Compute the total number of bits available in the buffer.
	// Each byte contributes exactly 8 bits.
	totalBits := uint64(len(reader.buffer)) * 8

	// Compute the number of bits fully consumed by advancing
	// past complete bytes.
	consumedBits := uint64(reader.byteIndex) * 8

	// If a byte has been loaded and partially consumed, account
	// for the number of bits already read from the current byte.
	// The byteIndex check ensures we do not count bits before any
	// byte has been loaded.
	if reader.bitOffset < 8 && reader.byteIndex > 0 {
		consumedBits += uint64(reader.bitOffset)
	}

	// If consumption has reached or exceeded the total available
	// bits, no bits remain to be read.
	if consumedBits >= totalBits {
		return 0
	}

	// Return the number of unread bits remaining in the stream.
	return totalBits - consumedBits
}

// readSingleBit reads and returns the next least-significant bit from the
// stream. The second return value reports whether the read was successful.
func (reader *bitStreamReader) readSingleBit() (uint8, bool) {
	// If all bits in the current byte have been consumed,
	// advance to the next byte in the buffer.
	if reader.bitOffset == 8 {
		// Ensure there is another byte available to read.
		// If not, the stream has been exhausted.
		if reader.byteIndex >= len(reader.buffer) {
			return 0, false
		}

		// Load the next byte from the buffer and reset the bit offset.
		reader.currentByte = reader.buffer[reader.byteIndex]
		reader.byteIndex++
		reader.bitOffset = 0
	}

	// Extract the current bit by shifting the byte right by the
	// current bit offset and masking the least significant bit.
	bit := (reader.currentByte >> reader.bitOffset) & 1

	// Advance the bit offset so the next call reads the next bit.
	reader.bitOffset++

	// Return the extracted bit and indicate a successful read.
	return bit, true
}

// readLeastSignificantBits reads the specified number of bits from the stream
// and assembles them into a uint64 value in least-significant-bit-first order.
// The function fails if the requested number of bits exceeds 63 or if the
// stream is exhausted.
func (reader *bitStreamReader) readLeastSignificantBits(bitCount uint8) (uint64, bool) {
	// Guard against invalid bit counts. Reading more than 63 bits
	// would overflow a uint64 when assembling the result.
	if bitCount > 63 {
		return 0, false
	}

	// Accumulator for the reconstructed value.
	// Bits are assembled in least-significant-bit-first order.
	var value uint64

	// Read the requested number of bits sequentially from the stream.
	for bitIndex := uint8(0); bitIndex < bitCount; bitIndex++ {
		// Read the next bit from the underlying bit stream.
		bit, ok := reader.readSingleBit()

		// Fail immediately if the stream is exhausted or malformed.
		if !ok {
			return 0, false
		}

		// Incorporate the bit into the result at the corresponding
		// bit position. The first bit read becomes bit 0, the next
		// bit 1, and so on.
		value |= uint64(bit) << bitIndex
	}

	// Successfully reconstructed the requested bit field.
	return value, true
}

// readUnaryEncodedValue decodes a unary-encoded integer from the stream.
// The value is represented as a sequence of one bits terminated by a zero bit.
// Decoding is bounded by the number of remaining bits to prevent infinite loops
// on malformed input.
func (reader *bitStreamReader) readUnaryEncodedValue() (uint64, bool) {
	var count uint64

	// Limit the maximum unary run length to the number of remaining
	// bits to avoid infinite loops on malformed input.
	maximumPossibleUnaryLength := reader.remainingBits()

	for count < maximumPossibleUnaryLength {
		// Read the next bit from the stream.
		bit, ok := reader.readSingleBit()

		if !ok {
			return 0, false
		}

		// A zero bit terminates the unary code.
		if bit == 0 {
			return count, true
		}

		// Each leading one increments the decoded value.
		count++
	}

	// Unary code did not terminate within the remaining bit budget.
	return 0, false
}

// readGolombRiceEncodedValue decodes a Golomb–Rice encoded value using the
// provided Rice parameter.
// The function returns the decoded value and a boolean indicating success.
func (reader *bitStreamReader) readGolombRiceEncodedValue(riceParameter uint8) (uint64, bool) {
	// The Rice parameter determines the split between unary quotient
	// and fixed-width remainder bits.
	if riceParameter > 63 {
		return 0, false
	}

	// Decode the unary-encoded quotient portion.
	quotient, ok := reader.readUnaryEncodedValue()

	if !ok {
		return 0, false
	}

	// Decode the fixed-width remainder portion.
	remainder, ok := reader.readLeastSignificantBits(riceParameter)

	if !ok {
		return 0, false
	}

	// Recombine quotient and remainder to form the original value.
	return (quotient << riceParameter) | remainder, true
}
