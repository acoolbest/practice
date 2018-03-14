#include "PublicFunction.h"
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h> 
#include "def.h"
#include <stdarg.h>
#include "common.h"


using namespace std;  

CPublicFunction::CPublicFunction()
{
}


CPublicFunction::~CPublicFunction()
{
}

///////////////////////////////////////////
//  check directory
//////////////////////////////////////////
bool  CPublicFunction::CheckFolder(const   char   *sPathName)
{

	char   DirName[256];
	strcpy(DirName, sPathName);
	int   i, len = strlen(DirName);

	if (DirName[len - 1] != '/')
	{
		strcat(DirName, "/");
	}

	len = strlen(DirName);

	for (i = 1; i < len; i++)
	{
		if (DirName[i] == '/')
		{
			DirName[i] = 0;
			if (access(DirName, F_OK) != 0)
			{
				if (mkdir(DirName, 0777) == -1)
				{
					CPublicFunction::WriteLog("[DEBUG] mkdir %s failed", sPathName);
					return   false;
				}
			}
			DirName[i] = '/';
		}
	}

	if (access(sPathName, F_OK) != 0)
	{
		return false;
	}

	return  true;
}

std::string CPublicFunction::GetCurrentWorkFolder()
{
	char szFilePath[MAX_PATH];
	getcwd(szFilePath, MAX_PATH);
	std::string strPath(szFilePath);
	return strPath;
}

void CPublicFunction::ColorPrint(const char *fm, ...)
{
	int iSize = 0;

	char buff[MAXLEN_LOGRECORD] = { 0 };

	int i = 0;
	char curTime[32] = { 0 };
	GetCurTime(curTime, true);
	i = sprintf(buff, "%s\t", curTime);
	int m_maxlen = MAXLEN_LOGRECORD - (i + 3);
	va_list args;
	va_start(args, fm);
	iSize = vsprintf(buff + i, fm, args);
	va_end(args);

	if (iSize > m_maxlen || iSize < 0)
	{
		memset(buff, 0, MAXLEN_LOGRECORD);
		iSize = sprintf(buff, "%s\t", curTime);
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
void CPublicFunction::WriteLog(const char *fm, ...)
{
	int iSize = 0;

	char buff[MAXLEN_LOGRECORD] = { 0 };

	int i = 0;
	char curTime[32] = { 0 };
	GetCurTime(curTime, true);
	i = sprintf(buff, "%s\t", curTime);
	int m_maxlen = MAXLEN_LOGRECORD - (i + 3);
	va_list args;
	va_start(args, fm);
	iSize = vsprintf(buff + i, fm, args);
	va_end(args);

	if (iSize > m_maxlen || iSize < 0)
	{
		memset(buff, 0, MAXLEN_LOGRECORD);
		iSize = sprintf(buff, "%s\t", curTime);
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
	GetCurDate(curDate);
	int  j = 0;
	j = sprintf(fileName, "%s/%s", GetParentFolder(GetCurrentWorkFolder()).c_str(), "logs");
	CheckFolder(fileName);

	j += sprintf(fileName + j, "/%s_%s%s", "update", curDate, ".log");
	
	AppendToFile(fileName, buff, iSize);
	//cout << fileName << endl;

	return;
}


bool CPublicFunction::AppendToFile(const char* fileName, const char* content, size_t fileLength)
{
	FILE* fp = NULL;

	try
	{
		fp = fopen(fileName, "a+b");
		if (fp == NULL)
		{
			return false;
		}
		fwrite(content, 1, fileLength, fp);
		fclose(fp);
	}
	catch (...)
	{
		fclose(fp);
		return false;
	}

	return true;
}

///////////////////////////////////////////
//  deal with time
//////////////////////////////////////////
void CPublicFunction::GetCurTime(char* curTime, bool btype)
{
	time_t lTime;
	struct tm *ptmTime;

	time(&lTime);
	ptmTime = localtime(&lTime);
	if (btype)
	{
		strftime(curTime, 32, "%Y/%m/%d %H:%M:%S", ptmTime);
	}
	else
	{
		strftime(curTime, 32, "%Y%m%d%H%M%S", ptmTime);
	}

}

void CPublicFunction::GetCurDate(char* curDate)
{
	time_t lTime;
	struct tm *ptmTime;

	time(&lTime);

	ptmTime = localtime(&lTime);

	strftime(curDate, 16, "%Y%m%d", ptmTime);
}

bool CPublicFunction::CopyFolderFiles(std::string strSource,std::string strDest,bool bCover)
{
	std::string cmd = "";
	if (bCover)
	{
        char temp[MAX_BUFF_LEN*sizeof(char)];
        snprintf(temp,MAX_BUFF_LEN*sizeof(char),"cp %s%s  %s -r 2>>/dev/null",strSource.c_str(),"/*", strDest.c_str());
        cmd = string(temp);
	}
	return system(cmd.c_str());
}

bool CPublicFunction::MoveFolderFiles(std::string strSource,std::string strDest,bool bCover)
{
	std::string cmd = "";
	if (bCover)
	{
        char temp[MAX_BUFF_LEN*sizeof(char)];
        snprintf(temp,MAX_BUFF_LEN*sizeof(char),"mv %s %s", strSource.c_str(), strDest.c_str());
        cmd = string(temp);
        printf("cmd is %s\n",cmd.c_str());
	    return system(cmd.c_str());
	}
    return false;
}

bool CPublicFunction::MkdirFolder(std::string folder)
{
    char cmd[MAX_BUFF_LEN];
    sprintf(cmd,"mkdir %s -p",folder.c_str());
	return system(cmd);
}
bool CPublicFunction::is_dir_exist(const char *dir_path){
    if(dir_path == NULL){
        return false;
    }
    if(opendir(dir_path) == NULL){
        return false;
    }
    return true;
}
bool CPublicFunction::moveFile(std::string strSource,std::string strDest,bool bCover)
{
	std::string cmd = "";
	std::string cmd_pre = "";
	if (bCover)
	{
		char curDate[50] = "";
		GetCurDate(curDate);
		char curTime[50] = "";
		GetCurTime(curTime,true);
		cmd = formatstr("mv %s %s ", strSource.c_str(),strDest.c_str());
		WriteLog("%s", cmd.c_str());
	    return	-1 != system(cmd.c_str());
	}
    return false;
}
bool CPublicFunction::delPath(std::string strSource,bool bCover)
{
	std::string cmd = "";
	if (bCover)
	{
		char curDate[50] = "";
		GetCurDate(curDate);
		char curTime[50] = "";
		GetCurTime(curTime,true);
		cmd = formatstr("rm -r %s ", strSource.c_str());
		WriteLog("%s", cmd.c_str());
	}
	return	-1 != system(cmd.c_str());
}
bool CPublicFunction::mkPath(std::string strSource,bool bCover)
{
	std::string cmd = "";
	if (bCover)
	{
		char curDate[50] = "";
		GetCurDate(curDate);
		char curTime[50] = "";
		GetCurTime(curTime,true);
		cmd = formatstr("mkdir  %s -p ", strSource.c_str());
		WriteLog("%s", cmd.c_str());
	}
	return	-1 != system(cmd.c_str());
}
std::string CPublicFunction::formatstr(const char* fm, ...)
{
	char buff[1024 * 100];
	memset(buff, 0, sizeof(buff));
	va_list args;
	va_start(args, fm);
	vsprintf(buff, fm, args);//iSize = vsprintf(buff, fm, args);
	va_end(args);
	return std::string(buff);
}



std::string CPublicFunction::GetParentFolder(std::string strPath)
{
	int nIndex = strPath.find_last_of("/");
	if (-1 != nIndex )
	{
		if (nIndex == static_cast<int>(strPath.length() - 1))
		{
			strPath = strPath.substr(0, nIndex);//去掉末尾斜杠
		}
		nIndex = strPath.find_last_of("/");
		strPath = strPath.substr(0, nIndex);
		return strPath;
	}
	return "";
}

std::string CPublicFunction::GetRootWorkPath()
{
	std::string strPath = GetCurrentWorkFolder();
	strPath = GetParentFolder(GetParentFolder(strPath));
	return strPath;
}

std::string CPublicFunction::GetChildPath(std::string strPath)
{
	string_replace(strPath,"/", "/");
	int nIndex = strPath.find_last_of("/");
	if (-1 != nIndex)
	{
		if (nIndex == static_cast<int>(strPath.length() - 1))
		{
			strPath = strPath.substr(0, nIndex);//去掉末尾斜杠
		}
		nIndex = strPath.find_last_of("/");
		strPath = strPath.substr(nIndex + 1, strPath.length());
		return strPath;
	}
	return "";
}

bool CPublicFunction::EmptyFolder(std::string szPath)
{
	std::string cmd = "";
        char temp[MAX_BUFF_LEN*sizeof(char)];
        snprintf(temp,MAX_BUFF_LEN*sizeof(char),"rm %s -rf",szPath.c_str());
        cmd = string(temp);
	return system(cmd.c_str());
}

std::string CPublicFunction::GetNegateFolder(std::string strPath)
{
	std::string name = GetChildPath(strPath);
	int nIndex = name.find("_ass");
	if (nIndex == -1)
	{
		name += "_ass";
	}
	else
	{
		name = name.substr(0, nIndex);
	}
	return GetParentFolder(strPath) + "/" + name;
}

void CPublicFunction::string_replace(std::string &strBig, const std::string &strsrc, const std::string &strdst)
{
	std::string::size_type pos = 0;
	std::string::size_type srclen = strsrc.size();
	std::string::size_type dstlen = strdst.size();

	while ((pos = strBig.find(strsrc, pos)) != std::string::npos)
	{
		strBig.replace(pos, srclen, strdst);
		pos += dstlen;
	}
}

std::string  RemoveHeader(const std::string& fileName)
{
	std::string strTemp = "";
	int index = fileName.find("_");
	if (-1 == index)
	{
		strTemp = fileName;
	}
	else
	{
		strTemp = fileName.substr(index + 1, fileName.length());
	}
	return strTemp;
}

#if 1
int	CPublicFunction::findAllFilesFromPath(std::string path, std::vector<std::string>& vec)
{
	DIR *dirp;    
	struct dirent *direntp;
	struct stat statbuf;
	int fileCount = 0;
	std::string filePathName = path;
	if (stat(path.c_str(), &statbuf) == -1) {
		cout << "Get stat on" << path.c_str() << "Error:" << strerror(errno) << endl;

		return 0;
	}
	else 
	{
		if (S_ISREG(statbuf.st_mode))//Determine whether is a regular file
		{
			cout << path.c_str()
				<< "size:" << statbuf.st_size
				<< "\tmodified at" << ctime(&statbuf.st_mtime) << endl;
			return 1;
		}
		if ((dirp = opendir(path.c_str())) == NULL) {
			cout << "Open Directory" << path.c_str() << "Error:" << strerror(errno) << endl;
			
			return 0;
		}
		while ((direntp = readdir(dirp)) != NULL)
		{
			if (0 == strcmp(direntp->d_name, ".") || 0 == strcmp(direntp->d_name, ".."))
				continue;
			filePathName.replace(0, filePathName.size(), path + "/" + direntp->d_name);
			//cout << "findAllFilesFromPath : filePathName" << filePathName << endl;
			if (stat(filePathName.c_str(), &statbuf) == -1) {
				cout << "Get stat on" << filePathName.c_str() << "Error:" << strerror(errno) << endl;
				continue;
			}
			if (S_ISDIR(statbuf.st_mode))//Determine whether it is a directory
			{
				std::vector<std::string> vec1;
				fileCount += findAllFilesFromPath(filePathName, vec1);
				vec.insert(vec.end(), vec1.begin(), vec1.end());
				vec1.clear();
				std::vector<std::string>(vec1).swap(vec1);
			}
			else
			{
				vec.push_back(filePathName);
				fileCount++;
			}
		
		}
	}
	closedir(dirp);
	return fileCount;
}
#endif


uint32_t CPublicFunction::GetAppJsonConfigFilePath(char* filepath, uint32_t len)
{
	if (filepath == NULL || len < MAX_PATH)
		return CRG_INVALID_PARAM;

	char szModulePath[MAX_PATH] = { 0 };

	readlink("/proc/self/exe", szModulePath, MAX_PATH);
	strrchr(szModulePath, '/')[0] = '\0';
	strrchr(szModulePath, '/')[0] = '\0';

	memcpy(filepath, szModulePath, MAX_PATH);
	sprintf(filepath, "%s/%s", filepath, APP_LIST_NAME_JSON);
        
	return IsExistFile(filepath);
}
uint32_t CPublicFunction::GetUserInfoConfigFilePath(char* filepath, uint32_t len)
{
	if (filepath == NULL || len < MAX_PATH)
		return CRG_INVALID_PARAM;

	char szModulePath[MAX_PATH] = { 0 };

	readlink("/proc/self/exe", szModulePath, MAX_PATH);
 	strrchr(szModulePath, '/')[0] = '\0';
	strrchr(szModulePath, '/')[0] = '\0';

	memcpy(filepath, szModulePath, MAX_PATH);
	sprintf(filepath, "%s/%s", filepath, USERINFO_NAME_JSON);

	return IsExistFile(filepath);
}
uint32_t CPublicFunction::GetApplicationConfigFilePath(char* filepath, uint32_t len)
{
	if (filepath == NULL || len < MAX_PATH)
		return CRG_INVALID_PARAM;

	char szModulePath[MAX_PATH] = { 0 };

	readlink("/proc/self/exe", szModulePath, MAX_PATH);
	strrchr(szModulePath, '/')[0] = '\0';
	strrchr(szModulePath, '/')[0] = '\0';

	memcpy(filepath, szModulePath, MAX_PATH);
	sprintf(filepath, "%s/%s", filepath, CONFIG_NAME_JSON);

	return IsExistFile(filepath);
}


