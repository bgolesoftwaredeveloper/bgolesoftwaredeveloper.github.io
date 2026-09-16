"""
main.py

An operational execution entry point designed to demonstrate, benchmark, and
stress-test the mathematical accuracy of the HyperLogLog cardinality estimation
algorithm.

This script instantiates an estimator instance, injects a controlled stream of
synthetic unique data strings intermixed with redundant duplicate elements, and
extracts statistical variance measurements. It validates structural resilience
under heavy load and calculates the true error margin deviation against theoretical
algorithmic boundaries.
"""
from __future__ import annotations

import sys

from hyperloglog import HyperLogLog

def main() -> None:
    print("=" * 60)
    print("Starting HyperLogLog Cardinality Estimation Demonstration")
    print("=" * 60)

    # Step 1: Instantiate an estimator with a standard target error rate (e.g., 2%).
    target_error = 0.02
    
    try:
        hyperloglog = HyperLogLog(error_rate=target_error)
    except (TypeError, ValueError) as exc:
        print(f"Critical Initialization Failure: {exc}")
        sys.exit(1)

    # Step 2: Define tracking constraints for the data stream simulation.
    unique_element_count = 50_000
    duplicate_factor = 3

    print(f"[Status] Constructing element stream...")
    print(f"\tTargeting True Unique Count : {unique_element_count:,}")
    print(f"\tTotal Duplications Per Item: {duplicate_factor}")

    # Step 3: Stream items into the tracking arrays sequentially.
    # We purposefully add duplicates to prove that HyperLogLog filters out redundant data signatures.
    for step in range(unique_element_count):
        unique_token = f"user_session_token_identity_hash_{step}"

        # Inject the unique item multiple times.
        for _ in range(duplicate_factor):
            hyperloglog.add(unique_token)

    # Step 4: Extract the statistical estimation and evaluate mathematical error.
    estimated_cardinality = hyperloglog.estimate()
    absolute_variance = abs(estimated_cardinality - unique_element_count)
    observed_error_rate = (absolute_variance / unique_element_count) * 100

    # Step 5: Render structural diagnostics and summary analytics.
    hyperloglog.display()

    print("-- Runtime Accuracy Analysis Evaluation --")
    print(f"\tTrue Answer Invariant:\t\t{unique_element_count:,} items")
    print(f"\tHLL Computed Answer:\t\t{estimated_cardinality:,} items")
    print(f"\tAbsolute Discrepancy:\t\t{absolute_variance:,} items")
    print(f"\tObserved Error Rate:\t\t{observed_error_rate:.3f}%")
    print(f"\tConfigured Error Bound:\t\t{target_error * 100:.3f}%")

    if observed_error_rate <= (target_error * 100):
        print("\n\tResult: SUCCESS (Observed error falls within theoretical constraints).")
    else:
        print("\n\tResult: MARGINAL (Observed stochastic error slightly exceeds target variance).")
    print("=" * 60)

if __name__ == "__main__":
    main()