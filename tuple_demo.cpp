#include <iostream>
#include <string>
#include <tuple>

using namespace std;

int main()
{
	auto t = make_tuple(string("123"), 10, 1.23);

	string s = get<0>(t);
	int x = get<1>(t);
	double d = get<2>(t);

	cout << s << " "<< x << " " << d <<"\n";

	return 0;
}
