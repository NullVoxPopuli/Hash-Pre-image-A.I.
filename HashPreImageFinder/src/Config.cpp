#include "Config.h"
#include "hashes.h"
#include "hashes/Murmur.h"
#include <limits.h>

#include <boost/preprocessor/stringize.hpp>
#include <map>
#include <iostream>
#include <string>
#include "hashes.h"

int Config::OUTPUT_TO_FILE = false;
int Config::NEED_TO_TRAIN = false;
int Config::NO_FILE_TRAIN = false;
int Config::NEED_TO_TEST = false;
int Config::NEED_TO_AUTOTEST = false;
int Config::BYPASS_EVERYTHING = false;
int Config::GENERATE_TRAIN_DATA = false;
int Config::USE_SWARM = false;
int Config::USE_SEQUENTIAL_TESTING = false;

int Config::NUM_TEST_POINTS = 1000;
int Config::TEST_MIN = 0;
int Config::TEST_MAX = UINT_MAX;
int Config::MAX_NUMBER_OF_TRAINING_DATA = 50000;

int Config::HASH_WIDTH_IN_BITS = 16;
int Config::NUMBER_OF_INPUT_NEURONS = HASH_WIDTH_IN_BITS;
int Config::NUMBER_OF_OUTPUT_NEURONS = HASH_WIDTH_IN_BITS;
int Config::NUMBER_OF_BITS_FOR_INPUT = HASH_WIDTH_IN_BITS;
int Config::NUMBER_OF_LAYERS = 5;

//int unsigned Config::LAYERS[] = {16,16,16,1}; 1/4 fail
//int unsigned Config::LAYERS[] = {16,16,1}; 1/3 fail
int unsigned Config::LAYERS[] ={16,16,16,16,1};

int Config::MAX_EPOCHS = 5000;
int Config::REPORT_EVERY = 1000;

double Config::LEARNING_RATE = 0.7;
double Config::DESIRED_ERROR = 0.00001;
double Config::LEARNING_MOMENTUM = 1.5;

const char* Config::NETWORK_SAVE_NAME = "network_state.fannnet";
const char* Config::CONFIG_FOLDER_NAME = "NetworkConfig";
const char* Config::DATA_FILE_NAME = "hash_training_data.fanndata";

unsigned int (*Config::current_hash_function)(unsigned int) = kennys_hash_16;

function_name_map_type Config::fmap;


void register_hash_names()
{
	Config::fmap[kennys_hash_16] = BOOST_PP_STRINGIZE(kennys_hash_16);
	Config::fmap[kennys_hash] = BOOST_PP_STRINGIZE(kennys_hash);
	Config::fmap[prestons_hash_8] = BOOST_PP_STRINGIZE(prestons_hash_8);
	Config::fmap[Murmur::hash] = BOOST_PP_STRINGIZE(Murmur::hash);
	Config::fmap[add_one_hash] = BOOST_PP_STRINGIZE(add_one_hash);
	Config::fmap[mult_hash] = BOOST_PP_STRINGIZE(mult_hash);
	Config::fmap[testing_ground_hash] = BOOST_PP_STRINGIZE(testing_ground_hash);
	Config::fmap[x_squared] = BOOST_PP_STRINGIZE(x_squared);
	Config::fmap[lookup_table] = BOOST_PP_STRINGIZE(lookup_table);
	Config::fmap[mod_constant] = BOOST_PP_STRINGIZE(mod_constant);
}
