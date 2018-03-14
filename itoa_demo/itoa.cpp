/* ************************************************************************
 *       Filename:  itoa.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  2017年04月24日 17时06分25秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/


#include <iostream>
#include <string>
#include <sstream>

using namespace std;

int main(int argc, char *argv[])
{
	int i = 123456;

	ostringstream ostr;

	ostr << i;

	string str(ostr.str());

	cout << str << endl;
	return 0;
}
