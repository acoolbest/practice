/* ************************************************************************
 *       Filename:  sort_algorithm.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  2017年03月28日 17时28分34秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/


#include <iostream>
#include <algorithm>

using namespace std;


int main(int argc, char *argv[])
{
	int a[10] = {12,0,5,3,6,8,9,34,32,18};
	int b[5] = {5,3,6,8,9};
	int d[15];

//	sort(a,a+10);

	for(int i=0;i<10;i++)
		cout << a[i] << " ";
	cout << endl;

//	sort(b,b+5);

	if(includes(a,a+10,b,b+5))
		cout << "sorted b members are included in a." << endl;
	else
		cout << "sorted a does not contain sorted b!" << endl;

	merge(a,a+10,b,b+5,d);

	for(int j=0;j<15;j++)
		cout << d[j] << " ";
	cout << endl;

	return 0;
}


