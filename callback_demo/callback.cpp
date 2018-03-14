
#if 0
/*
   回调函数说白了就是：我请你帮我算一道题，你帮我解决问题的时候需要计算器，于是你借我的计算器来进行算术计算，那么我的这个计算器就是回调函数
   所谓回调，就是模块A要通过模块B的某个函数b()完成一定的功能，但是函数b()自己无法实现全部功能，需要反过头来调用模块A中的某个函数a()来完成，这个a()就是回调函数
   说明：回调函数指针的定义在模块B中定义，注册函数也在B模块中实现且由A模块来调用。
   */
//#include "B.h"
#include <iostream>
#include <stdlib.h>
using namespace std;
#if 1
//（2）模块B的文件：

//B.h头文件内容：

typedef int(*CallBackFun)(int, int);

extern int regCB(CallBackFun, int);

extern int calculate(int, int, int);


//B.cpp文件内容：

//#include "B.h"

CallBackFun funList[4] = {0};  //回调函数数组
#endif

enum OPT{
ADD = 0,
RED, 
MUL, 
DIV}; //加，减，乘，除

/*四个回调函数定义*/
int add(int a, int b) {return a + b;}

int reduce(int a, int b) {return a - b;}

int multiply(int a, int b) {return a * b;}

int divide(int a, int b)
{
	if(b != 0)
		return a / b;
	else
		exit(-1);
}

int regCB(CallBackFun cb, int type)  //注册回调函数
{
	if(type < 0 || type > 3) 
		return -1;
	funList[type] = cb;  //将对应的函数指针保存到数组中
	return 0;
}

int calculate(int a, int b, int type)  //计算
{
	return (*funList[type])(a, b);
}

int main()
{
	int err;
	for(int i = ADD; i < 4; i++)
	{
		switch(i)
		{
			case ADD:
				if((err = regCB(add, i)) < 0)  //注册回调函数
				{
					cout << "reg call back funtion error!" << endl;
					exit(-1);
				}
				break;
			case RED:
				if((err = regCB(reduce, i)) < 0)
				{
					cout << "reg call back funtion error!" << endl;
					exit(-1);
				}
				break;
			case MUL:
				if((err = regCB(multiply, i)) < 0)
				{
					cout << "reg call back funtion error!" << endl;
					exit(-1);
				}
				break;
			case DIV:
				if((err = regCB(divide, i)) < 0)
				{
					cout << "reg call back funtion error!" << endl;
					exit(-1);
				}
				break;
			default:
				break;
		}
	}

	int a = 10, b = 5;
	cout << calculate(a, b, MUL) << endl; //调用B模块的calculate函数来实现运算
}
//流程就是：A调用B的calculate函数进行运算，然后B根据运算类型调用A相应的回调函数计算。
#endif

