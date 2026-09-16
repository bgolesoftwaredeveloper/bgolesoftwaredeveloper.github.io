"""
n_body.py

An implementation of a generalized, dimension-agnostic N-Body physics simulation
that models the gravitational interactions among multiple celestial bodies over time.

This module utilizes vectorized matrix operations via `numpy` to calculate pairwise
gravitational forces efficiently, bypassing nested Python loops. It implements a
Semi-implicit Euler integration scheme to dynamically update positions and velocities,
and integrates with `matplotlib` for real-time visualization.

Design Note:
    The force computation requires $O(N^2)$ operations, where $N$ is the number of
    bodies. To prevent infinite acceleration during close-range particle collisions,
    a small numeric "softening factor" is integrated into the denominator of the
    gravitational force formula.
"""
import numpy

class NBody:
    """
    Encapsulates the mathematical modeling, force computation, and physics
    updates for an arbitrary number of interacting bodies in multi-dimensional space.
    """

    def __init__(self, masses: list[float],
            positions: list[list[float]],
            velocities: list[list[float]],
            gravity: float = 1.0,
            epsilon: float = 0.1
    ):
        """
        Initializes the physical parameters and state matrices for the simulation.

        Args:
            masses (list[float]): Mass values for each individual body of shape (N,).
            positions (list[list[float]]): Initial spatial coordinates of shape (N, D).
            velocities (list[list[float]]): Initial velocity vectors of shape (N, D).
            gravity (float): Scaled universal gravitational constant (G). Defaults to 1.0.
            epsilon (float): Numeric softening factor to prevent singularities. Defaults to 0.1.
        """
        # Reshape masses to an (N, 1) matrix to enable column-wise matrix broadcasting.
        self.masses = numpy.array(masses, dtype=float).reshape(-1, 1)
        self.positions = numpy.array(positions, dtype=float)
        self.velocities = numpy.array(velocities, dtype=float)

        self.gravity = gravity
        self.softening = epsilon

    def update(self, step: float):
        """
        Advances the entire physical system forward by a discrete time delta
        using a semi-implicit symplectic Euler integration step.

        Args:
            step (float): The discrete change in time (dt) for this simulation frame.
        """
        accelerations = self.compute_acceleration()

        # Advance velocities first using the current acceleration vectors.
        self.velocities += accelerations * step

        # Advance positions using the newly calculated velocities.
        self.positions += self.velocities * step

    def compute_acceleration(self) -> numpy.ndarray:
        """
        Calculates the instantaneous gravitational acceleration vector for every
        body by computing all pairwise displacements simultaneously.

        Returns:
            numpy.ndarray: An acceleration matrix of shape (N, D) representing current forces.
        """
        # Create a pairwise displacement matrix.
        displacements = self.positions[numpy.newaxis, :, :] - self.positions[:, numpy.newaxis, :]

        # Calculate the scalar distance squared between every pair of particles.
        distance_squared = numpy.sum(displacements ** 2, axis=-1)

        # Apply the softening factor to smoothly handle overlapping or colliding bodies.
        distance_squared += self.softening ** 2

        # Convert distances into the inverse cubed scale needed for Newton's law of gravity.
        inverse_distance_cubed = distance_squared ** (-1.5)

        # Determine the individual scalars using mass broadcasting.
        acceleration_magnitudes = self.gravity * self.masses.T * inverse_distance_cubed

        # Project scalar forces back onto displacement directions and sum along the acting axes.
        return numpy.sum(acceleration_magnitudes[:, :, numpy.newaxis] * displacements, axis=1)