#include <iostream>
#include <sstream>

#include "hashes.h"

using namespace std;



/*
	Developed by Kenny Skaggs for initial hash testing
*/
unsigned int kennys_hash(unsigned int out)
{
	char second_half = out >> 4;
	char first_half = out & 0x0fu;

	second_half |= first_half;
	return ( first_half << 4 | second_half );
}

unsigned int kennys_hash_16(unsigned int out)
{
	unsigned char lookup[] = {0x01u, 0x0fu, 0xfcu, 0xe9u, 0x50u, 0x22u, 0xa3u, 0x8au};
	
	short second_half = out >> 8;
	short first_half = out & 0x00ffu;
	
	short lookup_index = second_half >> 5;

	second_half |= first_half;
	
	first_half |= lookup[lookup_index];
	
	return ( first_half << 8 | second_half );
}

// based of murmur
unsigned int prestons_hash_8(unsigned int pre_image)
{	
	// if we want to use actual hashing where the pre_image
	// can be of any length, we can pass what we are hashing
	// as a string of any length
	const unsigned char * data;
	ostringstream num;
	num << pre_image;
	
	unsigned int step_size = 2;
	
	// convert our unsigen int, into string, like a real hash
	// would already use
	data = (unsigned const char *)string(num.str()).c_str();
	int len = 8;
	
	const unsigned int m = 0xab; // constant
	const int num_to_shift_by = 4;
	
	unsigned int h = 0x89; // initialize the hash output
	
	while (len >= step_size)
	{
		// cast data back to int
		// will be useful later, when we have inputs of any length
		h += *(unsigned int *)data;
		h *= m;
		h ^= h >> num_to_shift_by;
		
		data += step_size;
		len -= step_size;
	}
	
	// make the hash hard on the neural net
	switch (len)
	{
		case 2:
			h += data[1] << 2 * step_size;
		case 1:
		h += data[0];
		h *= m;
		h ^= h >> num_to_shift_by;
	}
	
	h *= m;
	h ^= h >> 5;
	h *= m;
	h ^= h >> 7;
	
	return h;
	
}

unsigned int prestons_8bit_sha(unsigned int pre_image)
{
	
}