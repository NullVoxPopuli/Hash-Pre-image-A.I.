
#include <iostream>
#include <bitset>
#include <sstream>
#include <fstream>
#include <time.h>
#include <string.h>


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
    const unsigned int num_input = hash_width_in_bits;
    const unsigned int num_output = 32;
    const unsigned int num_layers = 4;
    const unsigned int num_neurons_hidden = 32;

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
    const unsigned int num_input = 8;
    const unsigned int num_output = 8;
    const unsigned int num_layers = 4;
    const unsigned int num_neurons_hidden = 16;

    float error = DESIRED_ERROR + 1;
    unsigned int epoch = 0;
    int desired_error_reached;
    int acceptable = false;

    struct fann *ann = fann_create_standard(num_layers, num_input,
                                                 num_neurons_hidden, 8, num_output);

	fann_set_learning_rate(ann, LEARNING_RATE);
    fann_set_activation_function_hidden(ann, FANN_LINEAR);
    fann_set_activation_function_output(ann, FANN_GAUSSIAN_SYMMETRIC);

    while (!acceptable && epoch < MAX_EPOCHS)
    {
		for(epoch = 0; epoch <= MAX_EPOCHS; epoch++)
		{
			struct fann_train_data *data = generate_data(num_input, num_output, 200, 0, pow(2, hash_width_in_bits));
			error = fann_train_epoch(ann, data);
			fann_destroy_train(data);
			desired_error_reached = fann_desired_error_reached(ann, DESIRED_ERROR);

			if(epoch % REPORT_EVERY == 0 || epoch == MAX_EPOCHS || epoch == 1 ||
						desired_error_reached == 0)
					{
						printf("Epochs     %8d. Current error: %.10f. Bit fail %d.\n", epoch, error,
							   ann->num_bit_fail);
					}

			if(desired_error_reached == 0)
				break;
		}

		struct fann_train_data *data = generate_data(num_input, num_output, 600, 0, pow(2, hash_width_in_bits));
		error = fann_train_epoch(ann, data);
		if (fann_desired_error_reached(ann, DESIRED_ERROR))
		{
			acceptable = true;
		}
    }

    fann_save(ann, NETWORK_SAVE_NAME);
    fann_destroy(ann);
}

struct fann_train_data *generate_data(unsigned int num_input, unsigned int num_output, unsigned int num_pairs, unsigned int min_value, unsigned int max_value)
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
	srand(time(0));

	data_input = (fann_type *) calloc(num_input * num_pairs, sizeof(fann_type));
	data_output = (fann_type *) calloc(num_output * num_pairs, sizeof(fann_type));

	for(i = 0; i != num_pairs; i++)
	{
		unsigned int value = (rand() % max_value) + min_value;
		unsigned int hash = kennys_hash_16(value);

		unsigned int mask = pow(2, hash_width_in_bits-1);

		data->input[i] = data_input;
		data_input += num_input;
		unsigned int tempMask = mask;

		for(j = 0; j != num_input; j++)
		{
			if ((hash & tempMask) > 0)
				data->input[i][j] = 1;
			else
				data->input[i][j] = 0;
			tempMask >>= 1;
		}

		data->output[i] = data_output;
		data_output += num_output;
		tempMask = mask;

		for(j = 0; j != num_output; j++)
		{
			if ((value & tempMask) > 0)
				data->output[i][j] = 1;
			else
				data->output[i][j] = 0;
			tempMask >>= 1;
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
	cout << "Input: " << convert_array_to_string(fann_input, hash_width_in_bits) << "\n";
	cout << "Output: " << convert_array_to_string(calc_out, hash_width_in_bits) << "\n";
	
	unsigned int output_binary = convert_fann_out_to_binary(calc_out, hash_width_in_bits);
	unsigned int hashed = MurmurHash(output_binary, hash_width_in_bits, 0);

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
		cout << "Need to at least train or test\n\n";
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

				char temp_array[hash_width_in_bits];
				strcpy(temp_array, argv[i + 1]);
				
				for(int j = 0; j < hash_width_in_bits; j++)
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
				
			}
			else if (strcmp(argv[i], "-non") == 0) // # output neurons
			{
				
			}
			else if (strcmp(argv[i], "-nl") == 0) // # layers
			{
				
			}
			else if (strcmp(argv[i], "-nnil") == 0) // number of neurons in each layer
			{
				// should be formatted 8,16,6,8
				// as in in,middle,middle,out
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
			else if (strcmp(argv[i], "-nofile") == 0)
			{
				NEED_TO_TRAIN = true;
				NO_FILE_TRAIN = true;
			}
			else
			{
				cout << "Argument not recognized: " << argv[i] << "\n";
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
