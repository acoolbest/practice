/*================================================================
*     Copyright (c) 2015年 lanhu. All rights reserved.
*   
*   文件名称：UserModel.h
*   创 建 者：Zhang Yuanhao
*   邮    箱：bluefoxah@gmail.com
*   创建日期：2015年01月05日
*   描    述：
*
#pragma once
================================================================*/
#ifndef __USERMODEL_H__
#define __USERMODEL_H__

#include "IM.BaseDefine.pb.h"
#include "ImPduBase.h"
#include "public_define.h"
#include "json/json.h"

#define USER_MODEL_USER_NO_OPEN_IM			-2

////added by george jiang on Jan 28th,2016
//typedef struct UserDetails
//{
//	string m_strUserUUID;
//	string m_strUUID;
//	string m_strTitle;
//	string m_strContent;
//	int m_nType;
//}USER_DETAILS;
//typedef list<USER_DETAILS>  LIST_USER_DETAILS;
//typedef LIST_USER_DETAILS::iterator IT_LIST_USER_DETAILS;


class CUserModel
{
public:
    static CUserModel* getInstance();
    ~CUserModel();
    //void getChangedId(uint32_t& nLastTime, list<uint32_t>& lsIds);
	void getChangedId(uint32_t& nLastTime, list<uint32_t>& lsIds,uint32_t nCompanyID);
    void getUsers(list<uint32_t> lsIds, list<IM::BaseDefine::UserInfo>& lsUsers);
    bool getUserInfo(uint32_t nUserId,string& strNickName, string& strAvatar);
    bool updateUser(DBUserInfo_t& cUser,uint32_t nNow);
	bool updateUser(IM::BaseDefine::UserInfo& cUser, uint32_t nNow);
	//更新个性签名
	bool updateNagure(uint32_t nUserId,std::string strNagure,uint32_t nNow);
    bool insertUser(DBUserInfo_t& cUser);
	//获取出公司id
	bool GetCompanyId(uint32_t nUserId,uint32_t &nCompanyId);
	bool GetCompanyId(string& strUserID, uint32_t &nCompanyId);
	//获取出公司名称和部门名称
	std::string getComanyName(uint32_t nCompanyId);
	//std::string getDepartName(uint32_t nDepartId,uint32_t &nCompanyId);
	std::string getDepartName(string& strDepartId, uint32_t &nCompanyId);
//    void getUserByNick(const list<string>& lsNicks, list<IM::BaseDefine::UserInfo>& lsUsers);
    void clearUserCounter(uint32_t nUserId, uint32_t nPeerId, IM::BaseDefine::SessionType nSessionType);
	bool updateUserAvatar(uint32_t nUserId, string& strNewAvatar);
	void GetPosition(uint32_t nUserID, list<IM::BaseDefine::PositionInfo>& listPosition);
	int IsValidUser(uint32_t nUserID);
	//bool IsValidUser(uint32_t nUserID);
	bool setPush(uint32_t unUserId, uint32_t unPeerId, uint32_t unType, uint32_t unStatus);
	bool getPush(uint32_t unUserId, list<IM::BaseDefine::ShieldUser>& lsPush);
	bool SetShieldStatus(uint32_t unUserId, uint32_t unPeerId, uint32_t unStatus);
	void GetUserShieldStatus(uint32_t unUserId, list<IM::BaseDefine::ShieldUser>& listShieldStatus);
	void GetDuty(string& strUserUuid, list<IM::BaseDefine::DutyInfo>& listDuty, map<string, list<string>>& mapPosition, uint32_t unCompanyId);
	void GetUserAdditionalInfo(string& strUserUUID, list<IM::BaseDefine::UserDetails>& listAddiInfo, uint32_t nType = 1);
	//bool SyncUserInfo(uint32_t nUserID, string& strAvatar);
	//void getUsers(uint32_t unCompanyId, uint32_t& unLastTime, list<IM::BaseDefine::UserInfo>& lsUsers, uint32_t& unUsers);
	void GetJoinedCompany(uint32_t unUserId, list<IM::BaseDefine::CompanyInfo>& listCompany);
	void GetCompanyStaff(uint32_t unUserId, uint32_t unCompanyId, uint32_t& unLastTime, list<IM::BaseDefine::StaffInfo>& lsStaff, uint32_t& unStaff);
	void GetCompanyStaff(uint32_t unUserId, uint32_t unCompanyId, uint32_t& unLastTime, list<IM::BaseDefine::StaffInfoLite>& lsStaff);
	bool UpdateUser(string& strUserUuid, uint32_t unSex, int32_t nBirthday, string& strSignature, string& strUserRealName, list<IM::BaseDefine::UserDetails>& lsUserDetails, uint32_t unNow);
	bool UpdateStaff(string& strStaffUuid,  list<IM::BaseDefine::UserDetails>& lsStaffDetails, uint32_t unNow);
	void GetUsers(uint32_t unUserId, uint32_t& unLastTime, list<IM::BaseDefine::UserInfo>& lsUsers, uint32_t& unUsers);
	void GetStaffInfo(uint32_t unUserId, uint32_t unStaffId, list<IM::BaseDefine::StaffInfo>& lsStaffInfo);
	void GetVip(uint32_t unUserId, list<uint32_t>& lsPerson);
	void GetUserInfo(uint32_t unUserId, IM::BaseDefine::UserInfo& UserInfo);
	void GetUserInfo(string& strUserUuid, IM::BaseDefine::UserInfo& UserInfo);
	void GetStaffInfo(uint32_t unUserId, uint32_t unCompanyId, IM::BaseDefine::StaffInfo& CurrentStaff);
	bool ChangeVip(uint32_t unUserId, uint32_t unVipId, uint32_t unOperationType, uint32_t& unNow);
	void GetAssistantInfo(uint32_t& unLastTime,list<IM::BaseDefine::UserInfo>& lsUserInfo);
private:
    CUserModel();
	uint32_t GetJobGrade(string& strType);
	bool SyncInfoWithEc(Json::Value& root, uint32_t unSyncType=1);
	
private:
    static CUserModel* m_pInstance;
	static string  m_strHost;
};

#endif /*defined(__USERMODEL_H__) */
