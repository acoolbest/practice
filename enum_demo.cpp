/* ************************************************************************
 *       Filename:  enum_demo.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  2016年11月19日 13时35分39秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/


#include <stdio.h>
#include <stdint.h>

enum com_cmd_type
{
	power_on = 0x5A,
	power_off = 0x59,
	file_called = 0x13,
	handshake = 0x16
};


int main(int argc, char *argv[])
{
	uint8_t x = (uint8_t)power_on;
	printf("%x\n", x);
//	printf("%x, %x, %x, %x\n", power_on, power_off, file_called, handshake);
	return 0;
}


