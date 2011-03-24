/*
 
 Header for main
 Contains global constants
 
 */

// #define FALSE 0
// #define TRUE 1
// #define  False 0
// #define True 1
// #define false 0
// #define true 1

int OUTPUT_TO_FILE = false;
int NEED_TO_TRAIN = false;
int NEED_TO_TEST = false;
int BYPASS_EVERYTHING = false;
int MAX_EPOCHS = 10000;
int REPORT_EVERY = 500;

double LEARNING_RATE = 0.5;
double DESIRED_ERROR = 0.00001;



int hash_width_in_bits = 8;

void train_network();

void load_trained_network();

int main (int argc, const char * argv[]);