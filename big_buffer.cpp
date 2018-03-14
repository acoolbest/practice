/* ************************************************************************
 *       Filename:  big_buffer.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  2017年08月04日 14时58分02秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/

/**
可设置栈为无限制，Linux申请局部变量就可超越默认8192KB的限制
ulimit -a // 查看所有的限制项
ulimit -s unlimited // 设置栈为无限制
对于堆空间来说，默认是没有软限制的，只依赖硬限制
**/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	char buffer0[8*1024*1024] = "hello!";
	char *p = (char *)malloc(10*1024*1024*1024);
	printf("%s\n", buffer0);
	sleep(20);
	free(p);
	return 0;
}


