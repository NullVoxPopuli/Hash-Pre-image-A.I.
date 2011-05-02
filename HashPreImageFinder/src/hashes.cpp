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