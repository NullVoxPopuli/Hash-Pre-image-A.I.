
#include "fann_utils.h"

using namespace std;

string convert_binary_byte_to_hex(string binary_value, int number_of_bits)
{
    bitset<8> set(binary_value);	
    ostringstream result;
    result << hex << set.to_ulong() << endl;
    return result.str();
}

// take from: 
//http://stackoverflow.com/questions/3418231/c-replace-part-of-a-string-with-another-string
//
void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) 
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

string convert_binary_to_FANN_array(string binary_string)
{
    string result = binary_string;
    
    replaceAll(result, "1", "1 ");
    replaceAll(result, "0", "-1 ");
    
    return result;
}

string convert_FANN_array_to_binary(string fann_array)
{
    string result = fann_array;
    
    replaceAll(result, "-1 ", "0");
    replaceAll(result, "1 ", "1");
    
    return result;
}