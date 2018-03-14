#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h> 
#include <dlfcn.h>
#include <stdarg.h>
#include "def.h"
#include "public_function.h"



static string get_cur_work_folder()
{
	char sz_file_path[MAX_PATH] = {0};
	getcwd(sz_file_path, MAX_PATH);
	return string(sz_file_path);
}

static string get_parent_folder(string str_path)
{
	string str_parent_path = str_path;
	int index = str_parent_path.find_last_of("/");
	if (-1 != index)
	{
		if (index == static_cast<int>(str_parent_path.length() - 1))
		{
			str_parent_path = str_parent_path.substr(0, index);//去掉末尾斜杠
		}
		index = str_parent_path.find_last_of("/");
		
		return str_parent_path.substr(0, index);
	}
	return "";
}

static void get_cur_date(char* cur_date)
{
	time_t lTime;
	struct tm *ptmTime;

	time(&lTime);

	ptmTime = localtime(&lTime);

	strftime(cur_date, 16, "%Y%m%d", ptmTime);
}

static bool check_folder(const char* p_path_name)
{
	char  dir_name[MAX_PATH] = {0};
	strcpy(dir_name, p_path_name);
	int i = 0, len = strlen(dir_name);

	if (dir_name[len - 1] != '/')
	{
		strcat(dir_name, "/");
	}

	len = strlen(dir_name);

	for (i = 1; i < len; i++)
	{
		if (dir_name[i] == '/')
		{
			dir_name[i] = 0;
			if (access(dir_name, F_OK) != 0)
			{
				if (mkdir(dir_name, 0777) == -1)
				{
					write_log("mkdir %s failed", dir_name);
					return false;
				}
			}
			dir_name[i] = '/';
		}
	}

	return (access(p_path_name, F_OK) == 0) ? true : false;
}

static bool append_to_file(const char* file_name, const char* content, size_t file_length)
{
	FILE* fp = NULL;

	try
	{
		fp = fopen(file_name, "a+b");
		if (fp == NULL)
			return false;
		
		fwrite(content, 1, file_length, fp);
		fclose(fp);
	}
	catch (...)
	{
		fclose(fp);
		return false;
	}

	return true;
}

uint32_t get_share_module_path(char* p_module_path, uint32_t len)
{
	uint32_t ret = CRG_FAIL;
	Dl_info info = { 0 };
	if (p_module_path == NULL || len == 0)
		return CRG_INVALID_PARAM;

	if (dladdr((const void*)get_share_module_path, &info) != 0)
	{
		if (info.dli_fname != NULL && strlen(info.dli_fname) > 3)
		{
			if (memcmp(info.dli_fname + strlen(info.dli_fname) - 3, ".so", 3) == 0)
			{
				if (strlen(info.dli_fname) < len)
				{
					memcpy(p_module_path, info.dli_fname, strlen(info.dli_fname));
					ret = CRG_SUCCESS;
				}
				else
					ret = CRG_NOT_ENOUGH_BUFF;
			}
		}
	}
	return ret;
}

char *strlwr(char *str)
{
	char *orign = str;

	for (size_t i = 0; i < strlen(str); i++)
	{
		str[i] = tolower(str[i]);
	}
	return orign;
}

void clear_install_app_result_log_info(install_app_result_log_info* pinfo)
{
	if (pinfo != NULL)
	{
		pinfo->dw_result = 0;
		pinfo->dw_app_source = 0;
		memset(&pinfo->device_info, 0, sizeof(pinfo->device_info));
		pinfo->vec_installed_app_time.clear();
		vector<install_app_time_info>(pinfo->vec_installed_app_time).swap(pinfo->vec_installed_app_time);
	}
}

bool check_sum(char* p_frame, int8_t len, int8_t& sum)
{
	sum = 0;
	if (len < 4) return false;
	for (int i = 2; i < len - 2; i++) sum += p_frame[i];
	return sum == p_frame[len - 2];
}

void write_file(char *path, string js)
{
	FILE *fp = fopen(path, "w");
	if (fp){
		fputs(js.c_str(), fp);
		fclose(fp);
	}
}

uint32_t is_string_exist(map<string, string> src_map, string find_key, string& out_val)
{
	uint32_t ret = CRG_NOT_FOUND;
	map<string, string>::iterator iter;
	iter = src_map.find(find_key);
	if (iter != src_map.end())
	{
		out_val = iter->second;
		ret = CRG_FOUND;
	}
	return ret;
}

void clear_map(map<string, string> & src_map)
{
	for (map<string, string>::iterator i = src_map.begin(); i!=src_map.end();)  
	{  
		src_map.erase(i++);
	}
	map<string, string>(src_map).swap(src_map);
}

uint32_t insert_string_map(map<string, string>* out, string &key, string &val)
{
	if (out == NULL)
		return CRG_INVALID_PARAM;

	pair<map<string, string>::iterator, bool> insert_pair;

	insert_pair = out->insert(pair<string, string>(key, val));

	return insert_pair.second ? CRG_SUCCESS : CRG_FAIL;
}

string get_current_time(void)
{
	time_t timep;
	struct tm *p = NULL;
	char s[64] = {0};
	time(&timep);
	p = localtime(&timep);
	strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", p);
	return string(s);
}

void string_split(string s, string delim, vector<string>* ret)
{
	string str;
	size_t last = 0;
	size_t index = s.find(delim, last);
	while (index != string::npos)
	{
		str = s.substr(last, index - last);
		if (!str.empty())
		{
			ret->push_back(str);

		}
		last = index + delim.length();
		index = s.find(delim, last);
	}
	if (index - last > 0)
	{
		str = s.substr(last, index - last);
		if (!str.empty())
		{
			ret->push_back(str);

		}
	}
}

uint32_t find_string_from_vector(string find_str, vector<string>* vec_str, string::size_type src_index)
{
	if (vec_str == NULL)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_NOT_FOUND;
	for (string::size_type i = 0; i < vec_str->size(); i++)
	{
		if ((*vec_str)[i].length() > src_index)
		{
			string src = (*vec_str)[i].substr(src_index);
			if (strcasecmp(find_str.c_str(), src.c_str()) == 0)
			{
				ret = CRG_FOUND;
				break;
			}
		}
	}
	return ret;
}

void color_print(const char *fm, ...)
{
	int iSize = 0;

	char buff[MAXLEN_LOGRECORD] = { 0 };

	int i = 0;
	char cur_time[32] = { 0 };
	get_cur_time(cur_time, true);
	i = sprintf(buff, "%s\t", cur_time);
	int m_maxlen = MAXLEN_LOGRECORD - (i + 3);
	va_list args;
	va_start(args, fm);
	iSize = vsprintf(buff + i, fm, args);
	va_end(args);

	if (iSize > m_maxlen || iSize < 0)
	{
		memset(buff, 0, MAXLEN_LOGRECORD);
		iSize = sprintf(buff, "%s\t", cur_time);
		iSize += sprintf(buff + iSize, "-- * --\n");
	}
	else
	{
		iSize += i;
		if (iSize < MAXLEN_LOGRECORD - 2)
		{
			buff[iSize] = 13;
			buff[iSize+1] = 10;
			iSize+=2;
		}
	}
	printf("\033[31m%s\033[0m\n",buff);
}

void write_log(const char *fm, ...)
{
	int iSize = 0;

	char buff[MAXLEN_LOGRECORD] = { 0 };

	int i = 0;
	char cur_time[32] = { 0 };
	get_cur_time(cur_time, true);
	i = sprintf(buff, "%s\t", cur_time);
	int m_maxlen = MAXLEN_LOGRECORD - (i + 3);
	va_list args;
	va_start(args, fm);
	iSize = vsprintf(buff + i, fm, args);
	va_end(args);

	if (iSize > m_maxlen || iSize < 0)
	{
		memset(buff, 0, MAXLEN_LOGRECORD);
		iSize = sprintf(buff, "%s\t", cur_time);
		iSize += sprintf(buff + iSize, "-- * --\n");
	}
	else
	{
		iSize += i;
		if (iSize < MAXLEN_LOGRECORD - 2)
		{
			buff[iSize] = 13;
			buff[iSize+1] = 10;
			iSize+=2;
		}
	}

	char curDate[20] = { 0 };
	char fileName[300] = { 0 };
	get_cur_date(curDate);
	int  j = 0;
	j = sprintf(fileName, "%s/%s", get_parent_folder(get_cur_work_folder()).c_str(), "logs");
	check_folder(fileName);

	j += sprintf(fileName + j, "/%s_%s%s", "charge", curDate, ".log");

	append_to_file(fileName, buff, iSize);
	//cout << fileName << endl;

	return;
}

void get_cur_time(char* cur_time, bool btype)
{
	time_t lTime;
	struct tm *ptmTime;

	time(&lTime);
	ptmTime = localtime(&lTime);
	if (btype)
	{
		strftime(cur_time, 32, "%Y/%m/%d %H:%M:%S", ptmTime);
	}
	else
	{
		strftime(cur_time, 32, "%Y%m%d%H%M%S", ptmTime);
	}

}

uint32_t is_exist_file(const char* file_name)
{
	if (file_name == NULL)
		return CRG_INVALID_PARAM;
	return access(file_name, 4) == -1 ? CRG_NOT_FOUND : CRG_FOUND;
}

uint32_t is_exist_dir(const char* dir_name)
{
	if (dir_name == NULL)
		return CRG_INVALID_PARAM;
	return access(dir_name, 4) == -1 ? CRG_NOT_FOUND : CRG_FOUND;
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


uint32_t get_app_json_config_file_path(string & out_file_path)
{
	char sz_module_path[MAX_PATH] = { 0 };

	if (get_share_module_path(sz_module_path, MAX_PATH) != CRG_SUCCESS)
	{
		readlink("/proc/self/exe", sz_module_path, MAX_PATH);
	}

	if(strrchr(sz_module_path, '/') == NULL)
	{
		return CRG_NOT_FOUND;
	}
	strrchr(sz_module_path, '/')[0] = '\0';

	if(strrchr(sz_module_path, '/') == NULL)
	{
		return CRG_NOT_FOUND;
	}
	strrchr(sz_module_path, '/')[0] = '\0';

	out_file_path = sz_module_path;
	out_file_path += "/";
	out_file_path += APP_LIST_NAME_JSON;

	return is_exist_file(out_file_path.c_str());
}

uint32_t get_current_path(char* out_path, uint32_t len)
{
	if (out_path == NULL || len == 0)
		return CRG_INVALID_PARAM;

	char sz_module_path[MAX_PATH] = { 0 };

	if (get_share_module_path(sz_module_path, MAX_PATH) != CRG_SUCCESS)
	{
		readlink("/proc/self/exe", sz_module_path, MAX_PATH);
	}

	if (strrchr(sz_module_path, '/') == NULL)
	{
		return CRG_NOT_FOUND;
	}
	strrchr(sz_module_path, '/')[0] = '\0';

	memset(out_path, 0, len);
	memcpy(out_path, sz_module_path, len < MAX_PATH ? len : MAX_PATH);

	return CRG_SUCCESS;
}

static uint32_t get_exe_parent_path(char* path, uint32_t len)
{
	if (path == NULL)
		return CRG_INVALID_PARAM;
	
	char sz_module_path[MAX_PATH] = { 0 };

	if(get_share_module_path(sz_module_path, MAX_PATH) != CRG_SUCCESS)
	{
		readlink("/proc/self/exe", sz_module_path, MAX_PATH);
	}

	if (strrchr(sz_module_path, '/') == NULL)
	{
		return CRG_NOT_FOUND;
	}
	strrchr(sz_module_path, '/')[0] = '\0';

	if (strrchr(sz_module_path, '/') == NULL)
	{
		return CRG_NOT_FOUND;
	}
	strrchr(sz_module_path, '/')[0] = '\0';

	memcpy(path, sz_module_path, len < MAX_PATH ? len : MAX_PATH);
	
	return CRG_SUCCESS;
}

uint32_t get_path_of_userinfo(char* file_path, uint32_t len)
{
	int ret = get_exe_parent_path(file_path, len);
	if(ret != CRG_SUCCESS) return ret;
	
	sprintf(file_path, "%s/%s", file_path, USERINFO_NAME_JSON);
	
	return is_exist_file(file_path);
}

uint32_t get_path_of_net_traffic(char* file_path, uint32_t len)
{
	int ret = get_exe_parent_path(file_path, len);
	if(ret != CRG_SUCCESS) return ret;
	
	sprintf(file_path, "%s/%s", file_path, NET_TRAFFIC_JSON);
	
	return is_exist_file(file_path);
}

uint32_t get_path_of_net_traffic_sh(char* file_path, uint32_t len)
{
	int ret = get_exe_parent_path(file_path, len);
	if(ret != CRG_SUCCESS) return ret;
	
	sprintf(file_path, "%s/%s", file_path, NET_TRAFFIC_SH);
	
	return is_exist_file(file_path);
}


uint32_t get_path_of_config_json(char* file_path, uint32_t len)
{
	int ret = get_exe_parent_path(file_path, len);
	if(ret != CRG_SUCCESS) return ret;
	
	sprintf(file_path, "%s/%s", file_path, CONFIG_NAME_JSON);
	
	return is_exist_file(file_path);
}

uint32_t convert_uint_to_string(char* dest, uint32_t dest_len, uint8_t* src, uint32_t src_len)
{
	if (src == NULL || src_len > dest_len || dest == NULL || dest_len == 0)
		return CRG_INVALID_PARAM;

	for (uint32_t i = 0; i < src_len; i++)
	{
		snprintf(dest + i, dest_len - i, "%d", src[i]);
	}
	return CRG_SUCCESS;
}

uint32_t run_cmd(string cmd, string& out_str)
{
	if (cmd.length() == 0)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	FILE * fp = NULL;
	char buf[MAX_BUFF_LEN] = { 0 };

	cmd += " 2>&1";
	fp = popen(cmd.c_str(), "r");

	if (fp == NULL)
		return ret;

	while (fgets(buf, sizeof(buf), fp) != NULL)
	{
		out_str += buf;
		memset(buf, 0, sizeof(buf));
	}
	// write_log("RunCmdRecv:%s\n", out_str.c_str());

	ret = CRG_SUCCESS;

	pclose(fp);

	return ret;
}


