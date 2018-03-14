/**
g++ conversion_U8_from_S32_demo.cpp -ansi -pedantic -W -Wall -Wextra -Wconversion -fPIC -fstrict-aliasing -Wstrict-aliasing=2 -Wno-long-long -g -O2
**/
#include <iostream>
#include <vector>
#include <stdio.h>

int main()
{
	std::vector<unsigned char> data;
	data.push_back('A');
	short arrayPos = 0;
	unsigned char bytePos = 3;
	data[arrayPos] |= (unsigned char)( 0x01 << bytePos );
	printf("%c, %d, %d, %d\n", data[arrayPos], (unsigned char)(0x01 << bytePos), bytePos, bytePos);
	//std::cout << data[arrayPos] << (0x01 << bytePos) << std::endl;
	return 0;
}
