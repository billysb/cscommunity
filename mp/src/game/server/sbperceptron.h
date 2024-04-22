#ifdef SB_EXPERIMENTS
#ifndef SBPERCEPT_H
#define SBPERCEPT_H

#ifdef _WIN32
#pragma once
#endif

#include <vector>

/*
* A VERY SIMPLE MULTI-LAYER PERCEPTRON FOR SIMPLE CLASSIFICATION
*/

class Perceptron {
private:
	std::vector<std::vector<std::vector<float>>> weights; // Change double* to vector<vector<float>>
	std::vector<int> layerSizes;
	float learningRate; // Change double to float
	bool IsLinear;

public:
	Perceptron(const std::vector<int>& layerSizes, float lr, bool IsLinear) : layerSizes(layerSizes), learningRate(lr), IsLinear(IsLinear) {
		// Initialize weights to random values
		for (size_t i = 1; i < layerSizes.size(); ++i) {
			std::vector<std::vector<float>> layerWeights(layerSizes[i]); // Weight matrix for this layer
			for (int j = 0; j < layerSizes[i]; ++j) {
				std::vector<float> neuronWeights(layerSizes[i - 1] + 1); // +1 for bias
				if (!IsLinear)
				{
					for (float k = 0; k < neuronWeights.size(); ++k) {
						// Initialize weights to small random values
						neuronWeights[k] = static_cast<float>((rand() % 100) / 100.0); // Random values between 0 and 1
					}
				}
				else
				{
					for (float k = 0; k < neuronWeights.size(); ++k) {
						// Initialize weights to small random values
						neuronWeights[k] = float(0); // Linear models should be 0.
					}
				}
				layerWeights[j] = neuronWeights;
			}
			weights.push_back(layerWeights);
		}
	}

	// Activation function (sigmoid function)
	float activate(float sum) {
		return 1 / (1 + exp(-sum));
	}

	// linear activation. Good for prediction
	float linear(float x) {
		return x;
	}

	// Feedforward function
	std::vector<float> feedforward(const std::vector<float>& inputs) {
		std::vector<float> outputs;
		outputs = inputs; // Initialize input layer

		for (size_t i = 0; i < weights.size(); ++i) {
			std::vector<float> newOutputs(layerSizes[i + 1], 0); // Initialize new output layer

			// Compute sum for each neuron in the current layer
			for (int j = 0; j < layerSizes[i + 1]; ++j) {
				float sum = 0;
				for (float k = 0; k < outputs.size(); ++k) {
					sum += outputs[k] * weights[i][j][k];
				}
				// Add bias
				sum += weights[i][j][outputs.size()];
				// Apply activation function
				if (!IsLinear)
					newOutputs[j] = activate(sum);
				else
					newOutputs[j] = linear(sum);
			}

			outputs = newOutputs; // Update outputs for next layer
		}

		return outputs;
	}

	// Train the perceptron
	void train(const std::vector<float>& inputs, const std::vector<float>& targets) {
		std::vector<std::vector<float>> layerOutputs;
		layerOutputs.push_back(inputs); // Initialize input layer

		// Forward pass
		for (size_t i = 0; i < weights.size(); ++i) {
			std::vector<float> newOutputs(layerSizes[i + 1], 0); // Initialize new output layer

			// Compute sum for each neuron in the current layer
			for (int j = 0; j < layerSizes[i + 1]; ++j) {
				float sum = 0;
				for (float k = 0; k < layerOutputs[i].size(); ++k) {
					sum += layerOutputs[i][k] * weights[i][j][k];
				}
				// Add bias
				sum += weights[i][j][layerOutputs[i].size()];
				// Apply activation function
				if (!IsLinear)
					newOutputs[j] = activate(sum);
				else
					newOutputs[j] = linear(sum);
			}

			layerOutputs.push_back(newOutputs); // Update outputs for next layer
		}

		// Backpropagation
		std::vector<std::vector<float>> deltas(layerOutputs.size() - 1);
		for (size_t i = 0; i < targets.size(); ++i) {
			// Calculate output layer delta
			deltas.back().push_back(layerOutputs.back()[i] * (1 - layerOutputs.back()[i]) * (targets[i] - layerOutputs.back()[i]));
		}

		// Calculate deltas for hidden layers
		for (int i = layerOutputs.size() - 2; i > 0; --i) {
			for (int j = 0; j < layerSizes[i]; ++j) {
				float error = 0;
				for (float k = 0; k < layerSizes[i + 1]; ++k) {
					error += deltas[i][k] * weights[i][k][j];
				}
				deltas[i - 1].push_back(layerOutputs[i][j] * (1 - layerOutputs[i][j]) * error);
			}
		}

		// Update weights
		for (size_t i = 0; i < weights.size(); ++i) {
			for (size_t j = 0; j < weights[i].size(); ++j) {
				for (size_t k = 0; k < weights[i][j].size(); ++k) {
					if (k < layerOutputs[i].size()) {
						weights[i][j][k] += learningRate * deltas[i][j] * layerOutputs[i][k];
					}
					else {
						weights[i][j][k] += learningRate * deltas[i][j]; // Update bias weight
					}
				}
			}
		}
	}
};

#endif

#endif