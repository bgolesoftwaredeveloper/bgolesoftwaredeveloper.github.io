/*
File            :   Dbscan.java
Project         :   DBSCAN
Programmer      :   Braiden Gole
First version   :   2026-07-29
Description     :   Implements the Density-Based Spatial Clustering of Applications 
                    with Noise (DBSCAN) algorithm to group points based on spatial 
                    density and noise thresholding.
*/
package com.dbscan;

import java.util.ArrayList;
import java.util.List;

/**
 * The Dbscan class executes density-based clustering over a collection of spatial points.
 * Points are categorized into distinct clusters based on a distance radius (epsilon) 
 * and a density threshold (minimum points).
 */
public class Dbscan {
    private final double _epsilon;
    private final int _minimumPoints;

    /**
     * Constructs a Dbscan instance with specified density parameters.
     *
     * @param epsilon The maximum distance radius to consider points as neighbors.
     * @param minimumPoints The minimum number of neighbor points required to form a core cluster.
     */
    public Dbscan(double epsilon, int minimumPoints) {
        this._epsilon = epsilon;
        this._minimumPoints = minimumPoints;
    }

    /**
     * Identifies all points within the epsilon radius of a target point.
     *
     * @param target The point from which neighbor distances are measured.
     * @param points The full dataset of points to evaluate against.
     * @return A list of points that fall within the specified epsilon distance of the target.
     */
    private List<Point> getNeighbors(Point target, List<Point> points) {
        List<Point> neighbors = new ArrayList<>();

        // Iterate through all dataset points to calculate distance from target.
        for (Point point : points) {
            // Check if point falls within the max distance boundary.
            if (target.distance(point) <= _epsilon) {
                neighbors.add(point);
            }
        }

        return neighbors;
    }

    /**
     * Recursively expands a discovered cluster by traversing neighbor networks 
     * and assigning non-visited points or noise points to the active cluster.
     *
     * @param point The seed point initiating the cluster expansion.
     * @param neighbors The initial neighborhood list associated with the seed point.
     * @param allPoints The entire collection of points in the dataset.
     * @param clusterId The active cluster identifier to assign to valid points.
     */
    private void expandCluster(Point point, List<Point> neighbors, List<Point> allPoints, int clusterId) {
        // Assign the initiating core point directly to the current cluster.
        point.setClusterId(clusterId);

        // Create a dynamic work queue (seeds list) initialized with direct neighbors.
        List<Point> seeds = new ArrayList<>(neighbors);

        int index = 0;

        // Iterate through the seed queue sequentially using an index tracker.
        while (index < seeds.size()) {
            Point current = seeds.get(index);

            // Capture the state before potential mutation in this pass.
            int state = current.getClusterId();

            // Reassign border points previously flagged as NOISE.
            if (state == Point.NOISE) {
                current.setClusterId(clusterId);
            }

            // If unvisited, assign to cluster and expand reachable seed queue.
            if (state == Point.UNVISITED) {
                // Claim the unvisited point as part of the current cluster.
                current.setClusterId(clusterId);

                // Check the neighborhood of this newly visited point to see if it's also a core point.
                List<Point> currentNeighbors = getNeighbors(current, allPoints);

                // If it meets the density requirement minimum points, propagate its neighbors into the seed queue.
                if (currentNeighbors.size() >= _minimumPoints) {
                    for (Point neighbor : currentNeighbors) {
                        int neighborState = neighbor.getClusterId();

                        // Only consider adding points that are unvisited or previously tagged as noise.
                        if (neighborState == Point.UNVISITED || neighborState == Point.NOISE) {
                            if (!seeds.contains(neighbor)) {
                                seeds.add(neighbor);
                            }
                        }
                    }
                }
            }

            index++;
        }
    }

    /**
     * Runs the DBSCAN clustering algorithm across the provided dataset, 
     * mutating each Point's cluster state to a valid cluster ID or NOISE.
     *
     * @param points The dataset of Point objects to be partitioned into clusters.
     */
    public void cluster(List<Point> points) {
        if (points == null || points.isEmpty()) {
            return;
        }

        // Reset points state for idemptency.
        for (Point point : points) {
            point.setClusterId(Point.UNVISITED);
        }

        int clusterId = 0;

        // Iterate through all points in dataset to assign cluster groups.
        for (Point point : points) {
            // Skip points that have already been assigned or visited.
            if (point.getClusterId() != Point.UNVISITED) {
                continue;
            }

            // Find all points within radius of current unvisited point.
            List<Point> neighbors = getNeighbors(point, points);

            // Mark as NOISE provisionally if neighbor density is below minimum threshold.
            if (neighbors.size() < _minimumPoints) {
                point.setClusterId(Point.NOISE);
            } else {
                // Point meets density threshold; expand and assign new cluster ID.
                expandCluster(point, neighbors, points, clusterId);
                clusterId++;
            }
        }
    }
}