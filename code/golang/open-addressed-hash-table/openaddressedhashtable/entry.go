// -----------------------------------------------------------------------------
// Package: openaddressedhashtable
// File: entry.go
//
// Description:
//
//	This file defines the internal entry representation used by the
//	open-addressed hash table implementation. An entry models a single
//	key–value slot within the backing array and explicitly tracks its
//	occupancy state to support linear probing and deletion semantics.
//
//	The entryState enumeration distinguishes between empty, occupied,
//	and deleted slots. This explicit state tracking is required to
//	preserve probe chains during lookup while allowing safe removal
//	of elements without immediate rehashing.
//
//	Entries are not exposed outside the package and are intended to be
//	manipulated exclusively by the hash table implementation.
//
// Structure:
//
//   - entryState
//     Enumerates the lifecycle state of a table slot.
//
//   - entry
//     Stores a key–value pair along with its current occupancy state.
//
// Usage:
//
//	This file is used internally by the hash table to manage storage,
//	probing, and deletion behavior. It is not intended for direct use
//	by consumers of the package.
//
// Notes:
//   - This file defines internal implementation details only.
//   - State transitions must preserve probe-chain correctness.
//   - Deleted entries act as tombstones and are reused on insertion.
//   - No memory allocation occurs at the entry level.
//
// -----------------------------------------------------------------------------
package openaddressedhashtable

type entryState uint8

const (
	empty entryState = iota
	occupied
	deleted
)

// entry represents a single slot in the open-addressed hash table.
// Each entry stores a key–value pair and an explicit state indicating
// whether the slot is empty, occupied, or a tombstone from a prior deletion.
// The state is used to preserve probe-chain correctness during lookup
// and insertion operations.
type entry[K comparable, V any] struct {
	key   K
	value V
	state entryState
}
