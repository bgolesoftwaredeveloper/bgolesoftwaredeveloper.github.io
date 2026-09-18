/*
File            :   ClusterFeature.java
Project         :   BIRCH
Programmer      :   Braiden Gole
First version   :   2026-08-03
Description     :   Represents a Clustering Feature (CF) vector defined as CF = (N, LS, SS).
                    Stores statistical summaries of data clusters (Count, Linear Sum, 
                    and Squared Sum) and provides mathematical operations for vector addition, 
                    centroid evaluation, radius calculation, and Euclidean distance.
*/
package com.birch;

import java.util.Arrays;

public class ClusterFeature {
    public int pointCount;
    public double[] linearSum;
    public double squaredSum;

    /**
     * Constructs an empty Cluster Feature vector initialized with zeros.
     *
     * @param featureDimension Number of feature dimensions
     */
    public ClusterFeature(int featureDimension) {
        this.pointCount = 0;
        this.linearSum = new double[featureDimension];
        this.squaredSum = 0.0;
    }

    /**
     * Constructs a Cluster Feature vector from a single data point.
     *
     * @param point The raw data point coordinates
     */
    public ClusterFeature(double[] point) {
        this.pointCount = 1;
        this.linearSum = Arrays.copyOf(point, point.length);
        this.squaredSum = 0.0;

        // Calculate initial squared sum.
        for (double coordinate : point) {
            this.squaredSum += coordinate * coordinate;
        }
    }

    /**
     * Copy constructor to create a deep copy of an existing Cluster Feature.
     *
     * @param template The Cluster Feature instance to copy
     */
    public ClusterFeature(ClusterFeature template) {
        this.pointCount = template.pointCount;
        this.linearSum = Arrays.copyOf(template.linearSum, template.linearSum.length);
        this.squaredSum = template.squaredSum;
    }

    /**
     * Merges another Cluster Feature into this one using the CF Additivity Theorem.
     *
     * @param feature The Cluster Feature vector to add
     */
    public void addClusterFeature(ClusterFeature feature) {
        this.pointCount += feature.pointCount;

        for (int index = 0; index < linearSum.length; index++) {
            this.linearSum[index] += feature.linearSum[index];
        }

        this.squaredSum += feature.squaredSum;
    }

    /**
     * Calculates the centroid (mean vector) of the cluster.
     *
     * @return Array containing the centroid coordinates
     */
    public double[] getCentroid() {
        double[] centroid = new double[linearSum.length];

        if (pointCount == 0) {
            return centroid;
        }

        for (int index = 0; index < linearSum.length; index++) {
            centroid[index] = linearSum[index] / pointCount;
        }

        return centroid;
    }

    /**
     * Calculates the Radius (R) of the cluster, representing the average distance
     * of member points to the centroid.
     *
     * @return The calculated radius value
     */
    public double getRadius() {
        if (pointCount <= 1) {
            return 0.0;
        }

        double[] centroid = getCentroid();
        double sumOfSquaredCentroidCoordinates = 0.0;

        for (double coordinate : centroid) {
            sumOfSquaredCentroidCoordinates += coordinate * coordinate;
        }

        // Variance calculation derived from SS, N, and centroid.
        double variance = (squaredSum / pointCount) - sumOfSquaredCentroidCoordinates;

        // Math.max prevents negative floating-point precision artifacts under the square root.
        return Math.sqrt(Math.max(0.0, variance));
    }

    /**
     * Calculates the Euclidean distance between this cluster's centroid and a target data point.
     *
     * @param target Raw coordinate array of target point
     * @return The calculated Euclidean distance
     */
    public double distanceToPoint(double[] target) {
        double[] centroid = getCentroid();
        double squaredDistanceSum = 0.0;

        for (int index = 0; index < target.length; index++) {
            double coordinateDifference = centroid[index] - target[index];

            squaredDistanceSum += coordinateDifference * coordinateDifference;
        }

        return Math.sqrt(squaredDistanceSum);
    }

    /**
     * Calculates the Euclidean distance between the centroids of two Cluster Features (D0 Distance).
     *
     * @param other Target Cluster Feature vector
     * @return The distance between cluster centroids
     */
    public double distanceToClusterFeature(ClusterFeature other) {
        return distanceToPoint(other.getCentroid());
    }
}
