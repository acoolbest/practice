#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include <string>
#include <pthread.h>
#include "def.h"
			
char chrupr(const char chr) //char to upper 
{
	char ret = chr;
	if (ret >= 'a' && ret <= 'z') ret -= 32;
	return ret;
}

uint32_t GetPrivateProfileInt(const char* lpAppName, const char* lpKeyName, uint32_t nDefault, const char* lpFileName)
{
	uint32_t ret = nDefault;
	FILE* file = NULL;
	try
	{
		file = fopen(lpFileName, "r");
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
				strcpy(_desbuf, lpAppName);
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
				strcpy(_desbuf, lpKeyName);
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
					if (strcmp(lpresult, "0\r\n") == 0 || strcmp(lpresult, "0") == 0)
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
}//end of GetPrivateProfileInt 


uint32_t GetPrivateProfileString(const char* lpAppName, const char* lpKeyName, const char* lpDefault, char* lpReturnedString, uint32_t nSize, const char* lpFileName)
{
	uint32_t ret = 0;
	FILE* file = NULL;
	try
	{
		ret = strlen(lpDefault);
		if (ret < nSize)
			strcpy(lpReturnedString, lpDefault);
		else
		{
			uint32_t i;
			for (i = 0; i < nSize - 1; i++)
				lpReturnedString[i] = lpDefault[i];
			lpReturnedString[i] = '\0';
			ret = nSize - 1;
		}

		file = fopen(lpFileName, "r");
		if (file == NULL)
			return ret;

		//if (lpAppName == "")
		if(!strcmp(lpAppName, ""))
		{
			char buf[256];
			int size = 256;
			char* _buf = buf;
			char* lpret = lpReturnedString;
			uint32_t count = 0;

			while (fgets(_buf, size, file) != NULL && (count < nSize - 2))
			{
				if (_buf[0] != '[') continue;
				for (uint32_t i = 1; count < nSize - 2 && _buf[i] != ']' && _buf[i] != '\r'&& _buf[i] != '\n'; i++)
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
		}// end of lpAppName == "" 

		//if (lpKeyName == "")
		if(!strcmp(lpKeyName, ""))
		{
			char srcbuf[256];
			char desbuf[256];
			int size = 256;
			char* _srcbuf = srcbuf;
			char* _desbuf = desbuf;
			char target = 'S'; //'S' find section name,'K' find key name 
			char* lpret = lpReturnedString;
			uint32_t count = 0;
			while (fgets(_srcbuf, size, file) != NULL && (count < nSize - 2))
			{
				bool flag = true;
				if (target == 'S')
				{
					if (_srcbuf[0] != '[') continue;
					strcpy(_desbuf, lpAppName);
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
					for (int i = 0; count < nSize - 2 && _srcbuf[i] != ' '&&_srcbuf[i] != '=' &&_srcbuf[i] != '\r' &&_srcbuf[i] != '\n'; i++)
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

		}// end of lpKeyName == "" 

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
				strcpy(_desbuf, lpAppName);
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
				strcpy(_desbuf, lpKeyName);
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
					char* lpret = lpReturnedString;
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
		ret = strlen(lpDefault);
		if (ret < nSize)
			strcpy(lpReturnedString, lpDefault);
		else
		{
			uint32_t i;
			for (i = 0; i < nSize - 1; i++)
				lpReturnedString[i] = lpDefault[i];
			lpReturnedString[i] = '\0';
			ret = nSize - 1;
		}
		return ret;
	}

} //end of GetPrivateProfileString 


