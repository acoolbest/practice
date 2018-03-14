/* ************************************************************************
 *       Filename:  non_mutating_algorithm.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  2017年03月28日 16时47分52秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/


#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;


int main(int argc, char *argv[])
{
	int a[10] = {12,31,5,2,23,121,0,89,34,66};

	vector<int> v1(a, a+10);
	vector<int>::iterator result1, result2;

	result1 = find(v1.begin(),v1.end(),2);
	
	result2 = find(v1.begin(),v1.end(),8);
	
	cout << result1 - v1.begin() << endl;

	cout << result2 - v1.begin() << endl;

	int b[9] = {5,2,23,54,5,5,5,2,2};

	vector<int> v2(a+2,a+8);
	vector<int> v3(b,b+4);
	
	result1 = search(v1.begin(),v1.end(),v2.begin(),v2.end());

	cout << *result1 << endl;
	
	result1 = search(v1.begin(),v1.end(),v3.begin(),v3.end());

	cout << *(result1-1) << endl;

	vector<int> v4(b,b+9);

	int i=count(v4.begin(),v4.end(),5);
	int j=count(v4.begin(),v4.end(),2);

	cout << i << " , " << j << endl;

	return 0;
}
