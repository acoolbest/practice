#pragma once
//#include "HttpRequestTools.h"

#include <sstream>
#include "libjson/libjson.h"
#include "def.h"

class CJsonWriter
{
private:
//	AutoPtr<XMLConfiguration> m_pConf;
	std::string m_strConfigPath;
	std::string m_strSendListPath;
	std::string m_strSendListPathJson;
public:
	CJsonWriter();
	~CJsonWriter();
public:
	bool SaveConfig(UserConfig cnfg);
	bool ReadConfig(UserConfig& cnfg);
	bool CreatEmptyFile(std::string path);
	bool ReadConfigApplication(UserConfig& cnfg);
	//void CreateListFile(AppSendList iosList, AppSendList android);
	bool ClearFile();
	//void SaveJson(AppSendList iosList,AppSendList androidList);
	// 读取Json文件内容到Vector中
	int ReadJsonFile(std::vector<std::string> & vec);
	// get now - JsonFile last update time   (seconds).
	time_t getFileUpdateTime(std::string findStr);//__time64_t	           CJsonWriter::getFileUpdateTime();
        bool bakInifile();
};

