#include "Config.h"

int OUTPUT_TO_FILE = false;
int NEED_TO_TRAIN = false;
int NO_FILE_TRAIN = false;
int NEED_TO_TEST = false;
int BYPASS_EVERYTHING = false;
int GENERATE_TRAIN_DATA = false;

int MAX_NUMBER_OF_TRAINING_DATA = 10000;

int HASH_WIDTH_IN_BITS = 32;
int NUMBER_OF_INPUT_NEURONS = HASH_WIDTH_IN_BITS;
int NUMBER_OF_OUTPUT_NEURONS = HASH_WIDTH_IN_BITS;
int NUMBER_OF_BITS_FOR_INPUT = HASH_WIDTH_IN_BITS;

int MAX_EPOCHS = 10000;
int REPORT_EVERY = 500;

double LEARNING_RATE = 0.5;
double DESIRED_ERROR = 0.00001;

const char* NETWORK_SAVE_NAME = "network_state.fannnet";
const char* DATA_FILE_NAME = "hash_training_data.fanndata";