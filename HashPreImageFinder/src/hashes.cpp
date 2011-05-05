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
	const int num_to_shift_by = 2;
	
	unsigned char h = 0x89; // initialize the hash output
	
	while (len >= step_size)
	{
		// cast data back to int
		// will be useful later, when we have inputs of any length
		h += *(unsigned char *)data;
		h *= m;
		// h ^= h >> num_to_shift_by;
		
		data += step_size;
		len -= step_size;
	}
	
	// make the hash hard on the neural net
	switch (len)
	{
		case 2:
		h += data[1];
		case 1:
			h += data[0];
			h *= m;
	}
	
	return h;
	
}

unsigned int prestons_8bit_sha(unsigned int pre_image)
{
}
	
unsigned int add_one_hash(unsigned int out)
{
	return out + 1;
}

unsigned int mult_hash(unsigned int out)
{
	short first_half = out >> 8;
	short second_half = out & 0x00ffu;
	
	return first_half * second_half;
}

unsigned int x_squared(unsigned int pre_image)
{
	return pre_image * pre_image;
}

unsigned int lookup_table(unsigned int pre_image)
{
	int lookup_table[] = {
		0x98, 0x45, 0x54, 0x10, 0x02, 0x05, 0x08, 0x12,
		0x45, 0x78, 0x88, 0x11, 0x44, 0x55, 0x99, 0x79,
		0x4a, 0x7a, 0x8a, 0x1a, 0x4a, 0x5a, 0x9a, 0x7a,
		0xa5, 0xa8, 0xa8, 0xa1, 0xa4, 0xa5, 0xa9, 0xa9,
		0x4b, 0x7b, 0x8b, 0x1b, 0x4b, 0x5b, 0x9b, 0x7b,
		0xb5, 0xb8, 0xb8, 0xb1, 0xb4, 0xb5, 0xb9, 0xb9,
		0x4c, 0x7c, 0x8c, 0x1c, 0x4c, 0x5c, 0x9c, 0x7c,
		0xc5, 0xc8, 0xc8, 0xc1, 0xc4, 0xc5, 0xc9, 0xc9,
		0x4d, 0x7d, 0x8d, 0x1d, 0x4d, 0x5d, 0x9d, 0x7d,
		0xd5, 0xd8, 0xd8, 0xd1, 0xd4, 0xd5, 0xd9, 0xd9,
		0x4e, 0x7e, 0x8e, 0x1e, 0x4e, 0x5e, 0x9e, 0x7e,
		0xe5, 0xe8, 0xe8, 0xe1, 0xe4, 0xe5, 0xe9, 0xe9,
		0x4f, 0x7f, 0x8f, 0x1f, 0x4f, 0x5f, 0x9f, 0x7f,
		0xf5, 0xf8, 0xf8, 0xf1, 0xf4, 0xf5, 0xf9, 0xf9,
		0x41, 0x71, 0x83, 0x13, 0x43, 0x53, 0x93, 0x73,
		0x15, 0x18, 0x38, 0x31, 0x34, 0x35, 0x39, 0x39
	};
	
	short lookup_index = pre_image >> 9;  // get the left most 7 (of a 16 bit pre image)
	
	lookup_index = lookup_table[lookup_index];
	
	return lookup_index && (pre_image << 7);
}

unsigned int testing_ground_hash(unsigned int out)
{
	bool zero = ((out & 0x80) > 0) && ((out & 0x40) > 0) && ((out & 0x20) > 0) && ((out & 0x10) > 0) && ((out & 0x08) > 0) && ((out & 0x04) > 0) && ((out & 0x04) > 0) && ((out & 0x02) > 0) && ((out & 0x01) > 0);
	bool one = ((out & 0x80) > 0) || ((out & 0x40) > 0) || ((out & 0x20) > 0) || ((out & 0x10) > 0) || ((out & 0x08) > 0) || ((out & 0x04) > 0) || ((out & 0x04) > 0) || ((out & 0x02) > 0) || ((out & 0x01) > 0);
	bool two = !zero;
	bool three = !one;
	bool four = zero;
	bool five = one;
	bool six = two;
	bool seven = three;

	int result = 0;
	if (zero)
		result |= 0x80;
	if (one)
		result |= 0x40;
	if (two)
		result |= 0x20;
	if (three)
		result |= 0x10;
	if (four)
		result |= 0x08;
	if (five)
		result |= 0x04;
	if (six)
		result |= 0x02;
	if (seven)
		result |= 0x01;

	return result;
}