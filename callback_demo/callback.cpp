
#if 0
/*
   �ص�����˵���˾��ǣ������������һ���⣬����ҽ�������ʱ����Ҫ����������������ҵļ������������������㣬��ô�ҵ�������������ǻص�����
   ��ν�ص�������ģ��AҪͨ��ģ��B��ĳ������b()���һ���Ĺ��ܣ����Ǻ���b()�Լ��޷�ʵ��ȫ�����ܣ���Ҫ����ͷ������ģ��A�е�ĳ������a()����ɣ����a()���ǻص�����
   ˵�����ص�����ָ��Ķ�����ģ��B�ж��壬ע�ắ��Ҳ��Bģ����ʵ������Aģ�������á�
   */
//#include "B.h"
#include <iostream>
#include <stdlib.h>
using namespace std;
#if 1
//��2��ģ��B���ļ���

//B.hͷ�ļ����ݣ�

typedef int(*CallBackFun)(int, int);

extern int regCB(CallBackFun, int);

extern int calculate(int, int, int);


//B.cpp�ļ����ݣ�

//#include "B.h"

CallBackFun funList[4] = {0};  //�ص���������
#endif

enum OPT{
ADD = 0,
RED, 
MUL, 
DIV}; //�ӣ������ˣ���

/*�ĸ��ص���������*/
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

int regCB(CallBackFun cb, int type)  //ע��ص�����
{
	if(type < 0 || type > 3) 
		return -1;
	funList[type] = cb;  //����Ӧ�ĺ���ָ�뱣�浽������
	return 0;
}

int calculate(int a, int b, int type)  //����
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
				if((err = regCB(add, i)) < 0)  //ע��ص�����
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
	cout << calculate(a, b, MUL) << endl; //����Bģ���calculate������ʵ������
}
//���̾��ǣ�A����B��calculate�����������㣬Ȼ��B�����������͵���A��Ӧ�Ļص��������㡣
#endif

