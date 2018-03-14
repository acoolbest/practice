#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
using namespace std;

#define			CRG_FAIL							0x0000
#define			CRG_SUCCESS							0x0001
#define			CRG_INVALID_PARAM					0x0002
#define			CRG_FOUND							0x0003
#define			CRG_NOT_FOUND						0x0004

uint32_t is_exist_file(const char* file_name)
{
	if (file_name == NULL)
		return CRG_INVALID_PARAM;
	return access(file_name, 4) == -1 ? CRG_NOT_FOUND : CRG_FOUND;
}

uint32_t read_file(const char * file_name, string & str_content)
{
	if (file_name == NULL || strlen(file_name) == 0)
		return CRG_INVALID_PARAM;

	FILE *fp = NULL;
	long file_len = 0;
		
	fp = fopen(file_name, "rb");
	if (fp == NULL)
		return CRG_FAIL;

	fseek(fp, 0, SEEK_END);
	file_len = ftell(fp);

	fseek(fp, 0, SEEK_SET);
	str_content.resize(file_len + 1);
	fread((void*)str_content.c_str(), 1, file_len, fp);
	str_content += '\0';
	fclose(fp);

	return CRG_SUCCESS;
}

uint32_t get_system_version(string & system_version)
{
	string sysversion_path = "/usr/sbin/sysversion";
	string keyword = "version: ";
	int ret = is_exist_file(sysversion_path.c_str());
	if(ret == CRG_FOUND)
	{
		ret = read_file(sysversion_path.c_str(), system_version);
		size_t found = system_version.rfind(keyword);
		
		if(found != std::string::npos && (found + keyword.length()) < system_version.length())
		{
			system_version = system_version.substr(found+keyword.length());
			found = system_version.rfind("\n");
			if(found != std::string::npos)
				system_version = system_version.substr(0, found);
		}	
		else
			ret = CRG_FAIL;
	}
	if(ret != CRG_SUCCESS)
	{
		system_version = "NULL";
		printf("get system version ret[%d] failed\n", ret);
	}
	return ret;
}
int main()
{
	string system_version = "";
	get_system_version(system_version);
	cout << system_version << endl;
	return 0;
}
