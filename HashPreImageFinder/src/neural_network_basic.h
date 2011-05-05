#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/lexical_cast.hpp>


#include "fann.h"
#include "floatfann.h"

#include "fann_utils.h"
#include "Config.h"


struct fann *fann_create_network(int num, unsigned int args[]);
void print_config();

void train_network();
void train_network_no_file();

struct fann_train_data *generate_data(unsigned int num_input, unsigned int num_output, unsigned int num_pairs);
struct fann_train_data *generate_swarm_data(unsigned int num_input, unsigned int num_pairs, unsigned int output_position);

struct fann **allocate_swarm();
void train_the_swarm(struct fann **swarm);
void free_the_swarm(struct fann **swarm);

struct fann * load_trained_network();


void test_network(struct fann * trained_network, fann_type *fann_input);
unsigned int test_network_with_value(struct fann * trained_network, unsigned int hash_value);
void auto_test_network_with_random_data();

unsigned int test_swarm_with_value(struct fann **swarm, int hash_value);
void auto_test_swarm(struct fann **swarm, unsigned int num_of_data_sets_to_test);
struct fann **load_trained_swarm();