/*
File            :   DbscanTest.java
Project         :   DBSCAN
Programmer      :   Braiden Gole
First version   :   2026-07-29
Description     :   Provides comprehensive unit test coverage for the DBSCAN 
                    clustering algorithm, verifying point calculations, edge 
                    cases, cluster expansions, and spatial boundaries.
*/
package com.test.dbscan;

import com.dbscan.Dbscan;
import com.dbscan.Point;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.DisplayName;

import java.util.ArrayList;
import java.util.List;

import static org.junit.jupiter.api.Assertions.*;

/**
 * The DbscanTest class provides comprehensive unit coverage for spatial point calculations
 * and the DBSCAN clustering algorithm execution pipeline.
 */
public class DbscanTest {
    /**
     * Verifies that newly instantiated points default to the UNVISITED state.
     */
    @Test
    @DisplayName("1. Point initial cluster ID must be UNVISITED (-2)")
    void testPointInitialState() {
        // Arrange.
        double[] startingCoordinates = new double[]{1.0, 2.0};

        Point unvisitedPoint = new Point(startingCoordinates);

        // Act.
        int initialClusterIdentifier = unvisitedPoint.getClusterId();

        // Assert.
        assertEquals(Point.UNVISITED, initialClusterIdentifier);
    }

    /**
     * Verifies that measuring distance from a point to itself returns zero distance.
     */
    @Test
    @DisplayName("2. Distance between identical points must be zero")
    void testPointDistanceToSelf() {
        // Arrange.
        Point stationaryReferencePoint = new Point(new double[]{3.0, 4.0});

        // Act.
        double calculatedSelfDistance = stationaryReferencePoint.distance(stationaryReferencePoint);

        // Assert.
        assertEquals(0.0, calculatedSelfDistance, 0.0001);
    }

    /**
     * Tests standard 2D Euclidean distance calculation using a 3-4-5 triangle.
     */
    @Test
    @DisplayName("3. Distance calculation (3-4-5 Pythagorean triangle)")
    void testPointDistanceEuclidean2D() {
        // Arrange.
        Point coordinateOriginPoint = new Point(new double[]{0.0, 0.0});
        Point pythagoreanHypotenuseTargetPoint = new Point(new double[]{3.0, 4.0});

        // Act.
        double calculatedEuclideanDistance2D = coordinateOriginPoint.distance(pythagoreanHypotenuseTargetPoint);

        // Assert.
        assertEquals(5.0, calculatedEuclideanDistance2D, 0.0001);
    }

    /**
     * Tests standard 3D Euclidean distance calculation across three dimensions.
     */
    @Test
    @DisplayName("4. Distance calculation in 3D space")
    void testPointDistanceEuclidean3D() {
        // Arrange.
        Point startingVolumePoint = new Point(new double[]{1.0, 2.0, 3.0});
        Point endingVolumePoint = new Point(new double[]{2.0, 4.0, 5.0});

        // Act.
        double calculatedEuclideanDistance3D = startingVolumePoint.distance(endingVolumePoint);

        // Assert.
        assertEquals(3.0, calculatedEuclideanDistance3D, 0.0001);
    }

    /**
     * Verifies the commutative distance property where distance(A, B) equals distance(B, A).
     */
    @Test
    @DisplayName("5. Distance symmetry: d(A, B) == d(B, A)")
    void testPointDistanceSymmetry() {
        // Arrange.
        Point primarySymmetricPoint = new Point(new double[]{1.5, -2.3});
        Point secondarySymmetricPoint = new Point(new double[]{-4.1, 0.2});

        // Act.
        double forwardDistanceMeasurement = primarySymmetricPoint.distance(secondarySymmetricPoint);
        double reverseDistanceMeasurement = secondarySymmetricPoint.distance(primarySymmetricPoint);

        // Assert.
        assertEquals(forwardDistanceMeasurement, reverseDistanceMeasurement, 0.0001);
    }

    /**
     * Verifies getter and setter operations for cluster membership identifiers.
     */
    @Test
    @DisplayName("6. Verify getter and setter for Cluster ID")
    void testPointSetAndGetClusterId() {
        // Arrange.
        Point mutableClusterPoint = new Point(new double[]{0.0, 0.0});

        int designatedClusterIdentifier = 5;

        // Act.
        mutableClusterPoint.setClusterId(designatedClusterIdentifier);

        int retrievedClusterIdentifier = mutableClusterPoint.getClusterId();

        // Assert.
        assertEquals(designatedClusterIdentifier, retrievedClusterIdentifier);
    }

    /**
     * Verifies coordinate array integrity when retrieved from a Point object.
     */
    @Test
    @DisplayName("7. Point getter returns original coordinate array")
    void testPointGetCoordinates() {
        // Arrange.
        double[] inputCoordinateValues = new double[]{10.5, 20.2};

        Point coordinateHoldingPoint = new Point(inputCoordinateValues);

        // Act.
        double[] retrievedCoordinateValues = coordinateHoldingPoint.getCoordinates();

        // Assert.
        assertArrayEquals(inputCoordinateValues, retrievedCoordinateValues);
    }

    /**
     * Verifies string representation formatting when a point is designated as NOISE.
     */
    @Test
    @DisplayName("8. Point toString formatting with NOISE label")
    void testPointToStringNoise() {
        // Arrange.
        Point noiseLabeledPoint = new Point(new double[]{1.0, 2.0});

        noiseLabeledPoint.setClusterId(Point.NOISE);

        // Act.
        String formattedPointString = noiseLabeledPoint.toString();

        // Assert.
        assertTrue(formattedPointString.contains("Noise"));
    }

    // ==========================================
    // Category 2: Edge Cases & Datasets (9–15)
    // ==========================================

    /**
     * Verifies that clustering an empty dataset executes smoothly without runtime exceptions.
     */
    @Test
    @DisplayName("9. Empty dataset should execute without throwing exceptions")
    void testEmptyDataset() {
        // Arrange.
        Dbscan clusteringEngine = new Dbscan(1.0, 2);

        List<Point> emptyPointCollection = new ArrayList<>();

        // Act & Assert.
        assertDoesNotThrow(() -> clusteringEngine.cluster(emptyPointCollection));
    }

    /**
     * Verifies that a single isolated point with unmet minPts threshold turns to NOISE.
     */
    @Test
    @DisplayName("10. Single point with minPts > 1 must be marked as NOISE")
    void testSinglePointNoise() {
        // Arrange.
        Dbscan clusteringEngine = new Dbscan(1.0, 2);
        Point isolatedLonePoint = new Point(new double[]{1.0, 1.0});

        List<Point> singlePointDataset = List.of(isolatedLonePoint);

        // Act.
        clusteringEngine.cluster(singlePointDataset);

        // Assert.
        assertEquals(Point.NOISE, isolatedLonePoint.getClusterId());
    }

    /**
     * Verifies that a single point creates Cluster 0 when minPts is configured to 1.
     */
    @Test
    @DisplayName("11. Single point with minPts = 1 forms Cluster 0")
    void testSinglePointMinPtsOne() {
        // Arrange.
        Dbscan clusteringEngine = new Dbscan(1.0, 1);
        Point selfSufficientPoint = new Point(new double[]{1.0, 1.0});

        List<Point> singlePointDataset = List.of(selfSufficientPoint);

        // Act.
        clusteringEngine.cluster(singlePointDataset);

        // Assert.
        assertEquals(0, selfSufficientPoint.getClusterId());
    }

    /**
     * Verifies that identical overlapping spatial coordinates are all grouped into one cluster.
     */
    @Test
    @DisplayName("12. All identical overlapping points form a single cluster")
    void testIdenticalCoordinates() {
        // Arrange.
        Dbscan clusteringEngine = new Dbscan(0.1, 3);

        List<Point> overlappingPointsDataset = new ArrayList<>();

        for (int iterationIndex = 0; iterationIndex < 5; iterationIndex++) {
            overlappingPointsDataset.add(new Point(new double[]{2.0, 2.0}));
        }

        // Act.
        clusteringEngine.cluster(overlappingPointsDataset);

        // Assert.
        for (Point evaluatedPoint : overlappingPointsDataset) {
            assertEquals(0, evaluatedPoint.getClusterId());
        }
    }

    /**
     * Verifies that a huge epsilon merges distant points into a single cluster.
     */
    @Test
    @DisplayName("13. Extremely large Epsilon groups all scattered points into one cluster")
    void testHugeEpsilon() {
        // Arrange.
        Dbscan clusteringEngineWithLargeEpsilon = new Dbscan(1000.0, 2);

        List<Point> widelyScatteredDataset = List.of(
            new Point(new double[]{0.0, 0.0}),
            new Point(new double[]{10.0, 10.0}),
            new Point(new double[]{50.0, 50.0})
        );

        // Act.
        clusteringEngineWithLargeEpsilon.cluster(widelyScatteredDataset);

        // Assert.
        for (Point evaluatedPoint : widelyScatteredDataset) {
            assertEquals(0, evaluatedPoint.getClusterId());
        }
    }

    /**
     * Verifies that a tiny epsilon marks separated points as NOISE.
     */
    @Test
    @DisplayName("14. Extremely small Epsilon marks all separated points as NOISE")
    void testTinyEpsilon() {
        // Arrange.
        Dbscan clusteringEngineWithTinyEpsilon = new Dbscan(0.0001, 2);

        List<Point> spacedOutPointDataset = List.of(
            new Point(new double[]{0.0, 0.0}),
            new Point(new double[]{1.0, 1.0}),
            new Point(new double[]{2.0, 2.0})
        );

        // Act.
        clusteringEngineWithTinyEpsilon.cluster(spacedOutPointDataset);

        // Assert.
        for (Point evaluatedPoint : spacedOutPointDataset) {
            assertEquals(Point.NOISE, evaluatedPoint.getClusterId());
        }
    }

    /**
     * Verifies that setting an unreachable minPts marks the entire dataset as NOISE.
     */
    @Test
    @DisplayName("15. Unreachable minPts marks entire dataset as NOISE")
    void testUnreachableMinPts() {
        // Arrange.
        Dbscan clusteringEngineWithStrictMinPts = new Dbscan(1.0, 100);

        List<Point> smallPopulationDataset = new ArrayList<>();

        for (int pointIndex = 0; pointIndex < 10; pointIndex++) {
            smallPopulationDataset.add(new Point(new double[]{(double) pointIndex, (double) pointIndex}));
        }

        // Act.
        clusteringEngineWithStrictMinPts.cluster(smallPopulationDataset);

        // Assert.
        for (Point evaluatedPoint : smallPopulationDataset) {
            assertEquals(Point.NOISE, evaluatedPoint.getClusterId());
        }
    }

    /**
     * Verifies that points sitting exactly on the distance epsilon boundary are included in the cluster.
     */
    @Test
    @DisplayName("16. Two points exactly at Epsilon boundary form a cluster (<= comparison)")
    void testEpsilonExactBoundary() {
        // Arrange.
        Dbscan boundaryTestingEngine = new Dbscan(2.0, 2);
        Point originBoundaryPoint = new Point(new double[]{0.0, 0.0});
        Point exactEpsilonDistancePoint = new Point(new double[]{2.0, 0.0});

        List<Point> boundaryTestDataset = List.of(originBoundaryPoint, exactEpsilonDistancePoint);

        // Act.
        boundaryTestingEngine.cluster(boundaryTestDataset);

        // Assert.
        assertEquals(0, originBoundaryPoint.getClusterId());
        assertEquals(0, exactEpsilonDistancePoint.getClusterId());
    }

    /**
     * Verifies that points lying beyond the epsilon distance boundary are identified as NOISE.
     */
    @Test
    @DisplayName("17. Two points just beyond Epsilon boundary are marked as NOISE")
    void testEpsilonExceededBoundary() {
        // Arrange.
        Dbscan boundaryTestingEngine = new Dbscan(2.0, 2);
        Point originBoundaryPoint = new Point(new double[]{0.0, 0.0});
        Point exceededEpsilonDistancePoint = new Point(new double[]{2.0001, 0.0});

        List<Point> boundaryExceededDataset = List.of(originBoundaryPoint, exceededEpsilonDistancePoint);

        // Act.
        boundaryTestingEngine.cluster(boundaryExceededDataset);

        // Assert.
        assertEquals(Point.NOISE, originBoundaryPoint.getClusterId());
        assertEquals(Point.NOISE, exceededEpsilonDistancePoint.getClusterId());
    }

    /**
     * Verifies that two distinct clusters are formed and assigned sequential cluster IDs 0 and 1.
     */
    @Test
    @DisplayName("18. Two distinct well-separated clusters are assigned IDs 0 and 1")
    void testTwoSeparateClusters() {
        // Arrange.
        Dbscan dualClusterEngine = new Dbscan(0.5, 2);

        Point firstClusterPrimaryPoint = new Point(new double[]{0.0, 0.0});
        Point firstClusterSecondaryPoint = new Point(new double[]{0.1, 0.1});
        Point secondClusterPrimaryPoint = new Point(new double[]{10.0, 10.0});
        Point secondClusterSecondaryPoint = new Point(new double[]{10.1, 10.1});

        List<Point> multiClusterDataset = List.of(
            firstClusterPrimaryPoint, 
            firstClusterSecondaryPoint, 
            secondClusterPrimaryPoint, 
            secondClusterSecondaryPoint
        );

        // Act.
        dualClusterEngine.cluster(multiClusterDataset);

        // Assert.
        assertEquals(0, firstClusterPrimaryPoint.getClusterId());
        assertEquals(0, firstClusterSecondaryPoint.getClusterId());
        assertEquals(1, secondClusterPrimaryPoint.getClusterId());
        assertEquals(1, secondClusterSecondaryPoint.getClusterId());
    }

    /**
     * Verifies density expansion across a linear chain of interconnected neighbors.
     */
    @Test
    @DisplayName("19. Line density chain expands into a single long cluster")
    void testLinearClusterChainExpansion() {
        // Arrange.
        Dbscan chainExpansionEngine = new Dbscan(1.1, 2);

        List<Point> linearChainDataset = List.of(
            new Point(new double[]{0.0, 0.0}),
            new Point(new double[]{1.0, 0.0}),
            new Point(new double[]{2.0, 0.0}),
            new Point(new double[]{3.0, 0.0})
        );

        // Act.
        chainExpansionEngine.cluster(linearChainDataset);

        // Assert.
        for (Point chainMemberPoint : linearChainDataset) {
            assertEquals(0, chainMemberPoint.getClusterId());
        }
    }

    /**
     * Verifies that points initially flagged as NOISE are reassigned to valid clusters if reached during expansion.
     */
    @Test
    @DisplayName("20. Border point reassignment: Noise converted to cluster member")
    void testNoiseReassignedToBorderPoint() {
        // Arrange.
        Dbscan borderReassignmentEngine = new Dbscan(1.0, 3);

        Point potentialBorderNoisePoint = new Point(new double[]{2.0, 0.0});
        Point coreClusterAnchorPointOne = new Point(new double[]{0.0, 0.0});
        Point coreClusterAnchorPointTwo = new Point(new double[]{0.5, 0.0});
        Point coreClusterAnchorPointThree = new Point(new double[]{1.0, 0.0});

        List<Point> reassignmentDataset = List.of(
            potentialBorderNoisePoint, 
            coreClusterAnchorPointOne, 
            coreClusterAnchorPointTwo, 
            coreClusterAnchorPointThree
        );

        // Act.
        borderReassignmentEngine.cluster(reassignmentDataset);

        // Assert.
        assertEquals(0, potentialBorderNoisePoint.getClusterId());
    }

    /**
     * Verifies multi-cluster isolation with a true noise point positioned between clusters.
     */
    @Test
    @DisplayName("21. Three isolated clusters with intermediate noise point")
    void testMultipleClustersWithIsolatedNoise() {
        // Arrange.
        Dbscan multiClusterWithNoiseEngine = new Dbscan(0.5, 2);

        Point groupAlphaMemberOne = new Point(new double[]{0.0, 0.0});
        Point groupAlphaMemberTwo = new Point(new double[]{0.1, 0.0});
        Point unattachedIntermediateNoisePoint = new Point(new double[]{5.0, 5.0});
        Point groupBetaMemberOne = new Point(new double[]{10.0, 10.0});
        Point groupBetaMemberTwo = new Point(new double[]{10.1, 10.0});

        List<Point> complexDataset = List.of(
            groupAlphaMemberOne,
            groupAlphaMemberTwo,
            unattachedIntermediateNoisePoint,
            groupBetaMemberOne,
            groupBetaMemberTwo
        );

        // Act.
        multiClusterWithNoiseEngine.cluster(complexDataset);

        // Assert.
        assertEquals(0, groupAlphaMemberOne.getClusterId());
        assertEquals(0, groupAlphaMemberTwo.getClusterId());
        assertEquals(Point.NOISE, unattachedIntermediateNoisePoint.getClusterId());
        assertEquals(1, groupBetaMemberOne.getClusterId());
        assertEquals(1, groupBetaMemberTwo.getClusterId());
    }

    /**
     * Verifies algorithm idempotency when cluster() is executed multiple times on the same dataset.
     */
    @Test
    @DisplayName("22. Idempotency: Re-running cluster() on clustered dataset preserves state")
    void testClusteringIdempotency() {
        // Arrange.
        Dbscan idempotencyTestingEngine = new Dbscan(0.5, 2);

        List<Point> stableDataset = List.of(
            new Point(new double[]{0.0, 0.0}),
            new Point(new double[]{0.2, 0.2})
        );

        // Act.
        idempotencyTestingEngine.cluster(stableDataset);

        int initialExecutionClusterResult = stableDataset.get(0).getClusterId();

        idempotencyTestingEngine.cluster(stableDataset);

        int secondaryExecutionClusterResult = stableDataset.get(0).getClusterId();

        // Assert.
        assertEquals(initialExecutionClusterResult, secondaryExecutionClusterResult);
    }

    /**
     * Verifies clustering functionality in a single-dimensional vector space.
     */
    @Test
    @DisplayName("23. 1D spatial line dataset clustering")
    void testOneDimensionalClustering() {
        // Arrange.
        Dbscan singleDimensionEngine = new Dbscan(1.0, 2);

        Point linearCoordinatePointOne = new Point(new double[]{1.0});
        Point linearCoordinatePointTwo = new Point(new double[]{1.5});
        Point isolatedLinearNoisePoint = new Point(new double[]{10.0});

        List<Point> singleDimensionDataset = List.of(
            linearCoordinatePointOne, 
            linearCoordinatePointTwo, 
            isolatedLinearNoisePoint
        );

        // Act.
        singleDimensionEngine.cluster(singleDimensionDataset);

        // Assert.
        assertEquals(0, linearCoordinatePointOne.getClusterId());
        assertEquals(0, linearCoordinatePointTwo.getClusterId());
        assertEquals(Point.NOISE, isolatedLinearNoisePoint.getClusterId());
    }

    /**
     * Verifies clustering functionality in 4D hyper-space coordinates.
     */
    @Test
    @DisplayName("24. 4D hyper-space coordinate dataset clustering")
    void testFourDimensionalClustering() {
        // Arrange.
        Dbscan hyperSpaceClusteringEngine = new Dbscan(1.0, 2);

        Point fourDimensionalClusterPointOne = new Point(new double[]{1.0, 1.0, 1.0, 1.0});
        Point fourDimensionalClusterPointTwo = new Point(new double[]{1.2, 1.1, 0.9, 1.0});
        Point fourDimensionalNoisePoint = new Point(new double[]{9.0, 9.0, 9.0, 9.0});

        List<Point> hyperSpaceDataset = List.of(
            fourDimensionalClusterPointOne, 
            fourDimensionalClusterPointTwo, 
            fourDimensionalNoisePoint
        );

        // Act.
        hyperSpaceClusteringEngine.cluster(hyperSpaceDataset);

        // Assert.
        assertEquals(0, fourDimensionalClusterPointOne.getClusterId());
        assertEquals(0, fourDimensionalClusterPointTwo.getClusterId());
        assertEquals(Point.NOISE, fourDimensionalNoisePoint.getClusterId());
    }

    /**
     * Verifies that negative spatial coordinates are correctly processed in distance calculations.
     */
    @Test
    @DisplayName("25. Negative coordinate spatial clusters")
    void testNegativeCoordinates() {
        // Arrange.
        Dbscan negativeSpaceEngine = new Dbscan(0.5, 2);

        Point negativeQuadrantPointOne = new Point(new double[]{-5.0, -5.0});
        Point negativeQuadrantPointTwo = new Point(new double[]{-5.1, -4.9});
        Point positiveQuadrantNoisePoint = new Point(new double[]{5.0, 5.0});

        List<Point> CartesianPlaneDataset = List.of(
            negativeQuadrantPointOne, 
            negativeQuadrantPointTwo, 
            positiveQuadrantNoisePoint
        );

        // Act.
        negativeSpaceEngine.cluster(CartesianPlaneDataset);

        // Assert.
        assertEquals(0, negativeQuadrantPointOne.getClusterId());
        assertEquals(0, negativeQuadrantPointTwo.getClusterId());
        assertEquals(Point.NOISE, positiveQuadrantNoisePoint.getClusterId());
    }

    /**
     * Evaluates clustering performance across a larger 100-point continuous dataset.
     */
    @Test
    @DisplayName("26. Large dataset execution performance smoke test (100 points)")
    void testLargeDatasetSmokeTest() {
        // Arrange.
        Dbscan performanceTestingEngine = new Dbscan(0.5, 3);

        List<Point> highDensityDiagonalDataset = new ArrayList<>();

        for (int stepIndex = 0; stepIndex < 100; stepIndex++) {
            highDensityDiagonalDataset.add(new Point(new double[]{(double) stepIndex * 0.01, (double) stepIndex * 0.01}));
        }

        // Act & Assert.
        assertDoesNotThrow(() -> performanceTestingEngine.cluster(highDensityDiagonalDataset));
        assertEquals(0, highDensityDiagonalDataset.get(0).getClusterId());
        assertEquals(0, highDensityDiagonalDataset.get(99).getClusterId());
    }

    /**
     * Verifies clustering under varied neighborhood density requirements.
     */
    @Test
    @DisplayName("27. Asymmetric clusters with varying density thresholds")
    void testVaryingDensityClusters() {
        // Arrange.
        Dbscan strictDensityEngine = new Dbscan(1.0, 4);

        Point denseClusterMemberOne = new Point(new double[]{0.0, 0.0});
        Point denseClusterMemberTwo = new Point(new double[]{0.1, 0.0});
        Point denseClusterMemberThree = new Point(new double[]{0.0, 0.1});
        Point denseClusterMemberFour = new Point(new double[]{0.1, 0.1});

        Point sparseClusterMemberOne = new Point(new double[]{10.0, 10.0});
        Point sparseClusterMemberTwo = new Point(new double[]{10.1, 10.0});

        List<Point> varyingDensityDataset = List.of(
            denseClusterMemberOne, 
            denseClusterMemberTwo, 
            denseClusterMemberThree, 
            denseClusterMemberFour,
            sparseClusterMemberOne, 
            sparseClusterMemberTwo
        );

        // Act.
        strictDensityEngine.cluster(varyingDensityDataset);

        // Assert.
        assertEquals(0, denseClusterMemberOne.getClusterId());
        assertEquals(Point.NOISE, sparseClusterMemberOne.getClusterId());
    }

    /**
     * Verifies that the neighborhood point count for a core point includes the point itself.
     */
    @Test
    @DisplayName("28. Core point neighbor count includes the core point itself")
    void testCorePointIncludesSelfInNeighborCount() {
        // Arrange.
        Dbscan selfInclusionEngine = new Dbscan(1.0, 2);

        Point primaryInclusiveCorePoint = new Point(new double[]{0.0, 0.0});
        Point secondaryInclusiveCorePoint = new Point(new double[]{0.5, 0.0});

        List<Point> selfInclusionDataset = List.of(primaryInclusiveCorePoint, secondaryInclusiveCorePoint);

        // Act.
        selfInclusionEngine.cluster(selfInclusionDataset);

        // Assert.
        assertEquals(0, primaryInclusiveCorePoint.getClusterId());
        assertEquals(0, secondaryInclusiveCorePoint.getClusterId());
    }

    /**
     * Checks floating-point calculation boundaries near epsilon thresholds.
     */
    @Test
    @DisplayName("29. Floating point distance precision check near epsilon limit")
    void testFloatingPointPrecisionDistance() {
        // Arrange.
        Dbscan precisionTestingEngine = new Dbscan(0.3, 2);

        Point baselinePrecisionPoint = new Point(new double[]{0.1, 0.1});
        Point calculatedOffsetPoint = new Point(new double[]{0.1 + Math.sqrt(0.04), 0.1});

        List<Point> precisionTestDataset = List.of(baselinePrecisionPoint, calculatedOffsetPoint);

        // Act.
        precisionTestingEngine.cluster(precisionTestDataset);

        // Assert.
        assertEquals(0, baselinePrecisionPoint.getClusterId());
    }

    /**
     * Verifies that new cluster IDs are generated sequentially (0, 1, 2...).
     */
    @Test
    @DisplayName("30. Cluster IDs generated strictly increment sequentially (0, 1, 2...)")
    void testSequentialClusterIds() {
        // Arrange.
        Dbscan sequentialIdEngine = new Dbscan(0.5, 2);
        
        Point firstGroupMember = new Point(new double[]{0.0, 0.0});
        Point firstGroupPartner = new Point(new double[]{0.1, 0.0});

        Point secondGroupMember = new Point(new double[]{5.0, 5.0});
        Point secondGroupPartner = new Point(new double[]{5.1, 5.0});

        Point thirdGroupMember = new Point(new double[]{10.0, 10.0});
        Point thirdGroupPartner = new Point(new double[]{10.1, 10.0});

        List<Point> multiGroupDataset = List.of(
            firstGroupMember, firstGroupPartner,
            secondGroupMember, secondGroupPartner,
            thirdGroupMember, thirdGroupPartner
        );

        // Act.
        sequentialIdEngine.cluster(multiGroupDataset);

        // Assert.
        assertEquals(0, firstGroupMember.getClusterId());
        assertEquals(1, secondGroupMember.getClusterId());
        assertEquals(2, thirdGroupMember.getClusterId());
    }
}