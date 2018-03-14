/* ************************************************************************
 *       Filename:  vector_stru.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  2016年11月19日 15时58分59秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/


#include <iostream>
#include <string.h>
#include <stdio.h>
#include <vector>
using namespace std;


typedef struct _BOARD_CONFIG_
{
	char board_num[100];
	int dst_addr;
}board_config_stru;


void fun(int a, vector<board_config_stru> & con = vector<board_config_stru>())
{
	if(a = 1)
	{	
		board_config_stru bconfig;
		strcpy(bconfig.board_num, "123");
		bconfig.dst_addr = 99;
		con.push_back(bconfig);

		printf("%s, %d, %ld\n", con[0].board_num, con[0].dst_addr, con.size());
	}
}

void fun1(vector<board_config_stru> con)
{	
	printf("1111%s, %d, %ld\n", con[0].board_num, con[0].dst_addr, con.size());
}
int main(int argc, char *argv[])
{
	vector<board_config_stru> con;
	fun(1, con);
	fun1(con);
	printf("%s, %d\n", con[0].board_num, con[0].dst_addr);
	return 0;
}


