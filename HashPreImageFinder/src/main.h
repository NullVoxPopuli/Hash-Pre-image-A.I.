

#include <string>
#include <stdio.h>
#include <stdlib.h>

fann_type fann_input[32];


void train_network();

void load_trained_network();

int main (int argc, const char * argv[]);

void display_help();

struct fann_train_data *generate_data(unsigned int num_input, unsigned int num_output, unsigned int num_pairs);
