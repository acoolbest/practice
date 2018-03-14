/* ************************************************************************
 *       Filename:  test_for.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  2016年11月18日 16时06分43秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/


#include <stdio.h>
int main(int argc, char *argv[])
{
	for(int i=0,j=0;i<5;i++)
	{
		if(i%2 == 0)
		{
			j++;
		}
		printf("%d %d\n", i,j);
	}
	return 0;
}


