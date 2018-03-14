/* ************************************************************************
 *       Filename:  arr_demo.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  2017年05月19日 17时11分43秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/


#include <iostream>
#include <string.h>

using namespace std;

void print_strlen(char * arr)
{
	cout << strlen(arr) << endl;
}

int main(int argc, char *argv[])
{
	char a[10] = "123";
	print_strlen(a);
	return 0;
}
