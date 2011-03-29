#include <iostream>
#include <sstream>

#include "hashes.h"

using namespace std;

//http://tanjent.livejournal.com/756623.html
unsigned int MurmurHash ( unsigned int output_binary, int len, unsigned int h )
{
	const unsigned char * data;
	ostringstream num;
	num << output_binary;
	data = (unsigned const char * )string(num.str()).c_str();
		
	const unsigned int m = 0x7fd652ad;
	const int r = 16;

	h += 0xdeadbeef;

	while(len >= 4)
	{
		h += *(unsigned int *)data;
		h *= m;
		h ^= h >> r;

		data += 4;
		len -= 4;
	}

	switch(len)
	{
	case 3:
		h += data[2] << 16;
	case 2:
		h += data[1] << 8;
	case 1:
		h += data[0];
		h *= m;
		h ^= h >> r;
	};

	h *= m;
	h ^= h >> 10;
	h *= m;
	h ^= h >> 17;

	return h;
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
