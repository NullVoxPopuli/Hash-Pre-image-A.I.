#include <stdio.h>
#include <iostream>
#include <string>
#include <bitset>
#include <sstream>

#include "fann.h"
#include "floatfann.h"

using namespace std;

const char* network_save_name = "network_float_save.net";
const char* data_file_name = "data_file.fanndata";

struct fann *trained_network;

string convert_binary_to_hex(string binary_value, int number_of_bits)
{
    bitset<8> set(binary_value);	
    ostringstream result;
    result << hex << set.to_ulong() << endl;
    return result.str();
}

string convert_binary_to_FANN_array(string binary_string)
{
    
}

string convert_FANN_array_to_binary(string fann_array)
{
    
}

void train_network()
{
    printf("Training ... \n");
    const unsigned int num_input = 2;
    const unsigned int num_output = 1;
    const unsigned int num_layers = 3;
    const unsigned int num_neurons_hidden = 3;
    const float desired_error = (const float) 0.001;
    const unsigned int max_epochs = 500000;
    const unsigned int epochs_between_reports = 1000;
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

int main (int argc, const char * argv[])
{
//    train_network();
//    
//    load_trained_network()
//    
//    test_network();
//    
//    fann_destroy(trained_network);
    
    cout << convert_binary_to_hex("1111111100000000", 16);
}