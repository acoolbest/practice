#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include <string>
#include <pthread.h>
#include "public_func/src/def.h"

char chrupr(const char chr) //char to upper 
{
	char ret = chr;
	if (ret >= 'a' && ret <= 'z') ret -= 32;
	return ret;
}

uint32_t get_private_profile_int(const char* lp_app_name, const char* lp_key_name, uint32_t n_default, const char* lp_filename)
{
	uint32_t ret = n_default;
	FILE* file = NULL;
	try
	{
		file = fopen(lp_filename, "r");
		if (file == NULL)
			return ret;
		char srcbuf[256];
		char desbuf[256];
		int size = 256;
		char* _srcbuf = srcbuf;
		char* _desbuf = desbuf;
		char target = 'S'; //'S' find section name,'K' find key name 
		while (fgets(_srcbuf, size, file) != NULL)
		{
			bool flag = true;
			if (target == 'S')
			{
				if (_srcbuf[0] != '[') continue;
				strcpy(_desbuf, lp_app_name);
				int len = strlen(_desbuf);
				int i;
				for (i = 0; i < len; i++)
				{
					if (_desbuf[i] != _srcbuf[i + 1])
					{
						flag = false;
						break;
					}
				}
				if (flag == false) continue;
				if (_srcbuf[i + 1] != ']') continue;
				target = 'K';
			}
			else
			{
				if (_srcbuf[0] == '[') break;
				strcpy(_desbuf, lp_key_name);
				int len = strlen(_desbuf);
				int i;
				for (i = 0; i < len; i++)
				{
					if (_desbuf[i] != _srcbuf[i])
					{
						flag = false;
						break;
					}
				}
				if (flag == false) continue;

				bool _flag = false;
				while (_srcbuf[i] != '\0')
				{
					if (_srcbuf[i] == ' ')
					{
						i++;
						continue;
					}
					if (_srcbuf[i] == '=')
					{
						_flag = true;
						i++;
						continue;
					}
					if (_flag == false) break;
					char* lpresult = &_srcbuf[i];
					if (strcmp(lpresult, "0\r\n") == 0 || strcmp(lpresult, "0\n") == 0 || strcmp(lpresult, "0") == 0)
					{
						fclose(file);
						file = NULL;
						return 0;
					}
					uint32_t value = strtoul(lpresult, NULL, 0);
					if (value != 0) ret = value;
					fclose(file);
					file = NULL;
					return ret;
				}
			}//end of if 
		}// end of while 
		fclose(file);
		file = NULL;
		return ret;
	}// end of try 
	catch (...)
	{
		if (file != NULL)
		{
			fclose(file);
			file = NULL;
		}
		return ret;
	}
}//end of get_private_profile_int 


uint32_t get_private_profile_string(const char* lp_app_name, const char* lp_key_name, const char* lp_default, char* lp_return_string, uint32_t n_size, const char* lp_filename)
{
	uint32_t ret = 0;
	FILE* file = NULL;
	try
	{
		ret = strlen(lp_default);
		if (ret < n_size)
			strcpy(lp_return_string, lp_default);
		else
		{
			uint32_t i;
			for (i = 0; i < n_size - 1; i++)
				lp_return_string[i] = lp_default[i];
			lp_return_string[i] = '\0';
			ret = n_size - 1;
		}

		file = fopen(lp_filename, "r");
		if (file == NULL)
			return ret;

		//if (lp_app_name == "")
		if(!strcmp(lp_app_name, ""))
		{
			char buf[256];
			int size = 256;
			char* _buf = buf;
			char* lpret = lp_return_string;
			uint32_t count = 0;

			while (fgets(_buf, size, file) != NULL && (count < n_size - 2))
			{
				if (_buf[0] != '[') continue;
				for (uint32_t i = 1; count < n_size - 2 && _buf[i] != ']' && _buf[i] != '\r'&& _buf[i] != '\n'; i++)
				{
					*lpret = _buf[i];
					lpret++;
					count++;
				}
				*lpret = '\0';
				lpret++;
				count++;
			}
			*lpret = '\0';
			if (count > 0) count--;
			fclose(file);
			file = NULL;
			return ret = count;
		}// end of lp_app_name == "" 

		//if (lp_key_name == "")
		if(!strcmp(lp_key_name, ""))
		{
			char srcbuf[256];
			char desbuf[256];
			int size = 256;
			char* _srcbuf = srcbuf;
			char* _desbuf = desbuf;
			char target = 'S'; //'S' find section name,'K' find key name 
			char* lpret = lp_return_string;
			uint32_t count = 0;
			while (fgets(_srcbuf, size, file) != NULL && (count < n_size - 2))
			{
				bool flag = true;
				if (target == 'S')
				{
					if (_srcbuf[0] != '[') continue;
					strcpy(_desbuf, lp_app_name);
					int len = strlen(_desbuf);
					int i;
					for (i = 0; i < len; i++)
					{
						if (_desbuf[i] != _srcbuf[i + 1])
						{
							flag = false;
							break;
						}
					}
					if (flag == false) continue;
					if (_srcbuf[i + 1] != ']') continue;
					target = 'K';
				}
				else
				{
					if (_srcbuf[0] == '[') break;
					for (int i = 0; count < n_size - 2 && _srcbuf[i] != ' '&&_srcbuf[i] != '=' &&_srcbuf[i] != '\r' &&_srcbuf[i] != '\n'; i++)
					{
						*lpret = _srcbuf[i];
						lpret++;
						count++;

					}
					*lpret = '\0';
					lpret++;
					count++;
				}
			}
			*lpret = '\0';
			if (count > 0) count--;
			fclose(file);
			file = NULL;
			return ret = count;

		}// end of lp_key_name == "" 

		char srcbuf[256];
		char desbuf[256];
		int size = 256;
		char* _srcbuf = srcbuf;
		char* _desbuf = desbuf;
		char target = 'S'; //'S' find section name,'K' find key name 
		while (fgets(_srcbuf, size, file) != NULL)
		{
			bool flag = true;
			if (target == 'S')
			{
				if (_srcbuf[0] != '[') continue;
				strcpy(_desbuf, lp_app_name);
				int len = strlen(_desbuf);
				int i;
				for (i = 0; i < len; i++)
				{
					if (_desbuf[i] != _srcbuf[i + 1])
					{
						flag = false;
						break;
					}
				}
				if (flag == false) continue;
				if (_srcbuf[i + 1] != ']') continue;
				target = 'K';
			}
			else
			{
				if (_srcbuf[0] == '[') break;
				strcpy(_desbuf, lp_key_name);
				int len = strlen(_desbuf);
				int i;
				for (i = 0; i < len; i++)
				{
					if (_desbuf[i] != _srcbuf[i])
					{
						flag = false;
						break;
					}
				}
				if (flag == false) continue;

				bool _flag = false;
				while (_srcbuf[i] != '\0')
				{
					if (_srcbuf[i] == ' ')
					{
						i++;
						continue;
					}
					if (_srcbuf[i] == '=')
					{
						_flag = true;
						i++;
						continue;
					}
					if (_flag == false) break;
					char* lpresult = &_srcbuf[i];
					char* lpret = lp_return_string;
					int count = 0;
					while (*lpresult != '\r' &&
							*lpresult != '\n' &&
							*lpresult != '\0'
						  )
					{
						*lpret = *lpresult;
						lpret++;
						lpresult++;
						count++;
					}
					*lpret = '\0';
					fclose(file);
					file = NULL;
					return ret = count;
				}// end of while 
			}// end of if 
		}// end of while 
		fclose(file);
		file = NULL;
		return ret;
	}//end of try 
	catch (...)
	{
		if (file != NULL)
		{
			fclose(file);
			file = NULL;
		}
		ret = strlen(lp_default);
		if (ret < n_size)
			strcpy(lp_return_string, lp_default);
		else
		{
			uint32_t i;
			for (i = 0; i < n_size - 1; i++)
				lp_return_string[i] = lp_default[i];
			lp_return_string[i] = '\0';
			ret = n_size - 1;
		}
		return ret;
	}

} //end of get_private_profile_string 


