// -----------------------------------------------------------------------------
// Package: lrucache
// File: eviction_controller.go
//
// Description:
//
//	This file contains the eviction controller used by the LRU cache.
//	The eviction controller is responsible for enforcing the configured
//	maximum capacity by evicting entries according to Least Recently Used
//	semantics.
//
//	The controller operates over two internal data structures:
//	  1. A hash map for constant-time key lookup and deletion.
//	  2. A doubly linked recency list for tracking usage order.
//
//	The eviction logic is fully defensive and never panics. All invalid or
//	degenerate states (nil receivers, disabled capacity, uninitialized
//	components, or corrupted internal state) result in safe no-op behavior.
//
// Design Notes:
//   - Eviction is triggered only when capacity is strictly exceeded.
//   - A maximumCapacity value less than or equal to zero disables eviction.
//   - The tail of the recency list always represents the least recently used entry.
//   - Map and list consistency is verified before deletion.
//   - All operations are expected to be externally synchronized by the parent cache.
//
// Intended Use:
//
//	This controller is an internal implementation detail of the LRU cache
//	and is not intended for direct use by consumers of the package.
//
// -----------------------------------------------------------------------------
package lrucache

// evictionController coordinates capacity enforcement for the LRU cache.
// It manages eviction by removing the least recently used entry when the
// number of stored items exceeds the configured maximum capacity.
type evictionController struct {
	maximumCapacity int
	recencyList     *recencyList
	entriesByKey    map[string]*cacheEntry
}

// evictIfNecessary removes the least recently used cache entry if the
// current number of entries exceeds the configured maximum capacity.
//
// This method is fully defensive and guarantees:
//   - Safe execution under nil receivers or partially initialized state.
//   - No panics under map or list desynchronization.
//   - No effect when eviction is disabled or unnecessary.
//
// This operation runs in constant time, O(1).
func (controller *evictionController) evictIfNecessary() {
	// Ensure the controller itself is valid before proceeding.
	if controller == nil {
		return
	}

	// Treat non-positive capacity as eviction disabled.
	if controller.maximumCapacity <= 0 {
		return
	}

	// Ensure required internal structures are initialized.
	if controller.entriesByKey == nil || controller.recencyList == nil {
		return
	}

	// If the number of entries does not exceed capacity, no eviction is required.
	if len(controller.entriesByKey) <= controller.maximumCapacity {
		return
	}

	// Remove the least recently used entry from the recency list.
	// This corresponds to the tail of the list.
	leastRecentlyUsedNode := controller.recencyList.removeLeastRecentlyUsed()

	if leastRecentlyUsedNode == nil {
		return
	}

	// Ensure the evicted node contains a valid key.
	// An empty key indicates corrupted or invalid state.
	if leastRecentlyUsedNode.key == "" {
		return
	}

	// Verify that the key is still present in the map.
	// This protects against double-eviction or map/list desynchronization.
	if _, exists := controller.entriesByKey[leastRecentlyUsedNode.key]; !exists {
		return
	}

	// Remove the entry from the map, completing the eviction.
	delete(controller.entriesByKey, leastRecentlyUsedNode.key)
}
