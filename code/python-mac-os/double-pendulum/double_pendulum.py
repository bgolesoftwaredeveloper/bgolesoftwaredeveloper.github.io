"""
double_pendulum.py

An implementation of the Lagrangian equations of motion for a double pendulum system
configured for numerical integration.

This module provides the core state-space differential equations describing the chaotic
dynamics of a planar, two-link compound pendulum. It sets up foundational physical parameters
(mass, length, gravity) and configuration bounds, deriving instantaneous angular velocities
and accelerations from ordinary differential equations (ODEs). These derivatives are formatted
specifically to interface with SciPy's numerical initial-value problem solvers.
"""
import numpy

# Global physical parameters and system constraints.
GRAVITY = 9.81
TIME_START = 0.0
TIME_END = 20.0
FRAME_RATE = 60.0
TOTAL_FRAMES = int((TIME_END - TIME_START) * FRAME_RATE)

upper_arm_length = 1.0
upper_bob_mass = 1.0

lower_arm_length = 1.0
lower_bob_mass = 1.0

# Fixed initial state coordination.
INITIAL_STATE = [numpy.radians(90), 0.0, numpy.radians(90), 0.0]

def compute_pendulum_derivatives(time: float, state: list[float]) -> list[float]:
    """
    Computes the instantaneous state derivatives of the double pendulum system.

    This function solves for the angular accelerations (alpha1, alpha2) by evaluating
    non-linear coupled ordinary differential equations derived from the system's
    Lagrangian. It maps the current state vector back to its respective time-derivatives
    to advance the numerical integration step.

    Args:
        time (float): The current simulation time step (t), required by solve_ivp.
        state (list[float]): A 4-element array containing the current system state:
            - state[0] (float): Upper link angular position (theta1) in radians.
            - state[1] (float): Upper link angular velocity (omega1) in rad/s.
            - state[2] (float): Lower link angular position (theta2) in radians.
            - state[3] (float): Lower link angular velocity (omega2) in rad/s.

    Returns:
        list[float]: A 4-element array tracking the rates of change [d_theta1, d_omega1, d_theta2, d_omega2]:
            - index 0 (float): Current upper angular velocity (omega1).
            - index 1 (float): Calculated upper angular acceleration (alpha1).
            - index 2 (float): Current lower angular velocity (omega2).
            - index 3 (float): Calculated lower angular acceleration (alpha2).
    """
    upper_angle, upper_velocity, lower_angle, lower_velocity = state
    delta = upper_angle - lower_angle

    angle_differences = upper_angle - lower_angle

    # Denominator terms derived from Lagrangian mechanics.
    primary_denominator = upper_arm_length * (
        2 * upper_bob_mass + lower_bob_mass - lower_bob_mass * numpy.cos(2 * upper_angle - 2 * lower_angle)
    )

    secondary_denominator = lower_arm_length * (
        2 * upper_bob_mass + lower_bob_mass - lower_bob_mass * numpy.cos(2 * upper_angle - 2 * lower_angle)
    )

    upper_numerator = (
        -GRAVITY * (2 * upper_bob_mass + lower_bob_mass) * numpy.sin(upper_angle)
        - lower_bob_mass * GRAVITY * numpy.sin(upper_angle - 2 * lower_angle)
        - 2 * numpy.sin(delta) * lower_bob_mass * (
            lower_velocity**2 * lower_arm_length
            + upper_velocity**2 * upper_arm_length * numpy.cos(delta)
        )
    )

    upper_acceleration = upper_numerator / primary_denominator

    lower_numerator = 2 * numpy.sin(angle_differences) * (
            upper_velocity ** 2 * upper_arm_length * (upper_bob_mass + lower_bob_mass)
            + GRAVITY * (upper_bob_mass + lower_bob_mass) * numpy.cos(upper_angle)
            + lower_velocity ** 2 * lower_arm_length * lower_bob_mass * numpy.cos(angle_differences)
    )

    lower_acceleration = lower_numerator / secondary_denominator

    return [upper_velocity, upper_acceleration, lower_velocity, lower_acceleration]