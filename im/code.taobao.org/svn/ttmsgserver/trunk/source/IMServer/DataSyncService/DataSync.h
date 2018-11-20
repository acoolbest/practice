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


//#define CACHE_POOL_NAME	"im_sync_json"
//#define CACHE_POOL_NAME_GROUP_MEMBER	"group_member"
//#define LOCAL_CACHE_SERVER_IP	"127.0.0.1"
//#define LOCAL_CACHE_SERVER_PORT	6379
//#define LOCAL_CACHE_DB_NO	8
//#define LOCAL_CACHE_DB_NO_GROUP_MEMBER	1
//#define LOCAL_CACHE_DB_MAX_COUNT	16
//#define LOCAL_CACHE_DB_MAX_COUNT_GROUP_MEMBER	48
#define FILE_NAME_DOMAIN_PREFIX	"server_domain"
#define FILE_NAME_USER_PREFIX	"domain_user"
#define FILE_NAME_GROUP_PREFIX	"domain_group"
#define KEY_TOTAL_URLS  "TOTAL_URLS"
#define KEY_AVATAR_URL  "AVATAR_URL"
#define AVATAR_CONFIG_FILE_NAME "defaultavatarurl.txt"
#define FORMAT_KEY_TOTAL_URLS	"TOTAL_URLS=%d"
#define FORMAT_KEY_AVATAR_URL	"AVATAR_URL%d=%s"


//#define DB_POOL_NAME	"teamtalk_master"
//#define DB_SERVR_IP	"127.0.0.1"
//#define DB_SERVER_PORT	3306
//#define DB_NAME "teamtalk"
//#define USER_NAME	"root"
//#define USER_PASSWORD	"12345"
//#define MAX_CONN_COUNT	16

typedef list<string> LIST_STRING;
typedef LIST_STRING::iterator  IT_LIST_STRING;


typedef struct UserInfo
{
	string m_strUserID;
	string m_strUserName;
	string m_strUserCode;
	string m_strDomainID;
	string m_strAvatarUrl;
	string m_strName;
	int m_nSex;
	string m_strPhone;
	int m_nStatus;
	string m_strKeyword;
	string m_strBirthday;
	string m_strPassword;
	string m_strDepartmentID;
	//string m_strUserCode;
	string m_strAliasName;
	string m_strBuyProducts;
	//unsigned int m_unUpdatedTime;
	int m_nContactStatus;
	string m_strOriginalPassword;
	string m_strHistoryPassword;
	int m_nIsAdmin;
	string m_strRemark;
	int m_nAdminStatus;
	unsigned int m_nBeginTime;
	unsigned int m_nExpiredTime;
	unsigned int m_nUpdateTime;
	int m_nIsIPLimit;
	unsigned int m_nLastModifyTime;
	int m_nLock;
	string m_strLockIP;
	unsigned int m_nLockStartTime;
	unsigned int m_nLockEndTime;
	string m_strNickName;
	string m_strSignature;
	string m_strTmpPassword;
}USER_INFO;


typedef list<USER_INFO> LIST_USER_INFO;
typedef LIST_USER_INFO::iterator IT_LIST_USER_INFO;


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
	string m_strDomainID;
	string m_strDomainName;
	string m_strCompanyName;
	string m_strSyncServer;
	int m_nStatus;
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

class CDataSync
{
	public:
		CDataSync();
		~CDataSync();
		void SetConnection(CDBConn* pInDBConn, CacheConn* pInGroupMember, CacheConn* pInCacheUser);
		int ParseJsonFromFile(const char *pszFileName, LIST_DOMAIN_INFO& refDomainInfo, LIST_COMPANY_INFO& refCompanyInfo, LIST_DEPARTMENT_INFO& refDepartmentInfo, LIST_USER_INFO& refUserInfo,
			LIST_DOMAIN_GROUP& refDomainGroup, LIST_GROUP_USER& refGroupUser);
		int ParseJsonFromString(string& strInputString, LIST_DOMAIN_INFO& refDomainInfo, LIST_COMPANY_INFO& refCompanyInfo, LIST_DEPARTMENT_INFO& refDepartmentInfo, LIST_USER_INFO& refUserInfo,
			LIST_DOMAIN_GROUP& refDomainGroup, LIST_GROUP_USER& refGroupUser);
		void ImportCompanyInfo(LIST_COMPANY_INFO& refCompanyInfo, LIST_DOMAIN_INFO& refDomainInfo);
		void ImportDepartmentInfo(LIST_DEPARTMENT_INFO& refDepartmentInfo);
		void ImportUserInfo(LIST_USER_INFO& refUserInfo);
		void ImportUserPosition(LIST_USER_INFO& refUserInfo);
		void CheckSyncData(void);
		void ImportDomainGroup(LIST_DOMAIN_GROUP& refDomainGroup);
		void ImportGroupUser(LIST_GROUP_USER& refGroupUser);
		void AddDomainUser(USER_INFO& UserInfo);
		int  GetUserInfo(string& strInputString, USER_INFO& UserInfo);
		void UpdateDomainUser(USER_INFO& UserInfo);
		void DeleteDomainUser(string& strUserID);
		int GetUserDetails(string& strInputString, LIST_USER_DETAILS& listUserDetails);
		void AddUserDetails(LIST_USER_DETAILS& listUserDetails);
		void SetUserDetails(LIST_USER_DETAILS& listUserDetails);
		void UpdateUserDetails(LIST_USER_DETAILS& listUserDetails);
		int GetDepartInfo(string& strInputString, DEPARTMENT_INFO& DepartInfo);
		void AddDomainDepart(DEPARTMENT_INFO& DepartInfo);
		void UpdateDomainDepart(DEPARTMENT_INFO& DepartInfo);
		void DeleteDomainDepart(string& strDepartID);
		int GetPositionInfo(string& strInputString, POSITION_INFO& PositionInfo);
		void AddDomainPosition(POSITION_INFO& PositionInfo);
		void UpdateDomainPosition(POSITION_INFO& PositionInfo);
		void DeleteDomainPosition(string& strPositionID);
		void AddUserPosition(string& strDepartID, string& strPositionID, string& strUserID);
		void DeleteUserPosition(string& strDepartID, string& strPositionID, string& strUserID);
		void DeleteUserPositionByDepartmentID(string& strDepartID);
		void DeleteUserPositionByPositionID(string& strPositionID);
		void DeleteUserPositionByDepartmentIDAndPositionID(string& strDepartmentID, string& strPositionID);
		void GetUserProducts(string& strInputString);
		void DeleteDpeartmentByDepartmentID(string& strDepartID);
		void DeletePositionByPositionID(string& strPositionID);
		void DeletePositionByDomainID(string& strDomainID);
		void DeleteDepartmentAndPosition(string& strDepartID, string& strPositionID);
		void DeleteDepartIDAndUserID(string& strOrgID, string& strUserID);
		void DeletePositionIdAndUserId(string& strPositionId, string& strUserId);
		void GetDomainInfo(string& strInputString, DOMAIN_INFO& DomainInfo);
		void GetDomainByDomainId(string& strInputString, DOMAIN_INFO& DomainInfo);
		void AddNewDomain(DOMAIN_INFO& DomainInfo);
		void UpdateDomain(DOMAIN_INFO& DomainInfo);
		void UpdateDomainByDomainId(DOMAIN_INFO& DomainInfo);
		int GetGroupInfo(string& strInputString, DOMAIN_GROUP& GroupInfo, list<string>& listUsers);
		void AddNewMailGroupAndMembers(DOMAIN_GROUP& GroupInfo, list<string>& listUsers);
		void UpdateMailGroup(DOMAIN_GROUP& GroupInfo);
		void DeleteMailGroup(string& strGroupID);
		int GetUserAlias(const string& strInputString, USER_ALIAS& Alias);
		void AddUserAlias(USER_ALIAS& Alias);
		void UpdateUserAlias(USER_ALIAS& Alias);
		int GetUserAllAlias(string& strInputString, LIST_USER_ALIAS& listAlias);
		void SetUserAllAlias(LIST_USER_ALIAS& listAlias);
		void DeleteUserAliasByAliasId(const string& strAliasId);
		void DeleteUserAliasByUserId(const string& strUserId);
		void DeleteUserAliasByAliasName(const string& strAliasName);
		int GetMailGroupUser(string& strInputString, list<string>& listUsers, string& strGroupID);
		void SetMailGroupUser(string& strGroupID, list<string>& listUsers);
		//void DeleteMailGroupUser(string& strGroupID, list<string>& listUsers);
		void SetAllUserStatus(void);
		void ChangeUserInfo(uint32_t unUserID, string& strUserID, string& strUserInfo);
		void SetAllGroupUserStatus(void);

	protected:
		CDBConn* m_pDBConn;
		CacheConn* m_pCacheGroupMember;
		CacheConn* m_pCacheUser;
		void SetUserStatus(uint32_t unUserID, int32_t unStatus);
		void SetGroupUserStatus(uint32_t unGroupID, uint32_t unUserID, uint32_t unStatus, uint32_t nCreateTime);
		int GetDefaultAvatarUrl(char** ppAvatarUrl);
		int GetDomainUser(Json::Value &root, const char* pszParent, int nLevel, string& strCompanyID, LIST_DEPARTMENT_INFO& DepartmentInfo, LIST_USER_INFO& UserInfo);
		int GetCompanyByDomain(string& strDomainID);
		void CheckUser(string& strUserID);
		void SetGroupUserStatus(string& strGroupID);
};
#endif