#include <string>
#include <bitset>
#include <sstream>

using namespace std;

string convert_binary_byte_to_hex(string binary_value, int number_of_bits);
void replaceAll(std::string& str, const std::string& from, const std::string& to);
string convert_binary_to_FANN_array(string binary_string);
string convert_FANN_array_to_binary(string fann_array);
