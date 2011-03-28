#include <stdio.h>
#include <fstream>
#include <iostream>
#include <bitset>

#include "fann.h"
#include "floatfann.h"

#include "fann_utils.h"
#include "config.h"


using namespace std;

int bits_in_a_nibble = 4;

unsigned short convert_fann_out_to_binary(fann_type * net_out, int len)
{
	ostringstream str_result;
	unsigned char result = 0;
	int neuron_output;
	for (int i = 0; i < len; i++)
	{
		if (net_out[i] > 0.5)
			neuron_output = 1;
		else
			neuron_output = 0;
			
		str_result << neuron_output;
	}
	const char * out = str_result.str().c_str();\
	char * end; // throwaway
	return strtol(out, &end, 2);
}

/*
	Developed by Kenny Skaggs for initial hash testing
*/
unsigned char kennys_hash(unsigned char out)
{
	char second_half = out >> 4;
	char first_half = out & 0x0fu;

	second_half |= first_half;
	return ( first_half << 4 | second_half );
}

unsigned short kennys_hash_16(unsigned short out)
{
	unsigned char lookup[] = {0x01u, 0x0fu, 0xfcu, 0xe9u, 0x50u, 0x22u, 0xa3u, 0x8au};
	
	short second_half = out >> 8;
	short first_half = out & 0x00ffu;
	
	short lookup_index = second_half >> 5;

	second_half |= first_half;
	
	first_half |= lookup[lookup_index];
	
	return ( first_half << 8 | second_half );
}

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
    replaceAll(result, "0", "0 ");
    
    return result;
}

string convert_FANN_array_to_binary(string fann_array)
{
    string result = fann_array;
    
    replaceAll(result, "0 ", "0");
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

void generate_train_file()
{
	cout << "Generating training file ... \n";
	int max_num_data = 65535;
	ofstream file;
	file.open(DATA_FILE_NAME);
	unsigned char cur_value = 0x0000u;
	
	file << max_num_data << " " << hash_width_in_bits << " " << hash_width_in_bits << "\n";
	
	for(int i = 0; i < max_num_data; i++)
	{
		bitset<16> bits_hash(kennys_hash_16(cur_value));
		bitset<16> bits_value(cur_value);

		file << convert_binary_to_FANN_array(bits_hash.to_string<char,char_traits<char>,allocator<char> >()) << "\n";
		file << convert_binary_to_FANN_array(bits_value.to_string<char,char_traits<char>,allocator<char> >()) << "\n";
		cur_value++;
		
	}
	file.close();
	
	cout << "Done\n\n";
}