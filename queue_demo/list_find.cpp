/* ************************************************************************
 *       Filename:  list_find.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  2017年08月29日 15时51分30秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/


#include <list>
#include <iostream>
#include <algorithm>
#include <stdio.h>

using namespace std;

class CPerson
{
	public:
		CPerson() { } 
		~CPerson() { }
		#if 1 // find
		bool operator == (const CPerson &rhs) const
		{
			return (age == rhs.age);
		}
		#endif
		#if 1 // find_if 
		bool operator ()(const CPerson *rhs)
		{
			if (rhs != NULL)
			{
				return age == rhs->age;
			}
			return false;	
		}
		#endif
	public:
		int age; // 年龄
		int index;
};

int main()
{
	list<CPerson *> lst;
	
	
	for(int j=10;j>0;j--)
	for(int i=0;i<2;i++)
	{
		CPerson *cp_to_find = new CPerson(); // 要查找的对象
		cp_to_find->age = 10*i;
		cp_to_find->index = j;
		lst.push_back(cp_to_find);
		printf("%d, %d, %p\n", cp_to_find->age, cp_to_find->index, cp_to_find);
	}
	
	CPerson cp_to_find1; // 要查找的对象
	cp_to_find1.age = 10;
	
	while(1)
	{
		list<CPerson *>::iterator it = find_if(lst.begin(), lst.end(), cp_to_find1);
		if (it != lst.end()) // 找到了
		{
		  // cout << (*it)->age << " " << (*it)->index << endl;
		   printf("~~~~~~~~~%d, %d, %p\n~~~~~~~~~~~~\n", (*it)->age, (*it)->index, *it);
		   cout << "found" << endl;
		   lst.erase(it);
		}
		else // 没找到
		{
			cout << "no found" << endl;
			break;
		}
	}
	
	
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	list<CPerson> lst1;
	
	
	for(int j=10;j>0;j--)
	for(int i=0;i<2;i++)
	{
		CPerson cp_to_find; // 要查找的对象
		cp_to_find.age = 10*i;
		cp_to_find.index = j;
		lst1.push_back(cp_to_find);
		printf("%d, %d\n", cp_to_find.age, cp_to_find.index);
	}
	
	CPerson cp_to_find2; // 要查找的对象
	cp_to_find2.age = 10;
	
	while(1)
	{
		list<CPerson>::iterator it = find(lst1.begin(), lst1.end(), cp_to_find2);
		if (it != lst1.end()) // 找到了
		{
		  // cout << (*it)->age << " " << (*it)->index << endl;
		   printf("~~~~~~~~~%d, %d\n~~~~~~~~~~~~\n", it->age, it->index);
		   cout << "found" << endl;
		   lst1.erase(it);
		}
		else // 没找到
		{
			cout << "no found" << endl;
			break;
		}
	}
	
	#if 0
    list<int> lst;
    lst.push_back(10);
    lst.push_back(20);
    lst.push_back(30);
    list<int>::iterator it = find(lst.begin(), lst.end(), 10); // 查找list中是否有元素“10”
	#endif
    
	return 0;
}


