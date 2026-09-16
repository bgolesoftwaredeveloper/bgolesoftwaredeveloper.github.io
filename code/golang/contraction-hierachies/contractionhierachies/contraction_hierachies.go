// ---------------------------------------------------------------------------
// Package: contractionhierachies
// File:    contraction_hierarchy.go
//
// Description:
//
//	This file implements a simplified Contraction Hierarchy (CH) structure
//	for accelerated graph routing. It demonstrates the "preprocessing"
//	phase where nodes are "contracted" to create shortcuts.
//
//	Key Features:
//	- Graph representation using Adjacency Lists.
//	- Node Contraction logic to preserve shortest paths.
//	- Shortcut generation to skip over unimportant nodes.
//	- Support for weighted directed edges (representing distance or time).
//
// Usage:
//
//  1. Create a Graph and add your nodes/edges.
//  2. Order nodes by "importance" (simplified here as an index).
//  3. Call ContractNode to pre-process the graph and generate shortcuts.
//  4. Use the resulting augmented graph for bi-directional Dijkstra searches.
//
// -----------------------------------------------------------------------------
package contractionhierachies

import (
	"errors"
	"fmt"
)

// Edge represents a directed connection between two nodes, carrying a cost
// weight and a metadata flag identifying it as a standard road or a shortcut.
type Edge struct {
	Source      int
	Destination int
	Weight      float64
	IsShortcut  bool
}

// Graph manages the network topology using dual adjacency lists to allow
// efficient traversal in both forward and reverse directions.
type Graph struct {
	forwardEdges  map[int][]Edge
	backwardEdges map[int][]Edge
	count         int
}

// addShortcut is an internal helper that persists a generated shortcut into
// both adjacency structures.
func (graph *Graph) addShortcut(shortcut Edge) error {
	// Defensive check for nil receiver.
	if graph == nil {
		return errors.New("nil graph receiver")
	}

	graph.forwardEdges[shortcut.Source] = append(graph.forwardEdges[shortcut.Source], shortcut)
	graph.backwardEdges[shortcut.Destination] = append(graph.backwardEdges[shortcut.Destination], shortcut)

	return nil
}

// NewGraph initializes a Graph with the specified node capacity and
// prepares the internal map structures.
func NewGraph(numberOfNodes int) (*Graph, error) {
	// Guard against non-positive node count.
	if numberOfNodes <= 0 {
		return nil, errors.New("graph must contain at least one node")
	}

	return &Graph{
		forwardEdges:  make(map[int][]Edge),
		backwardEdges: make(map[int][]Edge),
		count:         numberOfNodes,
	}, nil
}

// AddDirectedEdge inserts a standard directed road into the graph, ensuring
// the weight is non-negative and indices are within valid bounds.
func (graph *Graph) AddDirectedEdge(from, to int, weight float64) error {
	// Defensive check for nil receiver.
	if graph == nil {
		return errors.New("cannot add edge to a nil graph")
	}

	// Validate node indices against the graph's fixed count.
	if from < 0 || to < 0 || from >= graph.count || to >= graph.count {
		return errors.New("node index out of graph bounds")
	}

	// Ensure weights are mathematically valid for routing.
	if weight < 0 {
		return errors.New("edge weight cannot be negative")
	}

	newEdge := Edge{
		Source:      from,
		Destination: to,
		Weight:      weight,
		IsShortcut:  false,
	}

	// Update both directional adjacency lists.
	graph.forwardEdges[from] = append(graph.forwardEdges[from], newEdge)
	graph.backwardEdges[to] = append(graph.backwardEdges[to], newEdge)

	return nil
}

// RetrieveEdges provides a public accessor to a specific node's outgoing edges.
func (graph *Graph) RetrieveEdges(id int) ([]Edge, error) {
	// Defensive check for nil receiver
	if graph == nil {
		return nil, errors.New("nil graph")
	}

	edges, exists := graph.forwardEdges[id]

	if !exists {
		return []Edge{}, nil
	}

	return edges, nil
}

// Contract effectively "removes" a node from the search space by connecting
// all its predecessors to all its successors via shortcut edges.
func (graph *Graph) Contract(index int) ([]Edge, error) {
	// Defensive check for nil receiver.
	if graph == nil {
		return nil, errors.New("cannot contract node on a nil graph")
	}

	// Verify the node exists within the hierarchy.
	if index < 0 || index >= graph.count {
		return nil, errors.New("invalid node index for contraction")
	}

	generatedShortcuts := make([]Edge, 0)

	// Identify neighbors that point to the node and nodes it points to.
	incomingEdges := graph.backwardEdges[index]
	outgoingEdges := graph.forwardEdges[index]

	// Iterate through all possible path combinations through this node.
	for _, inEdge := range incomingEdges {
		for _, outEdge := range outgoingEdges {
			// Skip cases where the source and destination are the same (self-loop).
			if inEdge.Source == outEdge.Destination {
				continue
			}

			// Calculate shortcut weight as the sum of the two segment weights.
			shortcutWeight := inEdge.Weight + outEdge.Weight

			shortcut := Edge{
				Source:      inEdge.Source,
				Destination: outEdge.Destination,
				Weight:      shortcutWeight,
				IsShortcut:  true,
			}

			// Persist the shortcut in the graph hierarchy.
			err := graph.addShortcut(shortcut)

			if err != nil {
				return nil, err
			}

			generatedShortcuts = append(generatedShortcuts, shortcut)
		}
	}

	return generatedShortcuts, nil
}

// String generates a formatted summary of the graph's current hierarchy,
// distinguishing between standard roads and generated shortcuts.
func (graph *Graph) String() string {
	// Defensive check for nil receiver.
	if graph == nil {
		return "Graph: <nil>"
	}

	output := fmt.Sprintf("Graph Hierachy (%d nodes):\n", graph.count)

	for index := 0; index < graph.count; index++ {
		edges := graph.forwardEdges[index]

		for _, edge := range edges {
			typeLabel := "Standard"

			if edge.IsShortcut {
				typeLabel = "Shortcut"
			}

			output += fmt.Sprintf("\t[%s] %d -> %d (Weight: %.2f)\n", typeLabel,
				edge.Source, edge.Destination, edge.Weight)
		}
	}

	return output
}
