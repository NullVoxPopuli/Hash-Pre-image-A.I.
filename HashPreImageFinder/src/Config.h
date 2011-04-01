/*
 Contains global constants
 
 */

#pragma once

class Config
{
public:

	static int OUTPUT_TO_FILE;
	static int NEED_TO_TRAIN;
	static int NO_FILE_TRAIN;
	static int NEED_TO_TEST;
	static int BYPASS_EVERYTHING;
	static int GENERATE_TRAIN_DATA;

	static int MAX_NUMBER_OF_TRAINING_DATA;

	static int HASH_WIDTH_IN_BITS;
	static int NUMBER_OF_INPUT_NEURONS;
	static int NUMBER_OF_OUTPUT_NEURONS;
	static int NUMBER_OF_BITS_FOR_INPUT;

	static int MAX_EPOCHS;
	static int REPORT_EVERY;

	static double LEARNING_RATE;
	static double DESIRED_ERROR;

	static const char* NETWORK_SAVE_NAME;
	static const char* DATA_FILE_NAME;
};
