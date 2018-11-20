#include <iostream>

int main()
{
	int a[10] = {0};

	int * p = a;
	
	*p++ = 1;

	std::cout << a[0] << std::endl;
	std::cout << a[1] << std::endl;

	return 0;
}
