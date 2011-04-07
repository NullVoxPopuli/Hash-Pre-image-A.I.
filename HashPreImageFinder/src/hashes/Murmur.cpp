#include <iostream>
#include <sstream>

#include "Murmur.h"

using namespace std;

//http://tanjent.livejournal.com/756623.html
unsigned int Murmur::hash ( unsigned int output_binary, int len, unsigned int h )
{
	const unsigned char * data;
	ostringstream num;
	num << output_binary;
	data = (unsigned const char * )string(num.str()).c_str();
		
	const unsigned int m = 0x7fd652ad;
	const int r = 16;

	h = Murmur::add_constant(h, 0xdeadbeef);

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


unsigned int Murmur::add_constant(unsigned int original, unsigned int constant)
{
	return original + constant;
}