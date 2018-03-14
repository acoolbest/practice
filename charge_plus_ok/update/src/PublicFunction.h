#pragma once
#include <tuple>
#include <vector>
#include "stdbool.h"
#define MAX_PATH                260//win_include
#define MAXLEN_LOGRECORD 500*1024

using std::vector;

std::string  RemoveHeader(const std::string& fileName);
class CPublicFunction
{
public:
	CPublicFunction();
	~CPublicFunction();
public:
	static void GetCurTime(char* curTime, bool btype);
	static bool AppendToFile(const char* fileName, const char* content, size_t fileLength);
	static void ColorPrint(const char *fm, ...);
	static void WriteLog(const char *fm, ...);
	static bool CheckFolder(const   char   *sPathName);
	static void GetCurDate(char* curDate);
	static std::string GetCurrentWorkFolder();
	static bool CopyFolderFiles(std::string strSource, std::string strDest, bool bCover = true);
	static bool MoveFolderFiles(std::string strSource,std::string strDest,bool bCover = true);
	static bool moveFile(std::string strSource, std::string strDest, bool bCover = true);
	static bool delPath(std::string strSource,bool bCover = true);
	static bool mkPath(std::string strSource,bool bCover = true);

	static std::string formatstr(const char* fm, ...);
    static bool  MkdirFolder(std::string folder);
    static bool is_dir_exist(const char *dir_path);
	static std::string GetParentFolder(std::string strPath);
	static std::string GetRootWorkPath();
	static std::string GetChildPath(std::string strPath);
	static bool EmptyFolder(std::string szPath);
	static std::string GetNegateFolder(std::string strPath);
	static void string_replace(std::string &strBig, const std::string &strsrc, const std::string &strdst);
	static int	findAllFilesFromPath(std::string path, std::vector<std::string>& vec);
	static uint32_t GetAppJsonConfigFilePath(char* filepath, uint32_t len);
	static uint32_t GetUserInfoConfigFilePath(char* filepath, uint32_t len);
	static uint32_t GetApplicationConfigFilePath(char* filepath, uint32_t len);
};

