#include "basic.h"

struct fann *fann_create_network(int num, unsigned int args[])
{
	return fann_create_standard_array(num, &args[0]);
}


void train_network()
{
    printf("Training ... \n");
	print_config();
    // struct fann *ann = fann_create_standard(num_layers, 
    // 											Config::NUMBER_OF_INPUT_NEURONS,
    //                                            Config::HASH_WIDTH_IN_BITS, 
    // 											Config::HASH_WIDTH_IN_BITS, 
    // 											Config::NUMBER_OF_OUTPUT_NEURONS);

	struct fann *ann = fann_create_network(Config::NUMBER_OF_LAYERS, Config::LAYERS);

	// struct fann *ann = fann_create_shortcut(3, 8, 16, 8);

	fann_set_learning_rate(ann, Config::LEARNING_RATE);
	fann_set_learning_momentum(ann, Config::LEARNING_MOMENTUM);
	
    fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

	fann_set_training_algorithm(ann, FANN_TRAIN_BATCH);
	
    fann_train_on_file(ann, 
						Config::DATA_FILE_NAME, 
						Config::MAX_EPOCHS, 
						Config::REPORT_EVERY, 
						Config::DESIRED_ERROR);
	// fann_cascadetrain_on_file(ann, data_file_name, 200, 1, DESIRED_ERROR);
    fann_save(ann, Config::NETWORK_SAVE_NAME);
    fann_destroy(ann);
}

void train_network_no_file()
{
	printf("Training without file ... \n");
    const unsigned int num_layers = 4;

    const unsigned int num_neurons_hidden = Config::HASH_WIDTH_IN_BITS;

    float error = Config::DESIRED_ERROR + 1;
    unsigned int epoch = 0;
    int desired_error_reached;
    int acceptable = false;

	struct fann *ann = fann_create_network(Config::NUMBER_OF_LAYERS, Config::LAYERS);
	

	fann_set_learning_rate(ann, Config::LEARNING_RATE);
    fann_set_activation_function_hidden(ann, FANN_LINEAR);
    fann_set_activation_function_output(ann, FANN_GAUSSIAN_SYMMETRIC);

    while (!acceptable && epoch < Config::MAX_EPOCHS)
    {
		for(epoch = 0; epoch <= Config::MAX_EPOCHS; epoch++)
		{
			struct fann_train_data *data = generate_data( Config::NUMBER_OF_INPUT_NEURONS, 
														 Config::NUMBER_OF_OUTPUT_NEURONS, Config::MAX_NUMBER_OF_TRAINING_DATA);
			error = fann_train_epoch(ann, data);
			fann_destroy_train(data);
			desired_error_reached = fann_desired_error_reached(ann, Config::DESIRED_ERROR);

			if(epoch % Config::REPORT_EVERY == 0 || epoch == Config::MAX_EPOCHS || epoch == 1 ||
						desired_error_reached == 0)
					{
						std::cout << "\nEpochs     " << epoch << ". Current error: " << error << ". Bit fail " << ann->num_bit_fail << "\n";
					}
			else
			{
				std::cout << "\r" << epoch;
			}

			if(desired_error_reached == 0)
				break;
		}

		struct fann_train_data *data = generate_data( Config::NUMBER_OF_INPUT_NEURONS,  Config::NUMBER_OF_OUTPUT_NEURONS, 600);
		error = fann_train_epoch(ann, data);
		fann_destroy_train(data);
		if (fann_desired_error_reached(ann,  Config::DESIRED_ERROR))
		{
			acceptable = true;
		}
    }

    fann_save(ann, Config::NETWORK_SAVE_NAME);
    fann_destroy(ann);
}

struct fann_train_data *generate_data(unsigned int num_input, unsigned int num_output, unsigned int num_pairs)
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

	time_t T = time(0);
	boost::mt19937 gen(T);

	data_input = (fann_type *) calloc(num_input * num_pairs, sizeof(fann_type));
	data_output = (fann_type *) calloc(num_output * num_pairs, sizeof(fann_type));

	for(i = 0; i != num_pairs; i++)
	{
		boost::uniform_real<> dist(0, pow(2,  Config::HASH_WIDTH_IN_BITS));
		boost::variate_generator<boost::mt19937&, boost::uniform_real<> > random(gen, dist);

		boost::dynamic_bitset<> value( Config::HASH_WIDTH_IN_BITS, random());
		boost::dynamic_bitset<> hash( Config::HASH_WIDTH_IN_BITS, Config::current_hash_function(value.to_ulong()));

		data->input[i] = data_input;
		data_input += num_input;

		for(j = 0; j != num_input; j++)
		{
			data->input[i][j] = hash[j];
		}

		data->output[i] = data_output;
		data_output += num_output;

		for(j = 0; j != num_output; j++)
		{
			data->output[i][j] = value[j];
		}
	}
	return data;
}


struct fann * load_trained_network()
{
    return fann_create_from_file(Config::NETWORK_SAVE_NAME);

}


void test_network(struct fann * trained_network, fann_type *fann_input)
{
    printf("Running trained network ... \n ");
    
    fann_type *calc_out;

    calc_out = fann_run(trained_network, fann_input);

    // printf("xor test (%f,%f) -> %f\n", input[0], input[1], calc_out[0]);
	std::cout << "Input: " << convert_array_to_string(fann_input, Config::HASH_WIDTH_IN_BITS) << "\n";
	std::cout << "Output: " << convert_array_to_string(calc_out, Config::HASH_WIDTH_IN_BITS) << "\n";
	
	unsigned int output_binary = convert_fann_out_to_binary(calc_out, Config::HASH_WIDTH_IN_BITS);

	unsigned int hashed = Config::current_hash_function(output_binary);

	printf("Output: ... meh, Which hashes back to: %x\n\n", hashed);
	
}

unsigned int test_network_with_value(struct fann * trained_network, unsigned int hash_value)
{
	fann_type auto_fann_input[Config::HASH_WIDTH_IN_BITS];
	
	boost::dynamic_bitset<> output (Config::NUMBER_OF_OUTPUT_NEURONS);
	boost::dynamic_bitset<> bit_value(Config::HASH_WIDTH_IN_BITS, hash_value);
	string buffer;
	to_string(bit_value, buffer);
	
	// // convert to fann format
	 // for(int j = 0; j < Config::HASH_WIDTH_IN_BITS; j++)
	 // {
	 // 	auto_fann_input[j] = (float)(buffer[j]);
	 // }
	for(int j=0; j < Config::HASH_WIDTH_IN_BITS; j++)
		{
			auto_fann_input[j] = (float)(buffer[j] == 0 ? -1 : 1);
			cout << auto_fann_input[j];
		}
		
	
	fann_type *calc_out;
	calc_out = fann_run(trained_network, auto_fann_input);

	for(int j = 0; j < Config::HASH_WIDTH_IN_BITS; j++)
	{
		if (calc_out[j] > 0) output[j] = 1;
		else output[j] = 0;
	}
	 cout << "\n";
		
	return output.to_ulong();
}

void auto_test_network_with_random_data()
{
	// init randomness 
	srand((unsigned)time(0));
	unsigned int random_pre_image_value;
	unsigned int hashed_value;
	unsigned int result;
	int failed = false;
	int num_failed = 0;
	int range = Config::TEST_MAX - Config::TEST_MIN;
	struct fann * trained_network = load_trained_network();
	
	time_t T = time(0);
	boost::mt19937 gen(T);
	
	for (unsigned int i = 0; i < range; i ++)
	{
		boost::uniform_real<> dist(0, pow(2,  Config::HASH_WIDTH_IN_BITS));
		boost::variate_generator<boost::mt19937&, boost::uniform_real<> > random(gen, dist);

		
		random_pre_image_value = Config::TEST_MIN + ((int)random() % range);		
		hashed_value = Config::current_hash_function(random_pre_image_value);
		result = test_network_with_value(trained_network, hashed_value); 
		
		if (result != random_pre_image_value)
		{
			failed = true;
			std::cout << "Error:\n";
			std::cout << "   Hash:             " << hashed_value << "\n";
			std::cout << "   Pre-Image:        " << result << "\n";
			std::cout << "   Should Have been: " << random_pre_image_value << "\n\n";
			num_failed++;
		}
	}
	
	if (!failed)
	{
		std::cout << "All tested hashes were reversed successfully... \n";
	}
	std::cout << "Number of failed tests: " << num_failed << "\n";
}

