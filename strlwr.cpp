/* ************************************************************************
 *       Filename:  strlwr.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  2016年12月01日 14时09分39秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/


#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <iostream>
using namespace std;
void strlwr(char * str, string & out_str)
{
	char buf[100];
	for(size_t i=0; i< strlen(str); i++)
	{
		buf[i] = tolower(str[i]);
	}
	out_str = string(buf);
}

int main(int argc, char *argv[])
{
	string out_str;
	while(1)
	{
		usleep(100*1000);
		strlwr((char *)"ABcdEF", out_str);
		cout <<  out_str  << endl;
	}
	return 0;
}

