// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file demonstrates the usage of the trie package through a simple
//	command-line program. The example exercises all major trie operations,
//	including insertion, lookup, prefix queries, enumeration, deletion,
//	and word count tracking.
//
//	The purpose of this file is educational: to show how the trie behaves
//	as words are added, queried, and removed, and to illustrate the expected
//	control flow when integrating the trie into an application.
//
// Notes:
//   - This example uses hard-coded sample data for clarity.
//   - Output is printed directly to standard output.
//   - No concurrency or synchronization is involved.
//   - Error handling is implicit through return values.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/edward-fredkin/trie"
)

func main() {
	// -------------------------------------------------------------------------
	// Create a new empty trie instance.
	// The trie starts with only a root node and contains no words.
	// -------------------------------------------------------------------------
	wordTrie := trie.NewTrie()

	fmt.Println("Initial word count:", wordTrie.WordCount())
	fmt.Println()

	// -------------------------------------------------------------------------
	// Insert a collection of words into the trie.
	// These words share common prefixes to demonstrate prefix behavior.
	// -------------------------------------------------------------------------
	wordsToInsert := []string{
		"cat",
		"car",
		"cart",
		"carbon",
		"dog",
		"dot",
	}

	for _, word := range wordsToInsert {
		wordTrie.Insert(word)

		fmt.Println("\tInserted word:", word)
	}

	fmt.Println("Word count after insertion:", wordTrie.WordCount())

	// -------------------------------------------------------------------------
	// Perform exact word lookups.
	// These checks distinguish between full words and partial prefixes.
	// -------------------------------------------------------------------------
	fmt.Println("\nExact word lookups:")

	testWords := []string{
		"cat",
		"car",
		"cap",
		"carbon",
		"do",
	}

	for _, word := range testWords {
		exists := wordTrie.ContainsWord(word)

		fmt.Printf("\tContainsWord(%q): %v\n", word, exists)
	}

	// -------------------------------------------------------------------------
	// Perform prefix existence checks.
	// These checks only require that the prefix path exists in the trie.
	// -------------------------------------------------------------------------
	fmt.Println("\nPrefix existence checks:")

	testPrefixes := []string{
		"ca",
		"car",
		"cart",
		"do",
		"z",
	}

	for _, prefix := range testPrefixes {
		exists := wordTrie.ContainsPrefix(prefix)

		fmt.Printf("\tContainsPrefix(%q): %v\n", prefix, exists)
	}

	// -------------------------------------------------------------------------
	// Retrieve all words that share a common prefix.
	// This demonstrates the trie's ability to act as an autocomplete structure.
	// -------------------------------------------------------------------------
	fmt.Println("\nWords with prefix \"car\":")

	wordsWithPrefix := wordTrie.GetWordsWithPrefix("car")

	for _, word := range wordsWithPrefix {
		fmt.Println(" ", word)
	}

	// -------------------------------------------------------------------------
	// Enumerate all words currently stored in the trie.
	// The order of returned words is not guaranteed.
	// -------------------------------------------------------------------------
	fmt.Println("\nAll words in trie:")

	allWords := wordTrie.AllWords()

	for _, word := range allWords {
		fmt.Println(" ", word)
	}

	// -------------------------------------------------------------------------
	// Delete a word from the trie.
	// This operation removes only the specified word and prunes unused nodes.
	// -------------------------------------------------------------------------
	wordToDelete := "cart"
	deleted := wordTrie.Delete(wordToDelete)

	fmt.Printf("\nDeleted word %q: %v\n", wordToDelete, deleted)
	fmt.Println("Word count after deletion:", wordTrie.WordCount())

	// -------------------------------------------------------------------------
	// Verify that deletion affected only the target word.
	// Shared prefixes should remain intact.
	// -------------------------------------------------------------------------
	fmt.Println("\nPost-deletion checks:")
	fmt.Println("\tContainsWord(\"cart\"):", wordTrie.ContainsWord("cart"))
	fmt.Println("\tContainsWord(\"car\"):", wordTrie.ContainsWord("car"))
	fmt.Println("\tContainsPrefix(\"car\"):", wordTrie.ContainsPrefix("car"))

	// -------------------------------------------------------------------------
	// Final enumeration to confirm the trie's internal state.
	// -------------------------------------------------------------------------
	fmt.Println("\nFinal words in trie:")

	finalWords := wordTrie.AllWords()

	for _, word := range finalWords {
		fmt.Println(" ", word)
	}
}
