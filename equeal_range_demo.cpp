#include <algorithm>
#include <vector>
#include <iostream>
 
struct S
{
    int number;
    char name;
    // 注意：此比较运算符忽略 name
    bool operator< ( const S& s ) const { 
		if(number < s.number)
			std::cout << "S : true" << number<< " "  << name << s.number<< s.name << '\n';
		else
			std::cout << "S : false" << number<< " " << name << s.number  << s.name << '\n';
		return number < s.number; 
	}
};
 
int main()
{
    // 注意：非有序，仅相对于定义于下的 S 划分
    std::vector<S> vec = { {1,'A'}, {2,'B'}, {2,'C'}, {2,'D'}, {4,'G'}, {3,'F'} };
 
    S value = {2, '?'};
 
    auto p = std::equal_range(vec.begin(), vec.end(), value);
 
    for ( auto i = p.first; i != p.second; ++i )
        std::cout << i->name << ' ';
	std::cout << "\n";

    // 异相比较:
    struct Comp
    {
		#if 1
        bool operator() ( const S& s, int i ) const {
			if(s.number < i)
				std::cout << "Comp S : true" << s.number<< " " << i <<'\n';
			else
				std::cout << "Comp S : false" << s.number<< " " << i <<'\n';
			return s.number < i; 
		}
		//#else
        bool operator() ( int i, const S& s ) const {
			if(i < s.number)
				std::cout << "Comp i : true" << i << " " << s.number <<'\n';
			else
				std::cout << "Comp i : false" << i << " " << s.number <<'\n';
			return i < s.number; 
		}
		#endif
    };
 
    auto p2 = std::equal_range(vec.begin(),vec.end(), 2, Comp{});
 
    for ( auto i = p2.first; i != p2.second; ++i )
        std::cout << i->name << ' ';
	std::cout << "\n";
}