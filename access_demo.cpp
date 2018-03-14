/* ************************************************************************
 *       Filename:  access_demo.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  2016年11月18日 18时21分40秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/


#include <unistd.h>
#include <stdio.h>
int main(int argc, char *argv[])
{
	printf("%d\n", access("/abc", 4));
	return 0;
}


