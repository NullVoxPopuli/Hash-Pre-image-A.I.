#include <string>
#include <bitset>
#include <sstream>

using namespace std;

unsigned int convert_fann_out_to_binary(fann_type * net_out, int len);
void fann_run_many(struct fann **anns, fann_type * input, fann_type **output, int num_anns, int num_runs);


void replaceAll(std::string& str, const std::string& from, const std::string& to);

string convert_binary_byte_to_hex(string binary_value, int number_of_bits);
string convert_binary_to_FANN_array(string binary_string);
string convert_FANN_array_to_binary(string fann_array);

string convert_array_to_string(fann_type *arr, int width);

string pad_word(string word_to_pad, int width);
void generate_train_file();
