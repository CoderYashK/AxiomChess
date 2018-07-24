

#include <stdio.h>
#include "defs.h"

const int BitTable[64] = {
	63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,
	51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
	26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
	58, 20, 37, 17, 36, 8//All the 64 bits stored in an array which hold the bits represented by ULL64.
};

int PopBit(U64 *bb) {//Function to pop a bit that is to make a bit 'O'.
	U64 b = *bb ^ (*bb - 1);//Standard code taken from the internet.
	unsigned int fold = (unsigned) ((b & 0xffffffff) ^ (b >> 32));
	*bb &= (*bb - 1);
	return BitTable[((U64)(fold * 0x783a9b23)) >> 26];
}

int CountBits(U64 b) {//Function to count the no. of bits having sign 1.
	int r;
	for(r = 0; b; r++, b &= b - 1);
	return r;
}
