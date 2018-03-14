#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
using namespace	std;

int main()
{
	string file_name = "./picture/area1/123.bmp";
	/*
	size_t found = file_name.rfind(".bmp");
	if (found!=string::npos)
		file_name.replace(found, 4, "");
	*/
	cout << (uint8_t)atoi((file_name.substr(strlen("./picture/area1/"))).c_str()) << endl;
	cout << file_name << endl;
	cout << file_name.size() << endl;
	cout << file_name.length() << endl;
	return 0;
}