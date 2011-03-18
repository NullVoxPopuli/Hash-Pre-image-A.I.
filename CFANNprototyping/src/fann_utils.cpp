#include <stdio.h>
#include <iostream>
#include "fann_utils.h"

using namespace std;

int bits_in_a_nibble = 4;

/*
 Converts an ascii binary string to ascii hex 4 bits at a time
 */
string convert_binary_byte_to_hex(string binary_value, int number_of_bits)
{
    ostringstream result;

    int number_of_nibbles = number_of_bits / 4;
    
    for (int i = 0; i < number_of_nibbles; i++)
    {
        cout << binary_value.substr(i * bits_in_a_nibble, bits_in_a_nibble) << "\n";
        bitset<4> set(binary_value.substr(i * bits_in_a_nibble, bits_in_a_nibble));	
        result << hex << set.to_ulong() << endl;
    }

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

string pad_word(string word_to_pad, int width)
{
    ostringstream result;
    int padding_to_add = width - word_to_pad.length();
    for (int i = 0; i < padding_to_add; i++)
    {
        result << "0";
    }
    result << word_to_pad;
    return result.str();
}

