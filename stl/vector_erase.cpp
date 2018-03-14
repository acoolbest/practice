/* ************************************************************************
 *       Filename:  vector_erase.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  2017年05月24日 12时57分40秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/


#include <iostream>
#include <vector>
#include <stdint.h>
#include <unistd.h>

using namespace std;

#define CRG_FOUND				0x01
#define CRG_NOT_FOUND			0x02
#define CRG_INVALID_PARAM		0x03

vector<string> vec_file_name;

uint32_t is_exist_file(const char* file_name)
{
	if (file_name == NULL)
		return CRG_INVALID_PARAM;
	return access(file_name, 4) == -1 ? CRG_NOT_FOUND : CRG_FOUND;
}

uint32_t merge_all_exist_file(vector<string> &vec)
{
	uint32_t ret = vec.size();
	vector<string>::iterator itor;
	for (itor = vec.begin(); itor != vec.end();)
	{
		if (is_exist_file(itor->c_str()) == CRG_NOT_FOUND)
		{
			vec.erase(itor);
			//vector<string>(vec).swap(vec);
		}
		else
			itor++;
	}
	vector<string>(vec).swap(vec);
	return ret;
}

int main(int argc, char *argv[])
{
	vec_file_name.clear();
	vector<string>(vec_file_name).swap(vec_file_name);
	vec_file_name.push_back("./map_mq/00.bmp");
	vec_file_name.push_back("./map_mq/a.out");
	vec_file_name.push_back("./map_mq/02.bmp");
	vec_file_name.push_back("./map_mq/tags");
	vec_file_name.push_back("./map_mq/04.bmp");
	merge_all_exist_file(vec_file_name);
	cout << "success" << endl;
	return 0;
}
