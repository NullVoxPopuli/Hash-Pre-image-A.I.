#include "swarm.h"
#include "basic.h"
#include <dirent.h>


struct fann_train_data *generate_swarm_data(unsigned int num_input, unsigned int num_pairs, unsigned int output_position)
{
	struct fann_train_data *data = (struct fann_train_data *) malloc(sizeof(struct fann_train_data));
	fann_init_error_data((struct fann_error *) data);
	unsigned int i, j;
	fann_type *data_input, *data_output;

	data->num_data = num_pairs;
	data->num_input = num_input;
	data->num_output = 1;
	data->input = (fann_type **) calloc(num_pairs, sizeof(fann_type *));
	data->output = (fann_type **) calloc(num_pairs, sizeof(fann_type *));

	time_t T = time(0);
	boost::mt19937 gen(T);

	data_input = (fann_type *) calloc(num_input * num_pairs, sizeof(fann_type));
	data_output = (fann_type *) calloc(num_pairs, sizeof(fann_type));

	for(i = 0; i != num_pairs; i++)
	{
		boost::uniform_real<> dist(0, pow(2,  Config::HASH_WIDTH_IN_BITS));
		boost::variate_generator<boost::mt19937&, boost::uniform_real<> > random(gen, dist);

		boost::dynamic_bitset<> value( Config::HASH_WIDTH_IN_BITS, random());
		boost::dynamic_bitset<> hash( Config::HASH_WIDTH_IN_BITS, Config::current_hash_function(value.to_ulong()));
//		cout << value << " hashes to " << hash << "\n";

		data->input[i] = data_input;
		data_input += num_input;

		for(j = 0; j != num_input; j++)
		{
			data->input[i][j] = (hash[j] == 0 ? -1 : 1);
		}

		data->output[i] = data_output;
		data_output += 1;

		data->output[i][0] = (value[output_position] == 0 ? -1 : 1);
		
//		cout << "this network should map to " << value[output_position] << "\n";
	}
	return data;
}

struct fann **allocate_swarm()
{
	struct fann **for_the_swarm;
	for_the_swarm = (fann**) malloc(sizeof(fann*) * Config::NUMBER_OF_OUTPUT_NEURONS);

	Config::LAYERS[Config::NUMBER_OF_LAYERS-1] = 1;
	for(int i=0; i<Config::NUMBER_OF_OUTPUT_NEURONS; i++)
	{
		for_the_swarm[i] = fann_create_network(Config::NUMBER_OF_LAYERS, Config::LAYERS);
		fann_set_learning_rate(for_the_swarm[i], Config::LEARNING_RATE);
		fann_set_learning_momentum(for_the_swarm[i], Config::LEARNING_MOMENTUM);
	    fann_set_activation_function_hidden(for_the_swarm[i], FANN_SIGMOID_SYMMETRIC);
	    fann_set_activation_function_output(for_the_swarm[i], FANN_SIGMOID_SYMMETRIC);
	    fann_set_training_algorithm(for_the_swarm[i], FANN_TRAIN_BATCH);
	}

	return for_the_swarm;
}

void train_the_swarm(struct fann **swarm)
{
	for(int i=0; i<Config::NUMBER_OF_OUTPUT_NEURONS; i++)
	{
		cout << "Training ANN " << i+1 << " of " << Config::NUMBER_OF_OUTPUT_NEURONS << "\n";
		struct fann_train_data *data = generate_swarm_data(Config::NUMBER_OF_INPUT_NEURONS, Config::MAX_NUMBER_OF_TRAINING_DATA, i);
		fann_train_on_data(swarm[i], data, Config::MAX_EPOCHS, Config::REPORT_EVERY, Config::DESIRED_ERROR);
		fann_destroy_train(data);
		cout << "\n";
	}
}

void free_the_swarm(struct fann **swarm)
{
	for(int i=0; i<Config::NUMBER_OF_OUTPUT_NEURONS; i++)
	{
		//First, forever freeze the swarm
		char *save_name = new char[100];
		strcpy(save_name, Config::NETWORK_SAVE_NAME);
		strcat(save_name, boost::lexical_cast<std::string>(i).c_str());
	    fann_save(swarm[i], save_name);
		fann_destroy(swarm[i]);
		free(save_name);
	}
	free(swarm);
}


struct fann **load_trained_swarm()
{
    struct fann **for_the_swarm;
	for_the_swarm = (fann**) malloc(sizeof(fann*) * (Config::HASH_WIDTH_IN_BITS));
	
	
	if ((dir = opendir("SwarmConfig")) == NULL)
	{
		std::cout << "Swarm Directory not found\n";
		exit(1);
	}
	else 
	{
		for(int i=0; i<Config::HASH_WIDTH_IN_BITS; i++)
		{
			char *load_name = new char[100];
			strcpy(load_name, Config::NETWORK_SAVE_NAME);
			strcat(load_name, boost::lexical_cast<std::string>(i).c_str());
			for_the_swarm[i] = fann_create_from_file(load_name);
			free(load_name);
		}
	}

	
	return for_the_swarm;
}


unsigned int test_swarm_with_value(struct fann **swarm, int hash_value)
{
	boost::dynamic_bitset<> output (Config::HASH_WIDTH_IN_BITS);
	boost::dynamic_bitset<> buffer (Config::HASH_WIDTH_IN_BITS, hash_value);
	fann_type auto_fann_input[Config::HASH_WIDTH_IN_BITS];

	for(int j=0; j < Config::HASH_WIDTH_IN_BITS; j++)
	{
		auto_fann_input[j] = (float)(buffer[j] == 0 ? -1 : 1);
	}
	
	for(int i=0; i < Config::HASH_WIDTH_IN_BITS; i++)
	{
		fann_type *calc_out;
		calc_out = fann_run(swarm[i], auto_fann_input);
//		unsigned int output_binary = convert_fann_out_to_binary(calc_out, 1);
//		cout << output_binary << "\n";
		if (calc_out[0] > 0)
		{
			output[i] = 1;
		}
		else
		{
			output[i] = 0;
		}
	}
//	cout << output << "\n";
	return output.to_ulong();
}

void auto_test_swarm(struct fann **swarm, unsigned int num_of_data_sets_to_test)
{
	srand((unsigned)time(0));
	unsigned int random_pre_image_value;
	unsigned int hashed_value;
	unsigned int result;
	int failed = false;
	int num_fail = 0;
	time_t T = time(0);
	boost::mt19937 gen(T);
	unsigned int range = Config::TEST_MAX - Config::TEST_MIN;
	
	for (unsigned int i=0; i<num_of_data_sets_to_test; i++)
	{
		boost::uniform_real<> dist(0, pow(2,  Config::HASH_WIDTH_IN_BITS));
		boost::variate_generator<boost::mt19937&, boost::uniform_real<> > random(gen, dist);

		
		random_pre_image_value = Config::TEST_MIN + ((int)random() % range);
		hashed_value = Config::current_hash_function(random_pre_image_value);
		result = test_swarm_with_value(swarm, hashed_value);

		unsigned int result_hash = (unsigned int)Config::current_hash_function(result);
		if (result_hash != hashed_value)
		{
			failed = true;
			cout << "Error:\n";
			cout << "   Hash:             " << hashed_value << "\n";
			cout << "   Result:           " << result << " (which hashes to " << (unsigned int)Config::current_hash_function(result) << ")\n";
			cout << "   Should Have been: " << random_pre_image_value << "\n\n";
			num_fail += 1;
		}
	}
	
	if (!failed)
	{
		cout << "All tested hashes were reversed successfully... \n";
	}
	else
	{
		cout << num_fail << " of " << num_of_data_sets_to_test << " failed\n";
	}
}

