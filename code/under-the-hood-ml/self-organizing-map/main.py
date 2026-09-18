"""
main.py

Demonstration script for training and evaluating a Self-Organizing Map (SOM).

This module initializes a 2D topological lattice grid, trains the network on 
a synthetically generated continuous 3D RGB color space dataset, and projects 
known benchmark color vectors to locate their respective Best Matching Unit (BMU) 
coordinates on the converged map.
"""
from self_organizing_map import SelfOrganizingMap

import numpy

def main():
    # Set target lattice grid dimensions and feature dimensions RGH components.
    rows = 10
    columns = 10
    dimensions = 3
    maximum_iterations = 1000

    # Define reference target vectors representing pure color endpoints.
    primary_color_samples = numpy.array(
        [
            [1.0, 0.0, 0.0],
            [0.0, 1.0, 0.0],
            [0.0, 0.0, 1.0],
            [1.0, 1.0, 0.0],
            [0.0, 1.0, 1.0],
            [1.0, 0.0, 1.0],
        ]
    )

    # Generate a uniform random continuous color dataset for unsupervised training.
    dataset = numpy.random.rand(200, dimensions)

    # Instantiate the SOM grid engine with initial learning rate.
    som = SelfOrganizingMap(rows, columns, dimensions, learning_rate=0.5)

    print("Beginning training process...")

    # Execute competitive learning iterations across the dataset.
    som.train(dataset, maximum_iterations)

    # Map evaluation benchmark color sampole to their converged BMU grid locations.
    assigned_coordinates_list = (
        som.map_dataset_to_grid(primary_color_samples)
    )

    color_labels = [
        "Red",
        "Green",
        "Blue",
        "Yellow",
        "Cyan",
        "Magenta",
    ]

    print("Mapped primary colors to grid coordinates.")

    # Print resulting topological mapping grid node coordinates.
    for label, coordinate in zip(color_labels, assigned_coordinates_list):
        print(f"\t{label:7s} -> BMU Grid Node: {coordinate}.")

if __name__ == "__main__":
    main()