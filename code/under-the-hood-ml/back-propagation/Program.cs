/// File            :   Program.cs
/// Project         :   BackPropagation
/// Programmer      :   Braiden Gole
/// First version   :   2026-08-01
/// Description     :   Entry point for the Backpropagation Neural Network demonstration.
/// Compile:
/// 

namespace BackPropagation
{
    internal class Program
    {
        static void Main(string[] args)
        {
            // Define the set of training input samples for the network.
            double[][] data =
            {
                new double[] { 0.0, 0.0 },
                new double[] { 0.0, 1.0 },
                new double[] { 1.0, 0.0 },
                new double[] { 1.0, 1.0 }
            };
            
            // Define the target output values corresponding to each input sample.
            double[][] targets =
            {
                new double[] { 0.0, 0.0 },
                new double[] { 0.0, 1.0 },
                new double[] { 1.0, 0.0 },
                new double[] { 1.0, 1.0 }
            };

            // Instantiate the neural network with specified dimensions (2 inputs, 3 hidden, 1 output).
            NeuralNetwork network = new NeuralNetwork(entryDimensionCount: 2,
                intermediateDimensionCount: 3,
                exitDimensionCount: 2);

            // Display initial network predictions prior to training.
            EvaluateNetwork(network, data, targets);

            // Set operational training parameters.
            int epochs = 60000;
            double learningRate = 0.5;

            Console.WriteLine($"\nTraining network over {epochs:N0} iterations (Learning Rate = {learningRate})...");

            // Execute teh backpropagation training loop across all epoch and samples.
            for (int epoch = 0; epoch < epochs; epoch++)
            {
                for (int index = 0; index < data.Length; index++)
                {
                    network.Backpropagate(data[index], targets[index], learningRate);
                }
            }

            // Display network predictions after backpropagation training is complete.
            EvaluateNetwork(network, data, targets);
        }

        /// <summary>
        /// Evaluates the neural network by performing a forward pass on each input sample 
        /// and printing the inputs, expected target outputs, and actual model predictions to the console.
        /// </summary>
        /// <param name="network">The initialized <see cref="NeuralNetwork"/> instance to evaluate.</param>
        /// <param name="inputs">A jagged array containing input feature vectors.</param>
        /// <param name="targets">A jagged array containing expected target values for each input vector.</param>
        private static void EvaluateNetwork(NeuralNetwork network, double[][] inputs, double[][] targets)
        {
            for (int index = 0; index < inputs.Length; index++)
            {
                // Perform forwrward evaluation to retrieve current network prediction.
                double[] output = network.FeedForward(inputs[index]);

                // Output input feature values, target values, and model outputs.
                Console.WriteLine($"Input: [{inputs[index][0]}, {inputs[index][1]}] | " +
                    $"Expected: {targets[index][0]} | " +
                    $"Output: {output[0]:F4}");
            }
        }
    }
}