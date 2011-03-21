#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <bitset>
#include <sstream>
#include <fstream>
#include <time.h>


#include "fann.h"
#include "floatfann.h"

#include "main.h"
#include "fann_utils.h"

using namespace std;

const char* network_save_name = "network_float_save.net";
const char* data_file_name = "8bithash.fanndata";

struct fann *trained_network;

void train_network()
{
    printf("Training ... \n");
    const unsigned int num_input = 8;
    const unsigned int num_output = 8;
    const unsigned int num_layers = 4;
    const unsigned int num_neurons_hidden = 17;
    const float desired_error = (const float) 0.001;
    const unsigned int max_epochs = 10000;
    const unsigned int epochs_between_reports = 500;
    struct fann *ann = fann_create_standard(num_layers, num_input,
                                            num_neurons_hidden, 9, num_output);
    
    fann_set_activation_function_hidden(ann, FANN_LINEAR);
    fann_set_activation_function_output(ann, FANN_GAUSSIAN_SYMMETRIC);
    fann_train_on_file(ann, data_file_name, max_epochs,
                       epochs_between_reports, desired_error);
    fann_save(ann, network_save_name);
    fann_destroy(ann);
    
}

void load_trained_network()
{
    trained_network = fann_create_from_file(network_save_name);

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
    fann_type input[8];
    
    
    input[0] = 1;
    input[1] = 1;
	input[2] = 1;
	input[3] = 1;
	input[4] = 1;
	input[5] = 1;
	input[6] = 1;
	input[7] = 1;
    calc_out = fann_run(trained_network, input);

    // printf("xor test (%f,%f) -> %f\n", input[0], input[1], calc_out[0]);
	cout << "Input: " << convert_array_to_string(input, 8) << "\n";
	cout << "Output: " << convert_array_to_string(calc_out, 8) << "\n";
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
			if (strcmp(argv[i], "-test"))
			{
				NEED_TO_TEST = true;
			}
			else if (strcmp(argv[i], "-train"))
			{
				NEED_TO_TRAIN = true;
			}
			else if (strcmp(argv[i], "-file"))
			{
				OUTPUT_TO_FILE = true;
			}
			else
			{
				cout << "Argument not recognized: " << argv[i] << "\n";
			}
		}
	}
	

	// actually begin executing
    string fname = get_current_time();
    fname.append(".txt");
	// fname = "output/".append(fname);
    char *filename = (char*)fname.c_str();
	
    FILE  *fs;
    if (OUTPUT_TO_FILE)
	{
		cout << "File name for this run is going to be: " << filename << "\n";
    
	    //cout.rdbuf(fs.rdbuf()); // redirect cout
    
	    if((fs=freopen(filename, "w" ,stdout)) == NULL) {
	        printf("Cannot open file.\n");
	        exit(1);
	    }
	
	}

    
    if (NEED_TO_TRAIN) train_network();
    if (NEED_TO_TEST)
	{
		load_trained_network();
		test_network();     
		fann_destroy(trained_network);
	}

}