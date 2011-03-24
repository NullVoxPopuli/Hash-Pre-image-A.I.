/*
 
 Header for main
 Contains global constants
 
 */

#include <string>
#include <stdio.h>
#include <stdlib.h>

int OUTPUT_TO_FILE = false;
int NEED_TO_TRAIN = false;
int NEED_TO_TEST = false;
int BYPASS_EVERYTHING = false;
int MAX_EPOCHS = 10000;
int REPORT_EVERY = 500;

double LEARNING_RATE = 0.5;
double DESIRED_ERROR = 0.00001;

const char* network_save_name = "network_float_save.net";
const char* data_file_name = "8bithash.fanndata";

int hash_width_in_bits = 8;

fann_type fann_input[8];


void train_network();

void load_trained_network();

int main (int argc, const char * argv[]);