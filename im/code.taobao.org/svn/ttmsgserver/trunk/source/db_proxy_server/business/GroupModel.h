/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *
 *   文件名称：GroupModel.h
 *   创 建 者：Zhang Yuanhao
 *   邮    箱：bluefoxah@gmail.com
 *   创建日期：2014年12月15日
 *   描    述：
 *
 ================================================================*/

#ifndef __IM_GROUP_MODEL__
#define __IM_GROUP_MODEL__

#include <stdio.h>
#include <iostream>
#include <string>
#include <list>
#include <map>
#include <set>

#include "MessageModel.h"
#include "IM.BaseDefine.pb.h"

using namespace std;

const uint32_t MAX_UNREAD_COUNT = 100;
#define MAX_SYSTEM_USER_ID	3


class CGroupModel {
public:
    virtual ~CGroupModel();
    static CGroupModel* getInstance();
    
public:
    uint32_t createGroup(uint32_t nUserId, const string& strGroupName, const string& strGroupAvatar, uint32_t nGroupType, set<uint32_t>& setMember, string& strGroupId, uint32_t unCompanyId);
    //bool removeGroup(uint32_t nUserId, uint32_t nGroupId, list<uint32_t>& lsCurUserId);
	bool removeGroup(uint32_t nUserId, uint32_t nGroupId, list<uint32_t>& lsCurUserId, set<uint32_t>& setGroupUsers);
    //void getUserGroup(uint32_t nUserId, list<IM::BaseDefine::GroupVersionInfo>& lsGroup, uint32_t nGroupType, uint32_t unLatestUpdateTime);
	void getUserGroup(uint32_t nUserId, list<IM::BaseDefine::GroupVersionInfo>& lsGroup, uint32_t unLatestUpdateTime);
	//void getUserGroup(uint32_t nUserId, list<IM::BaseDefine::GroupVersionInfo>& lsGroup, uint32_t nGroupType);
	//void getUserGroupIds(uint32_t nUserId, list<uint32_t>& lsGroupId, uint32_t nLimited = 100);
    void getUserGroupIds(uint32_t nUserId, list<uint32_t>& lsGroupId, uint32_t unLatestUpdateTime = 0, uint32_t nLimited = 100);
    void getGroupInfo(map<uint32_t,IM::BaseDefine::GroupVersionInfo>& mapGroupId, list<IM::BaseDefine::GroupInfo>& lsGroupInfo, uint32_t unUserID);
    bool setPush(uint32_t nUserId, uint32_t nGroupId, uint32_t nType, uint32_t nStatus);
	bool SetShieldStatus(uint32_t nUserID, uint32_t nGroupID, uint32_t nStatus);
    void getPush(uint32_t nGroupId, list<uint32_t>& lsUser, list<IM::BaseDefine::ShieldStatus>& lsPush);
    bool modifyGroupMember(uint32_t nUserId, uint32_t nGroupId, IM::BaseDefine::GroupModifyType nType, set<uint32_t>& setUserId,
                           list<uint32_t>& lsCurUserId, string& strGroupName, uint32_t& nCreator);
    void getGroupUser(uint32_t nGroupId, list<uint32_t>& lsUserId);
	bool removeMember(uint32_t nGroupId, set<uint32_t>& setUser,list<uint32_t>& lsCurUserId);
	void removeSession(uint32_t nGroupId, const set<uint32_t>& lsUser);
	void ResetSession(uint32_t nGroupId, const set<uint32_t>& setUser);
	bool hasModifyPermission(uint32_t nUserId, uint32_t nGroupId, IM::BaseDefine::GroupModifyType nType,uint32_t &nCreator);
	bool hasModifyPermission(uint32_t nUserId, uint32_t nGroupId, bool bCreator = true); //added by george.jiang on June 23rd,2015
	bool hasModifyPermission(uint32_t nUserId, uint32_t nGroupId, IM::BaseDefine::GroupModifyType nType, uint32_t &nCreator, string& strGroupName, bool bCreator = true); //added by george jiang on Aug 25th,2015
	//bool hasModifyPermission(uint32_t nGroupId); //added by george jiang on Oct 10th,2015
    bool isInGroup(uint32_t nUserId, uint32_t nGroupId);
    void updateGroupChat(uint32_t nGroupId);
    bool isValidateGroupId(uint32_t nGroupId);
    uint32_t getUserJoinTime(uint32_t nGroupId, uint32_t nUserId);
	bool updateGroupName(uint32_t nUserId, uint32_t nGroupId, const string& strGroupName, list<uint32_t>& lsCurUserId);
	bool updateGroupLeader(uint32_t nUserId, uint32_t nGroupId, uint32_t nNewUserId, list<uint32_t>& lsCurUserId);
	bool GetGroupInfo(uint32_t nGroupID, string& strGroupName, string& strGroupAvatar); //added by george.jiang on Aug 14th,2015
private:
    CGroupModel();
    
	bool insertNewGroup(uint32_t reqUserId, const string& groupName, const string& groupAvatar, uint32_t groupType, uint32_t memCnt, uint32_t& groupId, string& strGroupId, uint32_t unCompanyId);
    bool insertNewMember(uint32_t nGroupId,set<uint32_t>& setUsers);
        string GenerateGroupAvatar(uint32_t groupId);
    //void getGroupVersion(list<uint32_t>&lsGroupId, list<IM::BaseDefine::GroupVersionInfo>& lsGroup, uint32_t nGroupType);
	void getGroupVersion(list<uint32_t>&lsGroupId, list<IM::BaseDefine::GroupVersionInfo>& lsGroup);
    
    bool addMember(uint32_t nGroupId, set<uint32_t>& setUser,list<uint32_t>& lsCurUserId);
  
    void removeRepeatUser(uint32_t nGroupId, set<uint32_t>& setUser);
   
    bool incGroupVersion(uint32_t nGroupId);
    void clearGroupMember(uint32_t nGroupId);
    
    void fillGroupMember(list<IM::BaseDefine::GroupInfo>& lsGroups);
        
private:
    static CGroupModel*	m_pInstance;
};

#endif /* defined(__IM_GROUP_MODEL__) */
