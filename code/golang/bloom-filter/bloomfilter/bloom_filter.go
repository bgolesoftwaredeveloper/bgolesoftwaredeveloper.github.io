// ----------------------------------------------------------------------------
// Package: bloomfilter
// File: bloom_filter.go
//
// Description:
//
//	  This file provides a defensive implementation of a Counting Bloom Filter.
//	  Unlike a standard Bloom Filter, this version uses byte-sized counters
//	  (buckets) to allow for the removal of elements.
//
//	  The implementation is intentionally defensive:
//	- All exported methods are safe to call on a nil *BloomFilter receiver.
//	- Empty data slices are treated as invalid input and ignored.
//	- Internal math prevents division by zero and logarithmic errors.
//	- Counters are capped at 255 to prevent overflow/wrap-around.
//
//	  Data Structure:
//
//	  A slice of bytes where each byte acts as a counter. Multiple hash
//	  indices are derived from a single pair of 64-bit hashes using the
//	  Kirsch-Mitzenmacher optimization.
//
// Notes:
//   - The filter is probabilistic; false positives are possible, but false
//     negatives are not (provided the Remove operation is used correctly).
//   - Thread safety is managed via a sync.RWMutex, allowing concurrent reads.
//   - Memory usage is O(m) where m is the calculated number of buckets.
//
// -----------------------------------------------------------------------------
package bloomfilter

import (
	"hash/fnv"
	"math"
	"sync"
)

// BloomFilter represents a probabilistic, thread-safe counting bloom filter.
type BloomFilter struct {
	mutex        sync.RWMutex
	buckets      []byte
	totalBuckets uint64
	iterations   uint32
}

// NewOptimalBloomFilter calculates the necessary size and hash iterations based
// on the expected number of entries and a desired false positive rate.
func NewOptimalBloomFilter(expectedEntries uint64, falsePositiveRate float64) *BloomFilter {
	// Guard against non-positive entries or invalid rates.
	if expectedEntries == 0 || falsePositiveRate <= 0 {
		expectedEntries = 100
		falsePositiveRate = 0.01
	}

	// Calculate optimal bits (m) and convert to buckets.
	optimalBits := -float64(expectedEntries) * math.Log(falsePositiveRate) / math.Pow(math.Log(2), 2)
	numberOfBuckets := uint64(math.Ceil(optimalBits / 8))

	if numberOfBuckets == 0 {
		numberOfBuckets = 1
	}

	// Calculate optimal hash functions (k).
	optimalHashes := uint32(math.Round((float64(numberOfBuckets*8) / float64(expectedEntries)) * math.Log(2)))

	if optimalHashes == 0 {
		optimalHashes = 1
	}

	return &BloomFilter{
		buckets:      make([]byte, numberOfBuckets),
		totalBuckets: numberOfBuckets,
		iterations:   optimalHashes,
	}
}

// hashPair generates two 64-bit hashes using FNV-1 and FNV-1a.
// These are used to simulate multiple hash functions efficiently.
func (filter *BloomFilter) hashPair(data []byte) (uint64, uint64) {
	// Guard against nil receiver or empty data.
	if filter == nil || len(data) == 0 {
		return 0, 0
	}

	hasher := fnv.New64()
	hasher.Write(data)

	otherHasher := fnv.New64a()
	otherHasher.Write(data)

	return hasher.Sum64(), otherHasher.Sum64()
}

// noLock performs a membership check without acquiring a mutex.
// It is used internally by public methods to avoid deadlock scenarios.
func (filter *BloomFilter) noLock(data []byte) bool {
	// Guard against nil receiver, empty data, or uninitialized filter.
	if filter == nil || len(data) == 0 || filter.totalBuckets == 0 {
		return false
	}

	hasher, otherHasher := filter.hashPair(data)

	for index := uint32(0); index < filter.iterations; index++ {
		// Kirsch-Mitzenmacher optimization: Hash_i = h1 + i*h2.
		hashValue := hasher + uint64(index)*otherHasher
		bucketIndex := hashValue % filter.totalBuckets

		// If any bucket is zero, the data is definitely not in the set.
		if filter.buckets[bucketIndex] == 0 {
			return false
		}
	}

	return true
}

// Add inserts data into the filter by incrementing the associated counters.
func (filter *BloomFilter) Add(data []byte) {
	// Guard against nil receiver or empty data.
	if filter == nil || len(data) == 0 {
		return
	}

	filter.mutex.Lock()
	defer filter.mutex.Unlock()

	hasher, otherHasher := filter.hashPair(data)

	for index := uint32(0); index < filter.iterations; index++ {
		hashValue := hasher + uint64(index)*otherHasher
		bucketIndex := hashValue % filter.totalBuckets

		// Increment counter while preventing uint8 overflow (cap at 255).
		if filter.buckets[bucketIndex] < 255 {
			filter.buckets[bucketIndex]++
		}
	}
}

// Contains reports whether the data might be present in the filter.
func (filter *BloomFilter) Contains(data []byte) bool {
	// Guard against nil receiver or empty data.
	if filter == nil || len(data) == 0 {
		return false
	}

	filter.mutex.RLock()
	defer filter.mutex.RUnlock()

	return filter.noLock(data)
}

// Remove decrements the counters for the given data, effectively removing it.
// This is only safe if the data was previously added.
func (filter *BloomFilter) Remove(data []byte) {
	// Guard against nil receiver or empty data.
	if filter == nil || len(data) == 0 {
		return
	}

	filter.mutex.Lock()
	defer filter.mutex.Unlock()

	// Only attempt removal if the item appears to exist.
	if !filter.noLock(data) {
		return
	}

	hasher, otherHasher := filter.hashPair(data)

	for index := uint32(0); index < filter.iterations; index++ {
		hashValue := hasher + uint64(index)*otherHasher
		bucketIndex := hashValue % filter.totalBuckets

		// Decrement counter without allowing it to go below zero.
		if filter.buckets[bucketIndex] > 0 {
			filter.buckets[bucketIndex]--
		}
	}
}
