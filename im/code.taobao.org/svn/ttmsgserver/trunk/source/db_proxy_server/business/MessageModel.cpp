/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *
 *   文件名称：MessageModel.cpp
 *   创 建 者：Zhang Yuanhao
 *   邮    箱：bluefoxah@gmail.com
 *   创建日期：2014年12月15日
 *   描    述：
 *
 ================================================================*/

#include <map>
#include <set>

#include "../DBPool.h"
#include "../CachePool.h"
#include "MessageModel.h"
#include "AudioModel.h"
#include "SessionModel.h"
#include "RelationModel.h"
#include "FileModel.h"
#include "../base/security/security.h"
#include "../base/jsoncpp/json/json.h"

using namespace std;

CMessageModel* CMessageModel::m_pInstance = NULL;
extern string strAudioEnc;

CMessageModel::CMessageModel()
{

}

CMessageModel::~CMessageModel()
{

}

CMessageModel* CMessageModel::getInstance()
{
	if (!m_pInstance) {
		m_pInstance = new CMessageModel();
	}

	return m_pInstance;
}

void CMessageModel::getMessage(uint32_t nUserId, uint32_t nPeerId, uint32_t nMsgId,
                               uint32_t nMsgCnt, list<IM::BaseDefine::MsgInfo>& lsMsg)
{
    uint32_t nRelateId = CRelationModel::getInstance()->getRelationId(nUserId, nPeerId, false);
	if (nRelateId != INVALID_VALUE)
    {
        CDBManager* pDBManager = CDBManager::getInstance();
        CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
        if (pDBConn)
        {
            string strTableName = "IMMessage_" + int2string(nRelateId % 8);
            string strSql;
			//george change the sql on Dec 15th,2015, change from "order by created,id" to "order by id,created"
            if (nMsgId == 0) {
                strSql = "select * from " + strTableName + " force index (idx_relateId_status_created) where relateId= " + int2string(nRelateId) + " and status = 0 order by id desc, created desc limit " + int2string(nMsgCnt);
            }
            else
            {
                strSql = "select * from " + strTableName + " force index (idx_relateId_status_created) where relateId= " + int2string(nRelateId) + " and status = 0 and msgId <=" + int2string(nMsgId)+ " order by id desc, created desc limit " + int2string(nMsgCnt);
            }
            CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
            if (pResultSet)
            {
                while (pResultSet->Next())
                {
                    IM::BaseDefine::MsgInfo cMsg;
                    cMsg.set_msg_id(pResultSet->GetInt("msgId"));
                    cMsg.set_from_session_id(pResultSet->GetInt("fromId"));
                    cMsg.set_create_time(pResultSet->GetInt("created"));
					cMsg.set_msg_flag(pResultSet->GetInt("flag")); //normal flag
                    IM::BaseDefine::MsgType nMsgType = IM::BaseDefine::MsgType(pResultSet->GetInt("type"));
                    if(IM::BaseDefine::MsgType_IsValid(nMsgType))
                    {
                        cMsg.set_msg_type(nMsgType);
                        cMsg.set_msg_data(pResultSet->GetString("content"));
                        lsMsg.push_back(cMsg);
                    }
                    else
                    {
                        log("invalid msgType. userId=%u, peerId=%u, msgId=%u, msgCnt=%u, msgType=%u", nUserId, nPeerId, nMsgId, nMsgCnt, nMsgType);
                    }
                }
                delete pResultSet;
            }
            else
            {
                log("no result set: %s", strSql.c_str());
            }
            pDBManager->RelDBConn(pDBConn);
            if (!lsMsg.empty())
            {
                CAudioModel::getInstance()->readAudios(lsMsg);
            }
        }
        else
        {
            log("no db connection for teamtalk_slave");
        }
	}
    else
    {
        log("no relation between %lu and %lu", nUserId, nPeerId);
    }
}

/*
 * IMMessage 分表
 * AddFriendShip()
 * if nFromId or nToId is ShopEmployee
 * GetShopId
 * Insert into IMMessage_ShopId%8
 */
bool CMessageModel::sendMessage(uint32_t nRelateId, uint32_t nFromId, uint32_t nToId, IM::BaseDefine::MsgType nMsgType, uint32_t nCreateTime, uint32_t nMsgId, string& strMsgContent, int32_t nFlag)
{
    bool bRet =false;
    //if (nFromId == 0 || nToId == 0) {
	if (nToId == 0)
	{
        //log("invalid userId.%u->%u", nFromId, nToId);
		log("invalid dest userId:%u", nToId);
        return bRet;
    }

	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
	if (pDBConn)
    {
        string strTableName = "IMMessage_" + int2string(nRelateId % 8);
        string strSql = "insert into " + strTableName + " (`relateId`, `fromId`, `toId`, `msgId`, `content`, `status`, `type`, `created`, `updated`,flag) values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
        // 必须在释放连接前delete CPrepareStatement对象，否则有可能多个线程操作mysql对象，会crash
        CPrepareStatement* pStmt = new CPrepareStatement();
        if (pStmt->Init(pDBConn->GetMysql(), strSql))
        {
            uint32_t nStatus = 0;
            uint32_t nType = nMsgType;
            uint32_t index = 0;
            pStmt->SetParam(index++, nRelateId);
            pStmt->SetParam(index++, nFromId);
            pStmt->SetParam(index++, nToId);
            pStmt->SetParam(index++, nMsgId);
            pStmt->SetParam(index++, strMsgContent);
            pStmt->SetParam(index++, nStatus);
            pStmt->SetParam(index++, nType);
            pStmt->SetParam(index++, nCreateTime);
            pStmt->SetParam(index++, nCreateTime);
			pStmt->SetParam(index++, nFlag);
            bRet = pStmt->ExecuteUpdate();
        }
        delete pStmt;
        pDBManager->RelDBConn(pDBConn);
        if (bRet)
        {
            uint32_t nNow = (uint32_t) time(NULL);
			if (nFromId != 0)
			{
				incMsgCount(nFromId, nToId);
			}
        }
        else
        {
            log("insert message failed: %s", strSql.c_str());
        }
	}
    else
    {
        log("no db connection for teamtalk_master");
    }
	return bRet;
}

bool CMessageModel::sendAudioMessage(uint32_t nRelateId, uint32_t nFromId, uint32_t nToId, IM::BaseDefine::MsgType nMsgType, uint32_t nCreateTime, uint32_t nMsgId, const char* pMsgContent, uint32_t nMsgLen)
{
	if (nMsgLen <= 4) {
		return false;
	}

	CAudioModel* pAudioModel = CAudioModel::getInstance();
	int nAudioId = pAudioModel->saveAudioInfo(nFromId, nToId, nCreateTime, pMsgContent, nMsgLen);

	bool bRet = true;
	if (nAudioId != -1) {
		string strMsg = int2string(nAudioId);
		bRet = sendMessage(nRelateId, nFromId, nToId, nMsgType, nCreateTime, nMsgId, strMsg);
	} else {
		bRet = false;
	}

	return bRet;
}


bool CMessageModel::sendFileMessage(uint32_t nRelateId, uint32_t nFromId, uint32_t nToId, IM::BaseDefine::MsgType nMsgType, uint32_t nCreateTime,
	uint32_t nMsgId, const char* pMsgContent, uint32_t nMsgLen, string& strFileInfo)
{
	if (nMsgLen <= 4) {
		return false;
	}

	bool bResult = false;
	CFileModel* pFileModel = CFileModel::getInstance();
	char *pszData = NULL;
	uint32_t nDataLen = 0;
	int nResult = DecryptMsg(pMsgContent, nMsgLen, &pszData, nDataLen);
	if (!nResult)
	{
		log("JSON DATA AFTER DECRYPTION: %s", pszData);
		string strCurrentTask = "";
		string strCurrentFile = "";
		uint32_t unStatus = 0;
		uint32_t unUserID = 0, unToUserID = 0;
		string strFileName = "";
		string strFileId = "";
		uint32_t unCreatedTime = 0u, unValidTime = 0u, unFileSize = 0u;

		try
		{
			Json::Reader reader;
			Json::Value root;
			if (reader.parse(pszData, root))
			{
				strCurrentTask = root["task_id"].asString();
				if (!root["file_id"].isNull())
				{
					strCurrentFile = root["file_id"].asString();
				}

				if (!root["status"].isNull())
				{
					unStatus = root["status"].asUInt();
					unUserID = root["from_id"].asUInt();
					unToUserID = root["to_id"].asUInt();
				}

				if (!root["create_time"].isNull() && root["create_time"].isUInt64())
				{
					unCreatedTime = root["create_time"].asUInt64() / 1000;
				}

				if (!root["file_path"].isNull() && root["file_path"].isString())
				{
					strFileName = root["file_path"].asString();
				}

				if (!root["file_size"].isNull() && root["file_size"].isUInt())
				{
					unFileSize = root["file_size"].asUInt();
				}

				if (!root["valid_time"].isNull() && root["valid_time"].isUInt())
				{
					unValidTime = root["valid_time"].asUInt();
				}

				if (!root["fileId"].isNull() && root["fileId"].isString())
				{
					strFileId = root["fileId"].asString();
				}
			}
		}
		catch (exception &e)
		{
			;
			log("an exception occurs when parsing json data; %s", e.what());
		}

		//log("TASK ID: %s, FILE ID: %s", strCurrentTask.c_str(), strCurrentFile.c_str());
		if (pFileModel && strFileId != "")
		{
			pFileModel->AddOfflineFile(nFromId, nToId, strFileId, strFileName, unFileSize, IM::BaseDefine::SESSION_TYPE_SINGLE, unCreatedTime, unValidTime);
			strFileInfo = pMsgContent;
			bResult = sendMessage(nRelateId, nFromId, nToId, nMsgType, nCreateTime, nMsgId, strFileInfo);
		}
		else if (pFileModel && strCurrentTask != "")
		{		
			bResult = pFileModel->getOfflineFileDetails(strCurrentTask, strCurrentFile, strFileInfo, unStatus, unUserID, unToUserID);
			if (bResult)
			{
				bResult = sendMessage(nRelateId, nFromId, nToId, nMsgType, nCreateTime, nMsgId, strFileInfo);
			}
		}
	}

	if (pszData)
	{
		Free(pszData);
		pszData = NULL;
	}
	return bResult;
}


void CMessageModel::incMsgCount(uint32_t nFromId, uint32_t nToId)
{
	CacheManager* pCacheManager = CacheManager::getInstance();
	// increase message count
	CacheConn* pCacheConn = pCacheManager->GetCacheConn("unread");
	if (pCacheConn) {
		pCacheConn->hincrBy("unread_" + int2string(nToId), int2string(nFromId), 1);
		pCacheManager->RelCacheConn(pCacheConn);
	} else {
		log("no cache connection to increase unread count: %d->%d", nFromId, nToId);
	}
}

void CMessageModel::getUnreadMsgCount(uint32_t nUserId, uint32_t &nTotalCnt, list<IM::BaseDefine::UnreadInfo>& lsUnreadCount)
{
    CacheManager* pCacheManager = CacheManager::getInstance();
    CacheConn* pCacheConn = pCacheManager->GetCacheConn("unread");
    if (pCacheConn)
    {
        map<string, string> mapUnread;
        string strKey = "unread_" + int2string(nUserId);
        bool bRet = pCacheConn->hgetAll(strKey, mapUnread);
        pCacheManager->RelCacheConn(pCacheConn);
        if(bRet)
        {
            IM::BaseDefine::UnreadInfo cUnreadInfo;
            for (auto it = mapUnread.begin(); it != mapUnread.end(); it++) {
                cUnreadInfo.set_session_id(atoi(it->first.c_str()));
                cUnreadInfo.set_unread_cnt(atoi(it->second.c_str()));
                cUnreadInfo.set_session_type(IM::BaseDefine::SESSION_TYPE_SINGLE);
                uint32_t nMsgId = 0;
                string strMsgData;
                IM::BaseDefine::MsgType nMsgType;
				uint32_t unFromId = cUnreadInfo.session_id();
                getLastMsg(unFromId, nUserId, nMsgId, strMsgData, nMsgType);
                if(IM::BaseDefine::MsgType_IsValid(nMsgType))
                {
                    cUnreadInfo.set_latest_msg_id(nMsgId);
                    cUnreadInfo.set_latest_msg_data(strMsgData);
                    cUnreadInfo.set_latest_msg_type(nMsgType);
                    cUnreadInfo.set_latest_msg_from_user_id(unFromId);
					//cUnreadInfo.set_latest_msg_from_user_id(cUnreadInfo.session_id());
                    lsUnreadCount.push_back(cUnreadInfo);
                    nTotalCnt += cUnreadInfo.unread_cnt();
                }
                else
                {
                    log("invalid msgType. userId=%u, peerId=%u, msgType=%u", nUserId, cUnreadInfo.session_id(), nMsgType);
                }
            }
        }
        else
        {
            log("hgetall %s failed!", strKey.c_str());
        }
    }
    else
    {
        log("no cache connection for unread");
    }
}

uint32_t CMessageModel::getMsgId(uint32_t nRelateId)
{
    uint32_t nMsgId = 0;
    CacheManager* pCacheManager = CacheManager::getInstance();
    CacheConn* pCacheConn = pCacheManager->GetCacheConn("unread");
	if (pCacheConn)
	{
		string strKey = "msg_id_" + int2string(nRelateId);
		if (pCacheConn->isExists(strKey))
		{
			nMsgId = pCacheConn->incrBy(strKey, 1);
			pCacheManager->RelCacheConn(pCacheConn);
			log("key: %s, msg id for realted %d : %d", strKey.c_str(), nRelateId, nMsgId);
			return nMsgId;
		}

		//key is not found,we should get maximum msgid from db
		pCacheManager->RelCacheConn(pCacheConn);

		CDBManager* pDBManager = CDBManager::getInstance();
		CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
		if (pDBConn)
		{
			string strSql = "select max(msgid) as maxmsgid from IMMessage_" + int2string(nRelateId%8) + " where relateid=" + int2string(nRelateId);
			CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
			log("key %s is not found in cache and get max msgid from db: %s", strKey.c_str(), strSql.c_str());
			if (pResultSet)
			{
				while (pResultSet->Next())
				{
					if (pResultSet->GetString("maxmsgid") != NULL)
					{
						nMsgId = pResultSet->GetInt("maxmsgid");
					}
					log("max msgid for %d is %d", nRelateId, nMsgId);
				}
				delete pResultSet;
				pResultSet = NULL;
			}
			pDBManager->RelDBConn(pDBConn);
		}

		if (nMsgId > 0)
		{
			pCacheConn = pCacheManager->GetCacheConn("unread");
			if (pCacheConn)
			{
				string strMsgId = int2string(nMsgId);
				pCacheConn->set(strKey, strMsgId);
				nMsgId = pCacheConn->incrBy(strKey, 1);
				pCacheManager->RelCacheConn(pCacheConn);	
				log("get msgid from db and increase by 1, key: %s, value: %s,msgid: %d", strKey.c_str(), strMsgId.c_str(), nMsgId);
			}
			else
			{
				nMsgId = 0;
			}
		}
    }

	return nMsgId;
}

/**
 *  <#Description#>
 *
 *  @param nFromId    <#nFromId description#>
 *  @param nToId      <#nToId description#>
 *  @param nMsgId     <#nMsgId description#>
 *  @param strMsgData <#strMsgData description#>
 *  @param nMsgType   <#nMsgType description#>
 *  @param nStatus    0获取未被删除的，1获取所有的，默认获取未被删除的
 */
void CMessageModel::getLastMsg(uint32_t& nFromId, uint32_t nToId, uint32_t& nMsgId, string& strMsgData, IM::BaseDefine::MsgType& nMsgType, uint32_t nStatus)
{
    uint32_t nRelateId = CRelationModel::getInstance()->getRelationId(nFromId, nToId, false);
    
    if (nRelateId != INVALID_VALUE)
    {

        CDBManager* pDBManager = CDBManager::getInstance();
        CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
        if (pDBConn)
        {
            string strTableName = "IMMessage_" + int2string(nRelateId % 8);
            string strSql = "select fromid, msgId,type,content from " + strTableName + " force index (idx_relateId_status_created) where relateId= " + int2string(nRelateId) + " and status = 0 order by created desc, id desc limit 1";
            CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
            if (pResultSet)
            {
                while (pResultSet->Next())
                {
                    nMsgId = pResultSet->GetInt("msgId");
					nFromId = pResultSet->GetInt("fromid");

                    nMsgType = IM::BaseDefine::MsgType(pResultSet->GetInt("type"));
                    if (nMsgType == IM::BaseDefine::MSG_TYPE_SINGLE_AUDIO)
                    {
                        // "[语音]"加密后的字符串
                        strMsgData = strAudioEnc;
                    }
                    else
                    {
                        strMsgData = pResultSet->GetString("content");
                    }
                }
                delete pResultSet;
            }
            else
            {
                log("no result set: %s", strSql.c_str());
            }
            pDBManager->RelDBConn(pDBConn);
        }
        else
        {
            log("no db connection_slave");
        }
    }
    else
    {
        log("no relation between %lu and %lu", nFromId, nToId);
    }
}

void CMessageModel::getUnReadCntAll(uint32_t nUserId, uint32_t &nTotalCnt)
{
    CacheManager* pCacheManager = CacheManager::getInstance();
    CacheConn* pCacheConn = pCacheManager->GetCacheConn("unread");
    if (pCacheConn)
    {
        map<string, string> mapUnread;
        string strKey = "unread_" + int2string(nUserId);
        bool bRet = pCacheConn->hgetAll(strKey, mapUnread);
        pCacheManager->RelCacheConn(pCacheConn);
        
        if(bRet)
        {
            for (auto it = mapUnread.begin(); it != mapUnread.end(); it++) {
                nTotalCnt += atoi(it->second.c_str());
				log("Unread Count: %d for User: %d", nTotalCnt, nUserId);
            }
        }
        else
        {
            log("hgetall %s failed!", strKey.c_str());
        }
    }
    else
    {
        log("no cache connection for unread");
    }
}

void CMessageModel::getMsgByMsgId(uint32_t nUserId, uint32_t nPeerId, const list<uint32_t> &lsMsgId, list<IM::BaseDefine::MsgInfo> &lsMsg, bool bAudioData)
{
    if(lsMsgId.empty())

    {
        return ;
    }
    uint32_t nRelateId = CRelationModel::getInstance()->getRelationId(nUserId, nPeerId, false);

    if(nRelateId == INVALID_VALUE)
    {
        log("invalid relation id between %u and %u", nUserId, nPeerId);
        return;
    }

    CDBManager* pDBManager = CDBManager::getInstance();
    CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
    if (pDBConn)
    {
        string strTableName = "IMMessage_" + int2string(nRelateId % 8);
        string strClause ;
        bool bFirst = true;
        for(auto it= lsMsgId.begin(); it!=lsMsgId.end();++it)
        {
            if (bFirst) {
                bFirst = false;
                strClause = int2string(*it);
            }
            else
            {
                strClause += ("," + int2string(*it));
            }
        }

        string strSql = "select * from " + strTableName + " where relateId=" + int2string(nRelateId) + "  and status=0 and msgId in (" + strClause + ") order by created desc, id desc limit 100";
        CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
        if (pResultSet)
        {
            while (pResultSet->Next())

            {
                IM::BaseDefine::MsgInfo msg;
                msg.set_msg_id(pResultSet->GetInt("msgId"));
                msg.set_from_session_id(pResultSet->GetInt("fromId"));
                msg.set_create_time(pResultSet->GetInt("created"));
                IM::BaseDefine::MsgType nMsgType = IM::BaseDefine::MsgType(pResultSet->GetInt("type"));
                if(IM::BaseDefine::MsgType_IsValid(nMsgType))
                {
                    msg.set_msg_type(nMsgType);
                    msg.set_msg_data(pResultSet->GetString("content"));
                    lsMsg.push_back(msg);
                }
                else
                {
                    log("invalid msgType. userId=%u, peerId=%u, msgType=%u, msgId=%u", nUserId, nPeerId, nMsgType, msg.msg_id());
                }
            }
            delete pResultSet;
        }
        else
        {
            log("no result set for sql:%s", strSql.c_str());
        }
        pDBManager->RelDBConn(pDBConn);
		if (bAudioData) //george modified the bracket on Dec 30th,2015
		{
			if (!lsMsg.empty())
			{
				CAudioModel::getInstance()->readAudios(lsMsg);
			}
		}
    }
    else
    {
        log("no db connection for teamtalk_slave");
    }
}

bool CMessageModel::resetMsgId(uint32_t nRelateId)
{
    bool bRet = false;
    uint32_t nMsgId = 0;
    CacheManager* pCacheManager = CacheManager::getInstance();
    CacheConn* pCacheConn = pCacheManager->GetCacheConn("unread");
    if(pCacheConn)
    {
        string strKey = "msg_id_" + int2string(nRelateId);
        string strValue = "0";
        string strReply = pCacheConn->set(strKey, strValue);
        //if(strReply == strValue)
		//log("strReplay: %s", strReply.c_str());
		if (strReply == "OK")
        {
            bRet = true;
        }
        pCacheManager->RelCacheConn(pCacheConn);
		log("set msg id for relateid: %d", nRelateId);
    }
    return bRet;
}


uint32_t CMessageModel::sendFileStatusMessage(uint32_t nRelateId, uint32_t unUserId, const char* pMsgContent, uint32_t nMsgLen)
{
	if (nMsgLen < 1)
	{
		return 0;
	}

	CFileModel* pFileModel = CFileModel::getInstance();

	uint32_t unMsgId = 0;
	uint32_t unResult = 0;
	char *pszData = NULL;
	uint32_t nDataLen = 0;
	string strFileId = "";
	int nResult = DecryptMsg(pMsgContent, nMsgLen, &pszData, nDataLen);
	if (!nResult)
	{
		log("JSON DATA AFTER DECRYPTION: %s", pszData);
		try
		{
			Json::Reader reader;
			Json::Value root;
			if (reader.parse(pszData, root))
			{
				if (!root["msgId"].isNull() && root["msgId"].isUInt())
				{
					unMsgId = root["msgId"].asUInt(); //get message id
				}

				if (!root["fileId"].isNull() && root["fileId"].isString())
				{
					strFileId = root["fileId"].asString();
				}
			}
		}
		catch (exception &e)
		{
			;
			log("an exception occurs when parsing json data; %s", e.what());
		}

		if (pFileModel && strFileId != "")
		{
			pFileModel->DownloadOffineFile(unUserId, strFileId);
		}

		if (unMsgId > 0)	//get flag
		{
			CDBManager* pDBManager = CDBManager::getInstance();
			CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
			if (pDBConn)
			{
				int32_t nFlag = -1;
				string strTableName = "IMMessage_" + int2string(nRelateId % 8);
				string strSql = "select flag from " + strTableName + " where relateId= " + int2string(nRelateId) + " and msgid = " + int2string(unMsgId);
				CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
				if (pResultSet)
				{
					while (pResultSet->Next())
					{
						nFlag = pResultSet->GetInt("flag");
					}

					delete pResultSet;
					pResultSet = NULL;
				}

				if (nFlag == FILE_STATUS_READY)
				{
					strSql = "update " + strTableName + " set flag = " + int2string(FILE_STATUS_RECEIVED) + ", updated=unix_timestamp() where relateid=" + int2string(nRelateId) + " and msgid=" + int2string(unMsgId);
					pDBConn->ExecuteUpdate(strSql.c_str()); //filed is received
					unResult = unMsgId; // result is set msgid or status(file is received)?
				}

				pDBManager->RelDBConn(pDBConn);
			}
		}
	}

	if (pszData)
	{
		Free(pszData);
		pszData = NULL;
	}

	return unResult;
}

