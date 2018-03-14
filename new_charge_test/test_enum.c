/* ************************************************************************
 *       Filename:  test_enum.c
 *    Description:  
 *        Version:  1.0
 *        Created:  2016年11月09日 20时31分41秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/



#include <stdio.h>
enum operation_type{
	write_request = 1
};
int main(int argc, char *argv[])
{

	printf("%d\n", write_request);
	return 0;
}


