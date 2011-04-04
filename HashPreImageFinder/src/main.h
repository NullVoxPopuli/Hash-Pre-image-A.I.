

#include <string>
#include <stdio.h>
#include <stdlib.h>

fann_type fann_input[32];


void train_network();
void test_network();
unsigned int test_network_with_value(int hash_image);
void auto_test_network_with_random_data(unsigned int start, unsigned int end, unsigned int num_of_data_sets_to_test);



void load_trained_network();

int main (int argc, const char * argv[]);

void display_help();

struct fann_train_data *generate_data(unsigned int num_input, unsigned int num_output, unsigned int num_pairs);
