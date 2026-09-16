"""
main.py

An implementation of a numerical simulation driver configured for the chaotic
evolution and real-time visualization of a double pendulum system.

This module provides an operational framework for executing numerical integration
of non-linear coupled differential equations modeling a two-body planar pendulum.
It explicitly couples SciPy's advanced initial-value problem solvers alongside
Matplotlib's animation subsystem to synthesize dynamic frame sequences of the
structural kinematics, trajectory history, and tip path trails.

Design Note:
    Due to thread-blocking behavior and event-loop interception native to many
    integrated development environment (IDE) graphical backends (e.g., PyCharm SciView,
    VS Code Interactive View), live blitting operations may fail silently. To preserve
    hardware-accelerated live display pipelines, execution should be explicitly
    directed from a native shell terminal prompt:

        $ python3 main.py
"""
import numpy
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from scipy.integrate import solve_ivp

import double_pendulum as dp

def main():
    print("Step 1/2: Running numerical physics integration...")

    grid = numpy.linspace(dp.TIME_START, dp.TIME_END, dp.TOTAL_FRAMES)

    solution = solve_ivp(
        fun=dp.compute_pendulum_derivatives,
        t_span=[dp.TIME_START, dp.TIME_END],
        y0=dp.INITIAL_STATE,
        t_eval=grid,
        method="Radau"
    )

    # Correct indices to extract tracking angles (0 and 2).
    upper_angle_history = solution.y[0]
    lower_angle_history = solution.y[2]

    # Convert angular positions to Cartesian coordinates (hanging down).
    upper_node_x = dp.upper_arm_length * numpy.sin(upper_angle_history)
    upper_node_y = -dp.upper_arm_length * numpy.cos(upper_angle_history)

    lower_node_x = upper_node_x + dp.lower_arm_length * numpy.sin(lower_angle_history)
    lower_node_y = upper_node_y - dp.lower_arm_length * numpy.cos(lower_angle_history)

    print("Step 2/2: Launching animation canvas window...")

    figure_canvas, axis_plot = plt.subplots(figsize=(6, 6))

    boundary = (dp.upper_arm_length + dp.lower_arm_length) * 1.2

    axis_plot.set_xlim(-boundary, boundary)
    axis_plot.set_ylim(-boundary, boundary)
    axis_plot.set_aspect("equal")
    axis_plot.grid(True, linestyle=":", alpha=0.6)
    axis_plot.set_title("Double Pendulum Simulation")

    # Structural components (rods and mass bobs).
    (linkage_render_line,) = axis_plot.plot([], [], "o-", lw=2, color="#2c3e50", markerfacecolor="#e74c3c", markersize=8)
    (motion_trail_line,) = axis_plot.plot([], [], "-", lw=1, color="#3498db", alpha=0.6)

    tip_path_history_x, tip_path_history_y = [], []

    def update_animation_frame(frame_step):
        coordinate_chain_x = [0, upper_node_x[frame_step], lower_node_x[frame_step]]
        coordinate_chain_y = [0, upper_node_y[frame_step], lower_node_y[frame_step]]

        linkage_render_line.set_data(coordinate_chain_x, coordinate_chain_y)

        tip_path_history_x.append(lower_node_x[frame_step])
        tip_path_history_y.append(lower_node_y[frame_step])

        if len(tip_path_history_x) > 150:
            tip_path_history_x.pop(0)
            tip_path_history_y.pop(0)

        motion_trail_line.set_data(tip_path_history_x, tip_path_history_y)

        return linkage_render_line, motion_trail_line

    pendulum_animation = animation.FuncAnimation(
        fig=figure_canvas,
        func=update_animation_frame,
        frames=dp.TOTAL_FRAMES,
        interval=1000 / dp.FRAME_RATE,
        blit=False,
        repeat=False,
    )

    return pendulum_animation

if __name__ == "__main__":
    handler = main()

    # Plot the results.
    plt.pause(0.001)
    plt.show()