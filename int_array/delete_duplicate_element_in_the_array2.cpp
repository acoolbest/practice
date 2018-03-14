/**
题目：把一个整形数组中重复的数字去掉

例如： 
1,   2,   0,   2,   -1,   999,   3,   999,   88 
答案应该是： 
1,   2，   0，   -1，   999，   3，   88

思路：一次遍历数组找出绝对值最大的数，以便确定哈希表的大小。
第二次遍历数组，当遇到第一次出现的数，则放入应该放的位置（仍在原数组中），如果重复则不放入。

ubuntu16.04中测试正确代码如下：
 **/ 

#include<iostream>
#include <stdlib.h>
#define INT_MIN     (-2147483647 - 1) /* minimum (signed) int value , (2147483647 == 0x7FFFFFF) */
using namespace std;

typedef struct Info
{
	Info():isPositive(0),isNegative(0){}
	bool isPositive;
	bool isNegative;
}HashTable;

int main()
{
	int a[] = {1,2,0,2,-1,999,3,999,88,-1,2,2,2,-1,4,4,44,444,44,444,4444};
	int max = INT_MIN;//#define INT_MIN     (-2147483647 - 1) /* minimum (signed) int value */

	for(int i=0;i<sizeof(a)/sizeof(int);++i)
	{
		cout<<a[i]<<" ";
	}
	cout<<endl;

	for(int i=0;i<sizeof(a)/sizeof(int);++i)
	{
		if(abs(a[i])>max)
			max = abs(a[i]);
	}

	HashTable* hash = new HashTable[max+1];

	int toReplace=0;
	for(int current=0;current<sizeof(a)/sizeof(int);++current)
	{
		if(hash[abs(a[current])].isPositive==0 && hash[abs(a[current])].isNegative==0)
		{
			if(a[current]>0 || a[current] == 0)
				hash[abs(a[current])].isPositive=1;
			if(a[current]<0)
				hash[abs(a[current])].isNegative=1;

			a[toReplace] = a[current];
			toReplace++;
		}
		else
		{
			//确定重复
			if(hash[abs(a[current])].isPositive==1 && (a[current]>0 || a[current]==0))
				continue ;
			if(hash[abs(a[current])].isNegative==1 && a[current]<0)
				continue ;
			else
			{//不重复
				if(a[current]>0 || a[current] == 0)
					hash[abs(a[current])].isPositive=1;
				if(a[current]<0)
					hash[abs(a[current])].isNegative=1;

				a[toReplace] = a[current];
				toReplace++;
			}
		}
	}

	for(int i=0;i<toReplace;++i)
	{
		cout<<a[i]<<" ";
	}
	cout<<endl;

	return 0;
}
