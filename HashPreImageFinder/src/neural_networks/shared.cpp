#include "shared.h"

void print_config()
{
	std::cout << "Configuration: \n";
	std::cout << "     Network: \n";
	std::cout << "\t # layers: " << Config::NUMBER_OF_LAYERS << "\n";
	std::cout << "\t Network save name: " << Config::NETWORK_SAVE_NAME << "\n";
	std::cout << "\t #input: " << Config::NUMBER_OF_INPUT_NEURONS << "\n";
	for(int i = 1; i < Config::NUMBER_OF_LAYERS - 1; i++)
	{
		std::cout << "\t #hidden: " << Config::LAYERS[i] << "\n";
	}
	std::cout << "\t #output: " << Config::NUMBER_OF_OUTPUT_NEURONS << "\n";

	
	std::cout << "     Training: \n";
	std::cout << "\t Max Epochs:        " << Config::MAX_EPOCHS << "\n";
	std::cout << "\t Reports every:     " << Config::REPORT_EVERY << "\n";
	std::cout << "\t Desired error:     " << Config::DESIRED_ERROR << "\n";
	std::cout << "\t Learning Rate:     " << Config::LEARNING_RATE << "\n";
	std::cout << "\t Learning Momentum: " << Config::LEARNING_MOMENTUM << "\n";
	std::cout << "\n";
}