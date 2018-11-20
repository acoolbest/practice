#include <iostream>
#include <regex>
using namespace std;

int main()
{
	regex pat(R"(\w{2}\s*\d{5}(-\d{4})?)");
	cout << "pattern: " << pat << "\n"
	return 0;
}
