/*
File            :   Point.java
Project         :   DBSCAN
Programmer      :   Braiden Gole
First version   :   2026-07-29
Description     :   Represents a multi-dimensional spatial point holding array-based 
                    coordinates and cluster membership status for DBSCAN.
*/
package com.dbscan;

import java.util.Arrays;

/**
 * The Point class models a point in Euclidean space, storing its spatial 
 * coordinates along with its assigned cluster ID (e.g., UNVISITED, NOISE, 
 * or a specific cluster number).
 */
public class Point {
    public static final int UNVISITED = -2;
    public static final int NOISE = -1;

    private final double[] _coordinates;
    private int _clusterId = UNVISITED;

    /**
     * Constructs a Point with specified multi-dimensional coordinates and sets 
     * its initial state to UNVISITED.
     *
     * @param coordinates Array of numerical values representing position in space.
     */
    public Point(double[] coordinates) {
        this._coordinates = coordinates;
        this._clusterId = UNVISITED;
    }

    /**
     * Retrieves the current cluster identification number of the point.
     *
     * @return The cluster ID (e.g., -2 for UNVISITED, -1 for NOISE, or >= 0 for a valid cluster).
     */
    public int getClusterId() {
        return _clusterId;
    }

    /**
     * Retrieves the coordinate array of the point.
     *
     * @return Array of double values representing the spatial coordinates.
     */
    public double[] getCoordinates() {
        return _coordinates;
    }

    /**
     * Sets the cluster ID for this point.
     *
     * @param id The new cluster identifier to assign.
     */
    public void setClusterId(int id) {
        _clusterId = id;
    }

    /**
     * Calculates the standard Euclidean distance between this point and another point.
     *
     * @param other The target Point to calculate distance to.
     * @return The straight-line distance as a double value.
     */
    public double distance(Point other) {
        double sum = 0.0;

        for (int index = 0; index < _coordinates.length; index++) {
            double difference = _coordinates[index] - other._coordinates[index];

            sum += difference * difference;
        }

        return Math.sqrt(sum);
    }

    /**
     * Formats the point's coordinates and cluster assignment as a readable string.
     *
     * @return String representation of coordinates and cluster state.
     */
    @Override
    public String toString() {
        return Arrays.toString(_coordinates) + "\t-> Cluster: " +
            (_clusterId == NOISE ? "Noise" : _clusterId);
    }
}