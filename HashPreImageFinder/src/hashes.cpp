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
