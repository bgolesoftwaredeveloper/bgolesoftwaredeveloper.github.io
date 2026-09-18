/*
File            :   BirchTest.java
Project         :   BIRCH
Programmer      :   Braiden Gole
First version   :   2026-08-03
Description     :   Provides comprehensive unit test coverage for the BIRCH 
                    clustering algorithm, verifying Cluster Feature triplet math, 
                    radius thresholds, Euclidean distances, node splitting, and 
                    tree extraction behaviors.
*/
package com.test.birch;

import com.birch.ClusterFeature;
import com.birch.Tree;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.DisplayName;

import java.util.List;

import static org.junit.jupiter.api.Assertions.*;

public class BirchTest {
    /**
     * Verifies that initializing an empty ClusterFeature sets initial count and squared sum to zero.
     */
    @Test
    @DisplayName("1. Empty ClusterFeature initializes with zero state")
    void testEmptyClusterFeatureInitialization() {
        // Arrange.
        int designatedDimension = 3;

        // Act.
        ClusterFeature emptyFeatureVector = new ClusterFeature(designatedDimension);

        // Assert.
        assertEquals(0, emptyFeatureVector.pointCount);
        assertEquals(0.0, emptyFeatureVector.squaredSum, 0.0001);
        assertEquals(3, emptyFeatureVector.linearSum.length);
        assertArrayEquals(new double[]{0.0, 0.0, 0.0}, emptyFeatureVector.getCentroid(), 0.0001);
    }

    /**
     * Verifies that initializing a ClusterFeature from a single point calculates correct sum of squares.
     */
    @Test
    @DisplayName("2. Single-point ClusterFeature construction computes accurate SS and LS")
    void testSinglePointClusterFeatureInitialization() {
        // Arrange.
        double[] startingCoordinates = new double[]{3.0, 4.0};

        // Act.
        ClusterFeature singlePointFeatureVector = new ClusterFeature(startingCoordinates);

        // Assert.
        assertEquals(1, singlePointFeatureVector.pointCount);
        assertArrayEquals(new double[]{3.0, 4.0}, singlePointFeatureVector.linearSum, 0.0001);
        assertEquals(25.0, singlePointFeatureVector.squaredSum, 0.0001);
    }

    /**
     * Verifies deep copying of ClusterFeature instance values.
     */
    @Test
    @DisplayName("3. Copy constructor produces an independent deep copy of ClusterFeature")
    void testClusterFeatureCopyConstructor() {
        // Arrange.
        double[] inputCoordinates = new double[]{2.0, 5.0};

        ClusterFeature originalFeatureVector = new ClusterFeature(inputCoordinates);

        // Act.
        ClusterFeature copiedFeatureVector = new ClusterFeature(originalFeatureVector);

        copiedFeatureVector.linearSum[0] = 99.0;

        // Assert.
        assertEquals(originalFeatureVector.pointCount, copiedFeatureVector.pointCount);
        assertEquals(2.0, originalFeatureVector.linearSum[0], 0.0001);
        assertEquals(99.0, copiedFeatureVector.linearSum[0], 0.0001);
    }

    /**
     * Verifies centroid coordinate evaluation for multi-point cluster features.
     */
    @Test
    @DisplayName("4. Centroid evaluation yields exact average vector across dimensions")
    void testClusterFeatureCentroidCalculation() {
        // Arrange.
        ClusterFeature combinedFeatureVector = new ClusterFeature(new double[]{1.0, 2.0});

        combinedFeatureVector.addClusterFeature(new ClusterFeature(new double[]{3.0, 6.0}));

        // Act.
        double[] calculatedCentroidCoordinates = combinedFeatureVector.getCentroid();

        // Assert.
        assertEquals(2, combinedFeatureVector.pointCount);
        assertEquals(2.0, calculatedCentroidCoordinates[0], 0.0001);
        assertEquals(4.0, calculatedCentroidCoordinates[1], 0.0001);
    }

    /**
     * Verifies that a ClusterFeature containing <= 1 point yields zero radius.
     */
    @Test
    @DisplayName("5. Cluster Feature containing <= 1 point must return zero radius")
    void testSinglePointRadiusZero() {
        // Arrange.
        ClusterFeature lonePointFeatureVector = new ClusterFeature(new double[]{5.5, -2.3});

        // Act.
        double calculatedPointRadius = lonePointFeatureVector.getRadius();

        // Assert.
        assertEquals(0.0, calculatedPointRadius, 0.0001);
    }

    /**
     * Verifies radius calculation across multiple spatial points.
     */
    @Test
    @DisplayName("6. Multi-point cluster feature radius calculation evaluation")
    void testMultiPointClusterRadiusCalculation() {
        // Arrange.
        ClusterFeature aggregatedClusterVector = new ClusterFeature(new double[]{0.0, 0.0});

        aggregatedClusterVector.addClusterFeature(new ClusterFeature(new double[]{2.0, 0.0}));

        // Act.
        double calculatedClusterRadius = aggregatedClusterVector.getRadius();

        // Assert.
        assertEquals(1.0, calculatedClusterRadius, 0.0001);
    }

    /**
     * Verifies Euclidean distance calculation from a centroid to a target point.
     */
    @Test
    @DisplayName("7. Distance from ClusterFeature centroid to target point calculation")
    void testDistanceToPointCalculation() {
        // Arrange.
        ClusterFeature originClusterFeature = new ClusterFeature(new double[]{0.0, 0.0});

        double[] targetDestinationCoordinates = new double[]{3.0, 4.0};

        // Act.
        double calculatedEuclideanDistance = originClusterFeature.distanceToPoint(targetDestinationCoordinates);

        // Assert.
        assertEquals(5.0, calculatedEuclideanDistance, 0.0001);
    }

    /**
     * Verifies distance calculation between two separate ClusterFeature centroids (D0 distance).
     */
    @Test
    @DisplayName("8. Distance between two ClusterFeature centroids (D0 metric)")
    void testDistanceBetweenClusterFeatures() {
        // Arrange.
        ClusterFeature primaryClusterFeature = new ClusterFeature(new double[]{1.0, 1.0});
        ClusterFeature secondaryClusterFeature = new ClusterFeature(new double[]{4.0, 5.0});

        // Act.
        double calculatedCentroidDistance = primaryClusterFeature.distanceToClusterFeature(secondaryClusterFeature);

        // Assert.
        assertEquals(5.0, calculatedCentroidDistance, 0.0001);
    }

    /**
     * Verifies CF Additivity Theorem logic when merging two features.
     */
    @Test
    @DisplayName("9. CF Additivity Theorem correctly merges N, LS, and SS components")
    void testClusterFeatureAdditivityTheorem() {
        // Arrange.
        ClusterFeature primaryClusterFeature = new ClusterFeature(new double[]{1.0, 2.0});
        ClusterFeature secondaryClusterFeature = new ClusterFeature(new double[]{3.0, 4.0});

        // Act.
        primaryClusterFeature.addClusterFeature(secondaryClusterFeature);

        // Assert.
        assertEquals(2, primaryClusterFeature.pointCount);
        assertEquals(4.0, primaryClusterFeature.linearSum[0], 0.0001);
        assertEquals(6.0, primaryClusterFeature.linearSum[1], 0.0001);
        assertEquals(30.0, primaryClusterFeature.squaredSum, 0.0001);
    }

    /**
     * Verifies inter-cluster distance symmetry.
     */
    @Test
    @DisplayName("10. Inter-cluster distance symmetry property verification")
    void testClusterFeatureDistanceSymmetry() {
        // Arrange.
        ClusterFeature firstClusterVector = new ClusterFeature(new double[]{-2.5, 4.1});
        ClusterFeature secondClusterVector = new ClusterFeature(new double[]{1.2, -3.9});

        // Act.
        double forwardDistanceMeasurement = firstClusterVector.distanceToClusterFeature(secondClusterVector);
        double reverseDistanceMeasurement = secondClusterVector.distanceToClusterFeature(firstClusterVector);

        // Assert.
        assertEquals(forwardDistanceMeasurement, reverseDistanceMeasurement, 0.0001);
    }

    /**
     * Verifies merging identical spatial points preserves overall centroid location.
     */
    @Test
    @DisplayName("11. Merging identical overlapping points preserves centroid location")
    void testMergingIdenticalPointsPreservesCentroid() {
        // Arrange.
        ClusterFeature baseClusterFeature = new ClusterFeature(new double[]{5.0, 5.0});

        // Act.
        for (int iterationIndex = 0; iterationIndex < 4; iterationIndex++) {
            baseClusterFeature.addClusterFeature(new ClusterFeature(new double[]{5.0, 5.0}));
        }

        // Assert.
        assertEquals(5, baseClusterFeature.pointCount);
        assertArrayEquals(new double[]{5.0, 5.0}, baseClusterFeature.getCentroid(), 0.0001);
        assertEquals(0.0, baseClusterFeature.getRadius(), 0.0001);
    }

    /**
     * Verifies that precision tolerance guards prevent negative floating point values under square root.
     */
    @Test
    @DisplayName("12. Floating point precision guards prevent negative variance under sqrt")
    void testFloatingPointPrecisionGuardInRadius() {
        // Arrange.
        double[] highPrecisionCoordinates = new double[]{0.3333333333333333, 0.6666666666666666};
        ClusterFeature highPrecisionFeature = new ClusterFeature(highPrecisionCoordinates);
        highPrecisionFeature.addClusterFeature(new ClusterFeature(highPrecisionCoordinates));

        // Act & Assert.
        assertDoesNotThrow(highPrecisionFeature::getRadius);
        assertTrue(highPrecisionFeature.getRadius() >= 0.0);
    }

    /**
     * Verifies 1D vector spatial calculations inside ClusterFeature.
     */
    @Test
    @DisplayName("13. One-dimensional ClusterFeature math and radius calculations")
    void testOneDimensionalClusterFeatureCalculations() {
        // Arrange.
        ClusterFeature singleDimensionFeatureOne = new ClusterFeature(new double[]{2.0});
        ClusterFeature singleDimensionFeatureTwo = new ClusterFeature(new double[]{8.0});

        // Act.
        singleDimensionFeatureOne.addClusterFeature(singleDimensionFeatureTwo);

        // Assert.
        assertEquals(2, singleDimensionFeatureOne.pointCount);
        assertEquals(5.0, singleDimensionFeatureOne.getCentroid()[0], 0.0001);
        assertEquals(3.0, singleDimensionFeatureOne.getRadius(), 0.0001);
    }

    /**
     * Verifies 4D vector spatial calculations inside ClusterFeature.
     */
    @Test
    @DisplayName("14. Four-dimensional hyper-space ClusterFeature math calculations")
    void testFourDimensionalClusterFeatureCalculations() {
        // Arrange.
        double[] hyperPointOne = new double[]{1.0, 2.0, 3.0, 4.0};
        double[] hyperPointTwo = new double[]{3.0, 4.0, 5.0, 6.0};

        ClusterFeature hyperFeature = new ClusterFeature(hyperPointOne);

        // Act.
        hyperFeature.addClusterFeature(new ClusterFeature(hyperPointTwo));

        // Assert.
        assertEquals(2, hyperFeature.pointCount);
        assertArrayEquals(new double[]{2.0, 3.0, 4.0, 5.0}, hyperFeature.getCentroid(), 0.0001);
    }

    /**
     * Verifies points within radius threshold T are merged into a single sub-cluster.
     */
    @Test
    @DisplayName("15. Points within radius threshold T are absorbed into a single sub-cluster")
    void testPointsWithinThresholdMergedIntoSingleSubcluster() {
        // Arrange.
        Tree birchTree = new Tree(3, 2.0, 2);

        // Act.
        birchTree.insert(new double[]{0.0, 0.0});
        birchTree.insert(new double[]{1.0, 0.0});
        birchTree.insert(new double[]{0.5, 0.5});

        List<ClusterFeature> extractedSubclusters = birchTree.getSubclusters();

        // Assert.
        assertEquals(1, extractedSubclusters.size());
        assertEquals(3, extractedSubclusters.get(0).pointCount);
    }

    /**
     * Verifies points exceeding radius threshold T trigger a new sub-cluster entry.
     */
    @Test
    @DisplayName("16. Points exceeding radius threshold T create separate sub-clusters")
    void testPointsExceedingThresholdCreateNewSubcluster() {
        // Arrange.
        Tree birchTree = new Tree(5, 0.5, 2);

        // Act.
        birchTree.insert(new double[]{0.0, 0.0});
        birchTree.insert(new double[]{10.0, 10.0});

        List<ClusterFeature> extractedSubclusters = birchTree.getSubclusters();

        // Assert.
        assertEquals(2, extractedSubclusters.size());
        assertEquals(1, extractedSubclusters.get(0).pointCount);
        assertEquals(1, extractedSubclusters.get(1).pointCount);
    }

    /**
     * Verifies leaf node split when node entry capacity is exceeded.
     */
    @Test
    @DisplayName("17. Exceeding leaf node capacity triggers a leaf node split operation")
    void testLeafNodeCapacityOverflowTriggersSplit() {
        // Arrange.
        Tree restrictedCapacityTree = new Tree(2, 0.1, 2);

        // Act.
        restrictedCapacityTree.insert(new double[]{0.0, 0.0});
        restrictedCapacityTree.insert(new double[]{5.0, 5.0});
        restrictedCapacityTree.insert(new double[]{10.0, 10.0});

        List<ClusterFeature> extractedSubclusters = restrictedCapacityTree.getSubclusters();

        // Assert.
        assertEquals(3, extractedSubclusters.size());
    }

    /**
     * Verifies merge behavior on the exact radius threshold boundary.
     */
    @Test
    @DisplayName("18. Cluster radius equal to threshold T allows successful merge")
    void testRadiusExactThresholdBoundaryMerge() {
        // Arrange.
        Tree exactBoundaryTree = new Tree(3, 1.0, 2);

        // Act.
        exactBoundaryTree.insert(new double[]{0.0, 0.0});
        exactBoundaryTree.insert(new double[]{2.0, 0.0});

        List<ClusterFeature> extractedSubclusters = exactBoundaryTree.getSubclusters();

        // Assert.
        assertEquals(1, extractedSubclusters.size());
        assertEquals(2, extractedSubclusters.get(0).pointCount);
        assertEquals(1.0, extractedSubclusters.get(0).getRadius(), 0.0001);
    }

    /**
     * Verifies rejection of merge when radius threshold is slightly exceeded.
     */
    @Test
    @DisplayName("19. Cluster radius slightly exceeding threshold T prevents merging")
    void testRadiusExceedingThresholdBoundaryRejection() {
        // Arrange.
        Tree exactBoundaryTree = new Tree(3, 0.999, 2);

        // Act.
        exactBoundaryTree.insert(new double[]{0.0, 0.0});
        exactBoundaryTree.insert(new double[]{2.0, 0.0});

        List<ClusterFeature> extractedSubclusters = exactBoundaryTree.getSubclusters();

        // Assert.
        assertEquals(2, extractedSubclusters.size());
    }

    /**
     * Verifies multi-level tree creation and root splitting.
     */
    @Test
    @DisplayName("20. Multi-level tree height expansion upon root node split")
    void testTreeHeightExpansionOnRootSplit() {
        // Arrange.
        Tree branchingTree = new Tree(2, 0.1, 2);

        // Act.
        for (int pointIndex = 0; pointIndex < 6; pointIndex++) {
            branchingTree.insert(new double[]{(double) pointIndex * 10.0, 0.0});
        }

        List<ClusterFeature> extractedSubclusters = branchingTree.getSubclusters();

        // Assert.
        assertEquals(6, extractedSubclusters.size());
    }

    /**
     * Verifies point insertion routing to closest cluster entry.
     */
    @Test
    @DisplayName("21. Insert routing directs incoming points to nearest existing sub-cluster")
    void testInsertRoutingToNearestSubcluster() {
        // Arrange.
        Tree routingTree = new Tree(3, 2.0, 2);

        routingTree.insert(new double[]{0.0, 0.0});
        routingTree.insert(new double[]{10.0, 10.0});

        // Act.
        routingTree.insert(new double[]{0.2, 0.1});

        List<ClusterFeature> extractedSubclusters = routingTree.getSubclusters();

        // Assert.
        assertEquals(2, extractedSubclusters.size());

        ClusterFeature targetRegion = extractedSubclusters.stream()
            .filter(cf -> cf.getCentroid()[0] < 2.0)
            .findFirst()
            .orElseThrow();

        assertEquals(2, targetRegion.pointCount);
    }

    /**
     * Verifies furthest entry pair identification during node splits.
     */
    @Test
    @DisplayName("22. Node split seed selection picks the two furthest entries")
    void testNodeSplitSeedSelectionFurthestPair() {
        // Arrange.
        Tree seedTestingTree = new Tree(2, 0.1, 2);

        // Act.
        seedTestingTree.insert(new double[]{0.0, 0.0});
        seedTestingTree.insert(new double[]{1.0, 0.0});
        seedTestingTree.insert(new double[]{10.0, 0.0});

        List<ClusterFeature> extractedSubclusters = seedTestingTree.getSubclusters();

        // Assert.
        assertEquals(3, extractedSubclusters.size());
    }

    /**
     * Verifies insertion of negative coordinates into tree.
     */
    @Test
    @DisplayName("23. Tree insertion correctly processes negative coordinate points")
    void testNegativeCoordinateInsertion() {
        // Arrange.
        Tree negativeSpaceTree = new Tree(3, 1.5, 2);

        // Act.
        negativeSpaceTree.insert(new double[]{-5.0, -5.0});
        negativeSpaceTree.insert(new double[]{-5.2, -4.8});
        negativeSpaceTree.insert(new double[]{5.0, 5.0});

        List<ClusterFeature> extractedSubclusters = negativeSpaceTree.getSubclusters();

        // Assert.
        assertEquals(2, extractedSubclusters.size());
    }

    /**
     * Verifies retrieving sub-clusters from an empty tree.
     */
    @Test
    @DisplayName("24. Newly instantiated empty Tree yields zero sub-clusters")
    void testEmptyTreeSubclusterExtraction() {
        // Arrange.
        Tree emptyBirchTree = new Tree(3, 1.0, 2);

        // Act.
        List<ClusterFeature> emptySubclusters = emptyBirchTree.getSubclusters();

        // Assert.
        assertNotNull(emptySubclusters);
        assertTrue(emptySubclusters.isEmpty());
    }

    /**
     * Verifies single point insertion into the tree.
     */
    @Test
    @DisplayName("25. Single point insertion yields exactly one sub-cluster with count 1")
    void testSinglePointTreeInsertion() {
        // Arrange.
        Tree singlePointTree = new Tree(3, 1.0, 2);

        // Act.
        singlePointTree.insert(new double[]{4.2, 8.1});

        List<ClusterFeature> extractedSubclusters = singlePointTree.getSubclusters();

        // Assert.
        assertEquals(1, extractedSubclusters.size());
        assertEquals(1, extractedSubclusters.get(0).pointCount);
        assertArrayEquals(new double[]{4.2, 8.1}, extractedSubclusters.get(0).getCentroid(), 0.0001);
    }

    /**
     * Verifies high-volume identical point streaming into tree.
     */
    @Test
    @DisplayName("26. Massive identical point streaming accumulates into single sub-cluster")
    void testMassiveIdenticalPointStreaming() {
        // Arrange.
        Tree highVolumeTree = new Tree(3, 1.0, 2);

        double[] constantPoint = new double[]{2.5, 2.5};

        // Act.
        for (int stepIndex = 0; stepIndex < 200; stepIndex++) {
            highVolumeTree.insert(constantPoint);
        }

        List<ClusterFeature> extractedSubclusters = highVolumeTree.getSubclusters();

        // Assert.
        assertEquals(1, extractedSubclusters.size());
        assertEquals(200, extractedSubclusters.get(0).pointCount);
        assertEquals(0.0, extractedSubclusters.get(0).getRadius(), 0.0001);
    }

    /**
     * Verifies large radius threshold forces single combined cluster.
     */
    @Test
    @DisplayName("27. Extremely large radius threshold T forces single sub-cluster outcome")
    void testHugeRadiusThresholdMergesAllPoints() {
        // Arrange.
        Tree giantThresholdTree = new Tree(10, 1000.0, 2);

        double[][] scatteredPoints = {
            {0.0, 0.0},
            {10.0, 10.0},
            {50.0, 50.0},
            {100.0, 100.0}
        };

        // Act.
        for (double[] point : scatteredPoints) {
            giantThresholdTree.insert(point);
        }

        List<ClusterFeature> extractedSubclusters = giantThresholdTree.getSubclusters();

        // Assert.
        assertEquals(1, extractedSubclusters.size());
        assertEquals(4, extractedSubclusters.get(0).pointCount);
    }

    /**
     * Verifies zero radius threshold isolates every unique point.
     */
    @Test
    @DisplayName("28. Zero radius threshold T isolates every unique point into its own sub-cluster")
    void testZeroRadiusThresholdIsolatesPoints() {
        // Arrange.
        Tree zeroThresholdTree = new Tree(5, 0.0, 2);

        double[][] uniquePoints = {
            {1.0, 1.0},
            {1.01, 1.0},
            {1.02, 1.0}
        };

        // Act.
        for (double[] point : uniquePoints) {
            zeroThresholdTree.insert(point);
        }

        List<ClusterFeature> extractedSubclusters = zeroThresholdTree.getSubclusters();

        // Assert.
        assertEquals(3, extractedSubclusters.size());
        for (ClusterFeature subcluster : extractedSubclusters) {
            assertEquals(1, subcluster.pointCount);
        }
    }

    /**
     * Verifies clustering two distinct dense point clouds.
     */
    @Test
    @DisplayName("29. Clustering two dense spatial point clouds into two separate sub-clusters")
    void testTwoDistinctDensePointClouds() {
        // Arrange.
        Tree denseCloudTree = new Tree(4, 1.5, 2);

        double[][] firstClusterPoints = {
            {1.0, 1.0}, {1.1, 1.2}, {0.9, 1.1}, {1.2, 0.9}, {1.0, 0.8}
        };

        double[][] secondClusterPoints = {
            {20.0, 20.0}, {20.1, 20.2}, {19.9, 20.1}, {20.2, 19.9}, {20.0, 19.8}
        };

        // Act.
        for (double[] point : firstClusterPoints) {
            denseCloudTree.insert(point);
        }
        for (double[] point : secondClusterPoints) {
            denseCloudTree.insert(point);
        }

        List<ClusterFeature> extractedSubclusters = denseCloudTree.getSubclusters();

        // Assert.
        assertEquals(2, extractedSubclusters.size());

        int totalPointCount = extractedSubclusters.stream()
            .mapToInt(cf -> cf.pointCount)
            .sum();

        assertEquals(10, totalPointCount);
    }

    /**
     * High volume execution smoke test with 500 streamed points.
     */
    @Test
    @DisplayName("30. High-volume stress test streaming 500 multi-cluster points without exceptions")
    void testHighVolumeStressTestExecution() {
        // Arrange.
        Tree stressTree = new Tree(5, 1.0, 2);

        // Act & Assert.
        assertDoesNotThrow(() -> {
            for (int pointIndex = 0; pointIndex < 500; pointIndex++) {
                double xCoordinate = (pointIndex % 5) * 10.0 + (pointIndex * 0.001);
                double yCoordinate = (pointIndex % 5) * 10.0 + (pointIndex * 0.001);
                stressTree.insert(new double[]{xCoordinate, yCoordinate});
            }
        });

        List<ClusterFeature> extractedSubclusters = stressTree.getSubclusters();

        assertFalse(extractedSubclusters.isEmpty());

        int totalAccumulatedPoints = extractedSubclusters.stream()
            .mapToInt(cf -> cf.pointCount)
            .sum();

        assertEquals(500, totalAccumulatedPoints);
    }
}