// -----------------------------------------------------------------------------
// Package: concurrenthashtable
// File: concurrent_hash_table.go
//
// Description:
//
//	This file contains a sharded, lock-based implementation of a concurrent
//	hash table using Go generics. The implementation prioritizes correctness,
//	clear concurrency semantics, and defensive programming over aggressive
//	micro-optimizations.
//
//	The hash table is internally divided into multiple independent shards,
//	each protected by its own RWMutex. Keys are deterministically mapped to
//	shards using a stable hash function, reducing lock contention under
//	concurrent access.
//
//	All public operations are linearizable at the shard level and safely
//	support concurrent reads and writes. Atomic-style operations such as
//	LoadOrStore and CompareAndSwap are provided to support race-free
//	coordination patterns.
//
// Design Notes:
//   - The table uses sharded locking (lock striping) to reduce contention.
//   - Each shard owns an independent map and RWMutex.
//   - All map accesses are protected by the owning shard’s mutex.
//   - Hashing is stable across executions for supported key types.
//   - The table panics on misuse (nil or uninitialized receiver).
//   - The zero value of ConcurrentHashTable is not valid; use the constructor.
//   - Length is computed by aggregating shard sizes and is not globally
//     linearizable under concurrent mutation.
//
// Intended Use:
//
//	This package is intended as a reference implementation of a concurrent
//	hash table in Go, suitable for learning, experimentation, and extension.
//	It is not intended to replace sync.Map, but to demonstrate explicit
//	concurrency control and sharded data structure design.
//
// -----------------------------------------------------------------------------
package concurrenthashtable

import (
	"fmt"
	"hash/fnv"
	"sync"
)

const defaultSharedCount = 64

// normalizeRequestedShardCount ensures a valid shard count is used.
// If the requested count is zero or negative, a default is applied.
func normalizeRequestedShardCount(requestedShardCount int) int {
	// Guard against invalid shard counts.
	if requestedShardCount <= 0 {
		return defaultSharedCount
	}

	// Use the caller-provided shard count.
	return requestedShardCount
}

// fnv1a computes a 64-bit FNV-1a hash for the provided string.
// This function is used as the primary hashing mechanism for string keys.
func fnv1a(inout string) uint64 {
	// Create a new FNV-1a hasher.
	hasher := fnv.New64a()

	// Feed the input bytes into the hasher.
	// The error is ignored because Write on hash never fails.
	_, _ = hasher.Write([]byte(inout))

	// Return the final hash value.
	return hasher.Sum64()
}

// calculateStableHashForKey computes a deterministic hash value for a key.
// Common primitive key types are handled explicitly; all others fall back
// to a formatted string representation.
func calculateStableHashForKey[Key comparable](key Key) uint64 {
	// Perform a type switch on the concrete key type.
	switch concreteKey := any(key).(type) {
	case string:
		return fnv1a(concreteKey)
	case int:
		return uint64(concreteKey)
	case int64:
		return uint64(concreteKey)
	case uint64:
		return uint64(concreteKey)
	default:
		return fnv1a(fmt.Sprintf("%v", concreteKey))
	}
}

// shard represents a single shard within the hash table.
// Each shard owns its own map and RWMutex.
type shard[Key comparable, Value any] struct {
	mutex   sync.RWMutex
	buckets map[Key]Value
}

// SyncTable represents a sharded, concurrent hash table.
// Keys are distributed across shards to reduce lock contention.
type SyncTable[Key comparable, Value any] struct {
	shards []shard[Key, Value]
}

// NewSyncTable creates and returns a new concurrent hash table
// with the specified number of shards.
// If the requested shard count is invalid, a default is used.
func NewSyncTable[Key comparable, Value any](requestedShardCount int) *SyncTable[Key, Value] {
	// Normalize the requested shard count.
	normalizedShardCount := normalizeRequestedShardCount(requestedShardCount)

	// Allocate the shard slice.
	allocatedShardContainers := make([]shard[Key, Value], normalizedShardCount)

	// Initialize each shard with an empty map.
	for shardIndex := 0; shardIndex < normalizedShardCount; shardIndex++ {
		allocatedShardContainers[shardIndex] = shard[Key, Value]{
			buckets: make(map[Key]Value),
		}
	}

	// Return the fully initialized table.
	return &SyncTable[Key, Value]{
		shards: allocatedShardContainers,
	}
}

// ensureInitialized verifies that the hash table is non-nil and properly
// constructed. Misuse results in a panic.
func (table *SyncTable[Key, Value]) ensureInitialized() {
	// Reject nil receivers or empty shard slices.
	if table == nil || len(table.shards) == 0 {
		panic("concurrent hash table is nil or uninitialized")
	}
}

// getShardForKey returns the shard responsible for the provided key.
// The caller may safely assume the returned shard is valid.
func (table *SyncTable[Key, Value]) getShardForKey(key Key) *shard[Key, Value] {
	// Ensure the table is in a valid state.
	table.ensureInitialized()

	// Compute the total number of shards.
	shardCount := uint64(len(table.shards))

	// Hash the key deterministically.
	hashValue := calculateStableHashForKey(key)

	// Map the hash to a shard index.
	shardIndex := int(hashValue % shardCount)

	// Return a pointer to the selected shard.
	return &table.shards[shardIndex]
}

// Length returns the total number of key-value pairs stored in the table.
// This operation runs in linear time relative to the number of shards.
func (table *SyncTable[Key, Value]) Length() int {
	// Ensure the table is initialized.
	table.ensureInitialized()

	// Accumulate the total element count.
	totalElementCount := 0

	// Iterate over all shards.
	for shardIndex := 0; shardIndex < len(table.shards); shardIndex++ {
		currentShard := &table.shards[shardIndex]

		// Acquire a read lock to safely inspect the shard.
		currentShard.mutex.RLock()

		// Add the number of entries in this shard.
		totalElementCount += len(currentShard.buckets)

		// Release the read lock.
		currentShard.mutex.RUnlock()
	}

	// Return the aggregated count.
	return totalElementCount
}

// ShardCount returns the number of shards used by the table.
func (table *SyncTable[Key, Value]) ShardCount() int {
	// Ensure the table is initialized.
	table.ensureInitialized()

	// Return the shard count.
	return len(table.shards)
}

// Set inserts or replaces the value associated with the given key.
// This operation acquires an exclusive lock on the target shard.
func (table *SyncTable[Key, Value]) Set(key Key, value Value) {
	// Identify the shard responsible for the key.
	targetShard := table.getShardForKey(key)

	// Acquire an exclusive lock to mutate the map.
	targetShard.mutex.Lock()

	// Insert or overwrite the value.
	targetShard.buckets[key] = value

	// Release the lock.
	targetShard.mutex.Unlock()
}

// Get retrieves the value associated with the given key.
// If the key is not present, the zero value and false are returned.
func (table *SyncTable[Key, Value]) Get(key Key) (Value, bool) {
	// Identify the shard responsible for the key.
	targetShard := table.getShardForKey(key)

	// Acquire a read lock to safely read the map.
	targetShard.mutex.RLock()

	// Look up the value.
	retrievedValue, wasPresent := targetShard.buckets[key]

	// Release the read lock.
	targetShard.mutex.RUnlock()

	// Return the value and presence flag.
	return retrievedValue, wasPresent
}

// Delete removes the key-value pair associated with the given key.
// The return value indicates whether the key was present.
func (table *SyncTable[Key, Value]) Delete(key Key) bool {
	// Identify the shard responsible for the key.
	targetShard := table.getShardForKey(key)

	// Acquire an exclusive lock to mutate the map.
	targetShard.mutex.Lock()

	// Check whether the key exists.
	_, wasPresent := targetShard.buckets[key]

	// Remove the key if present.
	if wasPresent {
		delete(targetShard.buckets, key)
	}

	// Release the lock.
	targetShard.mutex.Unlock()

	// Report whether a deletion occurred.
	return wasPresent
}

// LoadOrStore returns the existing value for a key if present.
// Otherwise, it stores and returns the provided value.
// The boolean result reports whether the value was loaded.
func (table *SyncTable[Key, Value]) LoadOrStore(key Key, value Value) (Value, bool) {
	// Identify the shard responsible for the key.
	targetShard := table.getShardForKey(key)

	// Acquire an exclusive lock to ensure atomicity.
	targetShard.mutex.Lock()
	defer targetShard.mutex.Unlock()

	// Check if the key already exists.
	existingValue, exists := targetShard.buckets[key]

	// If present, return the existing value.
	if exists {
		return existingValue, true
	}

	// Otherwise, store the new value.
	targetShard.buckets[key] = value

	// Report that the value was stored.
	return value, false
}

// CompareAndSwap atomically updates the value for a key if and only if
// the current value matches the expected value according to the provided
// equality comparator.
func (table *SyncTable[Key, Value]) CompareAndSwap(key Key,
	expectedValue Value,
	newValue Value,
	equalityComparator func(compare, against Value) bool) bool {
	// Reject a nil comparator.
	if equalityComparator == nil {
		panic("compare and swap requires a non-nil equality comparator")
	}

	// Identify the shard responsible for the key.
	targetShard := table.getShardForKey(key)

	// Acquire an exclusive lock to ensure atomic compare-and-swap.
	targetShard.mutex.Lock()
	defer targetShard.mutex.Unlock()

	// Retrieve the current value.
	currentValue, exists := targetShard.buckets[key]

	// Fail if the key does not exist or the values do not match.
	if !exists || !equalityComparator(currentValue, expectedValue) {
		return false
	}

	// Replace the value.
	targetShard.buckets[key] = newValue

	// Report success.
	return true
}

// Range iterates over all key-value pairs in the table and invokes the
// provided visitor function for each entry. Iteration stops early if
// the visitor returns false.
func (table *SyncTable[Key, Value]) Range(visitor func(key Key, value Value) bool) {
	// Reject a nil visitor function.
	if visitor == nil {
		panic("range requires a non-nil visitor function")
	}

	// Ensure the table is initialized.
	table.ensureInitialized()

	// Iterate over all shards.
	for shardIndex := 0; shardIndex < len(table.shards); shardIndex++ {
		currentShard := &table.shards[shardIndex]

		// Acquire a read lock for safe iteration.
		currentShard.mutex.RLock()

		// Visit each key-value pair in the shard.
		for key, value := range currentShard.buckets {
			if !visitor(key, value) {
				currentShard.mutex.RUnlock()

				return
			}
		}

		// Release the read lock.
		currentShard.mutex.RUnlock()
	}
}
