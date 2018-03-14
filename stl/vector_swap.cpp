/* ************************************************************************
 *       Filename:  vector_swap.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  2017年05月03日 17时56分44秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/


#include <iostream>
#include <vector>

using namespace std;

void print_vector(vector<int> vec)
{
	vector<int> :: iterator i = vec.begin();
	while(i != vec.end())
	{
		cout << *i++ << endl;
	}
}

int main(int argc, char *argv[])
{
	vector<int> a(2,3), b;
	
//	print_vector(a);
//	cout << "~~~~~~~~~~~~~~~~~" << endl;
//	print_vector(b);
	
	cout << "@@@@@@@@@@@@@@@@@" << endl;
	
	vector<int>(a).swap(b);

	print_vector(a);
	cout << "~~~~~~~~~~~~~~~~~" << endl;
	print_vector(b);

	cout << "@@@@@@@@@@@@@@@@@" << endl;
	vector<int> c;
	c.push_back(1);
	c.push_back(2);

	cout << c.back() << endl;
	cout << *(c.end()-1) << endl;


	return 0;
}
