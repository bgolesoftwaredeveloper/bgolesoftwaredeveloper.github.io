import numpy
import time

from kd_tree import KDTree

def main():
    # 1. Define dataset dimensions and cardinality.
    number_of_points = 100_000
    number_of_dimensions = 3

    print(f"Generating {number_of_points:,} random {number_of_dimensions}D points...")

    # 2. Generate random coordinate dataset and single query point.
    dataset = numpy.random.rand(number_of_points, number_of_dimensions).astype(numpy.float64)
    point = numpy.random.rand(number_of_dimensions).astype(numpy.float64)

    # 3. Construct the KDTree and record construction latency.
    timer = time.perf_counter()

    tree = KDTree(dataset)

    milliseconds = (time.perf_counter() - timer) * 1000

    print(f"KDTree constructed in: {milliseconds:.2f} ms")

    # 4. Query spatial structure for exact nearest neighbor.
    timer = time.perf_counter()

    index, distance = tree.nearest_neighbor(point)

    milliseconds = (time.perf_counter() - timer) * 1000

    print(f"Nearest neighbor query executed in: {milliseconds:.4f} ms")

    # 5. Compute brute-force exact linear scan to verify correctness.
    differences = dataset - point
    distances_squared = numpy.sum(differences * differences, axis=1)

    expected_index = numpy.argmin(distances_squared)
    expected_distance = numpy.sqrt(distances_squared[expected_index])

    # Summary and verification.
    print()
    print("--- Validation Results ---")
    print(f"KDTree Search    -> Index: {index}, Distance: {distance:.8f}")
    print(f"Brute Force Scan -> Index: {expected_index}, Distance: {expected_distance:.8f}")

    if index != expected_index:
        raise ValueError("Verification Failed: Indices do not match!")

    if not numpy.isclose(distance, expected_distance):
        raise ValueError("Verification Failed: Distances do not match!")

    print("\nSUCCESS: KDTree output matches exact brute-force search!")

if __name__ == "__main__":
    main()
