#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <bitset>
#include <sstream>
#include <fstream>
#include <time.h>
#include <string.h>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/lexical_cast.hpp>


#include "fann.h"
#include "floatfann.h"

#include "Config.h"

#include "hashes.h"
#include "main.h"
#include "fann_utils.h"


using namespace std;


struct fann *trained_network;

struct fann *fann_create_network(int num, unsigned int args[])
{
	return fann_create_standard_array(num, &args[0]);
}

void print_config()
{
	cout << "Configuration: \n";
	cout << "     Network: \n";
	cout << "\t Network save name: " << Config::NETWORK_SAVE_NAME << "\n";
	cout << "\t #input: " << Config::NUMBER_OF_INPUT_NEURONS << "\n";
	for(int i = 1; i < Config::NUMBER_OF_LAYERS - 1; i++)
	{
		cout << "\t #hidden: " << Config::LAYERS[i] << "\n";
	}
	cout << "\t #output: " << Config::NUMBER_OF_OUTPUT_NEURONS << "\n";

	
	cout << "     Training: \n";
	cout << "\t Max Epochs: " << Config::MAX_EPOCHS << "\n";
	cout << "\t Reports every: " << Config::REPORT_EVERY << "\n";
	cout << "\t Desired error: " << Config::DESIRED_ERROR << "\n";
	cout << "\t Learning Rate: " << Config::LEARNING_RATE << "\n";
	cout << "\n";
}

void train_network()
{
    printf("Training ... \n");
	print_config();
    // struct fann *ann = fann_create_standard(num_layers, 
    // 											Config::NUMBER_OF_INPUT_NEURONS,
    //                                            Config::HASH_WIDTH_IN_BITS, 
    // 											Config::HASH_WIDTH_IN_BITS, 
    // 											Config::NUMBER_OF_OUTPUT_NEURONS);

	struct fann *ann = fann_create_network(Config::NUMBER_OF_LAYERS, Config::LAYERS);

	// struct fann *ann = fann_create_shortcut(3, 8, 16, 8);

	fann_set_learning_rate(ann, Config::LEARNING_RATE);
    fann_set_activation_function_hidden(ann, FANN_LINEAR);
    fann_set_activation_function_output(ann, FANN_GAUSSIAN_SYMMETRIC);
    fann_train_on_file(ann, 
						Config::DATA_FILE_NAME, 
						Config::MAX_EPOCHS, 
						Config::REPORT_EVERY, 
						Config::DESIRED_ERROR);
	// fann_cascadetrain_on_file(ann, data_file_name, 200, 1, DESIRED_ERROR);
    fann_save(ann, Config::NETWORK_SAVE_NAME);
    fann_destroy(ann);
}

void train_network_no_file()
{
	printf("Training without file ... \n");
    const unsigned int num_layers = 4;

    const unsigned int num_neurons_hidden = Config::HASH_WIDTH_IN_BITS;

    float error = Config::DESIRED_ERROR + 1;
    unsigned int epoch = 0;
    int desired_error_reached;
    int acceptable = false;

	struct fann *ann = fann_create_network(Config::NUMBER_OF_LAYERS, Config::LAYERS);
	

	fann_set_learning_rate(ann, Config::LEARNING_RATE);
    fann_set_activation_function_hidden(ann, FANN_LINEAR);
    fann_set_activation_function_output(ann, FANN_GAUSSIAN_SYMMETRIC);

    while (!acceptable && epoch < Config::MAX_EPOCHS)
    {
		for(epoch = 0; epoch <= Config::MAX_EPOCHS; epoch++)
		{
			struct fann_train_data *data = generate_data( Config::NUMBER_OF_INPUT_NEURONS, 
														 Config::NUMBER_OF_OUTPUT_NEURONS, 30000);
			error = fann_train_epoch(ann, data);
			fann_destroy_train(data);
			desired_error_reached = fann_desired_error_reached(ann, Config::DESIRED_ERROR);

			if(epoch % Config::REPORT_EVERY == 0 || epoch == Config::MAX_EPOCHS || epoch == 1 ||
						desired_error_reached == 0)
					{
						cout << "\nEpochs     " << epoch << ". Current error: " << error << ". Bit fail " << ann->num_bit_fail << "\n";
					}
			else
			{
				cout << "\r" << epoch;
			}

			if(desired_error_reached == 0)
				break;
		}

		struct fann_train_data *data = generate_data( Config::NUMBER_OF_INPUT_NEURONS,  Config::NUMBER_OF_OUTPUT_NEURONS, 600);
		error = fann_train_epoch(ann, data);
		fann_destroy_train(data);
		if (fann_desired_error_reached(ann,  Config::DESIRED_ERROR))
		{
			acceptable = true;
		}
    }

    fann_save(ann, Config::NETWORK_SAVE_NAME);
    fann_destroy(ann);
}

struct fann_train_data *generate_data(unsigned int num_input, unsigned int num_output, unsigned int num_pairs)
{
	struct fann_train_data *data = (struct fann_train_data *) malloc(sizeof(struct fann_train_data));
	fann_init_error_data((struct fann_error *) data);
	unsigned int i, j;
	fann_type *data_input, *data_output;

	data->num_data = num_pairs;
	data->num_input = num_input;
	data->num_output = num_output;
	data->input = (fann_type **) calloc(num_pairs, sizeof(fann_type *));
	data->output = (fann_type **) calloc(num_pairs, sizeof(fann_type *));

	time_t T = time(0);
	boost::mt19937 gen(T);

	data_input = (fann_type *) calloc(num_input * num_pairs, sizeof(fann_type));
	data_output = (fann_type *) calloc(num_output * num_pairs, sizeof(fann_type));

	for(i = 0; i != num_pairs; i++)
	{
		boost::uniform_real<> dist(0, pow(2,  Config::HASH_WIDTH_IN_BITS));
		boost::variate_generator<boost::mt19937&, boost::uniform_real<> > random(gen, dist);

		boost::dynamic_bitset<> value( Config::HASH_WIDTH_IN_BITS, random());
		boost::dynamic_bitset<> hash( Config::HASH_WIDTH_IN_BITS, kennys_hash_16(value.to_ulong()));

		data->input[i] = data_input;
		data_input += num_input;

		for(j = 0; j != num_input; j++)
		{
			data->input[i][j] = hash[j];
		}

		data->output[i] = data_output;
		data_output += num_output;

		for(j = 0; j != num_output; j++)
		{
			data->output[i][j] = value[j];
		}
	}
	return data;
}

struct fann_train_data *generate_swarm_data(unsigned int num_input, unsigned int num_pairs, unsigned int output_position)
{
	struct fann_train_data *data = (struct fann_train_data *) malloc(sizeof(struct fann_train_data));
	fann_init_error_data((struct fann_error *) data);
	unsigned int i, j;
	fann_type *data_input, *data_output;

	data->num_data = num_pairs;
	data->num_input = num_input;
	data->num_output = 1;
	data->input = (fann_type **) calloc(num_pairs, sizeof(fann_type *));
	data->output = (fann_type **) calloc(num_pairs, sizeof(fann_type *));

	time_t T = time(0);
	boost::mt19937 gen(T);

	data_input = (fann_type *) calloc(num_input * num_pairs, sizeof(fann_type));
	data_output = (fann_type *) calloc(num_pairs, sizeof(fann_type));

	for(i = 0; i != num_pairs; i++)
	{
		boost::uniform_real<> dist(0, pow(2,  Config::HASH_WIDTH_IN_BITS));
		boost::variate_generator<boost::mt19937&, boost::uniform_real<> > random(gen, dist);

		boost::dynamic_bitset<> value( Config::HASH_WIDTH_IN_BITS, random());
		boost::dynamic_bitset<> hash( Config::HASH_WIDTH_IN_BITS, kennys_hash_16(value.to_ulong()));
//		cout << value << " hashes to " << hash << "\n";

		data->input[i] = data_input;
		data_input += num_input;

		for(j = 0; j != num_input; j++)
		{
			data->input[i][j] = hash[j];
		}

		data->output[i] = data_output;
		data_output += 1;

		data->output[i][0] = value[output_position];
//		cout << "this network should map to " << value[output_position] << "\n";
	}
	return data;
}

struct fann **allocate_swarm()
{
	struct fann **for_the_swarm;
	for_the_swarm = (fann**) malloc(sizeof(fann*) * Config::NUMBER_OF_OUTPUT_NEURONS);

	int unsigned lastlayer = Config::LAYERS[Config::NUMBER_OF_LAYERS-1];
	for(int i=0; i<Config::NUMBER_OF_OUTPUT_NEURONS; i++)
	{
		for_the_swarm[i] = fann_create_network(Config::NUMBER_OF_LAYERS, Config::LAYERS);
		fann_set_learning_rate(for_the_swarm[i], Config::LEARNING_RATE);
	    fann_set_activation_function_hidden(for_the_swarm[i], FANN_SIN_SYMMETRIC);
	    fann_set_activation_function_output(for_the_swarm[i], FANN_COS_SYMMETRIC);
	    fann_set_training_algorithm(for_the_swarm[i], FANN_TRAIN_BATCH);
	}

	return for_the_swarm;
}

void train_the_swarm(struct fann **swarm)
{
	for(int i=0; i<Config::NUMBER_OF_OUTPUT_NEURONS; i++)
	{
		cout << "Training ANN " << i+1 << " of " << Config::NUMBER_OF_OUTPUT_NEURONS << "\n";
		struct fann_train_data *data = generate_swarm_data(Config::NUMBER_OF_INPUT_NEURONS, Config::MAX_NUMBER_OF_TRAINING_DATA, i);
		fann_train_on_data(swarm[i], data, Config::MAX_EPOCHS, Config::REPORT_EVERY, Config::DESIRED_ERROR);
		fann_destroy_train(data);
		cout << "\n";
	}
}

void free_the_swarm(struct fann **swarm)
{
	for(int i=0; i<Config::NUMBER_OF_OUTPUT_NEURONS; i++)
	{
		//First, forever freeze the swarm
		char *save_name = new char[100];
		strcpy(save_name, Config::NETWORK_SAVE_NAME);
		strcat(save_name, boost::lexical_cast<std::string>(i).c_str());
	    fann_save(swarm[i], save_name);
		fann_destroy(swarm[i]);
		free(save_name);
	}
	free(swarm);
}

void load_trained_network()
{
    trained_network = fann_create_from_file(Config::NETWORK_SAVE_NAME);
}

struct fann **load_trained_swarm()
{
    struct fann **for_the_swarm;
	for_the_swarm = (fann**) malloc(sizeof(fann*) * Config::NUMBER_OF_OUTPUT_NEURONS);
	
	for(int i=0; i<Config::NUMBER_OF_OUTPUT_NEURONS; i++)
	{
		char *load_name = new char[100];
		strcpy(load_name, Config::NETWORK_SAVE_NAME);
		strcat(load_name, boost::lexical_cast<std::string>(i).c_str());
	    for_the_swarm[i] = fann_create_from_file(load_name);
		free(load_name);
	}
	
	return for_the_swarm;
}

string convert_array_to_string(fann_type *arr, int width)
{
	ostringstream result;
	for (int i = 0; i < width; i++)
	{
		result << arr[i];
	}
	return result.str();
}

void test_network()
{
    printf("Running trained network ... \n ");
    
    fann_type *calc_out;

    calc_out = fann_run(trained_network, fann_input);

    // printf("xor test (%f,%f) -> %f\n", input[0], input[1], calc_out[0]);
	cout << "Input: " << convert_array_to_string(fann_input, Config::HASH_WIDTH_IN_BITS) << "\n";
	cout << "Output: " << convert_array_to_string(calc_out, Config::HASH_WIDTH_IN_BITS) << "\n";
	
	unsigned int output_binary = convert_fann_out_to_binary(calc_out, Config::HASH_WIDTH_IN_BITS);

	unsigned int hashed = kennys_hash_16(output_binary);

	printf("Output: ... meh, Which hashes back to: %x\n\n", hashed);
	
}

unsigned int test_network_with_value(int hash_value)
{
	fann_type auto_fann_input[Config::HASH_WIDTH_IN_BITS];
	string buffer = bitset<16>(hash_value).to_string();// convert to binary, store to string
	
	// convert to fann format
	for(int j = 0; j < Config::HASH_WIDTH_IN_BITS; j++)
	{
		auto_fann_input[j] = (float)(buffer[j]);
	}
	
	fann_type *calc_out;
	calc_out = fann_run(trained_network, auto_fann_input);
	
	unsigned int output_binary = convert_fann_out_to_binary(calc_out, Config::HASH_WIDTH_IN_BITS);
	
	return output_binary;
}

unsigned int test_swarm_with_value(struct fann **swarm, int hash_value)
{
	boost::dynamic_bitset<> output (Config::NUMBER_OF_OUTPUT_NEURONS);
	boost::dynamic_bitset<> buffer( Config::HASH_WIDTH_IN_BITS, hash_value);
	fann_type auto_fann_input[Config::HASH_WIDTH_IN_BITS];

	for(int j=0; j < Config::HASH_WIDTH_IN_BITS; j++)
	{
		auto_fann_input[j] = (float)(buffer[j]);
	}
	
	for(int i=0; i < Config::HASH_WIDTH_IN_BITS; i++)
	{
		fann_type *calc_out;
		calc_out = fann_run(swarm[i], auto_fann_input);
//		unsigned int output_binary = convert_fann_out_to_binary(calc_out, 1);
//		cout << output_binary << "\n";
		if (calc_out[0] > 0.5)
			output[i] = 1;
		else
			output[i] = 0;
	}
//	cout << output << "\n";
	return output.to_ulong();
}

void auto_test_network_with_random_data(unsigned int start, unsigned int end, unsigned int num_of_data_sets_to_test)
{
	// init randomness 
	srand((unsigned)time(0));
	unsigned int random_pre_image_value;
	unsigned int hashed_value;
	unsigned int result;
	int failed = false;
	
	load_trained_network();
	
	for (unsigned int i = 0; i < num_of_data_sets_to_test; i ++)
	{
		random_pre_image_value = start + (unsigned int)(((end - start) * rand()) / (RAND_MAX + 1.0));
		hashed_value = kennys_hash_16(random_pre_image_value);
		result = test_network_with_value(hashed_value); 
		
		if (result != random_pre_image_value)
		{
			failed = true;
			cout << "Error:\n";
			cout << "   Hash:             " << hashed_value << "\n";
			cout << "   Result:           " << result << "\n";
			cout << "   Should Have been: " << random_pre_image_value << "\n\n";
		}
	}
	
	if (!failed)
	{
		cout << "All tested hashes were reversed successfully... \n";
	}
	
	fann_destroy(trained_network);
}

void auto_test_swarm(struct fann **swarm, unsigned int num_of_data_sets_to_test)
{
	srand((unsigned)time(0));
	unsigned int random_pre_image_value;
	unsigned int hashed_value;
	unsigned int result;
	int failed = false;
	int num_fail = 0;
	time_t T = time(0);
	boost::mt19937 gen(T);
	
	for (unsigned int i=0; i<num_of_data_sets_to_test; i++)
	{
		boost::uniform_real<> dist(0, pow(2,  Config::HASH_WIDTH_IN_BITS));
		boost::variate_generator<boost::mt19937&, boost::uniform_real<> > random(gen, dist);

		random_pre_image_value = random();
		hashed_value = kennys_hash_16(random_pre_image_value);
		result = test_swarm_with_value(swarm, hashed_value);

		unsigned int result_hash = (unsigned int)kennys_hash_16(result);
		if (result_hash != hashed_value)
		{
			failed = true;
			cout << "Error:\n";
			cout << "   Hash:             " << hashed_value << "\n";
			cout << "   Result:           " << result << " (which hashes to " << (int)kennys_hash_16(result) << ")\n";
			cout << "   Should Have been: " << random_pre_image_value << "\n\n";
			num_fail += 1;
		}
	}
	
	if (!failed)
	{
		cout << "All tested hashes were reversed successfully... \n";
	}
	else
	{
		cout << num_fail << " of " << num_of_data_sets_to_test << " failed\n";
	}
}

//http://stackoverflow.com/questions/5354194/how-do-i-change-the-default-local-time-format-in-c
string get_current_time()
{
    time_t rawtime;
    struct tm * timeinfo;
    
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    
    char output[30];
    strftime(output, 30, "%Y %m-%d %H-%M-%S %a", timeinfo);
    
    return string(output);
}
#include <vector>
#include <functional>
//http://www.blog.highub.com/c-plus-plus/c-parse-split-delimited-string/
void split(const string& s, char c,
           vector<string>& v) {
   string::size_type i = 0;
   string::size_type j = s.find(c);
   while (j != string::npos) {
      v.push_back(s.substr(i, j-i));
      i = ++j;
      j = s.find(c, j);
      if (j == string::npos)
         v.push_back(s.substr(i, s.length( )));
   }
}


int main (int argc, const char * argv[])
{
    // handle arguments
    if (argc < 2)
	{
		display_help();
	}
	else
	{
		for (int i = 1; i < argc; i++)
		{
			if (argv[i][0] != '-') continue;
			
			if (strcmp(argv[i], "-test") == 0)
			{
				Config::NEED_TO_TEST = true;
				// next var is going to be the input

//				char temp_array[Config::HASH_WIDTH_IN_BITS];
//				strcpy(temp_array, argv[i + 1]);
//				
//				for(int j = 0; j < Config::HASH_WIDTH_IN_BITS; j++)
//				{
//					fann_input[j] = (float)(temp_array[j] - '0');
//				}
			}
			else if (strcmp(argv[i], "-autoTest") == 0)
			{
				auto_test_network_with_random_data(atoi(argv[i + 1]), atoi(argv[i + 2]), atoi(argv[i + 3]));
			}
			else if (strcmp(argv[i], "-genTrain") == 0)
			{
				Config::GENERATE_TRAIN_DATA = true;
			}
			else if (strcmp(argv[i], "-train") == 0)
			{
				Config::NEED_TO_TRAIN = true;
				bool printHelpAndExit = false;
				if (i+1 >= argc)
				{
					printHelpAndExit = true;
				}
				else if (strcmp(argv[i+1], "file") == 0)
				{
					Config::NO_FILE_TRAIN = false;
				}
				else if (strcmp(argv[i+1], "block") == 0)
				{
					Config::NO_FILE_TRAIN = true;
				}
				else if (strcmp(argv[i+1], "swarm") == 0)
				{
					printHelpAndExit = false;
				}
				else
				{
					printHelpAndExit = true;
				}

				if (printHelpAndExit)
				{
					cout << "need to specify train mode ('file' or 'block')\n";
					exit(1);
				}
			}
			else if (strcmp(argv[i], "-file") == 0)
			{
				Config::OUTPUT_TO_FILE = true;
			}
			else if (strcmp(argv[i], "-bypass") == 0)
			{
				// bypasses all functions, used for testing single functions
				Config::BYPASS_EVERYTHING = true;
			}
			else if (strcmp(argv[i], "-i") == 0) // input
			{
				
			}
			else if (strcmp(argv[i], "-nnil") == 0) // number of neurons in each layer
			{
				// should be formatted 8,16,6,8
				// as in in,middle,middle,out
				vector<string> v;
				split(argv[i + 1], ',', v);
				for (int i = 0; i < v.size( ); ++i) {
					Config::LAYERS[i] = atoi(v[i].c_str());
			   	}
				Config::NUMBER_OF_INPUT_NEURONS = Config::LAYERS[0];
				Config::NUMBER_OF_OUTPUT_NEURONS = Config::LAYERS[v.size() - 1];
				Config::NUMBER_OF_LAYERS = v.size();					
			}
			else if (strcmp(argv[i], "-nb") == 0)
			{
				Config::NUMBER_OF_BITS_FOR_INPUT = atoi(argv[i + 1]);
			}
			else if (strcmp(argv[i], "-de") == 0) // desired error
			{
				Config::DESIRED_ERROR = atoi(argv[i + 1]);
			}
			else if (strcmp(argv[i], "-lrate") == 0) // learning rate
			{
				Config::LEARNING_RATE = atoi(argv[i + 1]);
			}
			else if (strcmp(argv[i], "-epochs") == 0) // max epochs
			{
				Config::MAX_EPOCHS = atoi(argv[i + 1]);
			}
			else if (strcmp(argv[i], "-reports") == 0) // reports every number of epochs
			{
				Config::REPORT_EVERY = atoi(argv[i + 1]);
			}
			else if (strcmp(argv[i], "-help") == 0)
			{
				display_help();
			}
			else if (strcmp(argv[i], "-mTrnData") == 0)
			{
				Config::MAX_NUMBER_OF_TRAINING_DATA = atoi(argv[i + 1]);
			}
			else if (strcmp(argv[i], "-useSwarm") == 0)
			{
				Config::USE_SWARM = true;
			}
			else
			{
				display_help();
			}
			
		}
		
	}
	
	if (!Config::BYPASS_EVERYTHING)
	{
		// actually begin executing
	    string fname = get_current_time();
	    fname.append(".txt");


	    char *filename = (char*)fname.c_str();
		struct fann **swarm;

	    FILE  *fs;
	    if (Config::OUTPUT_TO_FILE)
		{
			cout << "File name for this run is going to be: " << filename << "\n";

		    if((fs=freopen(filename, "w" ,stdout)) == NULL) {
		        printf("Cannot open file.\n");
		        exit(1);
		    }

		}
		
		if (Config::GENERATE_TRAIN_DATA) generate_train_file();
	    if (Config::NEED_TO_TRAIN)
	    {
	    	if (Config::NO_FILE_TRAIN) {
	    		train_network_no_file();
			} else if (Config::USE_SWARM) {
				swarm = allocate_swarm();
				train_the_swarm(swarm);
	    	} else {
	    		train_network();
	    	}
	    }
	    if (Config::NEED_TO_TEST)
		{
			if (Config::USE_SWARM)
			{
				swarm = load_trained_swarm();
				auto_test_swarm(swarm, 1000);
			}
			else
			{
				load_trained_network();
				test_network();     
				fann_destroy(trained_network);
			}
		}
		
	    if (Config::USE_SWARM)
	    {
	    	free_the_swarm(swarm);
	    }
	}
	else
	{
		cout << "Nothing here at the moment...\n\n";
	}
	

}

void display_help()
{
	cout << "\n\n";
	cout << "\t :::::::::::::::::::::::::::::\n";
	cout << "\t ::: Hash Pre-Image Finder ::: \n";
	cout << "\t :::::::::::::::::::::::::::::\n";
	
	cout << "\t\t by:\n";
	cout << "\t\t\t L. Preston Sego III and Kenny Skaggs\n\n";
	
	cout << "Available Options: \n";
	cout << "\t -mTrnData 123\t\t[" << Config::MAX_NUMBER_OF_TRAINING_DATA << "] Max Number of Training Data Sets\n";
	cout << "\t -genTrain \t\tGenerates the training file to train the neural network with\n";
	cout << "\t -train <mode> \t\tTrains the network using the specified training mode\n";
	cout << "\t -test 1010101001 \tRuns a trained network with the given input (should be the hash you\n";
						cout << "\t\t\t\t are trying to find the pre-image for)\n";
	cout << "\t -autoTest strt end n \t Auto test a trained newwork between two values for n number of values.\n";
	cout << "\t -file \t\t\tInstead of standard out, outputting to the console, including this will have\n";
						cout << "\t\t\t\t all output go to a file with the date and time\n";
	
	cout << "\n\nTraining Modes: \n";
	cout << "\t block\t\tTrains the network using dynamically generated, random data\n";
	cout << "\t file\t\tTrains the network using a pre-generated data file\n";

	cout << "\n\nNetwork Configuration: \n";
	cout << "\t -i \t\tInput? [Not Yet Implemented]\n";
	cout << "\t -nnil in,...,out \tNumber of neurons in each layer\n";
	cout << "\t -nb \t\t[" << Config::NUMBER_OF_BITS_FOR_INPUT << "] Number of bits the network should expect for the input\n";
	
	cout << "\n\nNetwork Training Constants:\n";
	cout << "\t -de 0.00001 \t\t[" << Config::DESIRED_ERROR << "] Desired Error\n";
	cout << "\t -lrate 0.73 \t\t[" << Config::LEARNING_RATE << "] Learning Rate\n";
	cout << "\t -epochs 10000 \t\t[" << Config::MAX_EPOCHS << "] Number of training iterations (epochs)\n";
	cout << "\t -reports 500 \t\t[" << Config::REPORT_EVERY << "]After how many epochs should should the training report each time?\n";
	
	cout << "\n\nDeveloper Options: \n";
	cout << "\t -bypass \tUsed for skipping all normal functionality and for testing specific functions\n";
	
	cout << "\n\n Example: ./hPif -genTrain -train -test 1111000011110000\n";
	cout << "\t\t Generates the training file, trains the network, and tests the network (assuming the \n";
	cout << "\t\t\tnetwork is set up for a 16 bit hash) with the value F0F0\n\n";
	
	cout << "* Numbers contained in brackets ( [number] ) are the default value...\n\n";
	
}
