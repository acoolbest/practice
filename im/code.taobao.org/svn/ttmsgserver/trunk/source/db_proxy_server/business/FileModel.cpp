/*================================================================
*     Copyright (c) 2014年 lanhu. All rights reserved.
*   
*   文件名称：FileModel.cpp
*   创 建 者：Zhang Yuanhao
*   邮    箱：bluefoxah@gmail.com
*   创建日期：2014年12月31日
*   描    述：
*
================================================================*/
#include "FileModel.h"
#include "../DBPool.h"
//#include "jsonxx.h"
#include "json/json.h"
#include "../../base/security/security.h"

CFileModel* CFileModel::m_pInstance = NULL;

CFileModel::CFileModel()
{
    
}

CFileModel::~CFileModel()
{
    
}

CFileModel* CFileModel::getInstance()
{
    if (m_pInstance == NULL) {
        m_pInstance = new CFileModel();
    }
    return m_pInstance;
}

//void CFileModel::getOfflineFile(uint32_t userId, list<IM::BaseDefine::OfflineFileInfo>& lsOffline)
//{
//    CDBManager* pDBManager = CDBManager::getInstance();
//    CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
//    if (pDBConn)
//    {
//        string strSql = "select * from IMTransmitFile where toUserId="+int2string(userId) + " and status=0 order by created";
//        CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
//        if(pResultSet)
//        {
//            while (pResultSet->Next())
//            {
//                IM::BaseDefine::OfflineFileInfo offlineFile;
//                offlineFile.set_from_user_id(pResultSet->GetInt("userId"));
//                offlineFile.set_task_id(pResultSet->GetString("taskId"));
//                offlineFile.set_file_name(pResultSet->GetString("fileName"));
//                offlineFile.set_file_size(pResultSet->GetInt("fsize"));
//                lsOffline.push_back(offlineFile);
//            }
//            delete pResultSet;
//        }
//        else
//        {
//            log("no result for:%s", strSql.c_str());
//        }
//        pDBManager->RelDBConn(pDBConn);
//
//		log("SQL for getting file transmittion: %s, records: %d", strSql.c_str(), lsOffline.size());
//    }
//    else
//    {
//        log("no db connection for teamtalk_slave");
//    }
//}

//bool CFileModel::addOfflineFile(uint32_t fromId, uint32_t toId, string& taskId, string& fileName, uint32_t fileSize, string& strIP, uint32_t nPort, uint32_t nCreateTime, uint32_t nPeriod, string& refFileID, uint32_t unStat)
bool CFileModel::addOfflineFile(uint32_t fromId, uint32_t toId, string& taskId, string& fileName, uint32_t fileSize, string& strIP, uint32_t nPort, uint32_t nCreateTime, uint32_t nPeriod, string& refFileID, string& refFilePath, uint32_t unStat)
{
    CDBManager* pDBManager = CDBManager::getInstance();
    CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
	bool bResult = false;
    if (pDBConn)
    {
        string strSql = "insert into IMTransmitFile (`userId`,`toUserId`,`fileName`,`fsize`,`taskId`,`status`,`created`,`updated`,serverip,serverport, validperiod, filestate, filerole,fileid,filepath) values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
		log("SQL for inserting a file transmition: %s", strSql.c_str());

        // 必须在释放连接前delete CPrepareStatement对象，否则有可能多个线程操作mysql对象，会crash
        CPrepareStatement* pStmt = new CPrepareStatement();
        if (pStmt->Init(pDBConn->GetMysql(), strSql))
        {
            uint32_t status = unStat;
            uint32_t nCreated = (uint32_t)time(NULL);
			uint32_t unState = 4;
			uint32_t unRole = 3;
            
            uint32_t index = 0;
            pStmt->SetParam(index++, fromId);
            pStmt->SetParam(index++, toId);
            pStmt->SetParam(index++, fileName);
            pStmt->SetParam(index++, fileSize);
            pStmt->SetParam(index++, taskId);
            pStmt->SetParam(index++, status);
            pStmt->SetParam(index++, nCreateTime);
            pStmt->SetParam(index++, nCreateTime);
			pStmt->SetParam(index++, strIP);
			pStmt->SetParam(index++, nPort);
			pStmt->SetParam(index++, nPeriod);
			pStmt->SetParam(index++, unState);
			pStmt->SetParam(index++, unRole);
			pStmt->SetParam(index++, refFileID);
			pStmt->SetParam(index++, refFilePath);
            
           //bool bRet = pStmt->ExecuteUpdate();
			bResult = pStmt->ExecuteUpdate();
            
            //if (!bRet)
			if (!bResult)
            {
                log("insert message failed: %s", strSql.c_str());
            }
        }
        delete pStmt;
        pDBManager->RelDBConn(pDBConn);
    }
    else
    {
        log("no db connection for teamtalk_master");
    }

	return bResult;
}

void CFileModel::delOfflineFile(uint32_t fromId, uint32_t toId, string& taskId)
{
    CDBManager* pDBManager = CDBManager::getInstance();
    CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
    if (pDBConn)
    {
        string strSql = "delete from IMTransmitFile where  userId=" + int2string(fromId) + " and toUserId="+int2string(toId) + " and taskId='" + taskId + "'";
		log("SQL for deleting a file transmition: %s", strSql.c_str());
		pDBConn->ExecuteUpdate(strSql.c_str());        
        pDBManager->RelDBConn(pDBConn);
    }
    else
    {
        log("no db connection for teamtalk_master");
    }
}


bool CFileModel::getOfflineFileDetails(string& strTaskID, string& strFileID, string& strDetails, uint32_t unStat, uint32_t unFrom, uint32_t unTo)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	//jsonxx::Object json_obj;
	Json::Value jsonObj;
	bool bResult = false;

	uint32_t nFileSize = 0;
	uint32_t nFileState = 0;
	uint32_t nFileRole = 0;
	uint32_t nCreated = 0;
	uint32_t nValidPeriod = 0;
	string strFileName;
	string strServerIP;
	uint32_t nServerPort = 0;
	uint32_t nFromUserID = 0;
	uint32_t nToUserID = 0;
	uint32_t nBasePeerID = 0;
	string strCurrTaskID;
	string strFilePath;

	if (pDBConn)
	{
		string strSql = "select * from IMTransmitFile where fileid='" + strFileID + "' and status=0 order by created";
		log("SQL for getting offline details: %s", strSql.c_str());

		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		bool bFirst = false;
		//bool bFound = false;
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				nFileSize = pResultSet->GetInt("fsize");
				nFileState = pResultSet->GetInt("filestate");
				nFileRole = pResultSet->GetInt("filerole");
				nCreated = pResultSet->GetInt("created");
				nValidPeriod = pResultSet->GetInt("validperiod");
				strFileName = pResultSet->GetString("fileName");
				strServerIP = pResultSet->GetString("serverip");
				nServerPort = pResultSet->GetInt("serverport");
				nFromUserID = pResultSet->GetInt("userId");
				nToUserID = pResultSet->GetInt("toUserId");
				strFilePath = pResultSet->GetString("filepath");
				if (!bFirst)
				{
					nBasePeerID = nToUserID; //get the original receiver
					bFirst = true;
				}
				strCurrTaskID = pResultSet->GetString("taskId");
				if (strCurrTaskID == strTaskID)
				{
					//bFound = true;
					break;
				}
				//if (unStat !=2) //hardcode here
				//{
				//	json_obj << "task_id" << strTaskID;
				//	//json_obj << "file_name" << pResultSet->GetString("filename");
				//	json_obj << "file_size" << nFileSize;
				//	json_obj << "file_state" << nFileState;
				//	json_obj << "file_client_mode" << nFileRole;
				//	json_obj << "create_time" << nCreated;
				//	json_obj << "valid_time" << nValidPeriod;
				//	json_obj << "file_path" << strFilePath;
				//	json_obj << "ip_addr" << strServerIP;
				//	json_obj << "ip_port" << nServerPort;
				//	json_obj << "from_id" << nFromUserID;
				//	json_obj << "to_id" << nToUserID;
				//}
			}
			delete pResultSet;
			pResultSet = NULL;
		}
		else
		{
			log("no result for:%s", strSql.c_str());
		}
		
		pDBManager->RelDBConn(pDBConn);

	}
	else
	{
		log("no db connection for teamtalk_slave");
	}

	//the file is forwarding to others(transferring to others)
	if (unStat == 2 && nFileSize)
	{
		nFromUserID = unFrom;
		nToUserID = unTo;
		uint32_t unCreateTime = (uint32_t)time(NULL);
		addOfflineFile(nFromUserID, nToUserID, strTaskID, strFileName, nFileSize, strServerIP, nServerPort, unCreateTime, nValidPeriod, strFileID,strFilePath, unStat); //add a history record for retransferring a file
	}

	jsonObj["task_id"] = strTaskID;
	//jsonObj[ "file_name"] = pResultSet->GetString("filename");
	jsonObj["file_size"] = nFileSize;
	jsonObj["file_state"] = nFileState;
	jsonObj["file_client_mode"] = nFileRole;
	jsonObj["create_time"] = nCreated;
	jsonObj["valid_time"] = nValidPeriod;
	jsonObj["file_path"] = strFileName; //'file_path' means file_name. change from 'file_path' to 'file_name' for the next version.(Dec,18th)
	jsonObj["ip_addr"] = strServerIP;
	jsonObj["ip_port"] = nServerPort;
	jsonObj["from_id"] = nFromUserID;
	jsonObj["to_id"] = nToUserID;
	jsonObj["base_peer_id"] = nBasePeerID;
	jsonObj["file_id"] = strFileID;
	jsonObj["from_file_path"] = strFilePath;

	char *pszData = NULL;
	uint32_t nDataLen = 0;
	string strJsonValue = jsonObj.toStyledString();

	int nResult = EncryptMsg(strJsonValue.c_str(), strJsonValue.length(), &pszData, nDataLen);
	if (!nResult)
	{
		log("JSON DATA: %s,LEN: %d", strJsonValue.c_str(), strJsonValue.length());
		log("DATA AFTER ENCRYPTING: %s, LEN:%d", pszData,nDataLen );
		strDetails = pszData;
		Free(pszData);
		pszData = NULL;
		bResult = true;
	}
	return bResult;
}


void CFileModel::AddOfflineFile(uint32_t unFromId, uint32_t unToId, string& strFileId, string& strFileName, uint32_t unFileSize, uint32_t unToType, uint32_t unFileTime, uint32_t unPeriod)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
	bool bResult = false;
	if (pDBConn)
	{
		string strSql = "insert into IMTransmitFile (fileid,filename,filesize,userid,peerid,peertype,filecreatedtime,validperiod,created,updated) values(?,?,?,?,?,?,?,?, unix_timestamp(),unix_timestamp())";
		log("SQL for inserting a file transmition: %s", strSql.c_str());

		// 必须在释放连接前delete CPrepareStatement对象，否则有可能多个线程操作mysql对象，会crash
		CPrepareStatement* pStmt = new CPrepareStatement();
		if (pStmt->Init(pDBConn->GetMysql(), strSql))
		{
			uint32_t index = 0;
			//uint32_t nCreated = (uint32_t)time(NULL);

			pStmt->SetParam(index++, strFileId);
			pStmt->SetParam(index++, strFileName);
			pStmt->SetParam(index++, unFileSize);
			pStmt->SetParam(index++, unFromId);
			pStmt->SetParam(index++, unToId);
			pStmt->SetParam(index++, unToType);
			pStmt->SetParam(index++, unFileTime);
			pStmt->SetParam(index++, unPeriod);
			//pStmt->SetParam(index++, nCreated);

			//bool bRet = pStmt->ExecuteUpdate();
			bResult = pStmt->ExecuteUpdate();

			//if (!bRet)
			if (!bResult)
			{
				log("insert record for uploading a file failed: %s", strSql.c_str());
			}
		}
		delete pStmt;
		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_master");
	}
}


void CFileModel::DownloadOffineFile(uint32_t unUserId, string& strFileId)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
	bool bResult = false;
	if (pDBConn)
	{
		string strSql = "insert into IMDownloadFileList (fileid,userid,created) values(?,?, unix_timestamp())";
		log("SQL for inserting a record for downloading a file: %s", strSql.c_str());

		// 必须在释放连接前delete CPrepareStatement对象，否则有可能多个线程操作mysql对象，会crash
		CPrepareStatement* pStmt = new CPrepareStatement();
		if (pStmt->Init(pDBConn->GetMysql(), strSql))
		{
			uint32_t index = 0;
			//uint32_t nCreated = (uint32_t)time(NULL);

			pStmt->SetParam(index++, strFileId);
			pStmt->SetParam(index++, unUserId);
			//pStmt->SetParam(index++, nCreated);

			//bool bRet = pStmt->ExecuteUpdate();
			bResult = pStmt->ExecuteUpdate();

			//if (!bRet)
			if (!bResult)
			{
				log("insert download file list failed: %s", strSql.c_str());
			}
		}
		delete pStmt;
		pStmt = NULL;
		pDBManager->RelDBConn(pDBConn);

		FillDownloadReport(strFileId);
	}
	else
	{
		log("no db connection for teamtalk_master");
	}
}


void CFileModel::FillDownloadReport(string& strFileId)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
	if (pDBConn)
	{
		string strSql = "select id from IMTransmitFileStatistics where fileid='" + strFileId + "'";
		uint32_t unSequenceNo = 0;
		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				unSequenceNo = pResultSet->GetInt("id");
			}
			delete pResultSet;
			pResultSet = NULL;
		}

		//insert a record for statistics
		if (unSequenceNo == 0)
		{
			strSql = "insert into IMTransmitFileStatistics (fileid,created,updated) values(?,unix_timestamp(),unix_timestamp())";
			log("SQL for inserting a record for file downloads statistics: %s", strSql.c_str());

			// 必须在释放连接前delete CPrepareStatement对象，否则有可能多个线程操作mysql对象，会crash
			CPrepareStatement* pStmt = new CPrepareStatement();
			if (pStmt->Init(pDBConn->GetMysql(), strSql))
			{
				uint32_t index = 0;
				//uint32_t nCreated = (uint32_t)time(NULL);

				pStmt->SetParam(index++, strFileId);
				//pStmt->SetParam(index++, nCreated);
				//pStmt->SetParam(index++, nCreated);

				bool bResult = pStmt->ExecuteUpdate();

				//if (!bRet)
				if (!bResult)
				{
					log("insert download file statistics failed: %s", strSql.c_str());
				}
			}
			delete pStmt;
			pStmt = NULL;
		}
		else
		{
			strSql = "update IMTransmitFileStatistics set downloads=downloads+1, updated=unix_timestamp() where fileid='" + strFileId + "'";
			log("SQL for updating downloads: %s", strSql.c_str());
			pDBConn->ExecuteUpdate(strSql.c_str());
		}

		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_master");
	}
}