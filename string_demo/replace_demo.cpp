#include <string>
#include <iostream>
//g++ replace_demo.cpp -std=c++11

int main()
{
	std::string a {"abc 111222333"};

	a.replace(0,3, "hehehe");

	std::cout << a << "\n";

	return 0;
}
