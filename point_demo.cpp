/* ************************************************************************
 *       Filename:  point_demo.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  2017年09月04日 09时25分20秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/


#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdint.h>

void fun(uint16_t * p)
{
	(*p)++;	
	printf("*p is %c\n", *p);
}

void fun2(uint8_t * p)
{
	int len = strlen((const char *)p);
	while(len--) fun((uint16_t *)p++);
}
int main(int argc, char *argv[])
{
//	int * p = NULL;

//	printf("point size is %ld, int size is %ld\n", sizeof(p), sizeof(int));
	
	uint8_t a[] = "abcdefg";
//	char *p = a;
//	int len = strlen(a);

//	while(len--)
//	{
//		fun2(p++);
//	}
	fun2(a);
	printf("%s\n", a);
	return 0;
}


