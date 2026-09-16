// -----------------------------------------------------------------------------
// Package: golombcodedset
// File: bit_stream_writer.go
//
// Description:
//
//	This file defines a bit-level stream writer used for encoding
//	Golomb–Rice values into a compact byte buffer.
//
// -----------------------------------------------------------------------------
package golombcodedset

// bitStreamWriter provides bit-level, least-significant-bit-first writing
// over a dynamically growing byte buffer.
// It is used to encode Golomb–Rice values during set construction.
type bitStreamWriter struct {
	buffer        []byte
	currentByte   uint8
	bitsInCurrent uint8
}

// newBitStreamWriter constructs a new bit stream writer with an initialized
// internal buffer.
func newBitStreamWriter() *bitStreamWriter {
	// Allocate a bit stream writer with an initial buffer capacity
	// to reduce reallocations during encoding.
	return &bitStreamWriter{
		buffer: make([]byte, 0, 128),
	}
}

// writeSingleBit writes a single bit to the stream. The bit is appended in
// least-significant-bit-first order.
// The function panics if the internal bit state becomes invalid, indicating
// a programmer error.
func (writer *bitStreamWriter) writeSingleBit(bit uint8) {
	// Guard against an invalid internal state. bitsInCurrent should
	// never exceed 7 prior to writing a new bit. Exceeding this
	// indicates a programmer error in bit accounting.
	if writer.bitsInCurrent >= 8 {
		panic("bit stream writer invalid bit state")
	}

	// If the bit value is non-zero, set the corresponding bit position
	// in the current output byte. Bits are written least-significant
	// bit first.
	if bit != 0 {
		writer.currentByte |= 1 << writer.bitsInCurrent
	}

	// Advance the bit position within the current byte.
	writer.bitsInCurrent++

	// If the current byte is now fully populated, flush it to the
	// output buffer and reset the byte accumulator state.
	if writer.bitsInCurrent == 8 {
		writer.buffer = append(writer.buffer, writer.currentByte)
		writer.currentByte = 0
		writer.bitsInCurrent = 0
	}
}

// writeLeastSignificantBits writes the specified number of least-significant
// bits from the provided value to the stream.
func (writer *bitStreamWriter) writeLeastSignificantBits(value uint64, bitCount uint8) {
	// Writing more than 63 bits would exceed uint64 capacity
	// and indicates invalid caller usage.
	if bitCount > 63 {
		return
	}

	// Emit bits starting from the least significant bit,
	// matching the LSB-first encoding convention used throughout.
	for bitIndex := uint8(0); bitIndex < bitCount; bitIndex++ {
		writer.writeSingleBit(uint8((value >> bitIndex) & 1))
	}
}

// writeUnaryEncodedValue writes a unary-encoded representation of the provided
// value to the stream.
func (writer *bitStreamWriter) writeUnaryEncodedValue(value uint64) {
	// Write a sequence of one bits equal to the value.
	for index := uint64(0); index < value; index++ {
		writer.writeSingleBit(1)
	}

	// Write the terminating zero bit to complete the unary code.
	writer.writeSingleBit(0)
}

// writeGolombRiceEncodedValue writes a Golomb–Rice encoded representation of
// the provided value using the specified Rice parameter.
func (writer *bitStreamWriter) writeGolombRiceEncodedValue(value uint64, riceParameter uint8) {
	// Reject invalid Rice parameters defensively.
	if riceParameter > 63 {
		return
	}

	// Split the value into a unary-encoded quotient and a
	// fixed-width remainder as defined by Golomb–Rice coding.
	quotient := value >> riceParameter
	remainder := value & ((uint64(1) << riceParameter) - 1)

	// Write the quotient followed by the remainder bits.
	writer.writeUnaryEncodedValue(quotient)
	writer.writeLeastSignificantBits(remainder, riceParameter)
}

// finalize flushes any partially written byte to the buffer and returns the
// complete encoded byte slice.
func (writer *bitStreamWriter) finalize() []byte {
	// If a partial byte is present, flush it to the buffer
	// so that no encoded bits are lost.
	if writer.bitsInCurrent != 0 {
		writer.buffer = append(writer.buffer, writer.currentByte)
	}

	// Return the complete encoded bitstream.
	return writer.buffer
}
