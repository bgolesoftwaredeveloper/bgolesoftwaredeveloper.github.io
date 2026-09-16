// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file demonstrates usage of the lrucache package through a
//	realistic user session caching example.
//
//	The example models authenticated application sessions using
//	distinct, non-generic session identifiers and shows how an LRU
//	cache behaves under normal access patterns, including:
//
//	  - Session insertion
//	  - Recency updates on access
//	  - Automatic eviction when capacity is exceeded
//	  - Explicit deletion and removal
//
//	The cache capacity is intentionally small to make eviction behavior
//	observable and easy to reason about.
//
// Design Notes:
//   - Sessions are identified by unique, token-like string keys.
//   - Cached values are full domain objects, not primitives.
//   - Access patterns intentionally manipulate recency ordering.
//   - Output is printed to standard output for clarity.
//
// Intended Use:
//
//	This file is intended as a reference example for developers learning
//	or validating the behavior of the LRU cache implementation. It may
//	be adapted for testing, benchmarking, or integration demos.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"
	"time"

	"github.com/bgolesoftwaredeveloper/lru-cache/lrucache"
)

// userSession represents an authenticated application session.
type userSession struct {
	UserID       string
	EmailAddress string
	IssuedAt     time.Time
	LastAccessed time.Time
}

func main() {
	fmt.Println("=== LRU Cache Example: User Session Cache ===")

	// ---------------------------------------------------------------------
	// Create a cache that holds active user sessions.
	// Capacity is intentionally small to demonstrate eviction behavior.
	// ---------------------------------------------------------------------
	sessionCache := lrucache.NewLRUCache(2)
	fmt.Println("Session cache initialized with capacity 2")

	// ---------------------------------------------------------------------
	// Create distinct, realistic session identifiers.
	// These resemble actual session or authentication tokens.
	// ---------------------------------------------------------------------
	sessionNorthstar := userSession{
		UserID:       "sess-9f3a1c2b-northstar",
		EmailAddress: "northstar@acmecloud.io",
		IssuedAt:     time.Now(),
		LastAccessed: time.Now(),
	}

	sessionIronclad := userSession{
		UserID:       "sess-a72d84ef-ironclad",
		EmailAddress: "ironclad@acmecloud.io",
		IssuedAt:     time.Now(),
		LastAccessed: time.Now(),
	}

	sessionBlueNova := userSession{
		UserID:       "sess-4e91c0ad-bluenova",
		EmailAddress: "bluenova@acmecloud.io",
		IssuedAt:     time.Now(),
		LastAccessed: time.Now(),
	}

	// ---------------------------------------------------------------------
	// Insert sessions into the cache.
	// ---------------------------------------------------------------------
	sessionCache.Put(sessionNorthstar.UserID, sessionNorthstar)
	sessionCache.Put(sessionIronclad.UserID, sessionIronclad)

	fmt.Println("Inserted sessions: Northstar and Ironclad")
	fmt.Println("Cache size:", sessionCache.Length())

	// ---------------------------------------------------------------------
	// Access the Northstar session to update recency.
	// ---------------------------------------------------------------------
	cachedSession, exists := sessionCache.Get(sessionNorthstar.UserID)

	if exists {
		session := cachedSession.(userSession)
		session.LastAccessed = time.Now()

		sessionCache.Put(session.UserID, session)

		fmt.Println("Accessed session: Northstar (recency updated)")
	}

	// ---------------------------------------------------------------------
	// Insert a third session to trigger eviction.
	// Ironclad should be evicted as the least recently used.
	// ---------------------------------------------------------------------
	sessionCache.Put(sessionBlueNova.UserID, sessionBlueNova)

	fmt.Println("Inserted session: BlueNova (LRU eviction expected)")

	_, exists = sessionCache.Get(sessionIronclad.UserID)

	fmt.Printf("Ironclad session exists after eviction: %v\n", exists)

	// ---------------------------------------------------------------------
	// Explicitly delete the Northstar session.
	// ---------------------------------------------------------------------
	deleted := sessionCache.Delete(sessionNorthstar.UserID)

	fmt.Printf("Deleted Northstar session: %v\n", deleted)

	// ---------------------------------------------------------------------
	// Remove the BlueNova session.
	// ---------------------------------------------------------------------
	removed := sessionCache.Remove(sessionBlueNova.UserID)

	fmt.Printf("Removed BlueNova session: %v\n", removed)

	// ---------------------------------------------------------------------
	// Final state validation.
	// ---------------------------------------------------------------------
	fmt.Println("Final cache size:", sessionCache.Length())

	fmt.Println("=== End of Example ===")
}
