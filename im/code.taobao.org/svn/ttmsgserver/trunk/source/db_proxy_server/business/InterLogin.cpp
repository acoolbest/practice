/*================================================================
*     Copyright (c) 2015年 lanhu. All rights reserved.
*   
*   文件名称：InterLogin.cpp
*   创 建 者：Zhang Yuanhao
*   邮    箱：bluefoxah@gmail.com
*   创建日期：2015年03月09日
*   描    述：
*
================================================================*/
#include "InterLogin.h"
#include "../DBPool.h"
#include "EncDec.h"
#include "md5.h"
#include "UserModel.h"
#include <cctype>
#include "HttpClient.h"
#include "ConfigFileReader.h"
#include <time.h>

using namespace std;
string CInterLoginStrategy::m_strHost = "";

bool CInterLoginStrategy::doLogin(const std::string& strName, const std::string& strPass, uint32_t& unUserId, int32_t& nStatus, uint32_t unIsActivated, std::string& strClientIp)
{
	bool bRet = false;

	nStatus = LOGIN_RESULT_USER_NO_FOUND; //invalid username
	nStatus = Validating(strName, strPass,unUserId,unIsActivated, strClientIp);
	if (nStatus == 0)
	{
		bRet = true;
	}
	
	return bRet; 
}


int CInterLoginStrategy::Validating(const string& strName, const string& strPass, uint32_t &unUserId, uint32_t unActivatedStatus, string& strConnIp)
{
	int nResult = LOGIN_RESULT_USER_NO_FOUND;
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");

	if (pDBConn)
	{
		string strName1 = strName;
		string strUserUuid = "";
		uint32_t unIsInitPassword = 0, unLastModifiedTime = 0, unMinValidity = 0;
		transform(strName1.begin(), strName1.end(), strName1.begin(), ::tolower); //change the string name to lower case

		//updated on Aug 16th,2016. try at most 3 times if user no found(due to delayed sync)
		for (int i = 0; i < 2; i++)
		{
			string strSql = "select a.* from IMUser a left join UserAlias b on a.useruuid=b.useruuid where (username ='" + strName + "' or email ='" + strName + "' or phone='" + strName + "' or account='" + strName + "' or aliasname = '" + strName + "') order by a.updated desc limit 1";
			log("SQL for login: %s", strSql.c_str());
			CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());

			if (pResultSet)
			{
				string strPassword = "", strSalt = "";

				while (pResultSet->Next())
				{
					if (pResultSet->GetString("password") != NULL)
					{
						strPassword = pResultSet->GetString("password");
					}

					if (pResultSet->GetString("salt") != NULL)
					{
						strSalt = pResultSet->GetString("salt");
					}

					unUserId = pResultSet->GetInt("id");
					unIsInitPassword = pResultSet->GetInt("isinitpassword");
					strUserUuid = pResultSet->GetString("useruuid");
					unLastModifiedTime = pResultSet->GetInt("lastmodifiedtime");
					uint32_t unIsLock = pResultSet->GetInt("islock");
					//uint32_t unLastUpdateTime = pResultSet->GetInt("lastmodifytime");
					uint32_t unIsActived = pResultSet->GetInt("isactived");
					int32_t nStatus = pResultSet->GetInt("status");

					string strAccount = "";
					if (pResultSet->GetString("account") != NULL)
					{
						strAccount = pResultSet->GetString("account");
					}
					string strPhone = "";
					if (pResultSet->GetString("phone") != NULL)
					{
						strPhone = pResultSet->GetString("phone");
					}

					if (strPhone != "" && strAccount == strName)
					{
						nResult = LOGIN_REEULT_BIND_PHONE;
					}
					else
					{
						//nResult = Authenticate(strPass, strPassword, strSalt);
						nResult = Authenticate(strPass, strUserUuid, unIsInitPassword, unLastModifiedTime); //updated on Aug 16th,2016. authorizing by querying from EC but not in local db
						if (nResult == 0)
						{
							log("passed actived status: %d, actived status: %d", unActivatedStatus, unIsActived);
							//if (!unIsActived && !unActivatedStatus)
							//{
							//	nResult = LOGIN_RESULT_INACTIVE_USER;
							//}
							//else if (nStatus == 0)
							//{
							//	nResult = LOGIN_RESULT_DISABLED_USER;
							//}
							//else if (nStatus == -1)
							//{
							//	nResult = -1; //deleted user
							//}
							//else if (unIsLock)
							//{
							//	nResult = LOGIN_RESULT_LOCKED_BY_EC;
							//}
						}
					}
				}

				delete pResultSet;
				pResultSet = NULL;
			}
			else
			{
				log("no result set for sql:%s", strSql.c_str());
			}

			if (nResult == 0)
			{
				//get limited ip list
				log("Connected IP: %s", strConnIp.c_str());
				list<string> listIpList;
				bool bIsValidIp = true;
				strSql = "select limitip from IMUserIpLimit where useruuid='" + strUserUuid + "'";
				pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
				if (pResultSet)
				{
					while (pResultSet->Next())
					{
						string strIp = pResultSet->GetString("limitip");
						listIpList.push_back(strIp);
					}
					delete pResultSet;
					pResultSet = NULL;
				}

				if (listIpList.size() > 0)
				{
					bIsValidIp = IsLimitedIp(strConnIp, listIpList);
				}

				if (!bIsValidIp)
				{
					nResult = LOGIN_RESULT_LIMITED_IP;
				}
				else if (nResult == 0)
				{

					//get company password rule setting
					bool bValidityStatus = false;
					strSql = "select isforcepassword, validitystatus,validity from IMCompany a ";
					strSql += "where exists (select 1 from (select companyid from IMStaff where userid=" + int2string(unUserId) + " and status=1) as b where a.id=b.companyid)";
					log("SQL for getting force status: %s", strSql.c_str());
					pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
					if (pResultSet)
					{
						uint32_t unIsForcePassword = 0, unValidityStatus = 0, unValidity = 30;

						while (pResultSet->Next())
						{
							unIsForcePassword = pResultSet->GetInt("isforcepassword");
							unValidityStatus = pResultSet->GetInt("validitystatus");
							unValidity = pResultSet->GetInt("validity");
							if (unIsForcePassword && unIsInitPassword)  //force to change initial password
							{
								log("force to change password");
								nResult = LOGIN_RESULT_FORCE_CHG_PASSWORD;
								break;
							}
							else if (unValidityStatus)  //checking password validity period 
							{
								bValidityStatus = true;
								if (unMinValidity == 0)
								{
									unMinValidity = unValidity;
								}
								else if (unValidity < unMinValidity)
								{
									unMinValidity = unValidity;
								}
								log("Min: %d,Curr: %d", unMinValidity, unValidity);
							}
						}

						delete pResultSet;
						pResultSet = NULL;
					}
					else
					{
						log("no result set for sql:%s", strSql.c_str());
					}

					if (nResult == 0 && bValidityStatus)
					{
						log("password time expired...............");
						time_t unNow = time(NULL);
						if (unNow > unLastModifiedTime + unMinValidity * 24 * 3600)
						{
							nResult = LOGIN_RESULT_EXPIRED_PASSWORD;
						}
					}
				}
			}

			if (unUserId != 0)
			{
				break;
			}
			else
			{
				log("user: %s no  found, retry again after 5 seconds", strName.c_str());
				struct timespec TimeOut;
				memset(&TimeOut, 0, sizeof(TimeOut));
				TimeOut.tv_sec = 5;
				TimeOut.tv_nsec = 0;
				nanosleep(&TimeOut, NULL);
			}
		}

		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_slave");
	}

	return nResult;
}


int32_t CInterLoginStrategy::Authenticate(const string& strUserPassword, const string& strDbPassword, const string& strSalt)
{
	int32_t nResult = LOGIN_RESULT_INVALID_PASSWORD;
	char szPassword[36];
	memset(szPassword, 0, sizeof(szPassword));
	string strOrigPassword = strUserPassword + "{" + strSalt + "}";
	MD5_Calculate(strOrigPassword.c_str(), strOrigPassword.length(), szPassword);
	log("passed password:%s, salt:%s, user password: %s, password: %s", strUserPassword.c_str(), strSalt.c_str(), szPassword, strDbPassword.c_str());
	if (strDbPassword == szPassword)
	{
		nResult = 0;
	}

	return nResult;
}


void CInterLoginStrategy::GetBasicUserInfo(uint32_t unUserId, IM::BaseDefine::UserInfo& UserInfo)
{
	CUserModel::getInstance()->GetUserInfo(unUserId, UserInfo);
}


void CInterLoginStrategy::GetCompanyInfo(uint32_t unUserId, list<IM::BaseDefine::CompanyInfo>& lsCompanyInfo)
{
	CUserModel::getInstance()->GetJoinedCompany(unUserId, lsCompanyInfo);
}


void CInterLoginStrategy::GetShieldUsers(uint32_t unUserId, list<IM::BaseDefine::ShieldUser>& lsShieldUsers)
{
	CUserModel::getInstance()->GetUserShieldStatus(unUserId, lsShieldUsers);
}


void CInterLoginStrategy::GetUserDetails(string& strUserUuid, list<IM::BaseDefine::UserDetails>& lsUserDetails)
{
	CUserModel::getInstance()->GetUserAdditionalInfo(strUserUuid, lsUserDetails);
}


void CInterLoginStrategy::GetVips(uint32_t unUserId, list<uint32_t>& lsVips)
{
	CUserModel::getInstance()->GetVip(unUserId, lsVips);
}


void CInterLoginStrategy::GetStaffInfo(uint32_t unUserId, uint32_t unCompanyId, IM::BaseDefine::StaffInfo& StaffInfo)
{
	CUserModel::getInstance()->GetStaffInfo(unUserId, unCompanyId, StaffInfo);
}


void CInterLoginStrategy::GetStaffDuty(string& strUserUuid, uint32_t unCompanyId, list<IM::BaseDefine::DutyInfo>& listDuty, map<string, list<string>>& mapPosition)
{
	CUserModel::getInstance()->GetDuty(strUserUuid, listDuty, mapPosition, unCompanyId);
}


void CInterLoginStrategy::GetStaffDetails(string& strStaffUuid, list<IM::BaseDefine::UserDetails>& lsStaffDetails)
{
	CUserModel::getInstance()->GetUserAdditionalInfo(strStaffUuid, lsStaffDetails, 2);
}


//bool CInterLoginStrategy::doLogin(const std::string &strName, const std::string &strPass, IM::BaseDefine::UserInfo& user)
//{
//	bool bRet = false;
//
//	CDBManager* pDBManger = CDBManager::getInstance();
//	CDBConn* pDBConn = pDBManger->GetDBConn("teamtalk_slave");
//
//	int32_t nStatus = LOGIN_RESULT_USER_NO_FOUND; //invalid username
//	uint32_t unUserId = 0;
//	string strUserUuid = "";
//
//	if (pDBConn)
//	{
//		//string strSql = "select * from IMUser where name='" + strName + "' and status=0";
//		//string strSql = "select * from IMUser where email='" + strName + "' and status=0";
//		string strName1 = strName;
//		transform(strName1.begin(), strName1.end(), strName1.begin(), ::tolower); //change the string name to lower case
//		int nCurrPos = strName1.find('@');
//		int nValidityStatus = 0, nValidity = 0, nIsForcePassword = 0;
//		unsigned int unExpiredDate = 0;
//
//		//comment on July 5th,2016
//		//if (nCurrPos != string::npos)
//		//{
//		//	string strCurrDomain = strName.substr(nCurrPos + 1);
//		//	string strSql = "select * from IMCompany where domainname='" + strCurrDomain + "' and status=1";
//		//	log("SQL for getting domain id: %s", strSql.c_str());
//		//	CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
//		//	if (pResultSet)
//		//	{
//		//		while (pResultSet->Next())
//		//		{
//		//			nValidityStatus = pResultSet->GetInt("validitystatus");
//		//			nValidity = pResultSet->GetInt("validity");
//		//			nIsForcePassword = pResultSet->GetInt("isforcepassword");
//		//			unExpiredDate = pResultSet->GetInt("expireddate");
//		//			bRet = true;
//		//		}
//		//		delete pResultSet;
//		//		pResultSet = NULL;
//		//	}
//		//}
//
//		//if (!bRet || (unExpiredDate != 0 && unExpiredDate - time(NULL) <= 0))  //domain no found or domain is time expired
//		//{
//		//	pDBManger->RelDBConn(pDBConn);
//		//	user.set_status(IM::BaseDefine::REFUSE_REASON_INVALID_DOMAIN_USER);
//		//	return bRet;
//		//}
//
//		bRet = false; //reset returen value
//		//user.set_status(nStatus); //invalid user name
//
//		//string strSql = "select a.*,aliasname from IMUser a left join UserAlias b on a.useruuid=b.useruuid where a.username='" + strName1 + "' or b.aliasname ='" + strName1 + "' order by a.updated desc";
//		//string strSql = "select a.* from IMUser a left join UserAlias b on a.useruuid=b.useruuid where (a.username='" +  strName1 + "' or b.aliasname ='" + strName1 + "') and a.password = '" + strPass + "'";
//		//string strSql = "select a.* from IMUser a left join UserAlias b on a.useruuid=b.useruuid where (a.username='" + strName1 + "' or b.aliasname ='" + strName1 + "') and (a.password = '" + strPass + "' or a.tmppassword='" + strPass + "')";
//		//string strSql = "select * from IMUser where username='" + strName1 + "' and password = '" + strPass + "'";
//		string strSql = "select * from IMUser where username ='" + strName + "' or email ='" + strName + "' or phone='" + "' order by updated desc limit 1";
//		log("SQL for login: %s", strSql.c_str());
//		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
//		if (pResultSet)
//		{
//			uint32_t unGender = 2;//(unknown);
//			string strNickName = "", strAvatar = "", strRealName = "", strPhone = "", strSignature = "", strKeyword = "", strUserName = "", strPassword = "", strNickName = "";
//			string strEmail = "", strSalt = "";
//
//			while (pResultSet->Next())
//			{
//				unUserId = pResultSet->GetInt("id");
//
//				if (pResultSet->GetString("nick") != NULL)
//				{
//					strNickName = pResultSet->GetString("nick");
//				}
//
//				if (pResultSet->GetString("useruuid") != NULL)
//				{
//					strUserUuid = pResultSet->GetString("useruuid");
//				}
//
//				if (pResultSet->GetString("signature") != NULL)
//				{
//					strSignature = pResultSet->GetString("signature");
//				}
//
//				unGender = pResultSet->GetInt("sex");
//
//				if (pResultSet->GetString("name") != NULL)
//				{
//					strRealName = pResultSet->GetString("name");
//				}
//
//				if (strNickName == "")
//				{
//					strNickName = strRealName;
//				}
//
//				if (pResultSet->GetString("username") != NULL)
//				{
//					strUserName = pResultSet->GetString("username");
//				}
//
//				if (pResultSet->GetString("keyword") != NULL)
//				{
//					strKeyword = pResultSet->GetString("keyword");
//				}
//
//				if (pResultSet->GetString("phone") != NULL)
//				{
//					strPhone = pResultSet->GetString("phone");
//				}
//
//				if (pResultSet->GetString("password") != NULL)
//				{
//					strPassword = pResultSet->GetString("password");
//				}
//
//				int32_t nBirthday = pResultSet->GetInt("birthday");
//
//				if (pResultSet->GetString("avatar") != NULL)
//				{
//					strAvatar = pResultSet->GetString("avatar");
//				}
//
//				if (pResultSet->GetString("email") != NULL)
//				{
//					strEmail = pResultSet->GetString("email");
//				}
//
//				nStatus = pResultSet->GetInt("status");
//
//				user.set_user_id(unUserId);
//				user.set_user_uuid(strUserUuid);
//				user.set_user_name(strUserName);
//				user.set_user_real_name(strRealName);
//				user.set_avatar_url(strAvatar);
//				user.set_user_gender(unGender);
//				user.set_birthday(nBirthday);
//				user.set_mobile_phone(strPhone);
//				user.set_status(nStatus);
//				user.set_keyword(strKeyword);
//				user.set_user_nick_name(strNickName);
//				user.set_signature(strSignature);
//				user.set_email(strEmail);
//			}
//
//			uint32_t unIsInitPassword = pResultSet->GetInt("isinitpassword");
//			uint32_t unIsLock = pResultSet->GetInt("islock");
//			uint32_t unLastmodifyTime = pResultSet->GetInt("lastmodifytime");
//
//			if (nStatus == 1) //normal user
//			{
//				if (unIsLock == 1)
//				{
//					nStatus = LOGIN_RESULT_LOCKED_BY_EC;  //locked by ec
//					break;
//				}
//				else if (nIsForcePassword == 1 && unIsInitPassword == 1)
//				{
//					nStatus = LOGIN_RESULT_FORCE_CHG_PASSWORD;
//					//user.set_status(nStatus);
//					break;
//				}
//				//else if (strBuyProducts == "")
//				//{
//				//	nStatus = LOGIN_RESULT_NO_BUY_PRODUCT;
//				//	//user.set_status(nStatus);
//				//	break;
//				//}
//				else if (strPassword == strPass)
//				{
//					/*	if (strBuyProducts == "")
//					{
//					nStatus = LOGIN_RESULT_NO_BUY_PRODUCT;
//					}*/
//					if (nValidityStatus == 1)
//					{
//						time_t nTicks = time(NULL);
//						if (nTicks > unLastmodifyTime + nValidity * 24 * 3600)
//						{
//							nStatus = LOGIN_RESULT_EXPIRED_PASSWORD;
//						}
//					}
//					break; //added by george on Feb 23rd,2016 (handle 1 time if too much records)
//				}
//				else  //invalid password
//				{
//					//unUserID = 0;
//					nStatus = LOGIN_RESULT_INVALID_PASSWORD; //login by user name is default
//					if (strUserName == strName1) //login by user name
//					{
//						//user.set_status(nStatus); //invalid password
//						break;
//					}
//					else    //login by alias
//					{
//						nStatus = LOGIN_RESULT_LOGIN_BY_ALIAS_NAME;
//						//user.set_status(nStatus);
//						if (strAliasName == strName1)
//						{
//							break;
//						}
//					}
//				}
//			}
//			break;
//		}
//		delete  pResultSet;
//		pResultSet = NULL;
//	}
//
//	pDBManger->RelDBConn(pDBConn);
//
//	user.set_status(nStatus);
//
//	//if (unUserID != 0)  //get userid successfully
//	if (nStatus == 1)
//	{
//		//get company name
//		uint32_t nCompanyId = 0;
//		CUserModel::getInstance()->GetCompanyId(strUserUUID, nCompanyId);
//		string strCompanyName = CUserModel::getInstance()->getComanyName(nCompanyId);
//		user.set_companyname(strCompanyName);
//
//		list<IM::BaseDefine::DutyInfo> listDutyInfo;
//		map<string, list<IM::BaseDefine::PositionInfo>> mapPositionInfo;
//		CUserModel::getInstance()->GetDuty(strUserUUID, listDutyInfo, mapPositionInfo);
//
//		for (auto it = listDutyInfo.begin(); it != listDutyInfo.end(); it++) //get departments for a user
//		{
//			IM::BaseDefine::DutyInfo *pDuty = user.add_duty_list();
//			pDuty->set_depart_uuid(it->depart_uuid());
//			pDuty->set_depart_name(it->depart_name());
//			pDuty->set_status(it->status());
//
//			auto it1 = mapPositionInfo.find(it->depart_uuid());
//			if (it1 != mapPositionInfo.end())
//			{
//				for (auto it2 = it1->second.begin(); it2 != it1->second.end(); it2++) //get position for a department
//				{
//					IM::BaseDefine::PositionInfo *pPosition = pDuty->add_position_list();
//					pPosition->set_position_uuid(it2->position_uuid());
//					pPosition->set_position_type(it2->position_type());
//					pPosition->set_position_name(it2->position_name());
//					pPosition->set_status(it2->status());
//					pPosition->set_sort(it2->sort());
//				}
//			}
//		}
//
//		//get additional information for a user
//		list<IM::BaseDefine::UserDetails> listUserAddInfo;
//		CUserModel::getInstance()->GetUserAdditionalInfo(strUserUUID, listUserAddInfo);
//		for (auto it = listUserAddInfo.begin(); it != listUserAddInfo.end(); it++)
//		{
//			IM::BaseDefine::UserDetails *pAddInfo = user.add_add_info_list();
//			pAddInfo->set_details_uuid(it->details_uuid());
//			pAddInfo->set_details_type(it->details_type());
//			pAddInfo->set_details_title(it->details_title());
//			pAddInfo->set_details_content(it->details_content());
//		}
//
//		//get black list
//		list<IM::BaseDefine::ShieldUser> lsShieldUser;
//		CUserModel::getInstance()->GetUserShieldStatus(unUserID, lsShieldUser);
//		//log("Black List for %d: %d", nId, lsShieldUser.size());
//		for (auto it = lsShieldUser.begin(); it != lsShieldUser.end(); it++)
//		{
//			IM::BaseDefine::ShieldUser* pShieldUser = user.add_black_list();
//			pShieldUser->set_user_id(it->user_id());
//			pShieldUser->set_shield_status(it->shield_status());
//			//log("peerid: %d, shield status: %d", it->user_id(), it->shield_status());
//		}
//	}
//
//	if (nStatus > 0) //-1:deleted;0:disabled;1:enable
//	{
//		bRet = true;
//	}
//}
//	else
//	{
//		log("no db connection for teamtalk_slave");
//	}
//	return bRet;
//}



// return 0 means invalid ip here
unsigned long  CInterLoginStrategy::StringIp2Long(string& strIp)
{
	uint32_t nIp[4];
	memset(nIp, 0, sizeof(nIp));
	unsigned long ulIpValue = 0;
	int nCount = sscanf(strIp.c_str(), "%d.%d.%d.%d", nIp, nIp + 1, nIp + 2, nIp + 3);
	if (nCount == 4 && nIp[0] <= 255 && nIp[1] <= 255 && nIp[2] <= 255 && nIp[3] <= 255)
	{
		ulIpValue = nIp[0] << 24 | nIp[1] << 16 | nIp[2] << 8 | nIp[3];
	}
	return ulIpValue;
}


bool CInterLoginStrategy::IsLimitedIp(string& strIp, list<string>& lsLimitIpList)
{
	bool bValidIp = false;
	unsigned long ulCurrIp = StringIp2Long(strIp);

	for (auto it = lsLimitIpList.begin(); it != lsLimitIpList.end(); it++)
	{
		string strLimitIp = *it;
		log("Limited IP/Segment: %s", strLimitIp.c_str());
		int nCurrentPos = strLimitIp.find("~");
		if (nCurrentPos != string::npos)
		{
			string strStartIp = strLimitIp.substr(0, nCurrentPos);
			string strEndIp = strLimitIp.substr(nCurrentPos + 1);
			unsigned long ulStartLimitIp = StringIp2Long(strStartIp);
			unsigned long ulEndLimitIp = StringIp2Long(strEndIp);
			if (ulCurrIp >= ulStartLimitIp && ulCurrIp <= ulEndLimitIp)
			{
				bValidIp = true;
				break;
			}
		}
		else
		{
			unsigned long ulLimitIp = StringIp2Long(strLimitIp);
			if (ulCurrIp == ulLimitIp)
			{
				bValidIp = true;
				break;
			}
		}
	}
	return bValidIp;
}


int32_t CInterLoginStrategy::Authenticate(const string& strUserPassword, const string& strUserUuid, uint32_t& unInitPassword, uint32_t& unLastUpdateTime)
{
	string strSalt = "";
	string strDbPassword = "";
	uint32_t unIsActived = 1;
	int32_t nStatus = 1;
	uint32_t unIsLocked = 0;

	if (m_strHost == "")
	{
		//read config from server
		CConfigFileReader configFile("serverconfig.conf");
		char* pHttpHost = configFile.GetConfigName("HttpHost");
		if (pHttpHost)
		{
			m_strHost = pHttpHost;
		}
	}

	if (m_strHost != "")
	{
		string strPostData = "userId=";
		strPostData.append(strUserUuid);

		CHttpClient HttpClient;
		string strAddiHeader = "";
		string strToken = "";

		HttpClient.GetAdditionalInfo(strToken, strAddiHeader);
		string strPath = "/rs/s/ec/userHelper/getUser?";
	
		string strActualPath = m_strHost + strPath + strPostData + "&rs_token=" + strToken;
		string strResponse;

		HttpClient.Post(strActualPath, "", strResponse, strAddiHeader);
		log("URL: %s, Header: %s, Response: %s", strActualPath.c_str(), strAddiHeader.c_str(), strResponse.c_str());
		if (strResponse != "")
		{
			try
			{
				Json::Reader Reader;
				Json::Value Root;

				if (Reader.parse(strResponse, Root))
				{
					if (!Root["password"].isNull() && Root["password"].isString())
					{
						strDbPassword = Root["password"].asString();
					}

					if (!Root["salt"].isNull() && Root["salt"].isString())
					{
						strSalt = Root["salt"].asString();
					}

					if (!Root["isActivate"].isNull() && Root["isActivate"].isInt())
					{
						unIsActived = Root["isActivate"].asInt();
					}

					if (!Root["status"].isNull() && Root["status"].isInt())
					{
						nStatus = Root["status"].asInt();
					}

					if (!Root["isLock"].isNull() && Root["isLock"].isInt())
					{
						unIsLocked = Root["isLock"].asInt();
					}

					if (!Root["lastModifiedTime"].isNull() && Root["lastModifiedTime"].isInt64())
					{
						unLastUpdateTime = Root["lastModifiedTime"].asInt64() / 1000;
					}

					if (!Root["isInitPassword"].isNull() && Root["isInitPassword"].isInt())
					{
						unInitPassword = Root["isInitPassword"].asInt();
					}
				}
			}
			catch (std::runtime_error& msg)
			{
				log("json parse failed");
			}
		}
	}
	else
	{
		log("unknow http host");
	}

	int32_t nResult = LOGIN_RESULT_INVALID_PASSWORD;
	char szPassword[36];
	memset(szPassword, 0, sizeof(szPassword));
	string strOrigPassword = strUserPassword + "{" + strSalt + "}";
	MD5_Calculate(strOrigPassword.c_str(), strOrigPassword.length(), szPassword);
	log("passed password:%s, salt:%s, user password: %s, password: %s", strUserPassword.c_str(), strSalt.c_str(), szPassword, strDbPassword.c_str());
	if (strDbPassword == szPassword)
	{
		nResult = 0;
		if (!unIsActived)
		{
			nResult = LOGIN_RESULT_INACTIVE_USER;
		}
		else if (nStatus == 0)
		{
			nResult = LOGIN_RESULT_DISABLED_USER;
		}
		else if (nStatus == -1)
		{
			nResult = -1; //deleted user
		}
		else if (unIsLocked)
		{
			nResult = LOGIN_RESULT_LOCKED_BY_EC;
		}
	}

	return nResult;
}
