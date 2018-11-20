#ifndef HUAYUN_IM_DATA_SYNC_H
#define HUAYUN_IM_DATA_SYNC_H

#include "json/json.h"
#include "CachePool.h"
#include "DBPool.h"
#include "ConfigFileReader.h"
#include "util.h"
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <time.h>
#include <stdlib.h>


#define FILE_NAME_DOMAIN_PREFIX	"server_domain"
#define FILE_NAME_USER_PREFIX	"domain_user"
#define FILE_NAME_GROUP_PREFIX	"domain_group"
#define KEY_TOTAL_URLS  "TOTAL_URLS"
#define KEY_AVATAR_URL  "AVATAR_URL"
#define AVATAR_CONFIG_FILE_NAME "defaultavatarurl.txt"
#define FORMAT_KEY_TOTAL_URLS	"TOTAL_URLS=%d"
#define FORMAT_KEY_AVATAR_URL	"AVATAR_URL%d=%s"

#define DATA_SYNC_USER_NO_OPEN_IM	-2
#define DATA_SYNC_DEFAULT_POSITION_UUID	"00000000000000000000000000000000"

typedef list<string> LIST_STRING;
typedef LIST_STRING::iterator  IT_LIST_STRING;
typedef list<uint32_t> LIST_UINT;
typedef LIST_UINT::iterator IT_LIST_UINT;

//NOTE:
//1.all structures are replace with class later

typedef struct UserInfo
{
	string m_strUserUuid;
	string m_strUserName;
	string m_strAvatar;
	string m_strName;
	int m_nSex;
	string m_strPhone;
	int m_nStatus;
	string m_strKeyword;
	int m_unBirthday;
	string m_strPassword;
	string m_strBuyProducts;
	string m_strOriginalPassword;
	string m_strHistoryPassword;
	int m_nIsAdmin;
	string m_strRemark;
	int m_nAdminStatus;
	unsigned int m_nBeginTime;
	unsigned int m_nExpiredTime;
	unsigned int m_nUpdateTime;
	int m_nIsIpLimit;
	unsigned int m_nLastModifiedTime;
	int m_nLock;
	string m_strLockIP;
	unsigned int m_nLockStartTime;
	unsigned int m_nLockEndTime;
	string m_strNickName;
	string m_strSignature;
	unsigned int m_unInitPasswordStatus;
	unsigned int m_unIsActived;
	string m_strEmail;
	string m_strSalt;
	string m_strAccount;
}USER_INFO;

typedef map<uint32_t, string > MAP_USER_KEY;
typedef map<uint32_t, MAP_USER_KEY> MAP_COMPANY_USER;
typedef MAP_USER_KEY::iterator IT_MAP_USER_KEY;
typedef MAP_COMPANY_USER::iterator IT_MAP_COMPANY_USER;


typedef struct CompanyInfo
{
	string m_strCompanyID;
	string m_strCompanyName;
}COMPANY_INFO;
typedef list<COMPANY_INFO> LIST_COMPANY_INFO;
typedef LIST_COMPANY_INFO::iterator IT_LIST_COMPANY_INFO;

typedef struct DepartmentInfo
{
	string m_strDepartmentID;
	string m_strDepartmentName;
	string m_strDomainID;
	int m_nPriority;
	int m_nLevel;
	string m_strParentDepartmentID;
	int m_nStatus;
	int m_nIsDepart;
	string m_strDepartPath;
	int m_nContactStatus;
}DEPARTMENT_INFO;
typedef list<DEPARTMENT_INFO> LIST_DEPARTMENT_INFO;
typedef LIST_DEPARTMENT_INFO::iterator IT_LIST_DEPARTMENT_INFO;


typedef struct PositionInfo
{
	string m_strDomainID;
	string m_strPositionID;
	string m_strPositionName;
	string m_strPositionType;
	int m_nStatus;
	string m_strRemark;
	int m_nSort;
}POSITION_INFO;
typedef list<POSITION_INFO> LIST_POSITION_INFO;
typedef LIST_POSITION_INFO::iterator IT_LIST_POSITION_INFO;


typedef struct DomainInfo
{
	string m_strCompanyUuid;
	string m_strCompanyName;
	string m_strSyncServer;
	int m_nStatus;
	unsigned int m_unExpiredDate;
	string m_strCompanyNo;
	string m_strCompanyFullName;
	string m_strIndustryType;
	unsigned int m_unIsJoined;
	unsigned int m_unIsInvited;
}DOMAIN_INFO;

typedef list<DOMAIN_INFO> LIST_DOMAIN_INFO;
typedef LIST_DOMAIN_INFO::iterator IT_LIST_DOMAIN_INFO;

typedef struct DomainGroup
{
	string m_strGroupID;
	string m_strDomainID;
	string m_strGroupName;
	string m_strEmail;
	int m_nUserCount;
	int m_nStatus;
}DOMAIN_GROUP;
typedef list<DOMAIN_GROUP> LIST_DOMAIN_GROUP;
typedef LIST_DOMAIN_GROUP::iterator IT_LIST_DOMAIN_GROUP;

typedef struct GroupUser
{
	string m_strUserID;
	string m_strEmail;
	int m_nUserType;
	string m_strGroupID;
}GROUP_USER;
typedef list<GROUP_USER> LIST_GROUP_USER;
typedef LIST_GROUP_USER::iterator IT_LIST_GROUP_USER;


typedef struct MemberInfo
{
	uint32_t m_unStatus;
	uint32_t m_unCreateTime;
}MEMBER_INFO;


typedef struct UserDetails
{
	string m_strUserUUID;
	string m_strUUID;
	string m_strTitle;
	string m_strContent;
	int m_nType;
}USER_DETAILS;

typedef list<USER_DETAILS> LIST_USER_DETAILS;
typedef LIST_USER_DETAILS::iterator IT_LIST_USER_DETAILS;

typedef map<uint32_t, MEMBER_INFO> MAP_MEMBER_INFO;
typedef MAP_MEMBER_INFO::iterator IT_MAP_MEMBER_INFO;

typedef struct UserAlias
{
	string m_strUUID;
	string m_strUserID;
	string m_strUserName;
	string m_strAliasName;
}USER_ALIAS;
typedef list<USER_ALIAS> LIST_USER_ALIAS;
typedef LIST_USER_ALIAS::iterator IT_LIST_USER_ALIAS;


typedef struct DutyInfo
{
	uint32_t m_unUserId;
	string m_strUserId;
	string m_strDepartId;
	string m_strPositionId;
	string m_strJobGrade;
}USER_DUTY;
typedef list<USER_DUTY> LIST_USER_DUTY;
typedef LIST_USER_DUTY::iterator IT_LIST_USER_DUTY;


typedef struct SessionInfo
{
	uint32_t m_unUserId;
	uint32_t m_unSessionId;
	uint32_t m_unSessionType;
}SESSION_INFO;
typedef list<SESSION_INFO> LIST_SESSION_INFO;
typedef LIST_SESSION_INFO::iterator IT_LIST_SESSION_INFO;

typedef struct DomainPassword
{
	string m_strDomainId;
	uint32_t m_unValidityStatus;
	uint32_t m_unValidity;
	uint32_t m_unIsForcePassword;
	uint32_t m_unPasswordStrength;
	uint32_t m_unPasswordLength;
}DOMAIN_PASSWORD;


typedef struct StaffInfo
{
	uint32_t m_unUserId;
	string m_strUserUuid;
	string m_strStaffUuid;
	string m_strCompanyUuid;
	string m_strJobNumber;
	string m_strStaffName;
	string m_strMobilePhone;
	string m_strKeyword;
	string m_strBuyProduct;
	uint32_t m_unIsAdmin;
	uint32_t m_unIsActived;
	uint32_t m_unAdminStatus;
	uint32_t m_unIsSuperAdmin;
	int32_t m_nStatus;
	uint32_t m_unContactStatus;
	string m_strRemark;
	string m_strAccount;
	string m_strUserName;
	string m_strEmail;
	string m_strDefaultEmail;
}STAFF_INFO;


typedef struct limitip
{
	string m_strLimitIpUuid;
	string m_strUserUuid;
	string m_strLimitIp;
}LIMIT_IP;

typedef list<LIMIT_IP> LIST_LIMIT_IP;
typedef LIST_LIMIT_IP::iterator IT_LIST_LIMIT_IP;


class CDataSync
{
	public:
		CDataSync();
		~CDataSync();
		void SetConnection(CDBConn* pInDBConn, CacheConn* pInGroupMember, CacheConn* pInCacheUser, CacheConn* pInCacheUnread);
	/*	void ImportCompanyInfo(LIST_COMPANY_INFO& refCompanyInfo, LIST_DOMAIN_INFO& refDomainInfo);
		void ImportDepartmentInfo(LIST_DEPARTMENT_INFO& refDepartmentInfo);
		void CheckSyncData(void);
		void ImportDomainGroup(LIST_DOMAIN_GROUP& refDomainGroup);
		void ImportGroupUser(LIST_GROUP_USER& refGroupUser);*/

		//company
		void GetCompanyInfo(string& strInputString, DOMAIN_INFO& DomainInfo);
		void GetCompanyByCompanyId(string& strInputString, DOMAIN_INFO& DomainInfo);
		void AddNewCompany(DOMAIN_INFO& DomainInfo);
		void UpdateCompany(DOMAIN_INFO& DomainInfo);
		void DeleteCompany(string& strCompanyUuid);

		int GetCompanyPasswordRule(string& strInputString, DOMAIN_PASSWORD& PasswordRule);
		void UpdateCompanyPassword(DOMAIN_PASSWORD& DomainPassword);
		void DeleteCompanyPasswordRule(string& strCompanyUuid);


		//int GetDomainPassword(const string& strInputString, DOMAIN_PASSWORD& DomainPassword);
		int GetCompanyInfoSet(string& strInputString, DOMAIN_INFO& InfoSet);
		void UpdateCompanyInfoSet(DOMAIN_INFO& InfoSet);
		void DeleteCompanyInfoSet(string& strCompanyUuid);
		
		
		//void UpdateCompanyByDomainId(DOMAIN_INFO& DomainInfo);

		int  GetUserInfo(string& strInputString, USER_INFO& UserInfo);
		void AddUser(USER_INFO& UserInfo);
		void UpdateUser(USER_INFO& UserInfo);
		void DeleteUser(string& strUserID);

		int GetUserDetails(string& strInputString, LIST_USER_DETAILS& listUserDetails);
		void AddUserDetails(LIST_USER_DETAILS& listUserDetails);
		void SetUserDetails(string& strUserUuid, LIST_USER_DETAILS& listUserDetails);
		void UpdateUserDetails(LIST_USER_DETAILS& listUserDetails);

		int  GetStaffInfo(string& strInputString, STAFF_INFO& StaffInfo);
		void AddCompanyStaff(STAFF_INFO& StaffInfo);
		void UpdateCompanyStaff(STAFF_INFO& StaffInfo);
		void DeleteCompanyStaff(string& strStaffUuid);
		int GetStaffDetails(string& strInputString, LIST_USER_DETAILS& listUserDetails);
		void AddStaffDetails(LIST_USER_DETAILS& listUserDetails);
		void SetStaffDetails(string& strStaffUuid, LIST_USER_DETAILS& listUserDetails);
		void UpdateStaffDetails(LIST_USER_DETAILS& listUserDetails);

		int GetDepartInfo(string& strInputString, DEPARTMENT_INFO& DepartInfo);
		void AddCompanyDepart(DEPARTMENT_INFO& DepartInfo);
		void UpdateCompanyDepart(DEPARTMENT_INFO& DepartInfo);
		void DeleteCompanyDepart(string& strDepartID);

		int GetPositionInfo(string& strInputString, POSITION_INFO& PositionInfo);
		void AddCompanyPosition(POSITION_INFO& PositionInfo);
		void UpdateCompanyPosition(POSITION_INFO& PositionInfo);
		void DeleteCompanyPosition(string& strPositionID);
		void DeletePositionByCompanyUuid(string& strCompanyId);

		void AddStaffPosition(string& strDepartID, string& strPositionID, string& strUserID, string& strJobGrade);
		void UpdateStaffPosition(string& strStaffId, string& strDepartId, string& strJobGrade);
		void DeleteStaffPosition(string& strDepartID, string& strPositionID, string& strUserID);
		void DeleteStaffPosition(string& strDepartID, string& strUserID);
		void DeleteStaffPositionByDepartmentId(string& strDepartID);
		void DeleteStaffPositionByPositionID(string& strPositionID);
		void DeleteStaffPositionByDepartmentIDAndPositionID(string& strDepartmentID, string& strPositionID);
		void DeleteStaffPositionByStaffId(string& strStaffId);

		void GetUserProducts(string& strInputString);
		void GetStaffProducts(string& strInputString);

		void DeleteDepartmentByDepartmentID(string& strDepartID);
		void DeletePositionByPositionID(string& strPositionID);
		void DeletePositionByDomainID(string& strDomainID);
		void DeleteDepartmentAndPosition(string& strDepartID, string& strPositionID);
		void DeleteDepartIDAndUserID(string& strOrgID, string& strUserID);
		void DeletePositionIdAndUserId(string& strPositionId, string& strUserId);

		int GetGroupInfo(string& strInputString, DOMAIN_GROUP& GroupInfo, list<string>& listUsers);
		void AddNewMailGroupAndMembers(DOMAIN_GROUP& GroupInfo, list<string>& listUsers);
		void UpdateMailGroup(DOMAIN_GROUP& GroupInfo);
		void DeleteMailGroup(string& strGroupID);

		int GetUserAlias(const string& strInputString, USER_ALIAS& Alias);
		void AddUserAlias(USER_ALIAS& Alias);
		void UpdateUserAlias(USER_ALIAS& Alias);

		void GetUserAllAlias(string& strInputString, LIST_USER_ALIAS& listAlias);
		void SetUserAllAlias(string& strUserUuid, LIST_USER_ALIAS& listAlias);

		void DeleteUserAliasByAliasId(const string& strAliasId);
		void DeleteUserAliasByUserId(const string& strUserId);
		void DeleteUserAliasByAliasName(const string& strAliasName);

		int GetMailGroupUser(string& strInputString, list<string>& listUsers, string& strGroupID);
		void SetMailGroupUser(string& strGroupID, list<string>& listUsers);
		void SetAllGroupUserStatus(void);
		void SetAllCompanyStaff(void);
		void SetAllUsers(void);

		//void DeleteMailGroupUser(string& strGroupID, list<string>& listUsers);

		void SetAllUserStatus(void);
		void ChangeUserInfo(uint32_t unUserID, string& strUserID, string& strUserInfo);
		
		//get limited ip list for user
		void GetLimitedIpInfo(string strInputString, LIST_LIMIT_IP& lsLimitIp);
		void SetUserLimitedIp(string& strUserUuid, LIST_LIMIT_IP& lsLimitIp);

	protected:
		CDBConn* m_pDBConn;
		CacheConn* m_pCacheGroupMember;
		CacheConn* m_pCacheUser;
		CacheConn* m_pCacheUnreadMsg;
		int m_nCompanyId;
		void SetUserStatus(uint32_t unUserID, int32_t unStatus);
		void SetGroupUserStatus(uint32_t unGroupID, uint32_t unUserID, uint32_t unStatus, uint32_t nCreateTime);
		int GetDefaultAvatarUrl(char** ppAvatarUrl);
		//int GetDomainUser(Json::Value &root, const char* pszParent, int nLevel, string& strCompanyID, LIST_DEPARTMENT_INFO& DepartmentInfo, LIST_USER_INFO& UserInfo);
		int GetCompanyByDomain(string& strDomainID);
		void CheckUser(string& strUserID);
		void RemoveStaffFromGroup(uint32_t unUserId, uint32_t unCompanyId);
		void SetGroupUserStatus(string& strGroupID);
		void UpdateRecentSession(string& strGroupId);
		static MAP_COMPANY_USER m_mapDomainUser;
		static LIST_UINT m_listUser;
		int GetCompanyIdByCompanyUuid(string& strCompanyUuid);
		int GetCompanyIdByStaffUuid(string& strStaffUuid);
		int GetCompanyIdByUserId(uint32_t unUserId);
		int GetCompanyIdByDepartId(string& strDepartId);
		int GetCompanyIdByPositionId(string& strPositionId);
		int GetCompanyInfoByCompanyUuid(string& strCompanyUuid, DOMAIN_INFO& CompanyInfo);
		void SendUserAddNotify(uint32_t unUserId, USER_INFO& UserInfo);
		void SendUserUpdateNotify(uint32_t unUserId, int nStatus, USER_INFO& UserInfo);
		void SendUserDelNotify(uint32_t unUserId, string& strUserId);
		void SendPasswordChangeNotify(uint32_t unUserId);
		void SendGroupMemberChangeNotify(uint32_t unGroupId, uint32_t unUserId, list<uint32_t>& listCurrUsers);
		void SendCreatorChangeNotify(uint32_t unGroupId,uint32_t unNewCreator, list<uint32_t>& listCurrUsers);
		void SendDepartmentChangeNotify(DEPARTMENT_INFO& DepartInfo, uint32_t unSyncType);
		void SendDepartmentDelNotify(string& strDepartId);
		void SendPositionChangeNotify(POSITION_INFO& PositionInfo, uint32_t unSyncType);
		void SendPositionDelNotify(string& strPositionId);
		void SendUserPositionChangeNotify(string& strUserId, uint32_t unUserId, string& strDepartId, string& strPositionId, int unSyncType, string& strJobGrade);
		int GetUserInfoByUserId(string& strUserId, USER_INFO& UserInfo);
		void SendMailGroupCreateNotify(uint32_t unGroupId, uint32_t unCompanyId, string& strGroupName, string& strGroupId);
		void SendMailGroupDelNotify(string& strGroupId, time_t nTicks);
		void SendMailGroupChangeNameNotify(string& strGroupId, string& strNewGroupName);
		void SendMailGroupChangeMemberNotify(uint32_t unGroupId, list<uint32_t>& listChangeUsers, list<uint32_t>& listUsers, int nChangeType);
		void SendUserDetailsChangeNotify(string& strUserUuid,LIST_USER_DETAILS& listUserDetails);
		void SendRemoveDepartmentUserNotify(string& strDepartId);
		void SendRemoveDepartmentUserNotify(string& strDepartId, string& strUserId);
		void SendUserPositionChangeNotify(LIST_USER_DUTY& lsUserDuty);
		uint32_t GetUserIdByUserUuid(string& strUserId);
		int GetStatusByProduct(string& strProduct, int nStatus);
		void clearUserCounter(uint32_t nUserId);
		void clearUserCounter(uint32_t unUserId, uint32_t unGroupId);
		void SendRemoveSessionNotify(uint32_t unUserId);
		void SendRemoveSessionNotify(uint32_t unUserId, uint32_t unGroupId);
		void RemoveSession(uint32_t unUserId);
		void DeleteUser(uint32_t unUserId);
		void DeleteStaff(uint32_t unUserId, uint32_t unGroupId);
		void SendStaffAddNotify(uint32_t unUserId, STAFF_INFO& UserInfo);
		void SendStaffUpdateNotify(uint32_t unUserId, int nStatus, STAFF_INFO& UserInfo);
		void SendStaffDelNotify(uint32_t unUserId, string& strUserId, string& strStaffUuid);
		void SendStaffDetailsChangeNotify(string& strStaffUuid, LIST_USER_DETAILS& listUserDetails);
		uint32_t GetUserIdByStaffUuid(string& strStaffUuid);
		int GetStaffInfoByStaffUuid(string& strStaffUuid, STAFF_INFO& StaffInfo);
		//uint32_t GetCompanyInfoByCompanyUuid(string& strCompanyUuid, DOMAIN_INFO& CompanyInfo);
		void GetCompanyInfoByCompanyId(uint32_t unCompanyId, DOMAIN_INFO& CompanyInfo);
		void SendCompanyAddNotify(uint32_t unUserId, uint32_t unCompanyId, DOMAIN_INFO& CompanyInfo);
		void SendCompanyUpdateNotify(uint32_t unCompanyId, DOMAIN_INFO& CompanyInfo);
		void SendCompanyDelNotify(uint32_t unCompanyId, DOMAIN_INFO& CompanyInfo);
		void UpdateUserChangeTime(string& strUserUuid);
		void UpdateStaffChangeTime(string& strStaffUuid);
		void GetStaffDuty(string& strStaffUuid, LIST_USER_DUTY& listDuty);
};
#endif