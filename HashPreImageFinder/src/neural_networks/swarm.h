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

#include "../fann_utils.h"
#include "../Config.h"

#include "shared.h"

struct fann_train_data *generate_swarm_data(unsigned int num_input, unsigned int num_pairs, unsigned int output_position);

struct fann **allocate_swarm();
void train_the_swarm(struct fann **swarm);
void free_the_swarm(struct fann **swarm);



unsigned int test_swarm_with_value(struct fann **swarm, int hash_value);
void auto_test_swarm(struct fann **swarm, unsigned int num_of_data_sets_to_test);
struct fann **load_trained_swarm();