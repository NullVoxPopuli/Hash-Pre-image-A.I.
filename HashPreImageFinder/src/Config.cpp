#include "Config.h"
#include "hashes.h"
#include "hashes/Murmur.h"

int Config::OUTPUT_TO_FILE = false;
int Config::NEED_TO_TRAIN = false;
int Config::NO_FILE_TRAIN = false;
int Config::NEED_TO_TEST = false;
int Config::BYPASS_EVERYTHING = false;
int Config::GENERATE_TRAIN_DATA = false;

int Config::MAX_NUMBER_OF_TRAINING_DATA = 10000;

int Config::HASH_WIDTH_IN_BITS = 16;
int Config::NUMBER_OF_INPUT_NEURONS = HASH_WIDTH_IN_BITS;
int Config::NUMBER_OF_OUTPUT_NEURONS = HASH_WIDTH_IN_BITS;
int Config::NUMBER_OF_BITS_FOR_INPUT = HASH_WIDTH_IN_BITS;
int Config::NUMBER_OF_LAYERS = 4;

int unsigned Config::LAYERS[] = {HASH_WIDTH_IN_BITS, HASH_WIDTH_IN_BITS};

int Config::MAX_EPOCHS = 10000;
int Config::REPORT_EVERY = 500;

double Config::LEARNING_RATE = 0.5;
double Config::DESIRED_ERROR = 0.00001;

const char* Config::NETWORK_SAVE_NAME = "network_state.fannnet";
const char* Config::DATA_FILE_NAME = "hash_training_data.fanndata";

unsigned int (*Config::current_hash_function)(unsigned int) = kennys_hash_16;
