"""
main.py

An execution script that initializes and animates a multi-body celestial system.

This module sets up a representative three-body orbital configuration consisting
of a massive central star and two planetary bodies in a 2D plane. It handles
the graphical canvas allocation, registers history tracking buffers for orbital
paths, and runs the active animation loop using the `NBody` physics simulation engine.
"""
import matplotlib.pyplot as plot
from matplotlib.animation import FuncAnimation

from n_body import NBody

def main():
    # Define the necessary parameters for the N-Body simulation.
    masses = [1000.0, 1.0, 5.0]

    positions = [
        [0.0, 0.0],
        [0.0, 10.0],
        [0.0, -18.0]
    ]

    velocities = [
        [0.0, -0.05],
        [10.0, 0.0],
        [-7.5, 0.0]
    ]

    # Instantiate the core N-Body physics simulator with our initial state vectors.
    simulation = NBody(masses, positions, velocities, 1.0, 0.1)

    # Configure the structural layout and boundaries of the rendering window.
    figure, axes = plot.subplots(figsize=(6, 6))

    axes.set_xlim(-25, 25)
    axes.set_ylim(-25, 25)
    axes.set_aspect('equal')

    axes.grid(True, linestyle='--', alpha=0.5)

    # Extract initial X and Y coordinates so the shapes match the size.
    x = [position[0] for position in positions]
    y = [position[1] for position in positions]

    # Pre-allocate linear list buffers to log historical coordinates for path trails.
    history_x = [[] for _ in range(len(masses))]
    history_y = [[] for _ in range(len(masses))]

    # Construct the graphic visual elements (scatter points for bodies, lines for paths).
    scatter = axes.scatter(x, y, c=['gold', 'deepskyblue', 'crimson'], s=[100, 30, 50])
    lines = [axes.plot([], [], alpha=0.4, lw=1, c=color)[0] for color in ['gold', 'deepskyblue', 'crimson']]

    def animate(frame: int) -> list:
        """
        Steps the physical system forward and updates the positions of all graphic elements.
        """
        # Step the system forward by a tiny physical time slice delta (dt = 0.02).
        simulation.update(step=0.02)

        # Redraw the current geometric coordinates of the body nodes.
        scatter.set_offsets(simulation.positions)

        # Append active vector positions to the history and truncate old indices to cap trail lengths.
        for index in range(len(masses)):
            history_x[index].append(simulation.positions[index, 0])
            history_y[index].append(simulation.positions[index, 1])

            if len(history_x[index]) > 100:
                history_x[index].pop(0)
                history_y[index].pop(0)

            lines[index].set_data(history_x[index], history_y[index])

        return [scatter] + lines

    # Initiate the frame-by-frame loop wrapper to process and render the active timeline.
    animation = FuncAnimation(figure, animate, frames=200, interval=20, blit=True)

    plot.show()

if __name__ == "__main__":
    main()