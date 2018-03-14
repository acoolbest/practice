// UpdateService.cpp  ----Update program
//
#include "stdbool.h"
#include "HttpRequestInterface.h"
#include "JsonWriter.h"
#include "fileMd5.h"
#include "FileDown.h"
#include <signal.h>
#include <time.h>
#include <vector>
#include <string.h>
#include "def.h"
#include "PublicFunction.h"
#include "fileMd5.h"
#include <unistd.h>
#include <pthread.h>
#include "common.h"
#include "libjson/libjson.h"
#include <iostream>
#include <sys/stat.h>
#include <oss_get_object_to_file.h>
#include <semaphore.h>
#define SIG_CHARGE_EXIT 41
#define THREAD_COUNT 4
#define			CRG_SUCCESS										0x0001
#define SECOND_PER_MINUTE (60)
#define VERSION_TYPE_MOBILEAPP 1
#define VERSION_TYPE_APPLICATION_LINUX 5
#define VERSION_TYPE_WEB 4
#define DOWN_THREAD_SLEEP_SECOND 180
#define CHARGE_EXE_FILENAME "charge"
#define THREAD_INDEX_GET_VERSION      0
#define THREAD_INDEX_DOWN_APPLICATION 1
#define THREAD_INDEX_DOWN_MOBILE_APP 2
#define THREAD_INDEX_DOWN_WEB        3
bool g_bRunning = TRUE;
pthread_mutex_t mutex_downVersion = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_userInfoIO = PTHREAD_MUTEX_INITIALIZER;

int thread_run_state[THREAD_COUNT]={0};
sem_t  sem[THREAD_COUNT];

struct down_version
{
    std::string version;
    std::vector<DownLoadNode> downList;
    int down_stat;  //0,wait down;  1,downing; 2,down OK;3,down fail;
};
struct down_version version_mobile_app,version_application,version_web;
pthread_t pThreadId[THREAD_COUNT] = { 0 };
UserConfig cnfgApplication;
//sleep add_second. Stop sleep when *p_sem is haved
int wait_sig_timeout(sem_t  *p_sem,long add_second)
{        
    int ret; 
    struct timeval now; 
    struct timespec outtime; 
    gettimeofday(&now, NULL); 
    outtime.tv_sec = now.tv_sec +add_second; 
    outtime.tv_nsec = now.tv_usec * 1000;
    ret = sem_timedwait(p_sem,&outtime);
    return ret;
} 

std::string getCurVersion(int type,struct down_version * pVersion)
{
	    std::string curVersion="0";
        struct down_version * p=NULL;
		switch (type)
		{
		case VERSION_TYPE_MOBILEAPP:
			p=&version_mobile_app;
			break;
		case VERSION_TYPE_APPLICATION_LINUX:
			p=&version_application;
			break;
		case VERSION_TYPE_WEB:
			p=&version_web;
			break;
		default:
			return "0";
		}
        if(pthread_mutex_lock(&mutex_downVersion)!=0) //lock
        {
            CPublicFunction::WriteLog("pthread_mutex_lock downVersion failure, type: %d", type);
            return "0";
        }
	    pVersion->version=p->version;
        pVersion->downList=p->downList;
        pVersion->down_stat=p->down_stat;
        curVersion =p->version;
        if(pthread_mutex_unlock(&mutex_downVersion)!=0) //unlock
        {
            CPublicFunction::WriteLog("pthread_mutex_unlock downVersion failure, type: %d", type);
        }
        if(curVersion.empty())
        {
            curVersion=pVersion->version="0";
        }
	    return curVersion;
}
int setCurVersion(int type,struct down_version curVersion)
{//return 0,OK; other,fail
        struct down_version * p=NULL;
		switch (type)
		{
		case VERSION_TYPE_MOBILEAPP:
			p=&version_mobile_app;
			break;
		case VERSION_TYPE_APPLICATION_LINUX:
			p=&version_application;
			break;
		case VERSION_TYPE_WEB:
			p=&version_web;
			break;
		default:
			return -1;
		}
        if(pthread_mutex_lock(&mutex_downVersion)!=0) //lock
        {
            CPublicFunction::WriteLog("pthread_mutex_lock downVersion failure, type: %d", type);
            return -2;
        }
        p->version=curVersion.version;
        p->downList.swap(curVersion.downList);
        p->down_stat=curVersion.down_stat;
        if(pthread_mutex_unlock(&mutex_downVersion)!=0) //unlock
        {
            CPublicFunction::WriteLog("pthread_mutex_unlock downVersion failure, type: %d", type);
        }
	    return 0;

}
int clearCurVersion(int type)
{//return 0,OK; other,fail
        struct down_version * p=NULL;
		switch (type)
		{
		case VERSION_TYPE_MOBILEAPP:
			p=&version_mobile_app;
			break;
		case VERSION_TYPE_APPLICATION_LINUX:
			p=&version_application;
			break;
		case VERSION_TYPE_WEB:
			p=&version_web;
			break;
		default:
			return -1;
		}
        if(pthread_mutex_lock(&mutex_downVersion)!=0) //lock
        {
            CPublicFunction::WriteLog("pthread_mutex_lock downVersion failure, type: %d", type);
            return -2;
        }
	    p->version="0";
        p->downList.clear();
        std::vector<DownLoadNode>(p->downList).swap(p->downList);
        p->down_stat=0;
        if(pthread_mutex_unlock(&mutex_downVersion)!=0) //unlock
        {
            CPublicFunction::WriteLog("pthread_mutex_unlock downVersion failure, type: %d", type);
        }
	    return 0;

}
void initCurVersion()
{
    version_mobile_app.version="";
    version_mobile_app.down_stat=0;
    version_application.version="";
    version_application.down_stat=0;
    version_web.version="";
    version_web.down_stat=0;
}
void send_sig_to_application(int signum,const char * appname)
{
        char cmd[256]="";
        sprintf(cmd,"killall -%d %s",signum,appname);
        system(cmd);
}

void SetRunningStatus(bool bRunning)
{
	g_bRunning = bRunning;
}

bool IsRunning()
{
	return g_bRunning;
}
bool readConfig(UserConfig& cnfg)
{
    bool ret = false;
	CJsonWriter writer;
    if(pthread_mutex_lock(&mutex_userInfoIO)!=0) //lock
    {
            CPublicFunction::WriteLog("pthread_mutex_lock userinfo.json read failure");
            return false;
    }
	if (writer.ReadConfig(cnfg))
	{
        cnfg.time = cnfgApplication.time;
        cnfg.verUrl = cnfgApplication.verUrl;
        cnfg.logUrl = cnfgApplication.logUrl;
        cnfg.sucessUrl = cnfgApplication.sucessUrl ;
        cnfg.failUrl = cnfgApplication.failUrl;
        cnfg.heartUrl =cnfgApplication.heartUrl;
        cnfg.account = cnfgApplication.account;
        cnfg.password =cnfgApplication.password;
        cnfg.try_download_count=cnfgApplication.try_download_count;
        cnfg.per_download_timeout=cnfgApplication.per_download_timeout;
        ret= true;
	}
    else
    {
		CPublicFunction::WriteLog("读配置文件useInfo错!readConfig");
		ret= false;
    }
    if(pthread_mutex_unlock(&mutex_userInfoIO)!=0) //unlock
    {
            CPublicFunction::WriteLog("pthread_mutex_unlock userinfo.json read failure");
    }
    return ret;
}
bool saveConfig(int type,std::string version)
{	
    bool ret = false;
	char szConfigPath[MAX_PATH] = { 0 };
	CPublicFunction::GetUserInfoConfigFilePath(szConfigPath, MAX_PATH);
    string content = "";
    string version_node_name="";
	switch (type)
	{
		case VERSION_TYPE_MOBILEAPP:
			version_node_name="appsVersion";
			break;
		case VERSION_TYPE_APPLICATION_LINUX:
			version_node_name="installVersion";
			break;
		case VERSION_TYPE_WEB:
			version_node_name="webVersion";
			break;
		default:
			return false;
	}
    //lock
    if(pthread_mutex_lock(&mutex_userInfoIO)!=0) //lock
    {
            CPublicFunction::WriteLog("pthread_mutex_lock userinfo.json write failure, type: %d", type);
            return false;
    }
    read_file(szConfigPath, content); 
    bool valid = libjson::is_valid(content);
    if(valid)
    {
        JSONNode n = libjson::parse(content);
        JSONNode::iterator i = n.begin(); 
        string str;
        while(i != n.end())
        {  
            std::string node_name = i -> name();
            if(node_name==version_node_name)
            {
                JSONNode  an = JSONNode(node_name,version);
                i->swap(an);
            } 
            i++;
        }
        string js = n.write_formatted();
        write_file(szConfigPath,js); 
        ret =true;
    }
    else
    {
        CPublicFunction::WriteLog("[DEBUG]Load json file is failure, please check json format, file: %s\n", szConfigPath);
    }
    //unlock
    if(pthread_mutex_unlock(&mutex_userInfoIO)!=0) //unlock
    {
            CPublicFunction::WriteLog("pthread_mutex_unlock userinfo.json write failure, type: %d", type);
    }
    return ret;
}

void overWriteJson(std::vector<DownLoadNode> NeedDownList,std::string activePath)
{
        JSONNode rn(JSON_NODE);
        JSONNode ios(JSON_ARRAY); 
        ios.set_name("IOS");
        JSONNode android(JSON_ARRAY); 
        android.set_name("ANDROID");
        std::string writeJson;
	for (std::vector<DownLoadNode>::iterator itr = NeedDownList.begin(); itr != NeedDownList.end(); itr++)
    {
               JSONNode cn(JSON_NODE);
               cn.push_back(JSONNode("AppId",itr->fileVersion.appId));
               cn.push_back(JSONNode("AppName",itr->fileVersion.appName));
               cn.push_back(JSONNode("AppBundleId",itr->fileVersion.appPkgName));
               int pos = itr->fileVersion.downUrl.find_last_of('/');
            std::string fileName = RemoveHeader(itr->fileVersion.downUrl.substr(pos + 1)) ; 
                            //fileName = removeStr(fileName);
               cn.push_back(JSONNode("AppPath",activePath + "/" + fileName));
               cn.push_back(JSONNode("AppVersion",itr->fileVersion.version));
			   cn.push_back(JSONNode("autoOpen",itr->fileVersion.autoOpen));
        if(itr->fileVersion.appType==std::string("0")){ //android
            android.push_back(cn);
        }
        if(itr->fileVersion.appType==std::string("1")){ //ios
            ios.push_back(cn);
        }
    }
    rn.push_back(android);
    rn.push_back(ios);
    writeJson =rn.write_formatted();
	char szAppListPath[MAX_BUFF_LEN] = { 0 };
	CPublicFunction::GetAppJsonConfigFilePath(szAppListPath, MAX_BUFF_LEN);
	write_file(szAppListPath,writeJson);
	CPublicFunction::WriteLog("Write APP_LIST!");
	
}
std::vector<DownLoadNode> GetDownLoadList(VersionVector vec,std::string strSaveFolder,std::string strActivePath)
{
	std::vector<DownLoadNode> downList;
	for (std::vector<FileVersion>::iterator itr = vec.begin(); itr != vec.end();itr++)
	{
		FileVersion fileVersion = *itr;
		DownLoadNode node;
		if (fileVersion.downUrl.empty())
		{
			continue;
		}
		int pos = fileVersion.downUrl.find_last_of('/');
		std::string fileName = RemoveHeader(fileVersion.downUrl.substr(pos + 1)) ; 
		std::string fileName_preMd5 =fileVersion.downUrl.substr(pos + 1);
		node.bNeedDown = TRUE;
		node.savePath = strSaveFolder + "/" + fileName_preMd5;
		std::string strActive = strSaveFolder + "/" + fileName;
		if(access(strActive.c_str(),F_OK)==0)
        {
			CFileMd5 md5;
			if (md5.CompareMd5(strActive, fileVersion.md5))
			{
				node.bNeedDown = false;
			}

		}
		node.fileVersion = fileVersion;
		downList.push_back(node);

	}
	return downList;
}


bool FinishDownLoading()
{
	{
		SetRunningStatus(FALSE);
        sem_post(sem+THREAD_INDEX_GET_VERSION);
        sem_post(sem+THREAD_INDEX_DOWN_MOBILE_APP); 
        sem_post(sem+THREAD_INDEX_DOWN_WEB);
	}
	return TRUE;
}


int GetUpdateInterval()
{
	int dwIntervalSec = -1;
    if(cnfgApplication.time>0 && cnfgApplication.time<1440)
	    dwIntervalSec = cnfgApplication.time * SECOND_PER_MINUTE;
    else
	    dwIntervalSec = 10 * SECOND_PER_MINUTE;
	return dwIntervalSec;
}
void downloadThread(int type) //»ù±ŸÏÂÔØÏß³Ì£¬type²ÎÊýŽ«ÈëÏÂÔØÄÚÈÝ·ÖÀà£¬1¡¢ÊÖ»úAPP£»2¡¢ÓŠÓÃ³ÌÐò£»4¡¢Web¡£
//160523,ºÎ
{
	std::vector<DownLoadNode> downList;
	std::string newVersion, downVersion, usingVersion;
	std::string activePath, downLocalPath;
    struct down_version st_downVersion;
    sem_t * p_sem=NULL;
    switch(type)
    {
        case VERSION_TYPE_MOBILEAPP:
            p_sem=sem+THREAD_INDEX_DOWN_MOBILE_APP;
            break;
        case VERSION_TYPE_APPLICATION_LINUX:
            p_sem=sem+THREAD_INDEX_DOWN_APPLICATION;
            break;
        case VERSION_TYPE_WEB:
            p_sem=sem+THREAD_INDEX_DOWN_WEB;
            break;
        default:
		    CPublicFunction::WriteLog("downthread type ERROR, type=[%d] ....",type);
    }
	while (g_bRunning)
	{
		//µÈŽýÑÓÊ±Í¬Ê±µÈŽýœáÊøœø³ÌÏûÏ¢£¬160526£¬ºÎ
		CPublicFunction::WriteLog("downthread[%d] Waiting for Receive semaphore%ds....",type,DOWN_THREAD_SLEEP_SECOND);
        if(-1!=wait_sig_timeout(p_sem,DOWN_THREAD_SLEEP_SECOND))
            break;
		UserConfig cnfg;
		if (!readConfig(cnfg))
		{
			CPublicFunction::WriteLog("读配置文件useInfo错!downThread,type=%d",type);
			continue;
		}
		newVersion = downVersion = getCurVersion(type,&st_downVersion);
		if (newVersion == "0")
		{
			continue;
		}
		downList.swap(st_downVersion.downList);
		switch (type)
		{//version_mobile_app,version_application,version_web
		case VERSION_TYPE_MOBILEAPP:
			usingVersion = cnfg.appsVersion;
			activePath = cnfg.appPath;
			break;
		case VERSION_TYPE_APPLICATION_LINUX:
			usingVersion = cnfg.installVersion;
			activePath = cnfg.cmdPath;
			break;
		case VERSION_TYPE_WEB:
			usingVersion = cnfg.webVersion;
			activePath = cnfg.webPath;
			break;
		default:
			continue;
		}
		if (newVersion == usingVersion )
			continue;
		downLocalPath = CPublicFunction::GetNegateFolder(activePath);
		bool bDonwRet = true;
 		for (std::vector<DownLoadNode>::iterator itr = downList.begin(); itr != downList.end(); itr++)
		{
			DownLoadNode fv = *itr;
			if (fv.bNeedDown)
			{
				if (fv.savePath.substr(0, downLocalPath.size()) != downLocalPath)
				{
				    CPublicFunction::WriteLog("fv.savePath：%s----downLocalPath: %s", fv.savePath.substr(0, downLocalPath.size()).c_str(),downLocalPath.c_str());
					bDonwRet = false;
					break;
				}
				int nDownCount = 0;
				int nDownRet = 0;
				while (nDownCount++ < cnfg.try_download_count)
				{
				    CPublicFunction::WriteLog("第%d次下载文件start：%s 到  %s",nDownCount,fv.fileVersion.downUrl.c_str(), fv.savePath.c_str());
					nDownRet = download(fv.fileVersion.downUrl.c_str(),cnfg.account.c_str(), cnfg.password.c_str(),fv.savePath.c_str(),cnfg.per_download_timeout*SECOND_PER_MINUTE); //·Ö×ª»¯ÎªÃë
					if (CURLE_OK==nDownRet)
                        break;
                    else if (CURLE_OPERATION_TIMEDOUT!=nDownRet)
					{ 
					    if (!g_bRunning || downVersion !=(newVersion= getCurVersion(type,&st_downVersion)))
					    {
                            CPublicFunction::WriteLog("g_bRunning=%d,newVersion=%s,downVersion=%s",g_bRunning,newVersion.c_str(),downVersion.c_str());
						    bDonwRet = false;
						    break;
					    }
						CPublicFunction::WriteLog("第%d次down failed: [%s] ，二分钟后继续下载。。。。。。", nDownCount, fv.fileVersion.downUrl.c_str());
                        if(-1!=wait_sig_timeout(p_sem,120))
                            break;
					}
					else
						CPublicFunction::WriteLog("第%d次down 结束: [%s] 。。。。。。", nDownCount, fv.fileVersion.downUrl.c_str());
					if (!g_bRunning || downVersion !=(newVersion= getCurVersion(type,&st_downVersion)))
					{
                            CPublicFunction::WriteLog("g_bRunning=%d,newVersion=%s,downVersion=%s",g_bRunning,newVersion.c_str(),downVersion.c_str());
						    bDonwRet = false;
						    break;
					}

				}
				if (!g_bRunning || downVersion != getCurVersion(type,&st_downVersion))
				{
					bDonwRet = false;
					break;
				}
				if (nDownCount <= cnfg.try_download_count)
				{
					CPublicFunction::WriteLog("down sucessed: [%s] ", fv.fileVersion.downUrl.c_str());
					//»ñÈ¡±ŸµØÎÄŒþŽóÐ¡ÐÅÏ¢  
					std::string fileName = RemoveHeader(CPublicFunction::GetChildPath(fv.savePath));
					std::string cmd = "";
					char curDate[50] = "";
					CPublicFunction::GetCurDate(curDate);
					cmd = CPublicFunction::formatstr("rm  \"%s\" -f ", (downLocalPath + "/" + fileName).c_str()
						, CPublicFunction::GetParentFolder(CPublicFunction::GetCurrentWorkFolder()).c_str(), curDate);
					system(cmd.c_str());
					CPublicFunction::WriteLog("%s", cmd.c_str());
					struct stat file_info;
					if (stat((downLocalPath + "/" + fileName).c_str(), &file_info) < 0)
					{
						cmd = CPublicFunction::formatstr("mv -f %s %s", fv.savePath.c_str(), (downLocalPath + "/" + fileName).c_str());
						if (-1 != system(cmd.c_str()))
						{
							CPublicFunction::WriteLog("下载文件更名完成: [%s] ", cmd.c_str());
							itr->bNeedDown = FALSE;
                            if(fileName.substr(fileName.size()-3)==".sh")
                            {
                                cmd = CPublicFunction::formatstr("chmod 755 %s",(downLocalPath + "/" + fileName).c_str());
						        if (-1 != system(cmd.c_str()))
                                {
                                    CPublicFunction::WriteLog("下载文件chmod 755 success: [%s] ", cmd.c_str());
                                }
                                else
                                {
                                    CPublicFunction::WriteLog("下载文件chmod 755 faild: [%s] ", cmd.c_str());
                                }  
                            }
							bDonwRet = true;
						}
						else
						{
							CPublicFunction::WriteLog("下载文件更名失败: [%s] ", cmd.c_str());
							bDonwRet = false;
							break;
						}
					}
					else
					{
						CPublicFunction::WriteLog("未能删除过期文件: [%s] ,stat()返回值：%d", (downLocalPath + "/" + fileName).c_str(), stat((downLocalPath + "/" + fileName).c_str(), &file_info));
						bDonwRet = false;
						break;
					}
				}
				else
				{
					CPublicFunction::WriteLog("down failed: [%s], 配置下载次数：%d", fv.fileVersion.downUrl.c_str(), cnfg.try_download_count);
					bDonwRet = false;
					break;
				}
			}
			else
				bDonwRet = true;
		}
		if (!g_bRunning || downVersion != getCurVersion(type,&st_downVersion))
		{
			bDonwRet = false;
		}
		else if (!bDonwRet)
		{//ÏÂÔØÊ§°Ü£¬µ¥žöÎÄŒþµÄÏÂÔØ³É¹Š×ŽÌ¬£¬žÄÐŽ»ØDownList£¬160615£¬ºÎ
            st_downVersion.downList.swap(downList);
			setCurVersion(type, st_downVersion);
		}
		else
		{
            CPublicFunction::EmptyFolder(activePath+"/*");
			CPublicFunction::MoveFolderFiles(downLocalPath+"/*", activePath);
            //char cmd[255]="";
			switch (type)
			{
			case VERSION_TYPE_MOBILEAPP:
                overWriteJson(downList,activePath);
				break;
			case VERSION_TYPE_APPLICATION_LINUX:
                //set application can run
                //sprintf(cmd,"chmod 755 %s/*",activePath.c_str());
                //system(cmd);
				break;
			case VERSION_TYPE_WEB:
				break;
			default:
				continue;
			}
            clearCurVersion(type);
			if (!saveConfig(type,downVersion))
			{
				CPublicFunction::WriteLog("重写配置文件不成功，type=%d,Version:%s", type,downVersion.c_str());
			 }
			else
			{
				CPublicFunction::WriteLog("重写配置文件成功，type=%d,Version:%s",type,downVersion.c_str());
				if (VERSION_TYPE_APPLICATION_LINUX == type)
				{
					FinishDownLoading();
				}
				else if (VERSION_TYPE_WEB == type)
				{
 				    CPublicFunction::WriteLog("Restart browser by disp.sh!");
				    string restart_cmd = cnfg.webPath+"/disp.sh";
				    if(-1!=system(restart_cmd.c_str()))
                        CPublicFunction::WriteLog("Restart browser success!");
                    else
				        CPublicFunction::WriteLog("Restart browser faild!");               
                }
            }
		}
	}
}
//µÃµœÐÂ°æ±Ÿºó£¬Œì²éœÓÊÜÏÂÔØÄ¿ÂŒÎÄŒþMD5Öµ£¬·¢ÏÖ²»ÐèÒªÏÂÔØµÄ£¬±£ŽæµœÁÙÊ±Ä¿ÂŒ£¬160616£¬ºÎ
int saveOKFileToTmp(VersionVector vec, std::string strSaveFolder)
{
	int ret = 0;  //·µ»ØÖµ-1£¬vecÔªËØŽí»òÏµÍ³ÃüÁîŽí£¬ÎÞ·šŽŠÀí£»·ÇžºÕûÊý£¬ŽæÈëÁÙÊ±Ä¿ÂŒµÄÎÄŒþžöÊý
	std::string strActivePath;
	//ŽŽœšÁÙÊ±Ä¿ÂŒ£¬ÓÃÓÚŽæ²»ÐèÒªÏÂÔØµÄÎÄŒþ£¬Žý²»ÐèÏÂÔØµÄÎÄŒþ¿œ»ØÔ­Ä¿ÂŒºó£¬ÔÙÉŸ³ýÁÙÊ±Ä¿ÂŒ
	for (std::vector<FileVersion>::iterator itr = vec.begin(); itr != vec.end(); itr++)
	{
		FileVersion fileVersion = *itr;
		DownLoadNode node;
		std::string corresponsePath;
		if (fileVersion.downUrl.empty())
		{
				return -1;
		}
		corresponsePath = fileVersion.downUrl;
		std::string fileName = RemoveHeader(CPublicFunction::GetChildPath(corresponsePath));
		std::string strActive = strSaveFolder + "/" + fileName;
		std::string tmpPath = strSaveFolder + "/tmp/" + fileName;
		FILE* f = fopen(strActive.c_str(), "r");
		bool bUpdataExist = (f == NULL) ? FALSE : TRUE;
		if (f)
		{
			fclose(f);
		}
		f = NULL;
		CFileMd5 md5;
		if (bUpdataExist)//
		{
			if (md5.CompareMd5(strActive, fileVersion.md5))
			{//±£ŽæÎÄŒþµœÁÙÊ±Ä¿ÂŒ
	            if (0==ret && !CPublicFunction::mkPath(strSaveFolder + "/tmp"))
	            {
		            CPublicFunction::WriteLog("未能创建临时目录：%s", (strSaveFolder + "/tmp").c_str());
		            return -1;
	            }
				CPublicFunction::moveFile(strActive, tmpPath);
				ret++;
			}
		}

	}
	return ret;
}

bool get_version_from_server(int type,UserConfig cnfg)
{
    bool ret=true;
	std::string strContent = "";
    JSONNode insrn(JSON_NODE);
	std::string strVersion = "",str_old_version="";
	std::string strType = "";
	std::string targetPath="",active_path="";
	bool bHaveTmpFile = false;
	VersionVector fVec;
	string  str_json;
	int nResult=-1;
	char csType[2] = "";
    struct down_version st_downVersion={"0"};
    switch (type)
    {
        case VERSION_TYPE_APPLICATION_LINUX:
			str_old_version=cnfg.installVersion;
            active_path=cnfg.cmdPath;
 	        targetPath = CPublicFunction::GetNegateFolder(cnfg.cmdPath);
            break;
        case VERSION_TYPE_MOBILEAPP:
            str_old_version=cnfg.appsVersion;
            active_path=cnfg.appPath;
	        targetPath = CPublicFunction::GetNegateFolder(cnfg.appPath);
            break;
        case VERSION_TYPE_WEB:
            str_old_version=cnfg.webVersion;
            active_path=cnfg.webPath;
	        targetPath = CPublicFunction::GetNegateFolder(cnfg.webPath);
            break;
        default:
            ret=false;
    }
    if(!ret) return ret;
    if (str_old_version.empty())
    {
		    insrn.push_back(JSONNode("version","0"));
    }
    else
    {
		    insrn.push_back(JSONNode("version",str_old_version));
    }
	sprintf(csType,"%d",type);//VERSION_TYPE_APPLICATION_LINUX);
    insrn.push_back(JSONNode("type",csType));
    insrn.push_back(JSONNode("province",cnfg.province));
   	str_json =insrn.write_formatted();
   	nResult = HttpPost(cnfg.verUrl, str_json, "Content-Type:application/json; charset=utf-8",strContent);  
	CPublicFunction::WriteLog("TYPE:softVersion HTTP_RESULT:%d ---HTTP_CONTENT:%s", nResult, strContent.c_str());
	if (-1 == nResult)
	{
			CPublicFunction::WriteLog("未收到服务器返回内容！！！");
			ret=false;
	}
	else if (nResult != 200)
	{
			ret=false;
	}
	else	if (Json2Vector(strContent, strVersion, strType, fVec) != CRG_SUCCESS)
	{
			CPublicFunction::WriteLog("Json2Vector faled");
			ret=false;
	}
    if(!ret) return ret;
	if (strType == csType && strVersion != str_old_version && strVersion !=getCurVersion(type,&st_downVersion))
	{
			if (0 < saveOKFileToTmp(fVec, targetPath))
			{
				bHaveTmpFile = true; //ÓÐ²»ÐèÏÂÔØµÄÎÄŒþŽæÈëÁÙÊ±ÎÄŒþŒÐ£¬ŒÇµÃÒÆ¶¯ºóÉŸ³ýÁÙÊ±Ä¿ÂŒ
			}
			else
				bHaveTmpFile = false;
			CPublicFunction::CopyFolderFiles(active_path, targetPath);
			if (bHaveTmpFile)
			{
				CPublicFunction::CopyFolderFiles(targetPath + "/tmp", targetPath);
				CPublicFunction::delPath(targetPath + "/tmp");
			}
			std::vector<DownLoadNode> NeedDownList =GetDownLoadList(fVec, targetPath, active_path);
			st_downVersion.downList.swap(NeedDownList);
            st_downVersion.version=strVersion;
            st_downVersion.down_stat=0;
			setCurVersion(type, st_downVersion);
	}
    return ret;
}

int DeleteNoUsingFiles()
{
	int delFileCount = 0;
	CJsonWriter writer;
	UserConfig info;
	if(!readConfig(info))
	{
		CPublicFunction::WriteLog("¶ÁÅäÖÃÎÄŒþuseInfoŽí!DeleteNoUsingFiles");
		return 0;
	}
	std::vector <std::string> vecAllFiles, vecUseFiles;
	if (CPublicFunction::findAllFilesFromPath(info.appPath, vecAllFiles) > 0
		&& writer.ReadJsonFile(vecUseFiles) > 0)
	{
		for (unsigned int i = 0; i < vecAllFiles.size(); i++)
		{
			bool bFound = false;
			for (unsigned int j = 0; j < vecUseFiles.size(); j++)
			{
				if (vecAllFiles[i] == vecUseFiles[j])
				{
					bFound = true;
					break;
				}
			}
			if (!bFound && writer.getFileUpdateTime(vecAllFiles[i])>3600*240)
			{
				std::string cmd = "";
				cmd = CPublicFunction::formatstr("rm  \"%s\" -r -f", vecAllFiles[i].c_str());
				CPublicFunction::WriteLog("%s", cmd.c_str());
				//CPublicFunction::WriteLog("ÉŸÎÄŒþŽí!¡Ÿ%s¡¿,·µ»Ø¡Ÿ%ld¡¿", cmd.c_str(), WinExec(cmd.c_str(), SW_HIDE));
				system(cmd.c_str());
				//system(cmd.c_str());
				delFileCount++;
			} 
		}
	}
	return delFileCount;
}
//getVersionWork, updateApplicationWork, updateMobileAppWork, updateWebWork,4žöÏß³Ì
void*  getVersionWork(void* lpParam)
{
	DWORD dwUpdateCount = 0;
	CJsonWriter writer;
	writer.bakInifile();
	sleep(1);
	UserConfig cnfg;
    thread_run_state[THREAD_INDEX_GET_VERSION]=1;
	while (IsRunning())
	{
		if (dwUpdateCount < 10000)
			dwUpdateCount++;
		else
			dwUpdateCount = 1;
		CPublicFunction::WriteLog("GetVersion work is starting, work Count: %d", dwUpdateCount);
		int nUpdateInterval = GetUpdateInterval();
		if (nUpdateInterval <= 0)
			CPublicFunction::WriteLog("%s", "Get update time interval error, please check userInfo.xml, Update Count: %d", dwUpdateCount);
		nUpdateInterval = nUpdateInterval <=0 ? SECOND_PER_MINUTE * 10 : nUpdateInterval;
       //µÈŽýÑÓÊ±Í¬Ê±µÈŽýœáÊøœø³ÌÏûÏ¢£¬160526£¬ºÎ
		CPublicFunction::WriteLog("Waiting for Receive semaphore %ds....",nUpdateInterval);
        if(-1!=wait_sig_timeout(sem+THREAD_INDEX_GET_VERSION,nUpdateInterval))
            break;
		CPublicFunction::WriteLog("GetVersion work is starting!");
		if (!readConfig(cnfg))
		{
			CPublicFunction::WriteLog("读配置文件useInfo错!getVersionWork");
			continue;
		}

        get_version_from_server(VERSION_TYPE_APPLICATION_LINUX,cnfg);
        get_version_from_server(VERSION_TYPE_MOBILEAPP,cnfg);
        get_version_from_server(VERSION_TYPE_WEB,cnfg);
		CPublicFunction::WriteLog("GetVersion work is end!");
	}
	CPublicFunction::WriteLog("GetVersion work thread is the end!");
    thread_run_state[THREAD_INDEX_GET_VERSION]=0;
    return (void *)0;
}
void* updateApplicationWork(void* lpParam)
{
    thread_run_state[THREAD_INDEX_DOWN_APPLICATION]=1;
	downloadThread(VERSION_TYPE_APPLICATION_LINUX);
	CPublicFunction::WriteLog("updateApplicationWork thread is the end!");
    thread_run_state[THREAD_INDEX_DOWN_APPLICATION]=0;
    return (void *)0;
}
void* updateMobileAppWork(void* lpParam)
{
    thread_run_state[THREAD_INDEX_DOWN_MOBILE_APP]=1;
	downloadThread(VERSION_TYPE_MOBILEAPP);
	CPublicFunction::WriteLog("updateMobileAppWork thread is the end!");
    thread_run_state[THREAD_INDEX_DOWN_MOBILE_APP]=0;
    return (void *)0;
}
void*  updateWebWork(void* lpParam)
{
    thread_run_state[THREAD_INDEX_DOWN_WEB]=1;
	downloadThread(VERSION_TYPE_WEB);
	CPublicFunction::WriteLog("updateWebWork thread is the end!");
    thread_run_state[THREAD_INDEX_DOWN_WEB]=0;
    return (void *)0;
}
//ÉŸ³ý²»ÓÃµÄAPPÎÄŒþ£¬Íš¹ý¶ÔÕÕJsonÎÄŒþºÍAPPÎÄŒþÄ¿ÂŒÊµŒÊÎÄŒþ£¬°Ñ²»ÓÃµÄÎÄŒþÉŸ³ýµô

typedef void* (*FUNC)(void *);

void StartWork()
{
	CJsonWriter writer;
	if (!writer.ReadConfigApplication(cnfgApplication))
	{
			CPublicFunction::WriteLog("读配置文件config.json错!startWark");
			return;
	}
	if (!init_oss())
	{
		CPublicFunction::WriteLog("init_oss failure...");
		return;
	}
	initCurVersion();
	DWORD i = 0;
    int ret;
	pthread_mutex_init(&mutex_userInfoIO,NULL);//lock for userinfo.json read or write
 	pthread_mutex_init(&mutex_downVersion,NULL);
   //each thread each semaphore,create thread and init semaphore
	FUNC func[THREAD_COUNT] = {getVersionWork,updateApplicationWork, updateMobileAppWork,updateWebWork};
	for (i = 0; i < THREAD_COUNT; i++)
	{
        sem_init(sem+i,0,0); 
        ret=pthread_create(pThreadId+i,NULL,func[i],NULL);
        if(ret!=0)
        {
			CPublicFunction::WriteLog("Create thread [%d] work failure...\n",i+1);
			goto workExit;
		}
		sleep(1);
	}
    pthread_join(pThreadId[0],NULL);
workExit:
	send_sig_to_application(SIG_CHARGE_EXIT,CHARGE_EXE_FILENAME);
	CPublicFunction::WriteLog("Send signal num: %d to Charge process", SIG_CHARGE_EXIT);
	for(i=0;i<6;i++)
        if(thread_run_state[THREAD_INDEX_DOWN_MOBILE_APP]||thread_run_state[THREAD_INDEX_DOWN_WEB])
            sleep(30);
        else
            break;
    pthread_mutex_destroy(&mutex_userInfoIO); 	
    pthread_mutex_destroy(&mutex_downVersion); 	
    release_oss();
    for(i=0;i<THREAD_COUNT;i++)
        sem_destroy(sem+i);
}


int main()
{
	CPublicFunction::WriteLog("%s", "charge update process is running!");
	StartWork();
	CPublicFunction::WriteLog("%s", "charge update process is exit!");
	return 0;
}




