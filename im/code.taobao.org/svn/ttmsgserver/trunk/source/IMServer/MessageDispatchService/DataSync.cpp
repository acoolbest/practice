#include "DataSync.h"
#include "IM.Buddy.pb.h"
#include "IM.Group.pb.h"
#include "RouteServConn.h"
#include "../db_proxy_server/business/Common.h"

using namespace IM::BaseDefine;
using namespace SYNC;

MAP_COMPANY_USER CDataSync::m_mapDomainUser;
LIST_UINT CDataSync::m_listUser;

CDataSync::CDataSync()
{
	m_pDBConn = NULL;
	m_pCacheGroupMember = NULL;
	m_pCacheUser = NULL;
	m_pCacheUnreadMsg = NULL;
	m_nCompanyId = 0;
}


CDataSync::~CDataSync()
{
	if (m_pDBConn)
	{
		m_pDBConn = NULL;
	}

	if (m_pCacheGroupMember)
	{
		m_pCacheGroupMember = NULL;
	}

	if (m_pCacheUser)
	{
		m_pCacheUser = NULL;
	}

	if (m_pCacheUnreadMsg)
	{
		m_pCacheUnreadMsg = NULL;
	}
}


void CDataSync::SetConnection(CDBConn* pInDBConn, CacheConn* pInGroupMember, CacheConn* pInCacheUser, CacheConn* pInCacheUnread)
{
	m_pDBConn = pInDBConn;
	m_pCacheGroupMember = pInGroupMember;
	m_pCacheUser = pInCacheUser;
	m_pCacheUnreadMsg = pInCacheUnread;
}


void CDataSync::SetUserStatus(uint32_t unUserID, int32_t unStatus)
{
	if (m_pCacheUser)
	{
		string strKey = "user_" + int2string(unUserID);
		string strStatus = int2string(unStatus);
		//m_pCacheUser->set(strKey, int2string(unStatus));
		log("Set user %s 's status: %s", strKey.c_str(), strStatus.c_str());
		m_pCacheUser->set(strKey, strStatus);
	}
}


//structure USER_INFO is replaced with a class for later version
int  CDataSync::GetUserInfo(string& strInputString, USER_INFO& UserInfo)
{
	Json::Reader reader;
	Json::Value Root;

	if (reader.parse(strInputString, Root))
	{
		//string strResult = "";
	
		if (!Root["userId"].isNull() && Root["userId"].isString())
		{
			UserInfo.m_strUserUuid = Root["userId"].asString();
		}

		if (!Root["username"].isNull() && Root["username"].isString())
		{
			UserInfo.m_strUserName = Root["username"].asString();
		}
		else
		{
			UserInfo.m_strUserName = "";
		}

		if (!Root["fullName"].isNull() && Root["fullName"].isString())
		{
			UserInfo.m_strName = Root["fullName"].asString();
		}
		else
		{
			UserInfo.m_strName = "";
		}

		if (!Root["email"].isNull() && Root["email"].isString())
		{
			UserInfo.m_strEmail = Root["email"].asString();
		}
		else
		{
			UserInfo.m_strEmail = "";
		}

		if (!Root["password"].isNull() && Root["password"].isString())
		{
			UserInfo.m_strPassword = Root["password"].asString();
		}

		if (!Root["originalPassword"].isNull() && Root["originalPassword"].isString())
		{
			UserInfo.m_strOriginalPassword = Root["originalPassword"].asString();
		}
		else
		{
			UserInfo.m_strOriginalPassword = "";
		}

		if (!Root["historyPassword"].isNull() && Root["historyPassword"].isString())
		{
			UserInfo.m_strHistoryPassword = Root["historyPassword"].asString();
		}
		else
		{
			UserInfo.m_strHistoryPassword = "";
		}


		if (!Root["isAdmin"].isNull() && Root["isAdmin"].isInt())
		{
			UserInfo.m_nIsAdmin = Root["isAdmin"].asInt();
		}
		else
		{
			UserInfo.m_nIsAdmin = 0;
		}

		if (!Root["headPortrait"].isNull() && Root["headPortrait"].isString())
		{
			UserInfo.m_strAvatar = Root["headPortrait"].asString();
		}
		else
		{
			UserInfo.m_strAvatar = "";
		}

		if (!Root["sex"].isNull() && Root["sex"].isInt())
		{
			UserInfo.m_nSex = Root["sex"].asInt();
		}
		else
		{
			UserInfo.m_nSex = 2;
		}

		int nBirthday = 0;
		if (!Root["birthday"].isNull() && Root["birthday"].isInt64())
		{
			nBirthday = Root["birthday"].asInt64() / 1000;
		}

		UserInfo.m_unBirthday = nBirthday;

	/*	char szBirthday[32];
		memset(szBirthday, 0, sizeof(szBirthday));
		strftime(szBirthday, sizeof(szBirthday)-1, "%Y-%m-%d", localtime(&nBirthday));
		UserInfo.m_strBirthday = szBirthday;*/

		if (!Root["phone"].isNull() && Root["phone"].isString())
		{
			UserInfo.m_strPhone = Root["phone"].asString();
		}
		else
		{
			UserInfo.m_strPhone = "";
		}

		if (!Root["remark"].isNull() && Root["remark"].isString())
		{
			UserInfo.m_strRemark = Root["remark"].asString();
		}
		else
		{
			UserInfo.m_strRemark = "";
		}

	/*	if (!Root["adminStatus"].isNull() && Root["adminStatus"].isInt())
		{
			UserInfo.m_nAdminStatus = Root["adminStatus"].asInt();
		}
		else
		{
			UserInfo.m_nAdminStatus = 0;
		}*/

		if (!Root["isActivate"].isNull() && Root["isActivate"].isInt())
		{
			UserInfo.m_unIsActived = Root["isActivate"].asInt();
		}
		else
		{
			UserInfo.m_unIsActived = 0;
		}

		if (!Root["status"].isNull() && Root["status"].isInt())
		{
			UserInfo.m_nStatus = Root["status"].asInt();
		}
		else
		{
			UserInfo.m_nStatus = 1;
		}

		if (!Root["beginTime"].isNull() && Root["beginTime"].isInt64())
		{
			UserInfo.m_nBeginTime = Root["beginTime"].asInt64() / 1000;
		}
		else
		{
			UserInfo.m_nBeginTime = 0;
		}

		if (!Root["expiredTime"].isNull() && Root["expiredTime"].isInt64())
		{
			UserInfo.m_nExpiredTime = Root["expiredTime"].asInt64() / 1000;
		}
		else
		{
			UserInfo.m_nExpiredTime = 0;
		}

		if (!Root["updateTime"].isNull() && Root["updateTime"].isInt64())
		{
			UserInfo.m_nUpdateTime = Root["updateTime"].asInt64() / 1000;
		}
		else
		{
			UserInfo.m_nUpdateTime = 0;
		}

	/*	if (!Root["isIpLimit"].isNull() && Root["isIpLimit"].isInt())
		{
			UserInfo.m_nIsIPLimit = Root["isIpLimit"].asInt();
		}
		else
		{
			UserInfo.m_nIsIPLimit = 0;
		}*/

		if (!Root["lastModifiedTime"].isNull() && Root["lastModifiedTime"].isInt64())
		{
			UserInfo.m_nLastModifiedTime = Root["lastModifiedTime"].asInt64() / 1000;
		}
		else
		{
			UserInfo.m_nLastModifiedTime = 0;
		}

		if (!Root["isLock"].isNull() && Root["isLock"].isInt())
		{
			UserInfo.m_nLock = Root["isLock"].asInt();
		}
		else
		{
			UserInfo.m_nLock = 0;
		}


		if (!Root["isInitPassword"].isNull() && Root["isInitPassword"].isInt())
		{
			UserInfo.m_unInitPasswordStatus = Root["isInitPassword"].asInt();
		}
		else
		{
			UserInfo.m_unInitPasswordStatus = 0;
		}


		if (!Root["lockIp"].isNull() && Root["lockIp"].isString())
		{
			UserInfo.m_strLockIP = Root["lockIp"].asString();
		}
		else
		{
			UserInfo.m_strLockIP = "";
		}

		if (!Root["lockStartTime"].isNull() && Root["lockStartTime"].isInt64())
		{
			UserInfo.m_nLockStartTime = Root["lockStartTime"].asInt64();
		}
		else
		{
			UserInfo.m_nLockStartTime = 0;
		}

		if (!Root["lockEndTime"].isNull() && Root["lockEndTime"].isInt64())
		{
			UserInfo.m_nLockEndTime = Root["lockEndTime"].asInt64();
		}
		else
		{
			UserInfo.m_nLockEndTime = 0;
		}

		if (!Root["keyword"].isNull() && Root["keyword"].isString())
		{
			UserInfo.m_strKeyword = Root["keyword"].asString();
		}
		else
		{
			UserInfo.m_strKeyword = "";
		}

		if (!Root["signature"].isNull() && Root["signature"].isString())
		{
			UserInfo.m_strSignature = Root["signature"].asString();
		}
		else
		{
			UserInfo.m_strSignature = "";
		}

		if (!Root["nickname"].isNull() && Root["nickname"].isString())
		{
			UserInfo.m_strNickName = Root["nickname"].asString();
		}
		else
		{
			UserInfo.m_strNickName = "";
		}

		if (!Root["salt"].isNull() && Root["salt"].isString())
		{
			UserInfo.m_strSalt = Root["salt"].asString();
		}
		else
		{
			UserInfo.m_strSalt = "";
		}

		if (!Root["account"].isNull() && Root["account"].isString())
		{
			UserInfo.m_strAccount = Root["account"].asString();
		}
		else
		{
			UserInfo.m_strAccount = "";
		}
	}
	else
	{
		return -1;
	}
}


void CDataSync::AddUser(USER_INFO& UserInfo)
{
	string strSql = "select id from IMUser where useruuid='" + UserInfo.m_strUserUuid + "'";
	CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());

	uint32_t unUserId = 0;
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			unUserId = pResultSet->GetInt("id");
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	if (unUserId == 0)
	{
		strSql = "INSERT INTO IMUser (useruuid,username,name,password,origpassword,histpassword,avatar,sex,birthday,phone,status,begintime,expiredtime,updatedtime,lastmodifiedtime,";
		strSql += "islock,lockip,lockstarttime,lockendtime,keyword,remark,nick,signature,buyproduct,created,updated,isinitpassword,email,isactived,salt,account) ";
		strSql += "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";

		//bool bSuccess = false;
		CPrepareStatement* pStmt = new CPrepareStatement();
		if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
		{
			int nIndex = 0;
			unsigned int unCurrenTime = time(NULL);
			pStmt->SetParam(nIndex++, UserInfo.m_strUserUuid);
			pStmt->SetParam(nIndex++, UserInfo.m_strUserName);
			pStmt->SetParam(nIndex++, UserInfo.m_strName);
			pStmt->SetParam(nIndex++, UserInfo.m_strPassword);
			pStmt->SetParam(nIndex++, UserInfo.m_strOriginalPassword);
			pStmt->SetParam(nIndex++, UserInfo.m_strHistoryPassword);
			pStmt->SetParam(nIndex++, UserInfo.m_strAvatar);
			pStmt->SetParam(nIndex++, UserInfo.m_nSex);
			pStmt->SetParam(nIndex++, UserInfo.m_unBirthday);
			pStmt->SetParam(nIndex++, UserInfo.m_strPhone);
			pStmt->SetParam(nIndex++, UserInfo.m_nStatus);
			pStmt->SetParam(nIndex++, UserInfo.m_nBeginTime);
			pStmt->SetParam(nIndex++, UserInfo.m_nExpiredTime);
			pStmt->SetParam(nIndex++, UserInfo.m_nUpdateTime);
			pStmt->SetParam(nIndex++, UserInfo.m_nLastModifiedTime);
			pStmt->SetParam(nIndex++, UserInfo.m_nLock);
			pStmt->SetParam(nIndex++, UserInfo.m_strLockIP);
			pStmt->SetParam(nIndex++, UserInfo.m_nLockStartTime);
			pStmt->SetParam(nIndex++, UserInfo.m_nLockEndTime);
			pStmt->SetParam(nIndex++, UserInfo.m_strKeyword);
			pStmt->SetParam(nIndex++, UserInfo.m_strRemark);	
			pStmt->SetParam(nIndex++, UserInfo.m_strNickName);
			pStmt->SetParam(nIndex++, UserInfo.m_strSignature);
			pStmt->SetParam(nIndex++, UserInfo.m_strBuyProducts);
			pStmt->SetParam(nIndex++, unCurrenTime);
			pStmt->SetParam(nIndex++, unCurrenTime);
			pStmt->SetParam(nIndex++, UserInfo.m_unInitPasswordStatus);
			pStmt->SetParam(nIndex++, UserInfo.m_strEmail);
			pStmt->SetParam(nIndex++, UserInfo.m_unIsActived);
			pStmt->SetParam(nIndex++, UserInfo.m_strSalt);
			pStmt->SetParam(nIndex++, UserInfo.m_strAccount);

			pStmt->ExecuteUpdate();
			unUserId = pStmt->GetInsertId();
			log("Add a new user; %d", unUserId);
			int nStatus = UserInfo.m_nStatus;

			//comment by george on June 1st,2016
			//nStatus = GetStatusByProduct(UserInfo.m_strBuyProducts, nStatus); //get status by products
			SetUserStatus(unUserId, nStatus);
			//bSuccess = true;

			m_listUser.push_back(unUserId);
		}

		delete pStmt;
		pStmt = NULL;

		////if(bSuccess)
		//{
		//	SendUserAddNotify(unUserId, UserInfo);
		//}
	}
	else
	{
		log("User %s is already exists", UserInfo.m_strUserUuid.c_str());
	}
}


void CDataSync::UpdateUser(USER_INFO& UserInfo)
{
	uint32_t unUserId = 0;
	string strPassword = "";
	int nStatus = 1;

	string strSql = "select id, status, buyproduct,password from IMUser where useruuid='" + UserInfo.m_strUserUuid + "'";
	CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			unUserId = pResultSet->GetInt("id");
			strPassword = pResultSet->GetString("password");
			nStatus = pResultSet->GetInt("status");
			if (pResultSet->GetString("buyproduct") != NULL)
			{
				UserInfo.m_strBuyProducts = pResultSet->GetString("buyproduct");
			}
		}

		delete pResultSet;
		pResultSet = NULL;
	}

	//update all fields currently
	strSql = "update IMUser set username = '" + UserInfo.m_strUserName + "',name = '" + UserInfo.m_strName;
	strSql += "',password = '" + UserInfo.m_strPassword + "',origpassword = '" + UserInfo.m_strOriginalPassword + "',histpassword = '" + UserInfo.m_strHistoryPassword;
	strSql += "',avatar = '" + UserInfo.m_strAvatar + "',sex=" + int2string(UserInfo.m_nSex) + ",birthday =" + int2string(UserInfo.m_unBirthday) + ",phone = '" + UserInfo.m_strPhone;
	strSql += "',status= " + int2string(UserInfo.m_nStatus) + ",begintime=" + int2string(UserInfo.m_nBeginTime) + ",expiredtime=" + int2string(UserInfo.m_nExpiredTime);
	strSql += ",updatedtime=" + int2string(UserInfo.m_nUpdateTime) + ",lastmodifiedtime=" + int2string(UserInfo.m_nLastModifiedTime) + ",islock =" + int2string(UserInfo.m_nLock);
	strSql += + ",lockip='" + UserInfo.m_strLockIP + "',lockstarttime=" + int2string(UserInfo.m_nLockStartTime) + ",lockendtime=" + int2string(UserInfo.m_nLockEndTime);
	strSql += ",keyword='" + UserInfo.m_strKeyword + "',remark='" + UserInfo.m_strRemark + "',isinitpassword=" + int2string(UserInfo.m_unInitPasswordStatus);
	strSql += ",nick='" + UserInfo.m_strNickName + "',signature='" + UserInfo.m_strSignature + "',updated=unix_timestamp(),email='" + UserInfo.m_strEmail + "',isactived=" + int2string(UserInfo.m_unIsActived);
	strSql += ",salt='";
	strSql += UserInfo.m_strSalt + "',account='" + UserInfo.m_strAccount;
	strSql += "' where useruuid = '" + UserInfo.m_strUserUuid + "'";
	//strSql += ",keyword='" + UserInfo.m_strKeyword + "',remark='" + UserInfo.m_strRemark + "',tmppassword='" + UserInfo.m_strTmpPassword + "',updated=unix_timestamp() where useruuid='" + UserInfo.m_strUserID + "'";
	//if (UserInfo.m_strRemark != "")
	//{
	//	strSql += ",remark='";
	//	strSql += UserInfo.m_strRemark;
	//	strSql += "',";
	//}


	log("SQL for updating a user info: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	CheckUser(UserInfo.m_strUserUuid);

	if (strPassword != UserInfo.m_strPassword)
	{
		SendPasswordChangeNotify(unUserId);
	}
	else
	{
		SendUserUpdateNotify(unUserId, nStatus, UserInfo);
	}

	if (UserInfo.m_nStatus != 1)
	{
		DeleteUser(unUserId);
	}
}


void CDataSync::DeleteUser(string& strUserID)
{
	uint32_t unUserId = 0;
	string strSql = "select id from IMUser where useruuid='" + strUserID + "'";
	CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if(pResultSet)
	{
		while(pResultSet->Next())
		{
			unUserId = pResultSet->GetInt("id");
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	strSql = "update IMUser set status=-1,updated=unix_timestamp() where useruuid='" + strUserID + "'";
	log("SQL for deleting a user: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());


	DeleteUser(unUserId);

	////delete session
	//strSql = "update IMRecentSession set status=1,updated=unix_timestamp() where (userid=" + int2string(unUserId) + " or peerid=" + int2string(unUserId) + " and type=1) and status = 0";
	//log("SQL for deleting session for user: %d", unUserId);
	//m_pDBConn->ExecuteQuery(strSql.c_str());

	CheckUser(strUserID);

	SendUserDelNotify(unUserId, strUserID);
}


int CDataSync::GetUserDetails(string& strInputString, LIST_USER_DETAILS& listUserDetails)
{
	Json::Reader reader;
	Json::Value root;

	if (reader.parse(strInputString, root))
	{
		try
		{
			int nCount = root.size();
			for (int i = 0; i < nCount; i++)
			{
				USER_DETAILS UserDetails;

				Json::Value Details = root[i];
				if (!Details["userDetailsId"].isNull() && Details["userDetailsId"].isString())
				{
					UserDetails.m_strUUID = Details["userDetailsId"].asString();
				}

				if (!Details["userId"].isNull() && Details["userId"].isString())
				{
					UserDetails.m_strUserUUID = Details["userId"].asString();
				}

				if (!Details["type"].isNull() && Details["type"].isInt())
				{
					UserDetails.m_nType = Details["type"].asInt();
				}

				if (!Details["title"].isNull() && Details["title"].isString())
				{
					UserDetails.m_strTitle = Details["title"].asString();
				}

				if (!Details["content"].isNull() && Details["content"].isString())
				{
					UserDetails.m_strContent = Details["content"].asString();
				}

				listUserDetails.push_back(UserDetails);
			}
		}
		catch (std::runtime_error msg)
		{
			log("userdetails,parse json data failed.");
		}
	}

	return 1;

	//if (reader.parse(strInputString, root))
	//{
	//	string strResult = "";
	//	if (!root["resultCode"].isNull() && root["resultCode"].isString())
	//	{
	//		strResult = root["resultCode"].asString();
	//	}

	//	if (strResult != "0000")
	//	{
	//		return -1;
	//	}

	//	if (!root["data"].isNull())
	//	{
	//		Json::Value msgData = root["data"];
	//		int nCount = msgData.size();
	//		for (int i = 0; i < nCount; i++)
	//		{
	//			USER_DETAILS UserDetails;
	//			//if (!msgData[i]["userDetail"].isNull() && msgData[i]["userDetail"].isArray())
	//			{
	//				//Json::Value Details = msgData[i]["userDetail"];
	//				Json::Value Details = msgData[i];
	//				if (!Details["userDetailsId"].isNull() && Details["userDetailsId"].isString())
	//				{
	//					UserDetails.m_strUUID = Details["userDetailsId"].asString();
	//				}

	//				if (!Details["userId"].isNull() && Details["userId"].isString())
	//				{
	//					UserDetails.m_strUserUUID = Details["userId"].asString();
	//				}

	//				if (!Details["type"].isNull() && Details["type"].isInt())
	//				{
	//					UserDetails.m_nType = Details["type"].asInt();
	//				}

	//				if (!Details["title"].isNull() && Details["title"].isString())
	//				{
	//					UserDetails.m_strTitle = Details["title"].asString();
	//				}

	//				if (!Details["content"].isNull() && Details["content"].isString())
	//				{
	//					UserDetails.m_strContent = Details["content"].asString();
	//				}

	//				listUserDetails.push_back(UserDetails);
	//			}
	//		}

	//	}
	//}
	//return 1;
}


void CDataSync::AddUserDetails(LIST_USER_DETAILS& listUserDetails)
{
	string strSql = "insert into UserDetails (uuid,useruuid,type,title,content,created,updated) values(?,?,?,?,?,?,?)";
	// 必须在释放连接前delete CPrepareStatement对象，否则有可能多个线程操作mysql对象，会crash

	string strUserId;
	for (IT_LIST_USER_DETAILS it = listUserDetails.begin(); it != listUserDetails.end(); it++)
	{
		USER_DETAILS UserDetails = *it;
		strUserId = it->m_strUserUUID;
		
		CPrepareStatement* pStmt = new CPrepareStatement();
		if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
		{
			int nIndex = 0;
			unsigned int nCurrenTime = time(NULL);
			pStmt->SetParam(nIndex++, UserDetails.m_strUUID);
			pStmt->SetParam(nIndex++, UserDetails.m_strUserUUID);
			pStmt->SetParam(nIndex++, UserDetails.m_nType);
			pStmt->SetParam(nIndex++, UserDetails.m_strTitle);
			pStmt->SetParam(nIndex++, UserDetails.m_strContent);
			pStmt->SetParam(nIndex++, nCurrenTime);
			pStmt->SetParam(nIndex++, nCurrenTime);
			pStmt->ExecuteUpdate();
		}
		delete pStmt;
	}

	SendUserDetailsChangeNotify(strUserId, listUserDetails);
}


void CDataSync::SetUserDetails(string& strUserUuid, LIST_USER_DETAILS& listUserDetails)
{
	//delete all additional information first
	string strSql = "delete from UserDetails where useruuid='" + strUserUuid + "'";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "insert into UserDetails (uuid,useruuid,type,title,content,created,updated) values(?,?,?,?,?,?,?)";
	// 必须在释放连接前delete CPrepareStatement对象，否则有可能多个线程操作mysql对象，会crash

	CPrepareStatement* pStmt = new CPrepareStatement();
	if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
	{
		for (IT_LIST_USER_DETAILS it = listUserDetails.begin(); it != listUserDetails.end(); it++)
		{
			int nIndex = 0;
			unsigned int nCurrenTime = time(NULL);
			USER_DETAILS UserDetails = *it;

			pStmt->SetParam(nIndex++, UserDetails.m_strUUID);
			pStmt->SetParam(nIndex++, UserDetails.m_strUserUUID);
			pStmt->SetParam(nIndex++, UserDetails.m_nType);
			pStmt->SetParam(nIndex++, UserDetails.m_strTitle);
			pStmt->SetParam(nIndex++, UserDetails.m_strContent);
			pStmt->SetParam(nIndex++, nCurrenTime);
			pStmt->SetParam(nIndex++, nCurrenTime);
			pStmt->ExecuteUpdate();
		}
	}

	delete pStmt;
	log("ready to send notify for change user details");
	SendUserDetailsChangeNotify(strUserUuid, listUserDetails);

	UpdateUserChangeTime(strUserUuid);

	/*	for (; it != listUserDetails.end(); it++)
		{
			CPrepareStatement* pStmt = new CPrepareStatement();
			if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
			{
				int nIndex = 0;
				unsigned int nCurrenTime = time(NULL);
				USER_DETAILS UserDetails = *it;
				strUserId = UserDetails.m_strUserUUID;
				pStmt->SetParam(nIndex++, UserDetails.m_strUUID);
				pStmt->SetParam(nIndex++, UserDetails.m_strUserUUID);
				pStmt->SetParam(nIndex++, UserDetails.m_nType);
				pStmt->SetParam(nIndex++, UserDetails.m_strTitle);
				pStmt->SetParam(nIndex++, UserDetails.m_strContent);
				pStmt->SetParam(nIndex++, nCurrenTime);
				pStmt->SetParam(nIndex++, nCurrenTime);
				pStmt->ExecuteUpdate();
			}
			delete pStmt;
		}

		log("ready to send notify for change user details");
		SendUserDetailsChangeNotify(listUserDetails);*/
	//	string strSql = "truncate table UserDetailsTemp";
	//	m_pDBConn->ExecuteUpdate(strSql.c_str());

	//	strSql = "insert into UserDetailsTemp (uuid,useruuid,type,title,content,created,updated) values(?,?,?,?,?,?,?)";
	//	// 必须在释放连接前delete CPrepareStatement对象，否则有可能多个线程操作mysql对象，会crash
	//	string strUserId = "";
	//	for (IT_LIST_USER_DETAILS it = listUserDetails.begin(); it != listUserDetails.end(); it++)
	//	{
	//		CPrepareStatement* pStmt = new CPrepareStatement();
	//		if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
	//		{
	//			int nIndex = 0;
	//			unsigned int nCurrenTime = time(NULL);
	//			USER_DETAILS UserDetails = *it;
	//			strUserId = UserDetails.m_strUserUUID;
	//			pStmt->SetParam(nIndex++, UserDetails.m_strUUID);
	//			pStmt->SetParam(nIndex++, UserDetails.m_strUserUUID);
	//			pStmt->SetParam(nIndex++, UserDetails.m_nType);
	//			pStmt->SetParam(nIndex++, UserDetails.m_strTitle);
	//			pStmt->SetParam(nIndex++, UserDetails.m_strContent);
	//			pStmt->SetParam(nIndex++, nCurrenTime);
	//			pStmt->SetParam(nIndex++, nCurrenTime);
	//			pStmt->ExecuteUpdate();
	//		}
	//		delete pStmt;
	//	}

	//	//strSql = "delete from UserDetailsTemp using UserDetailsTemp,(select max(id) as maxid, uuid from UserDetailsTemp group by uuid having count(*) > 1) as b where UserDetailsTemp.uuid=b.uuid and UserDetailsTemp.id <> b.maxid";
	//	//m_pDBConn->ExecuteUpdate(strSql.c_str());

	//	strSql = "update UserDetails a, UserDetailsTemp b set a.type=b.type,a.title=b.title,a.content=b.content,a.updated=unix_timestamp() where (a.uuid=b.uuid) and (a.type <> b.type or a.title <> b.title or a.content <> b.content)";
	//	m_pDBConn->ExecuteUpdate(strSql.c_str());

	//	strSql = "insert into UserDetails (uuid,useruuid,type,title,content,created,updated) select uuid,useruuid,type,title,content,created,updated from UserDetailsTemp a where not exists(select 1 from UserDetails b where a.uuid=b.uuid);";
	//	m_pDBConn->ExecuteUpdate(strSql.c_str());

	//	strSql = "delete from UserDetails where useruuid='" + strUserId + "' and not exists(select 1 from UserDetailsTemp where UserDetailsTemp.uuid = UserDetails.uuid)";
	//	m_pDBConn->ExecuteUpdate(strSql.c_str());

	//	strSql = "update IMUser set updated=unix_timestamp() where useruuid='" + strUserId + "'";
	//	m_pDBConn->ExecuteUpdate(strSql.c_str());

	//	log("ready to send notify for change user details");
	//	SendUserDetailsChangeNotify(listUserDetails);
	//}
}


void CDataSync::UpdateUserDetails(LIST_USER_DETAILS& listUserDetails)
{
	for (IT_LIST_USER_DETAILS it = listUserDetails.begin(); it != listUserDetails.end(); it++)
	{
		USER_DETAILS UserDetails = *it;
		string strSql = "update UserDetails set type = " + int2string(UserDetails.m_nType) + ",title=' " + UserDetails.m_strTitle + "',content='" + UserDetails.m_strContent + "'updated=unix_timestamp() where uuid='" + UserDetails.m_strUUID + "'";
		log("SQL for updating a user details: %s", strSql.c_str());
		m_pDBConn->ExecuteUpdate(strSql.c_str());
	}

	//SendUserDetailsChangeNotify(listUserDetails);
}


int CDataSync::GetDepartInfo(string& strInputString, DEPARTMENT_INFO& DepartInfo)
{
	
	{
		Json::Reader Reader;
		Json::Value Root;
		if (Reader.parse(strInputString, Root))
		{
			string strResultCode = "";
			if (!Root["resultCode"].isNull() && Root["resultCode"].isString())
			{
				strResultCode = Root["resultCode"].asString();
			}

			if (strResultCode != "0000")
			{
				return -1;
			}

			if (Root["data"].isNull())
			{
				return -1;
			}

			Json::Value Department = Root["data"];
			if (!Department["orgId"].isNull() && Department["orgId"].isString())
			{
				DepartInfo.m_strDepartmentID = Department["orgId"].asString();
			}
			else
			{
				DepartInfo.m_strDepartmentID = "";
			}

			if (!Department["primaryOrgId"].isNull() && Department["primaryOrgId"].isString())
			{
				DepartInfo.m_strDomainID = Department["primaryOrgId"].asString();
			}
			else
			{
				DepartInfo.m_strDomainID = "";
			}

			if (!Department["orgName"].isNull() && Department["orgName"].isString())
			{
				DepartInfo.m_strDepartmentName = Department["orgName"].asString();
			}
			else
			{
				DepartInfo.m_strDepartmentName = "";
			}

			if (!Department["parentId"].isNull() && Department["parentId"].isString())
			{
				DepartInfo.m_strParentDepartmentID = Department["parentId"].asString();
				if (DepartInfo.m_strParentDepartmentID == "-1")
				{
					DepartInfo.m_strParentDepartmentID = "";
				}
			}
			else
			{
				DepartInfo.m_strParentDepartmentID = "";
			}

			if (!Department["level"].isNull() && Department["level"].isInt())
			{
				DepartInfo.m_nLevel = Department["level"].asInt();
			}
			else
			{
				DepartInfo.m_nLevel = 0;
			}

			if (!Department["sort"].isNull() && Department["sort"].isInt())
			{

				DepartInfo.m_nPriority = Department["sort"].asInt();
			}
			else
			{
				DepartInfo.m_nPriority = 65535;
			}

			if (!Department["path"].isNull() && Department["path"].isString())
			{
				DepartInfo.m_strDepartPath = Department["path"].asString();
			}
			else
			{
				DepartInfo.m_strDepartPath = "";
			}

			if (!Department["isOrg"].isNull() && Department["isOrg"].isInt())
			{
				DepartInfo.m_nIsDepart = Department["isOrg"].asInt();
			}
			else
			{
				DepartInfo.m_nIsDepart = 0;
			}

			if (!Department["status"].isNull() && Department["status"].isInt())
			{
				DepartInfo.m_nStatus = Department["status"].asInt();
			}
			else
			{
				DepartInfo.m_nStatus = 0;
			}

			if (!Department["contactsStatus"].isNull() && Department["contactsStatus"].isInt())
			{
				DepartInfo.m_nContactStatus = Department["contactsStatus"].asInt();
			}
			else
			{
				DepartInfo.m_nContactStatus = 1;
			}

			if (DepartInfo.m_strParentDepartmentID == "-2") //unallocated department
			{
				DepartInfo.m_strDepartmentID = DepartInfo.m_strDomainID + DepartInfo.m_strParentDepartmentID;
			}
		}
		return 1;
	}
	
	return 1;
}


void CDataSync::AddCompanyDepart(DEPARTMENT_INFO& DepartInfo)
{
	if(GetCompanyIdByCompanyUuid(DepartInfo.m_strDomainID) == 0)
	{
		log("domain does not exists: %s", DepartInfo.m_strDomainID.c_str());
		return;
	}

	string strSql = "select uuid from IMDepart where uuid='" + DepartInfo.m_strDepartmentID + "'";
	CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	bool bExists = false;
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			bExists = true;
		}
		delete pResultSet;
		pResultSet = NULL;
	}


	if (!bExists)
	{
		bool bSuccess = false;
		strSql = "insert into IMDepart (uuid,companyid,departname,priority,parentuuid,status,created,updated,level,isdepart,departpath,contactstatus)values(?,?,?,?,?,?,?,?,?,?,?,?)";
	// 必须在释放连接前delete CPrepareStatement对象，否则有可能多个线程操作mysql对象，会crash
		CPrepareStatement* pStmt = new CPrepareStatement();
		if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
		{
			int nIndex = 0;
			unsigned int nCurrenTime = time(NULL);
			pStmt->SetParam(nIndex++, DepartInfo.m_strDepartmentID);
			pStmt->SetParam(nIndex++, m_nCompanyId);
			pStmt->SetParam(nIndex++, DepartInfo.m_strDepartmentName);
			pStmt->SetParam(nIndex++, DepartInfo.m_nPriority);
			pStmt->SetParam(nIndex++, DepartInfo.m_strParentDepartmentID);
			pStmt->SetParam(nIndex++, DepartInfo.m_nStatus);
			pStmt->SetParam(nIndex++, nCurrenTime);
			pStmt->SetParam(nIndex++, nCurrenTime);
			pStmt->SetParam(nIndex++, DepartInfo.m_nLevel);
			pStmt->SetParam(nIndex++, DepartInfo.m_nIsDepart);
			pStmt->SetParam(nIndex++, DepartInfo.m_strDepartPath);
			pStmt->SetParam(nIndex++, DepartInfo.m_nContactStatus);
			pStmt->ExecuteUpdate();

			bSuccess = true;
		}
		delete pStmt;

		if(bSuccess)
		{
			SendDepartmentChangeNotify(DepartInfo, IM::BaseDefine::DATA_SYNC_TYPE_ADD_DEPART);
		}
	}
	else
	{
		log("No company for domain: %s", DepartInfo.m_strDomainID.c_str());
	}
}


void CDataSync::UpdateCompanyDepart(DEPARTMENT_INFO& DepartInfo)
{
	if(GetCompanyIdByCompanyUuid(DepartInfo.m_strDomainID) == 0)
	{
		log("domain does not exists: %s", DepartInfo.m_strDomainID.c_str());
		return;
	}

	string strSql = "update IMDepart set departname='" + DepartInfo.m_strDepartmentName + "',priority=" + int2string(DepartInfo.m_nPriority) + ",parentuuid='" + DepartInfo.m_strParentDepartmentID + "',";
	strSql += "status=" + int2string(DepartInfo.m_nStatus) + ",updated=unix_timestamp(),level=" + int2string(DepartInfo.m_nLevel) + ",isdepart=" + int2string(DepartInfo.m_nIsDepart);
	strSql += ",departpath='" + DepartInfo.m_strDepartPath + "',contactstatus=" + int2string(DepartInfo.m_nContactStatus) + " where uuid='" + DepartInfo.m_strDepartmentID + "'";
	log("SQL for updating department: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	SendDepartmentChangeNotify(DepartInfo, IM::BaseDefine::DATA_SYNC_TYPE_MOD_DEPART);
}


void CDataSync::DeleteCompanyDepart(string& strDepartID)
{
	if(GetCompanyIdByDepartId(strDepartID) == 0)
	{
		//log("domain does not exists: %s", DepartInfo.m_strDomainID.c_str());
		return;
	}

	string strSql = "update IMDepart set status=-1,updated=unix_timestamp() where uuid='" + strDepartID + "'";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	SendRemoveDepartmentUserNotify(strDepartID);
	SendDepartmentDelNotify(strDepartID);
	
}


int CDataSync::GetPositionInfo(string& strInputString, POSITION_INFO& PositionInfo)
{
	Json::Reader Reader;
	Json::Value Root;
	if (Reader.parse(strInputString, Root))
	{
		if (!Root["resultCode"].isNull() && Root["resultCode"].isString())
		{
			string strResult = Root["resultCode"].asString();
			if (strResult != "0000")
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}

		if (!Root["data"].isNull())
		{
			Json::Value msgData = Root["data"];
			if (!msgData["positionId"].isNull() && msgData["positionId"].isString())
			{
				PositionInfo.m_strPositionID = msgData["positionId"].asString();
			}
			else
			{
				PositionInfo.m_strPositionID = "";
			}

			if (!msgData["positionName"].isNull() && msgData["positionName"].isString())
			{
				PositionInfo.m_strPositionName = msgData["positionName"].asString();
			}
			else
			{
				PositionInfo.m_strPositionName = "";
			}

			if (!msgData["primaryOrgId"].isNull() && msgData["primaryOrgId"].isString())
			{
				PositionInfo.m_strDomainID = msgData["primaryOrgId"].asString();
			}
			else
			{
				PositionInfo.m_strDomainID = "";
			}

			if (!msgData["type"].isNull() && msgData["type"].isString())
			{
				PositionInfo.m_strPositionType = msgData["type"].asString();
			}
			else
			{
				PositionInfo.m_strPositionType = "";
			}

			if (!msgData["remark"].isNull() && msgData["remark"].isString())
			{
				PositionInfo.m_strRemark = msgData["reamrk"].asString();
			}
			else
			{
				PositionInfo.m_strRemark = "";
			}

			if (!msgData["sort"].isNull() && msgData["sort"].isInt())
			{
				PositionInfo.m_nSort = msgData["sort"].asInt();
			}
			else
			{
				PositionInfo.m_nSort = 0;
			}

			if (!msgData["status"].isNull() && msgData["status"].isInt())
			{
				PositionInfo.m_nStatus = msgData["status"].asInt();
			}
			else
			{
				PositionInfo.m_nStatus = 0;
			}
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}

	return 1;
}

void CDataSync::AddCompanyPosition(POSITION_INFO& PositionInfo)
{
	if(GetCompanyIdByCompanyUuid(PositionInfo.m_strDomainID) == 0)
	{
		log("domain does not exists: %s", PositionInfo.m_strDomainID.c_str());
		return;
	}

	if (m_nCompanyId > 0)
	{
		bool bSuccess = false;
		string strSql = "insert into IMPosition (uuid,companyid,posname,type,status,remark,sort,created,updated)values(?,?,?,?,?,?,?,?,?)";
		// 必须在释放连接前delete CPrepareStatement对象，否则有可能多个线程操作mysql对象，会crash
		CPrepareStatement* pStmt = new CPrepareStatement();
		if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
		{
			int nIndex = 0;
			unsigned int nCurrenTime = time(NULL);

			pStmt->SetParam(nIndex++, PositionInfo.m_strPositionID);
			pStmt->SetParam(nIndex++, m_nCompanyId);
			pStmt->SetParam(nIndex++, PositionInfo.m_strPositionName);
			pStmt->SetParam(nIndex++, PositionInfo.m_strPositionType);
			pStmt->SetParam(nIndex++, PositionInfo.m_nStatus);
			pStmt->SetParam(nIndex++, PositionInfo.m_strRemark);
			pStmt->SetParam(nIndex++, PositionInfo.m_nSort);
			pStmt->SetParam(nIndex++, nCurrenTime);
			pStmt->SetParam(nIndex++, nCurrenTime);
			pStmt->ExecuteUpdate();

			bSuccess = true;
		}
		delete pStmt;

		if(bSuccess)
		{
			SendPositionChangeNotify(PositionInfo, IM::BaseDefine::DATA_SYNC_TYPE_ADD_POSITION);
		}
	}
}


void CDataSync::UpdateCompanyPosition(POSITION_INFO& PositionInfo)
{
	if(GetCompanyIdByCompanyUuid(PositionInfo.m_strDomainID) == 0)
	{
		log("domain does not exists: %s", PositionInfo.m_strDomainID.c_str());
		return;
	}

	string strSql = "update IMPosition set posname='" + PositionInfo.m_strPositionName + "',type='" + PositionInfo.m_strPositionType + "',status=" + int2string(PositionInfo.m_nStatus);
	strSql += ",remark='" + PositionInfo.m_strRemark + "',sort = " + int2string(PositionInfo.m_nSort) + ",updated=unix_timestamp() where uuid='" + PositionInfo.m_strPositionID + "'";
	log("SQL for updating position: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	if (PositionInfo.m_nSort == -1)
	{
		//send notify first?
		SendPositionDelNotify(PositionInfo.m_strPositionID);
	}
	else
	{
		SendPositionChangeNotify(PositionInfo, IM::BaseDefine::DATA_SYNC_TYPE_MOD_POSITION);
	}
}


void CDataSync::DeleteCompanyPosition(string& strPositionID)
{
	if(GetCompanyIdByPositionId(strPositionID) > 0)
	{
		string strSql = "update IMPosition set status=-1,updated=unix_timestamp() where uuid='" + strPositionID + "'";
		m_pDBConn->ExecuteUpdate(strSql.c_str());

		LIST_USER_DUTY listUserDuty;
		//get departuuid 
		strSql = "select a.useruuid, a.departuuid, a.jobgrade,count(*) as acount,c.userid as uid from IMUserPosition a, (select departuuid from IMUserPosition where posuuid='" + strPositionID + "' and status = 0) as b, IMStaff c  where a.departuuid=b.departuuid and a.useruuid=c.uuid and a.status = 0 group by a.useruuid,a.departuuid";
		log("SQL for getting records for user,department and position: %s", strSql.c_str());
		CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			USER_DUTY struDuty;
			uint32_t unCount = 0;
			while (pResultSet->Next())
			{
				struDuty.m_strUserId = pResultSet->GetString("useruuid");
				struDuty.m_strDepartId = pResultSet->GetString("departuuid");
				int nCount = pResultSet->GetInt("acount");
				struDuty.m_unUserId = pResultSet->GetInt("uid");
				string strJobGrade = "";
				if (pResultSet->GetString("jobgrade") != NULL)
				{
					strJobGrade = pResultSet->GetString("jobgrade");
				}

				struDuty.m_strJobGrade = strJobGrade;

				if (nCount > 1)
				{
					struDuty.m_strPositionId = strPositionID;
				}
				else
				{
					struDuty.m_strPositionId = DATA_SYNC_DEFAULT_POSITION_UUID;
				}

				listUserDuty.push_back(struDuty);
			}

			delete pResultSet;
			pResultSet = NULL;
		}

		SendUserPositionChangeNotify(listUserDuty);
		SendPositionDelNotify(strPositionID);
	}
}


void CDataSync::AddStaffPosition(string& strDepartID, string& strPositionID, string& strUserID, string& strJobGrade)
{
	//if(GetCompanyIdByUserId(strUserID) == 0)
	if (GetCompanyIdByStaffUuid(strUserID) == 0)
	{
		return ;
	}

	uint32_t unUserId = GetUserIdByStaffUuid(strUserID); //get user id

	if (strPositionID == "")
	{
		strPositionID = DATA_SYNC_DEFAULT_POSITION_UUID;
	}

	string strSql = "select status from IMUserPosition where useruuid='" + strUserID+ "' and departuuid='" + strDepartID + "' and posuuid='" + strPositionID + "'";
	int nStatus = 1;
	log("SQL for getting status: %s", strSql.c_str());
	CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if(pResultSet)
	{
		while(pResultSet->Next())
		{
			nStatus = pResultSet->GetInt("status");
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	if(nStatus == 0)
	{
		return;
	}
	else if(nStatus == -1) //update(set status 0)
	{
		strSql = "update IMUserPosition set status = 0,jobgrade='" + strJobGrade + "',updated=unix_timestamp() where  posuuid='" + strPositionID + "' and departuuid='" + strDepartID + "' and useruuid = '" + strUserID + "'";
		log("SQL for updating position status; %s", strSql.c_str());
		m_pDBConn->ExecuteUpdate(strSql.c_str());

		SendUserPositionChangeNotify(strUserID, unUserId, strDepartID,strPositionID, IM::BaseDefine::DATA_SYNC_TYPE_ADD_USER_DUTY, strJobGrade);

		UpdateStaffChangeTime(strUserID);
	}
	else if(nStatus == 1) //new position
	{	
		strSql = "insert into IMUserPosition (companyid,departuuid,posuuid,useruuid,jobgrade,created,updated) select companyid,'" + strDepartID + "','" + strPositionID + "','" + strUserID + "','";
		strSql += strJobGrade + "',unix_timestamp(), unix_timestamp() from IMDepart where uuid='" + strDepartID + "'";
		log("SQL for inserting a record for IMUserPosition: %s", strSql.c_str());
		m_pDBConn->ExecuteUpdate(strSql.c_str());
	
		SendUserPositionChangeNotify(strUserID, unUserId, strDepartID, strPositionID, IM::BaseDefine::DATA_SYNC_TYPE_ADD_USER_DUTY, strJobGrade);

		UpdateStaffChangeTime(strUserID);

		////if the user no position previously?
		//string strSql = "select count(*) as totalcount from IMUserPosition where useruuid='" + strUserID + "' and departuuid='" + strDepartID + "' and posuuid='" + DATA_SYNC_DEFAULT_POSITION_UUID + "'";
		//int nCount = 0;
		//pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
		//if(pResultSet)
		//{
		//	while(pResultSet->Next())
		//	{
		//		nCount = pResultSet->GetInt("totalcount");
		//	}
		//	delete pResultSet;
		//	pResultSet = NULL;
		//}

		//if(nCount > 0) //update
		//{
		//	strSql = "update IMUserPosition set status = 0,updated=unix_timestamp(), posuuid='" + strPositionID + "' where departuuid='" + strDepartID + "' and useruuid = '" + strUserID + "'";
		//	log("SQL for updating position; %s", strSql.c_str());
		//	m_pDBConn->ExecuteUpdate(strSql.c_str());

		//	SendUserPositionChangeNotify(strUserID, unUserId, strDepartID, strPositionID, IM::BaseDefine::DATA_SYNC_TYPE_MOD_USER_DUTY);
		//}
		//else
		//{
		//	strSql = "insert into IMUserPosition (companyid,departuuid,posuuid,useruuid,created,updated) select companyid,'" + strDepartID + "','" + strPositionID + "','" + strUserID + "',";
		//	strSql += "unix_timestamp(), unix_timestamp() from IMDepart where uuid='" + strDepartID + "'";
		//	log("SQL for inserting a record for IMUserPosition: %s", strSql.c_str());
		//	m_pDBConn->ExecuteUpdate(strSql.c_str());

		//	SendUserPositionChangeNotify(strUserID, unUserId, strDepartID, strPositionID, IM::BaseDefine::DATA_SYNC_TYPE_ADD_USER_DUTY);

		//}
	}


	//strSql = "insert into IMUserPosition (companyid,departuuid,posuuid,useruuid,created,updated) select companyid,'" + strDepartID + "','" + strPositionID + "','" + strUserID + "',";
	//strSql += "unix_timestamp(), unix_timestamp() from IMDepart where uuid='" + strDepartID + "'";
	//log("SQL for inserting a record for IMUserPosition: %s", strSql.c_str());

	//m_pDBConn->ExecuteUpdate(strSql.c_str());

	////if the user no position previously
	//if (strPositionID != "00000000000000000000000000000000")
	//{
	//	strSql = "update IMUserPosition set status = -1,updated=unix_timestamp() where posuuid='00000000000000000000000000000000' and useruuid = '" + strUserID + "' and departuuid='" + strDepartID + "'";
	//	log("SQL for updating position status: %s", strSql.c_str());
	//	m_pDBConn->ExecuteUpdate(strSql.c_str());
	//}

}


void CDataSync::DeleteStaffPosition(string& strDepartID, string& strPositionID, string& strUserID)
{
	/*if(GetCompanyIdByUserId(strUserID) > 0)*/
	if (GetCompanyIdByStaffUuid(strUserID) > 0)
	{
		//if no position after deleting ,we should set '' for the user
		string strSql = "update IMUserPosition set status = -1, updated=unix_timestamp() where departuuid='" + strDepartID + "' and posuuid='" + strPositionID + "' and useruuid='" + strUserID + "'";
		m_pDBConn->ExecuteUpdate(strSql.c_str());
		log("SQL for deleting a record for IMUserPosition: %s", strSql.c_str());

		//get department status
		strSql = "select status from IMDepart where uuid='" + strDepartID + "'";
		CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
		int nStatus = 1;
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				nStatus = pResultSet->GetInt("status");
			}
			delete pResultSet;
			pResultSet = NULL;
		}

		uint32_t unUserId = GetUserIdByStaffUuid(strUserID); //get user id

		if (nStatus == 0) //department has already exists
		{
			string strJobGrade = "";
			SendUserPositionChangeNotify(strUserID, unUserId, strDepartID, strPositionID, IM::BaseDefine::DATA_SYNC_TYPE_DEL_USER_DUTY, strJobGrade);
		}

		UpdateStaffChangeTime(strUserID);
	}
}


void CDataSync::GetUserProducts(string& strInputString)
{
	Json::Reader reader;
	Json::Value root;

	if (reader.parse(strInputString, root))
	{
		try
		{
			int nCount = root.size();
			string strUserId = "";
			string strProducts = "";
		
			for (int i = 0; i < nCount; i++)
			{
				int nStatus = 0;
				Json::Value Product = root[i];
				if (!Product["status"].isNull() && Product["status"].isInt())
				{
					nStatus = Product["status"].asInt();
				}

				if (nStatus)
				{
					if (!Product["productId"].isNull() && Product["productId"].isString())
					{
						if (strProducts == "")
						{
							strProducts = Product["productId"].asString();
						}
						else
						{
							strProducts += "," + Product["productId"].asString();
						}
					}
				}

				if (!Product["userId"].isNull() && Product["userId"].isString())
				{
					strUserId = Product["userId"].asString();
				}
			}

			if (GetUserIdByUserUuid(strUserId) > 0)
			{
				string strSql = "update IMUser set buyproduct='" + strProducts + "',updated=unix_timestamp() where useruuid='" + strUserId + "'";
				log("SQL for updating IMUser: %s", strSql.c_str());
				m_pDBConn->ExecuteUpdate(strSql.c_str());

				//get user information
				USER_INFO UserInfo;
				uint32_t unUserId = GetUserInfoByUserId(strUserId, UserInfo);
				if (unUserId > 0)
				{
					////nStatus = GetStatusByProduct(strProducts, UserInfo.m_nStatus); //get status by products
					//nStatus = UserInfo.m_nStatus;
					//SetUserStatus(unUserId, nStatus);

					SendUserUpdateNotify(unUserId, 1, UserInfo);
				}
			}
		}
		catch (std::runtime_error msg)
		{
			log("user product,parse json data failed.");
		}
	}

	//	int nStatus = 0;
	//	//Json::Value Product = msgData[i];
	//	if (!Product["status"].isNull() && Product["status"].isInt())
	//	{
	//		nStatus = Product["status"].asInt();
	//	}

	//	if (nStatus)
	//	{
	//		if (!Product["productId"].isNull() && Product["productId"].isString())
	//		{
	//			if (strProducts == "")
	//				strProducts = Product["productId"].asString();
	//			else
	//				strProducts += "," + Product["productId"].asString();
	//		}
	//	}

	//	if (GetCompanyIdByStaffUuid(strUserID) > 0)
	//	{
	//		string strSql = "update IMStaff set buyproduct='" + strProducts + "',updated=unix_timestamp() where uuid='" + strUserID + "'";
	//		log("SQL for updating IMStaff: %s", strSql.c_str());
	//		m_pDBConn->ExecuteUpdate(strSql.c_str());

	//		//get user information
	//		STAFF_INFO UserInfo;
	//		uint32_t unUserId = GetStaffInfoByStaffUuid(strUserID, UserInfo);
	//		if (unUserId > 0)
	//		{
	//			////nStatus = GetStatusByProduct(strProducts, UserInfo.m_nStatus); //get status by products
	//			//nStatus = UserInfo.m_nStatus;
	//			//SetUserStatus(unUserId, nStatus);

	//			SendStaffUpdateNotify(unUserId, 1, UserInfo);
	//		}
	//	}
	//}

	//if (Reader.parse(strInputString, Root))
	//{
	//	if (!Root["resultCode"].isNull() && Root["resultCode"].isString())
	//	{
	//		string strResult = Root["resultCode"].asString();
	//		if (strResult != "0000")
	//		{
	//			return;
	//		}
	//	}
	//	else
	//	{
	//		return;
	//	}

	//	if (!Root["data"].isNull() && Root["data"].isArray())
	//	{
	//		Json::Value msgData = Root["data"];
	//		int nCount = msgData.size();
	//		string strUserID = "";
	//		string strProducts = "";
	//		if (nCount > 0)
	//		{
	//			int i = 0;
	//			for (; i < nCount - 1; i++)
	//			{
	//				int nStatus = 0;
	//				Json::Value Product = msgData[i];
	//				if (!Product["status"].isNull() && Product["status"].isInt())
	//				{
	//					nStatus = Product["status"].asInt();
	//				}

	//				if (nStatus)
	//				{
	//					if (!Product["productId"].isNull() && Product["productId"].isString())
	//					{
	//						if (strProducts == "")
	//							strProducts = Product["productId"].asString();
	//						else
	//							strProducts += "," + Product["productId"].asString();
	//					}
	//				}
	//			}


	//			Json::Value Product = msgData[i];
	//			if (!Product["userId"].isNull() && Product["userId"].isString())
	//			{
	//				strUserID = Product["userId"].asString();		
	//			}

	//			int nStatus = 0;
	//			//Json::Value Product = msgData[i];
	//			if (!Product["status"].isNull() && Product["status"].isInt())
	//			{
	//				nStatus = Product["status"].asInt();
	//			}

	//			if (nStatus)
	//			{
	//				if (!Product["productId"].isNull() && Product["productId"].isString())
	//				{
	//					if (strProducts == "")
	//						strProducts = Product["productId"].asString();
	//					else
	//						strProducts += "," + Product["productId"].asString();
	//				}
	//			}

	//			if (GetCompanyIdByStaffUuid(strUserID) > 0)
	//			{
	//				string strSql = "update IMStaff set buyproduct='" + strProducts + "',updated=unix_timestamp() where uuid='" + strUserID + "'";
	//				log("SQL for updating IMStaff: %s", strSql.c_str());
	//				m_pDBConn->ExecuteUpdate(strSql.c_str());

	//				//get user information
	//				STAFF_INFO UserInfo;
	//				uint32_t unUserId = GetStaffInfoByStaffUuid(strUserID, UserInfo);
	//				if (unUserId > 0)
	//				{
	//					////nStatus = GetStatusByProduct(strProducts, UserInfo.m_nStatus); //get status by products
	//					//nStatus = UserInfo.m_nStatus;
	//					//SetUserStatus(unUserId, nStatus);

	//					SendStaffUpdateNotify(unUserId, 1, UserInfo);
	//				}
	//			}
	//		}
	//	}
	//}
}


void CDataSync::DeleteDepartmentByDepartmentID(string& strDepartID)
{
	if (GetCompanyIdByDepartId(strDepartID) > 0)
	{
		SendRemoveDepartmentUserNotify(strDepartID);

		string strSql = "update IMDepart a, IMUserPosition b, IMStaff c set a.status = -1, a.updated = unix_timestamp(), b.status = -1, b.updated = unix_timestamp(),c.updated=unix_timestamp() where a.uuid = b.departuuid and c.uuid=b.useruuid and a.uuid = '" + strDepartID + "' and b.status=0";
		log("SQL for deleting department: %s", strSql.c_str());
		m_pDBConn->ExecuteUpdate(strSql.c_str());

		SendDepartmentDelNotify(strDepartID);
	}
}


void CDataSync::DeletePositionByPositionID(string& strPositionID)
{
	string strSql = "update Position a, IMUserPosition b set a.status = -1, a.updated = unix_timestamp(), b.status = -1, b.updated = unix_timestamp() where a.uuid = b.posuuid and a.uuid = '" + strPositionID + "'";
	log("SQL for deleting position: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::DeletePositionByDomainID(string& strDomainID)
{
	string strSql = "update Position a, IMUserPosition b, IMCompany c set a.status = -1, a.updated=unix_timestamp(), b.status=-1,b.updated=unix_timestamp() where a.companyid=b.companyid and c.domainid ='" + strDomainID + "'" + " and (a.companyid=c.id)";
	log("SQL for  deleting position for a domain: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::DeleteDepartmentAndPosition(string& strDepartID, string& strPositionID)
{
	string strSql = "update IMUserPosition  set status = -1, updated=unix_timestamp() where departuuid='" + strDepartID + "' and posuuid='" + strPositionID + "'";
	log("SQL for deleting position for a department and position: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::DeleteDepartIDAndUserID(string& strOrgID, string& strUserID)
{
	string strSql = "update IMUserPosition  set status = -1, updated=unix_timestamp() where departuuid='" + strOrgID + "' and useruuid='" + strUserID + "'";
	log("SQL for deleting position for a department and user: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	SendRemoveDepartmentUserNotify(strOrgID, strUserID);

	//string strPositionId = "";
	//AddStaffPosition(strOrgID, strPositionId, strUserID); //added by george on June 2nd,2016
}


void CDataSync::DeletePositionIdAndUserId(string& strPositionId, string& strUserId)
{
	string strSql = "update IMUserPosition  set status = -1, updated=unix_timestamp() where posuuid='" + strPositionId + "' and useruuid='" + strUserId + "'";
	log("SQL for deleting position for a position and user: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());

}


void CDataSync::GetCompanyInfo(string& strInputString, DOMAIN_INFO& DomainInfo)
{
	Json::Reader Reader;
	Json::Value Root;
	
	if (Reader.parse(strInputString, Root))
	{
		
		if (!Root["primaryOrgId"].isNull() && Root["primaryOrgId"].isString())
		{
			DomainInfo.m_strCompanyUuid = Root["primaryOrgId"].asString();
		}
		else
		{
			DomainInfo.m_strCompanyUuid = "";
		}

		if (!Root["orgCode"].isNull() && Root["orgCode"].isString())
		{
			DomainInfo.m_strCompanyNo = Root["orgCode"].asString();
		}
		else
		{
			DomainInfo.m_strCompanyNo = "";
		}

		if (!Root["orgName"].isNull() && Root["orgName"].isString())
		{
			DomainInfo.m_strCompanyName = Root["orgName"].asString();
		}
		else
		{
			DomainInfo.m_strCompanyName = "";
		}


		if (!Root["orgFullName"].isNull() && Root["orgFullName"].isString())
		{
			DomainInfo.m_strCompanyFullName = Root["orgFullName"].asString();
		}
		else
		{
			DomainInfo.m_strCompanyFullName = "";
		}

		if (!Root["industry"].isNull() && Root["industry"].isString())
		{
			DomainInfo.m_strIndustryType = Root["industry"].asString();
		}
		else
		{
			DomainInfo.m_strIndustryType = "";
		}

		if (!Root["status"].isNull() && Root["status"].isInt())
		{
			DomainInfo.m_nStatus = Root["status"].asInt();
		}
		else
		{
			DomainInfo.m_nStatus = 0;
		}

		//if (!msgData["serverName"].isNull() && msgData["serverName"].isString())
		//{
		//	DomainInfo.m_strSyncServer = msgData["serverName"].asString();
		//}
		//else
		//{
		//	DomainInfo.m_strSyncServer = "";
		//}

		//if (!msgData["expiredDate"].isNull() && msgData["expiredDate"].isUInt64())
		//{
		//	DomainInfo.m_unExpiredDate = msgData["expiredDate"].asUInt64()/1000;
		//}
		//else
		//{
		//	DomainInfo.m_unExpiredDate = 0;
		//}
	}
}


void CDataSync::AddNewCompany(DOMAIN_INFO& DomainInfo)
{
	int nCompanyID = 0;
	string strSql = "select id from IMCompany where companyuuid = '" + DomainInfo.m_strCompanyUuid + "'";
	CResultSet  *pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			nCompanyID = pResultSet->GetInt("id");
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	if (nCompanyID == 0)
	{
		strSql = "insert into IMCompany(companyuuid,companyno,companyname,companyfullname,industrytype,syncserver,expireddate,isjoined,isinvite,status,created,updated)values(?,?,?,?,?,?,?,?,?,?,?,?)";

		// 必须在释放连接前delete CPrepareStatement对象，否则有可能多个线程操作mysql对象，会crash
		CPrepareStatement* pStmt = new CPrepareStatement();
		if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
		{
			int nIndex = 0;
			unsigned int nCurrenTime = time(NULL);
			int nStatus = DomainInfo.m_nStatus;
			pStmt->SetParam(nIndex++, DomainInfo.m_strCompanyUuid);
			pStmt->SetParam(nIndex++, DomainInfo.m_strCompanyNo);
			pStmt->SetParam(nIndex++, DomainInfo.m_strCompanyName);
			pStmt->SetParam(nIndex++, DomainInfo.m_strCompanyFullName);
			pStmt->SetParam(nIndex++, DomainInfo.m_strIndustryType);
			pStmt->SetParam(nIndex++, DomainInfo.m_strSyncServer);
			pStmt->SetParam(nIndex++, DomainInfo.m_unExpiredDate);
			pStmt->SetParam(nIndex++, DomainInfo.m_unIsJoined);
			pStmt->SetParam(nIndex++, DomainInfo.m_unIsInvited);
			pStmt->SetParam(nIndex++, nStatus);
			pStmt->SetParam(nIndex++, nCurrenTime);
			pStmt->SetParam(nIndex++, nCurrenTime);
			pStmt->ExecuteUpdate();
		}

		delete pStmt;
		pStmt = NULL;
	}
	else
	{
		strSql = "update IMCompany set status = " + int2string(DomainInfo.m_nStatus);
		m_pDBConn->ExecuteUpdate(strSql.c_str());
	}
}


void CDataSync::UpdateCompany(DOMAIN_INFO& DomainInfo)
{
	string strSql = "update IMCompany set companyno='" + DomainInfo.m_strCompanyNo + "',companyname='" + DomainInfo.m_strCompanyName + "',status=" + int2string(DomainInfo.m_nStatus);
	strSql += ",companyfullname='" + DomainInfo.m_strCompanyFullName + "', industrytype='" + DomainInfo.m_strIndustryType + "',isjoined=" + int2string(DomainInfo.m_unIsJoined) + ",isinvite=" + int2string(DomainInfo.m_unIsInvited);
	strSql += ",syncserver='" + DomainInfo.m_strSyncServer + "',expireddate="+int2string(DomainInfo.m_unExpiredDate) + ",updated=unix_timestamp() where companyuuid='" + DomainInfo.m_strCompanyUuid + "'";
	log("SQL for updating company: %s", strSql.c_str());

	//string strSql = "update IMCompany set companyno='" + DomainInfo.m_strCompanyNo + "',companyname='" + DomainInfo.m_strCompanyName + "',status=" + int2string(DomainInfo.m_nStatus);
	//strSql += ",companyfullname='" + DomainInfo.m_strCompanyFullName + "', industrytype='" + DomainInfo.m_strIndustryType + "',updated=unix_timestamp() where companyuuid='" + DomainInfo.m_strCompanyUuid + "'";
	//log("SQL for updating company: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());
	m_nCompanyId = GetCompanyIdByCompanyUuid(DomainInfo.m_strCompanyUuid);
	SendCompanyUpdateNotify(m_nCompanyId, DomainInfo);
}


int  CDataSync::GetGroupInfo(string& strInputString, DOMAIN_GROUP& GroupInfo, list<string>& listUsers)
{
	Json::Reader Reader;
	Json::Value Root;
	int nResult = -1;
	try
	{
		if (Reader.parse(strInputString, Root))
		{
			if (!Root["groupId"].isNull() && Root["groupId"].isString())
			{
				GroupInfo.m_strGroupID = Root["groupId"].asString();
			}

			//if (!Root["primaryOrgId"].isNull() && Root["primaryOrgId"].isString())
			if (!Root["domainId"].isNull() && Root["domainId"].isString())
			{
				GroupInfo.m_strDomainID = Root["domainId"].asString();
				//GroupInfo.m_strDomainID = Root["primaryOrgId"].asString();
			}

			if (!Root["groupName"].isNull() && Root["groupName"].isString())
			{
				GroupInfo.m_strGroupName = Root["groupName"].asString();
			}

			if (!Root["groupEmail"].isNull() && Root["groupEmail"].isString())
			{
				GroupInfo.m_strEmail = Root["groupEmail"].asString();
			}

			if (!Root["status"].isNull() && Root["status"].isInt())
			{
				GroupInfo.m_nStatus = Root["status"].asInt();
				if (GroupInfo.m_nStatus == 1)
				{
					GroupInfo.m_nStatus = 0;
				}
				else if (GroupInfo.m_nStatus == 0)
				{
					GroupInfo.m_nStatus = 1;
				}
			}

			nResult = 1;
			/*log("INDEX:%d\tGROUP ID: %s\tDOMAIN ID: %s\tGROUP NAME: %s\tEMAIL; %s\tUSER COUNT: %d\tSTATUS: %d\n", i + 1, DomainGroup.m_strGroupID.c_str(), DomainGroup.m_strDomainID.c_str(),
				DomainGroup.m_strGroupName.c_str(), DomainGroup.m_strEmail.c_str(), DomainGroup.m_nUserCount, DomainGroup.m_nStatus);*/
			if (!Root["userIds"].isNull() && Root["userIds"].isArray())
			{
				Json::Value GroupUsers = Root["userIds"];
				int nUsers = GroupUsers.size();;
				for (int i = 0; i < nUsers; i++)
				{
					string strUserID = GroupUsers[i].asString();
					//log("USER ID: %s", strUserID.c_str());
					listUsers.push_back(strUserID);
					/*if (!GroupUsers[i]["userId"].isNull() && GroupUsers[i]["userId"].isString())
					{
						string strUserID = GroupUsers[i]["userId"].asString();
						listUsers.push_back(strUserID);
					}*/
				}
				GroupInfo.m_nUserCount = nUsers;
			}
			else
			{
				GroupInfo.m_nUserCount = 0;
			}
		}
		else
		{
			log("Invalid json data");
		}
	}
	catch (std::runtime_error msg)
	{
		log("error message");
	}
	catch (exception& Exception)
	{
		log("Exception occured: %s", Exception.what());
	}

	return nResult;
}


void CDataSync::AddNewMailGroupAndMembers(DOMAIN_GROUP& GroupInfo, list<string>& listUsers)
{
	int nCompanyID = GetCompanyIdByCompanyUuid(GroupInfo.m_strDomainID);

	if (nCompanyID != 0)
	{
		int nGroupID = 0;
		//int nStatus = 0;
		string strSql = "BEGIN";  //start the transaction
		m_pDBConn->ExecuteUpdate(strSql.c_str());

	/*	strSql = "select id,status from IMGroup where email = '" + GroupInfo.m_strEmail + "'";
		CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				nGroupID = pResultSet->GetInt("id");
				nStatus = pResultSet->GetInt("status");
			}
			delete pResultSet;
			pResultSet = NULL;
		}
*/

		unsigned int nCurrentTime = time(NULL);

		//group id equals to 0 means a new group,otherwise means deleted mail group
		//if (nGroupID == 0 || (nGroupID >0 && nStatus == 1))
		{
			strSql = "insert into IMGroup(groupid,companyid,name,type,usercnt,status,created,updated,email) values(?,?,?,?,?,?,?,?,?)";
			CPrepareStatement* pStmt = new CPrepareStatement();
			if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
			{
				int nIndex = 0;

				int nType = 3; //mail group
				int nUserCount = GroupInfo.m_nUserCount;
				int nStatus = GroupInfo.m_nStatus;
				pStmt->SetParam(nIndex++, GroupInfo.m_strGroupID);
				pStmt->SetParam(nIndex++, nCompanyID);
				pStmt->SetParam(nIndex++, GroupInfo.m_strGroupName);
				pStmt->SetParam(nIndex++, nType);
				pStmt->SetParam(nIndex++, nUserCount);
				pStmt->SetParam(nIndex++, nStatus);
				pStmt->SetParam(nIndex++, nCurrentTime);
				pStmt->SetParam(nIndex++, nCurrentTime);
				pStmt->SetParam(nIndex++, GroupInfo.m_strEmail);
				pStmt->ExecuteUpdate();
				nGroupID = pStmt->GetInsertId();
			}

			delete pStmt;
			pStmt = NULL;
		}
		

		//insert group members
		if (nGroupID > 0 && listUsers.size() > 0)
		{
			for (list<string>::iterator it = listUsers.begin(); it != listUsers.end(); it++)
			{
				strSql = "insert into IMGroupMember(groupid,userid,status,created,updated) select " + int2string(nGroupID) + ",userid, 0, " + int2string(nCurrentTime) + ",";
				strSql += int2string(nCurrentTime) + " from IMStaff where uuid='" + *it + "' and status = 1 and not exists(select 1 from IMGroupMember where groupid=" + int2string(nGroupID) + " and userid=IMStaff.userid)";
				log("SQL for inserting users: %s", strSql.c_str());
				m_pDBConn->ExecuteUpdate(strSql.c_str());
			}

			//add new session
			strSql = "insert into IMRecentSession (userid,peerid,type,status,created,updated) select userid,groupid,2,0,created,updated from IMGroupMember a where a.groupid=" + int2string(nGroupID) + " and a.status=0 and not exists(select 1 from IMRecentSession b where a.userid=b.userid and a.groupid=b.peerid and b.type=2)";
			log("SQL for inserting new session; %s", strSql.c_str());
			m_pDBConn->ExecuteUpdate(strSql.c_str());

			//update creator
			strSql = "update IMGroup a ,(select min(userid) as minuid,groupid,count(*) as totalcount from IMGroupMember where status = 0 group by groupid) as b set a.creator=b.minuid,a.usercnt=b.totalcount where a.type=3 and a.creator = 0 and a.id=b.groupid and a.id=" + int2string(nGroupID);
			log("SQL for update creator: %s", strSql.c_str());
			m_pDBConn->ExecuteUpdate(strSql.c_str());

			SendMailGroupCreateNotify(nGroupID,nCompanyID, GroupInfo.m_strGroupName, GroupInfo.m_strGroupID);
			SetGroupUserStatus(GroupInfo.m_strGroupID); //set cached group user
			/*strSql = "COMMIT";
			m_pDBConn->ExecuteUpdate(strSql.c_str());*/
		}

		strSql = "COMMIT";
		m_pDBConn->ExecuteUpdate(strSql.c_str());
	}
	else
	{
		log("%s does not exists.............", GroupInfo.m_strDomainID.c_str());
	}
}


void CDataSync::UpdateMailGroup(DOMAIN_GROUP& GroupInfo)
{
	int nCompanyID = GetCompanyIdByCompanyUuid(GroupInfo.m_strDomainID);
	if (nCompanyID != 0)
	{
		string strSql = "BEGIN";  //start the transaction
		m_pDBConn->ExecuteUpdate(strSql.c_str());

		strSql = "select name from IMGroup where groupid='" + GroupInfo.m_strGroupID + "'";
		string strGroupName = "";
		CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				if (pResultSet->GetString("name") != NULL)
				{
					strGroupName = pResultSet->GetString("name");
					//nOrigStatus = pResultSet->GetInt("status");
				}
			}
			delete pResultSet;
			pResultSet = NULL;
		}


		time_t nTicks = time(NULL);
		strSql = "update IMGroup set name='" + GroupInfo.m_strGroupName + "',status=" + int2string(GroupInfo.m_nStatus) + ",email='" + GroupInfo.m_strEmail + "',";
		strSql += "updated=" + longlong2string(nTicks) + ", version=version+1 where groupid='" + GroupInfo.m_strGroupID + "'";
		log("SQL for updating mail group:%s", strSql.c_str());
		m_pDBConn->ExecuteUpdate(strSql.c_str());


		if (GroupInfo.m_nStatus == 1)
		{
			strSql = "update IMGroupMember a, IMGroup b set a.status = 1, a.updated=" + longlong2string(nTicks) + ",b.usercnt = 0 where a.groupid=b.id and b.groupid='" + GroupInfo.m_strGroupID + "'";
			log("SQL for deleting a group; %s", strSql.c_str());
			m_pDBConn->ExecuteUpdate(strSql.c_str());
			
			//update recent session
			UpdateRecentSession(GroupInfo.m_strGroupID);

			SetGroupUserStatus(GroupInfo.m_strGroupID); //set cached group user

			//send notify to delete the group
			SendMailGroupDelNotify(GroupInfo.m_strGroupID, nTicks);

		}
		else if (strGroupName != GroupInfo.m_strGroupName)
		{
			//change group name
			SendMailGroupChangeNameNotify(GroupInfo.m_strGroupID, GroupInfo.m_strGroupName);
		}

		strSql = "COMMIT";  //start the transaction
		m_pDBConn->ExecuteUpdate(strSql.c_str());
	}
	else
	{
		log("domain %s does not exist", GroupInfo.m_strDomainID.c_str());
	}
}


void CDataSync::DeleteMailGroup(string& strGroupID)
{
	string strSql = "BEGIN";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	//delete group with group members
	time_t nCurrentTime = time(NULL);
	strSql = "update IMGroupMember a, IMGroup b set a.status = 1, a.updated=" + longlong2string(nCurrentTime) + ",b.status=1,b.userCnt = 0,version=version+1, b.updated=unix_timestamp() where a.groupid=b.id and b.groupid='" + strGroupID + "'";
	log("SQL from deleting a group: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	//update recent session
	UpdateRecentSession(strGroupID);

	//send notify here
	strSql = "COMMIT";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	SetGroupUserStatus(strGroupID); //set cached group user

	SendMailGroupDelNotify(strGroupID, nCurrentTime);
}



int CDataSync::GetCompanyByDomain(string& strDomainID)
{
	int nCompanyID = 0;
	string strSql = "select id from IMCompany where domainid = '" + strDomainID + "'";
	log("SQL for getting company id: %s", strSql.c_str());
	CResultSet  *pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			nCompanyID = pResultSet->GetInt("id");
		}
		delete pResultSet;
		pResultSet = NULL;
	}
	return nCompanyID;
}


int CDataSync::GetMailGroupUser(string& strInputString, list<string>& listUsers, string& strGroupID)
{
	Json::Reader Reader;
	Json::Value Root;
	int nResult = -1;
	try
	{
		if (Reader.parse(strInputString, Root))
		{
			if (!Root["groupId"].isNull() && Root["groupId"].isString())
			{
				strGroupID = Root["groupId"].asString();
			}
			else
			{	
				strGroupID = "";
			}

			if (!Root["userIds"].isNull() && Root["userIds"].isArray())
			{
				Json::Value GroupUsers = Root["userIds"];
				int nUsers = GroupUsers.size();;
				for (int i = 0; i < nUsers; i++)
				{
					if (!GroupUsers[i]["userId"].isNull() && GroupUsers[i]["userId"].isString())
					{
						string strUserID = GroupUsers[i]["userId"].asString();
						listUsers.push_back(strUserID);
					}
				}
				nResult = 1;
			}
		}
		else
		{
			log("Invalid json data");
		}
	}
	catch (std::runtime_error msg)
	{
		log("error message");
	}
	catch (exception& Exception)
	{
		log("Exception occured: %s", Exception.what());
	}

	return nResult;
}


void CDataSync::SetMailGroupUser(string& strGroupID, list<string>& listUsers)
{
	uint32_t nCurrentTime = time(NULL);
	int nGroupId = 0;
	int nStatus = 0;
	int nUserCount = listUsers.size();
	if (nUserCount > 0)
	{
		string strSql = "truncate table IMGroupMemberTemp";
		m_pDBConn->ExecuteUpdate(strSql.c_str());

		strSql = "BEGIN";
		m_pDBConn->ExecuteUpdate(strSql.c_str());

		string strGroupName;
		uint32_t unCompanyId = 0;

		strSql = "select id, companyid,name,status from IMGroup where groupid='" + strGroupID + "'";
		CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				nGroupId = pResultSet->GetInt("id");
				nStatus = pResultSet->GetInt("status");
				strGroupName = pResultSet->GetString("name");
				unCompanyId = pResultSet->GetInt("companyid");
			}
			delete pResultSet;
			pResultSet = NULL;
		}

		if (nGroupId > 0)
		{
			if (nStatus == 1) //enabled the disabled mail group
			{
				strSql = "update IMGroup set status=0,version=version+1,updated=unix_timestamp() where id=" + int2string(nGroupId);
				m_pDBConn->ExecuteUpdate(strSql.c_str());
			}

			for (list<string>::iterator it = listUsers.begin(); it != listUsers.end(); ++it)
			{
				uint32_t unCurrentTime = time(NULL);
				strSql = "insert into IMGroupMemberTemp (groupid,userid,created,updated,groupuuid,useruuid) select a.id,b.userid," + int2string(unCurrentTime) + "," + int2string(unCurrentTime);
				strSql += ",a.groupid,b.useruuid from IMGroup a, IMStaff b where a.groupid='" + strGroupID + "' and a.status=0 and b.uuid='" + *it + "' and b.status=1";
				log("SQL for inserting data to temp table: %s", strSql.c_str());
				m_pDBConn->ExecuteUpdate(strSql.c_str());
			}

			list<uint32_t> listAddUsers;
			list<uint32_t> listDelUsers;
			list<uint32_t> listTotalUsers;
			uint32_t unUserId = 0;

			//get users to be added to the group
			strSql = "select a.userid as auserid from IMGroupMemberTemp a where not exists(select 1 from IMGroupMember b where a.userid=b.userid and b.groupid=" + int2string(nGroupId) + ")";
			log("SQL for getting users to be added: %s", strSql.c_str());
			pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
			if (pResultSet)
			{
				while (pResultSet->Next())
				{
					unUserId = pResultSet->GetInt("auserid");
					listAddUsers.push_back(unUserId);
					//log("Adding a user into the group");
				}

				delete pResultSet;
				pResultSet = NULL;
			}

			//add new group member
			strSql = "insert into IMGroupMember (groupid,userid,created,updated,status) select groupid,userid,created,updated,0 from IMGroupMemberTemp where not exists(select 1 from IMGroupMember where IMGroupMember.groupid=IMGroupMemberTemp.groupid and IMGroupMember.userid=IMGroupMemberTemp.userid)";
			log("SQL for inserting new memers; %s", strSql.c_str());
			m_pDBConn->ExecuteUpdate(strSql.c_str());

			//add new session
			strSql = "insert into IMRecentSession (userid,peerid,type,status,created,updated) select userid,groupid,2,0,created,updated from IMGroupMemberTemp a where not exists(select 1 from IMRecentSession b where a.userid=b.userid and a.groupid=b.peerid and b.type=2)";
			log("SQL for inserting new session; %s", strSql.c_str());
			m_pDBConn->ExecuteUpdate(strSql.c_str());


			//get users to be deleted from the group
			strSql = "select userid as auserid from IMGroupMember a where groupid=" + int2string(nGroupId) + " and status=0 and not exists(select 1 from IMGroupMemberTemp b where a.userid = b.userid)";
			log("SQL for getting users to be deleted: %s", strSql.c_str());
			pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
			if (pResultSet)
			{
				while (pResultSet->Next())
				{
					unUserId = pResultSet->GetInt("auserid");
					listDelUsers.push_back(unUserId);
					//log("Deleting a user from the group");
				}

				delete pResultSet;
				pResultSet = NULL;
			}

			//log("ADD USERS: %d\tDEL USERS: %d", listAddUsers.size(), listDelUsers.size());

			//delete group member
			strSql = "update IMGroupMember set status=1, updated=unix_timestamp() where groupid=" + int2string(nGroupId) + " and status=0 and not exists(select 1 from IMGroupMemberTemp  where IMGroupMember.groupid=IMGroupMemberTemp.groupid and IMGroupMember.userid=IMGroupMemberTemp.userid)";
			log("SQL for deleting a group member: %s", strSql.c_str());
			m_pDBConn->ExecuteUpdate(strSql.c_str());

			//delete a session
			strSql = "update IMRecentSession a, IMGroupMember b set a.status=1, a.updated=unix_timestamp() where a.userid=b.userid and a.peerid=b.groupid and a.type = 2 and a.peerid=" + int2string(nGroupId) + " and b.status = 1 and a.status = 0";
			log("SQL for deleting a session: %s", strSql.c_str());
			m_pDBConn->ExecuteUpdate(strSql.c_str());
			

			//get existing and deleted users to be added to the group again
			strSql = "select userid as auserid from IMGroupMember a where groupid=" + int2string(nGroupId) + " and status = 1 and exists(select 1 from IMGroupMemberTemp b where a.userid=b.userid)";
			log("SQL for getting deleted users and readded: %s", strSql.c_str());
			pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
			if (pResultSet)
			{
				while (pResultSet->Next())
				{
					unUserId = pResultSet->GetInt("auserid");
					listAddUsers.push_back(unUserId);
					//log("Readding users");
				}

				delete pResultSet;
				pResultSet = NULL;
			}

			//update group member(add deleted and existing users. change create time)
			strSql = "update IMGroupMember a ,IMGroupMemberTemp b  set a.status=0,a.created=unix_timestamp(),a.updated=unix_timestamp() where a.groupid=b.groupid and a.userid=b.userid and a.status = 1";
			log("SQL for update users for a group: %s", strSql.c_str());
			m_pDBConn->ExecuteUpdate(strSql.c_str());

			strSql = "update IMGroup, (select count(*) as totalcount from IMGroupMember where groupid=" + int2string(nGroupId) + " and status = 0) as b set usercnt=b.totalcount,version=version+1,updated=unix_timestamp() where groupid='" + strGroupID + "'";
			log("SQL for updating group; %s", strSql.c_str());
			m_pDBConn->ExecuteUpdate(strSql.c_str());

			//update session status(reset session)
			strSql = "update IMRecentSession a, IMGroupMember b set a.status=0, a.updated=unix_timestamp() where a.userid=b.userid and a.peerid=b.groupid and a.type=2 and a.peerid=" + int2string(nGroupId) + " and b.status = 0 and a.status = 1";
			log("SQL for updating a session: %s", strSql.c_str());
			m_pDBConn->ExecuteUpdate(strSql.c_str());


			//get total users and to send notify
			strSql = "select userid from IMGroupMember where status=0 and groupid = " + int2string(nGroupId);
			pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
			if (pResultSet)
			{
				while (pResultSet->Next())
				{
					unUserId = pResultSet->GetInt("userid");
					listTotalUsers.push_back(unUserId);
				}

				delete pResultSet;
				pResultSet = NULL;
			}

			if (listAddUsers.size() > 0 && listTotalUsers.size() > 0 && nStatus == 0)
			{
				log("Adding members");
				SendMailGroupChangeMemberNotify(nGroupId, listAddUsers, listTotalUsers, IM::BaseDefine::GROUP_MODIFY_TYPE_ADD);
			}

			if (listDelUsers.size() > 0 && listTotalUsers.size() > 0 && nStatus == 0)
			{
				log("Deleting members");
				SendMailGroupChangeMemberNotify(nGroupId, listDelUsers, listTotalUsers, IM::BaseDefine::GROUP_MODIFY_TYPE_DEL);
			}

			//update creator
			strSql = "select a.status as status1 from IMGroupMember a, IMGroup b where a.groupid=b.id and a.userid= b.creator and b.id=" + int2string(nGroupId);
			log("SQL for getting creator status: %s", strSql.c_str());
			int nCreatorStatus = 0;
			pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
			if (pResultSet)
			{
				while (pResultSet->Next())
				{
					nCreatorStatus = pResultSet->GetInt("status1");
				}

				delete pResultSet;
				pResultSet = NULL;
			}
			
			if (nCreatorStatus == 1 || nStatus == 1) //creator is deleted and should be replaced
			{
				strSql = "update IMGroup a ,(select min(userid) as minuid from IMGroupMember where status = 0 and groupid=" + int2string(nGroupId) + ") as b set a.creator=b.minuid,version=version+1, a.updated=unix_timestamp() where a.id=" + int2string(nGroupId);
				log("SQL for updating creator: %s", strSql.c_str());
				m_pDBConn->ExecuteUpdate(strSql.c_str());

				if (nCreatorStatus == 1)
				{
					//get new creator
					uint32_t unNewCreator = 0;
					strSql = "select creator from IMGroup where id=" + int2string(nGroupId);
					pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
					if (pResultSet)
					{
						while (pResultSet->Next())
						{
							unNewCreator = pResultSet->GetInt("creator");
						}

						delete pResultSet;
						pResultSet = NULL;
					}

					//update creator notify
					SendCreatorChangeNotify(nGroupId, unNewCreator, listTotalUsers);
				}
			}

			SetGroupUserStatus(strGroupID);

			if (nStatus == 1)
			{
				SendMailGroupCreateNotify(nGroupId, unCompanyId, strGroupName, strGroupID);
			}
		}

		strSql = "COMMIT";
		m_pDBConn->ExecuteUpdate(strSql.c_str());
	}
}

void CDataSync::CheckUser(string& strUserID)
{
	string strSql = "BEGIN";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "select id,status,buyproduct from IMUser where useruuid='" + strUserID + "'";
	CResultSet *pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			uint32_t unUserId = pResultSet->GetInt("id");
			int nStatus = pResultSet->GetInt("status");

			string strBuyProduct = "";
			if (pResultSet->GetString("buyproduct") != NULL)
			{
				strBuyProduct = pResultSet->GetString("buyproduct");
			}

			//nStatus = GetStatusByProduct(strBuyProduct, nStatus); //get status by products
			SetUserStatus(unUserId, nStatus);
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	strSql = "select a.id as gid,creator,b.id as uid ,b.status as bstatus, usercnt from IMGroup a, IMUser b, IMGroupMember c where a.id=c.groupid and b.id=c.userid and a.type <> 3 and b.useruuid='" + strUserID + "'";
	log("SQL for getting groupid and userid: %s", strSql.c_str());

	pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		uint32_t unUserID = 0, unGroupID = 0, unCreator = 0;
		int nStatus = 0;
		while (pResultSet->Next())
		{
			unUserID = pResultSet->GetInt("uid");
			nStatus = pResultSet->GetInt("bstatus");
			unGroupID = pResultSet->GetInt("gid");
			unCreator = pResultSet->GetInt("creator");
			uint32_t unCurrentTime = time(NULL);
			int nUserCount = pResultSet->GetInt("usercnt");

			if (nStatus != 1) //user is deleted and disabled 
			{
				/*string strSql1 = "update IMGroup a, IMGroupMember b set userCnt = userCnt-1, a.updated=unix_timestamp(), version=version+1, b.status = 1,b.updated=a.updated where ";
				strSql1 += "b.userid=" + int2string(unUserID) + " and a.id=b.groupid and userCnt >= 1";
				log("SQL for updating group & group member: %s", strSql1.c_str());
				m_pDBConn->ExecuteUpdate(strSql1.c_str());*/

				string strSql1 = "update IMGroupMember set status = 1,updated=unix_timestamp() where groupid= " + int2string(unGroupID) + " and userid=" + int2string(unUserID);
				log("SQL for updating group member: %s", strSql1.c_str());
				m_pDBConn->ExecuteUpdate(strSql1.c_str());

				//if(creator == unUserID)
				//{
				//	strSql1 = "update IMGroup a, (select max(userid) as maxuserid from IMGroupMember where status = 0 and groupid = " + int2string(unGroupID) + ") as b set a.creator=b.maxuserid,a.updated=unix_timestamp(), a.version=a.version+1 where a.id=" + int2string(unGroupID) + " and creator = " + int2string(unUserID);
				//	log("SQL for updating creator: %s", strSql.c_str());
				//	m_pDBConn->ExecuteUpdate(strSql1.c_str()); //update creator 

				//	string strSql2 = "select creator from IMGroup where id=" + int2string(unGroupID);
				//	CResultSet *pResultSet2 = m_pDBConn->ExecuteQuery(strSql2.c_str());
				//	uint32_t unNewCreator = 0;
				//	if(pResultSet2)
				//	{
				//		while(pResultSet2->Next())
				//		{
				//			unNewCreator = pResultSet2->GetInt("creator");	
				//		}
				//		delete pResultSet2;
				//		pResultSet2 = NULL;
				//	}

				//	//SendCreatorChangeNotify();
				//}

				//to decide if we should remove the group
				if (nUserCount > 1)
				{
					string strSql1 = "update IMGroup set userCnt = userCnt-1, updated=unix_timestamp(), version=version+1 where id=" + int2string(unGroupID);
					log("SQL for updating group: %s", strSql1.c_str());
					m_pDBConn->ExecuteUpdate(strSql1.c_str());

					//get current user list
					strSql1 = "select userid from IMGroupMember where status=0 and groupid=" + int2string(unGroupID);
					CResultSet *pResultSet1 = m_pDBConn->ExecuteQuery(strSql1.c_str());
					list<uint32_t> listUsers;
					if(pResultSet1)
					{
						uint32_t unCurrUserId = 0;
						while(pResultSet1->Next())
						{
							unCurrUserId = pResultSet1->GetInt("userid");
							listUsers.push_back(unCurrUserId);
						}
						delete pResultSet1;
						pResultSet1 = NULL;
					}

					SendGroupMemberChangeNotify(unGroupID, unUserID, listUsers); //send notify

					if (unCreator == unUserID || unCreator == 0)
					{
						strSql1 = "update IMGroup a, (select min(userid) as minuserid from IMGroupMember where status = 0 and groupid = " + int2string(unGroupID) + ") as b set a.creator=b.minuserid,a.updated=unix_timestamp(), a.version=a.version+1 where a.id=" + int2string(unGroupID) + " and creator = " + int2string(unUserID);
						log("SQL for updating creator: %s", strSql.c_str());
						m_pDBConn->ExecuteUpdate(strSql1.c_str()); //update creator 

						strSql1 = "select creator from IMGroup where status = 0 and id=" + int2string(unGroupID);
						pResultSet1 = m_pDBConn->ExecuteQuery(strSql1.c_str());
						uint32_t unNewCreator = 0;
						if(pResultSet1)
						{
							while(pResultSet1->Next())
							{
								unNewCreator = pResultSet1->GetInt("creator");	
							}
							delete pResultSet1;
							pResultSet1 = NULL;
						}

						SendCreatorChangeNotify(unGroupID, unNewCreator, listUsers); //send notify
					}
				}
				else
				{
					string strSql1 = "update IMGroup set userCnt = 0, status=1, updated=unix_timestamp(), version=version+1 where id=" + int2string(unGroupID);
					log("SQL for removing group: %s", strSql1.c_str());
					m_pDBConn->ExecuteUpdate(strSql1.c_str());
				}

				SetGroupUserStatus(unGroupID, unUserID, 1, unCurrentTime); //don't forget here
			}
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	strSql = "COMMIT";
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::SetGroupUserStatus(uint32_t unGroupID, uint32_t unUserID, uint32_t unStatus, uint32_t nCreateTime)
{
	if (m_pCacheGroupMember)
	{
		string strKey = "group_member_" + int2string(unGroupID);
		map<string, string> mapCachedGroupMember;

		m_pCacheGroupMember->hgetAll(strKey, mapCachedGroupMember);

		string strUserID = int2string(unUserID);
		string strCreateTime = int2string(nCreateTime);

		if (mapCachedGroupMember.find(strUserID) == mapCachedGroupMember.end())
		{
			if (unStatus == 0)  //a new member
			{
				m_pCacheGroupMember->hset(strKey, strUserID, strCreateTime);
			}
		}
		else
		{
			if (unStatus == 1) //deleted member
			{
				m_pCacheGroupMember->hdel(strKey, strUserID);
			}
		}
	}
}


void CDataSync::SetGroupUserStatus(string& strGroupID)
{
	if (m_pCacheGroupMember)
	{
		string strSql = "select a.groupid as gid,userid,a.created as acreated, a.status as status1,b.status as status2 from IMGroupMember a, IMGroup b where b.groupid= '" + strGroupID + "' and a.groupid=b.id";
		map<string, string> mapCachedGroupMember;


		MAP_MEMBER_INFO mapDBGroupMember; //key:userid,value:status
		uint32_t unGroupID = 0;
		CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());

		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				unGroupID = pResultSet->GetInt("gid");
				uint32_t unUserID = pResultSet->GetInt("userid");
				uint32_t unCreated = pResultSet->GetInt("acreated");
				uint32_t unStatus = pResultSet->GetInt("status1");
				MEMBER_INFO CurrentUser;
				memset(&CurrentUser, 0, sizeof(CurrentUser));
				CurrentUser.m_unStatus = unStatus;
				if (unStatus == 0)
				{
					CurrentUser.m_unCreateTime = unCreated;
					mapDBGroupMember[unUserID] = CurrentUser;
				}
			}
			delete pResultSet;
			pResultSet = NULL;
		}
			
		//get members for a group from cache
		string strKey =  "group_member_"  + int2string(unGroupID);
		m_pCacheGroupMember->hgetAll(strKey, mapCachedGroupMember);
		
		//both cache and db data are consistent
		//delete cache data first
		for (map<string, string>::iterator it = mapCachedGroupMember.begin(); it != mapCachedGroupMember.end(); it++)
		{
			if (mapDBGroupMember.find(string2int(it->first)) == mapDBGroupMember.end())
			{
				m_pCacheGroupMember->hdel(strKey, it->first);
			}
		}


		for (IT_MAP_MEMBER_INFO it1 = mapDBGroupMember.begin(); it1 != mapDBGroupMember.end(); it1++)
		{
			char szUserID[32], szCreateTime[32];
			memset(szUserID, 0, sizeof(szUserID));
			memset(szCreateTime, 0, sizeof(szCreateTime));

			snprintf(szUserID, sizeof(szUserID)-1, "%d", it1->first);
			snprintf(szCreateTime, sizeof(szCreateTime)-1, "%d", it1->second.m_unCreateTime);

			if (mapCachedGroupMember.find(szUserID) == mapCachedGroupMember.end())
			{

				if (it1->second.m_unStatus == 0)  //a new member
				{
					m_pCacheGroupMember->hset(strKey, szUserID, szCreateTime);
				}
			}
		}
		
		//clear members 
		mapDBGroupMember.clear();
		mapCachedGroupMember.clear();
	}
}


void CDataSync::SetAllUserStatus(void)
{
	if (m_pCacheUser)
	{
		string strSql = "select id,status,buyproduct from IMUser";
		log("SQL for getting id: %s", strSql.c_str());
		CResultSet *pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				uint32_t unUserID = pResultSet->GetInt("id");
				int32_t nStatus = pResultSet->GetInt("status");
				string strBuyProduct = "";
				if (pResultSet->GetString("buyproduct") != NULL)
				{
					strBuyProduct = pResultSet->GetString("buyproduct");
				}

				//nStatus = GetStatusByProduct(strBuyProduct, nStatus); //get status by products

				string strKey = "user_" + int2string(unUserID);
				string strStatus = int2string(nStatus);
				//m_pCacheUser->set(strKey, int2string(unStatus));
				log("KEY: %s, VALUE: %s", strKey.c_str(), strStatus.c_str());
				m_pCacheUser->set(strKey, strStatus);
			}
			delete pResultSet;
			pResultSet = NULL;
		}
	}
}


void CDataSync::GetUserAllAlias(string& strInputString, LIST_USER_ALIAS& listAlias)
{
	Json::Reader Reader;
	Json::Value Root;

	int nResult = -1;
	try
	{
		if (Reader.parse(strInputString, Root))
		{
			int nSize = Root.size();

			for (int i = 0; i < nSize; i++)
			{
				Json::Value jsonUserAlias = Root[i];
				USER_ALIAS userAlias;
				
				if (!jsonUserAlias["aliasId"].isNull() && jsonUserAlias["aliasId"].isString())
				{
					userAlias.m_strUUID = jsonUserAlias["aliasId"].asString();
				}

				if (!jsonUserAlias["userId"].isNull() && jsonUserAlias["userId"].isString())
				{
					userAlias.m_strUserID = jsonUserAlias["userId"].asString();
				}

				if (!jsonUserAlias["aliasName"].isNull() && jsonUserAlias["aliasName"].isString())
				{
					userAlias.m_strAliasName = jsonUserAlias["aliasName"].asString();
				}
				else
				{
					userAlias.m_strAliasName = "";
				}

				listAlias.push_back(userAlias);
			}
		}
	}
	catch (std::runtime_error msg)
	{
		log("user alias,parse json failed");
	}
}


void CDataSync::SetUserAllAlias(string& strUserUuid, LIST_USER_ALIAS& listAlias)
{
	string strSql = "BEGIN";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "delete from UserAlias where useruuid='" + strUserUuid + "'";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	uint32_t unCurrentTime = time(NULL);
	strSql = "insert into UserAlias (uuid,useruuid,aliasname,created,updated) values(?,?,?,?,?)";
	CPrepareStatement* pStmt = new CPrepareStatement();
	if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
	{
		for (IT_LIST_USER_ALIAS it = listAlias.begin(); it != listAlias.end(); it++)
		{		
			int nIndex = 0;
			pStmt->SetParam(nIndex++, it->m_strUUID);
			pStmt->SetParam(nIndex++, it->m_strUserID);
			pStmt->SetParam(nIndex++, it->m_strAliasName);
			pStmt->SetParam(nIndex++, unCurrentTime);
			pStmt->SetParam(nIndex++, unCurrentTime);
			pStmt->ExecuteUpdate();
		}
	}

	delete pStmt;
	pStmt = NULL;

	strSql = "COMMIT";
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::DeleteUserAliasByAliasId(const string& strAliasId)
{
	string strSql = "BEGIN";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "delete from UserAlias where uuid='" + strAliasId + "'";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "update IMUser a, UserAlias b set a.updated=unix_timestamp() where a.useruuid=b.useruuid";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "COMMIT";
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::DeleteUserAliasByUserId(const string& strUserId)
{
	string strSql = "BEGIN";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "delete from UserAlias where useruuid='" + strUserId + "'";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "update IMUser set updated=unix_timestamp() where useruuid= '" + strUserId + "'";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "COMMIT";
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::DeleteUserAliasByAliasName(const string& strAliasName)
{
	string strSql = "BEGIN";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "delete from UserAlias where aliasname='" + strAliasName + "'";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "update IMUser a, UserAlias b set a.updated=unix_timestamp() where a.useruuid=b.useruuid and b.aliasname='" + strAliasName + "'";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "COMMIT";
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::ChangeUserInfo(uint32_t unUserID, string& strUserID, string& strUserInfo)
{
	Json::Value Root;
	/*string strSql = "BEGIN";
	m_pDBConn->ExecuteUpdate(strSql.c_str());*/

	string strSql = "select useruuid, avatar,unix_timestamp(birthday) as birth,sex,signature from IMUser where id=" + int2string(unUserID);
	CResultSet *pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			if (pResultSet->GetString("useruuid") != NULL)
			{
				strUserID = pResultSet->GetString("useruuid");
			}
			else
			{
				strUserID = "";
			}

			Root["userId"] = strUserID;

			if (pResultSet->GetString("avatar") != NULL)
			{
				Root["headPortraitd"] = pResultSet->GetString("avatar");
			}
			else
			{
				Root["headPortraitd"] = "";
			}
			
			if (pResultSet->GetString("signature") != NULL)
			{
				Root["signature"] = pResultSet->GetString("signature");
			}
			else
			{
				Root["signature"] = "";
			}

			if (pResultSet->GetString("birth") != NULL)
			{
				Root["birthday"] = pResultSet->GetInt("birth") * 1000;
			}
			else
			{
				Root["birthday"] = 0;
			}
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	//Root["userDetails"] = value;


	strSql = "select uuid,useruuid,type,title,content from IMUser where useruuid='" + strUserID + "'";
	pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			
			;

		}
		delete pResultSet;
		pResultSet = NULL;
	}
	//strOutData.append(Root.toStyledString())
}


int CDataSync::GetUserAlias(const string& strInputString, USER_ALIAS& Alias)
{
	Json::Reader Reader;
	Json::Value Root;

	int nResult = -1;
	try
	{
		if (Reader.parse(strInputString, Root))
		{
			if (!Root["resultCode"].isNull() && Root["resultCode"].isString())
			{
				string strResult = Root["resultCode"].asString();
				if (strResult == "0000")
				{
					nResult = 1;
				}
				else
				{
					return nResult;
				}
			}
			else
			{
				return nResult;
			}

			if (!Root["data"].isNull())
			{
				Json::Value Data = Root["data"];
				if (!Data["aliasId"].isNull() && Data["aliasId"].isString())
				{
					Alias.m_strUUID = Data["aliasId"].asString();
				}
				else
				{
					return -1;
				}

				if (!Data["userId"].isNull() && Data["userId"].isString())
				{
					Alias.m_strUserID = Data["userId"].asString();
				}
				else
				{
					return -1;
				}

				if (!Data["username"].isNull() && Data["username"].isString())
				{
					Alias.m_strUserName = Data["username"].asString();
				}
				else
				{
					Alias.m_strUserName = "";
				}

				if (!Data["aliasName"].isNull() && Data["aliasName"].isString())
				{
					Alias.m_strAliasName = Data["aliasName"].asString();
				}
				else
				{
					Alias.m_strAliasName = "";
				}
			}
		}
	}
	catch (exception& Exception)
	{
		log("an exception occurred; %s", Exception.what());
	}

	return nResult;
}


void CDataSync::AddUserAlias(USER_ALIAS& Alias)
{
	string strSql = "BEGIN";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	//if the uuid exists? to be judge first(make up later)
	uint32_t unCurrTime = time(NULL);
	strSql = "insert into UserAlias (uuid,useruuid,username,aliasname,created,updated) values(?,?,?,?,?,?)";
	log("SQL for inserting useralias: %s", strSql.c_str());
	CPrepareStatement* pStmt = new CPrepareStatement();
	if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
	{
		int nIndex = 0;

		pStmt->SetParam(nIndex++, Alias.m_strUUID);
		pStmt->SetParam(nIndex++, Alias.m_strUserID);
		pStmt->SetParam(nIndex++, Alias.m_strUserName);
		pStmt->SetParam(nIndex++, Alias.m_strAliasName);
		pStmt->SetParam(nIndex++, unCurrTime);
		pStmt->SetParam(nIndex++, unCurrTime);
		pStmt->ExecuteUpdate();
	}
	delete pStmt;
	pStmt = NULL;

	strSql = "update IMUser set updated=" + int2string(unCurrTime) + "  where useruuid='" + Alias.m_strUserID + "'";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "COMMIT";
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::UpdateUserAlias(USER_ALIAS& Alias)
{
	string strSql = "update IMUser a, UserAlias b set a.updated=unix_timestamp(), b.updated=a.updated, b.aliasname='" + Alias.m_strAliasName + "' where a.useruuid= b.useruuid and b.uuid='" + Alias.m_strUUID + "'";
	log("SQL for updating useralias; %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::SetAllGroupUserStatus(void)
{
	if (m_pCacheGroupMember)
	{
		//get group ids
		list<uint32_t> listGroup;
		string strSql = "select id from IMGroup where status = 0";
		CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				uint32_t unGroupId = pResultSet->GetInt("id");
				listGroup.push_back(unGroupId);
			}
			delete pResultSet;
			pResultSet = NULL;
		}

		for (list<uint32_t>::iterator it = listGroup.begin(); it != listGroup.end(); it++)
		{
			map<string, string> mapCachedGroupMember;
			//get members for a group from cache
			string strKey = "group_member_" + int2string(*it);
			m_pCacheGroupMember->hgetAll(strKey, mapCachedGroupMember);
			if (mapCachedGroupMember.size() == 0)
			{
				strSql = "select userid,created from IMGroupMember where groupid=" + int2string(*it) + " and status = 0";
				CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
				if (pResultSet)
				{
					while (pResultSet->Next())
					{
						uint32_t unUserID = pResultSet->GetInt("userid");
						uint32_t unCreated = pResultSet->GetInt("created");
						string strUserID = int2string(unUserID);
						string strCreated = int2string(unCreated);
						m_pCacheGroupMember->hset(strKey, strUserID, strCreated);
					}
					delete pResultSet;
					pResultSet = NULL;
				}
			}
			else
			{
				mapCachedGroupMember.clear();
			}
		}
	}
}


void CDataSync::GetCompanyByCompanyId(string& strInputString, DOMAIN_INFO& DomainInfo)
{
	try
	{
		Json::Reader Reader;
		Json::Value Root;

		if (Reader.parse(strInputString, Root))
		{
			string strResult = "";
			if (Root["resultCode"].isNull() && Root["resultCode"].isString())
			{
				strResult = Root["resultCode"].asString();
				if (strResult != "0000")
				{
					return;
				}
			}

			if (Root["data"].isNull())
			{
				return;
			}

			Json::Value msgData = Root["data"];

			if (!msgData["primaryOrgId"].isNull() && msgData["primaryOrgId"].isString())
			{
				DomainInfo.m_strCompanyUuid = msgData["primaryOrgId"].asString();
			}
			else
			{
				DomainInfo.m_strCompanyUuid = "";
			}

			if (!msgData["company"].isNull() && msgData["company"].isString())
			{
				DomainInfo.m_strCompanyName = msgData["company"].asString();
			}
			else
			{
				DomainInfo.m_strCompanyName = "";
			}
		}
	}
	catch (std::runtime_error msg)
	{
		log("error message");
	}
	catch (exception& Exception)
	{
		log("Exception occured: %s", Exception.what());
	}
}


//void CDataSync::UpdateCompanyByDomainId(DOMAIN_INFO& DomainInfo)
//{
//	string strSql = "update IMCompany set companyname='" + DomainInfo.m_strCompanyName + "',updated=unix_timestamp() where domainid='" + DomainInfo.m_strDomainID + "'";
//	log("SQL for getting company name: %s", strSql.c_str());
//	m_pDBConn->ExecuteUpdate(strSql.c_str());
//}


void CDataSync::UpdateRecentSession(string& strGroupId)
{
	//update recent session
	string strSql = "update IMRecentSession a, IMGroup b set a.status=1, a.updated=unix_timestamp() where a.peerid=b.id and b.groupid='" + strGroupId + "' and a.type=2";
	log("SQL for delete recent session: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::SetAllCompanyStaff(void)
{
	//get company id
	string strSql = "select id from IMCompany where status = 1";
	log("SQL for getting company id: %s", strSql.c_str());

	list<uint32_t> listCompany;
	CResultSet *pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		uint32_t unCompanyID = 0;
		while (pResultSet->Next())
		{
			unCompanyID = pResultSet->GetInt("id");
			listCompany.push_back(unCompanyID);
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	//get users for each company
	for (list<uint32_t>::iterator it = listCompany.begin(); it != listCompany.end(); it++)
	{
		strSql = "select userid,uuid from IMStaff where companyid = " + int2string(*it) + " and status <> -1";
		log("SQL for getting staff: %s", strSql.c_str());
		pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
		uint32_t unUserId = 0;
		string strStaffUuid;
		MAP_USER_KEY mapUserKey;

		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				unUserId = pResultSet->GetInt("userid");
				strStaffUuid = pResultSet->GetString("uuid");
				mapUserKey[unUserId] = strStaffUuid;
			}

			delete pResultSet;
			pResultSet = NULL;

			if (mapUserKey.size() > 0)
			{
				//m_mapDomainUser[*it] = mapUserKey;
				m_mapDomainUser.insert(make_pair((*it), mapUserKey));
				log("USERS %d in the company: %d, elements: %d", mapUserKey.size(), *it, m_mapDomainUser.size());
			}
		}
	}
}


int  CDataSync::GetCompanyIdByCompanyUuid(string& strCompanyUuid)
{
	string strSql = "select id from IMCompany where companyuuid='" + strCompanyUuid + "'";
	CResultSet *pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());

	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			m_nCompanyId = pResultSet->GetInt("id");
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	return m_nCompanyId;
}


int CDataSync::GetCompanyIdByStaffUuid(string& strStaffUuid)
{
	string strSql = "select companyid from IMStaff where uuid='" + strStaffUuid + "'";
	CResultSet *pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			m_nCompanyId = pResultSet->GetInt("companyid");
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	return m_nCompanyId;
}


int CDataSync::GetCompanyIdByUserId(uint32_t unUserId)
{
	string strSql = "select companyid from IMUser where id= " + int2string(unUserId);
	CResultSet *pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			m_nCompanyId = pResultSet->GetInt("id");
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	return m_nCompanyId;
}


int CDataSync::GetCompanyIdByDepartId(string& strDepartId)
{
	string strSql = "select distinct companyid from IMDepart where uuid='" + strDepartId + "'";
	CResultSet *pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			m_nCompanyId = pResultSet->GetInt("companyid");
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	return m_nCompanyId;
}


int CDataSync::GetCompanyIdByPositionId(string& strPositionId)
{
	string strSql = "select distinct companyid from Position where uuid='" + strPositionId + "'";
	CResultSet *pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			m_nCompanyId = pResultSet->GetInt("companyid");
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	return m_nCompanyId;

}


void CDataSync::SendUserAddNotify(uint32_t unUserId, USER_INFO& UserInfo)
{
	IM::Buddy::IMUserSyncNotify msgNotify;

	msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_ADD_USER);
	msgNotify.set_sync_user_id(unUserId);
	msgNotify.set_sync_user_uuid(UserInfo.m_strUserUuid);
	msgNotify.set_user_name(UserInfo.m_strUserName);
	msgNotify.set_user_real_name(UserInfo.m_strName);
	msgNotify.set_avatar_url(UserInfo.m_strAvatar);
	msgNotify.set_user_gender(UserInfo.m_nSex);
	msgNotify.set_birthday(UserInfo.m_unBirthday);
	msgNotify.set_mobile_phone(UserInfo.m_strPhone);
	msgNotify.set_status(UserInfo.m_nStatus);
	msgNotify.set_keyword(UserInfo.m_strKeyword);
	msgNotify.set_user_nick_name(UserInfo.m_strNickName);
	msgNotify.set_signature(UserInfo.m_strSignature);
	msgNotify.set_is_actived(UserInfo.m_unIsActived);
	msgNotify.set_email(UserInfo.m_strEmail);
	msgNotify.set_account(UserInfo.m_strAccount);

	//msgNotify.set_buyproduct(UserInfo.m_strBuyProducts);
	//uint32_t unBuyProduct = GetProduct(UserInfo.m_strBuyProducts);
	//msgNotify.set_orderproduct(unBuyProduct);

	//CRouteServConn* pRouteConn = get_route_serv_conn();
	//if (pRouteConn)
	//{
	//	pRouteConn->Lock();

	//	for (IT_LIST_UINT it = m_listUser.begin(); it != m_listUser.end(); it++)
	//	{
	//		log("Sending notify to  %d", *it);
	//		msgNotify.set_user_id(*it);
	//		CImPdu pdu;
	//		pdu.SetPBMsg(&msgNotify);
	//		pdu.SetServiceId(SID_BUDDY_LIST);
	//		pdu.SetCommandId(CID_BUDDY_LIST_SYNC_USER_NOTIFY);

	//		int nBytes = pRouteConn->SendPdu(&pdu);
	//		log("Sent %d bytes to route server", nBytes);
	//	}

	//	log("%d users before adding a user: %d", m_listUser.size(), unUserId);
	//	m_listUser.push_back(unUserId); // add a new user for the company
	//	log("%d users after adding a user: %d", m_listUser.size(), unUserId);
	//

	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn)
	{
		pRouteConn->Lock();

		//send usr add notify to all staff in the company
		IT_MAP_COMPANY_USER it = m_mapDomainUser.find(m_nCompanyId);
		if (it != m_mapDomainUser.end())
		{
			for (IT_MAP_USER_KEY itUser = it->second.begin(); itUser != it->second.end(); itUser++)	//send notify to all users in the company
			{
				log("Sending notify to  %d", itUser->first);
				msgNotify.set_user_id(itUser->first);
				CImPdu pdu;
				pdu.SetPBMsg(&msgNotify);
				pdu.SetServiceId(SID_BUDDY_LIST);
				pdu.SetCommandId(CID_BUDDY_LIST_SYNC_USER_NOTIFY);

				int nBytes = pRouteConn->SendPdu(&pdu);
				log("Sent %d bytes to route server", nBytes);
			}
			//log("%d users before adding a user: %d", it->second.size(), unUserId);
			//it->second.insert(make_pair(unUserId, UserInfo.m_strUserID)); // add a new user for the company
			//log("%d users after adding a user: %d", it->second.size(), unUserId);
		}

		pRouteConn->Unlock();
	}
	else
	{
		log("route server is unavailable!");
	}
}


void CDataSync::SendUserUpdateNotify(uint32_t unUserId, int nStatus, USER_INFO& UserInfo)
{
	IM::Buddy::IMUserSyncNotify msgNotify;
	
	msgNotify.set_sync_user_id(unUserId);
	msgNotify.set_sync_user_uuid(UserInfo.m_strUserUuid);
	msgNotify.set_user_name(UserInfo.m_strUserName);
	msgNotify.set_user_real_name(UserInfo.m_strName);
	msgNotify.set_avatar_url(UserInfo.m_strAvatar);
	msgNotify.set_user_gender(UserInfo.m_nSex);
	msgNotify.set_birthday(UserInfo.m_unBirthday);
	msgNotify.set_mobile_phone(UserInfo.m_strPhone);
	msgNotify.set_status(UserInfo.m_nStatus);
	msgNotify.set_keyword(UserInfo.m_strKeyword);
	msgNotify.set_user_nick_name(UserInfo.m_strNickName);
	msgNotify.set_signature(UserInfo.m_strSignature);
	msgNotify.set_is_actived(UserInfo.m_unIsActived);
	msgNotify.set_email(UserInfo.m_strEmail);
	msgNotify.set_account(UserInfo.m_strAccount);

	/*uint32_t unBuyProduct = GetProduct(UserInfo.m_strBuyProducts);
	msgNotify.set_orderproduct(unBuyProduct);*/

	if(nStatus == 1 && UserInfo.m_nStatus == 0)
	{
		msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_DISABLE_USER);
	}
	else if(nStatus == 1 && UserInfo.m_nStatus == -1)
	{
		msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_DEL_USER);
	}
	else if(nStatus == 0 && UserInfo.m_nStatus == 1)
	{
		msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_ENABLE_USER);
	}
	else if(nStatus == 0 && UserInfo.m_nStatus == -1)
	{
		msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_DEL_USER);
	}
	else if(nStatus == -1 && UserInfo.m_nStatus == 1)
	{
		msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_ENABLE_USER);
	}
	else if(nStatus == -1 && UserInfo.m_nStatus == 0)
	{
		msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_DISABLE_USER);
	}
	else if(nStatus == UserInfo.m_nStatus)
	{
		msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_MOD_USER);
	}	
	
	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn)
	{
		pRouteConn->Lock();

		for (IT_LIST_UINT it = m_listUser.begin(); it != m_listUser.end();)	//send notify to all users in the company
		{
			log("Sending notify to  %d", *it);
			msgNotify.set_user_id(*it);

			CImPdu pdu;
			pdu.SetPBMsg(&msgNotify);
			pdu.SetServiceId(SID_BUDDY_LIST);
			pdu.SetCommandId(CID_BUDDY_LIST_SYNC_USER_NOTIFY);

			int nBytes = pRouteConn->SendPdu(&pdu);
			log("Sent %d bytes to route server", nBytes);

			if (*it == unUserId && UserInfo.m_nStatus == -1)
			{
				IT_LIST_UINT it1 = it;
				it++;
				m_listUser.erase(it1);
			}
			else
			{
				it++;
			}
		}

		pRouteConn->Unlock();
	}
	else
	{
		log("route server is unavailable!");
	}

	if (nStatus == -1 && UserInfo.m_nStatus != -1)
	{
		m_listUser.push_back(unUserId); //add a new user
	}


	//IT_MAP_COMPANY_USER it = m_mapDomainUser.find(m_nCompanyId);
	//if (it != m_mapDomainUser.end())
	//{
	//	CRouteServConn* pRouteConn = get_route_serv_conn();
	//	if (pRouteConn)
	//	{
	//		pRouteConn->Lock();

	//		for (IT_LIST_UINT it = m_listUser.begin(); it != m_listUser.end(); )	//send notify to all users in the company
	//		{
	//			log("Sending notify to  %d", *it);
	//			msgNotify.set_user_id(*it);

	//			CImPdu pdu;
	//			pdu.SetPBMsg(&msgNotify);
	//			pdu.SetServiceId(SID_BUDDY_LIST);
	//			pdu.SetCommandId(CID_BUDDY_LIST_SYNC_USER_NOTIFY);

	//			int nBytes = pRouteConn->SendPdu(&pdu);
	//			log("Sent %d bytes to route server", nBytes);

	//			if (*it == unUserId && UserInfo.m_nStatus == -1)
	//			{
	//				IT_LIST_UINT it1 = it;
	//				it++;
	//				m_listUser.erase(it1);
	//			}
	//			else
	//			{
	//				it++;
	//			}
	//		}

	//		pRouteConn->Unlock();
	//	}
	//	else
	//	{
	//		log("route server is unavailable!");
	//	}
	//	
	//	//if(UserInfo.m_nStatus == -1)
	//	//{
	//	//	it->second.erase(unUserId);  //delete the user
	//	//}
	//	//else if(nStatus == -1 && UserInfo.m_nStatus != -1)
	//	if (nStatus == -1 && UserInfo.m_nStatus != -1)
	//	{
	//		//it->second.insert(make_pair(unUserId, UserInfo.m_strUserID)); // add a new user for the company
	//		m_listUser.push_back(unUserId); //add a new user
	//	}
	//}
}


void CDataSync::SendPasswordChangeNotify(uint32_t unUserId)
{
	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn)
	{
		pRouteConn->Lock();

		IM::Buddy::IMPasswordChangeNotify msgNotify;
		msgNotify.set_user_id(unUserId);
				
		CImPdu pdu;
		pdu.SetPBMsg(&msgNotify);
		pdu.SetServiceId(SID_BUDDY_LIST);
		pdu.SetCommandId(CID_BUDDY_LIST_SYNC_PASSWORD_NOTIFY);

		int nBytes = pRouteConn->SendPdu(&pdu);
		log("Sent %d bytes to route server", nBytes);
			
		pRouteConn->Unlock();
	}
	else
	{
		log("route server is unavailable!");
	}
}


void CDataSync::SendUserDelNotify(uint32_t unUserId, string& strUserId)
{
	IM::Buddy::IMUserSyncNotify msgNotify;
	
	msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_DEL_USER);
	msgNotify.set_sync_user_id(unUserId);
	msgNotify.set_sync_user_uuid(strUserId);
	msgNotify.set_status(-1);

	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn)
	{
		pRouteConn->Lock();

		for (IT_LIST_UINT it = m_listUser.begin(); it != m_listUser.end();)	//send notify to all users in the company
		{
			log("Sending notify to  %d", *it);
			msgNotify.set_user_id(*it);

			CImPdu pdu;
			pdu.SetPBMsg(&msgNotify);
			pdu.SetServiceId(SID_BUDDY_LIST);
			pdu.SetCommandId(CID_BUDDY_LIST_SYNC_USER_NOTIFY);

			int nBytes = pRouteConn->SendPdu(&pdu);
			log("Sent %d bytes to route server", nBytes);

			if (*it == unUserId)
			{
				IT_LIST_UINT it1 = it;
				it++;
				m_listUser.erase(it1);
			}
			else
			{
				it++;
			}
		}

		pRouteConn->Unlock();
	}
	else
	{
		log("route server is unavailable!");
	}

	//IT_MAP_COMPANY_USER it = m_mapDomainUser.find(m_nCompanyId);
	//if (it != m_mapDomainUser.end())
	//{
	//	log("%d users before deleting %d", it->second.size(), unUserId);

	//	CRouteServConn* pRouteConn = get_route_serv_conn();
	//	if (pRouteConn)
	//	{
	//		pRouteConn->Lock();

	//		for (IT_LIST_UINT it = m_listUser.begin(); it != m_listUser.end(); )	//send notify to all users in the company
	//		{
	//			log("Sending notify to  %d", *it);
	//			msgNotify.set_user_id(*it);

	//			CImPdu pdu;
	//			pdu.SetPBMsg(&msgNotify);
	//			pdu.SetServiceId(SID_BUDDY_LIST);
	//			pdu.SetCommandId(CID_BUDDY_LIST_SYNC_USER_NOTIFY);

	//			int nBytes = pRouteConn->SendPdu(&pdu);
	//			log("Sent %d bytes to route server", nBytes);

	//			if (*it == unUserId)
	//			{
	//				IT_LIST_UINT it1 = it;
	//				it++;
	//				m_listUser.erase(it1);
	//			}
	//			else
	//			{
	//				it++;
	//			}
	//		}

	//		//for (IT_MAP_USER_KEY itUser = it->second.begin(); itUser != it->second.end(); itUser++)	//send notify to all users in the company
	//		//{
	//		//	log("Sending notify to  %d", itUser->first);
	//		//	msgNotify.set_user_id(itUser->first);

	//		//	CImPdu pdu;
	//		//	pdu.SetPBMsg(&msgNotify);
	//		//	pdu.SetServiceId(SID_BUDDY_LIST);
	//		//	pdu.SetCommandId(CID_BUDDY_LIST_SYNC_USER_NOTIFY);

	//		//	int nBytes = pRouteConn->SendPdu(&pdu);
	//		//	log("Sent %d bytes to route server", nBytes);
	//		//}

	//		pRouteConn->Unlock();
	//	}
	//	else
	//	{
	//		log("route server is unavailable!");
	//	}
	//	
		//m_listUser.erase(unUserId);  //delete the user
		log("%d users after deleting %d", m_listUser.size(), unUserId);
	//}
}


void CDataSync::SendGroupMemberChangeNotify(uint32_t unGroupId, uint32_t unUserId, list<uint32_t>& listCurrUsers)
{
	IM::Group::IMGroupChangeMemberNotify msgNotify;
	msgNotify.set_user_id(0);
	msgNotify.set_group_id(unGroupId);
	msgNotify.set_change_type(IM::BaseDefine::GROUP_MODIFY_TYPE_DEL);

	msgNotify.add_chg_user_id_list(unUserId); //do not send notify to be deleted /disabled user
	for(list<uint32_t>::iterator it = listCurrUsers.begin(); it != listCurrUsers.end(); it++)
	{
		msgNotify.add_cur_user_id_list(*it);
	}

	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn)
	{
		pRouteConn->Lock();

		CImPdu pdu;
		pdu.SetPBMsg(&msgNotify);
		pdu.SetServiceId(SID_GROUP);
		pdu.SetCommandId(CID_GROUP_CHANGE_MEMBER_NOTIFY);

		int nBytes = pRouteConn->SendPdu(&pdu);
		log("Sent %d bytes to route server", nBytes);

		pRouteConn->Unlock();
	}
	else
	{
		log("route server is unavailable!");
	}
}


void CDataSync::SendCreatorChangeNotify(uint32_t unGroupId,uint32_t unNewCreator, list<uint32_t>& listCurrUsers)
{
	IM::Group::IMGroupTransferLeaderNotify msgNotify;
	msgNotify.set_group_id(unGroupId);
	msgNotify.set_new_leader_id(unNewCreator);

	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn)
	{
		pRouteConn->Lock();

		for (list<uint32_t>::iterator it = listCurrUsers.begin(); it != listCurrUsers.end(); it++) 
		{
			log("Sending notify to  %d", *it);
			msgNotify.set_user_id(*it);

			CImPdu pdu;
			pdu.SetPBMsg(&msgNotify);
			pdu.SetServiceId(SID_GROUP);
			pdu.SetCommandId(CID_GROUP_TRANSFER_LEADER_NOTIFY);

			int nBytes = pRouteConn->SendPdu(&pdu);
			log("Sent %d bytes to route server", nBytes);
		}

		pRouteConn->Unlock();
	}
	else
	{
		log("route server is unavailable!");
	}
}


void CDataSync::SendDepartmentChangeNotify(DEPARTMENT_INFO& DepartInfo, uint32_t unSyncType)
{
	IM::Buddy::IMDepartSyncNotify msgNotify;

	msgNotify.set_sync_type((IM::BaseDefine::DataSyncType)unSyncType);
	msgNotify.set_dept_uuid(DepartInfo.m_strDepartmentID);
	msgNotify.set_priority(DepartInfo.m_nPriority);
	msgNotify.set_dept_name(DepartInfo.m_strDepartmentName);
	msgNotify.set_parent_dept_uuid(DepartInfo.m_strParentDepartmentID);
	msgNotify.set_dept_status(DepartInfo.m_nStatus);
	msgNotify.set_dept_level(DepartInfo.m_nLevel);
	msgNotify.set_contact_status(DepartInfo.m_nContactStatus);
	msgNotify.set_company_id(m_nCompanyId);

	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn)
	{
		pRouteConn->Lock();

		//find users in a company
		IT_MAP_COMPANY_USER it = m_mapDomainUser.find(m_nCompanyId);
		if (it != m_mapDomainUser.end())
		{
			for (IT_MAP_USER_KEY itUser = it->second.begin(); itUser != it->second.end(); itUser++)	//send notify to all users in the company
			{
				msgNotify.set_user_id(itUser->first);

				CImPdu pdu;
				pdu.SetPBMsg(&msgNotify);
				pdu.SetServiceId(SID_BUDDY_LIST);
				pdu.SetCommandId(CID_BUDDY_LIST_SYNC_DEPART_NOTIFY);
						
				int nBytes = pRouteConn->SendPdu(&pdu);
				log("Sent %d bytes to route server", nBytes);
			}
		}

		pRouteConn->Unlock();
	}
	else
	{
		log("route server is unavailable!........");
	}
}

void CDataSync::SendDepartmentDelNotify(string& strDepartId)
{
	IM::Buddy::IMDepartSyncNotify msgNotify;

	msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_DEL_DEPART);
	msgNotify.set_dept_uuid(strDepartId);

	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn)
	{
		pRouteConn->Lock();

		//find users in a company
		IT_MAP_COMPANY_USER it = m_mapDomainUser.find(m_nCompanyId);
		if (it != m_mapDomainUser.end())
		{
			for (IT_MAP_USER_KEY itUser = it->second.begin(); itUser != it->second.end(); itUser++)	//send notify to all users in the company
			{
				msgNotify.set_user_id(itUser->first);

				CImPdu pdu;
				pdu.SetPBMsg(&msgNotify);
				pdu.SetServiceId(SID_BUDDY_LIST);
				pdu.SetCommandId(CID_BUDDY_LIST_SYNC_DEPART_NOTIFY);
						
				int nBytes = pRouteConn->SendPdu(&pdu);
				log("Sent %d bytes to route server", nBytes);
			}
		}

		pRouteConn->Unlock();
	}
	else
	{
		log("route server is unavailable!........");
	}
}


void CDataSync::SendPositionChangeNotify(POSITION_INFO& PositionInfo, uint32_t unSyncType)
{
	IM::Buddy::IMPositionSyncNotify msgNotify;

	msgNotify.set_sync_type((IM::BaseDefine::DataSyncType)unSyncType);
	msgNotify.set_position_uuid(PositionInfo.m_strPositionID);  

	uint32_t unPositionType = IM::BaseDefine::NONE;
	if (PositionInfo.m_strPositionType == "chiefleader")
	{
		unPositionType = IM::BaseDefine::CHIEF_LEADER;
	}
	else if (PositionInfo.m_strPositionType == "leader")
	{
		unPositionType = IM::BaseDefine::LEADER;
	}
	else if (PositionInfo.m_strPositionType == "staff")
	{
		unPositionType = IM::BaseDefine::STAFF;
	}
	else if (PositionInfo.m_strPositionType == "other")
	{
		unPositionType = IM::BaseDefine::OTHER;
	}

	msgNotify.set_position_type((IM::BaseDefine::JobGradeType)unPositionType);
 	msgNotify.set_position_name(PositionInfo.m_strPositionName); 
	msgNotify.set_status(PositionInfo.m_nStatus);
	msgNotify.set_sort(PositionInfo.m_nSort);
	msgNotify.set_company_id(m_nCompanyId);

	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn)
	{
		pRouteConn->Lock();

		//find users in a company
		IT_MAP_COMPANY_USER it = m_mapDomainUser.find(m_nCompanyId);
		if (it != m_mapDomainUser.end())
		{
			for (IT_MAP_USER_KEY itUser = it->second.begin(); itUser != it->second.end(); itUser++)	//send notify to all users in the company
			{
				msgNotify.set_user_id(itUser->first);

				CImPdu pdu;
				pdu.SetPBMsg(&msgNotify);
				pdu.SetServiceId(SID_BUDDY_LIST);
				pdu.SetCommandId(CID_BUDDY_LIST_SYNC_POSITION_NOTIFY);
						
				int nBytes = pRouteConn->SendPdu(&pdu);
				log("Sent %d bytes to route server", nBytes);
			}
		}

		pRouteConn->Unlock();
	}
	else
	{
		log("route server is unavailable!........");
	}
}


void CDataSync::SendPositionDelNotify(string& strPositionId)
{
	IM::Buddy::IMPositionSyncNotify msgNotify;

	msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_DEL_POSITION);
	msgNotify.set_position_uuid(strPositionId);  

	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn)
	{
		pRouteConn->Lock();

		//find users in a company
		IT_MAP_COMPANY_USER it = m_mapDomainUser.find(m_nCompanyId);
		if (it != m_mapDomainUser.end())
		{
			for (IT_MAP_USER_KEY itUser = it->second.begin(); itUser != it->second.end(); itUser++)	//send notify to all users in the company
			{
				msgNotify.set_user_id(itUser->first);

				CImPdu pdu;
				pdu.SetPBMsg(&msgNotify);
				pdu.SetServiceId(SID_BUDDY_LIST);
				pdu.SetCommandId(CID_BUDDY_LIST_SYNC_POSITION_NOTIFY);
						
				int nBytes = pRouteConn->SendPdu(&pdu);
				log("Sent %d bytes to route server", nBytes);
			}
		}

		pRouteConn->Unlock();
	}
	else
	{
		log("route server is unavailable!........");
	}
}


void CDataSync::SendUserPositionChangeNotify(string& strUserId, uint32_t unUserId, string& strDepartId, string& strPositionId, int unSyncType, string& strJobGrade)
{
	////temp use and will be deleted in the later version
	//POSITION_INFO PositionInfo;
	//string strSql = "select posname,type,status,sort from Position where uuid='" + strPositionId + "'";
	//CResultSet *pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	//if(pResultSet)
	//{
	//	while(pResultSet->Next())
	//	{
	//		if(pResultSet->GetString("posname") != NULL)
	//		{
	//			PositionInfo.m_strPositionName = pResultSet->GetString("posname");
	//		}
	//				
	//		if(pResultSet->GetString("type") != NULL)
	//		{
	//			PositionInfo.m_strPositionType = pResultSet->GetString("type");
	//		}

	//		PositionInfo.m_nSort = pResultSet->GetInt("sort");
	//		PositionInfo.m_nStatus = pResultSet->GetInt("status");
	//	}
	//	delete pResultSet;
	//	pResultSet = NULL;
	//}

	IM::Buddy::IMUserDutySyncNotify msgNotify;

	uint32_t unJobGrade = IM::BaseDefine::NONE;
	if (strJobGrade == "chiefleader")
	{
		unJobGrade = IM::BaseDefine::CHIEF_LEADER;
	}
	else if (strJobGrade == "leader")
	{
		unJobGrade = IM::BaseDefine::LEADER;
	}
	else if (strJobGrade == "staff")
	{
		unJobGrade = IM::BaseDefine::STAFF;
	}
	else if (strJobGrade == "other")
	{
		unJobGrade = IM::BaseDefine::OTHER;
	}


	//msgNotify.set_user_id(itUser->first);
	//msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_ADD_USER_DUTY);
	msgNotify.set_sync_type((IM::BaseDefine::DataSyncType)unSyncType);
	msgNotify.set_sync_user_id(unUserId);
	msgNotify.set_sync_user_uuid(strUserId);
	msgNotify.set_depart_uuid(strDepartId);
	msgNotify.set_position_uuid(strPositionId);  
	msgNotify.set_job_grade((IM::BaseDefine::JobGradeType)unJobGrade);
	/*msgNotify.set_position_type(PositionInfo.m_strPositionType); 
 	msgNotify.set_position_name(PositionInfo.m_strPositionName); 
	msgNotify.set_status(PositionInfo.m_nStatus);
	msgNotify.set_sort(PositionInfo.m_nSort);
*/
	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn)
	{
		pRouteConn->Lock();

		//find users in a company
		IT_MAP_COMPANY_USER it = m_mapDomainUser.find(m_nCompanyId);
		if (it != m_mapDomainUser.end())
		{
			for (IT_MAP_USER_KEY itUser = it->second.begin(); itUser != it->second.end(); itUser++)	//send notify to all users in the company
			{
				msgNotify.set_user_id(itUser->first);

				CImPdu pdu;
				pdu.SetPBMsg(&msgNotify);
				pdu.SetServiceId(SID_BUDDY_LIST);
				pdu.SetCommandId(CID_BUDDY_LIST_SYNC_USER_DUTY_NOTIFY);
						
				int nBytes = pRouteConn->SendPdu(&pdu);
				log("Sent %d bytes to route server", nBytes);
			}
		}

		pRouteConn->Unlock();
	}
	else
	{
		log("route server is unavailable!........");
	}
}


int CDataSync::GetUserInfoByUserId(string& strUserId, USER_INFO& UserInfo)
{
	UserInfo.m_strUserUuid = strUserId;
	uint32_t unUserId = 0;

	string strSql = "select id,username,name,avatar,sex,birthday,phone,status,keyword,signature,nick,email,isactived,buyproduct from IMUser where useruuid='" + strUserId + "'";
	log("SQL for getting user information: %s", strSql.c_str());

	CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		while (pResultSet->Next())
		{

			unUserId = pResultSet->GetInt("id");

			if (pResultSet->GetString("signature") == NULL)
			{
				UserInfo.m_strSignature = "";
			}
			else
			{
				UserInfo.m_strSignature = pResultSet->GetString("signature");
			}

			UserInfo.m_nSex = pResultSet->GetInt("sex");

			if (pResultSet->GetString("name") == NULL)
			{
				UserInfo.m_strName = "";
			}
			else
			{
				UserInfo.m_strName = pResultSet->GetString("name");
			}

			if (pResultSet->GetString("username") == NULL)
			{
				UserInfo.m_strUserName = "";
			}
			else
			{
				UserInfo.m_strUserName = pResultSet->GetString("username");
			}

			if (pResultSet->GetString("nick") == NULL)
			{
				UserInfo.m_strNickName = UserInfo.m_strName;
			}
			else
			{
				UserInfo.m_strNickName = pResultSet->GetString("nick");
			}

			if (pResultSet->GetString("keyword") == NULL)
			{
				UserInfo.m_strKeyword = "";
			}
			else
			{
				UserInfo.m_strKeyword = pResultSet->GetString("keyword");
			}

			if (pResultSet->GetString("phone") == NULL)
			{
				UserInfo.m_strPhone = "";
			}
			else
			{
				UserInfo.m_strPhone = pResultSet->GetString("phone");
			}

			/*if (pResultSet->GetString("birthday") == NULL)
			{
				UserInfo.m_strBirthday = "";
			}
			else
			{
				UserInfo.m_strBirthday = pResultSet->GetString("birthday");
			}*/

			UserInfo.m_unBirthday = pResultSet->GetInt("birthday");
			if (pResultSet->GetString("avatar") == NULL)
			{
				UserInfo.m_strAvatar = "";
			}
			else
			{
				UserInfo.m_strAvatar = pResultSet->GetString("avatar");
			}

			//appened by george on Nov 12th,2015
			if (pResultSet->GetString("buyproduct") == NULL)
			{
				UserInfo.m_strBuyProducts = "";
			}
			else
			{
				UserInfo.m_strBuyProducts = pResultSet->GetString("buyproduct");
			}

			UserInfo.m_nStatus = pResultSet->GetInt("status");
			UserInfo.m_unIsActived = pResultSet->GetInt("isactived");

			if (pResultSet->GetString("email") == NULL)
			{
				UserInfo.m_strEmail = "";
			}
			else
			{
				UserInfo.m_strEmail = pResultSet->GetString("email");
			}

		}
		delete pResultSet;
		pResultSet = NULL;
	}

	return unUserId;
}


void CDataSync::SendMailGroupCreateNotify(uint32_t unGroupId, uint32_t unCompanyId, string& strGroupName, string& strGroupId)
{
	IM::Group::IMGroupCreateNotify msgNotify;
	msgNotify.set_user_id(0); //0 means system
	msgNotify.set_group_id(unGroupId);
	msgNotify.set_group_name(strGroupName);
	msgNotify.set_group_uuid(strGroupId);
	msgNotify.set_company_id(unCompanyId);

	string strSql = "select userid from IMGroupMember where status = 0 and groupid= " + int2string(unGroupId);
	CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	uint32_t unUserId = 0;
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			unUserId = pResultSet->GetInt("userid");
			msgNotify.add_user_id_list(unUserId);
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn)
	{
		pRouteConn->Lock();

		CImPdu pdu;
		pdu.SetPBMsg(&msgNotify);
		pdu.SetServiceId(SID_GROUP);
		pdu.SetCommandId(CID_GROUP_CREATE_NOTIFY);

		int nBytes = pRouteConn->SendPdu(&pdu);
		log("Sent %d bytes to route server", nBytes);
	
		pRouteConn->Unlock();
	}
	else
	{
		log("route server is unavailable!........");
	}
}


void CDataSync::SendMailGroupDelNotify(string& strGroupId, time_t nTicks)
{
	IM::Group::IMGroupChangeMemberNotify msgNotify;
	msgNotify.set_user_id(0);
	msgNotify.set_change_type(IM::BaseDefine::GROUP_MODIFY_TYPE_DEL);

	//get group id
	string strSql = "select id from IMGroup where groupid='" + strGroupId + "'";
	CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	uint32_t unGroupId = 0;
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			unGroupId = pResultSet->GetInt("id");
			msgNotify.set_group_id(unGroupId);
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	if (unGroupId == 0)
	{
		return;
	}

	//get recently deleted users 
	strSql = "select userid from IMGroupMember where groupid=" + int2string(unGroupId) + " and status = 1 and updated=" + longlong2string(nTicks);
	pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	uint32_t unUserId = 0;
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			unUserId = pResultSet->GetInt("userid");
			msgNotify.add_chg_user_id_list(unUserId);
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn)
	{
		pRouteConn->Lock();

		CImPdu pdu;
		pdu.SetPBMsg(&msgNotify);
		pdu.SetServiceId(SID_GROUP);
		pdu.SetCommandId(CID_GROUP_CHANGE_MEMBER_NOTIFY);

		int nBytes = pRouteConn->SendPdu(&pdu);
		log("Sent %d bytes to route server", nBytes);

		pRouteConn->Unlock();
	}
	else
	{
		log("route server is unavailable!........");
	}
}



void CDataSync::SendMailGroupChangeNameNotify(string& strGroupId, string& strNewGroupName)
{
	IM::Group::IMGroupChangeNameNotify msgNotify;
	msgNotify.set_group_new_name(strNewGroupName);

	string strSql = "select userid,b.id as bgid from IMGroupMember a, IMGroup b where a.groupid=b.id and b.groupid='" + strGroupId + "'";
	CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	uint32_t unUserId = 0;
	uint32_t unGroupId = 0;
	list<uint32_t> listUsers;

	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			unUserId = pResultSet->GetInt("userid");
			unGroupId = pResultSet->GetInt("bgid");
			listUsers.push_back(unUserId);
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	msgNotify.set_group_id(unGroupId);

	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn)
	{
		pRouteConn->Lock();

		for (list<uint32_t>::iterator it = listUsers.begin(); it != listUsers.end(); it++)
		{
			msgNotify.set_user_id(*it);

			CImPdu pdu;
			pdu.SetPBMsg(&msgNotify);
			pdu.SetServiceId(SID_GROUP);
			pdu.SetCommandId(CID_GROUP_CHANGE_GRPNAME_NOTIFY);

			int nBytes = pRouteConn->SendPdu(&pdu);
			log("Sent %d bytes to route server", nBytes);
		}

		pRouteConn->Unlock();
	}
	else
	{
		log("route server is unavailable!........");
	}
}


void CDataSync::SendMailGroupChangeMemberNotify(uint32_t unGroupId, list<uint32_t>& listChangeUsers, list<uint32_t>& listUsers, int nChangeType)
{
	IM::Group::IMGroupChangeMemberNotify msgNotify;
	msgNotify.set_user_id(0);
	msgNotify.set_change_type((IM::BaseDefine::GroupModifyType)nChangeType);
	msgNotify.set_group_id(unGroupId);

	log("Change Users: %d\tCurrent Users: %d", listChangeUsers.size(), listUsers.size());

	for (list<uint32_t>::iterator it = listChangeUsers.begin(); it != listChangeUsers.end(); it++)
	{
		msgNotify.add_chg_user_id_list(*it);
	}

	for (list<uint32_t>::iterator it = listUsers.begin(); it != listUsers.end(); it++)
	{
		msgNotify.add_cur_user_id_list(*it);
	}

	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn)
	{
		pRouteConn->Lock();

		CImPdu pdu;
		pdu.SetPBMsg(&msgNotify);
		pdu.SetServiceId(SID_GROUP);
		pdu.SetCommandId(CID_GROUP_CHANGE_MEMBER_NOTIFY);

		int nBytes = pRouteConn->SendPdu(&pdu);
		log("Sent %d bytes to route server", nBytes);

		pRouteConn->Unlock();
	}
	else
	{
		log("route server is unavailable!........");
	}
}


void CDataSync::SendUserDetailsChangeNotify(string& strUserId,LIST_USER_DETAILS& listUserDetails)
{
	uint32_t unUserId = GetUserIdByUserUuid(strUserId);
	log("UserId: %d, UserUUID: %s", unUserId, strUserId.c_str());

	LIST_UINT lsCompany;
	string strSql = "select companyid from IMStaff where userid=" + int2string(unUserId) + " and status =1";
	CResultSet *pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			uint32_t unCompanyId = pResultSet->GetInt("companyid");
			lsCompany.push_back(unCompanyId);
		}
		delete pResultSet;
		pResultSet = NULL;
	}


	IM::Buddy::IMUserDetailsSyncNotify msgNotify;

	msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_CHG_USER_DETAILS);
	msgNotify.set_sync_user_uuid(strUserId);
	msgNotify.set_sync_user_id(unUserId);
	
	for(IT_LIST_USER_DETAILS it = listUserDetails.begin(); it != listUserDetails.end(); it++)
	{
		IM::BaseDefine::UserDetails *pUserDetails = msgNotify.add_user_details_list();
		pUserDetails->set_details_uuid(it->m_strUUID);
		pUserDetails->set_details_title(it->m_strTitle);
		pUserDetails->set_details_type(it->m_nType);
		pUserDetails->set_details_content(it->m_strContent);
	}
	
	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn)
	{
		pRouteConn->Lock();

		for (IT_LIST_UINT it1 = lsCompany.begin(); it1 != lsCompany.end(); it1++)
		{
			//find users in a company
			IT_MAP_COMPANY_USER it = m_mapDomainUser.find(*it1);
			if (it != m_mapDomainUser.end())
			{
				for (IT_MAP_USER_KEY itUser = it->second.begin(); itUser != it->second.end(); itUser++)	//send notify to all users in the company
				{
					msgNotify.set_user_id(itUser->first);

					CImPdu pdu;
					pdu.SetPBMsg(&msgNotify);
					pdu.SetServiceId(SID_BUDDY_LIST);
					pdu.SetCommandId(CID_BUDDY_LIST_SYNC_USER_DETAILS_NOTIFY);

					int nBytes = pRouteConn->SendPdu(&pdu);
					log("Sent %d bytes to route server", nBytes);
				}
			}
		}


		//IT_MAP_COMPANY_USER it = m_mapDomainUser.find(m_nCompanyId);
		//if (it != m_mapDomainUser.end())
		//{
		//	for (IT_MAP_USER_KEY itUser = it->second.begin(); itUser != it->second.end(); itUser++)	//send notify to all users in the company
		//	{
		//		msgNotify.set_user_id(itUser->first);

		//		CImPdu pdu;
		//		pdu.SetPBMsg(&msgNotify);
		//		pdu.SetServiceId(SID_BUDDY_LIST);
		//		pdu.SetCommandId(CID_BUDDY_LIST_SYNC_USER_DETAILS_NOTIFY);
		//				
		//		int nBytes = pRouteConn->SendPdu(&pdu);
		//		log("Sent %d bytes to route server", nBytes);
		//	}
		//}

		pRouteConn->Unlock();
	}
	else
	{
		log("route server is unavailable!........");
	}
}


void CDataSync::SendRemoveDepartmentUserNotify(string& strDepartId)
{
	IM::Buddy::IMRemoveDepartUserSyncNotify msgNotify;
	msgNotify.set_depart_uuid(strDepartId);
	msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_DEL_USER_FROM_DEPT);

	//string strSql = "select a.id as uid from IMUser a, IMUserPosition b where a.useruuid=b.useruuid and b.departuuid='" + strDepartId + "'";
	string strSql = "select a.userid as uid from IMStaff a, IMUserPosition b where a.uuid=b.useruuid and b.departuuid='" + strDepartId + "' and b.status=0 group by b.useruuid";
	log("SQL for getting user id by departid: %s", strSql.c_str());

	CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		uint32_t unUserId = 0;
		while (pResultSet->Next())
		{
			unUserId = pResultSet->GetInt("uid");
			msgNotify.add_sync_user_id_list(unUserId);
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn)
	{
		pRouteConn->Lock();

		//find users in a company
		IT_MAP_COMPANY_USER it = m_mapDomainUser.find(m_nCompanyId);
		if (it != m_mapDomainUser.end())
		{
			for (IT_MAP_USER_KEY itUser = it->second.begin(); itUser != it->second.end(); itUser++)	//send notify to all users in the company
			{
				msgNotify.set_user_id(itUser->first);

				CImPdu pdu;
				pdu.SetPBMsg(&msgNotify);
				pdu.SetServiceId(SID_BUDDY_LIST);
				pdu.SetCommandId(CID_BUDDY_LIST_SYNC_USER_DEPT_NOTIFY);

				int nBytes = pRouteConn->SendPdu(&pdu);
				log("Sent %d bytes to route server", nBytes);
			}
		}

		pRouteConn->Unlock();
	}
	else
	{
		log("route server is unavailable!........");
	}
}


void CDataSync::SendRemoveDepartmentUserNotify(string& strDepartId, string& strUserId)
{
	if (GetCompanyIdByStaffUuid(strUserId) == 0)
	{
		return;
	}

	IM::Buddy::IMRemoveDepartUserSyncNotify msgNotify;
	msgNotify.set_depart_uuid(strDepartId);
	msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_DEL_USER_FROM_DEPT);

	uint32_t unUserId = GetUserIdByStaffUuid(strUserId);
	msgNotify.add_sync_user_id_list(unUserId);

	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn)
	{
		pRouteConn->Lock();

		//find users in a company
		IT_MAP_COMPANY_USER it = m_mapDomainUser.find(m_nCompanyId);
		if (it != m_mapDomainUser.end())
		{
			for (IT_MAP_USER_KEY itUser = it->second.begin(); itUser != it->second.end(); itUser++)	//send notify to all users in the company
			{
				msgNotify.set_user_id(itUser->first);

				CImPdu pdu;
				pdu.SetPBMsg(&msgNotify);
				pdu.SetServiceId(SID_BUDDY_LIST);
				pdu.SetCommandId(CID_BUDDY_LIST_SYNC_USER_DEPT_NOTIFY);

				int nBytes = pRouteConn->SendPdu(&pdu);
				log("Sent %d bytes to route server", nBytes);
			}
		}

		pRouteConn->Unlock();
	}
	else
	{
		log("route server is unavailable!........");
	}
}


void CDataSync::SendUserPositionChangeNotify(LIST_USER_DUTY& lsUserDuty)
{
	for (IT_LIST_USER_DUTY it = lsUserDuty.begin(); it != lsUserDuty.end(); it++)
	{
		IM::Buddy::IMUserDutySyncNotify msgNotify;
		uint32_t unSyncType = 0;
		if (it->m_strPositionId == DATA_SYNC_DEFAULT_POSITION_UUID)
		{
			unSyncType == DATA_SYNC_TYPE_MOD_USER_DUTY;
		}
		else
		{
			unSyncType = DATA_SYNC_TYPE_DEL_USER_DUTY;
		}

		msgNotify.set_sync_type((IM::BaseDefine::DataSyncType)unSyncType);
		msgNotify.set_sync_user_id(it->m_unUserId);
		msgNotify.set_sync_user_uuid(it->m_strUserId);
		msgNotify.set_depart_uuid(it->m_strDepartId);
		msgNotify.set_position_uuid(it->m_strPositionId);

		uint32_t unJobGradeType = IM::BaseDefine::NONE;
		if (it->m_strJobGrade == "chiefleader")
		{
			unJobGradeType = IM::BaseDefine::CHIEF_LEADER;
		}
		else if (it->m_strJobGrade == "leader")
		{
			unJobGradeType = IM::BaseDefine::LEADER;
		}
		else if (it->m_strJobGrade == "staff")
		{
			unJobGradeType = IM::BaseDefine::STAFF;
		}
		else if (it->m_strJobGrade == "other")
		{
			unJobGradeType = IM::BaseDefine::OTHER;
		}
		msgNotify.set_job_grade((IM::BaseDefine::JobGradeType)unJobGradeType);

		CRouteServConn* pRouteConn = get_route_serv_conn();
		if (pRouteConn)
		{
			pRouteConn->Lock();

			//find users in a company
			IT_MAP_COMPANY_USER it = m_mapDomainUser.find(m_nCompanyId);
			if (it != m_mapDomainUser.end())
			{
				for (IT_MAP_USER_KEY itUser = it->second.begin(); itUser != it->second.end(); itUser++)	//send notify to all users in the company
				{
					msgNotify.set_user_id(itUser->first);

					CImPdu pdu;
					pdu.SetPBMsg(&msgNotify);
					pdu.SetServiceId(SID_BUDDY_LIST);
					pdu.SetCommandId(CID_BUDDY_LIST_SYNC_USER_DUTY_NOTIFY);

					int nBytes = pRouteConn->SendPdu(&pdu);
					log("Sent %d bytes to route server", nBytes);
				}
			}

			pRouteConn->Unlock();
		}
		else
		{
			log("route server is unavailable!........");
		}
	}
}


uint32_t CDataSync::GetUserIdByUserUuid(string& strUserId)
{
	uint32_t unUserId = 0;
	string strSql = "select id from IMUser where useruuid='" + strUserId + "'";
	CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());

	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			unUserId = pResultSet->GetInt("id");
		}

		delete pResultSet;
		pResultSet = NULL;
	}
	return unUserId;
}


int CDataSync::GetStatusByProduct(string& strProduct, int nStatus)
{
	int nResult = nStatus;
	if (strProduct.find("IM") == string::npos)
	{
		nResult = DATA_SYNC_USER_NO_OPEN_IM; //change status
	}
	return nResult;
}


void CDataSync::clearUserCounter(uint32_t nUserId)
{
	list<uint32_t> listPeerId, listFromId, listGroupId;
	string strSql = "select userid,peerid,type from IMRecentSession where status=0 and (userid=" + int2string(nUserId) + " or peerid=" + int2string(nUserId) + " and type=1)";
	log("SQL for getting recent session: %s", strSql.c_str());
	
	CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			if (pResultSet->GetInt("userid") == nUserId)
			{
				if (pResultSet->GetInt("type") == IM::BaseDefine::SESSION_TYPE_SINGLE)
				{
					listPeerId.push_back(pResultSet->GetInt("peerid"));
				}
				else if (pResultSet->GetInt("type") == IM::BaseDefine::SESSION_TYPE_GROUP)
				{
					listGroupId.push_back(pResultSet->GetInt("peerid"));
				}
			}
			else if (pResultSet->GetInt("peerid") == nUserId)
			{
				listFromId.push_back(pResultSet->GetInt("userid"));
			}
		}

		delete pResultSet;
		pResultSet = NULL;
	}

	
	if (m_pCacheUnreadMsg)
	{
		// Clear P2P msg Counter ( user id to peer id
		for (list<uint32_t>::iterator it = listPeerId.begin(); it != listPeerId.end(); it++)
		{
			int nRet = m_pCacheUnreadMsg->hdel("unread_" + int2string(nUserId), int2string(*it));
			log("clear unread message count for user %d ---->%d", *it, nUserId);
			if (!nRet)
			{
				log("hdel failed %d->%d", *it, nUserId);
			}
		}

		// Clear P2P msg Counter(peer id to user id
		for (list<uint32_t>::iterator it = listFromId.begin(); it != listFromId.end(); it++)
		{
			int nRet = m_pCacheUnreadMsg->hdel("unread_" + int2string(*it), int2string(nUserId));
			log("clear unread message count for user %d ---->%d",nUserId, *it);
			if (!nRet)
			{
				log("hdel failed %d->%d", nUserId, *it);
			}
		}

		// Clear Group msg Counter	
		for (list<uint32_t>::iterator it = listGroupId.begin(); it != listGroupId.end(); ++it)
		{
			string strGroupKey = int2string(*it) + GROUP_TOTAL_MSG_COUNTER_REDIS_KEY_SUFFIX;
			map<string, string> mapGroupCount;
			bool bRet = m_pCacheUnreadMsg->hgetAll(strGroupKey, mapGroupCount);

			for (auto it = mapGroupCount.begin(); it != mapGroupCount.end(); ++it)
			{
				log("CLEARKEY: %s, FIRST: %s, SECOND: %s", strGroupKey.c_str(), it->first.c_str(), it->second.c_str());
			}

			if (bRet)
			{
				string strUserKey = int2string(nUserId) + "_" + int2string(*it) + GROUP_USER_MSG_COUNTER_REDIS_KEY_SUFFIX;
				string strReply = m_pCacheUnreadMsg->hmset(strUserKey, mapGroupCount);
				log("clear unread messge count for user %d in group %d", nUserId, *it);
				if (strReply.empty())
				{
					log("hmset %s failed !", strUserKey.c_str());
				}
				//reset lastatmsgid (user----peer(group id) frequently?
				string strSql = "update IMRecentSession set lastatmsgid = NULL,updated=unix_timestamp() where userid = " + int2string(nUserId) + " and peerid= " + int2string(*it) + " and type=2";
				log("SQL for updating session last at msgid: %s", strSql.c_str());
				m_pDBConn->ExecuteUpdate(strSql.c_str());
			}
			else
			{
				log("hgetall %s failed!", strGroupKey.c_str());
			}
		}
		//pCacheManager->RelCacheConn(pCacheConn);
	}
	else
	{
		log("no cache connection for unread");
	}
}


void CDataSync::SendRemoveSessionNotify(uint32_t unUserId)
{
	LIST_SESSION_INFO lsSessionInfo;
	string strSql = "select id,userid,type from IMRecentSession where (userid=" + int2string(unUserId) + " or (peerid = " + int2string(unUserId) + " and type=1)) and status = 0";
	log("SQL for getting session id: %s", strSql.c_str());
	CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			SESSION_INFO SessionInfo;
			SessionInfo.m_unUserId = pResultSet->GetInt("userid");
			SessionInfo.m_unSessionType = pResultSet->GetInt("type");
			SessionInfo.m_unSessionId = pResultSet->GetInt("id");
			lsSessionInfo.push_back(SessionInfo);
		}

		delete pResultSet;
		pResultSet = NULL;
	}

	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn)
	{
		IM::Buddy::IMRemoveSessionNotify msgNotify;
	
		pRouteConn->Lock();

		for (IT_LIST_SESSION_INFO it = lsSessionInfo.begin(); it != lsSessionInfo.end(); it++)
		{
			msgNotify.set_user_id(it->m_unUserId);
			msgNotify.set_session_type((IM::BaseDefine::SessionType)it->m_unSessionType);
			msgNotify.set_session_id(it->m_unSessionId);

			CImPdu pdu;
			pdu.SetPBMsg(&msgNotify);
			pdu.SetServiceId(SID_BUDDY_LIST);
			pdu.SetCommandId(CID_BUDDY_LIST_REMOVE_SESSION_NOTIFY);

			int nBytes = pRouteConn->SendPdu(&pdu);
			log("Sent %d bytes to route server", nBytes);
		}

		pRouteConn->Unlock();
	}
	else
	{
		log("route server is unavailable!........");
	}
}


void CDataSync::RemoveSession(uint32_t unUserId)
{
	//delete session
	string strSql = "update IMRecentSession set status=1,updated=unix_timestamp() where (userid=" + int2string(unUserId) + " or peerid=" + int2string(unUserId) + " and type=1) and status = 0";
	log("SQL for deleting session for user: %d", unUserId);
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::DeleteUser(uint32_t unUserId)
{
	//clear unread messages first
	clearUserCounter(unUserId);

	SendRemoveSessionNotify(unUserId);

	RemoveSession(unUserId);
}


//int CDataSync::GetDomainPassword(const string& strInputString, DOMAIN_PASSWORD& DomainPassword)
//{
//	Json::Reader Reader;
//	Json::Value Root;
//
//	int nResult = 1;
//
//	try
//	{
//		if (Reader.parse(strInputString, Root))
//		{
//			/*if (!Root["resultCode"].isNull() && Root["resultCode"].isString())
//			{
//			string strResult = Root["resultCode"].asString();
//			if (strResult != "0000")
//			{
//			return -1;
//			}
//			}
//			else
//			{
//			return -1;
//			}*/
//
//			if (!Root["primaryOrgId"].isNull() && Root["primaryOrgId"].isString())
//			{
//				DomainPassword.m_strDomainId = Root["primaryOrgId"].asString();
//			}
//
//			if (!Root["validityStatus"].isNull() && Root["validityStatus"].isInt())
//			{
//				DomainPassword.m_unValidityStatus = Root["validityStatus"].asInt();
//			}
//
//			if (!Root["validity"].isNull() && Root["validity"].isInt())
//			{
//				DomainPassword.m_unValidity = Root["validity"].asInt();
//			}
//
//			if (!Root["isForcePassword"].isNull() && Root["isForcePassword"].isInt())
//			{
//				DomainPassword.m_unIsForcePassword = Root["isForcePassword"].asInt();
//			}
//		}
//		else
//		{
//			log("parse domainpassword data error");
//			nResult = -1;
//		}
//	}
//	catch (std::runtime_error msg)
//	{
//		log("an exception occurred when parsing domain password");
//		nResult = -1;
//	}
//
//	return nResult;
//}


void CDataSync::UpdateCompanyPassword(DOMAIN_PASSWORD& DomainPassword)
{
	string strSql = "update IMCompany set validitystatus=" + int2string(DomainPassword.m_unValidityStatus) + ",validity=" + int2string(DomainPassword.m_unValidity) + ",isforcepassword=" + int2string(DomainPassword.m_unIsForcePassword);
	strSql += ",updated=unix_timestamp(), passwordstrength=" + int2string(DomainPassword.m_unPasswordStrength) + ",passwordlength=" + int2string(DomainPassword.m_unPasswordLength) + " where companyuuid='" + DomainPassword.m_strDomainId + "'";

	log("SQL for updating company password rule; %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::DeleteStaffPosition(string& strDepartID, string& strUserID)
{
	if (GetCompanyIdByStaffUuid(strUserID) > 0)
	{

		string strSql = "update IMUserPosition set status = -1, updated=unix_timestamp() where departuuid='" + strDepartID + "' and posuuid <>'" + DATA_SYNC_DEFAULT_POSITION_UUID + "' and useruuid='" + strUserID + "'";
		m_pDBConn->ExecuteUpdate(strSql.c_str());
		log("SQL for deleting a record for IMUserPosition: %s", strSql.c_str());

		//get department status
		strSql = "select status from IMUserPosition where departuuid='" + strDepartID + "'" + " and posuuid ='" + DATA_SYNC_DEFAULT_POSITION_UUID + "' and useruuid='" + strUserID + "'";
		log("SQL for getting userpositon status: %s", strSql.c_str());
		CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
		int nStatus = -1;
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				nStatus = pResultSet->GetInt("status");
			}
			delete pResultSet;
			pResultSet = NULL;
		}

		uint32_t unUserId = GetUserIdByStaffUuid(strUserID); //get user id

		if (nStatus == 0) //department has already exists
		{
			string strPositionId = DATA_SYNC_DEFAULT_POSITION_UUID;
			string strJobGrade = "";
			SendUserPositionChangeNotify(strUserID, unUserId, strDepartID, strPositionId, IM::BaseDefine::DATA_SYNC_TYPE_MOD_USER_DUTY, strJobGrade);
		}

		UpdateStaffChangeTime(strUserID);
	}
}


void CDataSync::SendStaffAddNotify(uint32_t unUserId, STAFF_INFO& UserInfo)
{
	IM::Buddy::IMStaffSyncNotify msgNotify;

	msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_ADD_STAFF);
	msgNotify.set_sync_user_id(unUserId);
	msgNotify.set_sync_user_uuid(UserInfo.m_strUserUuid);
	msgNotify.set_sync_staff_uuid(UserInfo.m_strStaffUuid);
	msgNotify.set_company_id(m_nCompanyId);
	msgNotify.set_job_number(UserInfo.m_strJobNumber);
	msgNotify.set_staff_name(UserInfo.m_strStaffName);
	msgNotify.set_mobile_phone(UserInfo.m_strMobilePhone);
	msgNotify.set_keyword(UserInfo.m_strKeyword);
	uint32_t unBuyProduct = GetProduct(UserInfo.m_strBuyProduct);
	msgNotify.set_order_product(unBuyProduct);
	msgNotify.set_is_admin(UserInfo.m_unIsAdmin);
	msgNotify.set_admin_status(UserInfo.m_unAdminStatus);
	msgNotify.set_is_super_admin(UserInfo.m_unIsSuperAdmin);
	msgNotify.set_is_actived(UserInfo.m_unIsActived);
	msgNotify.set_status(UserInfo.m_nStatus);
	msgNotify.set_contact_status(UserInfo.m_unContactStatus);
	msgNotify.set_account(UserInfo.m_strAccount);
	msgNotify.set_user_name(UserInfo.m_strUserName);
	msgNotify.set_email(UserInfo.m_strEmail);
	msgNotify.set_default_email(UserInfo.m_strDefaultEmail);

	IT_MAP_COMPANY_USER it = m_mapDomainUser.find(m_nCompanyId);
	if (it == m_mapDomainUser.end())
	{
		MAP_USER_KEY mapUserKey;
		mapUserKey[unUserId] = UserInfo.m_strStaffUuid;
		m_mapDomainUser.insert(make_pair(m_nCompanyId, mapUserKey));
		log("USERS %d in the company: %d, elements: %d", mapUserKey.size(), m_nCompanyId, m_mapDomainUser.size());
		it = m_mapDomainUser.find(m_nCompanyId);
	}
	else
	{
		log("%d users before adding a user: %d", it->second.size(), unUserId);
		it->second.insert(make_pair(unUserId, UserInfo.m_strStaffUuid)); // add a new staff for the company
		log("%d users after adding a user: %d", it->second.size(), unUserId);
	}

	DOMAIN_INFO NewCompanyInfo;
	GetCompanyInfoByCompanyId(m_nCompanyId, NewCompanyInfo);
	SendCompanyAddNotify(unUserId, m_nCompanyId, NewCompanyInfo);

	USER_INFO NewUserInfo;
	uint32_t unNewUserId = GetUserInfoByUserId(UserInfo.m_strUserUuid, NewUserInfo);
	SendUserAddNotify(unNewUserId, NewUserInfo);


	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn)
	{
		pRouteConn->Lock();

		for (IT_MAP_USER_KEY itUser = it->second.begin(); itUser != it->second.end(); itUser++)	//send notify to all users in the company
		{
			log("Sending notify to  %d", itUser->first);
			msgNotify.set_user_id(itUser->first);
			CImPdu pdu;
			pdu.SetPBMsg(&msgNotify);
			pdu.SetServiceId(SID_BUDDY_LIST);
			pdu.SetCommandId(CID_BUDDY_LIST_SYNC_STAFF_NOTIFY);

			int nBytes = pRouteConn->SendPdu(&pdu);
			log("Sent %d bytes to route server", nBytes);
		}
		
	

		//if (it != m_mapDomainUser.end())
		//{
		//	for (IT_MAP_USER_KEY itUser = it->second.begin(); itUser != it->second.end(); itUser++)	//send notify to all users in the company
		//	{
		//		log("Sending notify to  %d", itUser->first);
		//		msgNotify.set_user_id(itUser->first);
		//		CImPdu pdu;
		//		pdu.SetPBMsg(&msgNotify);
		//		pdu.SetServiceId(SID_BUDDY_LIST);
		//		pdu.SetCommandId(CID_BUDDY_LIST_SYNC_STAFF_NOTIFY);

		//		int nBytes = pRouteConn->SendPdu(&pdu);
		//		log("Sent %d bytes to route server", nBytes);
		//	}
		//	log("%d users before adding a user: %d", it->second.size(), unUserId);
		//	it->second.insert(make_pair(unUserId, UserInfo.m_strStaffUuid)); // add a new staff for the company
		//	log("%d users after adding a user: %d", it->second.size(), unUserId);
		//}
		//else
		//{
		//	MAP_USER_KEY mapUserKey;

		//	mapUserKey[unUserId] = UserInfo.m_strStaffUuid;
		//	m_mapDomainUser.insert(make_pair(m_nCompanyId, mapUserKey));
		//	log("USERS %d in the company: %d, elements: %d", mapUserKey.size(), m_nCompanyId, m_mapDomainUser.size());
		//}

		pRouteConn->Unlock();
	}
	else
	{
		log("route server is unavailable!");
	}

}


void CDataSync::SendStaffUpdateNotify(uint32_t unUserId, int nStatus, STAFF_INFO& UserInfo)
{
	IM::Buddy::IMStaffSyncNotify msgNotify;

	msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_MOD_STAFF);
	msgNotify.set_sync_user_id(unUserId);
	msgNotify.set_sync_user_uuid(UserInfo.m_strUserUuid);
	msgNotify.set_sync_staff_uuid(UserInfo.m_strStaffUuid);
	msgNotify.set_company_id(m_nCompanyId);
	msgNotify.set_job_number(UserInfo.m_strJobNumber);
	msgNotify.set_staff_name(UserInfo.m_strStaffName);
	msgNotify.set_mobile_phone(UserInfo.m_strMobilePhone);
	msgNotify.set_keyword(UserInfo.m_strKeyword);
	uint32_t unBuyProduct = GetProduct(UserInfo.m_strBuyProduct);
	msgNotify.set_order_product(unBuyProduct);
	msgNotify.set_is_admin(UserInfo.m_unIsAdmin);
	msgNotify.set_admin_status(UserInfo.m_unAdminStatus);
	msgNotify.set_is_super_admin(UserInfo.m_unIsSuperAdmin);
	msgNotify.set_is_actived(UserInfo.m_unIsActived);
	msgNotify.set_status(UserInfo.m_nStatus);
	msgNotify.set_contact_status(UserInfo.m_unContactStatus);
	msgNotify.set_account(UserInfo.m_strAccount);
	msgNotify.set_user_name(UserInfo.m_strUserName);
	msgNotify.set_email(UserInfo.m_strEmail);
	msgNotify.set_default_email(UserInfo.m_strDefaultEmail);


	if (nStatus == 1 && UserInfo.m_nStatus == 0)
	{
		msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_DISABLE_STAFF);
	}
	else if (nStatus == 1 && UserInfo.m_nStatus == -1)
	{
		msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_DEL_STAFF);
	}
	else if (nStatus == 0 && UserInfo.m_nStatus == 1)
	{
		msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_ENABLE_STAFF);
	}
	else if (nStatus == 0 && UserInfo.m_nStatus == -1)
	{
		msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_DEL_STAFF);
	}
	else if (nStatus == -1 && UserInfo.m_nStatus == 1)
	{
		msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_ENABLE_STAFF);
	}
	else if (nStatus == -1 && UserInfo.m_nStatus == 0)
	{
		msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_DISABLE_STAFF);
	}
	else if (nStatus == UserInfo.m_nStatus)
	{
		msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_MOD_STAFF);
	}

	IT_MAP_COMPANY_USER it = m_mapDomainUser.find(m_nCompanyId);
	if (it != m_mapDomainUser.end())
	{
		CRouteServConn* pRouteConn = get_route_serv_conn();
		if (pRouteConn)
		{
			pRouteConn->Lock();

			for (IT_MAP_USER_KEY itUser = it->second.begin(); itUser != it->second.end(); itUser++)	//send notify to all users in the company
			{
				log("Sending notify to  %d", itUser->first);
				msgNotify.set_user_id(itUser->first);

				CImPdu pdu;
				pdu.SetPBMsg(&msgNotify);
				pdu.SetServiceId(SID_BUDDY_LIST);
				pdu.SetCommandId(CID_BUDDY_LIST_SYNC_STAFF_NOTIFY);

				int nBytes = pRouteConn->SendPdu(&pdu);
				log("Sent %d bytes to route server", nBytes);
			}

			pRouteConn->Unlock();
		}
		else
		{
			log("route server is unavailable!");
		}

		if (UserInfo.m_nStatus == -1)
		{
			it->second.erase(unUserId);  //delete the staff
		}
		else if (nStatus == -1 && UserInfo.m_nStatus != -1)
		{
			it->second.insert(make_pair(unUserId, UserInfo.m_strStaffUuid)); // add a new user for the company
		}
	}


	//CRouteServConn* pRouteConn = get_route_serv_conn();
	//if (pRouteConn)
	//{
	//	pRouteConn->Lock();

	//	IT_MAP_COMPANY_USER it = m_mapDomainUser.find(m_nCompanyId);
	//	if (it != m_mapDomainUser.end())
	//	{
	//		for (IT_MAP_USER_KEY itUser = it->second.begin(); itUser != it->second.end(); itUser++)	//send notify to all users in the company
	//		{
	//			log("Sending notify to  %d", itUser->first);
	//			msgNotify.set_user_id(itUser->first);
	//			CImPdu pdu;
	//			pdu.SetPBMsg(&msgNotify);
	//			pdu.SetServiceId(SID_BUDDY_LIST);
	//			pdu.SetCommandId(CID_BUDDY_LIST_SYNC_STAFF_NOTIFY);

	//			int nBytes = pRouteConn->SendPdu(&pdu);
	//			log("Sent %d bytes to route server", nBytes);
	//		}
	//		log("%d users before adding a user: %d", it->second.size(), unUserId);
	//		it->second.insert(make_pair(unUserId, UserInfo.m_strUserID)); // add a new user for the company
	//		log("%d users after adding a user: %d", it->second.size(), unUserId);
	//	}

	//	pRouteConn->Unlock();
	//}
	//else
	//{
	//	log("route server is unavailable!");
	//}
}


void CDataSync::SendStaffDelNotify(uint32_t unUserId, string& strUserId, string& strStaffUuid)
{
	IM::Buddy::IMStaffSyncNotify msgNotify;

	msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_DEL_STAFF);
	msgNotify.set_sync_user_id(unUserId);
	msgNotify.set_sync_user_uuid(strUserId);
	msgNotify.set_sync_staff_uuid(strStaffUuid);
	msgNotify.set_company_id(m_nCompanyId);
	msgNotify.set_status(-1);

	IT_MAP_COMPANY_USER it = m_mapDomainUser.find(m_nCompanyId);
	if (it != m_mapDomainUser.end())
	{
		log("%d users before deleting %d", it->second.size(), unUserId);

		CRouteServConn* pRouteConn = get_route_serv_conn();
		if (pRouteConn)
		{
			pRouteConn->Lock();

			for (IT_MAP_USER_KEY itUser = it->second.begin(); itUser != it->second.end(); itUser++)	//send notify to all users in the company
			{
				log("Sending notify to  %d", itUser->first);
				msgNotify.set_user_id(itUser->first);

				CImPdu pdu;
				pdu.SetPBMsg(&msgNotify);
				pdu.SetServiceId(SID_BUDDY_LIST);
				pdu.SetCommandId(CID_BUDDY_LIST_SYNC_STAFF_NOTIFY);

				int nBytes = pRouteConn->SendPdu(&pdu);
				log("Sent %d bytes to route server", nBytes);
			}

			pRouteConn->Unlock();
		}
		else
		{
			log("route server is unavailable!");
		}

		it->second.erase(unUserId);  //delete the user
		log("%d users after deleting %d", it->second.size(), unUserId);
	}
}


void CDataSync::SendStaffDetailsChangeNotify(string& strStaffUuid,LIST_USER_DETAILS& listUserDetails)
{
	if (GetCompanyIdByStaffUuid(strStaffUuid) == 0)
	{
		return;
	}
	
	uint32_t unUserId = GetUserIdByStaffUuid(strStaffUuid);
	log("UserId: %d, UserUUID: %s", unUserId, strStaffUuid.c_str());

	IM::Buddy::IMUserDetailsSyncNotify msgNotify;

	msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_CHG_STAFF_DETAILS);
	msgNotify.set_sync_user_uuid(strStaffUuid);
	msgNotify.set_sync_user_id(unUserId);

	for (IT_LIST_USER_DETAILS it = listUserDetails.begin(); it != listUserDetails.end(); it++)
	{
		IM::BaseDefine::UserDetails *pUserDetails = msgNotify.add_user_details_list();
		pUserDetails->set_details_uuid(it->m_strUUID);
		pUserDetails->set_details_title(it->m_strTitle);
		pUserDetails->set_details_type(it->m_nType);
		pUserDetails->set_details_content(it->m_strContent);
	}

	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn)
	{
		pRouteConn->Lock();

		//find users in a company
		IT_MAP_COMPANY_USER it = m_mapDomainUser.find(m_nCompanyId);
		if (it != m_mapDomainUser.end())
		{
			for (IT_MAP_USER_KEY itUser = it->second.begin(); itUser != it->second.end(); itUser++)	//send notify to all users in the company
			{
				msgNotify.set_user_id(itUser->first);

				CImPdu pdu;
				pdu.SetPBMsg(&msgNotify);
				pdu.SetServiceId(SID_BUDDY_LIST);
				pdu.SetCommandId(CID_BUDDY_LIST_SYNC_STAFF_DETAILS_NOTIFY);

				int nBytes = pRouteConn->SendPdu(&pdu);
				log("Sent %d bytes to route server", nBytes);
			}
		}

		pRouteConn->Unlock();
	}
	else
	{
		log("route server is unavailable!........");
	}
}


void CDataSync::AddStaffDetails(LIST_USER_DETAILS& listUserDetails)
{
	string strSql = "insert into StaffDetails (uuid,staffuuid,type,title,content,created,updated) values(?,?,?,?,?,?,?)";

	string strUserId;
	for (IT_LIST_USER_DETAILS it = listUserDetails.begin(); it != listUserDetails.end(); it++)
	{
		USER_DETAILS UserDetails = *it;
		strUserId = it->m_strUserUUID;

		CPrepareStatement* pStmt = new CPrepareStatement();
		if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
		{
			int nIndex = 0;
			unsigned int nCurrenTime = time(NULL);
			pStmt->SetParam(nIndex++, UserDetails.m_strUUID);
			pStmt->SetParam(nIndex++, UserDetails.m_strUserUUID);
			pStmt->SetParam(nIndex++, UserDetails.m_nType);
			pStmt->SetParam(nIndex++, UserDetails.m_strTitle);
			pStmt->SetParam(nIndex++, UserDetails.m_strContent);
			pStmt->SetParam(nIndex++, nCurrenTime);
			pStmt->SetParam(nIndex++, nCurrenTime);
			pStmt->ExecuteUpdate();
		}

		delete pStmt;
		pStmt = NULL;
	}

	SendStaffDetailsChangeNotify(strUserId,listUserDetails);
}


void CDataSync::SetStaffDetails(string& strStaffUuid, LIST_USER_DETAILS& listUserDetails)
{
	//delete all additional information first
	string strSql = "delete from StaffDetails where staffuuid='" + strStaffUuid + "'";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "insert into StaffDetails (uuid,staffuuid,type,title,content,created,updated) values(?,?,?,?,?,?,?)";
	// 必须在释放连接前delete CPrepareStatement对象，否则有可能多个线程操作mysql对象，会crash
	CPrepareStatement* pStmt = new CPrepareStatement();
	if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
	{
		for (IT_LIST_USER_DETAILS it = listUserDetails.begin(); it != listUserDetails.end(); it++)
		{
			int nIndex = 0;
			unsigned int nCurrenTime = time(NULL);
			USER_DETAILS UserDetails = *it;
			pStmt->SetParam(nIndex++, UserDetails.m_strUUID);
			pStmt->SetParam(nIndex++, UserDetails.m_strUserUUID);
			pStmt->SetParam(nIndex++, UserDetails.m_nType);
			pStmt->SetParam(nIndex++, UserDetails.m_strTitle);
			pStmt->SetParam(nIndex++, UserDetails.m_strContent);
			pStmt->SetParam(nIndex++, nCurrenTime);
			pStmt->SetParam(nIndex++, nCurrenTime);
			pStmt->ExecuteUpdate();
		}
	}

	delete pStmt;
	log("ready to send notify for change staff details");
	SendStaffDetailsChangeNotify(strStaffUuid, listUserDetails);

	UpdateStaffChangeTime(strStaffUuid);

	//if (listUserDetails.size() > 0)
	//{
	//	string strSql = "truncate table StaffDetailsTemp";
	//	m_pDBConn->ExecuteUpdate(strSql.c_str());

	//	strSql = "insert into StaffDetailsTemp (uuid,staffuuid,type,title,content,created,updated) values(?,?,?,?,?,?,?)";
	//	// 必须在释放连接前delete CPrepareStatement对象，否则有可能多个线程操作mysql对象，会crash
	//	string strUserId = "";
	//	for (IT_LIST_USER_DETAILS it = listUserDetails.begin(); it != listUserDetails.end(); it++)
	//	{
	//		CPrepareStatement* pStmt = new CPrepareStatement();
	//		if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
	//		{
	//			int nIndex = 0;
	//			unsigned int nCurrenTime = time(NULL);
	//			USER_DETAILS UserDetails = *it;
	//			strUserId = UserDetails.m_strUserUUID;
	//			pStmt->SetParam(nIndex++, UserDetails.m_strUUID);
	//			pStmt->SetParam(nIndex++, UserDetails.m_strUserUUID);
	//			pStmt->SetParam(nIndex++, UserDetails.m_nType);
	//			pStmt->SetParam(nIndex++, UserDetails.m_strTitle);
	//			pStmt->SetParam(nIndex++, UserDetails.m_strContent);
	//			pStmt->SetParam(nIndex++, nCurrenTime);
	//			pStmt->SetParam(nIndex++, nCurrenTime);
	//			pStmt->ExecuteUpdate();
	//		}

	//		delete pStmt;
	//		pStmt = NULL;
	//	}

	//	//strSql = "delete from StaffDetailsTemp using StaffDetailsTemp,(select max() as maxid, uuid from StaffDetailsTemp group by uuid having count(*) > 1) as b where StaffDetailsTemp.uuid=b.uuid and StaffDetailsTemp.id <> b.maxid";
	//	//m_pDBConn->ExecuteUpdate(strSql.c_str());

	//	strSql = "update StaffDetails a, StaffDetailsTemp b set a.type=b.type,a.title=b.title,a.content=b.content,a.updated=unix_timestamp() where (a.uuid=b.uuid) and (a.type <> b.type or a.title <> b.title or a.content <> b.content)";
	//	m_pDBConn->ExecuteUpdate(strSql.c_str());

	//	strSql = "insert into StaffDetails (uuid,staffuuid,type,title,content,created,updated) select uuid,staffuuid,type,title,content,created,updated from StaffDetailsTemp a where not exists(select 1 from StaffDetails b where a.uuid=b.uuid);";
	//	m_pDBConn->ExecuteUpdate(strSql.c_str());

	//	strSql = "delete from StaffDetails where staffuuid='" + strUserId + "' and not exists(select 1 from StaffDetailsTemp where StaffDetailsTemp.uuid = StaffDetails.uuid)";
	//	m_pDBConn->ExecuteUpdate(strSql.c_str());

	//	strSql = "update IMStaff set updated=unix_timestamp() where uuid='" + strUserId + "'";  //change update time for table 'IMStaff'
	//	m_pDBConn->ExecuteUpdate(strSql.c_str());

	//	log("ready to send notify for change user details");
	//	SendStaffDetailsChangeNotify(listUserDetails);
	//}
}


void CDataSync::UpdateStaffDetails(LIST_USER_DETAILS& listUserDetails)
{
	for (IT_LIST_USER_DETAILS it = listUserDetails.begin(); it != listUserDetails.end(); it++)
	{
		USER_DETAILS UserDetails = *it;
		string strSql = "update StaffDetails set type = " + int2string(UserDetails.m_nType) + ",title=' " + UserDetails.m_strTitle + "',content='" + UserDetails.m_strContent + "'updated=unix_timestamp() where uuid='" + UserDetails.m_strUUID + "'";
		log("SQL for updating a user details: %s", strSql.c_str());
		m_pDBConn->ExecuteUpdate(strSql.c_str());
	}

	//SendUserDetailsChangeNotify(listUserDetails);
}


int  CDataSync::GetStaffInfo(string& strInputString, STAFF_INFO& StaffInfo)
{
	Json::Reader reader;
	Json::Value Root;

	if (reader.parse(strInputString, Root))
	{
		if (!Root["staffId"].isNull() && Root["staffId"].isString())
		{
			StaffInfo.m_strStaffUuid = Root["staffId"].asString();
		}

		if (!Root["userId"].isNull() && Root["userId"].isString())
		{
			StaffInfo.m_strUserUuid = Root["userId"].asString();
		}


		if (!Root["primaryOrgId"].isNull() && Root["primaryOrgId"].isString())
		{
			StaffInfo.m_strCompanyUuid = Root["primaryOrgId"].asString();
		}

		if (!Root["staffCode"].isNull() && Root["staffCode"].isString())
		{
			StaffInfo.m_strJobNumber = Root["staffCode"].asString();
		}
		else
		{
			StaffInfo.m_strJobNumber = "";
		}

		if (!Root["fullName"].isNull() && Root["fullName"].isString())
		{
			StaffInfo.m_strStaffName = Root["fullName"].asString();
		}
		else
		{
			StaffInfo.m_strStaffName = "";
		}


		if (!Root["isAdmin"].isNull() && Root["isAdmin"].isInt())
		{
			StaffInfo.m_unIsAdmin = Root["isAdmin"].asInt();
		}
		else
		{
			StaffInfo.m_unIsAdmin = 0;
		}

		if (!Root["isSuperAdmin"].isNull() && Root["isSuperAdmin"].isInt())
		{
			StaffInfo.m_unIsSuperAdmin = Root["isSuperAdmin"].asInt();
		}
		else
		{
			StaffInfo.m_unIsSuperAdmin = 0;
		}

		if (!Root["isActivate"].isNull() && Root["isActivate"].isInt())
		{
			StaffInfo.m_unIsActived = Root["isActivate"].asInt();
		}
		else
		{
			StaffInfo.m_unIsActived = 0;
		}

		if (!Root["phone"].isNull() && Root["phone"].isString())
		{
			StaffInfo.m_strMobilePhone = Root["phone"].asString();
		}
		else
		{
			StaffInfo.m_strMobilePhone = "";
		}

		if (!Root["remark"].isNull() && Root["remark"].isString())
		{
			StaffInfo.m_strRemark = Root["remark"].asString();
		}
		else
		{
			StaffInfo.m_strRemark = "";
		}

		if (!Root["adminStatus"].isNull() && Root["adminStatus"].isInt())
		{
			StaffInfo.m_unAdminStatus = Root["adminStatus"].asInt();
		}
		else
		{
			StaffInfo.m_unAdminStatus = 0;
		}

		if (!Root["contactsStatus"].isNull() && Root["contactsStatus"].isInt())
		{
			StaffInfo.m_unContactStatus = Root["contactsStatus"].asInt();
		}
		else
		{
			StaffInfo.m_unContactStatus = 1;
		}

		if (!Root["status"].isNull() && Root["status"].isInt())
		{
			StaffInfo.m_nStatus = Root["status"].asInt();
		}
		else
		{
			StaffInfo.m_nStatus = 1;
		}


		if (!Root["keyword"].isNull() && Root["keyword"].isString())
		{
			StaffInfo.m_strKeyword = Root["keyword"].asString();
		}
		else
		{
			StaffInfo.m_strKeyword = "";
		}

		if (!Root["account"].isNull() && Root["account"].isString())
		{
			StaffInfo.m_strAccount = Root["account"].asString();
		}
		else
		{
			StaffInfo.m_strAccount = "";
		}

		if (!Root["username"].isNull() && Root["username"].isString())
		{
			StaffInfo.m_strUserName = Root["username"].asString();
		}
		else
		{
			StaffInfo.m_strUserName = "";
		}

		if (!Root["email"].isNull() && Root["email"].isString())
		{
			StaffInfo.m_strEmail = Root["email"].asString();
		}
		else
		{
			StaffInfo.m_strEmail = "";
		}

		if (!Root["defaultEmail"].isNull() && Root["defaultEmail"].isString())
		{
			StaffInfo.m_strDefaultEmail = Root["defaultEmail"].asString();
		}
		else
		{
			StaffInfo.m_strDefaultEmail = "";
		}

		//if (!Root["productIds"] && Root["productIds"].isArray())
		//{
		//	int nProducts = Root["productIds"].size();
		//	if (nProducts)
		//	{
		//		Json::Value Products = Root["productIds"];
		//		int i = 0;

		//		//get products a user buys
		//		string strBuyProducts = "";
		//		for (; i < nProducts - 1; i++)
		//		{
		//			strBuyProducts += Products[i].asString() + ',';
		//		}
		//		strBuyProducts += Products[i].asString();
		//		StaffInfo.m_strBuyProduct = strBuyProducts;
		//	}
		//}
		//else
		//{
		//	StaffInfo.m_strBuyProduct = "";
		//}

		return 1;
	}
	else
	{
		return -1;
	}
}


void CDataSync::AddCompanyStaff(STAFF_INFO& StaffInfo)
{
	if(GetCompanyIdByCompanyUuid(StaffInfo.m_strCompanyUuid) == 0)
	{
		log("company does not exists: %s", StaffInfo.m_strCompanyUuid.c_str());
		return;
	}

	//get userid from IMStaff by staff uuid
	string strSql = "select userid from IMStaff where uuid='" + StaffInfo.m_strStaffUuid + "'"; 
	log("SQL for getting user id: %s", strSql.c_str());
	CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());

	uint32_t unUserId = 0;
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			unUserId = pResultSet->GetInt("userid");
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	if (unUserId == 0)
	{
		//get user id from IMUser by user uuid
		strSql = "select id from IMUser where useruuid='" + StaffInfo.m_strUserUuid + "'"; 
		log("SQL for getting user id: %s", strSql.c_str());
		CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());

		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				unUserId = pResultSet->GetInt("id");
			}
			delete pResultSet;
			pResultSet = NULL;
		}

		if (unUserId != 0) //add a staff
		{
			strSql = "INSERT INTO IMStaff (uuid,useruuid,userid,jobnumber,mobilephone,staffname,isadmin,adminstatus,issuperadmin,isactived,contactstatus,status,";
			strSql += "companyid,keyword,remark,buyproduct,created,updated,username,account,email,defaultemail) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"; 

			//bool bSuccess = false;
			CPrepareStatement* pStmt = new CPrepareStatement();
			if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
			{
				int nIndex = 0;
				unsigned int unCurrenTime = time(NULL);
				pStmt->SetParam(nIndex++, StaffInfo.m_strStaffUuid);
				pStmt->SetParam(nIndex++, StaffInfo.m_strUserUuid);
				pStmt->SetParam(nIndex++, unUserId);
				pStmt->SetParam(nIndex++, StaffInfo.m_strJobNumber);
				pStmt->SetParam(nIndex++, StaffInfo.m_strMobilePhone);
				pStmt->SetParam(nIndex++, StaffInfo.m_strStaffName);
				pStmt->SetParam(nIndex++, StaffInfo.m_unIsAdmin);
				pStmt->SetParam(nIndex++, StaffInfo.m_unAdminStatus);
				pStmt->SetParam(nIndex++, StaffInfo.m_unIsSuperAdmin);
				pStmt->SetParam(nIndex++, StaffInfo.m_unIsActived);
				pStmt->SetParam(nIndex++, StaffInfo.m_unContactStatus);
				pStmt->SetParam(nIndex++, StaffInfo.m_nStatus);
				pStmt->SetParam(nIndex++, m_nCompanyId);
				pStmt->SetParam(nIndex++, StaffInfo.m_strKeyword);
				pStmt->SetParam(nIndex++, StaffInfo.m_strRemark);
				pStmt->SetParam(nIndex++, StaffInfo.m_strBuyProduct);

				pStmt->SetParam(nIndex++, unCurrenTime);
				pStmt->SetParam(nIndex++, unCurrenTime);

				pStmt->SetParam(nIndex++, StaffInfo.m_strUserName);
				pStmt->SetParam(nIndex++, StaffInfo.m_strAccount);
				pStmt->SetParam(nIndex++, StaffInfo.m_strEmail);
				pStmt->SetParam(nIndex++, StaffInfo.m_strDefaultEmail);

				pStmt->ExecuteUpdate();
				
				//int nStatus = StaffInfo.m_nStatus;
				//comment by george on June 1st,2016
				//nStatus = GetStatusByProduct(UserInfo.m_strBuyProducts, nStatus); //get status by products
			}

			delete pStmt;
			pStmt = NULL;

			//if (!unSyncStatus)  //send notify to clients to add a new user 
			//{
			//	SendUserAddNotify(unUserId, UserInfo);
			//}

			SendStaffAddNotify(unUserId, StaffInfo);
		}
		else
		{
			log("User %s is not exists", StaffInfo.m_strUserUuid.c_str());
		}
		
	}
	else
	{
		SendStaffAddNotify(unUserId, StaffInfo);
		log("Staff %s is already exists,changing it", StaffInfo.m_strStaffUuid.c_str());
	}
}


void CDataSync::UpdateCompanyStaff(STAFF_INFO& StaffInfo)
{

	if (GetCompanyIdByCompanyUuid(StaffInfo.m_strCompanyUuid) == 0)
	{
		log("company does not exists: %s", StaffInfo.m_strCompanyUuid.c_str());
		return;
	}

	uint32_t unUserId = 0;
	int nStatus = 1;
	string strUserUuid = "";

	string strSql = "select userid, useruuid,status from IMStaff where uuid='" + StaffInfo.m_strStaffUuid + "'";
	CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			unUserId = pResultSet->GetInt("userid");
			nStatus = pResultSet->GetInt("status");
			strUserUuid = pResultSet->GetString("useruuid");
		}

		delete pResultSet;
		pResultSet = NULL;
	}

	if (StaffInfo.m_strUserUuid != "" && StaffInfo.m_strUserUuid != strUserUuid)
	{
		DeleteCompanyStaff(StaffInfo.m_strStaffUuid); //delete user from the company
	}

	uint32_t unUserId1 = unUserId;
	if (StaffInfo.m_strUserUuid != "")
	{
		unUserId1 = GetUserIdByUserUuid(StaffInfo.m_strUserUuid);
	}
	
	//update all fields currently
	strSql = "update IMStaff set jobnumber = '";
	strSql.append(StaffInfo.m_strJobNumber).append("',mobilephone='").append(StaffInfo.m_strMobilePhone);
	strSql.append("',").append("staffname='").append(StaffInfo.m_strStaffName).append("',isadmin=");
	strSql += int2string(StaffInfo.m_unIsAdmin) + ",adminstatus=" + int2string(StaffInfo.m_unAdminStatus) + ",issuperadmin=" + int2string(StaffInfo.m_unIsSuperAdmin);
	strSql += ",isactived=" + int2string(StaffInfo.m_unIsActived) + ",contactstatus=" + int2string(StaffInfo.m_unContactStatus);
	strSql += ",status=" + int2string(StaffInfo.m_nStatus) + ",keyword='" + StaffInfo.m_strKeyword + "',remark='" + StaffInfo.m_strRemark;
	strSql += "',username='" + StaffInfo.m_strUserName + "',account='" + StaffInfo.m_strAccount + "',email = '" + StaffInfo.m_strEmail + "',defaultemail='" + StaffInfo.m_strDefaultEmail;
	strSql += "',updated=unix_timestamp(),userid=" +int2string(unUserId1) + ",useruuid='" + StaffInfo.m_strUserUuid + "' where uuid='" + StaffInfo.m_strStaffUuid + "'";

	log("SQL for updating staff info: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());


	if (StaffInfo.m_strUserUuid != "" && StaffInfo.m_strUserUuid != strUserUuid) //staff uuid is related with a new user uuid
	{
		StaffInfo.m_unUserId = unUserId1;
		AddCompanyStaff(StaffInfo); //add a new user to the company
		LIST_USER_DUTY listUserDuty;
		GetStaffDuty(StaffInfo.m_strStaffUuid, listUserDuty);
		for (IT_LIST_USER_DUTY it = listUserDuty.begin(); it != listUserDuty.end(); it++)
		{
			SendUserPositionChangeNotify(StaffInfo.m_strStaffUuid, unUserId1, it->m_strDepartId, it->m_strPositionId, IM::BaseDefine::DATA_SYNC_TYPE_ADD_USER_DUTY, it->m_strJobGrade);
		}
	}
	else
	{
		////update all fields currently
		//strSql = "update IMStaff set jobnumber = '";
		//strSql.append(StaffInfo.m_strJobNumber).append("',mobilephone='").append(StaffInfo.m_strMobilePhone);
		//strSql.append("',").append("staffname='").append(StaffInfo.m_strStaffName).append("',isadmin=");
		//strSql += int2string(StaffInfo.m_unIsAdmin) + ",adminstatus=" + int2string(StaffInfo.m_unAdminStatus) + ",issuperadmin=" + int2string(StaffInfo.m_unIsSuperAdmin);
		//strSql += ",isactived=" + int2string(StaffInfo.m_unIsActived) + ",contactstatus=" + int2string(StaffInfo.m_unContactStatus);
		//strSql += ",status=" + int2string(StaffInfo.m_nStatus) + ",keyword='" + StaffInfo.m_strKeyword + "',remark='" + StaffInfo.m_strRemark + "',buyproduct='" + StaffInfo.m_strBuyProduct;
		//strSql += "',username='" + StaffInfo.m_strUserName + "',account='" + StaffInfo.m_strAccount + "',email = '" + StaffInfo.m_strEmail + "',defaultemail='" + StaffInfo.m_strDefaultEmail;
		//strSql += "',updated=unix_timestamp() where uuid='" + StaffInfo.m_strStaffUuid + "'";

		//log("SQL for updating staff info: %s", strSql.c_str());
		//m_pDBConn->ExecuteUpdate(strSql.c_str());

		////CheckUser(UserInfo.m_strUserUuid);

		if (StaffInfo.m_nStatus != 1)
		{
			RemoveStaffFromGroup(unUserId, m_nCompanyId);
		}

		SendStaffUpdateNotify(unUserId, nStatus, StaffInfo);
	}
}


void CDataSync::DeleteCompanyStaff(string& strStaffUuid)
{
	uint32_t unUserId = 0;
	string strUserUuid = "";
	string strSql = "select userid,companyid,useruuid from IMStaff where uuid='" + strStaffUuid + "'";
	CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			unUserId = pResultSet->GetInt("userid");
			strUserUuid = pResultSet->GetString("useruuid");
			m_nCompanyId = pResultSet->GetInt("companyid");
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	strSql = "update IMStaff set status=-1,updated=unix_timestamp() where uuid='" + strStaffUuid + "'";
	log("SQL for deleting staff: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());


	//DeleteUser(unUserId);

	////delete session
	//strSql = "update IMRecentSession set status=1,updated=unix_timestamp() where (userid=" + int2string(unUserId) + " or peerid=" + int2string(unUserId) + " and type=1) and status = 0";
	//log("SQL for deleting session for user: %d", unUserId);
	//m_pDBConn->ExecuteQuery(strSql.c_str());

	//CheckUser(strUserID);
	RemoveStaffFromGroup(unUserId, m_nCompanyId);

	SendStaffDelNotify(unUserId, strUserUuid, strStaffUuid);

}


void CDataSync::SetAllUsers(void)
{
	//get company id
	string strSql = "select id from IMUser where status <> -1 and id > 10000";
	log("SQL for getting users: %s", strSql.c_str());

	CResultSet *pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	uint32_t unUserId = 0;
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			unUserId = pResultSet->GetInt("id");
			m_listUser.push_back(unUserId);
		}
		delete pResultSet;
		pResultSet = NULL;
	}
}


uint32_t CDataSync::GetUserIdByStaffUuid(string& strStaffUuid)
{
	uint32_t unUserId = 0;
	string strSql = "select userid from IMStaff where uuid='" + strStaffUuid + "'";
	CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());

	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			unUserId = pResultSet->GetInt("userid");
		}

		delete pResultSet;
		pResultSet = NULL;
	}
	return unUserId;
}


int CDataSync::GetStaffInfoByStaffUuid(string& strStaffUuid, STAFF_INFO& StaffInfo)
{
	StaffInfo.m_strStaffUuid = strStaffUuid;
	uint32_t unUserId = 0;

	//string strSql = "select userid,useruuid,jobnumber,mobilephone,staffname,isadusername,name,avatar,sex,birthday,phone,contactstatus,status,keyword,signature,nick,buyproduct from IMUser where useruuid='" + strUserId + "'";
	string strSql = "select * from IMStaff where uuid='" + strStaffUuid + "'";
	log("SQL for getting staff information: %s", strSql.c_str());

	CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			if (pResultSet->GetString("jobnumber") == NULL)
			{
				StaffInfo.m_strJobNumber = "";
			}
			else
			{
				StaffInfo.m_strJobNumber = pResultSet->GetString("jobnumber");
			}

			unUserId = pResultSet->GetInt("userid");

			if (pResultSet->GetString("useruuid") == NULL)
			{
				StaffInfo.m_strUserUuid = "";
			}
			else
			{
				StaffInfo.m_strUserUuid = pResultSet->GetString("useruuid");
			}
			StaffInfo.m_strCompanyUuid = int2string(m_nCompanyId);		
			StaffInfo.m_unIsAdmin = pResultSet->GetInt("isadmin");
			StaffInfo.m_unIsActived = pResultSet->GetInt("isactived");
			StaffInfo.m_unAdminStatus = pResultSet->GetInt("adminstatus");
			StaffInfo.m_unIsSuperAdmin = pResultSet->GetInt("issuperadmin");

			if (pResultSet->GetString("staffname") == NULL)
			{
				StaffInfo.m_strStaffName= "";
			}
			else
			{
				StaffInfo.m_strStaffName = pResultSet->GetString("staffname");
			}

			//StaffInfo.m_nSex = pResultSet->GetInt("sex");

			if (pResultSet->GetString("keyword") == NULL)
			{
				StaffInfo.m_strKeyword = "";
			}
			else
			{
				StaffInfo.m_strKeyword = pResultSet->GetString("keyword");
			}

			if (pResultSet->GetString("remark") == NULL)
			{
				StaffInfo.m_strRemark = "";
			}
			else
			{
				StaffInfo.m_strRemark = pResultSet->GetString("remark");
			}

			if (pResultSet->GetString("mobilephone") == NULL)
			{
				StaffInfo.m_strMobilePhone = "";
			}
			else
			{
				StaffInfo.m_strMobilePhone = pResultSet->GetString("mobilephone");
			}

				//appened by george on Nov 12th,2015
			if (pResultSet->GetString("buyproduct") == NULL)
			{
				StaffInfo.m_strBuyProduct = "";
			}
			else
			{
				StaffInfo.m_strBuyProduct = pResultSet->GetString("buyproduct");
			}

			StaffInfo.m_nStatus = pResultSet->GetInt("status");
			StaffInfo.m_unContactStatus = pResultSet->GetInt("contactstatus");
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	return unUserId;

}


int CDataSync::GetCompanyPasswordRule(string& strInputString, DOMAIN_PASSWORD& PasswordRule)
{
	Json::Reader Reader;
	Json::Value Root;

	int nResult = 1;

	try
	{
		if (Reader.parse(strInputString, Root))
		{
			/*if (!Root["resultCode"].isNull() && Root["resultCode"].isString())
			{
			string strResult = Root["resultCode"].asString();
			if (strResult != "0000")
			{
			return -1;
			}
			}
			else
			{
			return -1;
			}*/

			if (!Root["primaryOrgId"].isNull() && Root["primaryOrgId"].isString())
			{
				PasswordRule.m_strDomainId = Root["primaryOrgId"].asString();
			}

			if (!Root["validityStatus"].isNull() && Root["validityStatus"].isInt())
			{
				PasswordRule.m_unValidityStatus = Root["validityStatus"].asInt();
			}

			if (!Root["validity"].isNull() && Root["validity"].isInt())
			{
				PasswordRule.m_unValidity = Root["validity"].asInt();
			}

			if (!Root["isForcePassword"].isNull() && Root["isForcePassword"].isInt())
			{
				PasswordRule.m_unIsForcePassword = Root["isForcePassword"].asInt();
			}

			if (!Root["passwordStrength"].isNull() && Root["passwordStrength"].isInt())
			{
				PasswordRule.m_unPasswordStrength = Root["passwordStrength"].asInt();
			}

			if (!Root["passwordLength"].isNull() && Root["passwordLength"].isInt())
			{
				PasswordRule.m_unPasswordLength = Root["passwordLength"].asInt();
			}

		}
		else
		{
			log("parse domainpassword data error");
			nResult = -1;
		}
	}
	catch (std::runtime_error msg)
	{
		log("an exception occurred when parsing domain password");
		nResult = -1;
	}

	return nResult;
}


int CDataSync::GetCompanyInfoSet(string& strInputString, DOMAIN_INFO& InfoSet)
{
	Json::Reader Reader;
	Json::Value Root;

	int nResult = 1;

	try
	{
		if (Reader.parse(strInputString, Root))
		{
			if (!Root["primaryOrgId"].isNull() && Root["primaryOrgId"].isString())
			{
				InfoSet.m_strCompanyUuid = Root["primaryOrgId"].asString();
			}

			if (!Root["isSearch"].isNull() && Root["isSearch"].isInt())
			{
				InfoSet.m_unIsJoined = Root["isSearch"].asInt();
			}

			if (!Root["isInvite"].isNull() && Root["isInvite"].isInt())
			{
				InfoSet.m_unIsInvited = Root["isInvite"].asInt();
			}
		}
		else
		{
			log("parse company info set data error");
			nResult = -1;
		}
	}
	catch (std::runtime_error msg)
	{
		log("an exception occurred when parsing domain password");
		nResult = -1;
	}

	return nResult;
}


void CDataSync::UpdateCompanyInfoSet(DOMAIN_INFO& InfoSet)
{
	string strSql = "update IMCompany set isjoined=" + int2string(InfoSet.m_unIsJoined) + ",isinvite=" + int2string(InfoSet.m_unIsInvited);
	strSql += ",updated=unix_timestamp() where companyuuid='" + InfoSet.m_strCompanyUuid + "'";

	log("SQL for updating company info set; %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());
	uint32_t unCompanyId = GetCompanyInfoByCompanyUuid(InfoSet.m_strCompanyUuid, InfoSet);
	SendCompanyUpdateNotify(unCompanyId, InfoSet);
}

void CDataSync::DeleteCompany(string& strCompanyUuid)
{
	string strSql = "update IMCompany set status=-1,updated=unix_timestamp() where companyuuid='" + strCompanyUuid + "'";
	log("SQL for deleting a company; %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());
	DOMAIN_INFO NewCompanyInfo;
	m_nCompanyId = GetCompanyInfoByCompanyUuid(strCompanyUuid, NewCompanyInfo);
	SendCompanyDelNotify(m_nCompanyId, NewCompanyInfo);
}


void CDataSync::DeleteCompanyPasswordRule(string& strCompanyUuid)
{
	string strSql = "update IMCompany set validitystatus=0,validity=30,isforcepassword=0,updated=unix_timestamp(),passwordstrength=1,passwordlength=8 where companyuuid='" + strCompanyUuid + "'";
	log("SQL for deleting company password rule; %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::DeleteCompanyInfoSet(string& strCompanyUuid)
{
	string strSql = "update IMCompany set isjoined=0, isinvite=0 where companyuuid='" + strCompanyUuid + "'"; 
	log("delete company info set; %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());
	DOMAIN_INFO InfoSet;
	uint32_t unCompanyId = GetCompanyInfoByCompanyUuid(strCompanyUuid, InfoSet);
	SendCompanyUpdateNotify(unCompanyId, InfoSet);
}


int CDataSync::GetStaffDetails(string& strInputString, LIST_USER_DETAILS& listUserDetails)
{
	Json::Reader reader;
	Json::Value root;

	if (reader.parse(strInputString, root))
	{
		try
		{
			int nCount = root.size();
			for (int i = 0; i < nCount; i++)
			{
				USER_DETAILS UserDetails;
				Json::Value Details = root[i];
				if (!Details["staffDetailsId"].isNull() && Details["staffDetailsId"].isString())
				{
					UserDetails.m_strUUID = Details["staffDetailsId"].asString();
				}

				if (!Details["staffId"].isNull() && Details["staffId"].isString())
				{
					UserDetails.m_strUserUUID = Details["staffId"].asString();
				}

				if (!Details["type"].isNull() && Details["type"].isInt())
				{
					UserDetails.m_nType = Details["type"].asInt();
				}

				if (!Details["title"].isNull() && Details["title"].isString())
				{
					UserDetails.m_strTitle = Details["title"].asString();
				}

				if (!Details["content"].isNull() && Details["content"].isString())
				{
					UserDetails.m_strContent = Details["content"].asString();
				}

				listUserDetails.push_back(UserDetails);
			}
		}
		catch (std::runtime_error msg)
		{
			log("staffdetails,parse json data failed.");
		}
	}
	return 1;
}


//uint32_t CDataSync::GetCompanyInfoByCompanyUuid(string& strCompanyUuid, DOMAIN_INFO& CompanyInfo)
//{
//	CompanyInfo.m_strCompanyUuid = strCompanyUuid;
//	uint32_t unCompanyId = 0;
//	string strSql = "select id,companyno,companyname,companyfullname,industrytype,isjoined,isinvite,status from IMCompany where companyuuid='" + strCompanyUuid + "'";
//	log("SQL for getting company information: %s", strSql.c_str());
//
//	CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
//	if (pResultSet)
//	{
//		while (pResultSet->Next())
//		{
//			if (pResultSet->GetString("companyno") == NULL)
//			{
//				CompanyInfo.m_strCompanyNo = "";
//			}
//			else
//			{
//				CompanyInfo.m_strCompanyNo = pResultSet->GetString("companyno");
//			}
//
//			unCompanyId = pResultSet->GetInt("id");
//
//			if (pResultSet->GetString("companyname") == NULL)
//			{
//				CompanyInfo.m_strCompanyName = "";
//			}
//			else
//			{
//				CompanyInfo.m_strCompanyName = pResultSet->GetString("companyname");
//			}
//
//			if (pResultSet->GetString("companyfullname") == NULL)
//			{
//				CompanyInfo.m_strCompanyFullName = "";
//			}
//			else
//			{
//				CompanyInfo.m_strCompanyFullName = pResultSet->GetString("companyfullname");
//			}
//
//			//CompanyInfo.m_nSex = pResultSet->GetInt("sex");
//
//			if (pResultSet->GetString("industrytype") == NULL)
//			{
//				CompanyInfo.m_strIndustryType = "";
//			}
//			else
//			{
//				CompanyInfo.m_strIndustryType = pResultSet->GetString("industrytype");
//			}
//
//			CompanyInfo.m_nStatus = pResultSet->GetInt("status");
//			CompanyInfo.m_unIsJoined = pResultSet->GetInt("isjoined");
//			CompanyInfo.m_unIsInvited = pResultSet->GetInt("isinvite");
//		}
//		delete pResultSet;
//		pResultSet = NULL;
//	}
//
//	return unCompanyId;
//}

void CDataSync::GetCompanyInfoByCompanyId(uint32_t unCompanyId, DOMAIN_INFO& CompanyInfo)
{
	string strSql = "select companyuuid,companyno,companyname,companyfullname,industrytype,isjoined,isinvite,status from IMCompany where id=" + int2string(unCompanyId);
	log("SQL for getting company information: %s", strSql.c_str());

	CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			if (pResultSet->GetString("companyuuid") != NULL)
			{
				CompanyInfo.m_strCompanyUuid = pResultSet->GetString("companyuuid");
			}
			else
			{
				CompanyInfo.m_strCompanyUuid = "";
			}
			
			if (pResultSet->GetString("companyno") == NULL)
			{
				CompanyInfo.m_strCompanyNo = "";
			}
			else
			{
				CompanyInfo.m_strCompanyNo = pResultSet->GetString("companyno");
			}

			unCompanyId = pResultSet->GetInt("id");

			if (pResultSet->GetString("companyname") == NULL)
			{
				CompanyInfo.m_strCompanyName = "";
			}
			else
			{
				CompanyInfo.m_strCompanyName = pResultSet->GetString("companyname");
			}

			if (pResultSet->GetString("companyfullname") == NULL)
			{
				CompanyInfo.m_strCompanyFullName = "";
			}
			else
			{
				CompanyInfo.m_strCompanyFullName = pResultSet->GetString("companyfullname");
			}

			//CompanyInfo.m_nSex = pResultSet->GetInt("sex");

			if (pResultSet->GetString("industrytype") == NULL)
			{
				CompanyInfo.m_strIndustryType = "";
			}
			else
			{
				CompanyInfo.m_strIndustryType = pResultSet->GetString("industrytype");
			}

			CompanyInfo.m_nStatus = pResultSet->GetInt("status");
			CompanyInfo.m_unIsJoined = pResultSet->GetInt("isjoined");
			CompanyInfo.m_unIsInvited = pResultSet->GetInt("isinvite");
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	//return unCompanyId;
}


void CDataSync::SendCompanyAddNotify(uint32_t unUserId, uint32_t unCompanyId, DOMAIN_INFO& CompanyInfo)
{
	IM::Buddy::IMCompanySyncNotify msgNotify;

	msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_ADD_COMPANY);
	msgNotify.set_sync_company_id(unCompanyId);
	msgNotify.set_sync_company_uuid(CompanyInfo.m_strCompanyUuid);
	msgNotify.set_company_no(CompanyInfo.m_strCompanyNo);
	msgNotify.set_company_name(CompanyInfo.m_strCompanyName);
	msgNotify.set_company_full_name(CompanyInfo.m_strCompanyFullName);
	msgNotify.set_industry_type(CompanyInfo.m_strIndustryType);
	msgNotify.set_company_is_joined(CompanyInfo.m_unIsJoined);
	msgNotify.set_company_is_invite(CompanyInfo.m_unIsInvited);
	msgNotify.set_status(CompanyInfo.m_nStatus);

	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn)
	{
		pRouteConn->Lock();
	
		log("Sending notify to  %d", unUserId);
		msgNotify.set_user_id(unUserId);
		CImPdu pdu;
		pdu.SetPBMsg(&msgNotify);
		pdu.SetServiceId(SID_BUDDY_LIST);
		pdu.SetCommandId(CID_BUDDY_LIST_SYNC_COMPANY_NOTIFY);

		int nBytes = pRouteConn->SendPdu(&pdu);
		log("Sent %d bytes to route server", nBytes);

		pRouteConn->Unlock();
	}
	else
	{
		log("route server is unavailable!");
	}


	//CRouteServConn* pRouteConn = get_route_serv_conn();
	//if (pRouteConn)
	//{
	//	pRouteConn->Lock();

	//	IT_MAP_COMPANY_USER it = m_mapDomainUser.find(m_nCompanyId);
	//	if (it != m_mapDomainUser.end())
	//	{
	//		for (IT_MAP_USER_KEY itUser = it->second.begin(); itUser != it->second.end(); itUser++)	//send notify to all users in the company
	//		{
	//			log("Sending notify to  %d", itUser->first);
	//			msgNotify.set_user_id(itUser->first);
	//			CImPdu pdu;
	//			pdu.SetPBMsg(&msgNotify);
	//			pdu.SetServiceId(SID_BUDDY_LIST);
	//			pdu.SetCommandId(CID_BUDDY_LIST_SYNC_COMPANY_NOTIFY);

	//			int nBytes = pRouteConn->SendPdu(&pdu);
	//			log("Sent %d bytes to route server", nBytes);
	//		}
	//	}
	//	pRouteConn->Unlock();
	//}
	//else
	//{
	//	log("route server is unavailable!");
	//}
}


void CDataSync::SendCompanyUpdateNotify(uint32_t unCompanyId, DOMAIN_INFO& CompanyInfo)
{
	IM::Buddy::IMCompanySyncNotify msgNotify;

	msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_MOD_COMPANY);
	msgNotify.set_sync_company_id(unCompanyId);
	msgNotify.set_sync_company_uuid(CompanyInfo.m_strCompanyUuid);
	msgNotify.set_company_no(CompanyInfo.m_strCompanyNo);
	msgNotify.set_company_name(CompanyInfo.m_strCompanyName);
	msgNotify.set_company_full_name(CompanyInfo.m_strCompanyFullName);
	msgNotify.set_industry_type(CompanyInfo.m_strIndustryType);
	msgNotify.set_company_is_joined(CompanyInfo.m_unIsJoined);
	msgNotify.set_company_is_invite(CompanyInfo.m_unIsInvited);
	msgNotify.set_status(CompanyInfo.m_nStatus);

	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn)
	{
		pRouteConn->Lock();

		IT_MAP_COMPANY_USER it = m_mapDomainUser.find(unCompanyId);
		if (it != m_mapDomainUser.end())
		{
			for (IT_MAP_USER_KEY itUser = it->second.begin(); itUser != it->second.end(); itUser++)	//send notify to all users in the company
			{
				log("Sending notify to  %d", itUser->first);
				msgNotify.set_user_id(itUser->first);
				CImPdu pdu;
				pdu.SetPBMsg(&msgNotify);
				pdu.SetServiceId(SID_BUDDY_LIST);
				pdu.SetCommandId(CID_BUDDY_LIST_SYNC_COMPANY_NOTIFY);

				int nBytes = pRouteConn->SendPdu(&pdu);
				log("Sent %d bytes to route server", nBytes);
			}
		}
		pRouteConn->Unlock();
	}
	else
	{
		log("route server is unavailable!");
	}

}


void CDataSync::SendCompanyDelNotify(uint32_t unCompanyId, DOMAIN_INFO& CompanyInfo)
{
	IM::Buddy::IMCompanySyncNotify msgNotify;

	msgNotify.set_sync_type(IM::BaseDefine::DATA_SYNC_TYPE_DEL_COMPANY);
	msgNotify.set_sync_company_id(unCompanyId);
	msgNotify.set_sync_company_uuid(CompanyInfo.m_strCompanyUuid);
	msgNotify.set_status(CompanyInfo.m_nStatus);

	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn)
	{
		pRouteConn->Lock();

		IT_MAP_COMPANY_USER it = m_mapDomainUser.find(unCompanyId);
		if (it != m_mapDomainUser.end())
		{
			for (IT_MAP_USER_KEY itUser = it->second.begin(); itUser != it->second.end(); itUser++)	//send notify to all users in the company
			{
				log("Sending notify to  %d", itUser->first);
				msgNotify.set_user_id(itUser->first);
				CImPdu pdu;
				pdu.SetPBMsg(&msgNotify);
				pdu.SetServiceId(SID_BUDDY_LIST);
				pdu.SetCommandId(CID_BUDDY_LIST_SYNC_COMPANY_NOTIFY);

				int nBytes = pRouteConn->SendPdu(&pdu);
				log("Sent %d bytes to route server", nBytes);
			}

			it->second.clear();
			m_mapDomainUser.erase(it);
		}
		pRouteConn->Unlock();
	}
	else
	{
		log("route server is unavailable!");
	}
}


void CDataSync::GetStaffProducts(string& strInputString)
{
	Json::Reader reader;
	Json::Value root;

	if (reader.parse(strInputString, root))
	{
		try
		{
			int nCount = root.size();
			string strStaffId = "";
			string strProducts = "";

			for (int i = 0; i < nCount; i++)
			{
				int nStatus = 0;
				Json::Value Product = root[i];
				if (!Product["status"].isNull() && Product["status"].isInt())
				{
					nStatus = Product["status"].asInt();
				}

				if (nStatus)
				{
					if (!Product["productId"].isNull() && Product["productId"].isString())
					{
						if (strProducts == "")
						{
							strProducts = Product["productId"].asString();
						}
						else
						{
							strProducts += "," + Product["productId"].asString();
						}
					}
				}

				if (!Product["staffId"].isNull() && Product["staffId"].isString())
				{
					strStaffId = Product["staffId"].asString();
				}
			}

			if (GetCompanyIdByStaffUuid(strStaffId) > 0)
			{
				string strSql = "update IMStaff set buyproduct='" + strProducts + "',updated=unix_timestamp() where uuid='" + strStaffId + "'";
				log("SQL for updating IMStaff: %s", strSql.c_str());
				m_pDBConn->ExecuteUpdate(strSql.c_str());

				//get user information
				STAFF_INFO UserInfo;
				uint32_t unUserId = GetStaffInfoByStaffUuid(strStaffId, UserInfo);
				if (unUserId > 0)
				{
					////nStatus = GetStatusByProduct(strProducts, UserInfo.m_nStatus); //get status by products
					//nStatus = UserInfo.m_nStatus;
					//SetUserStatus(unUserId, nStatus);

					SendStaffUpdateNotify(unUserId, 1, UserInfo);
				}
			}
		}
		catch (std::runtime_error msg)
		{
			log("staff product,parse json data failed.");
		}
	}
}


void CDataSync::RemoveStaffFromGroup(uint32_t unUserId, uint32_t unCompanyId)
{
	string strSql = "BEGIN";
	m_pDBConn->ExecuteUpdate(strSql.c_str());
	
	strSql = "select a.id as gid,creator,usercnt from IMGroup a, IMGroupMember b where a.id=b.groupid and companyid=" + int2string(unCompanyId) + " and userid=" + int2string(unUserId) + " and type <> 3 and a.status= 0 and b.status=0";
	log("SQL for getting group info: %s", strSql.c_str());

	CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		uint32_t  unGroupId = 0, unCreator = 0;
		int32_t nUserCount = 0;
		while (pResultSet->Next())
		{
			unGroupId = pResultSet->GetInt("gid");
			unCreator = pResultSet->GetInt("creator");
			uint32_t unCurrentTime = time(NULL);
			nUserCount = pResultSet->GetInt("usercnt");

			list<uint32_t> listUsers;

			if (nUserCount > 1)
			{
				string strSql1 = "update IMGroup a, IMGroupMember b set userCnt = userCnt-1, a.updated=unix_timestamp(), b.updated=unix_timestamp(),version=version+1,b.status=1 where a.id=b.groupid and a.id=" + int2string(unGroupId) + " and b.userid=" + int2string(unUserId);
				log("SQL for updating group: %s", strSql1.c_str());
				m_pDBConn->ExecuteUpdate(strSql1.c_str());

				//get current user list
				strSql1 = "select userid from IMGroupMember where status=0 and groupid=" + int2string(unGroupId);
				CResultSet *pResultSet1 = m_pDBConn->ExecuteQuery(strSql1.c_str());
				
				if (pResultSet1)
				{
					uint32_t unCurrUserId = 0;
					while (pResultSet1->Next())
					{
						unCurrUserId = pResultSet1->GetInt("userid");
						listUsers.push_back(unCurrUserId);
					}
					delete pResultSet1;
					pResultSet1 = NULL;
				}

				//send member change notify
				SendGroupMemberChangeNotify(unGroupId, unUserId, listUsers);

				//remove group user cache
				SetGroupUserStatus(unGroupId, unUserId, 1, unCurrentTime);

				//clear count and remove session
				DeleteStaff(unUserId, unGroupId);

				//change creator
				if (unCreator == 0 || unCreator == unUserId) 
				{
					strSql1 = "select min(userid) as minuserid from IMGroupMember where groupid=" + int2string(unGroupId) + " and status = 0";
					pResultSet1 = m_pDBConn->ExecuteQuery(strSql1.c_str());
					uint32_t unNewCreator = 0;
					if (pResultSet1)
					{
						while (pResultSet1->Next())
						{
							unNewCreator = pResultSet1->GetInt("minuserid");
						}
						delete pResultSet1;
						pResultSet1 = NULL;
					}

					if (unNewCreator > 10000)
					{
						strSql1 = "update IMGroup set creator=" + int2string(unNewCreator) + ", updated=unix_timestamp(), version=version+1 where id=" + int2string(unGroupId);
						m_pDBConn->ExecuteUpdate(strSql1.c_str()); //update creator 
						SendCreatorChangeNotify(unGroupId, unNewCreator, listUsers); //send notify
					}
				}
			}
			else if (nUserCount == 1)
			{
				string strSql1 = "update IMGroup a, IMGroupMember b set userCnt=0, version=version+1, a.status=1, b.status=1,a.updated=unix_timestamp(),b.updated=unix_timestamp() where a.id=b.groupid and a.id=" + int2string(unGroupId) + " and b.status=0";
				log("SQL for updating group member: %s", strSql1.c_str());
				m_pDBConn->ExecuteUpdate(strSql1.c_str());
				//send member change notify
				SendGroupMemberChangeNotify(unGroupId, unUserId, listUsers);

				//remove group user cache
				SetGroupUserStatus(unGroupId, unUserId, 1, unCurrentTime);

				DeleteStaff(unUserId, unGroupId);
			}		
		}

		delete pResultSet;
		pResultSet = NULL;
	}

	strSql = "COMMIT";
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}



void CDataSync::clearUserCounter(uint32_t unUserId, uint32_t unGroupId)
{
	if (m_pCacheUnreadMsg)
	{
		string strGroupKey = int2string(unGroupId) + GROUP_TOTAL_MSG_COUNTER_REDIS_KEY_SUFFIX;
		map<string, string> mapGroupCount;
		bool bRet = m_pCacheUnreadMsg->hgetAll(strGroupKey, mapGroupCount);
		if (bRet)
		{
			string strUserKey = int2string(unUserId) + "_" + int2string(unGroupId) + GROUP_USER_MSG_COUNTER_REDIS_KEY_SUFFIX;
			string strReply = m_pCacheUnreadMsg->hmset(strUserKey, mapGroupCount);
			if (strReply.empty())
			{
				log("hmset %s failed !", strUserKey.c_str());
			}

			//reset lastatmsgid (user----peer(group id) frequently?
			string strSql = "update IMRecentSession set lastatmsgid = NULL,updated=unix_timestamp() where userid = " + int2string(unUserId) + " and peerid= " + int2string(unGroupId) + " and type=2 and lastatmsgid is NOT NULL";
			log("SQL for updating session last at msgid: %s", strSql.c_str());
			m_pDBConn->ExecuteUpdate(strSql.c_str());
		}
		else
		{
			log("hgetall %s failed!", strGroupKey.c_str());
		}
	}
	else
	{
		log("no cache connection for unread");
	}
}


void CDataSync::SendRemoveSessionNotify(uint32_t unUserId, uint32_t unGroupId)
{
	string strSql = "select id from IMRecentSession where userid=" + int2string(unUserId) + " and peerid = " + int2string(unGroupId) + " and type= 2 and status = 0";
	log("SQL for getting session id: %s", strSql.c_str());

	uint32_t unSessionId = 0;
	CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			unSessionId = pResultSet->GetInt("id");
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	strSql = "update IMRecentSession set status=1,updated=unix_timestamp() where id = " + int2string(unSessionId);
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn)
	{
		IM::Buddy::IMRemoveSessionNotify msgNotify;

		pRouteConn->Lock();

		msgNotify.set_user_id(unUserId);
		msgNotify.set_session_type(IM::BaseDefine::SESSION_TYPE_GROUP);
		msgNotify.set_session_id(unGroupId);

		CImPdu pdu;
		pdu.SetPBMsg(&msgNotify);
		pdu.SetServiceId(SID_BUDDY_LIST);
		pdu.SetCommandId(CID_BUDDY_LIST_REMOVE_SESSION_NOTIFY);

		int nBytes = pRouteConn->SendPdu(&pdu);
		log("Sent %d bytes to route server", nBytes);
		pRouteConn->Unlock();
	}
	else
	{
		log("route server is unavailable!........");
	}
}


void CDataSync::DeleteStaff(uint32_t unUserId, uint32_t unGroupId)
{
	clearUserCounter(unUserId, unGroupId);
	SendRemoveSessionNotify(unUserId, unGroupId);
}


void CDataSync::UpdateStaffPosition(string& strStaffId, string& strDepartId, string& strJobGrade)
{
	//if(GetCompanyIdByUserId(strUserID) == 0)
	if (GetCompanyIdByStaffUuid(strStaffId) == 0)
	{
		return;
	}

	uint32_t unUserId = GetUserIdByStaffUuid(strStaffId); //get user id

	string strPositionId = DATA_SYNC_DEFAULT_POSITION_UUID;

	string strSql = "update IMUserPosition set jobgrade='" + strJobGrade + "',updated=unix_timestamp() where  useruuid='" + strStaffId + "' and departuuid='" + strDepartId + "' and posuuid='" + strPositionId + "' and status=0";
	log("SQL for updating jobgrade; %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	SendUserPositionChangeNotify(strStaffId, unUserId, strDepartId, strPositionId, IM::BaseDefine::DATA_SYNC_TYPE_MOD_USER_DUTY, strJobGrade);

	UpdateStaffChangeTime(strStaffId);
}


void CDataSync::GetLimitedIpInfo(string strInputString, LIST_LIMIT_IP& lsLimitIp)
{
	Json::Reader reader;
	Json::Value root;

	if (reader.parse(strInputString, root))
	{
		try
		{
			int nSize = root.size();
			LIMIT_IP currIpInfo;
			for (int i = 0; i < nSize; i++)
			{
				Json::Value limitIpInfo = root[i];
				if (!limitIpInfo["limitIpId"].isNull() && limitIpInfo["limitIpId"].isString())
				{
					currIpInfo.m_strLimitIpUuid = limitIpInfo["limitIpId"].asString();
				}

				if (!limitIpInfo["userId"].isNull() && limitIpInfo["userId"].isString())
				{
					currIpInfo.m_strUserUuid = limitIpInfo["userId"].asString();
				}

				if (!limitIpInfo["limitIp"].isNull() && limitIpInfo["limitIp"].isString())
				{
					currIpInfo.m_strLimitIp = limitIpInfo["limitIp"].asString();
				}

				lsLimitIp.push_back(currIpInfo);
			}
		}
		catch (std::runtime_error msg)
		{
			log("ip limit,parse json data failed.");
		}
	}
}


void CDataSync::SetUserLimitedIp(string& strUserUuid, LIST_LIMIT_IP& lsLimitIp)
{
	//delete all iplimit information first
	string strSql = "delete from IMUserIpLimit where useruuid='" + strUserUuid + "'";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "insert into IMUserIpLimit (uuid,useruuid,limitip,created,updated) values(?,?,?,?,?)";
	for (IT_LIST_LIMIT_IP it = lsLimitIp.begin(); it != lsLimitIp.end(); it++)
	{
		CPrepareStatement* pStmt = new CPrepareStatement();
		if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
		{
			int nIndex = 0;
			unsigned int nCurrenTime = time(NULL);
			pStmt->SetParam(nIndex++, it->m_strLimitIpUuid);
			pStmt->SetParam(nIndex++, it->m_strUserUuid);
			pStmt->SetParam(nIndex++, it->m_strLimitIp);
			pStmt->SetParam(nIndex++, nCurrenTime);
			pStmt->SetParam(nIndex++, nCurrenTime);
			pStmt->ExecuteUpdate();
		}
		delete pStmt;
	}
}


int CDataSync::GetCompanyInfoByCompanyUuid(string& strCompanyUuid, DOMAIN_INFO& CompanyInfo)
{
	string strSql = "select id,companyno,companyname,companyfullname,industrytype,isjoined,isinvite,status from IMCompany where companyuuid='" + strCompanyUuid + "'";
	CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	uint32_t unCompanyId = 0;
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			unCompanyId = pResultSet->GetInt("id");	
			CompanyInfo.m_strCompanyUuid = strCompanyUuid;

			if (pResultSet->GetString("companyno") == NULL)
			{
				CompanyInfo.m_strCompanyNo = "";
			}
			else
			{
				CompanyInfo.m_strCompanyNo = pResultSet->GetString("companyno");
			}

			if (pResultSet->GetString("companyname") == NULL)
			{
				CompanyInfo.m_strCompanyName = "";
			}
			else
			{
				CompanyInfo.m_strCompanyName = pResultSet->GetString("companyname");
			}

			if (pResultSet->GetString("companyfullname") == NULL)
			{
				CompanyInfo.m_strCompanyFullName = "";
			}
			else
			{
				CompanyInfo.m_strCompanyFullName = pResultSet->GetString("companyfullname");
			}

			//CompanyInfo.m_nSex = pResultSet->GetInt("sex");

			if (pResultSet->GetString("industrytype") == NULL)
			{
				CompanyInfo.m_strIndustryType = "";
			}
			else
			{
				CompanyInfo.m_strIndustryType = pResultSet->GetString("industrytype");
			}

			CompanyInfo.m_nStatus = pResultSet->GetInt("status");
			CompanyInfo.m_unIsJoined = pResultSet->GetInt("isjoined");
			CompanyInfo.m_unIsInvited = pResultSet->GetInt("isinvite");
		}

		delete pResultSet;
		pResultSet = NULL;
	}

	return unCompanyId;
}


void CDataSync::UpdateUserChangeTime(string& strUserUuid)
{
	string strSql = "update IMUser set updated=unix_timestamp() where useruuid='" + strUserUuid + "'";
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::UpdateStaffChangeTime(string& strStaffUuid)
{
	string strSql = "update IMStaff set updated=unix_timestamp() where uuid='" + strStaffUuid + "'";
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::DeleteStaffPositionByStaffId(string& strStaffId)
{
	if (GetCompanyIdByStaffUuid(strStaffId) > 0)
	{
		LIST_STRING listDepartment;
		string strSql = "select departuuid from IMUserPosition where useruuid='" + strStaffId + "' and status = 0";
		CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				string strDepartment = pResultSet->GetString("departuuid");
				listDepartment.push_back(strDepartment);
			}
			delete pResultSet;
			pResultSet = NULL;
		}


		//if no position after deleting ,we should set '' for the user
		strSql = "update IMUserPosition set status = -1, updated=unix_timestamp() where useruuid='" + strStaffId + "' and status = 0";
		m_pDBConn->ExecuteUpdate(strSql.c_str());
		log("SQL for deleting a record for IMUserPosition: %s", strSql.c_str());


		uint32_t unUserId = GetUserIdByStaffUuid(strStaffId); //get user id

		string strJobGrade = "";
		string strPositionID = DATA_SYNC_DEFAULT_POSITION_UUID;
		for (IT_LIST_STRING it = listDepartment.begin(); it != listDepartment.end(); it++)
		{
			string strDepartment = *it;
			SendUserPositionChangeNotify(strStaffId, unUserId, strDepartment, strPositionID, IM::BaseDefine::DATA_SYNC_TYPE_DEL_USER_DUTY, strJobGrade);
		}

		UpdateStaffChangeTime(strStaffId);
	}
}


void CDataSync::DeleteStaffPositionByDepartmentId(string& strDepartID)
{
	if (GetCompanyIdByDepartId(strDepartID) == 0)
	{
		return;
	}

	SendRemoveDepartmentUserNotify(strDepartID);

	string strSql = "update IMUserPosition a, IMStaff b set a.status = -1, a.updated=unix_timestamp(),b.updated=unix_timestamp() where a.useruuid=b.uuid and a.departuuid='" + strDepartID + "' and a.status = 0";
	log("SQL fro updating user position: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::DeletePositionByCompanyUuid(string& strCompanyId)
{
	int unCompanyId = GetCompanyIdByCompanyUuid(strCompanyId);
	if (unCompanyId == 0)
	{
		return;
	}
	LIST_STRING listPosition;

	string strSql = "select uuid from IMPosition where companyid=" + int2string(unCompanyId);
	CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			string strPositionUuid = pResultSet->GetString("uuid");
			listPosition.push_back(strPositionUuid);
		}

		delete pResultSet;
		pResultSet = NULL;
	}

	for (IT_LIST_STRING it = listPosition.begin(); it != listPosition.end(); it++)
	{
		string strPosition = *it;
		DeleteCompanyPosition(strPosition);
	}
}


void CDataSync::GetStaffDuty(string& strStaffUuid, LIST_USER_DUTY& listDuty)
{
	string strSql = "select departuuid,posuuid,jobgrade from IMUserPosition where useruuid='" + strStaffUuid + "' and status = 0";
	log("SQL for getting duty: %s", strSql.c_str());
	CResultSet *pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			USER_DUTY struUserDuty;
			struUserDuty.m_unUserId = 0;
			struUserDuty.m_strUserId = strStaffUuid;
			if (pResultSet->GetString("departuuid") != NULL)
			{
				struUserDuty.m_strDepartId = pResultSet->GetString("departuuid");
			}
			else
			{
				struUserDuty.m_strDepartId = "";
			}

			if (pResultSet->GetString("posuuid") != NULL)
			{
				struUserDuty.m_strPositionId = pResultSet->GetString("posuuid");
			}
			else
			{
				struUserDuty.m_strPositionId = "";
			}

			if (pResultSet->GetString("jobgrade") != NULL)
			{
				struUserDuty.m_strJobGrade = pResultSet->GetString("jobgrade");
			}
			else
			{
				struUserDuty.m_strJobGrade = "";
			}

			listDuty.push_back(struUserDuty);
		}

		delete pResultSet;
		pResultSet = NULL;
	}
}
