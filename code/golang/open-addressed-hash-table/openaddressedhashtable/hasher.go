// -----------------------------------------------------------------------------
// Package: openaddressedhashtable
// File: hasher.go
//
// Description:
//
//	This file defines the hashing abstraction used by the open-addressed
//	hash table implementation. The Hasher interface decouples key hashing
//	from table storage logic, allowing different hashing strategies to be
//	substituted without modifying the core data structure.
//
//	A default StringHasher implementation is provided using the FNV-1a
//	64-bit hash function. This implementation is suitable for general
//	purpose string keys and provides a balance between simplicity and
//	hash distribution quality.
//
//	The hashing logic defined in this file is used to map keys to initial
//	table indices prior to probing.
//
// Usage:
//
//	Custom hashers may be supplied by implementing the Hasher interface
//	and passing the implementation to the hash table constructor.
//
// Notes:
//   - Hashers must be deterministic for a given key.
//   - Poor hash distribution will degrade probe performance.
//   - This file defines internal infrastructure, not table behavior.
//
// -----------------------------------------------------------------------------
package openaddressedhashtable

import "hash/fnv"

// Hasher defines the interface required to hash keys for use in the
// open-addressed hash table. Implementations must return a stable,
// deterministic hash value for the same key.
type Hasher[K comparable] interface {
	Hash(key K) uint64
}

// StringHasher implements the Hasher interface for string keys using
// the FNV-1a 64-bit hashing algorithm.
type StringHasher struct{}

// Hash computes and returns the FNV-1a 64-bit hash of the provided string key.
// The returned hash value is used to determine the initial probe index
// within the hash table.
func (hash StringHasher) Hash(key string) uint64 {
	hasher := fnv.New64a()
	hasher.Write([]byte(key))

	return hasher.Sum64()
}
