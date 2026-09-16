// -----------------------------------------------------------------------------
// Package: lrucache
// File: node.go
//
// Description:
//
//	This file defines the fundamental storage unit for the LRU cache.
//
//	The cacheEntry struct acts as a container for the cached data and
//	simultaneously serves as a node within the doubly linked recency list.
//	This dual-purpose design allows for O(1) transitions between the
//	hash map lookup and the list-based reordering logic.
//
// -----------------------------------------------------------------------------
package lrucache

// cacheEntryNode represents a single entry in the LRU cache.
// It participates in a doubly linked list to track recency of access.
type cacheEntry struct {
	key   string
	value any

	previous *cacheEntry
	next     *cacheEntry
}
