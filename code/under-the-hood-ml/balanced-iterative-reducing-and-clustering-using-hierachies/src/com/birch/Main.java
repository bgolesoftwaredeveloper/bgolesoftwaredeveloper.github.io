/*
File            :   Main.java
Project         :   BIRCH
Programmer      :   Braiden Gole
First version   :   2026-08-03
Description     :   Driver class for demonstrating the BIRCH (Balanced Iterative 
                    Reducing and Clustering using Hierarchies) algorithm. Builds a 
                    CF-Tree (Clustering Feature Tree) from input data stream and 
                    extracts sub-cluster statistics (point counts, centroids, radii).

Commands:
    javac -cp "lib/*:bin" -d bin $(find src -name "*.java")
    java -cp bin com.birch.Main

    java -jar lib/junit-platform-console-standalone-6.1.1.jar execute --class-path bin --select-package com.test.birch
*/
package com.birch;

import java.util.Arrays;
import java.util.List;

public class Main {
    public static void main(String[] args) {
        // Sample 2D dataset grouped around two distinct regions: (~1.0, 1.0) and (~10.0, 10.0).
        double[][] dataset = {
            {1.0, 1.0}, 
            {1.5, 1.2}, 
            {0.8, 1.1}, 
            {1.2, 0.9}, 
            {1.1, 1.3},

            {10.0, 10.0}, 
            {10.2, 9.8}, 
            {9.9, 10.5}, 
            {10.5, 10.1}, 
            {9.7, 9.9}
        };

        // Define the parameters to work with.
        int maximumNodeEntriesCapacity = 3;
        double maximumRadiusThreshold = 1.5;
        int featureDimension = 2;

        // Initialize the CF-Tree (Cluster Feature Tree).
        Tree clusteringTree = new Tree(maximumNodeEntriesCapacity, maximumRadiusThreshold, featureDimension);

        // Stream each data point into the CF-Tree to dynamically build/split clusters.
        for (double[] point : dataset) {
            clusteringTree.insert(point);
        }

        // Retrieve the condensed sub-clusters from the leaf entries of the CF-Tree.
        List<ClusterFeature> extractedSubclusters = clusteringTree.getSubclusters();

        // Output summary results and formatting.
        System.out.println("Extracted Sub-clusters (" + extractedSubclusters.size() + " total):");

        for (int index = 0; index < extractedSubclusters.size(); index++) {
            ClusterFeature current = extractedSubclusters.get(index);

            // Print formatted cluster entry details (rounding centroid double values to 2 decimal places).
            System.out.printf("\tSub-cluster %d -> Count: %d, Centroid: %s, Radius: %.4f%n", index + 1,
                current.pointCount,
                Arrays.stream(current.getCentroid()).mapToObj(value -> String.format("%.2f", value)).toList().toString(),
                current.getRadius()
            );
        }
    }
}
