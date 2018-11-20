/*================================================================
*     Copyright (c) 2015年 lanhu. All rights reserved.
*   
*   文件名称：InternalAuth.h
*   创 建 者：Zhang Yuanhao
*   邮    箱：bluefoxah@gmail.com
*   创建日期：2015年03月09日
*   描    述：内部数据库验证策略
*
#pragma once
================================================================*/
#ifndef __INTERLOGIN_H__
#define __INTERLOGIN_H__

#include "LoginStrategy.h"
#include <string>
#include <list>

using namespace std;


#define LOGIN_RESULT_USER_NO_FOUND	-2			//invalid user name
#define LOGIN_RESULT_FORCE_CHG_PASSWORD	-3		//force to change initial password
#define LOGIN_RESULT_NO_BUY_PRODUCT	-4			//no buying any product
#define LOGIN_RESULT_INVALID_PASSWORD	-5		//invalid password
#define LOGIN_RESULT_LOGIN_BY_ALIAS_NAME	-6	//login by alias name
#define LOGIN_RESULT_LOCKED_BY_EC			-7  //locked by ec
#define LOGIN_RESULT_EXPIRED_PASSWORD	-8		//expired password
#define LOGIN_RESULT_INACTIVE_USER	-9	//inactived user
#define LOGIN_RESULT_DISABLED_USER -10 //disabled user
#define LOGIN_RESULT_LIMITED_IP -11 //limited ip
#define LOGIN_REEULT_BIND_PHONE -12 //the user is related with phone


class CInterLoginStrategy :public CLoginStrategy
{
public:
	//virtual bool doLogin(const std::string& strName, const std::string& strPass, IM::BaseDefine::UserInfo& user);
	virtual bool doLogin(const std::string& strName, const std::string& strPass, uint32_t& unUserId, int32_t& nStatus, uint32_t unIsActivated, std::string& strClientIp);
	void GetBasicUserInfo(uint32_t unUserId, IM::BaseDefine::UserInfo& UserInfo);
	void GetCompanyInfo(uint32_t unUserId, list<IM::BaseDefine::CompanyInfo>& lsCompanyInfo);
	void GetShieldUsers(uint32_t unUserId, list<IM::BaseDefine::ShieldUser>& lsShieldUsers);
	void GetUserDetails(string& strUserUuid, list<IM::BaseDefine::UserDetails>& lsUserDetails);
	void GetVips(uint32_t unUserId, list<uint32_t>& lsVips);
	void GetStaffInfo(uint32_t unUserId, uint32_t unCompanyId, IM::BaseDefine::StaffInfo& StaffInfo);
	void GetStaffDuty(string& strUserUuid, uint32_t unCompanyId, list<IM::BaseDefine::DutyInfo>& listDuty, map<string, list<string>>& mapPosition);
	void GetStaffDetails(string& strStaffUuid, list<IM::BaseDefine::UserDetails>& lsStaffDetails);

private:
	int Validating(const string& strName, const string& strPass, uint32_t &unUserId, uint32_t unActivatedStatus, string& strConnIp);
	int32_t Authenticate(const string& strUserPassword, const string& strDbPassword, const string& strSalt);
	int32_t Authenticate(const string& strUserPassword, const string& strUserUuid, uint32_t& unInitPassword, uint32_t& unLastUpdateTime);
	unsigned long StringIp2Long(string& strIp);
	bool IsLimitedIp(string& strIp, list<string>& lsLimitIpList);
	static string m_strHost;

};

#endif /*defined(__INTERLOGIN_H__) */
