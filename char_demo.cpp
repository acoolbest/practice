#include <iostream>

using namespace std;

int main()
{
	signed char sc = -160;
	
	unsigned char uc = sc;

	cout << static_cast<int>(uc) << "\n";

	cout << sizeof(char) << "\n";

	char v1[] = "a\xah\129";
	char v2[] = "a\xah\127";
	char v3[] = "a\xad\127";
	char v4[] = "a\xad\0127";


	return 0;
}
