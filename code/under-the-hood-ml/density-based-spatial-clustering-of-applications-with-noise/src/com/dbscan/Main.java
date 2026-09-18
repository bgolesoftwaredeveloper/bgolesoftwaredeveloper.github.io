/*
File            :   Main.java
Project         :   DBSCAN
Programmer      :   Braiden Gole
First version   :   2026-07-29
Description     :   Driver class to demonstrate and verify the DBSCAN clustering 
                    algorithm using a sample multi-dimensional dataset.

Commands:
    javac -cp "lib/*:bin" -d bin $(find src -name "*.java")
    java -cp bin com.dbscan.Main

    java -jar lib/junit-platform-console-standalone-6.1.1.jar execute --class-path "bin:lib/*" --select-class com.test.dbscan.DbscanTest
*/
package com.dbscan;

import java.util.ArrayList;
import java.util.List;

public class Main {
    public static void main(String[] args) {
        List<Point> dataset = new ArrayList<>();

        // Cluster 0 points.
        dataset.add(new Point(new double[]{1.0, 1.0}));
        dataset.add(new Point(new double[]{1.1, 0.9}));
        dataset.add(new Point(new double[]{0.9, 1.2}));

        // Cluster 1 points.
        dataset.add(new Point(new double[]{5.0, 5.0}));
        dataset.add(new Point(new double[]{5.2, 5.1}));
        dataset.add(new Point(new double[]{4.9, 5.3}));

        // Noise point.
        dataset.add(new Point(new double[]{10.0, 10.0}));

        // Run DBSCAN with epsilon = 0.5 and minimumPoints = 2.
        Dbscan dbscan = new Dbscan(0.5, 2);

        // Build the clusters.
        dbscan.cluster(dataset);

        // Summary.
        for (Point point : dataset) {
            System.out.println(point);
        }
    }
}