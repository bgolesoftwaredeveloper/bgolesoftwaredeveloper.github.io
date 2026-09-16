"""
main.py

An execution wrapper and entry point designed to initialize a training dataset
and trigger the ID3 decision tree induction and visualization pipeline.

This script instantiates the classic Quinlan 'PlayTennis' discrete categorical
matrix, maps the underlying feature and target variable dimensions, and passes
the resulting dataframe structure to the external decision tree builder module.

Design Note:
    This module serves as a localized runtime environment. It abstracts data
    ingestion and formats tabular training slices into structures compatible
    with non-parametric machine learning routines.
"""
import pandas

from decisiontree import build_tree, show

def main():
    # Define a static mock dictionary representing categorical weather observation states.
    weather_data = {
        "Outlook": [
            "Sunny",
            "Sunny",
            "Overcast",
            "Rain",
            "Rain",
            "Rain",
            "Overcast",
            "Sunny",
            "Sunny",
            "Rain",
            "Sunny",
            "Overcast",
            "Overcast",
            "Rain",
        ],
        "Temperature": [
            "Hot",
            "Hot",
            "Hot",
            "Mild",
            "Cool",
            "Cool",
            "Cool",
            "Mild",
            "Cool",
            "Mild",
            "Mild",
            "Mild",
            "Hot",
            "Mild",
        ],
        "Humidity": [
            "High",
            "High",
            "High",
            "High",
            "Normal",
            "Normal",
            "Normal",
            "High",
            "Normal",
            "Normal",
            "Normal",
            "High",
            "Normal",
            "High",
        ],
        "Wind": [
            "Weak",
            "Strong",
            "Weak",
            "Weak",
            "Weak",
            "Strong",
            "Strong",
            "Weak",
            "Weak",
            "Weak",
            "Strong",
            "Strong",
            "Weak",
            "Strong",
        ],
        "Play": [
            "No",
            "No",
            "Yes",
            "Yes",
            "Yes",
            "No",
            "Yes",
            "No",
            "Yes",
            "Yes",
            "Yes",
            "Yes",
            "Yes",
            "No",
        ],
    }

    # Transform the raw structural dictionary records into a unified pandas DataFrame matrix.
    data_frame = pandas.DataFrame(weather_data)

    #  Delineate independent discrete attribute dimensions within the feature space.
    feature_columns = ["Outlook", "Temperature", "Humidity", "Wind"]

    # Establish the explicit column target string mapping for dependent classification labels.
    target_column = "Play"

    print("Building ID3 Decision Tree Model...\n")

    # Execute the external top-down recursive induction algorithm using the formatted training data.
    root = build_tree(data_frame, feature_columns, target_column)

    # Traverse the newly synthesized hierarchical node graph to render an ASCII layout to stdout.
    show(root)

if __name__ == "__main__":
    main()
