#include "JsonWriter.h"
#include <fstream>
#include "def.h"
#include "PublicFunction.h"
#include "common.h"
#include <sys/types.h> 
#include <sys/stat.h> 

CJsonWriter::CJsonWriter()
{
	m_strConfigPath = "../config/userInfo.json";
	m_strSendListPathJson =  "../config/APPList.json";
}


CJsonWriter::~CJsonWriter()
{
}

bool CJsonWriter::SaveConfig(UserConfig cnfg)
{	
    int ret = CRG_FAIL;
	char szConfigPath[MAX_PATH] = { 0 };
	CPublicFunction::GetUserInfoConfigFilePath(szConfigPath, MAX_PATH);
    //write jsonfile from cnfg......160721,he
    JSONNode cn(JSON_NODE);
    cn.push_back(JSONNode("activeWebPath",cnfg.webPath));
    cn.push_back(JSONNode("activeAppsPath",cnfg.appPath));
    cn.push_back(JSONNode("activeRunPath",cnfg.cmdPath));
    cn.push_back(JSONNode("appsVersion",cnfg.appsVersion));
    cn.push_back(JSONNode("webVersion",cnfg.webVersion));
    cn.push_back(JSONNode("installVersion",cnfg.installVersion));
    cn.push_back(JSONNode("province",cnfg.province));
    cn.push_back(JSONNode("terminal",cnfg.terminal));
	string js = cn.write_formatted();
	write_file(szConfigPath,js); 
	ret =CRG_SUCCESS;
    return ret;
}
bool CJsonWriter::ReadConfigApplication(UserConfig& cnfg)
{
    int ret = CRG_FAIL;
	char szConfigPath[MAX_PATH] = { 0 };
	ret = CPublicFunction::GetApplicationConfigFilePath(szConfigPath, MAX_PATH);
	if (ret != CRG_FOUND)
	{
		CPublicFunction::WriteLog("[DEBUG]File is not exist: %s file%s%s", szConfigPath, PRINT_POINT_STR, PRINT_ERR_FILE_STR);
		return ret;
	}
	std::string content = "";
	read_file(szConfigPath, content);
	bool valid = libjson::is_valid(content);
    if(valid)
    {
        JSONNode n = libjson::parse(content);
        JSONNode::const_iterator i = n.begin();
        string str;
        while(i != n.end())
        {  
            std::string node_name = i -> name();
            if(node_name=="requestTimeInterva"){
               cnfg.time = atoi(i->as_string().c_str());
            } 
            if(node_name=="verhttpRequestAdress"){
               cnfg.verUrl = i->as_string();
            } 
            if(node_name=="loghttpRequestAdress"){
               cnfg.logUrl = i->as_string();
            } 
            if(node_name=="sucesshttpRequestAdress"){
               cnfg.sucessUrl = i->as_string();
            } 
            if(node_name=="failhttpRequestAdress"){
               cnfg.failUrl = i->as_string();
            } 
            if(node_name=="hearthttpRequestAdress"){
               cnfg.heartUrl = i->as_string();
            } 
            if(node_name=="userName"){
               cnfg.account = i->as_string();
            } 
            if(node_name=="passWord"){
               cnfg.password = i->as_string();
            } 
            if(node_name=="tryDownloadCount")
            {
                cnfg.try_download_count=atoi(i->as_string().c_str());
            }
            if(node_name=="perDownloadTimeout")
            {
                cnfg.per_download_timeout=atoi(i->as_string().c_str());
            } 
            i++;
        }
 		if (cnfg.per_download_timeout > 150 || cnfg.per_download_timeout < 1)
		{
				CPublicFunction::WriteLog("config.xml文件错,per_download_timeout值%d不对，赋默认值：1\n", cnfg.per_download_timeout);
		    	cnfg.per_download_timeout = 1;
		}
 		if (cnfg.try_download_count > 150 || cnfg.try_download_count < 1)
		{
				CPublicFunction::WriteLog("config.xml文件错,cnfg.try_download_count值%d不对，赋默认值：6\n", cnfg.try_download_count);
		    	cnfg.try_download_count = 6;
		}       ret =CRG_SUCCESS;
    }
    else
    {
        CPublicFunction::WriteLog("[DEBUG]Load json file is failure, please check json format, file: %s\n", szConfigPath);
		return CRG_FAIL;
	}
    return ret;
}

bool CJsonWriter::ReadConfig(UserConfig& cnfg)
{
    int ret = CRG_FAIL;
	char szConfigPath[MAX_PATH] = { 0 };
	ret = CPublicFunction::GetUserInfoConfigFilePath(szConfigPath, MAX_PATH);
	if (ret != CRG_FOUND)
	{
		CPublicFunction::WriteLog("[DEBUG]File is not exist: %s file%s%s", szConfigPath, PRINT_POINT_STR, PRINT_ERR_FILE_STR);
		return ret;
	}
    string content = "";
	read_file(szConfigPath, content); 
    bool valid = libjson::is_valid(content);
 	if(valid)
    {
		JSONNode n = libjson::parse(content);
		JSONNode::const_iterator i = n.begin();
		string str;
		while(i != n.end())
        {  
			//  memset(&InstallApp,0, sizeof(InstallIosAppInfo));
			std::string node_name = i -> name();
			if(node_name=="appsVersion"){
				cnfg.appsVersion = i->as_string();
			} 
			if(node_name=="driverVersion"){
				cnfg.driverVersion = i->as_string();
			} 
			if(node_name=="webVersion"){
				cnfg.webVersion = i->as_string();
			} 
			if(node_name=="installVersion"){
				cnfg.installVersion = i->as_string();
			} 
            if(node_name=="activeWebPath"){
               cnfg.webPath = i->as_string();
            } 
            if(node_name=="activeAppsPath"){
               cnfg.appPath = i->as_string();
            } 
            if(node_name=="activeRunPath"){
               cnfg.cmdPath = i->as_string();
            }
			if(node_name=="province"){
				cnfg.province = i->as_string();
			}
			if(node_name=="terminal"){
				cnfg.terminal = i->as_string();
			}
			i++;
		}
		ret =CRG_SUCCESS;
    }
    else
    {
        CPublicFunction::WriteLog("[DEBUG]Load json file is failure, please check json format, file: %s\n", szConfigPath);
		return CRG_FAIL;
	}
    return ret;
}

bool CJsonWriter::CreatEmptyFile(std::string path)
{
	FILE* f = fopen(path.c_str(), "r");
	if(f == NULL)
		perror("fopen");
	else if(f != NULL)
		fclose(f);
	
	return true;
}

// ¶ÁÈ¡JsonÎÄŒþÄÚÈÝµœVectorÖÐ
int  CJsonWriter::ReadJsonFile(std::vector<std::string>& vec)
{
    int ret = CRG_FAIL;
	char szConfigPath[MAX_PATH] = { 0 };
	ret = CPublicFunction::GetAppJsonConfigFilePath(szConfigPath, MAX_PATH);
	if (ret != CRG_FOUND)
	{
		CPublicFunction::WriteLog("[DEBUG]File is not exist: %s file%s%s", szConfigPath, PRINT_POINT_STR, PRINT_ERR_FILE_STR);
		return ret;
	}
    string content = "";
	read_file(szConfigPath,content);
    // printf("content is %s\n",content.c_str());
    bool valid = libjson::is_valid(content);
    if(valid){
        ret = CRG_SUCCESS;
        JSONNode n = libjson::parse(content);
        JSONNode::const_iterator i = n.begin(); 
        string str;
        while(i != n.end()){  
            std::string node_name = i -> name();
            //if(node_name==IOS_STR){
                if(i->type()== JSON_ARRAY ){
                    JSONNode::const_iterator k = (*i).begin();
                    while(k != (*i).end()){
                        JSONNode::const_iterator j = (*k).begin(); 
                        while(j!=(*k).end())
                        {
                            node_name = j -> name();
                            if(node_name ==APP_PATH_STR){
                                str = j->as_string();
					           vec.push_back(str);
                            }
                            if(node_name ==APP_ICON_STR){
                                str = j->as_string();
					           vec.push_back(str);
                            }
                            ++j;
                        }
                        ++k;
                    } 
                }
           // }
            ++i;
        }
    }else{
        CPublicFunction::WriteLog("[DEBUG]File name: %s file%s%s", szConfigPath, PRINT_POINT_STR, PRINT_ERR_FILE_STR);
		ret = CRG_FAIL;
	}
	return ret;
}

time_t	 CJsonWriter::getFileUpdateTime(std::string findStr)
{
	struct stat file;//_finddata_t file;
	//long lf;
	time_t fileUpdateTime = 0;//__time64_t fileUpdateTime = 0;
	//std::string filePathName = m_strSendListPathJson;
	#if 0
	if ((lf = _findfirst(findStr.c_str(), &file)) == -1l)//_findfirst·µ»ØµÄÊÇlongÐÍ; long __cdecl _findfirst(const char *, struct _finddata_t *)
	{
		fileUpdateTime = 0;
	}
	else 
	{
		fileUpdateTime = file.time_write;
		time_t lTime;
		time(&lTime);
		//ptmTime = localtime(&lTime);
        //CPublicFunction::WriteLog("ÏÖÔÚÊ±Œä%ld,ÎÄŒþÐÞžÄÊ±Œä%ld,Ê±²î%ld\n", lTime,fileUpdateTime,lTime-fileUpdateTime);
		fileUpdateTime = lTime - fileUpdateTime;
        //CPublicFunction::WriteLog("ÏÖÔÚÊ±Œä%ld,Ê±²î%ld\n", lTime,fileUpdateTime);
	}
	_findclose(lf);
	#endif
	//cout << "getFileUpdateTime : m_strSendListPathJson:" << findStr << endl;
	if(stat(findStr.c_str(), &file)==0)
	{
		fileUpdateTime = file.st_mtime;
		time_t lTime;
		time(&lTime);
		fileUpdateTime = lTime - fileUpdateTime;

	}
	else
	{
		fileUpdateTime = 0;
		perror("time_t CJsonWriter::getFileUpdateTime():stat");
	}
	cout << "getFileUpdateTime : fileUpdateTime:" << fileUpdateTime << endl;
	return fileUpdateTime;
}
bool CJsonWriter::bakInifile()
{
	FILE * file;
    std::string iniFile = m_strConfigPath;
	std::string filePathName = iniFile;
	std::string bakFile =iniFile;
	CPublicFunction::string_replace(bakFile,"userInfo.json","userInfo_bak.json");
	std::string findStr = iniFile;
	int iniFileOK = 0;  //0,ini文件正常，1，ini文件不存在；2，ini文件空;3,NOJson;4,ErrorJson
	if ((file = fopen(findStr.c_str(),"r")) == NULL)
	{
		CPublicFunction::WriteLog("配置文件不存在【%s】",iniFile.c_str());
		iniFileOK = 1;
	}
	else
	{
        char fileTxt[50]="";
        int size=fread(fileTxt,1,21,file);
		if (20 > size)
		{
		    CPublicFunction::WriteLog("配置文件少于20字节【%s】，大小为【%d】",iniFile.c_str(),size);
			iniFileOK = 2;
		}
		else
		{
			iniFileOK = 0;//文件正常，做备份
		}
	    fclose(file);
	}

	if (0 == iniFileOK)
	{
		UserConfig cnfg;
		if (!ReadConfig(cnfg))
		{
			CPublicFunction::WriteLog("ini文件格式错，未能正确读取!【%s】", iniFile.c_str());
			iniFileOK = 3;
		}
		else if (4>cnfg.appPath.length()||4>cnfg.cmdPath.length()||4>cnfg.webPath.length()||2>cnfg.terminal.length())
		{
			CPublicFunction::WriteLog("ini文件内容不全，不能正常使用!【%s】", iniFile.c_str());
			iniFileOK = 4;

		}
		else
		{
			std::string cmd;
            int i_ret=0;
 			cmd = CPublicFunction::formatstr("cp -f \"%s\" \"%s\"", iniFile.c_str(), bakFile.c_str());
			if ((i_ret=system(cmd.c_str())) >= 0)
			{
			    CPublicFunction::WriteLog("备份userInfo文件!【%s】;i_ret=%d",cmd.c_str(),i_ret );
				return true;
			}
			else
			{
				CPublicFunction::WriteLog("备份ini文件错!【%s】;i_ret=%d", cmd.c_str(),i_ret);
				return false;
			}

		}
	}
	if ((file =fopen(bakFile.c_str(), "r")) == NULL)
	{
		CPublicFunction::WriteLog("备份配置文件不存在【%s】", bakFile.c_str());
		return false;
	}
	fclose(file);
	std::string cmd;
	cmd = CPublicFunction::formatstr("cp -f \"%s\" \"%s\"", bakFile.c_str(), iniFile.c_str());
	if (system(cmd.c_str()) >= 0)
	{
	    CPublicFunction::WriteLog("启用备份userInfo文件!【%s】",bakFile.c_str() );
		return true;
	}
	else
	{

		CPublicFunction::WriteLog("启用备份ini文件错!【%s】", cmd.c_str());
		return false;
	}
		
}
