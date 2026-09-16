// -----------------------------------------------------------------------------
// Package: lrucache
// File: cache.go
//
// Description:
//
//	This file contains the primary implementation of a thread-safe
//	Least Recently Used (LRU) cache.
//
//	The cache provides constant-time insertion, lookup, deletion, and
//	eviction by combining two internal data structures:
//	  1. A hash map for O(1) key-based access.
//	  2. A doubly linked recency list for tracking access order.
//
//	Thread safety is enforced via a single mutex guarding all public
//	operations. The implementation is intentionally defensive and
//	never panics due to nil receivers, partial initialization, or
//	internal corruption.
//
// Design Notes:
//   - All public methods are safe to call concurrently.
//   - A maximumCapacity <= 0 disables storage entirely.
//   - The cache degrades gracefully under invalid usage.
//   - Eviction semantics strictly follow Least Recently Used ordering.
//   - Internal invariants are preserved even under misuse.
//
// Intended Use:
//
//	This package is suitable for use as a general-purpose in-memory
//	cache in concurrent Go programs and as a reference implementation
//	for LRU eviction strategies.
//
// -----------------------------------------------------------------------------
package lrucache

import "sync"

// LRUCache represents a fixed-capacity, thread-safe LRU cache.
// All access to internal state is serialized via the embedded mutex.
type LRUCache struct {
	evictionController *evictionController
	mutex              sync.Mutex
}

// Length returns the number of entries currently stored in the cache.
// This method is defensive:
//   - A nil cache or uninitialized controller yields a length of zero.
//   - The internal map is validated before use.
//
// This operation runs in constant time.
func (cache *LRUCache) Length() int {
	// Handle nil receiver or missing controller.
	if cache == nil || cache.evictionController == nil {
		return 0
	}

	// Acquire the cache mutex to protect shared state.
	cache.mutex.Lock()
	defer cache.mutex.Unlock()

	// Ensure the internal map exists.
	if cache.evictionController.entriesByKey == nil {
		return 0
	}

	// Return the number of cached entries.
	return len(cache.evictionController.entriesByKey)
}

// NewLRUCache creates and returns a new LRU cache with the specified capacity.
// If maximumCapacity is less than or equal to zero, the returned cache
// is valid but effectively disabled and will not store entries.
func NewLRUCache(maximumCapacity int) *LRUCache {
	// Handle disabled or invalid capacity by constructing
	// a safe, non-storing cache.
	if maximumCapacity <= 0 {
		return &LRUCache{
			evictionController: &evictionController{
				maximumCapacity: 0,
				recencyList:     newRecencyList(),
				entriesByKey:    make(map[string]*cacheEntry),
			},
		}
	}

	// Initialize the recency list used for LRU ordering.
	recencyList := newRecencyList()

	// Initialize the hash map used for O(1) key lookup.
	entriesByKey := make(map[string]*cacheEntry)

	// Construct the eviction controller coordinating both structures.
	evictionController := &evictionController{
		maximumCapacity: maximumCapacity,
		recencyList:     recencyList,
		entriesByKey:    entriesByKey,
	}

	// Return the fully initialized cache.
	return &LRUCache{
		evictionController: evictionController,
	}
}

// Put inserts or updates a cache entry associated with the given key.
// If the key already exists, its value is updated and the entry is
// promoted to most recently used status.
// If inserting a new entry causes the cache to exceed capacity,
// the least recently used entry is evicted.
// This operation runs in constant time.
func (cache *LRUCache) Put(key string, value any) {
	// Handle nil reveiver or controller.
	if cache == nil || cache.evictionController == nil {
		return
	}

	controller := cache.evictionController

	// Do not store entries when capacity is disabled.
	if controller.maximumCapacity == 0 {
		return
	}

	// Aquire the cache mutex to protect shared state.
	cache.mutex.Lock()
	defer cache.mutex.Unlock()

	// Ensure required internal structures exist.
	if controller.entriesByKey == nil || controller.recencyList == nil {
		return
	}

	// If the key already exists, update its value and recency.
	if existingNode, exists := controller.entriesByKey[key]; exists {
		if existingNode != nil {
			existingNode.value = value
			controller.recencyList.moveNodeToFront(existingNode)
		}

		return
	}

	// Create a new cache entry for the key/value pair.
	newNode := &cacheEntry{
		key:   key,
		value: value,
	}

	// Insert the entry into the map and recency list.
	controller.entriesByKey[key] = newNode
	controller.recencyList.addNodeToFront(newNode)

	// Enforce capacity constraints if necessary.
	controller.evictIfNecessary()
}

// Get retrieves the value associated with the given key.
// If the key exists, the entry is promoted to most recently used status
// and the stored value is returned along with true.
// If the key does not exist, or the cache is in an invalid state,
// the zero value and false are returned.
//
// This method is defensive:
//   - A nil cache or uninitialized controller yields a cache miss.
//   - Internal structures are validated before use.
//   - No panics occur due to partial initialization.
//
// This operation runs in constant time.
func (cache *LRUCache) Get(key string) (any, bool) {
	// Handle nil receiver or controller.
	if cache == nil || cache.evictionController == nil {
		return nil, false
	}

	// Acquire the cache mutex to protect shared state.
	cache.mutex.Lock()
	defer cache.mutex.Unlock()

	controller := cache.evictionController

	// Ensure required internal structures exist.
	if controller.entriesByKey == nil || controller.recencyList == nil {
		return nil, false
	}

	// Attempt to locate the cache entry.
	node, exists := cache.evictionController.entriesByKey[key]

	if !exists || node == nil {
		return nil, false
	}

	// Promote the accessed entry to most recently used.
	cache.evictionController.recencyList.moveNodeToFront(node)

	return node.value, true
}

// Delete removes the cache entry associated with the given key.
// If the key exists, the entry is removed from both the map and
// the recency list and true is returned.
// If the key does not exist, false is returned.
// This operation runs in constant time.
func (cache *LRUCache) Delete(key string) bool {
	// Handle nil receiver or controller.
	if cache == nil || cache.evictionController == nil {
		return false
	}

	// Acquire the cache mutex to protect shared state.
	cache.mutex.Lock()
	defer cache.mutex.Unlock()

	controller := cache.evictionController

	// Ensure required internal structures exist.
	if controller.entriesByKey == nil || controller.recencyList == nil {
		return false
	}

	// Attempt to locate the cache entry.
	node, exists := controller.entriesByKey[key]

	if !exists || node == nil {
		return false
	}

	// Remove the entry from the recency list and map.
	controller.recencyList.removeNode(node)
	delete(controller.entriesByKey, key)

	return true
}

// Remove is an alias for Delete.
// It exists to support alternate naming conventions.
func (cache *LRUCache) Remove(key string) bool {
	return cache.Delete(key)
}
