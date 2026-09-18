/// File            :   Program.cs
/// Project         :   HypergraphNeuralNetwork
/// Programmer      :   Braiden Gole
/// First version   :   2026-08-07
/// Description     :   Entry point demonstrating hypergraph initialization, input feature matrix 
///                     formatting, incidence matrix definition, and forward pass execution.
/// Create, Build, and Compile:
/// dotnet new console --force -n hypergraph-neural-network
/// dotnet new nunit -o hypergraph-unit-tests -n HypergraphUnitTest
/// dotnet add hypergraph-unit-tests/HypergraphUnitTest.csproj reference hypergraph-neural-network.csproj
/// dotnet new sln --format slnx -n HypergraphNeuralNetwork
/// dotnet sln HypergraphNeuralNetwork.slnx add hypergraph-neural-network.csproj
/// dotnet sln HypergraphNeuralNetwork.slnx add hypergraph-unit-tests/HypergraphUnitTest.csproj --solution-folder hypergraph-unit-tests
/// find . -type d \( -name "obj" -o -name "bin" \) -exec rm -rf {} +
/// dotnet build HypergraphNeuralNetwork.slnx
/// dotnet run --project hypergraph-neural-network.csproj

namespace HypergraphNeuralNetwork
{
    internal class Program
    {
        public static void Main(string[] args)
        {
            // Define structural dimension for hypergraph nodes and hyperedges.
            int nodeCount = 4;
            int edgeCount = 3;

            // Define layer input and target output feature vector dimensions.
            int inputDimension = 3;
            int outputDimension = 2;

            if (nodeCount <= 0 || edgeCount <= 0 || inputDimension <= 0 || outputDimension <= 0)
            {
                throw new InvalidOperationException("Node count, edge count, and dimensinos must be positive.");
            }

            // Instantiate hypergraph neural network layer with weight initialization.
            Hypergraph layer = new Hypergraph(inputDimension, outputDimension);

            // Input node feature matrix flattened row-major (nodeCount * inputDimension).
            double[] nodeFeatures = new double[]
            {
                1.0, 0.5, -0.2,
                0.0, 1.2,  0.8,
                2.1, 0.0,  0.1,
                0.4, 0.9, -1.0
            };

            // Incidence matrix defining node-hyperedge connections flattened row-major (nodeCount * edgeCount).
            int[] incidence = new int[]
            {
                1, 0, 1,
                1, 1, 0,
                0, 1, 0,
                0, 1, 1
            };

            if (nodeFeatures.Length != nodeCount * inputDimension)
            {
                throw new InvalidOperationException("Node features array length does not match specified node count and input dimension.");
            }

            if (incidence.Length != nodeCount * edgeCount)
            {
                throw new InvalidOperationException("Incidence matrix array length does not match specified node count and edge count.");
            }

            double[] outputFeatures = layer.Forward(nodeFeatures, incidence, nodeCount, edgeCount);

            if (outputFeatures == null || outputFeatures.Length != nodeCount * outputDimension)
            {
                throw new InvalidOperationException("Forward pass returned invalid or mismatched output features array.");
            }

            Console.WriteLine("Forward pass output:");

            int count = 0;

            for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
            {
                Console.Write($"\tNode {nodeIndex}: [");

                for (int featureIndex = 0; featureIndex < outputDimension; featureIndex++)
                {
                    double value = outputFeatures[nodeIndex * outputDimension + featureIndex];

                    if (count < outputDimension - 1)
                    {
                        Console.Write($"{value:F4}, ");
                    }
                    else
                    {
                        Console.Write($"{value:F4}");
                    }
                    
                    ++count;
                }

                Console.WriteLine("]");

                count = 0;
            }
        }
    }
}