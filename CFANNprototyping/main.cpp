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
const char* data_file_name = "data_file.fanndata";

struct fann *trained_network;

void train_network()
{
    printf("Training ... \n");
    const unsigned int num_input = 2;
    const unsigned int num_output = 1;
    const unsigned int num_layers = 3;
    const unsigned int num_neurons_hidden = 3;
    const float desired_error = (const float) 0.001;
    const unsigned int max_epochs = 1000;
    const unsigned int epochs_between_reports = 100;
    struct fann *ann = fann_create_standard(num_layers, num_input,
                                            num_neurons_hidden, num_output);
    
    fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);
    fann_train_on_file(ann, data_file_name, max_epochs,
                       epochs_between_reports, desired_error);
    fann_save(ann, network_save_name);
    fann_destroy(ann);
    
}

void load_trained_network()
{
    trained_network = fann_create_from_file(network_save_name);

}

void test_network()
{
    printf("Running trained network ... \n ");
    
    fann_type *calc_out;
    fann_type input[2];
    
    
    input[0] = -1;
    input[1] = 1;
    calc_out = fann_run(trained_network, input);

    printf("xor test (%f,%f) -> %f\n", input[0], input[1], calc_out[0]);

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
    
    
    string fname = get_current_time();
    fname.append(".txt");
    char *filename = (char*)fname.c_str();

    FILE  *fs;
    cout << "File name for this run is going to be: " << filename << "\n";
    
    //cout.rdbuf(fs.rdbuf()); // redirect cout
    
    if((fs=freopen(filename, "w" ,stdout)) == NULL) {
        printf("Cannot open file.\n");
        exit(1);
    }
    
    
    
    train_network();
//    
//    load_trained_network()
//    
//    test_network();
//    
//    fann_destroy(trained_network);
    
//    cout << convert_binary_byte_to_hex("1111111100000000", 16) << "\n";

    
}