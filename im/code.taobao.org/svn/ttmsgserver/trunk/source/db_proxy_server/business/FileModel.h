/*================================================================
*     Copyright (c) 2014年 lanhu. All rights reserved.
*   
*   文件名称：FileModel.h
*   创 建 者：Zhang Yuanhao
*   邮    箱：bluefoxah@gmail.com
*   创建日期：2014年12月31日
*   描    述：
*
#pragma once
================================================================*/
#ifndef __FILEMODEL_H__
#define __FILEMODEL_H__
#include "IM.File.pb.h"
#include "ImPduBase.h"

class CFileModel
{
public:
    virtual ~CFileModel();
    static CFileModel* getInstance();
    
   /* void getOfflineFile(uint32_t userId, list<IM::BaseDefine::OfflineFileInfo>& lsOffline);*/
    //void addOfflineFile(uint32_t fromId, uint32_t toId, string& taskId, string& fileName, uint32_t fileSize);
	bool addOfflineFile(uint32_t fromId, uint32_t toId, string& taskId, string& fileName, uint32_t fileSize, string& strIP, uint32_t nPort, uint32_t nCreateTime, uint32_t nPeriod, string& refFileID, string& refFilePath,uint32_t unStat = 0);
    void delOfflineFile(uint32_t fromId, uint32_t toId, string& taskId);
	bool getOfflineFileDetails(string& strTaskID, string& strFileID, string& strDetails, uint32_t unStat = 0, uint32_t unFrom = 0, uint32_t unTo = 0);
	void AddOfflineFile(uint32_t unFromId, uint32_t unToId, string& strFileId, string& strFileName, uint32_t unFileSize, uint32_t unToType, uint32_t unFileTime, uint32_t unPeriod);
	void DownloadOffineFile(uint32_t unUserId, string& strFileId);
    
private:
    CFileModel();
	void FillDownloadReport(string& strFileId);
    
private:
    static CFileModel* m_pInstance;
};

#endif /*defined(__FILEMODEL_H__) */
