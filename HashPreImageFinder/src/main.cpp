
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


#include "fann.h"
#include "floatfann.h"

#include "config.h"

#include "fann_utils.h"
#include "hashes.h"
#include "main.h"

using namespace std;



struct fann *trained_network;

void train_network()
{
    printf("Training ... \n");
    const unsigned int num_input = NUMBER_OF_INPUT_NEURONS;
    const unsigned int num_output = NUMBER_OF_OUTPUT_NEURONS;
    const unsigned int num_layers = 4;
    const unsigned int num_neurons_hidden = 16;

    struct fann *ann = fann_create_standard(num_layers, num_input,
                                                 num_neurons_hidden, num_neurons_hidden, num_output);

	// struct fann *ann = fann_create_shortcut(3, 8, 16, 8);

	fann_set_learning_rate(ann, LEARNING_RATE);
    fann_set_activation_function_hidden(ann, FANN_LINEAR);
    fann_set_activation_function_output(ann, FANN_GAUSSIAN_SYMMETRIC);
    fann_train_on_file(ann, DATA_FILE_NAME, MAX_EPOCHS, REPORT_EVERY, DESIRED_ERROR);
	// fann_cascadetrain_on_file(ann, data_file_name, 200, 1, DESIRED_ERROR);
    fann_save(ann, NETWORK_SAVE_NAME);
    fann_destroy(ann);
}

void train_network_no_file()
{
	printf("Training without file ... \n");
    const unsigned int num_layers = 4;
    const unsigned int num_neurons_hidden = 16;

    float error = DESIRED_ERROR + 1;
    unsigned int epoch = 0;
    int desired_error_reached;
    int acceptable = false;

    struct fann *ann = fann_create_standard(num_layers, NUMBER_OF_INPUT_NEURONS,
                                                 num_neurons_hidden, num_neurons_hidden, NUMBER_OF_OUTPUT_NEURONS);

	fann_set_learning_rate(ann, LEARNING_RATE);
    fann_set_activation_function_hidden(ann, FANN_LINEAR);
    fann_set_activation_function_output(ann, FANN_GAUSSIAN_SYMMETRIC);

    while (!acceptable && epoch < MAX_EPOCHS)
    {
		for(epoch = 0; epoch <= MAX_EPOCHS; epoch++)
		{
			struct fann_train_data *data = generate_data(NUMBER_OF_INPUT_NEURONS, NUMBER_OF_OUTPUT_NEURONS, 5000);
			error = fann_train_epoch(ann, data);
			fann_destroy_train(data);
			desired_error_reached = fann_desired_error_reached(ann, DESIRED_ERROR);
			if(epoch % REPORT_EVERY == 0 || epoch == MAX_EPOCHS || epoch == 1 ||
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

		struct fann_train_data *data = generate_data(NUMBER_OF_INPUT_NEURONS, NUMBER_OF_OUTPUT_NEURONS, 600);
		error = fann_train_epoch(ann, data);
		fann_destroy_train(data);
		if (fann_desired_error_reached(ann, DESIRED_ERROR))
		{
			acceptable = true;
		}
    }

    fann_save(ann, NETWORK_SAVE_NAME);
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

	boost::mt19937 gen;

	data_input = (fann_type *) calloc(num_input * num_pairs, sizeof(fann_type));
	data_output = (fann_type *) calloc(num_output * num_pairs, sizeof(fann_type));

	for(i = 0; i != num_pairs; i++)
	{
		boost::uniform_real<> dist(0, pow(2, HASH_WIDTH_IN_BITS));
		boost::variate_generator<boost::mt19937&, boost::uniform_real<> > random(gen, dist);

		boost::dynamic_bitset<> value(HASH_WIDTH_IN_BITS, random());
		boost::dynamic_bitset<> hash(HASH_WIDTH_IN_BITS, kennys_hash_16(value.to_ulong()));

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

void load_trained_network()
{
    trained_network = fann_create_from_file(NETWORK_SAVE_NAME);

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
	cout << "Input: " << convert_array_to_string(fann_input, HASH_WIDTH_IN_BITS) << "\n";
	cout << "Output: " << convert_array_to_string(calc_out, HASH_WIDTH_IN_BITS) << "\n";
	
	unsigned int output_binary = convert_fann_out_to_binary(calc_out, HASH_WIDTH_IN_BITS);
	unsigned int hashed = kennys_hash_16(output_binary);

	printf("Output: ... meh, Which hashes back to: %x\n\n", hashed);
	
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
				NEED_TO_TEST = true;
				// next var is going to be the input

				char temp_array[HASH_WIDTH_IN_BITS];
				strcpy(temp_array, argv[i + 1]);
				
				for(int j = 0; j < HASH_WIDTH_IN_BITS; j++)
				{
					fann_input[j] = (float)(temp_array[j] - '0');
				}
			}
			else if (strcmp(argv[i], "-genTrain") == 0)
			{
				GENERATE_TRAIN_DATA = true;
			}
			else if (strcmp(argv[i], "-train") == 0)
			{
				NEED_TO_TRAIN = true;
				bool printHelpAndExit = false;
				if (i+1 >= argc)
				{
					printHelpAndExit = true;
				}
				else if (strcmp(argv[i+1], "file") == 0)
				{
					NO_FILE_TRAIN = false;
				}
				else if (strcmp(argv[i+1], "block") == 0)
				{
					NO_FILE_TRAIN = true;
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
				OUTPUT_TO_FILE = true;
			}
			else if (strcmp(argv[i], "-bypass") == 0)
			{
				// bypasses all functions, used for testing single functions
				BYPASS_EVERYTHING = true;
			}
			else if (strcmp(argv[i], "-i") == 0) // input
			{
				
			}
			else if (strcmp(argv[i], "-nin") == 0) // # input neurons
			{
				NUMBER_OF_INPUT_NEURONS = atoi(argv[i + 1]);
			}
			else if (strcmp(argv[i], "-non") == 0) // # output neurons
			{
				NUMBER_OF_OUTPUT_NEURONS = atoi(argv[i + 1]);
			}
			else if (strcmp(argv[i], "-nl") == 0) // # layers
			{
				
			}
			else if (strcmp(argv[i], "-nnil") == 0) // number of neurons in each layer
			{
				// should be formatted 8,16,6,8
				// as in in,middle,middle,out
				
			}
			else if (strcmp(argv[i], "-nb") == 0)
			{
				NUMBER_OF_BITS_FOR_INPUT = atoi(argv[i + 1]);
			}
			else if (strcmp(argv[i], "-de") == 0) // desired error
			{
				DESIRED_ERROR = atoi(argv[i + 1]);
			}
			else if (strcmp(argv[i], "-lrate") == 0) // learning rate
			{
				LEARNING_RATE = atoi(argv[i + 1]);
			}
			else if (strcmp(argv[i], "-epochs") == 0) // max epochs
			{
				MAX_EPOCHS = atoi(argv[i + 1]);
			}
			else if (strcmp(argv[i], "-reports") == 0) // reports every number of epochs
			{
				REPORT_EVERY = atoi(argv[i + 1]);
			}
			else if (strcmp(argv[i], "-help") == 0)
			{
				display_help();
			}
			else
			{
				display_help();
			}
			
		}
		
	}
	
	if (!BYPASS_EVERYTHING)
	{
		// actually begin executing
	    string fname = get_current_time();
	    fname.append(".txt");


	    char *filename = (char*)fname.c_str();


	    FILE  *fs;
	    if (OUTPUT_TO_FILE)
		{
			cout << "File name for this run is going to be: " << filename << "\n";

		    if((fs=freopen(filename, "w" ,stdout)) == NULL) {
		        printf("Cannot open file.\n");
		        exit(1);
		    }

		}
		
		if (GENERATE_TRAIN_DATA) generate_train_file();
	    if (NEED_TO_TRAIN)
	    {
	    	if (NO_FILE_TRAIN) {
	    		train_network_no_file();
	    	} else {
	    		train_network();
	    	}
	    }
	    if (NEED_TO_TEST)
		{
			load_trained_network();
			test_network();     
			fann_destroy(trained_network);
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
	cout << "\t -genTrain \t\tGenerates the training file to train the neural network with\n";
	cout << "\t -train <mode> \t\tTrains the network using the specified training mode\n";
	cout << "\t -test 1010101001 \tRuns a trained network with the given input (should be the hash you\n";
						cout << "\t\t\t\t are trying to find the pre-image for)\n";
	cout << "\t -file \t\t\tInstead of standard out, outputting to the console, including this will have\n";
						cout << "\t\t\t\t all output go to a file with the date and time\n";
	
	cout << "\n\nTraining Modes: \n";
	cout << "\t block\t\tTrains the network using dynamically generated, random data\n";
	cout << "\t file\t\tTrains the network using a pre-generated data file\n";

	cout << "\n\nNetwork Configuration: \n";
	cout << "\t -i \t\tInput? [Not Yet Implemented]\n";
	cout << "\t -nin \t\t[" << NUMBER_OF_INPUT_NEURONS << "] Number of input neurons \n";
	cout << "\t -non \t\t[" << NUMBER_OF_OUTPUT_NEURONS << "] Number of output neuors\n";
	cout << "\t -nl \t\tNumber of layers [Not Yet Implemented]\n";
	cout << "\t -nnil \t\tNumber of neurons in each layer [Not Yet Implemented]\n";
	cout << "\t -nb \t\t[" << NUMBER_OF_BITS_FOR_INPUT << "] Number of bits the network should expect for the input\n";
	
	cout << "\n\nNetwork Training Constants:\n";
	cout << "\t -de 0.00001 \t\t[" << DESIRED_ERROR << "] Desired Error\n";
	cout << "\t -lrate 0.73 \t\t[" << LEARNING_RATE << "] Learning Rate\n";
	cout << "\t -epochs 10000 \t\t[" << MAX_EPOCHS << "] Number of training iterations (epochs)\n";
	cout << "\t -reports 500 \t\t[" << REPORT_EVERY << "]After how many epochs should should the training report each time?\n";
	
	cout << "\n\nDeveloper Options: \n";
	cout << "\t -bypass \tUsed for skipping all normal functionality and for testing specific functions\n";
	
	cout << "\n\n Example: ./hPif -genTrain -train -test 1111000011110000\n";
	cout << "\t\t Generates the training file, trains the network, and tests the network (assuming the \n";
	cout << "\t\t\tnetwork is set up for a 16 bit hash) with the value F0F0\n\n";
	
	cout << "* Numbers contained in brackets ( [number] ) are the default value...\n\n";
	
}
