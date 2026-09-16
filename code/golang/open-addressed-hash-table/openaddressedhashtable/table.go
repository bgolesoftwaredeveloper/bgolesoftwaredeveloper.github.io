// -----------------------------------------------------------------------------
// Package: openaddressedhashtable
// File: table.go
//
// Description:
//
//	This file defines the core open-addressed hash table implementation.
//	The hash table uses linear probing for collision resolution and stores
//	entries in a contiguous backing array. Deleted entries are marked using
//	tombstones to preserve probe-chain correctness during lookup and
//	insertion operations.
//
//	The implementation supports generic key and value types, configurable
//	hashing via the Hasher interface, dynamic resizing based on load factor,
//	and standard dictionary operations including insertion, lookup, and
//	deletion.
//
//	This file contains the primary table logic and enforces all invariants
//	related to probing, resizing, and entry state transitions.
//
// Notes:
//   - This implementation is not thread-safe.
//   - Linear probing performance depends on hash quality and load factor.
//   - Resizing rehashes all occupied entries into a new backing array.
//
// -----------------------------------------------------------------------------
package openaddressedhashtable

// HashTable represents an open-addressed hash table using linear probing
// for collision resolution. The table stores entries in a contiguous
// backing array and uses tombstones to preserve probe chains during
// deletion.
type HashTable[K comparable, V any] struct {
	entries []entry[K, V]
	size    int
	hasher  Hasher[K]
}

// New constructs and returns a new hash table instance using the provided
// Hasher implementation. The table is initialized with a default capacity.
func New[K comparable, V any](hasher Hasher[K]) *HashTable[K, V] {
	// Ensure a valid hasher is provided, as hashing is required for all operations.
	if hasher == nil {
		panic("hasher must not be nil")
	}

	// Allocate the backing array with the default initial capacity.
	return &HashTable[K, V]{
		entries: make([]entry[K, V], defaultCapacity),
		hasher:  hasher,
	}
}

// Length returns the number of key–value pairs currently stored in the
// hash table.
func (table *HashTable[K, V]) Length() int {
	// Guard against calling methods on a nil table instance.
	if table == nil {
		panic("table is nil")
	}

	// Return the current count of occupied entries.
	return table.size
}

// indexFor computes the initial probe index for a given key based on the
// provided table capacity.
func (table *HashTable[K, V]) indexFor(key K, capacity int) int {
	// Guard against a nil table reference.
	if table == nil {
		panic("table is nil")
	}

	// Ensure the capacity is valid to avoid division-by-zero.
	if capacity <= 0 {
		panic("invalid table capacity")
	}

	// Hash the key and map it into the bounds of the backing array.
	return int(table.hasher.Hash(key) % uint64(capacity))
}

// Put inserts a key–value pair into the hash table. If the key already
// exists, its associated value is overwritten. The table automatically
// resizes when the load factor exceeds the configured threshold.
func (table *HashTable[K, V]) Put(key K, value V) {
	// Guard against a nil table reference.
	if table == nil {
		panic("table is nil")
	}

	// Check whether inserting a new element would exceed the load factor.
	// If so, grow the table and rehash existing entries.
	if float64(table.size+1)/float64(len(table.entries)) > defaultLoadFactor {
		table.resize(len(table.entries) * 2)
	}

	// Compute the initial probe index for the given key.
	index := table.indexFor(key, len(table.entries))

	// Probe linearly through the table, bounded by the table size.
	for probe := 0; probe < len(table.entries); probe++ {
		node := &table.entries[index]

		// If the slot is empty or marked deleted, insert the new entry here.
		if node.state == empty || node.state == deleted {
			*node = entry[K, V]{
				key:   key,
				value: value,
				state: occupied,
			}

			// Increment the count of occupied entries.
			table.size++

			return
		}

		// If the slot contains the same key, overwrite its value.
		if node.state == occupied && node.key == key {
			node.value = value

			return
		}

		// Advance to the next index, wrapping around the table.
		index = (index + 1) % len(table.entries)
	}

	// If all slots were probed without success, the table is full.
	panic("put failed because table is full")
}

// resize reallocates the backing array to the specified capacity and
// rehashes all occupied entries into the new table.
func (table *HashTable[K, V]) resize(newCapacity int) {
	// Guard against a nil table reference.
	if table == nil {
		panic("table is nil")
	}

	// Ensure the new capacity is valid.
	if newCapacity <= 0 {
		panic("invalid resize capacity")
	}

	// Preserve the old entries for rehashing.
	oldEntries := table.entries

	// Allocate a new backing array with the requested capacity.
	table.entries = make([]entry[K, V], newCapacity)

	// Reset the size; it will be recomputed during reinsertion.
	table.size = 0

	// Reinsert all previously occupied entries into the new table.
	for _, entry := range oldEntries {
		if entry.state == occupied {
			table.Put(entry.key, entry.value)
		}
	}
}

// Get retrieves the value associated with the given key. The boolean
// return value indicates whether the key was found.
func (table *HashTable[K, V]) Get(key K) (V, bool) {
	// Guard against a nil table reference.
	if table == nil {
		panic("table is nil")
	}

	// Compute the initial probe index for the key.
	index := table.indexFor(key, len(table.entries))

	// Probe linearly through the table, bounded by the table size.
	for probe := 0; probe < len(table.entries); probe++ {
		entry := table.entries[index]

		// Encountering an empty slot terminates the search.
		if entry.state == empty {
			var zero V

			return zero, false
		}

		// If a matching occupied entry is found, return its value.
		if entry.state == occupied && entry.key == key {
			return entry.value, true
		}

		// Advance to the next index, wrapping around the table.
		index = (index + 1) % len(table.entries)
	}

	// If the full table was probed without finding the key, return not found.
	var zero V

	return zero, false
}

// Delete removes the entry associated with the given key from the hash
// table. The slot is marked as deleted to preserve probe-chain integrity.
// The return value indicates whether a matching key was found and removed.
func (table *HashTable[K, V]) Delete(key K) bool {
	// Guard against a nil table reference.
	if table == nil {
		panic("table is nil")
	}

	// Compute the initial probe index for the key.
	index := table.indexFor(key, len(table.entries))

	// Probe linearly through the table, bounded by the table size.
	for probe := 0; probe < len(table.entries); probe++ {
		entry := &table.entries[index]

		// Encountering an empty slot means the key does not exist.
		if entry.state == empty {
			return false
		}

		// If a matching occupied entry is found, mark it as deleted.
		if entry.state == occupied && entry.key == key {
			entry.state = deleted

			// Decrement the count of occupied entries.
			table.size--

			return true
		}

		// Advance to the next index, wrapping around the table.
		index = (index + 1) % len(table.entries)
	}

	// If the full table was probed without finding the key, deletion failed.
	return false
}
