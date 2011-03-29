/*
  Contains global constants
 
 */


static int OUTPUT_TO_FILE = false;
static int NEED_TO_TRAIN = false;
static int NEED_TO_TEST = false;
static int BYPASS_EVERYTHING = false;
static int GENERATE_TRAIN_DATA = false;

static int MAX_EPOCHS = 10000;
static int REPORT_EVERY = 500;

static double LEARNING_RATE = 0.5;
static double DESIRED_ERROR = 0.00001;

static const char* NETWORK_SAVE_NAME = "network_float_save.net";
static const char* DATA_FILE_NAME = "8bithash.fanndata";

static int hash_width_in_bits = 32;
