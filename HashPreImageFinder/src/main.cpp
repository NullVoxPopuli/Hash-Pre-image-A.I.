
#include "main.h"

using namespace std;


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

//http://www.blog.highub.com/c-plus-plus/c-parse-split-delimited-string/
void split(const string& s, char c,
           vector<string>& v) {
   string::size_type i = 0;
   string::size_type j = s.find(c);
   while (j != string::npos) {
      v.push_back(s.substr(i, j-i));
      i = ++j;
      j = s.find(c, j);
      if (j == string::npos)
         v.push_back(s.substr(i, s.length( )));
   }
}


int main (int argc, const char * argv[])
{
	fann_type *fann_input;
	
    // handle arguments
    if (argc < 2)
	{
		display_help();
	}
	else
	{
		for (int i = 1; i < argc; i++)
		{
			if (argv[i][0] != '-') 
			{
				// bad argument
				display_help();
				exit(1);
			}
			
			if (strcmp(argv[i], "-test") == 0)
			{
				Config::NEED_TO_TEST = true;
				// next var is going to be the input

				char temp_array[Config::HASH_WIDTH_IN_BITS];
				strcpy(temp_array, argv[i + 1]);
				i++;
				
				// this should actually call the method
				
				// for(int j = 0; j < Config::HASH_WIDTH_IN_BITS; j++)
				// 			{
				// 				fann_input[j] = (float)(temp_array[j] - '0');
				// 			}
			}
			else if (strcmp(argv[i], "-autoTest") == 0)
			{
				auto_test_network_with_random_data(atoi(argv[i + 1]), atoi(argv[i + 2]), atoi(argv[i + 3]));
				i += 3;
			}
			else if (strcmp(argv[i], "-genTrain") == 0)
			{
				Config::GENERATE_TRAIN_DATA = true;
			}
			else if (strcmp(argv[i], "-train") == 0)
			{
				Config::NEED_TO_TRAIN = true;
				if (strcmp(argv[i+1], "file") == 0)
				{
					cout << "Training on file data... \n";
					Config::NO_FILE_TRAIN = false;
					i++;
				}
				else if (strcmp(argv[i+1], "block") == 0)
				{
					cout << "Training on generated data.... \n";
					Config::NO_FILE_TRAIN = true;
					i++;
				}
				else
				{
					cout << "need to specify train mode ('file' or 'block')\n";
					exit(1);
				}
			}
			else if (strcmp(argv[i], "-file") == 0)
			{
				Config::OUTPUT_TO_FILE = true;
			}
			else if (strcmp(argv[i], "-bypass") == 0)
			{
				// bypasses all functions, used for testing single functions
				Config::BYPASS_EVERYTHING = true;
			}
			else if (strcmp(argv[i], "-nnil") == 0) // number of neurons in each layer
			{
				// should be formatted 8,16,6,8
				// as in in,middle,middle,out
				vector<string> v;
				split(argv[i + 1], ',', v);
				for (int j = 0; j < v.size( ); ++j) {
					Config::LAYERS[j] = atoi(v[j].c_str());
			   	}
				Config::NUMBER_OF_INPUT_NEURONS = Config::LAYERS[0];
				Config::NUMBER_OF_OUTPUT_NEURONS = Config::LAYERS[v.size() - 1];
				Config::NUMBER_OF_BITS_FOR_INPUT = Config::NUMBER_OF_INPUT_NEURONS;
				Config::HASH_WIDTH_IN_BITS = Config::NUMBER_OF_INPUT_NEURONS;
				Config::NUMBER_OF_LAYERS = v.size();
				
				i++;					
			}
			else if (strcmp(argv[i], "-nb") == 0)
			{
				Config::NUMBER_OF_BITS_FOR_INPUT = atoi(argv[i + 1]);
				i++;
			}
			else if (strcmp(argv[i], "-de") == 0) // desired error
			{
				if (strcmp(argv[i+1], "auto") == 0)
				{
					Config::DESIRED_ERROR = 1.0 / Config::MAX_NUMBER_OF_TRAINING_DATA / 2.0;
				}
				else
				{
					Config::DESIRED_ERROR = atoi(argv[i + 1]);
				}
				i++;
			}
			else if (strcmp(argv[i], "-lrate") == 0) // learning rate
			{
				Config::LEARNING_RATE = atoi(argv[i + 1]);
				i++;
			}
			else if (strcmp(argv[i], "-epochs") == 0) // max epochs
			{
				Config::MAX_EPOCHS = atoi(argv[i + 1]);
				i++;
			}
			else if (strcmp(argv[i], "-reports") == 0) // reports every number of epochs
			{
				Config::REPORT_EVERY = atoi(argv[i + 1]);
				i++;
			}
			else if (strcmp(argv[i], "-help") == 0)
			{
				display_help();
			}
//=======
//			else if (strcmp(argv[i], "-noFile") == 0)
//			{
//				Config::NEED_TO_TRAIN = true;
//				Config::NO_FILE_TRAIN = true;
//			}
			else if (strcmp(argv[i], "-mTrnData") == 0)
			{
				Config::MAX_NUMBER_OF_TRAINING_DATA = atoi(argv[i + 1]);
				i++;
			}
//			else if (atoi(argv[i]) > 0)
			else if (strcmp(argv[i], "-kennys_hash_16") == 0)
			{
				Config::current_hash_function = &kennys_hash_16;
				Config::HASH_WIDTH_IN_BITS = 16;
			}
			else if (strcmp(argv[i], "-kennys_hash_8") == 0)
			{
				Config::current_hash_function = &kennys_hash;
				Config::HASH_WIDTH_IN_BITS = 8;
			}
			else if (strcmp(argv[i], "-murmur") == 0)
			{
				// need to figure out the syntax for this since the hash is in a class
				Config::current_hash_function = Murmur::hash;
				Config::HASH_WIDTH_IN_BITS = 32;
			}
			else
			{
				display_help();
			}
			
		}
		
	}
	
	if (!Config::BYPASS_EVERYTHING)
	{
		// actually begin executing
	    string fname = get_current_time();
	    fname.append(".txt");


	    char *filename = (char*)fname.c_str();


	    FILE  *fs;
	    if (Config::OUTPUT_TO_FILE)
		{
			cout << "File name for this run is going to be: " << filename << "\n";

		    if((fs=freopen(filename, "w" ,stdout)) == NULL) {
		        printf("Cannot open file.\n");
		        exit(1);
		    }

		}
		
		if (Config::GENERATE_TRAIN_DATA) generate_train_file();
	    if (Config::NEED_TO_TRAIN)
	    {
	    	if (Config::NO_FILE_TRAIN) {
	    		train_network_no_file();
	    	} else {
	    		train_network();
	    	}
	    }
	    if (Config::NEED_TO_TEST)
		{
			struct fann * trained_network = load_trained_network();
			test_network(trained_network, fann_input);
			fann_destroy(trained_network); 
		}
		
	}
	else
	{
		cout << "Nothing here at the moment...\n\n";
	}
	

}

void display_help()
{
	cout << "\n\n";
	cout << "\t :::::::::::::::::::::::::::::\n";
	cout << "\t ::: Hash Pre-Image Finder ::: \n";
	cout << "\t :::::::::::::::::::::::::::::\n";
	
	cout << "\t\t by:\n";
	cout << "\t\t\t L. Preston Sego III and Kenny Skaggs\n\n";
	
	cout << "Available Options: \n";
	cout << "\t -mTrnData 123\t\t[" << Config::MAX_NUMBER_OF_TRAINING_DATA << "] Max Number of Training Data Sets\n";
	cout << "\t -genTrain \t\tGenerates the training file to train the neural network with\n";
	cout << "\t -train <mode> \t\tTrains the network using the specified training mode\n";
	cout << "\t -test 1010101001 \tRuns a trained network with the given input (should be the hash you\n";
						cout << "\t\t\t\t are trying to find the pre-image for) [Needs to be Implemented]\n";
	cout << "\t -autoTest strt end n \t Auto test a trained newwork between two values for n number of values.\n";
	cout << "\t -file \t\t\tInstead of standard out, outputting to the console, including this will have\n";
						cout << "\t\t\t\t all output go to a file with the date and time\n";
	
	cout << "\n\nSelecting Hashes: \n";
	cout << "\t -kennys_hash_16\t [default] simple 16 bit hash\n";
	cout << "\t -kennys_hash_8\t\t simple 8 bit hash\n";
	cout << "\t -murmur\t\t 32 bit hash\n";
	
	cout << "\n\nTraining Modes: \n";
	cout << "\t block\t\tTrains the network using dynamically generated, random data\n";
	cout << "\t file\t\tTrains the network using a pre-generated data file\n";

	cout << "\n\nNetwork Configuration: \n";
	cout << "\t -nnil in,...,out \tNumber of neurons in each layer\n";
	cout << "\t -nb \t\t[" << Config::NUMBER_OF_BITS_FOR_INPUT << "] Number of bits the network should expect for the input\n";
	
	cout << "\n\nNetwork Training Constants:\n";
	cout << "\t -de 0.00001 \t\t[" << Config::DESIRED_ERROR << "] Desired Error. You may use 'auto' instead of a number.\n";
	cout << "\t -lrate 0.73 \t\t[" << Config::LEARNING_RATE << "] Learning Rate\n";
	cout << "\t -epochs 10000 \t\t[" << Config::MAX_EPOCHS << "] Number of training iterations (epochs)\n";
	cout << "\t -reports 500 \t\t[" << Config::REPORT_EVERY << "]After how many epochs should should the training report each time?\n";
	
	cout << "\n\nDeveloper Options: \n";
	cout << "\t -bypass \tUsed for skipping all normal functionality and for testing specific functions\n";
	
	cout << "\n\n Example: ./hPif -genTrain -train -test 1111000011110000\n";
	cout << "\t\t Generates the training file, trains the network, and tests the network (assuming the \n";
	cout << "\t\t\tnetwork is set up for a 16 bit hash) with the value F0F0\n\n";
	
	cout << "* Numbers contained in brackets ( [number] ) are the default value...\n\n";
	
}
