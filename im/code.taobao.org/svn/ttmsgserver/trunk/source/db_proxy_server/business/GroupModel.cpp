/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *
 *   文件名称：GroupModel.cpp
 *   创 建 者：Zhang Yuanhao
 *   邮    箱：bluefoxah@gmail.com
 *   创建日期：2014年12月15日
 *   描    述：
 *
 ================================================================*/

#include "../DBPool.h"
#include "../CachePool.h"

#include "GroupModel.h"
#include "ImPduBase.h"
#include "Common.h"
#include "AudioModel.h"
#include "UserModel.h"
#include "GroupMessageModel.h"
#include "public_define.h"
#include "SessionModel.h"

CGroupModel* CGroupModel::m_pInstance = NULL;

/**
 *  <#Description#>
 */
CGroupModel::CGroupModel()
{
    
}

CGroupModel::~CGroupModel()
{
    
}

CGroupModel* CGroupModel::getInstance()
{
    if (!m_pInstance) {
        m_pInstance = new CGroupModel();
    }
    return m_pInstance;
}

/**
 *  创建群
 *
 *  @param nUserId        创建者
 *  @param strGroupName   群名
 *  @param strGroupAvatar 群头像
 *  @param nGroupType     群类型1,固定群;2,临时群
 *  @param setMember      群成员列表，为了踢出重复的userId，使用set存储
 *
 *  @return 成功返回群Id，失败返回0;
 */
uint32_t CGroupModel::createGroup(uint32_t nUserId, const string& strGroupName, const string& strGroupAvatar, uint32_t nGroupType, set<uint32_t>& setMember, string& strGroupId, uint32_t unCompanyId)
{
	if (CUserModel::getInstance()->IsValidUser(nUserId) < 1)
	{
		log("invalid user, failed to created a group: %d", nUserId);
		return INVALID_VALUE;
	}

    uint32_t nGroupId = INVALID_VALUE;
    do {
        if(strGroupName.empty()) {
            break;
        }
        if (setMember.empty()) {
            break;
        }
        // remove repeat user
        
        
        //insert IMGroup
        //if(!insertNewGroup(nUserId, strGroupName, strGroupAvatar, nGroupType, (uint32_t)setMember.size(), nGroupId))
		if (!insertNewGroup(nUserId, strGroupName, strGroupAvatar, nGroupType, 0, nGroupId, strGroupId, unCompanyId)) //change inserted users  to 0
		{
            break;
        }
        bool bRet = CGroupMessageModel::getInstance()->resetMsgId(nGroupId);
        if(!bRet)
        {
            log("reset msgId failed. groupId=%u", nGroupId);
        }
        
        //insert IMGroupMember
        clearGroupMember(nGroupId);
        insertNewMember(nGroupId, setMember);
        
    } while (false);
    
    return nGroupId;
}

bool CGroupModel::removeGroup(uint32_t nUserId, uint32_t nGroupId, list<uint32_t>& lsCurUserId, set<uint32_t>& setGroupUsers)
{
    bool bRet = false;
    CDBManager* pDBManager = CDBManager::getInstance();
    CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
    //set<uint32_t> setGroupUsers;
    if(pDBConn)
    {
        string strSql = "select creator from IMGroup where id="+int2string(nGroupId);
        CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
        if(pResultSet)
        {
            uint32_t nCreator;
            while (pResultSet->Next()) {
                nCreator = pResultSet->GetInt("creator");
            }
            
            if(0 == nCreator || nCreator == nUserId)
            {
                //设置群组不可用。
                //strSql = "update IMGroup a, IMGroupMember b set a.status=1,usercnt = 0, a.updated=unix_timestamp(), version=version+1,b.status=1,b.updated=a.updated where a.id = b.groupid and a.id="+int2string(nGroupId);
				strSql = "update IMGroup set status=1,updated=unix_timestamp() where id ="+int2string(nGroupId);
                bRet = pDBConn->ExecuteUpdate(strSql.c_str());
            }
            delete  pResultSet;
        }
        
        if (bRet) {
            strSql = "select userId from IMGroupMember where groupId="+int2string(nGroupId);
			log("SQL for getting group member; %s", strSql.c_str());
            CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
            if(pResultSet)
            {
                while (pResultSet->Next()) {
                    uint32_t nId = pResultSet->GetInt("userId");
                    setGroupUsers.insert(nId);
                }
                delete pResultSet;
            }
        }
        pDBManager->RelDBConn(pDBConn);
    }
    
    if(bRet)
    {
		incGroupVersion(nGroupId);
        bRet = removeMember(nGroupId, setGroupUsers, lsCurUserId);
    }
    
    return bRet;
}


//void CGroupModel::getUserGroup(uint32_t nUserId, list<IM::BaseDefine::GroupVersionInfo>& lsGroup, uint32_t nGroupType)
//void CGroupModel::getUserGroup(uint32_t nUserId, list<IM::BaseDefine::GroupVersionInfo>& lsGroup, uint32_t nGroupType, uint32_t unLatestUpdateTime)
void CGroupModel::getUserGroup(uint32_t nUserId, list<IM::BaseDefine::GroupVersionInfo>& lsGroup, uint32_t unLatestUpdateTime)
{
    list<uint32_t> lsGroupId;
    getUserGroupIds(nUserId, lsGroupId,unLatestUpdateTime, 0);
    if(lsGroupId.size() != 0)
    {
        //getGroupVersion(lsGroupId, lsGroup, nGroupType);
		getGroupVersion(lsGroupId, lsGroup);
    }
}


void CGroupModel::getGroupInfo(map<uint32_t, IM::BaseDefine::GroupVersionInfo>& mapGroupId, list<IM::BaseDefine::GroupInfo>& lsGroupInfo, uint32_t unUserID)
{
    if (!mapGroupId.empty())
    {
        CDBManager* pDBManager = CDBManager::getInstance();
        CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
        if (pDBConn)
        {
            string strClause;
            bool bFirst = true;
            for(auto it=mapGroupId.begin(); it!=mapGroupId.end(); ++it)
            {
                if(bFirst)
                {
                    bFirst = false;
                    strClause = int2string(it->first);
                }
                else
                {
                    strClause += ("," + int2string(it->first));
                }
            }

			string strSql = "";
			//get all groups info including deleted group. changed by george on Sep 17th,2015
			if (unUserID)
			{
				strSql = "select a.id,version,name,avatar,type,creator,shieldstatus,email,a.groupid,a.status, companyid from IMGroup a, IMGroupMember b where a.id=b.groupid and a.id in (" + strClause + ") and b.userid=" + int2string(unUserID) + " and b.status = 0 order by a.updated desc";
				//strSql = "select a.id,version,name,avatar,type,creator,shieldstatus,email,a.groupid,companyid from IMGroup a, IMGroupMember b where a.status = 0 and a.id in (" + strClause + ") and b.userid=" + int2string(unUserID) + " and b.status = 0 and a.id=b.groupid order by a.updated desc";
				//strSql = "select a.id,version,name,avatar,type,creator,shieldstatus,a.status from IMGroup a, IMGroupMember b where a.id in (" + strClause + ") and b.userid=" + int2string(unUserID) + " and a.id=b.groupid order by a.updated desc";			
			}
			else
			{
				strSql = "select id,version,name,avatar,type,creator,email,groupid,status, companyid from IMGroup where id in (" + strClause + ")  order by updated desc";
				//strSql = "select id,version,name,avatar,type,creator,email,groupid,companyid from IMGroup where status = 0 and id in (" + strClause + ")  order by updated desc";
				//strSql = "select id,version,name,avatar,type,creator,status from IMGroup where id in (" + strClause + ")  order by updated desc";
			}
			log("Sql for getting group info: %s", strSql.c_str());
            CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
            if(pResultSet)
            {
                while (pResultSet->Next()) {
                    uint32_t nGroupId = pResultSet->GetInt("id");
                    uint32_t nVersion = pResultSet->GetInt("version");
                    //if(mapGroupId[nGroupId].version() < nVersion)
					if (mapGroupId[nGroupId].version() <= nVersion) //george jiang change the judgement(version comparision) on July 7th, 2015
                    {
                        IM::BaseDefine::GroupInfo cGroupInfo;
                        cGroupInfo.set_group_id(nGroupId);
                        cGroupInfo.set_version(nVersion);
						cGroupInfo.set_status(pResultSet->GetInt("status")); //george added on Sep 17th,2015
						string strName = "";
						if (pResultSet->GetString("name") != NULL)
						{
							strName = pResultSet->GetString("name");
						}
                        cGroupInfo.set_group_name(strName);

						string strAvatar = "";
						if (pResultSet->GetString("avatar") != NULL)
						{
							strAvatar = pResultSet->GetString("avatar");
						}
                        cGroupInfo.set_group_avatar(strAvatar);

						string strEmail;
						if (pResultSet->GetString("email") == NULL)
						{
							//log("Yes, NULL value");
							strEmail = "";
						}
						else
						{
							//log("No,value............:%s", strEmail.c_str());
							strEmail = pResultSet->GetString("email");
						}
						cGroupInfo.set_email(strEmail);

						if (unUserID)
						{
							cGroupInfo.set_shield_status(pResultSet->GetInt("shieldstatus"));
						}
						else //system
						{
							cGroupInfo.set_shield_status(0);
						}

						string strGroupId = "";
						if (pResultSet->GetString("groupid") != NULL)
						{
							strGroupId = pResultSet->GetString("groupid");
						}
						cGroupInfo.set_group_uuid(strGroupId);
						cGroupInfo.set_company_id(pResultSet->GetInt("companyid"));

                        IM::BaseDefine::GroupType nGroupType = IM::BaseDefine::GroupType(pResultSet->GetInt("type"));
                        if(IM::BaseDefine::GroupType_IsValid(nGroupType))
                        {
                            cGroupInfo.set_group_type(nGroupType);
                            cGroupInfo.set_group_creator_id(pResultSet->GetInt("creator"));
                            lsGroupInfo.push_back(cGroupInfo);
                        }
                        else
                        {
                            log("invalid groupType. groupId=%u, groupType=%u", nGroupId, nGroupType);
                        }
                    }
                }
                delete pResultSet;
            }
            else
            {
                log("no result set for sql:%s", strSql.c_str());
            }
            pDBManager->RelDBConn(pDBConn);
            if(!lsGroupInfo.empty())
            {
                fillGroupMember(lsGroupInfo); //maybe some problems occur due to no operating on db,just in redis.
            }
        }
        else
        {
            log("no db connection for teamtalk_slave");
        }
    }
    else
    {
        log("no ids in map");
    }
}

bool CGroupModel::modifyGroupMember(uint32_t nUserId, uint32_t nGroupId, IM::BaseDefine::GroupModifyType nType, set<uint32_t>& setUserId, list<uint32_t>& lsCurUserId, string& strGroupName, uint32_t& nCreator)
{
    bool bRet = false;
	//uint32_t nCreator = 0;

	//george jiang inserted the bracket on July 7th,2015
	switch (nType)
	{
		case IM::BaseDefine::GROUP_MODIFY_TYPE_ADD:
			log("Adding one or more members");
			if (hasModifyPermission(nUserId, nGroupId,false))
			{
				bRet = addMember(nGroupId, setUserId, lsCurUserId);
				ResetSession(nGroupId, setUserId);//for Adding removed members
			}
			else
			{
				log("user:%u has no permission to add members in group:%u", nUserId, nGroupId);
			}
			break;

		case IM::BaseDefine::GROUP_MODIFY_TYPE_DEL:
			log("Removing one or more members");
			if (hasModifyPermission(nUserId, nGroupId, nType, nCreator, strGroupName))
			{
				bRet = removeMember(nGroupId, setUserId, lsCurUserId);
				removeSession(nGroupId, setUserId);
				for (auto it = setUserId.begin(); it != setUserId.end(); it++)
				{
					CUserModel::getInstance()->clearUserCounter(*it, nGroupId, IM::BaseDefine::SESSION_TYPE_GROUP);
				}
			}
			else
			{
				log("user:%u has no permission to modify group:%u", nUserId, nGroupId);
			}  
			break;

		case IM::BaseDefine::GROUP_MODIFY_TYPE_DEL_BY_SELF:  //george.jiang inserted the case on July 8th,2015
			bRet = removeMember(nGroupId, setUserId, lsCurUserId);
			removeSession(nGroupId, setUserId);
			break;

		default:
			log("unknown type:%u while modify group.%u->%u", nType, nUserId, nGroupId);
			break;   
     }

    //if modify group member success, need to inc the group version and clear the user count;
    if(bRet)
    {
        //incGroupVersion(nGroupId);
        for (auto it=setUserId.begin(); it!=setUserId.end(); ++it) 
		{
            uint32_t nUserId=*it;
            CUserModel::getInstance()->clearUserCounter(nUserId, nGroupId, IM::BaseDefine::SESSION_TYPE_GROUP);
        }
    }
    //ending of inserting on July 7th, 2015

	//original procedure for adding and deleting a member
	//if (hasModifyPermission(nUserId, nGroupId, nType, nCreator))
	//{
	//	switch (nType) {
	//	case IM::BaseDefine::GROUP_MODIFY_TYPE_ADD:
	//		bRet = addMember(nGroupId, setUserId, lsCurUserId);
	//		break;
	//	case IM::BaseDefine::GROUP_MODIFY_TYPE_DEL:
	//		bRet = removeMember(nGroupId, setUserId, lsCurUserId);
	//		removeSession(nGroupId, setUserId);
	//		break;
	//	default:
	//		log("unknown type:%u while modify group.%u->%u", nType, nUserId, nGroupId);
	//		break;
	//	}
	//	//if modify group member success, need to inc the group version and clear the user count;
	//	if (bRet)
	//	{
	//		incGroupVersion(nGroupId);
	//		for (auto it = setUserId.begin(); it != setUserId.end(); ++it)
	//		{
	//			uint32_t nUserId = *it;
	//			CUserModel::getInstance()->clearUserCounter(nUserId, nGroupId, IM::BaseDefine::SESSION_TYPE_GROUP);
	//		}
	//	}
	//}
	//else
	//{
	//	log("user:%u has no permission to modify group:%u", nUserId, nGroupId);
	//}  
	return bRet;
}

bool CGroupModel::insertNewGroup(uint32_t nUserId, const string& strGroupName, const string& strGroupAvatar, uint32_t nGroupType, uint32_t nMemberCnt, uint32_t& nGroupId, string& strGroupId, uint32_t unCompanyId)
{
    bool bRet = false;
    nGroupId = INVALID_VALUE;
    CDBManager* pDBManager = CDBManager::getInstance();
    CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
    if (pDBConn)
    {
        string strSql = "insert into IMGroup(`name`, `avatar`, `creator`, `type`,`userCnt`, `status`, `version`, `lastChated`, `updated`, `created`, groupid, companyid) "\
        "values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
        
        CPrepareStatement* pStmt = new CPrepareStatement();
        if (pStmt->Init(pDBConn->GetMysql(), strSql))
        {
            uint32_t nCreated = (uint32_t)time(NULL);
            uint32_t index = 0;
            uint32_t nStatus = 0;
            uint32_t nVersion = 1;
            uint32_t nLastChat = 0;
            pStmt->SetParam(index++, strGroupName);
            pStmt->SetParam(index++, strGroupAvatar);
            pStmt->SetParam(index++, nUserId);
            pStmt->SetParam(index++, nGroupType);
            pStmt->SetParam(index++, nMemberCnt);
            pStmt->SetParam(index++, nStatus);
            pStmt->SetParam(index++, nVersion);
            pStmt->SetParam(index++, nLastChat);
            pStmt->SetParam(index++, nCreated);
            pStmt->SetParam(index++, nCreated);
			pStmt->SetParam(index++, strGroupId);
			pStmt->SetParam(index++, unCompanyId);
            
            bRet = pStmt->ExecuteUpdate();
            if(bRet) {
                nGroupId = pStmt->GetInsertId();
            }
        }
        delete pStmt;
        pDBManager->RelDBConn(pDBConn);
    }
    else
    {
        log("no db connection for teamtalk_master");
    }
    return bRet;
}

bool CGroupModel::insertNewMember(uint32_t nGroupId, set<uint32_t>& setUsers)
{
    bool bRet = false;
    uint32_t nUserCnt = (uint32_t)setUsers.size();
    if(nGroupId != INVALID_VALUE &&  nUserCnt > 0)
    {
        CDBManager* pDBManager = CDBManager::getInstance();
        CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
        if (pDBConn)
        {
            uint32_t nCreated = (uint32_t)time(NULL);
            // 获取 已经存在群里的用户
            string strClause;
            bool bFirst = true;
            for (auto it=setUsers.begin(); it!=setUsers.end(); ++it)
            {
                if(bFirst)
                {
                    bFirst = false;
                    strClause = int2string(*it);
                }
                else
                {
                    strClause += ("," + int2string(*it));
                }
            }

			//get existing user in the group
            string strSql = "select userId from IMGroupMember where groupId=" + int2string(nGroupId) + " and userId in (" + strClause + ")";
			log("Sql for getting existing users in the group:%s", strSql.c_str());
            CResultSet* pResult = pDBConn->ExecuteQuery(strSql.c_str());
            set<uint32_t> setHasUser;
            if(pResult)
            {
                while (pResult->Next()) {
                    setHasUser.insert(pResult->GetInt("userId"));
                }
                delete pResult;
				pResult = NULL;
            }
            else
            {
                log("no result for sql:%s", strSql.c_str());
            }

			//get deleted users in the company. george jiang added on Oct 16th, 2015
			set<uint32_t> setHasDeletedUser;
			//strSql = "select id from IMUser where id in (" + strClause + ") and status <> 0";
			strSql = "select id from IMUser where id in (" + strClause + ") and status < 0";  //updated on Nov 2nd,2015 
			pResult = pDBConn->ExecuteQuery(strSql.c_str());
			log("Sql for getting abnormal users when add members to the a group:%s", strSql.c_str());

			if (pResult)
			{
				while (pResult->Next())
				{
					setHasDeletedUser.insert(pResult->GetInt("id"));
				}
				delete pResult;
				pResult = NULL;
			}
			else
			{
				log("no result for sql:%s", strSql.c_str());
			}
			
            pDBManager->RelDBConn(pDBConn);
            
            pDBConn = pDBManager->GetDBConn("teamtalk_master");
            if (pDBConn)
            {
                CacheManager* pCacheManager = CacheManager::getInstance();
                CacheConn* pCacheConn = pCacheManager->GetCacheConn("group_member");
				if (pCacheConn)
				{
					// 设置已经存在群中人的状态
					if (!setHasUser.empty())
					{
						strClause.clear();
						bFirst = true;
						for (auto it = setHasUser.begin(); it != setHasUser.end(); ++it) {
							if (bFirst)
							{
								bFirst = false;
								strClause = int2string(*it);
							}
							else
							{
								strClause += ("," + int2string(*it));
							}
						}

						//change create time here? 
						strSql = "update IMGroupMember set status=0, updated=" + int2string(nCreated) + " where groupId=" + int2string(nGroupId) + " and userId in (" + strClause + ")";
						pDBConn->ExecuteUpdate(strSql.c_str());
					}

					strSql = "insert into IMGroupMember(`groupId`, `userId`, `status`, `created`, `updated`) values\
							                     (?,?,?,?,?)";

					log("Sql for adding a new member: %s", strSql.c_str());
					log("to be inserted users: %d, existing users: %d, abnormal users: %d", setUsers.size(), setHasUser.size(), setHasDeletedUser.size());

					//插入新成员
					auto it = setUsers.begin();
					uint32_t nStatus = 0;
					uint32_t nIncMemberCnt = 0;
					int nJoins = setUsers.size();
					int nDeleted = setHasDeletedUser.size();

					if (nJoins == nDeleted)
					{
						setUsers.clear();
					}
					else
					{
						for (; it != setUsers.end();)
						{
							uint32_t nUserId = *it;
							if (nUserId <= 10000) //reserved system named user could not be joined to any group.george added bhe bracket on Dec 30th,2015
							{
								++it;
								continue;
							}
							//george change the judgement on Oct 16th,2015
							set<uint32_t>::iterator it1 = setHasUser.find(nUserId);
							set<uint32_t>::iterator it2 = setHasDeletedUser.find(nUserId);
							if (it1 == setHasUser.end() && it2 == setHasDeletedUser.end())
							{
								CPrepareStatement* pStmt = new CPrepareStatement();
								if (pStmt->Init(pDBConn->GetMysql(), strSql))
								{
									uint32_t index = 0;
									pStmt->SetParam(index++, nGroupId);
									pStmt->SetParam(index++, nUserId);
									pStmt->SetParam(index++, nStatus);
									pStmt->SetParam(index++, nCreated);
									pStmt->SetParam(index++, nCreated);
									pStmt->ExecuteUpdate();
									++nIncMemberCnt;
									delete pStmt;
								}
								else
								{
									setUsers.erase(it++);
									delete pStmt;
									continue;
								}
								++it;
							}
							else if (it2 != setHasDeletedUser.end())  //deleted user
							{
								auto it3 = it;
								++it;
								setUsers.erase(it3);
								log("DELETED USER %d could not be added", nUserId);
							}
							else
							{
								++it;
								log("EXISING USER: %d should be reset status", nUserId);
							}
						}

						if (nIncMemberCnt != 0 || setHasUser.size() > 0)
						{
							int nUserCount = setHasUser.size();
							strSql = "update IMGroup set updated = unix_timestamp(),userCnt=userCnt+" + int2string(nIncMemberCnt) + "+" + int2string(nUserCount) + " where id=" + int2string(nGroupId);
							pDBConn->ExecuteUpdate(strSql.c_str());
						}

						//更新一份到redis中
						string strKey = "group_member_" + int2string(nGroupId);
						for (auto it = setUsers.begin(); it != setUsers.end(); ++it)
						{
							pCacheConn->hset(strKey, int2string(*it), int2string(nCreated));
						}
						pCacheManager->RelCacheConn(pCacheConn);
						bRet = true;
					}
				}						
                else
                {
                    log("no cache connection");
                }
                pDBManager->RelDBConn(pDBConn);
            }
            else
            {
                log("no db connection for teamtalk_master");
            }
        }
        else
        {
            log("no db connection for teamtalk_slave");
        }
    }
	else
	{
		log("Invalid group id: %d or invalid user set: %d", nGroupId, nUserCnt);
	}
    return bRet;
}

//void CGroupModel::getUserGroupIds(uint32_t nUserId, list<uint32_t>& lsGroupId, uint32_t nLimited)
void CGroupModel::getUserGroupIds(uint32_t nUserId, list<uint32_t>& lsGroupId, uint32_t unLatestUpdateTime, uint32_t nLimited)
{
    CDBManager* pDBManager = CDBManager::getInstance();
    CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
    if(pDBConn)
    {
        string strSql ;
        if (nLimited != 0)
		{
			if (unLatestUpdateTime != 0)
			{
				strSql = "select groupId from IMGroupMember where userId=" + int2string(nUserId) + " and updated > " + int2string(unLatestUpdateTime) + " order by updated desc, id desc limit " + int2string(nLimited);
				//strSql = "select groupId from IMGroupMember where userId=" + int2string(nUserId) + " and status = 0 and updated > " + int2string(unLatestUpdateTime) + " order by updated desc, id desc limit " + int2string(nLimited);
				//strSql = "select groupId from IMGroupMember where userId=" + int2string(nUserId) + " order by updated desc, id desc limit " + int2string(nLimited);
			}
			else
			{
				strSql = "select groupId from IMGroupMember where userId=" + int2string(nUserId) + " and status = 0 group by groupId order by updated desc, id desc limit " + int2string(nLimited);
				//strSql = "select groupId from IMGroupMember where userId=" + int2string(nUserId) + " order by updated desc, id desc limit " + int2string(nLimited);
			}
        }
        else
        {
			if (unLatestUpdateTime != 0)
			{
				//strSql = "select groupId from IMGroupMember where userId=" + int2string(nUserId) + " order by updated desc, id desc";
				strSql = "select groupId from IMGroupMember where userId=" + int2string(nUserId) + "  and updated > " + int2string(unLatestUpdateTime) + " order by updated desc, id desc";
				//strSql = "select groupId from IMGroupMember where userId=" + int2string(nUserId) + " and status = 0  and updated > " + int2string(unLatestUpdateTime) + " order by updated desc, id desc";
			}
			else
			{
				//strSql = "select groupId from IMGroupMember where userId=" + int2string(nUserId) + " order by updated desc, id desc";
				strSql = "select groupId from IMGroupMember where userId=" + int2string(nUserId) + " and status = 0 group by groupId order by updated desc, id desc";
			}
        }
        
        CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
        if(pResultSet)
        {
            while(pResultSet->Next())
            {
                uint32_t nGroupId = pResultSet->GetInt("groupId");
                lsGroupId.push_back(nGroupId);
            }
            delete pResultSet;
        }
        else
        {
            log("no result set for sql:%s", strSql.c_str());
        }
        pDBManager->RelDBConn(pDBConn);

		log("SQL for selecting group ids: %s, Count: %d", strSql.c_str(), lsGroupId.size());
    }
    else
    {
        log("no db connection for teamtalk_slave");
    }
}

//void CGroupModel::getGroupVersion(list<uint32_t> &lsGroupId, list<IM::BaseDefine::GroupVersionInfo> &lsGroup, uint32_t nGroupType)
//ignoring group type
void CGroupModel::getGroupVersion(list<uint32_t>&lsGroupId, list<IM::BaseDefine::GroupVersionInfo>& lsGroup)
{
    if(!lsGroupId.empty())
    {
        CDBManager* pDBManager = CDBManager::getInstance();
        CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
        if(pDBConn)
        {
            string strClause;
            bool bFirst = true;
            for(list<uint32_t>::iterator it=lsGroupId.begin(); it!=lsGroupId.end(); ++it)
            {
                if(bFirst)
                {
                    bFirst = false;
                    strClause = int2string(*it);
                }
                else
                {
                    strClause += ("," + int2string(*it));
                }
            }
            
           string strSql = "select id,version from IMGroup where status =0 and id in (" +  strClause  + ")";
			//string strSql = "select id,version from IMGroup where id in (" + strClause + ")";
           /* if(0 != nGroupType)
            {
                strSql += " and type="+int2string(nGroupType);
            }*/
            strSql += " order by updated desc";
            
            CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
            if(pResultSet)
            {
                while(pResultSet->Next())
                {
                    IM::BaseDefine::GroupVersionInfo group;
                    group.set_group_id(pResultSet->GetInt("id"));
                    group.set_version(pResultSet->GetInt("version"));
                    lsGroup.push_back(group);
                }
                delete pResultSet;
            }
            else
            {
                log("no result set for sql:%s", strSql.c_str());
            }
            pDBManager->RelDBConn(pDBConn);

			log("Sql for selecting version id: %s, Count: %d", strSql.c_str(), lsGroup.size());
        }
        else
        {
            log("no db connection for teamtalk_slave");
        }
    }
    else
    {
        log("group ids is empty");
    }
}

bool CGroupModel::isInGroup(uint32_t nUserId, uint32_t nGroupId)
{
	if (nUserId <= MAX_SYSTEM_USER_ID)
	{
		return true;
	}

    bool bRet = false;
    CacheManager* pCacheManager = CacheManager::getInstance();
    CacheConn* pCacheConn = pCacheManager->GetCacheConn("group_member");
    if (pCacheConn)
    {
        string strKey = "group_member_" + int2string(nGroupId);
        string strField = int2string(nUserId);
        string strValue = pCacheConn->hget(strKey, strField);
        pCacheManager->RelCacheConn(pCacheConn);
        if(!strValue.empty())
        {
            bRet = true;
        }
    }
    else
    {
        log("no cache connection for group_member");
    }
    return bRet;
}

bool CGroupModel::hasModifyPermission(uint32_t nUserId, uint32_t nGroupId, IM::BaseDefine::GroupModifyType nType,uint32_t &nCreate)
{
	//george commented the judgement on Aug 27th,2015
   /* if(nUserId == 0) {
        return true;
    }*/
    
    bool bRet = false;
    CDBManager* pDBManager = CDBManager::getInstance();
    CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
    if(pDBConn)
    {
        string strSql = "select creator,type from IMGroup where status = 0 and id="+ int2string(nGroupId);
        CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
        if(pResultSet)
        {
            while (pResultSet->Next())
            {
                nCreate = pResultSet->GetInt("creator");
                IM::BaseDefine::GroupType nGroupType = IM::BaseDefine::GroupType(pResultSet->GetInt("type"));
                if(IM::BaseDefine::GroupType_IsValid(nGroupType))
                {
                    if(IM::BaseDefine::GROUP_TYPE_TMP == nGroupType /*&& IM::BaseDefine::GROUP_MODIFY_TYPE_ADD == nType*/)
                    {
						//nCreate = nCreator;
                        bRet = true;
                        break;
                    }
					else if (IM::BaseDefine::GROUP_TYPE_EMAIL == nGroupType)
					{
						//nCreate = nCreator;
						bRet = false;
						break;
					}
                    else
                    {
                        if(nCreate == nUserId)
                        {
							//nCreate = nCreator;
                            bRet = true;
                            break;
                        }
                    }
                }
            }
            delete pResultSet;
        }
        else
        {
            log("no result for sql:%s", strSql.c_str());
        }
        pDBManager->RelDBConn(pDBConn);
    }
    else
    {
        log("no db connection for teamtalk_slave");
    }
    return bRet;
}


//george.jiang added the member function on June 23rd,2015
bool CGroupModel::hasModifyPermission(uint32_t nUserId, uint32_t nGroupId, bool bCreator)
{
	if (nUserId == 0) {
		return true;
	}

	bool bRet = false;
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		if (bCreator)
		{
			string strSql = "select creator, type from IMGroup where status = 0 and id=" + int2string(nGroupId);
			CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
			if (pResultSet)
			{
				while (pResultSet->Next())
				{
					uint32_t nCreator = pResultSet->GetInt("creator");
					IM::BaseDefine::GroupType nGroupType = IM::BaseDefine::GroupType(pResultSet->GetInt("type"));
					if (IM::BaseDefine::GroupType_IsValid(nGroupType))
					{
						if (IM::BaseDefine::GROUP_TYPE_TMP == nGroupType /*&& IM::BaseDefine::GROUP_MODIFY_TYPE_ADD == nType*/)
						{
							//nCreate = nCreator;
							bRet = true;
							break;
						}
						else if (IM::BaseDefine::GROUP_TYPE_EMAIL == nGroupType)
						{
							bRet = false;
							break;
						}
						else
						{
							if (nCreator == nUserId)
							{
								bRet = true;
								break;
							}
						}
					}
				}
				delete pResultSet;
			}
			else
			{
				log("no result for sql:%s", strSql.c_str());
			}
		}
		else
		{
			string strSql = "select type,b.status from IMGroup a, IMGroupMember b where b.groupId = " + int2string(nGroupId) + " and userId=" + int2string(nUserId) + " and a.id=b.groupid and a.status=0";
			int nStatus = 1;
			int nGroupType = 1;
			log("Sql for selecting a user's status: %s", strSql.c_str());

			CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
			if (pResultSet)
			{
				while (pResultSet->Next())
				{
					nStatus = pResultSet->GetInt("status");
					nGroupType = pResultSet->GetInt("type");
					if (IM::BaseDefine::GroupType_IsValid(nGroupType))
					{
						if (IM::BaseDefine::GROUP_TYPE_TMP == nGroupType /*&& IM::BaseDefine::GROUP_MODIFY_TYPE_ADD == nType*/)
						{
							//nCreate = nCreator;
							bRet = true;
							break;
						}
						else if (IM::BaseDefine::GROUP_TYPE_EMAIL == nGroupType)
						{
							bRet = false;
							break;
						}
						else
						{
							if (!nStatus)
							{
								bRet = true;
							}
							break;
						}
					}
				}
				delete pResultSet;
				pResultSet = NULL;
			}
			else
			{
				log("no result for sql:%s", strSql.c_str());
			}
		}
		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_slave");
	}
	return bRet;
}


bool CGroupModel::hasModifyPermission(uint32_t nUserId, uint32_t nGroupId, IM::BaseDefine::GroupModifyType nType, uint32_t &nCreator, string& strGroupName, bool bCreator)
{
	//george commented the judgement on Aug 27th,2015. As a system message ,we need creator and group name
	/*if (nUserId == 0) {
		return true;
	}*/

	bool bRet = false;
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		if (bCreator) //if the operator is the group creator
		{
			string strSql = "select creator,name,type from IMGroup where status = 0 and id=" + int2string(nGroupId);
			CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
			if (pResultSet)
			{
				while (pResultSet->Next())
				{
					nCreator = pResultSet->GetInt("creator");
					strGroupName = pResultSet->GetString("name");
					IM::BaseDefine::GroupType nGroupType = IM::BaseDefine::GroupType(pResultSet->GetInt("type"));
					if (IM::BaseDefine::GroupType_IsValid(nGroupType))
					{
						if (IM::BaseDefine::GROUP_TYPE_TMP == nGroupType /*&& IM::BaseDefine::GROUP_MODIFY_TYPE_ADD == nType*/)
						{
							//nCreator = nOwner;
							bRet = true;
							break;
						}
						else if (IM::BaseDefine::GROUP_TYPE_EMAIL == nGroupType)
						{
							bRet = false;
							break;
						}
						else
						{
							if (nCreator == nUserId)
							{
								bRet = true;
								break;
							}
						}
					}
				}
				delete pResultSet;
			}
			else
			{
				log("no result for sql:%s", strSql.c_str());
			}
		}
		else
		{
			string strSql = "select creator,name,type,b.status from IMGroup a, IMGroupMember b where a.status = 0 and a.id=" + int2string(nGroupId) + " and b.userid = " + int2string(nUserId) + " and a.id=b.groupid";
			CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
			if (pResultSet)
			{
				while (pResultSet->Next())
				{
					nCreator = pResultSet->GetInt("creator");
					strGroupName = pResultSet->GetString("name");
					IM::BaseDefine::GroupType nGroupType = IM::BaseDefine::GroupType(pResultSet->GetInt("type"));
					int nStatus = pResultSet->GetInt("status");
					if (IM::BaseDefine::GroupType_IsValid(nGroupType))
					{
						if (IM::BaseDefine::GROUP_TYPE_TMP == nGroupType /*&& IM::BaseDefine::GROUP_MODIFY_TYPE_ADD == nType*/)
						{
							//nCreator = nOwner;
							bRet = true;
							break;
						}
						else if (IM::BaseDefine::GROUP_TYPE_EMAIL == nGroupType)
						{
							bRet = false;
							break;
						}
						else
						{
							if (!nStatus)
							{
								bRet = true;
							}
							break;
						}
					}
				}
				delete pResultSet;
			}
			else
			{
				log("no result for sql:%s", strSql.c_str());
			}
		}
		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_slave");
	}
	return bRet;
}


bool CGroupModel::addMember(uint32_t nGroupId, set<uint32_t> &setUser, list<uint32_t>& lsCurUserId)
{
    // 去掉已经存在的用户ID
    removeRepeatUser(nGroupId, setUser);
    bool bRet = insertNewMember(nGroupId, setUser);
	if (bRet)
	{
		incGroupVersion(nGroupId);
	}
    getGroupUser(nGroupId,lsCurUserId);
    return bRet;
}

bool CGroupModel::removeMember(uint32_t nGroupId, set<uint32_t> &setUser, list<uint32_t>& lsCurUserId)
{
    if(setUser.size() <= 0)
    {
        return true;
    }

    bool bRet = false;
    CDBManager* pDBManager = CDBManager::getInstance();
    CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	//CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
    if(pDBConn)
    {
        CacheManager* pCacheManager = CacheManager::getInstance();
        CacheConn* pCacheConn = pCacheManager->GetCacheConn("group_member");
        if (pCacheConn)
        {
            string strClause ;
            bool bFirst = true;
            for(auto it= setUser.begin(); it!=setUser.end();++it)
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

			//george updated the following on Oct 30th,2015
			//get actual undeleted user from the group
			set<uint32_t> setNormalUser;
			string strSql = "select userId from IMGroupMember where groupid = " + int2string(nGroupId) + " and userid in (" + strClause + ") and status = 0";
			log("SQL for getting undeleted users: %s", strSql.c_str());
			CResultSet *pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
			if (pResultSet)
			{
				while (pResultSet->Next())
				{
					int nUserID = pResultSet->GetInt("userId");
					setNormalUser.insert(nUserID);
				}
				delete pResultSet;
				pResultSet = NULL;
			}
			pDBManager->RelDBConn(pDBConn);

			log("DELETED SIZE: %d, NORMAL SIZE: %d", setUser.size(), setNormalUser.size());

			if (setUser.size() > setNormalUser.size()) //some required users should be deleted from the list
			{
				for (auto it = setUser.begin(); it != setUser.end();)
				{
					int nCurrUserID = *it;
					if (setNormalUser.find(nCurrUserID) == setNormalUser.end())
					{
						auto currNode = it;
						++it;
						setUser.erase(currNode);
						log("REQUIRED USER %d NO EXISTS IN THE GROUP: %d, DELETING.....", nCurrUserID, nGroupId);
					}
					else
					{
						++it;
					}
				}
			}

			if (setUser.size() > 0) //after filtering,some members should be removed
			{
				pDBConn = pDBManager->GetDBConn("teamtalk_master"); //connect to master db
				if (pDBConn)
				{
					//updating field 'updated' when removing a user from a group(status is set to 1).updated by george jiang on Aug 6th,2015
					uint32_t nUserCount = setUser.size();
					strSql = "update IMGroupMember a, IMGroup b set a.status=1,a.updated=unix_timestamp(),b.updated=unix_timestamp(),b.usercnt = b.usercnt-" + int2string(nUserCount) + " where  a.groupId =" + int2string(nGroupId) + " and a.userId in(" + strClause + ") and a.groupid=b.id and b.usercnt >= " + int2string(nUserCount);
					log("SQL for removing members: %s", strSql.c_str());
					pDBConn->ExecuteUpdate(strSql.c_str());

					//从redis中删除成员
					string strKey = "group_member_" + int2string(nGroupId);
					for (auto it = setUser.begin(); it != setUser.end(); ++it)
					{
						string strField = int2string(*it);
						pCacheConn->hdel(strKey, strField);
					}
					pDBManager->RelDBConn(pDBConn);
					bRet = true;
				}
				else
				{
					log("no db connection for teamtalk_master");
				}
			}
			else
			{
				bRet = true;
			}

			//end of updating on Oct 30th,2015
            pCacheManager->RelCacheConn(pCacheConn);
            
        }
        else
        {
            log("no cache connection");
        }
        pDBManager->RelDBConn(pDBConn);

        if (bRet)
        {
            getGroupUser(nGroupId,lsCurUserId);
			incGroupVersion(nGroupId);
        }
    }
    else
    {
        log("no db connection for teamtalk_slave");
    }
    return bRet;
}

void CGroupModel::removeRepeatUser(uint32_t nGroupId, set<uint32_t> &setUser)
{
    CacheManager* pCacheManager = CacheManager::getInstance();
    CacheConn* pCacheConn = pCacheManager->GetCacheConn("group_member");
    if (pCacheConn)
    {
        string strKey = "group_member_"+int2string(nGroupId);
        for (auto it=setUser.begin(); it!=setUser.end();) {
            string strField = int2string(*it);
            string strValue = pCacheConn->hget(strKey, strField);
            pCacheManager->RelCacheConn(pCacheConn);
            if(!strValue.empty())
            {
                setUser.erase(it++);
            }
            else
            {
                ++it;
            }
        }
    }
    else
    {
        log("no cache connection for group_member");
    }
}

bool CGroupModel::setPush(uint32_t nUserId, uint32_t nGroupId, uint32_t nType, uint32_t nStatus)
{
    bool bRet = false;
    if(!isInGroup(nUserId, nGroupId))
    {
        log("user:%d is not in group:%d", nUserId, nGroupId);
        return bRet;;
    }
    
    CacheManager* pCacheManager = CacheManager::getInstance();
    CacheConn* pCacheConn = pCacheManager->GetCacheConn("group_set");
    if(pCacheConn)
    {
        string strGroupKey = "group_set_" + int2string(nGroupId);
        string strField = int2string(nUserId) + "_" + int2string(nType);
        int nRet = pCacheConn->hset(strGroupKey, strField, int2string(nStatus));
        pCacheManager->RelCacheConn(pCacheConn);
        if(nRet != -1)
        {
            bRet = true;
        }
    }
    else
    {
        log("no cache connection for group_set");
    }
    return bRet;
}

void CGroupModel::getPush(uint32_t nGroupId, list<uint32_t>& lsUser, list<IM::BaseDefine::ShieldStatus>& lsPush)
{
    if (lsUser.empty()) {
        return;
    }
    CacheManager* pCacheManager = CacheManager::getInstance();
    CacheConn* pCacheConn = pCacheManager->GetCacheConn("group_set");
    if(pCacheConn)
    {
        string strGroupKey = "group_set_" + int2string(nGroupId);
        map<string, string> mapResult;
        bool bRet = pCacheConn->hgetAll(strGroupKey, mapResult);
        pCacheManager->RelCacheConn(pCacheConn);
		if (!bRet)
		{
			//get group shield status from databse
			list<uint32_t> lsGroupMember;
			CDBManager* pDBManager = CDBManager::getInstance();
			CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
			if (pDBConn)
			{
				string strSql = "select userid from IMGroupMember where groupid=" + int2string(nGroupId) + " and status=0 and shieldstatus=1";
				log("SQL for getting group shield status: %s", strSql.c_str());
				CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
				if (pResultSet)
				{
					while (pResultSet->Next())
					{
						uint32_t unUserId = pResultSet->GetInt("userid");
						lsGroupMember.push_back(unUserId);
					}

					delete pResultSet;
					pResultSet = NULL;
				}
				pDBManager->RelDBConn(pDBConn);
			}
			else
			{
				log("no db connection for teamtalk_master");
			}

			if (lsGroupMember.size() > 0)
			{
				bRet = true;

				for (auto it = lsGroupMember.begin(); it != lsGroupMember.end(); it++)
				{
					setPush(*it, nGroupId, IM_GROUP_SETTING_PUSH, 1);
				}
			}
		}

        if(bRet)
        {
            for(auto it=lsUser.begin(); it!=lsUser.end(); ++it)
            {
                string strField = int2string(*it) + "_" + int2string(IM_GROUP_SETTING_PUSH);
                auto itResult = mapResult.find(strField);
                IM::BaseDefine::ShieldStatus status;
                status.set_group_id(nGroupId);
                status.set_user_id(*it);
                if(itResult != mapResult.end())
                {
                    status.set_shield_status(string2int(itResult->second));
                }
                else
                {
                    status.set_shield_status(0);
                }
                lsPush.push_back(status);
            }
        }
        else
        {
            log("hgetall %s failed!", strGroupKey.c_str());
        }
    }
    else
    {
        log("no cache connection for group_set");
    }
}

void CGroupModel::getGroupUser(uint32_t nGroupId, list<uint32_t> &lsUserId)
{
    CacheManager* pCacheManager = CacheManager::getInstance();
    CacheConn* pCacheConn = pCacheManager->GetCacheConn("group_member");
    if (pCacheConn)
    {
        string strKey = "group_member_" + int2string(nGroupId);
        map<string, string> mapAllUser;
        bool bRet = pCacheConn->hgetAll(strKey, mapAllUser);
        pCacheManager->RelCacheConn(pCacheConn);
        if(bRet)
        {
            for (auto it=mapAllUser.begin(); it!=mapAllUser.end(); ++it) {
                uint32_t nUserId = string2int(it->first);
                lsUserId.push_back(nUserId);
            }
        }
        else
        {
            log("hgetall %s failed!", strKey.c_str());
        }
    }
    else
    {
        log("no cache connection for group_member");
    }
}

void CGroupModel::updateGroupChat(uint32_t nGroupId)
{
    CDBManager* pDBManager = CDBManager::getInstance();
    CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
    if(pDBConn)
    {
        uint32_t nNow = (uint32_t)time(NULL);
        string strSql = "update IMGroup set lastChated=" + int2string(nNow) + " where id=" + int2string(nGroupId);
        pDBConn->ExecuteUpdate(strSql.c_str());
        pDBManager->RelDBConn(pDBConn);
    }
    else
    {
        log("no db connection for teamtalk_master");
    }
}

//bool CGroupModel::isValidateGroupId(uint32_t nGroupId)
//{
//    bool bRet = false;
//    CDBManager* pDBManager = CDBManager::getInstance();
//    CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
//    if(pDBConn)
//    {
//        string strSql = "select id from IMGroup where id=" + int2string(nGroupId)+" and status=0";
//        CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
//        if(pResultSet && pResultSet->Next())
//        {
//            bRet =  true;
//            delete pResultSet;
//        }
//        pDBManager->RelDBConn(pDBConn);
//    }
//    else
//    {
//        log("no db connection for teamtalk_slave");
//    }
//    return bRet;
//}

bool CGroupModel::isValidateGroupId(uint32_t nGroupId)
{
    bool bRet = false;
    CacheManager* pCacheManager = CacheManager::getInstance();
    CacheConn* pCacheConn = pCacheManager->GetCacheConn("group_member");
    if(pCacheConn)
    {
        string strKey = "group_member_"+int2string(nGroupId);
        bRet = pCacheConn->isExists(strKey);
		if (!bRet) //no found in cache,we should query from database
		{
			CDBManager* pDBManager = CDBManager::getInstance();
			CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
			if (pDBConn)
			{
				string strSql = "select userid,created from IMGroupMember where status = 0 and groupid = " + int2string(nGroupId);
				//log("SQL for getting group members: %s", strSql.c_str());

				CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
				if (pResultSet)
				{
					//insert the user in a group into cached table
					
					while (pResultSet->Next())
					{
						pCacheConn->hset(strKey, int2string(pResultSet->GetInt("userid")), int2string(pResultSet->GetInt("created")));
						log("USER %d is joining the GROUP: %d in CACHE", pResultSet->GetInt("userid"), pResultSet->GetInt("created"));
					}

					delete pResultSet;
					pResultSet = NULL;
					bRet = true;
				}
				pDBManager->RelDBConn(pDBConn);
			}
		}

		pCacheManager->RelCacheConn(pCacheConn);
    }
    return bRet;
}


void CGroupModel::removeSession(uint32_t nGroupId, const set<uint32_t> &setUser)
{
    for(auto it=setUser.begin(); it!=setUser.end(); ++it)
    {
        uint32_t nUserId=*it;
        uint32_t nSessionId = CSessionModel::getInstance()->getSessionId(nUserId, nGroupId, IM::BaseDefine::SESSION_TYPE_GROUP, false);
        CSessionModel::getInstance()->removeSession(nSessionId);
		log("Removing session %d for user; %d in group %d", nSessionId, nUserId, nGroupId);
    }
}


void CGroupModel::ResetSession(uint32_t nGroupId, const set<uint32_t>& setUser)
{
	for (auto it = setUser.begin(); it != setUser.end(); ++it)
	{
		uint32_t unUserID = *it;
		CSessionModel::getInstance()->ResetSession(unUserID, nGroupId, IM::BaseDefine::SESSION_TYPE_GROUP);
	}
}



bool CGroupModel::incGroupVersion(uint32_t nGroupId)
{
    bool bRet = false;
    CDBManager* pDBManager = CDBManager::getInstance();
    CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
    if(pDBConn)
    {
        string strSql = "update IMGroup set version=version+1 where id="+int2string(nGroupId);
        if(pDBConn->ExecuteUpdate(strSql.c_str()))
        {
            bRet = true;
        }
        pDBManager->RelDBConn(pDBConn);
    }
    else
    {
        log("no db connection for teamtalk_master");
    }
    return  bRet;
}


void CGroupModel::fillGroupMember(list<IM::BaseDefine::GroupInfo>& lsGroups)
{
    for (auto it=lsGroups.begin(); it!=lsGroups.end(); ++it) {
        list<uint32_t> lsUserIds;
        uint32_t nGroupId = it->group_id();
        getGroupUser(nGroupId, lsUserIds);
        for(auto itUserId=lsUserIds.begin(); itUserId!=lsUserIds.end(); ++itUserId)
        {
            it->add_group_member_list(*itUserId);
        }
    }
}

uint32_t CGroupModel::getUserJoinTime(uint32_t nGroupId, uint32_t nUserId)
{
    uint32_t nTime = 0;
    CacheManager* pCacheManager = CacheManager::getInstance();
    CacheConn* pCacheConn = pCacheManager->GetCacheConn("group_member");
    if (pCacheConn)
    {
        string strKey = "group_member_" + int2string(nGroupId);
        string strField = int2string(nUserId);
        string strValue = pCacheConn->hget(strKey, strField);
        pCacheManager->RelCacheConn(pCacheConn);
        if (!strValue.empty()) {
            nTime = string2int(strValue);
        }
    }
    else
    {
        log("no cache connection for group_member");
    }
    return  nTime;
}

void CGroupModel::clearGroupMember(uint32_t nGroupId)
{
    CDBManager* pDBManager = CDBManager::getInstance();
    CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
    if(pDBConn)
    {
        //string strSql = "delete from IMGroupMember where groupId="+int2string(nGroupId);
		string strSql = "update IMGroupMember set status = 1 where groupId=" + int2string(nGroupId); //changed by george on Sep 17th
        pDBConn->ExecuteUpdate(strSql.c_str());
        pDBManager->RelDBConn(pDBConn);
    }
    else
    {
        log("no db connection for teamtalk_master");
    }
    CacheManager* pCacheManager = CacheManager::getInstance();
    CacheConn* pCacheConn = pCacheManager->GetCacheConn("group_member");
    if(pCacheConn)
    {
        string strKey = "group_member_" + int2string(nGroupId);
        map<string, string> mapRet;
        bool bRet = pCacheConn->hgetAll(strKey, mapRet);
        if(bRet)
        {
            for(auto it=mapRet.begin(); it!=mapRet.end(); ++it)
            {
                pCacheConn->hdel(strKey, it->first);
            }
        }
        else
        {
            log("hgetall %s failed", strKey.c_str());
        }
        pCacheManager->RelCacheConn(pCacheConn);
    }
    else
    {
        log("no cache connection for group_member");
    }
}


bool CGroupModel::updateGroupName(uint32_t nUserId, uint32_t nGroupId, const string& strGroupName, list<uint32_t>& lsCurUserId)
{
	 bool bRet = false;
	 uint32_t nCreator = 0;
    if(hasModifyPermission(nUserId, nGroupId))
    {
        CDBManager* pDBManager = CDBManager::getInstance();
		CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
		if(pDBConn)
		{
			//string strSql = "update IMGroup set name = '" + strGroupName + "'" + " where id=" + int2string(nGroupId);
			string strSql = "update IMGroup set name = ? where id= ?";  //'" + strGroupName + "'" + " where id = " + int2string(nGroupId);
			log("SQL for Updating GroupName: %s with parameters: new group name: %s, group id: %d", strSql.c_str(), strGroupName.c_str(), nGroupId);
			CPrepareStatement* pStmt = new CPrepareStatement();
			if (pStmt->Init(pDBConn->GetMysql(), strSql))
			{
				uint32_t index = 0;
				pStmt->SetParam(index++, strGroupName);
				pStmt->SetParam(index++, nGroupId);
				bRet = pStmt->ExecuteUpdate();
			}
			delete pStmt;

			if (!bRet)
			{
				log("failed to execute :%s", strSql.c_str());
			}
			pDBManager->RelDBConn(pDBConn);

			////to judge if the new groupname has already exists
			//string strSql = "select count(*) as  Totals from IMGroup where status = 0 and name= '" + strGroupName + "'";
			//log("Sql for testing records: %s", strSql.c_str());
			//CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
			//if(pResultSet)
			//{
			//	int nRecords = 0;
			//	while (pResultSet->Next())
			//	{
			//		nRecords = pResultSet->GetInt("Totals");
			//	}
			//	delete pResultSet;

			//	if (!nRecords)  //new groupname is not existed
			//	{
			//		strSql = "update IMGroup set name = '" + strGroupName + "'" + " where id=" + int2string(nGroupId);
			//		if (pDBConn->ExecuteUpdate(strSql.c_str()))
			//		{
			//			bRet = true;
			//			log("SQL for Updating GroupName: %s", strSql.c_str());
			//		}
			//	}
			//	pDBManager->RelDBConn(pDBConn);
			//}
			//else
			//{
			//	log("failed to execute :%s", strSql.c_str());
			//}
		}
		else
		{
			log("no db connection for teamtalk_master");
		}
        //if modify group name success, need to inc the group version and get effected users in the group(for notifying)
        if(bRet)
        {
			getGroupUser(nGroupId, lsCurUserId);
            incGroupVersion(nGroupId);
        }
    }
    else
    {
        log("user:%u has no permission to modify group:%u", nUserId, nGroupId);
    } 

	return bRet;
}


bool CGroupModel::updateGroupLeader(uint32_t nUserId, uint32_t nGroupId, uint32_t nNewUserId, list<uint32_t>& lsCurUserId)
{
	bool bRet = false;
	uint32_t nCreator = 0;
	uint32_t nChangeType = IM::BaseDefine::GROUP_MODIFY_TYPE_TRANSFER_LEADER;
	int bResult = hasModifyPermission(nUserId, nGroupId, (IM::BaseDefine::GroupModifyType)(nChangeType), nCreator);
    if(bResult || (!bResult && nCreator == nUserId))
    {
        CDBManager* pDBManager = CDBManager::getInstance();
		CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
		if(pDBConn)
		{
			//to judge if the new user id has already exists in the group
			string strSql = "select status from IMGroupMember where groupId = " + int2string(nGroupId) + " and userId=" + int2string(nNewUserId);
			int nStatus = 1;
			log("Sql for selecting a user's status: %s", strSql.c_str());
			
			CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
			if (pResultSet)
			{
				while (pResultSet->Next())
				{
					nStatus = pResultSet->GetInt("status");
				}
				delete pResultSet;
				pResultSet = NULL;
			}

			if (!nStatus)
			{
				strSql = "update IMGroup set creator = " + int2string(nNewUserId) + " where id=" + int2string(nGroupId);
				log("SQL for Updating GroupLeader: %s", strSql.c_str());
				if (pDBConn->ExecuteUpdate(strSql.c_str()))
				{
					bRet = true;
				}
			}
				/*int nRecords = 0;
				while (pResultSet->Next())
				{
					nRecords = pResultSet->GetInt("Totals");
				}
				delete pResultSet;

				if (nRecords)
				{
					string strSql = "update IMGroup set creator = " + int2string(nNewUserId) + " where id=" + int2string(nGroupId);
					if (pDBConn->ExecuteUpdate(strSql.c_str()))
					{
						bRet = true;
						log("SQL for Updating GroupLeader: %s", strSql.c_str());
					}
				}*/
			pDBManager->RelDBConn(pDBConn);
		}
		else
		{
			log("no db connection for teamtalk_master");
		}
        //if modify group leader success, need to inc the group version and get effected users in the group(for notifying)
        if(bRet)
        {
			getGroupUser(nGroupId, lsCurUserId);
            incGroupVersion(nGroupId);
        }
    }
	else
	{
		log("user:%u has no permission to change group leader:%u", nUserId, nGroupId);
	}

	return bRet;

}


//Added by george jiang on Aug 14th,2015
bool CGroupModel::GetGroupInfo(uint32_t nGroupID, string& strGroupName, string& strGroupAvatar)
{
	bool bRet = false;
	if (isValidateGroupId(nGroupID))
	{
		CDBManager* pDBManager = CDBManager::getInstance();
		CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
		if (pDBConn)
		{
			//string strSql = "select name,avatar from IMGroup where id = "+ int2string(nGroupID) + " and status = 0";
			string strSql = "select name,avatar from IMGroup where id = " + int2string(nGroupID);
			//log("Sql for getting group name and avatar: %s", strSql.c_str());
			CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
			if (pResultSet)
			{
				while (pResultSet->Next())
				{
					strGroupName = pResultSet->GetString("name");
					strGroupAvatar = pResultSet->GetString("avatar");
				}
				delete pResultSet;
				bRet = true;
			}
			pDBManager->RelDBConn(pDBConn);
		}
		else
		{
			log("no db connection for teamtalk_master");
		}
	}
	else
	{
		log("invalid group id:%u", nGroupID);
	}

	return bRet;
}


bool CGroupModel::SetShieldStatus(uint32_t nUserID, uint32_t nGroupID, uint32_t nStatus)
{
	bool bRet = false;
	if (!isInGroup(nUserID, nGroupID))
	{
		log("user:%d is not in group:%d", nUserID, nGroupID);
		return bRet;;
	}

	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
	if (pDBConn)
	{
		string strSql = "update IMGroupMember set shieldstatus = " + int2string(nStatus) + ",updated=unix_timestamp() where groupid = " + int2string(nGroupID) + " and userid= " + int2string(nUserID) + " and status = 0";
		log("Sql for updating shield status: %s", strSql.c_str());
	
		if (pDBConn->ExecuteUpdate(strSql.c_str()))
		{
			bRet = true;
		}
		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_master");
	}

	return bRet;
}