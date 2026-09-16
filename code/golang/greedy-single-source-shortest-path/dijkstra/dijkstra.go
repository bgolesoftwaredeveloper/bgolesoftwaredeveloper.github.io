// -----------------------------------------------------------------------------
// Package: dijkstra
// File: dijkstra.go
//
// Description:
//
//	This file provides a defensive and readable implementation of Dijkstra’s
//	shortest-path algorithm for directed, weighted graphs.
//
//	The graph is represented using an adjacency list, and shortest paths
//	are computed from a single source vertex to all reachable vertices.
//	A min-priority queue is used to always expand the closest unvisited
//	vertex next.
//
// Notes:
//   - Edge weights must be non-negative.
//   - Graph vertices are identified by integers.
//   - Unreachable vertices will retain an infinite distance.
//   - Defensive guards exist for nil graphs and missing vertices.
//
// -----------------------------------------------------------------------------
package dijkstra

import (
	"container/heap"
	"math"
)

// Edge represents a directed weighted edge from one vertex to another.
// The destination field identifies the target vertex, and weight
// specifies the cost of traversing the edge.
type Edge struct {
	destination int
	weight      int
}

// Graph represents a directed graph using an adjacency list.
// Each vertex maps to a slice of outgoing edges.
type Graph struct {
	adjacencyLists map[int][]Edge
}

// NewGraph initializes and returns an empty graph with
// an allocated adjacency list map.
func NewGraph() *Graph {
	return &Graph{
		adjacencyLists: make(map[int][]Edge),
	}
}

// AddDirectedEdge inserts a directed edge into the graph from
// the source vertex to the destination vertex.
//
// Defensive behavior:
//   - Safely ignores calls on a nil graph.
//   - Rejects edges with negative weights.
//   - Initializes adjacency storage if it is missing.
func (graph *Graph) AddDirectedEdge(source, destination, weight int) {
	// Guard against nil graph usage.
	if graph == nil || weight < 0 {
		return
	}

	// Lazily initialize adjacency storage if required.
	if graph.adjacencyLists == nil {
		graph.adjacencyLists = make(map[int][]Edge)
	}

	// Ensure source vertex exists.
	if _, exists := graph.adjacencyLists[source]; !exists {
		graph.adjacencyLists[source] = []Edge{}
	}

	// Ensure destination vertex exists (even if it has no outgoing edges).
	if _, exists := graph.adjacencyLists[destination]; !exists {
		graph.adjacencyLists[destination] = []Edge{}
	}

	graph.adjacencyLists[source] = append(
		graph.adjacencyLists[source], Edge{
			destination: destination,
			weight:      weight,
		},
	)
}

// priorityQueueItem represents a single entry in the priority queue.
// Each item stores a vertex and its currently known shortest distance.
type priorityQueueItem struct {
	vertex   int
	distance int
}

// minimumPriorityQueue implements heap.Interface and represents
// a min-heap ordered by shortest distance.
type minimumPriorityQueue []priorityQueueItem

// Len returns the number of elements in the queue.
func (queue minimumPriorityQueue) Len() int {
	return len(queue)
}

// Less reports whether the element at leftIndex should be ordered
// before the element at rightIndex.
// This establishes a min-heap based on distance.
func (queue minimumPriorityQueue) Less(leftIndex, rightIndex int) bool {
	return queue[leftIndex].distance < queue[rightIndex].distance
}

// Swap exchanges the elements at the given indices.
// This operation is required by heap.Interface to maintain heap order.
func (queue minimumPriorityQueue) Swap(source, destination int) {
	// Ignore swaps on empty or single-element queues.
	if len(queue) < 2 {
		return
	}

	// Guard against invalid index values.
	if source < 0 || destination < 0 {
		return
	}

	queueLength := len(queue)

	if source >= queueLength || destination >= queueLength {
		return
	}

	// No-op if both indices refer to the same element.
	if source == destination {
		return
	}

	queue[source], queue[destination] =
		queue[destination], queue[source]
}

// Push inserts a new element into the priority queue.
//
// Defensive behavior:
//   - Safely ignores calls on a nil queue.
//   - Ignores values of unexpected type.
func (queue *minimumPriorityQueue) Push(element interface{}) {
	// Safely ignore pushes to a nil queue.
	if queue == nil {
		return
	}

	// Accept only valid priority queue items.
	item, typeAssertionSucceeded := element.(priorityQueueItem)

	if !typeAssertionSucceeded {
		return
	}

	// Append the new item to the heap backing slice.
	*queue = append(*queue, item)
}

// Pop removes and returns the element with the smallest distance.
//
// Defensive behavior:
//   - Returns a zero-value item if the queue is nil or empty.
func (queue *minimumPriorityQueue) Pop() interface{} {
	// Safely handle nil or empty queues.
	if queue == nil || len(*queue) == 0 {
		return priorityQueueItem{}
	}

	// Snapshot the current heap contents.
	oldQueueContents := *queue

	// Compute the index of the last element.
	lastIndex := len(oldQueueContents) - 1

	// Remove the element placed at the end by the heap machinery.
	removedItem := oldQueueContents[lastIndex]

	// Shrink the slice to exclude the removed element.
	*queue = oldQueueContents[:lastIndex]

	// Return the extracted queue item.
	return removedItem
}

// Dijkstra computes the shortest distance from the given start vertex
// to all reachable vertices using Dijkstra’s algorithm.
//
// Defensive behavior:
//   - Returns nil if the graph or adjacency storage is nil.
//   - Returns nil if the start vertex does not exist in the graph.
func Dijkstra(graph *Graph, vertex int) map[int]int {
	// Validate graph structure.
	if graph == nil || graph.adjacencyLists == nil {
		return nil
	}

	// Ensure the start vertex exists in the graph.
	if _, exists := graph.adjacencyLists[vertex]; !exists {
		return nil
	}

	distances := make(map[int]int)

	// Initialize all known vertices with infinite distance.
	for graphVertex := range graph.adjacencyLists {
		distances[graphVertex] = math.MaxInt
	}

	// The distance from the start vertex to itself is zero.
	distances[vertex] = 0

	queue := &minimumPriorityQueue{}
	heap.Init(queue)

	// Seed the priority queue with the start vertex.
	heap.Push(queue, priorityQueueItem{
		vertex:   vertex,
		distance: 0,
	})

	// Process vertices in increasing distance order.
	for queue.Len() > 0 {
		item := heap.Pop(queue).(priorityQueueItem)

		currentVertex := item.vertex
		currentDistance := item.distance

		// Skip stale queue entries.
		if currentDistance > distances[currentVertex] {
			continue
		}

		// Relax outgoing edges.
		for _, edge := range graph.adjacencyLists[currentVertex] {
			newDistance := currentDistance + edge.weight

			// Relax all outgoing edges from the current vertex.
			if newDistance < distances[edge.destination] {
				distances[edge.destination] = newDistance

				heap.Push(queue, priorityQueueItem{
					vertex:   edge.destination,
					distance: newDistance,
				})
			}
		}
	}

	return distances
}
