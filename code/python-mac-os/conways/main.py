"""
main.py

The main orchestration and control loop environment for running Conway's Game of
Life simulation.

This script manages the lifecycle of the cellular automation board, handling
sequential generation loops, checking for thermodynamic stabilization thresholds
(stagnation metrics), regulating rendering frames per second, and catching safe
exit interrupt flags triggered via user hardware signals.
"""
import time

from conways import initialize_grid, compute_next_generation, display

def main():
    # Configure grid dimensions and execution boundary limits.
    NUMBER_OF_ROWS = 10
    NUMBER_OF_COLUMNS = 20
    GENERATION_DELAY_SECONDS = 0.01
    MAXIMUM_GENERATIONS = 100

    # Allocate memory and inject the localized "Glider" starter sequence.
    grid = initialize_grid(NUMBER_OF_ROWS, NUMBER_OF_COLUMNS)

    # Tracking pointer to monitor runtime duration steps.
    generation_count = 0

    try:
        # Prevent runaway processing by bounding the loop to maximum allowed generations.
        while generation_count < MAXIMUM_GENERATIONS:
            # Refresh terminal and draw current state vectors.
            display(grid)

            # Evaluate neighbor configurations and derive the next matrix iteration.
            next_grid = compute_next_generation(grid, NUMBER_OF_ROWS, NUMBER_OF_COLUMNS)

            # Increment life epoch tracker.
            generation_count += 1

            # Stagnation check: Halt if the map reaches equilibrium and stops changing.
            if next_grid == grid:
                print("Simulation stabilized.")

                break

            # Advance state reference buffer for the upcoming calculation pass.
            grid = next_grid

            # Frame-rate throttle to keep update comfortably visible to the human eye.
            time.sleep(GENERATION_DELAY_SECONDS)
    except KeyboardInterrupt:
        print("Simulation stopped.")

if __name__ == "__main__":
    main()