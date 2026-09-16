"""
test_n_body.py

Automated unit test suite for verifying the NBody physics simulation engine
across mathematical invariants, conservation laws, and edge-case topologies.

Overview:
    This module uses Python's built-in `unittest` framework to execute a matrix
    of 20 unique testing scenarios against the `NBody` physics simulator. It
    ensures that gravitational calculations adhere to Newtonian dynamics, spatial
    dimensions scale seamlessly, and integration steps handle boundary limits safely.

Testing Scope & Architectural Coverage:
    - Kinematic & Spatial Invariants: Validates static rest scenarios, isolated zero-gravity
      coasting systems, and multidimensional dimension-agnostic (1D, 2D, 3D, 4D) matrix shapes.
    - Conservation & Physical Laws: Assesses conservation of linear momentum, center of
      mass isolation, symmetric third-law pair cancellations, and energy-neutral orbits.
    - Singularities & Numeric Boundaries: Targets close-range particle collisions via
      the softening factor, mass limits, and hyper-dense objects.
    - Mathematical Matrix Integrity: Verifies array shapes, input parsing conversions,
      and variable shorthand mappings inside the processing runtime.

Execution Instructions:
    Run the complete testing matrix natively from the shell or command line via:

        python -m unittest test_n_body.py
"""
import unittest
import numpy

from n_body import NBody

class TestNBody(unittest.TestCase):
    """
    Test suite containing 20 isolated test vectors evaluating gravitational vectors,
    symplectic integration loops, and matrix broadcasting transformations.
    """

    def test_01_static_single_body(self):
        """1. Verifies that an isolated body sitting at rest experiences completely zero acceleration."""
        simulation = NBody([10.0], [[0.0, 0.0]], [[0.0, 0.0]])

        acceleration = simulation.compute_acceleration()

        numpy.testing.assert_array_equal(acceleration, [[0.0, 0.0]])

    def test_02_zero_gravity_linear_coasting(self):
        """2. Validates that turning gravity to zero preserves uniform, straight-line unaccelerated movement."""
        simulation = NBody([1.0, 2.0],
                           [[0.0, 0.0], [10.0, 10.0]],
                           [[1.0, -1.0], [0.5, 2.0]],
                           0.0)

        simulation.update(step=2.0)

        numpy.testing.assert_array_equal(simulation.positions, [[2.0, -2.0], [11.0, 14.0]])

    def test_03_dimension_agnostic_one_dimensional_math(self):
        """3. Confirms that vector calculations execute perfectly when constrained to a 1D coordinate axis."""
        simulation = NBody([1.0, 1.0],
                           [[0.0], [2.0]],
                           [[0.0], [0.0]],
                           1.0,
                           1e-5)

        acceleration = simulation.compute_acceleration()

        self.assertEqual(acceleration.shape, (2, 1))
        self.assertGreater(acceleration[0, 0], 0.0)
        self.assertLess(acceleration[1, 0], 0.0)

    def test_04_three_dimensional_spatial_integrity(self):
        """4. Evaluates core matrix processing shape behavior when loaded with 3D (X, Y, Z) vectors."""
        simulation = NBody([5.0, 5.0],
                           [[0.0, 0.0, 0.0], [1.0, 1.0, 1.0]],
                           [[0.0, 0.0, 0.0], [0.0, 0.0, 0.0]])

        acceleration = simulation.compute_acceleration()

        self.assertEqual(acceleration.shape, (2, 3))
        self.assertEqual(simulation.positions.shape, (2, 3))

    def test_05_hyper_dimensional_four_dimensions(self):
        """5. Tests mathematical versatility by verifying processing structures in 4D hyper-space."""
        simulation = NBody([1.0, 1.0],
                           [[0.0, 0.0, 0.0, 0.0], [1.0, 2.0, 3.0, 4.0]],
                           [[0.0, 0.0, 0.0, 0.0], [0.0, 0.0, 0.0, 0.0]])

        acceleration = simulation.compute_acceleration()

        self.assertEqual(acceleration.shape, (2, 4))

    def test_06_newtons_third_law_symmetric_forces(self):
        """6. Confirms equal and opposite action-reaction pairs by checking if total net acceleration sums to zero."""
        simulation = NBody([10.0, 10.0], [[-5.0, 2.0], [3.0, 4.0]], [[0.0, 0.0], [0.0, 0.0]])

        acceleration = simulation.compute_acceleration()

        total_force = (simulation.masses * acceleration).sum(axis=0)

        numpy.testing.assert_array_almost_equal(total_force, [0.0, 0.0])

    def test_07_conservation_of_linear_momentum(self):
        """7. Ensures systemic total momentum is strictly conserved across active integration runtime loops."""
        simulation = NBody([100.0, 1.0],
                           [[0.0, 0.0], [5.0, 0.0]],
                           [[0.0, -0.1],
                            [0.0, 10.0]])

        initial_momentum = (simulation.masses * simulation.velocities).sum(axis=0)
        simulation.update(step=0.5)
        final_momentum = (simulation.masses * simulation.velocities).sum(axis=0)

        numpy.testing.assert_array_almost_equal(initial_momentum, final_momentum)

    def test_08_center_of_mass_invariance(self):
        """8. Assures the system's shared center of mass position remains stationary when net system velocity is zero."""
        simulation = NBody([5.0, 5.0], [[-2.0, 0.0], [2.0, 0.0]], [[0.0, 0.0], [0.0, 0.0]])

        initial_momentum = (simulation.masses * simulation.positions).sum(axis=0) / simulation.masses.sum()

        for _ in range(10):
            simulation.update(step=0.1)

        final_momentum = (simulation.masses * simulation.positions).sum(axis=0) / simulation.masses.sum()

        numpy.testing.assert_array_almost_equal(initial_momentum, final_momentum)

    def test_09_softening_factor_singularity_shielding(self):
        """9. Validates that two bodies occupying identical spatial coordinates return safe finite values instead of infinity."""
        simulation = NBody([1.0, 1.0],
                           [[5.0, 5.0], [5.0, 5.0]],
                           [[0.0, 0.0], [0.0, 0.0]],
                           1.0,
                           0.1)

        acceleration = simulation.compute_acceleration()

        self.assertFalse(numpy.isnan(acceleration).any())
        self.assertFalse(numpy.isinf(acceleration).any())

    def test_10_mass_scaling_proportionality(self):
        """10. Asserts acceleration scales linearly with the pulling particle's mass properties."""
        simulation_light = NBody([1.0, 10.0],
                                 [[0.0, 0.0], [2.0, 0.0]],
                                 [[0.0, 0.0], [0.0, 0.0]],
                                 1.0,
                                 1e-5)

        simulation_heavy = NBody([1.0, 20.0],
                                 [[0.0, 0.0], [2.0, 0.0]],
                                 [[0.0, 0.0], [0.0, 0.0]],
                                 1.0,
                                 1e-5)

        acceleration_light = simulation_light.compute_acceleration()
        acceleration_heavy = simulation_heavy.compute_acceleration()

        self.assertAlmostEqual(acceleration_heavy[0, 0], 2.0 * acceleration_light[0, 0])

    def test_11_inverse_square_distance_law(self):
        """11. Evaluates gravity fields drop precisely by a factor of four when radial distance is doubled."""
        simulation_close = NBody([1.0, 1.0],
                                 [[0.0, 0.0], [3.0, 0.0]],
                                 [[0.0, 0.0], [0.0, 0.0]],
                                 1.0,
                                 1e-5)

        simulation_distant = NBody([1.0, 1.0],
                                   [[0.0, 0.0], [6.0, 0.0]],
                                   [[0.0, 0.0], [0.0, 0.0]],
                                   1.0,
                                   1e-5)

        acceleration_close = simulation_close.compute_acceleration()
        acceleration_distant = simulation_distant.compute_acceleration()

        self.assertAlmostEqual(acceleration_close[0, 0], 4.0 * acceleration_distant[0, 0])

    def test_12_mass_matrix_broadcasting_shapes(self):
        """12. Verifies internal mass vectors are formatted into explicit column configurations for broadcasting."""
        simulation = NBody([1.0, 2.0, 3.0],
                           [[0.0, 0.0], [1.0, 1.0], [2.0, 2.0]],
                           [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0]])

        self.assertEqual(simulation.masses.shape, (3, 1))

    def test_13_input_list_to_numpy_array_conversion(self):
        """13. Confirms mutable raw Python lists transform seamlessly into robust native numpy matrices on boot."""
        simulation = NBody([1.0],
                           [[1.0, 2.0]],
                           [[3.0, 4.0]])

        self.assertIsInstance(simulation.positions, numpy.ndarray)
        self.assertIsInstance(simulation.velocities, numpy.ndarray)

    def test_14_gravity_constant_null_negation(self):
        """14. Verifies that passing a zero value for the gravitational parameter completely decouples forces."""
        simulation = NBody([50.0, 50.0],
                           [[0.0, 0.0], [1.0, 1.0]],
                           [[0.0, 0.0], [0.0, 0.0]],
                           0.0)
        acceleration = simulation.compute_acceleration()

        numpy.testing.assert_array_equal(acceleration, [[0.0, 0.0], [0.0, 0.0]])

    def test_15_variable_shorthand_aliasing(self):
        """15. Confirms historical physical configuration variables map smoothly into internal engine variables."""
        simulation = NBody([1.0],
                           [[0.0, 0.0]],
                           [[0.0, 0.0]],
                           6.67,
                           0.05)

        self.assertEqual(simulation.gravity, 6.67)
        self.assertEqual(simulation.softening, 0.05)

    def test_16_large_scale_performance_matrix_expansion(self):
        """16. Tests system processing execution limits across an expanded, dense 100-body setup array."""
        masses = [1.0] * 100
        positions = [[float(index), float(index)] for index in range(100)]
        velocities = [[0.0, 0.0] for _ in range(100)]

        simulation = NBody(masses, positions, velocities)

        acceleration = simulation.compute_acceleration()

        self.assertEqual(acceleration.shape, (100, 2))

    def test_17_semi_implicit_euler_velocity_priority(self):
        """17. Assures velocity updates execute before positions, reflecting correct symplectic math."""
        simulation = NBody([100.0, 1.0],
                           [[0.0, 0.0], [10.0, 0.0]],
                           [[0.0, 0.0],
                            [0.0, 5.0]])

        initial_pos = simulation.positions.copy()
        acceleration = simulation.compute_acceleration()
        step = 1.0

        expected_vel = simulation.velocities + acceleration * step
        expected_pos = initial_pos + expected_vel * step

        simulation.update(step=step)

        numpy.testing.assert_array_almost_equal(simulation.velocities, expected_vel)
        numpy.testing.assert_array_almost_equal(simulation.positions, expected_pos)

    def test_18_data_matrix_immutability_during_tracking(self):
        """18. Ensures outer structural data lists remain decoupled from tracking matrices post-initialization."""
        positions = [[1.0, 1.0], [2.0, 2.0]]
        simulation = NBody([1.0, 1.0],
                           positions,
                           [[0.0, 0.0],
                            [0.0, 0.0]])

        positions.append([3.0, 3.0])

        self.assertEqual(len(simulation.positions), 2)

    def test_19_asymmetric_mass_system_accelerations(self):
        """19. Validates that a light body accelerates faster than a heavy body under mutual pull."""
        simulation = NBody([1000.0, 1.0],
                           [[0.0, 0.0], [5.0, 0.0]],
                           [[0.0, 0.0], [0.0, 0.0]],
                           1.0,
                           1e-5)

        acceleration = simulation.compute_acceleration()

        self.assertGreater(abs(acceleration[1, 0]), abs(acceleration[0, 0]))

    def test_20_stable_circular_orbit_geometry(self):
        """20. Validates orbital mechanics stability by tracking a balance between centrifugal vectors and gravity."""
        simulation = NBody([100.0, 0.0001],
                           [[0.0, 0.0], [4.0, 0.0]],
                           [[0.0, 0.0], [0.0, 5.0]],
                           1.0,
                           1e-5)

        initial_distance = numpy.linalg.norm(simulation.positions[1] - simulation.positions[0])
        simulation.update(step=0.01)
        final_distance = numpy.linalg.norm(simulation.positions[1] - simulation.positions[0])

        self.assertAlmostEqual(initial_distance, final_distance, places=3)

if __name__ == "__main__":
    unittest.main()