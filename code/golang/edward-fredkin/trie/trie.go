// -----------------------------------------------------------------------------
// Package: trie
// File: trie.go
//
// Description:
//
//	This file provides an explicit implementation of a Trie (prefix tree)
//	data structure optimized for string storage and prefix-based queries.
//	The trie operates on raw byte sequences derived from Go strings, making it
//	suitable for ASCII and general byte-oriented text processing.
//
//	The implementation supports insertion, exact word lookup, prefix existence
//	checks, safe deletion with node cleanup, and enumeration of stored words.
//	All operations preserve internal invariants and are defensively implemented
//	to avoid panics due to nil receivers or invalid input.
//
// Design Notes:
//   - Strings are treated as byte slices with no rune-level normalization.
//   - Each node maintains a dynamic mapping of outgoing edges.
//   - Word termination is explicitly tracked at leaf and internal nodes.
//   - The root node always exists for a valid trie instance.
//   - A running word count is maintained to allow constant-time size queries.
//   - Deletion operations prune unused nodes to prevent memory growth.
//   - Enumeration order is nondeterministic due to map iteration.
//
// Intended Use:
//
//	This package is intended for learning, demonstration, and practical use
//	of prefix-based data structures such as dictionaries, autocomplete engines,
//	token filters, and symbol tables.
//
// -----------------------------------------------------------------------------
package trie

// node represents a single node within the trie.
// Each node maintains a map of outgoing edges indexed by byte value
// and a flag indicating whether the node terminates a valid word.
type node struct {
	children       map[byte]*node
	terminatesWord bool
}

// Trie represents the root structure of the prefix tree.
// All inserted words are reachable from the root node.
type Trie struct {
	root  *node
	count int
}

// NewTrie creates and returns a new empty trie.
// The returned trie is initialized to a valid zero state.
func NewTrie() *Trie {
	return &Trie{
		root: &node{
			children:       make(map[byte]*node),
			terminatesWord: false,
		},
		count: 0,
	}
}

// WordCount returns the number of words currently stored in the trie.
// This method is safe to call on a nil receiver.
// This operation runs in constant time.
func (trie *Trie) WordCount() int {
	if trie == nil {
		return 0
	}

	return trie.count
}

// GetWordsWithPrefix returns all stored words that begin with the given prefix.
// If the prefix does not exist or the trie is nil, nil is returned.
// This operation runs in time proportional to the size of the matching subtree.
func (trie *Trie) GetWordsWithPrefix(prefix string) []string {
	// Reject invalid calls early.
	if trie == nil || len(prefix) == 0 {
		return nil
	}

	currentNode := trie.root
	prefixBytes := []byte(prefix)

	// Traverse the trie along the prefix path.
	for byteIndex := 0; byteIndex < len(prefixBytes); byteIndex++ {
		// Attempt to follow the edge for the current byte.
		nextNode, exists := currentNode.children[prefixBytes[byteIndex]]

		// If the path does not exist, no words share this prefix.
		if !exists {
			return nil
		}

		currentNode = nextNode
	}

	results := make([]string, 0)

	// Collect all words reachable from the prefix node.
	trie.collectWords(currentNode, prefixBytes, &results)

	return results
}

// collectWords performs a depth-first traversal starting at the given node
// and accumulates complete words by extending the provided prefix.
// This helper assumes the prefix already corresponds to the start node.
func (trie *Trie) collectWords(start *node, prefix []byte, accumulator *[]string) {
	// A nil node indicates an invalid traversal state.
	if start == nil {
		return
	}

	// If this node terminates a word, record the current prefix.
	if start.terminatesWord {
		*accumulator = append(*accumulator, string(prefix))
	}

	// Recursively explore all child edges.
	for childByte, childNode := range start.children {
		trie.collectWords(childNode, append(prefix, childByte), accumulator)
	}
}

// AllWords returns all words stored in the trie.
// If the trie is nil, nil is returned.
// This operation runs in time proportional to the total number of nodes.
func (trie *Trie) AllWords() []string {
	// A nil trie has no stored words.
	if trie == nil {
		return nil
	}

	results := make([]string, 0)

	// Start traversal from the root with an empty prefix.
	trie.collectWords(trie.root, []byte{}, &results)

	return results
}

// Insert inserts a word into the trie.
// Duplicate insertions are ignored.
// This method is safe to call on a nil receiver.
func (trie *Trie) Insert(word string) {
	// Reject invalid insertions.
	if trie == nil || len(word) == 0 {
		return
	}

	currentNode := trie.root
	wordBytes := []byte(word)

	// Walk the trie, creating nodes as necessary.
	for byteIndex := 0; byteIndex < len(wordBytes); byteIndex++ {
		currentByte := wordBytes[byteIndex]

		nextNode, exists := currentNode.children[currentByte]

		// Create a new node if the edge does not exist.
		if !exists {
			nextNode = &node{
				children:       make(map[byte]*node),
				terminatesWord: false,
			}

			currentNode.children[currentByte] = nextNode
		}

		currentNode = nextNode
	}

	// Mark the final node as a terminating word if not already set.
	if !currentNode.terminatesWord {
		currentNode.terminatesWord = true

		trie.count++
	}
}

// ContainsWord reports whether the trie contains the given word
// as a complete entry. This method is safe to call on a nil receiver.
func (trie *Trie) ContainsWord(word string) bool {
	// Reject invalid queries.
	if trie == nil || len(word) == 0 {
		return false
	}

	currentNode := trie.root
	wordBytes := []byte(word)

	// Traverse the trie along the word path.
	for byteIndex := 0; byteIndex < len(wordBytes); byteIndex++ {
		currentByte := wordBytes[byteIndex]

		nextNode, exists := currentNode.children[currentByte]

		// A missing edge indicates the word is not present.
		if !exists {
			return false
		}

		currentNode = nextNode
	}

	// The word exists only if the final node terminates a word.
	return currentNode.terminatesWord
}

// ContainsPrefix reports whether the trie contains the given prefix.
// The prefix does not need to terminate a complete word.
// This method is safe to call on a nil receiver.
func (trie *Trie) ContainsPrefix(prefix string) bool {
	// Reject invalid queries.
	if trie == nil || len(prefix) == 0 {
		return false
	}

	currentNode := trie.root
	prefixBytes := []byte(prefix)

	// Traverse the trie along the prefix path.
	for byteIndex := 0; byteIndex < len(prefixBytes); byteIndex++ {
		currentByte := prefixBytes[byteIndex]

		nextNode, exists := currentNode.children[currentByte]

		// A missing edge indicates the prefix does not exist.
		if !exists {
			return false
		}

		currentNode = nextNode
	}

	return true
}

// Delete removes a word from the trie.
// If the word does not exist, false is returned.
// Unused nodes are pruned to preserve memory.
// This method is safe to call on a nil receiver.
func (trie *Trie) Delete(word string) bool {
	// Reject invalid deletions.
	if trie == nil || len(word) == 0 {
		return false
	}

	// traversalFrame records parent links for backtracking.
	type traversalFrame struct {
		parent *node
		key    byte
	}

	traversalStack := make([]traversalFrame, 0)

	currentNode := trie.root
	wordBytes := []byte(word)

	// Traverse the word path while recording parent nodes.
	for byteIndex := 0; byteIndex < len(wordBytes); byteIndex++ {
		currentByte := wordBytes[byteIndex]

		nextNode, exists := currentNode.children[currentByte]

		// If the path breaks, the word does not exist.
		if !exists {
			return false
		}

		traversalStack = append(traversalStack, traversalFrame{
			parent: currentNode,
			key:    currentByte,
		})

		currentNode = nextNode
	}

	// Ensure the word actually terminates here.
	if !currentNode.terminatesWord {
		return false
	}

	// Remove the terminating marker and update the count.
	currentNode.terminatesWord = false
	trie.count--

	// Prune unused nodes from the bottom up.
	for stackIndex := len(traversalStack) - 1; stackIndex >= 0; stackIndex-- {
		frame := traversalStack[stackIndex]
		childNode := frame.parent.children[frame.key]

		// Stop pruning if the node is still needed.
		if childNode.terminatesWord || len(childNode.children) > 0 {
			break
		}

		delete(frame.parent.children, frame.key)
	}

	return true
}
