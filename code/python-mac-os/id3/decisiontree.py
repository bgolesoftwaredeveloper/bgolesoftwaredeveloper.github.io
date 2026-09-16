"""
decisiontree.py

An implementation of the Iterative Dichotomiser 3 (ID3) decision tree algorithm
configured for discrete categorical classification tasks.

This module provides non-parametric supervised learning workflows for constructing,
visualizing, and evaluating hierarchical decision topologies. It explicitly combines
Shannon entropy calculations with information gain heuristics to determine optimal
feature space partitioning under deterministic splitting criteria.

Design Note:
    This implementation is optimized for educational evaluation and structural reference.
    It natively evaluates categorical data features via alphanumeric vector slices
    and lacks built-in optimizations for continuous numeric boundaries or post-pruning.
"""
import math
import numpy

class DecisionTree:
    """
    An engine responsible for managing structural node topologies, branch attributes,
    and evaluating split criteria within discrete classification pipelines.
    """

    def __init__(self, feature=None, value=None, results=None, children=None):
        """
        Initializes an independent decision node or leaf state inside the tree topology.

        Args:
            feature (str, optional): The name of the attribute split executed at this node.
            value (str, optional): The specific incoming branch condition matching the parent split.
            results (str, optional): The final class prediction value; populated exclusively at leaf states.
            children (dict, optional): A dictionary mapping branch conditions to nested child nodes.
        """
        # The categorical dimension or feature space column selected for evaluation.
        self.feature = feature

        # The contextual value from the antecedent node split that routes execution here.
        self.value = value

        # The discrete class label assignment (remains unpopulated unless this is a terminal leaf).
        self.results = results

        # A dictionary mapping localized branch keys to downstream DecisionTree nodes.
        self.children = children or {}

def calculate_entropy(target_column):
    """
    Calculates the Shannon entropy of a target array vector to quantify subset impurity.

    Mathematical Formula:
        $$H(X) = -\\sum_{i=1}^{n} p(x_i) \\log_2 p(x_i)$$

    Args:
        target_column (pandas.Series): The column containing target categorical class labels.

    Returns:
        float: The calculated entropy scaler; 0.0 represents absolute classification homogeneity.
    """
    # Isolate unique target outcomes alongside their respective absolute distribution frequencies.
    elements, counts = numpy.unique(target_column, return_counts=True)
    total_samples = len(target_column)

    entropy = 0.0

    # Accumulate entropy parameters by evaluating localized probability density.
    for count in counts:
        probability = count / total_samples
        entropy -= probability * math.log2(probability)

    return entropy

def calculate_information_gain(data, split_feature_name, target_name):
    """
    Computes the net Information Gain achieved by partitioning a dataset across a specific feature.

    Optimization Objective:
        $$IG(T, a) = H(T) - H(T|a)$$

    Args:
        data (pandas.DataFrame): The matrix slice comprising the active data subset.
        split_feature_name (str): The specific column chosen as a candidate splitting attribute.
        target_name (str): The column label identifying the dependent target variable.

    Returns:
        float: The net information metric value; larger values represent superior variance reduction.
    """
    # 1. Calculate the total entropy before the split.
    total_entropy = calculate_entropy(data[target_name])

    # 2. Calculate the weighted entropy after splitting by the features.
    feature_values, feature_counts = numpy.unique(data[split_feature_name], return_counts=True)
    total_samples = len(data)
    remainder_entropy = 0.0

    # Aggregate the conditionally weighted sub-entropies across all child partition targets.
    for value, count in zip(feature_values, feature_counts):
        subset = data[data[split_feature_name] == value]
        weight = count / total_samples
        remainder_entropy += weight * calculate_entropy(subset[target_name])

    # 3. Information gain = Information Before - Information After.
    information_gain = total_entropy - remainder_entropy

    return information_gain

def build_tree(data, features, target_name, parent_value=None):
    """
    Recursively constructs a top-down decision topology utilizing the ID3 tree-induction algorithm.

    Args:
        data (pandas.DataFrame): The horizontal data matrix processing through the active path.
        features (list[str]): The remaining pool of unselected feature strings available for evaluation.
        target_name (str): The column label defining the terminal target variable.
        parent_value (str, optional): The antecedent specific attribute value route that generated this call.

    Returns:
        DecisionTree: The initialized root node anchoring the newly synthesized subtree.
    """
    target_values = data[target_name]

    # If all target labels are identical, return a leaf node with that label.
    if len(numpy.unique(target_values)) == 1:
        return DecisionTree(results=target_values.iloc[0], value=parent_value)

    # If no features are left to split on, return the majority class label.
    if len(features) == 0:
        majority_class = target_values.mode()[0]

        return DecisionTree(results=majority_class, value=parent_value)

    # Step 1: Map information gain across all remaining active dimensions within the feature space.
    gains = [
        calculate_information_gain(data, feature, target_name)
        for feature in features
    ]

    # Step 2: Identify the precise column coordinate that maximizes the information gain metric.
    best_feature_index = numpy.argmax(gains)
    best_feature = features[best_feature_index]

    # Step 3: Instantiate a new internal decision node bound to the winning feature criterion.
    root = DecisionTree(feature=best_feature, value=parent_value)

    # Step 4: Purge the selected features from the pipeline to enforce acyclic growth patterns.
    remaining_features = [feature for feature in features if feature != best_feature]

    # Step 5: Isolate the complete set of valid discrete categorical routes belonging to the chosen feature.
    unique_feature_values = numpy.unique(data[best_feature])

    # Step 6: Loop through each discrete branch variant to recursively construct nested sub-graphs.
    for value in unique_feature_values:
        subset_data = data[data[best_feature] == value]

        child_node = build_tree(subset_data, remaining_features, target_name, value)
        root.children[value] = child_node

    return root

def show(node, depth=0):
    """
    Recursively traverses the structured DecisionTree topology to render an ASCII visual layout.

    Args:
        node (DecisionTree): The current structural node processing through the display pipeline.
        depth (int): The current hierarchical depth tracker used to compute nested indents.
    """
    indent = " " * depth

    if node.results is not None:
        print(f"{indent}|-- Leaf: Predict -> '{node.results}'")
    else:
        print(f"{indent}|-- Split on [{node.feature}]")
        for value, child in node.children.items():
            print(f"{indent} Condition: if {node.feature} == '{value}':")
            show(child, depth + 4)
