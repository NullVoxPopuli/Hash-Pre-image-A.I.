#include <string>
#include <bitset>
#include <sstream>

using namespace std;

unsigned int convert_fann_out_to_binary(fann_type * net_out, int len);

void replaceAll(std::string& str, const std::string& from, const std::string& to);

string convert_binary_byte_to_hex(string binary_value, int number_of_bits);
string convert_binary_to_FANN_array(string binary_string);
string convert_FANN_array_to_binary(string fann_array);

string pad_word(string word_to_pad, int width);
void generate_train_file();
