/*
File            :   Tree.java
Project         :   BIRCH
Programmer      :   Braiden Gole
First version   :   2026-08-03
Description     :   Implements the CF-Tree (Clustering Feature Tree) data structure for 
                    the BIRCH clustering algorithm. Handles dynamic streaming data 
                    insertion, nearest-child routing, radius threshold verification, 
                    leaf/non-leaf node splitting, and sub-cluster traversal.
*/
package com.birch;

import java.util.ArrayList;
import java.util.List;

public class Tree {
    /**
     * Inner class representing an individual Node within the CF-Tree.
     * Nodes can either be internal routing nodes or leaf nodes storing sub-clusters.
     */
    public static class Node {
        public boolean isLeaf;
        public List<ClusterFeature> clusters;
        public List<Node> children;

        /**
         * Constructs a tree Node.
         *
         * @param isLeaf True if this node is a leaf; false if internal node
         */
        public Node(boolean isLeaf) {
            this.isLeaf = isLeaf;
            this.clusters = new ArrayList<>();
            
            if (!isLeaf) {
                this.children = new ArrayList<>();
            }
        }
    }

    private final int maximumNodeEntriesCapacity;
    private final double maximumRadiusThreshold;
    private final int featureDimension;

    private Node root;

    /**
     * Constructs a new CF-Tree with specified capacity and radius constraints.
     *
     * @param maximumNodeEntriesCapacity Maximum number of entries a node can hold before splitting
     * @param maximumRadiusThreshold     Maximum radius allowed for merging entries into a sub-cluster
     * @param featureDimension           Number of dimensions in data points
     */
    public Tree(int maximumNodeEntriesCapacity, double maximumRadiusThreshold, int featureDimension) {
        this.maximumNodeEntriesCapacity = maximumNodeEntriesCapacity;
        this.maximumRadiusThreshold = maximumRadiusThreshold;
        this.featureDimension = featureDimension;

        // Initialize with a single empty leaf root.
        this.root = new Node(true);
    }

    /**
     * Finds the index of the ClusterFeature in a list that is closest in Euclidean distance
     * to the target ClusterFeature.
     *
     * @param list   List of candidate ClusterFeatures
     * @param target ClusterFeature to calculate distances against
     * @return Index of the closest entry, or -1 if list is empty
     */
    private int findClosestEntryIndex(List<ClusterFeature> list, ClusterFeature target) {
        if (list.isEmpty()) {
            return -1;
        }

        int closestIndex = 0;
        double minimumDistance = list.get(0).distanceToClusterFeature(target);

        // Handle root split if propagation reached the top.
        for (int index = 1; index < list.size(); index++) {
            //
            double currentDistance = list.get(index).distanceToClusterFeature(target);

            if (currentDistance < minimumDistance) {
                minimumDistance = currentDistance;
                closestIndex = index;
            }
        }

        return closestIndex;
    }

    /**
     * Finds the pair of entries within a list that are furthest apart from each other.
     * Used as seeds when splitting an overflowing node.
     *
     * @param list List of ClusterFeatures to evaluate
     * @return 2-element array containing indices of the furthest pair
     */
    private int[] findFurthestEntryPairIndices(List<ClusterFeature> list) {
        double maximumDistance = -1.0;
        int[] seedPairIndices = new int[]{0, 1};

        // Pairwise comparison across all entries in the node.
        for (int outerIndex = 0; outerIndex < list.size(); outerIndex++) {
            for (int innerIndex = outerIndex + 1; innerIndex < list.size(); innerIndex++) {
                double calculatedDistance = list.get(outerIndex).distanceToClusterFeature(list.get(innerIndex));

                if (calculatedDistance > maximumDistance) {
                    maximumDistance = calculatedDistance;

                    seedPairIndices[0] = outerIndex;
                    seedPairIndices[1] = innerIndex;
                }
            }
        }

        return seedPairIndices;
    }

    /**
     * Splits an overflowing internal non-leaf node, redistributing its child entries
     * and pointers to the seed that is closest.
     *
     * @param nonLeaf Internal node that exceeded branching capacity
     * @return Newly created sibling internal node
     */
    private Node splitNonLeafNode(Node nonLeaf) {
        // Identify furthest entries to serve as split seeds.
        int[] seedPairIndices = findFurthestEntryPairIndices(nonLeaf.clusters);

        Node newNonLeaf = new Node(false);

        List<ClusterFeature> previousEntries = new ArrayList<>(nonLeaf.clusters);
        List<Node> previousChildren = new ArrayList<>(nonLeaf.children);

        // Clear original node entries and child pointers.
        nonLeaf.clusters.clear();
        nonLeaf.children.clear();

        // Assign seeds and corresponding child references.
        ClusterFeature primarySeed = previousEntries.get(seedPairIndices[0]);
        ClusterFeature secondarySeed = previousEntries.get(seedPairIndices[1]);

        nonLeaf.clusters.add(primarySeed);
        nonLeaf.children.add(previousChildren.get(seedPairIndices[0]));

        nonLeaf.clusters.add(secondarySeed);
        nonLeaf.children.add(previousChildren.get(seedPairIndices[1]));

        // Redistribute remaining entries and child pointers to nearest seed.
        for (int index = 0; index < previousEntries.size(); index++) {
            // Skip seeds.
            if (index == seedPairIndices[0] || index == seedPairIndices[1]) {
                continue;
            }

            ClusterFeature entry = previousEntries.get(index);
            Node child = previousChildren.get(index);

            if (entry.distanceToClusterFeature(primarySeed) < entry.distanceToClusterFeature(secondarySeed)) {
                nonLeaf.clusters.add(entry);
                nonLeaf.children.add(child);
            } else {
                newNonLeaf.clusters.add(entry);
                newNonLeaf.children.add(child);
            }
        }

        return newNonLeaf;
    }

    /**
     * Splits an overflowing leaf node into two separate leaf nodes by picking the 
     * two furthest entries as seeds and redistributing remaining entries to closest seed.
     *
     * @param leaf Leaf node that exceeded capacity
     * @return Newly created sibling leaf node
     */
    private Node splitLeafNode(Node leaf) {
        // Identify the two furthest entries to serve as split seeds.
        int[] seedPairIndices = findFurthestEntryPairIndices(leaf.clusters);

        Node newLeaf = new Node(true);
        List<ClusterFeature> previousEntries = new ArrayList<>(leaf.clusters);
        
        // Clear existing entries in original leaf node.
        leaf.clusters.clear();

        // Assign seed entries to separate leaf nodes.
        ClusterFeature primarySeed = previousEntries.get(seedPairIndices[0]);
        ClusterFeature secondarySeed = previousEntries.get(seedPairIndices[1]);

        leaf.clusters.add(primarySeed);
        newLeaf.clusters.add(secondarySeed);

        // Redistribute all remaining entries based on proximity to seeds.
        for (int index = 0; index < previousEntries.size(); index++) {
            // Skip seeds.
            if (index == seedPairIndices[0] || index == seedPairIndices[1]) {
                continue;
            }

            ClusterFeature entry = previousEntries.get(index);

            if (entry.distanceToClusterFeature(primarySeed) < entry.distanceToClusterFeature(secondarySeed)) {
                leaf.clusters.add(entry);
            } else {
                newLeaf.clusters.add(entry);
            }
        }

        return newLeaf;
    }

    /**
     * Depth-first recursive traversal to collect leaf node entries into a flattened list.
     *
     * @param current          Current node being traversed
     * @param accumulatedList  List accumulating sub-clusters
     */
    private void collectSubclustersRecursively(Node current, List<ClusterFeature> accumulatedList) {
        if (current.isLeaf) {
            accumulatedList.addAll(current.clusters);
        } else {
            for (Node child : current.children) {
                collectSubclustersRecursively(child, accumulatedList);
            }
        }
    }

    /**
     * Public method to retrieve all final sub-clusters extracted from leaf entries across the tree.
     *
     * @return List of all leaf ClusterFeatures
     */
    public List<ClusterFeature> getSubclusters() {
        List<ClusterFeature> accumulatedSubclusters = new ArrayList<>();

        collectSubclustersRecursively(root, accumulatedSubclusters);
        
        return accumulatedSubclusters;
    }

    /**
     * Recursive helper method that navigates down the CF-Tree to find the proper leaf,
     * attempts to merge or insert, and handles node splitting upwards.
     *
     * @param current Current node being visited
     * @param target  ClusterFeature being inserted
     * @return Newly created Node if a split occurred at this level; null otherwise
     */
    private Node insertRecursively(Node current, ClusterFeature target) {
        if (current.isLeaf) {
            // Find the closest entry in this leaf node to the target feature.
            int closestEntryIndex = findClosestEntryIndex(current.clusters, target);

            if (closestEntryIndex != -1) {
                // Create a hypothetical candidate feature representing the merged cluster.
                ClusterFeature mergedCandidate = new ClusterFeature(current.clusters.get(closestEntryIndex));

                mergedCandidate.addClusterFeature(target);

                // Check if merged cluster radius remains within the threshold.
                if (mergedCandidate.getRadius() <= maximumRadiusThreshold) {
                    // Update entry with merged result and return (no split).
                    current.clusters.set(closestEntryIndex, mergedCandidate);

                    return null;
                }
            }

            // Cannot merge within threshold; append as a new standalone leaf entry.
            current.clusters.add(target);

            // Check if leaf node exceeds entry capacity.
            if (current.clusters.size() > maximumNodeEntriesCapacity) {
                return splitLeafNode(current);
            }

            return null;
        } else {
            // Route to closest child entry in this internal node.
            int closestEntryIndex = findClosestEntryIndex(current.clusters, target);

            Node closestChild = current.children.get(closestEntryIndex);

            // Recurse deeper toward the leaf.
            Node splitChild = insertRecursively(closestChild, target);

            // Update routing summary vector for chosen path.
            current.clusters.get(closestEntryIndex).addClusterFeature(target);

            // Handle child split propagation.
            if (splitChild != null) {
                // Generate summary for the newly split child node.
                ClusterFeature newSummary = summarizeNodeEntries(splitChild);

                // Add split child as a new entry in this internal node.
                current.clusters.add(newSummary);
                current.children.add(splitChild);

                // Check if this internal node exceeds branching.
                if (current.clusters.size() > maximumNodeEntriesCapacity) {
                    return splitNonLeafNode(current);
                }
            }

            return null;
        }
    }

    /**
     * Combines all ClusterFeatures stored in a node into a single aggregated ClusterFeature.
     *
     * @param target Node whose entries need to be summarized
     * @return Aggregated ClusterFeature
     */
    private ClusterFeature summarizeNodeEntries(Node target) {
        ClusterFeature consolidated = new ClusterFeature(featureDimension);

        for (ClusterFeature entry : target.clusters) {
            consolidated.addClusterFeature(entry);
        }

        return consolidated;
    }

    /**
     * Public entry point to stream a new raw data point into the CF-Tree.
     *
     * @param point The raw data point coordinates to insert
     */
    public void insert(double[] point) {
        // Step 1: Wrap point into a single-point cluster feature.
        ClusterFeature pointClusteringFeature = new ClusterFeature(point);

        // Step 2: Recursively route and insert into tree structure.
        Node split = insertRecursively(root, pointClusteringFeature);

        // Step 3: Handle root split if propagation reached the top.
        if (split != null) {
            // Root split required: construct a new internal non-leaf root.
            Node newRoot = new Node(false);

            // Summarize the old root and the split sibling.
            ClusterFeature leftRootSummary = summarizeNodeEntries(root);
            ClusterFeature rightRootSummary = summarizeNodeEntries(split);

            // Attach old root as left child.
            newRoot.clusters.add(leftRootSummary);
            newRoot.children.add(root);

            // Attach split sibling as right child.
            newRoot.clusters.add(rightRootSummary);
            newRoot.children.add(split);

            // Update root reference.
            this.root = newRoot;
        }
    }
}
