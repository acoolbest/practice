#ifndef _INIFILE_H__
#define _INIFILE_H__

uint32_t GetPrivateProfileInt(const char* lpAppName, const char* lpKeyName, uint32_t nDefault, const char* lpFileName);
uint32_t GetPrivateProfileString(const char* lpAppName, const char* lpKeyName, const char* lpDefault, char* lpReturnedString, uint32_t nSize, const char* lpFileName);

#endif
