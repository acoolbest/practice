/**
https://blog.csdn.net/qq_31828515/article/details/52843073
https://blog.csdn.net/qq_36782456/article/details/70954050

  详解C++中String类模拟实现以及深拷贝浅拷贝

  在C语言中/C++中，字符串是一个应用很广泛的类型，也是很基础的类型，C语言并没有直接处理字符串的操作而是采用字符指针和字符串数组进行操作，而在C++中标准库为我们封装了一个字符串的类供我们使用，使用需要#inlcude <string>头文件。我们也可以自己模拟实现一个简单的String类。
  在模拟实现String类的过程中，不可避免的会遇到深拷贝浅拷贝的问题，下面就深拷贝浅拷贝做一个简介。所谓深拷贝浅拷贝，简单来说就是浅拷贝只是简单的将值拷贝过来，用一个对象初始化另一个对象，只复制了成员，并没有复制资源，使两个对象同时指向了同一资源的。而深拷贝则是将资源和值一块拷贝过来，此时两个对象各自占用资源，尽管值相同，但是互不影响。
  下面通过代码进行对比：
 **/
#include <iostream>
#include <algorithm>
using namespace std;


#if 0
//浅拷贝 
class String { 
	public: 
		String(const char* s = "") 
		{ 
			if (NULL == s) { 
				_pStr = new char[1]; 
				*_pStr = '\0'; 
			} 
			else { 
				_pStr = new char[[=](int i){for(; *s; i++, s++); return i;} + 1]; 
				strcpy(_pStr, s); 
			} 
		} 

		String(const String& s):_pStr(s._pStr) { }

		String& operator=(const String& s) 
		{ 
			if (this != &s) { 
				delete[] _pStr;
				_pStr = s._pStr; 
			} 
			return *this; 
		}

		~String() 
		{ 
			if (NULL != _pStr) { 
				delete[] _pStr; 
				_pStr = NULL; 
			} 
		} 
		friend ostream&operator<<(ostream & _cout,const String &s)
		{
			_cout<<s._pStr;
			return _cout;
		}
	private: 
		char* _pStr; 
};
#endif

#if 1
//深拷贝 
class String { 
	public: 
		String(const char* s = "") 
		{
			if (NULL == s) { 
				_pStr = new char[1]; 
				*_pStr = '\0'; 
			} 
			else { 
				char *src = const_cast<char*>(s);
				char *temp = _pStr = new char[ ([=](){char *p = src; while(*p++); return p-src;})() + 1];
				while((*temp++ = *src++));

			} 
		}
		#if 0
		String(const String& s)
		{
			char *src = const_cast<char*>(s._pStr);
			char *temp = _pStr = new char[ ([=](){char *p = src; while(*p++); return p-src;})() + 1];
			while((*temp++ = *src++));
		}
		String& operator=(const String& s) 
		{
			//先申请空间将s的内容拷贝到一个临时变量再去释放原有的空间 
			//防止申请空间失败连原有的空间都没了
			if (this != &s) { 
			
				char *src = const_cast<char*>(s._pStr);
				char *temp = new char[ ([=](){char *p = src; while(*p++); return p-src;})() + 1];
				char *temp1 = temp;
				while((*temp1++ = *src++));
				delete[] _pStr;
				_pStr = temp;
			} 
			return *this; 
		}
		#else
		//现代写法： 根据拷贝构造函数让系统自己开辟空间 
		//拷贝构造函数
		String (const String& s):_pStr(nullptr)//必须置为空，要不然访问地址非法化
		{
			String temp(s._pStr);
			std::swap(temp._pStr,_pStr);
		}
		//赋值运算符重载
		#if 0
		String& operator=(const String& s)
		{
			if (this != &s) {
				String temp(s);//先调用拷贝构造函数，继而调用构造函数，当退出此函数时，销毁拷贝构造中的数据，避免内存泄漏
				std::swap(_pStr, temp._pStr);
			}
			return *this;
		}
		#endif
		#if 1
		String& operator=(const String& s)
		{
			if (this != &s) {
				String temp(s._pStr);//直接调用构造函数，当退出此函数时，销毁拷贝构造中的数据，避免内存泄漏
				std::swap(_pStr, temp._pStr);
			}
			return *this;
		}
		#endif
		#if 0
		String& operator=(String& temp) // 此方法会直接互换，而不是复制，数据源会被污染
		{
			std::swap(_pStr, temp._pStr);
			return *this;
		}
		#endif
		
		#endif
		~String() 
		{
			if (NULL != _pStr) { 
				addr();
				delete[] _pStr; 
				_pStr = NULL; 
			} 
		} 
		friend ostream&operator<<(ostream & _cout,const String &s)
		{
			_cout<<s._pStr;
			return _cout;
		}
		
		void addr() {cout << &_pStr << endl;}
	private: 
		char* _pStr; 
}; 
#endif
int main()
{
	String s1("sss");
	String s2(s1);
	String s3("hehe");
	s3 = s1;
	cout<<"s1: " << s1 <<endl;
	cout<<"s2: " << s2 <<endl;
	cout<<"s3: " << s3 <<endl;
	return 0;
}