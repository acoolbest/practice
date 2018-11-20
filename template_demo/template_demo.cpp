#include <iostream>
#include <vector>
#include <list>
#include <string>

using std::cout;
using std::vector;
using std::list;
using std::string;

//在容器c中查找v出现的所有位置
template<typename T>
using Iter = typename T::iterator;

template<typename C, typename V>
//vector<C::iterator> find_all(C & c, V v)
vector<Iter<C>> find_all(C & c, V v)
{
	vector<Iter<C>> res;
	//vector<C::iterator> res;
	for(auto p = c.begin();p!=c.end();++p)
		if(*p == v)
			res.push_back(p);
	return res;
}


int main()
{
	string a {"abcdeabeeda"};
	cout << a << "\n";
	auto v = find_all(a, 'a');
	for(auto & x : v)
	{		
		if(*x != 'a')
			cout << *x << "!=" << x-a.begin() << "\n";
		else
			cout << *x << "==" << x-a.begin() << "\n";

	}

	list<double> ld {1.1,2.2,3.3,1.1};

	for(auto & x : find_all(ld,1.1))
	{		
		if(*x != 1.1)
			cout << *x << "!=\n";
		else
			cout << *x << "==\n";

	}

}
