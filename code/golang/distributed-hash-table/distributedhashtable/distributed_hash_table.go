// -----------------------------------------------------------------------------
// Package: distributedhashtable
// File: distributed_hash_table.go
//
// Description:
//
//	This file contains a thread-safe implementation of a Distributed Hash Table (DHT)
//	using Consistent Hashing. The implementation enables horizontal scaling by
//	mapping both data keys and server nodes onto a logical hash ring.
//
//	To ensure a balanced distribution of data and avoid "hotspots," the ring
//	utilizes "virtual nodes" (replicas). When nodes are added or removed, the
//	Consistent Hashing algorithm ensures that only a minimal fraction of keys
//	need to be remapped, maintaining system stability.
//
// Design Notes:
//   - Cryptographic Hashing: Uses SHA-256 truncated to 32-bit for ring positioning.
//   - Virtual Nodes: The 'replicas' field determines the number of points each
//     physical server occupies on the ring to improve load balancing.
//   - Thread Safety: Employs a sync.RWMutex to allow concurrent reads while
//     protecting against race conditions during node membership changes.
//   - Search Efficiency: Uses binary search (O(log N)) via sort.Search to
//     quickly locate the responsible node on the ring.
//   - Defensive Design: Validates inputs for empty strings and ensures
//     replica counts are valid to prevent division or logic errors.
//
// Intended Use:
//
//	This package is designed for building distributed systems such as load
//	balancers, distributed caches, or sharded databases where node membership
//	is dynamic and data persistence across scaling events is required.
//
// -----------------------------------------------------------------------------
package distributedhashtable

import (
	"crypto/sha256"
	"encoding/binary"
	"fmt"
	"sort"
	"sync"
)

// Ring represents a consistent hashing ring containing multiple server nodes.
type Ring struct {
	nodes    map[uint32]string
	sorted   []uint32
	replicas int
	sync.RWMutex
}

// NewRing creates and returns a new empty consistent hashing ring.
// The replicaCount determines how many virtual nodes each server will have.
func NewRing(replicaCount int) *Ring {
	// A ring must have at least one replica to function.
	if replicaCount <= 0 {
		replicaCount = 1
	}

	return &Ring{
		nodes:    make(map[uint32]string),
		sorted:   []uint32{},
		replicas: replicaCount,
	}
}

// hashFunction generates a 32-bit unsigned integer hash for a given string key.
// It utilizes SHA-256 for high entropy and a uniform distribution across the ring.
func hashFunction(key string) uint32 {
	sum := sha256.Sum256([]byte(key))

	// Convert the first 4 bytes of the SHA-256 sum into a uint32.
	return binary.BigEndian.Uint32(sum[:4])
}

// Add inserts a new server node into the ring.
// Each node is replicated according to the Ring's replica count.
// This operation runs in O(N log N) time due to sorting after insertion.
func (ring *Ring) Add(serverName string) {
	if serverName == "" {
		return
	}

	ring.Lock()
	defer ring.Unlock()

	// Check if the server already exists using its first virtual node.
	hash := hashFunction(fmt.Sprintf("%s-0", serverName))

	if _, exists := ring.nodes[hash]; exists {
		return
	}

	// Create virtual nodes for the server to ensure even distribution.
	for index := 0; index < ring.replicas; index++ {
		id := fmt.Sprintf("%s-%d", serverName, index)

		hashValue := hashFunction(id)

		ring.nodes[hashValue] = serverName
		ring.sorted = append(ring.sorted, hashValue)
	}

	// Maintain the ring's sorted invariant to allow for binary search.
	sort.Slice(ring.sorted, func(compare, against int) bool {
		return ring.sorted[compare] < ring.sorted[against]
	})
}

// Get locates the server node responsible for the provided item's key.
// It finds the first server node encountered moving clockwise on the ring.
// This operation runs in O(log N) time.
func (ring *Ring) Get(item string) (string, bool) {
	ring.RLock()
	defer ring.RUnlock()

	// If no servers are present, return failure.
	if len(ring.sorted) == 0 {
		return "", false
	}

	hashValue := hashFunction(item)

	// Search for the smallest server hash that is >= the item's hash.
	index := sort.Search(len(ring.sorted), func(index int) bool {
		return ring.sorted[index] >= hashValue
	})

	// If the hash is greater than the largest server hash, wrap around to the first server.
	if index == len(ring.sorted) {
		index = 0
	}

	node, exists := ring.nodes[ring.sorted[index]]

	return node, exists
}

// removeFromSorted is an internal helper that removes a specific hash from the sorted slice.
// It uses binary search to find the target hash in O(log N) time.
func (ring *Ring) removeFromSorted(hashValue uint32) {
	index := sort.Search(len(ring.sorted), func(i int) bool {
		return ring.sorted[i] >= hashValue
	})

	// Ensure the found index actually matches the target hash before removal.
	if index < len(ring.sorted) && ring.sorted[index] == hashValue {
		ring.sorted = append(ring.sorted[:index], ring.sorted[index+1:]...)
	}
}

// Delete removes a server node and all its virtual replicas from the ring.
// This operation runs in O(Replicas * log N) time.
func (ring *Ring) Delete(server string) {
	if server == "" {
		return
	}

	ring.Lock()
	defer ring.Unlock()

	for index := 0; index < ring.replicas; index++ {
		hashValue := hashFunction(fmt.Sprintf("%s-%d", server, index))

		// Clean up both the node mapping and the search ring.
		delete(ring.nodes, hashValue)
		ring.removeFromSorted(hashValue)
	}
}

// Update replaces an existing server name with a new one.
// This effectively rebalances all keys previously belonging to the old node.
func (ring *Ring) Update(oldName, newName string) {
	// Re-utilizes existing logic to ensure consistent behavior.
	ring.Delete(oldName)
	ring.Add(newName)
}
