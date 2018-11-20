/*================================================================
*     Copyright (c) 2015年 lanhu. All rights reserved.
*   
*   文件名称：UserModel.cpp
*   创 建 者：Zhang Yuanhao
*   邮    箱：bluefoxah@gmail.com
*   创建日期：2015年01月05日
*   描    述：
*
================================================================*/
#include "UserModel.h"
#include "../DBPool.h"
#include "../CachePool.h"
#include "Common.h"
#include "Base64.h"
#include "HttpClient.h"
#include "SessionModel.h"
#include "ConfigFileReader.h"


CUserModel* CUserModel::m_pInstance = NULL;
string CUserModel::m_strHost = "";

CUserModel::CUserModel()
{

}

CUserModel::~CUserModel()
{
    
}

CUserModel* CUserModel::getInstance()
{
    if(m_pInstance == NULL)
    {
        m_pInstance = new CUserModel();
    }
    return m_pInstance;
}

//void CUserModel::getChangedId(uint32_t& nLastTime, list<uint32_t> &lsIds)
void CUserModel::getChangedId(uint32_t& nLastTime, list<uint32_t>& lsIds, uint32_t nCompanyID)
{
    CDBManager* pDBManager = CDBManager::getInstance();
    CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
    if (pDBConn)
    {
        string strSql ;
        if(nLastTime == 0)
        {
			strSql = "select a.id as uid, max(a.updated) as maxupdated1,max(b.updated) as maxupdated2 from IMUser a left join IMUserPosition b on a.useruuid=b.useruuid where (a.status <> -1  and a.companyid = " + int2string(nCompanyID) + " and b.status = 0) or a.companyid=0 group by a.id";
			//strSql = "select a.id as uid, max(a.updated) as maxupdated1,max(b.updated) as maxupdated2 from IMUser a, IMUserPosition b where (a.status <> -1  and b.companyid = " + int2string(nCompanyID) + " and a.useruuid=b.useruuid and b.status = 0) or a.companyid=0 group by a.id";
			//strSql = "select id, updated from IMUser where status != 3";
            //strSql = "select id, updated from IMUser where status != 3";
        }
        else
        {
			//strSql = "select distinct a.id, a.updated from IMUser a, IMUserPosition b where a.updated>=" + int2string(nLastTime) + " and b.companyid = " + int2string(nCompanyID) + " and a.id=b.userid";
			//strSql = "select a.id, max(a.updated) as maxupdated1,max(b.updated) as maxupdated2 from IMUser a, IMUserPosition b where (a.updated>=" + int2string(nLastTime) + " or b.updated >= " + int2string(nLastTime) +  ") and b.companyid = " + int2string(nCompanyID) + " and a.id=b.userid and (a.status <> 0 or b.status = 0)  group by a.id";
			//either user or position changes,we shoud tell client the changes,updated by george on Oct 27th,2015 
			//strSql = "select a.id as uid, max(a.updated) as maxupdated1,max(b.updated) as maxupdated2 from IMUser a, IMUserPosition b where (a.updated>=" + int2string(nLastTime) + " or b.updated >= " + int2string(nLastTime) + ") and ((a.companyid = " + int2string(nCompanyID) + " and a.useruuid=b.useruuid) or a.companyid = 0) group by a.id";
			strSql = "select a.id as uid, max(a.updated) as maxupdated1,max(b.updated) as maxupdated2 from IMUser a left join IMUserPosition b on a.useruuid=b.useruuid where (a.updated>=" + int2string(nLastTime) + " or b.updated >= " + int2string(nLastTime) + ") and (a.companyid = " + int2string(nCompanyID) + " or a.companyid = 0) group by a.id";

        }
		log("SQL for getting changed ids: %s", strSql.c_str());
        CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
        if(pResultSet)
        {
            while (pResultSet->Next())
			{
                uint32_t nId = pResultSet->GetInt("uid");
                uint32_t nUpdated1 = pResultSet->GetInt("maxupdated1");
				uint32_t nUpdated2 = 0;
				if (pResultSet->GetString("maxupdated2") != NULL)
				{
					nUpdated2 = pResultSet->GetInt("maxupdated2");
				}

                if(nLastTime < nUpdated1)
                {
                    nLastTime = nUpdated1;
                }

				if (nLastTime < nUpdated2)
				{
					nLastTime = nUpdated2;
				}

                lsIds.push_back(nId);
            }
            delete pResultSet;
        }
        else
        {
            log(" no result set for sql:%s", strSql.c_str());
        }
        pDBManager->RelDBConn(pDBConn);
    }
    else
    {
        log("no db connection for teamtalk_slave");
    }
}

//void CUserModel::getUsers(list<uint32_t> lsIds, list<IM::BaseDefine::UserInfo> &lsUsers)
//{
//    if (lsIds.empty()) {
//        log("list is empty");
//        return;
//    }
//    CDBManager* pDBManager = CDBManager::getInstance();
//    CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
//    if (pDBConn)
//    {
//        string strClause;
//        bool bFirst = true;
//        for (auto it = lsIds.begin(); it!=lsIds.end(); ++it)
//        {
//            if(bFirst)
//            {
//                bFirst = false;
//                strClause += int2string(*it);
//            }
//            else
//            {
//                strClause += ("," + int2string(*it));
//            }
//        }
//
//        string  strSql = "select * from IMUser where id in (" + strClause + ")";
//		log(" select IMUser sql:%s", strSql.c_str());
//        CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
//        if(pResultSet)
//        {
//            while (pResultSet->Next())
//            {
//                IM::BaseDefine::UserInfo cUser;
//
//				int32_t nStatus = -1;
//				uint32_t unUserID = 0, unGender = 2, unContactStatus = 0;//(unknown);
//				string strUserUUID, strNickName, strAvatar, strRealName, strPhone, strUserName, strSignature, strJobNumber, strBirthday, strBuyProducts, strKeyword;
//
//				if (pResultSet->GetString("jobnumber") == NULL)
//				{
//					strJobNumber = "";
//				}
//				else
//				{
//					strJobNumber = pResultSet->GetString("jobnumber");
//				}
//
//				unUserID = pResultSet->GetInt("id");
//
//				if (pResultSet->GetString("useruuid") == NULL)
//				{
//					strUserUUID = "";
//				}
//				else
//				{
//					strUserUUID = pResultSet->GetString("useruuid");
//				}
//
//				if (pResultSet->GetString("signature") == NULL)
//				{
//					strSignature = "";
//				}
//				else
//				{
//					strSignature = pResultSet->GetString("signature");
//				}
//
//				unGender = pResultSet->GetInt("sex");
//
//				if (pResultSet->GetString("name") == NULL)
//				{
//					strRealName = "";
//				}
//				else
//				{
//					strRealName = pResultSet->GetString("name");
//				}
//
//				if (pResultSet->GetString("username") == NULL)
//				{
//					strUserName = "";
//				}
//				else
//				{
//					strUserName = pResultSet->GetString("username");
//				}
//
//				if (pResultSet->GetString("nick") == NULL)
//				{
//					strNickName = "";
//				}
//				else
//				{
//					strNickName = pResultSet->GetString("nick");
//				}
//
//				if (strNickName == "")
//				{
//					strNickName = strRealName;
//				}
//
//				if (pResultSet->GetString("keyword") == NULL)
//				{
//					strKeyword = "";
//				}
//				else
//				{
//					strKeyword = pResultSet->GetString("keyword");
//				}
//
//				if (pResultSet->GetString("phone") == NULL)
//				{
//					strPhone = "";
//				}
//				else
//				{
//					strPhone = pResultSet->GetString("phone");
//				}
//
//				/*if (pResultSet->GetString("birthday") == NULL)
//				{
//					strBirthday = "";
//				}
//				else
//				{
//					strBirthday = pResultSet->GetString("birthday");
//				}*/
//				int32_t nBirthday = pResultSet->GetInt("birthday");
//
//				if (pResultSet->GetString("avatar") == NULL)
//				{
//					strAvatar = "";
//				}
//				else
//				{
//					strAvatar = pResultSet->GetString("avatar");
//					//string strAvatar1 = pResultSet->GetString("avatar");
//					//if (strAvatar1.find("http://") == string::npos)
//					//{
//					//	int nPos = strUserName.find("@");
//					//	if (nPos != string::npos)
//					//	{
//					//		//strAvatar = "http://ioa." + strUserName.substr(nPos + 1) + "/fs/file/previewPicture?fileId=" + strAvatar1;
//					//		strAvatar = "http://ioa." + strUserName.substr(nPos + 1) + "/fs/file/previewPicturePublic?fileId=" + strAvatar1; 
//					//	}
//					//	else
//					//	{
//					//		strAvatar = strAvatar1;
//					//	}
//					//}
//					//else
//					//{
//					//	strAvatar = strAvatar1;
//					//}
//				}
//
//				//appened by george on Nov 12th,2015
//				if (pResultSet->GetString("buyproduct") == NULL)
//				{
//					strBuyProducts = "";
//				}
//				else
//				{
//					strBuyProducts = pResultSet->GetString("buyproduct");
//				}
//
//				nStatus = pResultSet->GetInt("status");
//				unContactStatus = pResultSet->GetInt("contactstatus");
//
//				//get company name
//				uint32_t nCompanyId = 0;
//				CUserModel::getInstance()->GetCompanyId(strUserUUID, nCompanyId);
//				string strCompanyName = CUserModel::getInstance()->getComanyName(nCompanyId);
//
//				cUser.set_user_id(unUserID);
//				cUser.set_user_uuid(strUserUUID);
//				cUser.set_job_number(strJobNumber);
//				cUser.set_user_name(strUserName);
//				cUser.set_user_real_name(strRealName);
//				cUser.set_avatar_url(strAvatar);
//				cUser.set_user_gender(unGender);
//				//cUser.set_birthday(strBirthday);
//				cUser.set_birthday(nBirthday);
//				cUser.set_mobile_phone(strPhone);
//				cUser.set_contact_status(unContactStatus);
//				cUser.set_status(nStatus);
//				cUser.set_keyword(strKeyword);
//				cUser.set_user_nick_name(strNickName);
//				cUser.set_signature(strSignature);
//				cUser.set_companyname(strCompanyName);
//				cUser.set_buyproduct(strBuyProducts);
//
//				list<IM::BaseDefine::DutyInfo> listDutyInfo;
//				map<string, list<IM::BaseDefine::PositionInfo>> mapPositionInfo;
//				CUserModel::getInstance()->GetDuty(strUserUUID, listDutyInfo, mapPositionInfo);
//
//				for (auto it = listDutyInfo.begin(); it != listDutyInfo.end(); it++) //get departments for a user
//				{
//					IM::BaseDefine::DutyInfo *pDuty = cUser.add_duty_list();
//					pDuty->set_depart_uuid(it->depart_uuid());
//					pDuty->set_depart_name(it->depart_name());
//					pDuty->set_status(it->status());
//
//					auto it1 = mapPositionInfo.find(it->depart_uuid());
//					if (it1 != mapPositionInfo.end())
//					{
//						for (auto it2 = it1->second.begin(); it2 != it1->second.end(); it2++) //get position for a department
//						{
//							IM::BaseDefine::PositionInfo *pPosition = pDuty->add_position_list();
//							pPosition->set_position_uuid(it2->position_uuid());
//							pPosition->set_position_type(it2->position_type());
//							pPosition->set_position_name(it2->position_name());
//							pPosition->set_status(it2->status());
//							pPosition->set_sort(it2->sort());
//						}
//					}
//				}
//
//				//get additional information for a user
//				list<IM::BaseDefine::UserDetails> listUserAddInfo;
//				CUserModel::getInstance()->GetUserAdditionalInfo(strUserUUID, listUserAddInfo);
//				for (auto it = listUserAddInfo.begin(); it != listUserAddInfo.end(); it++)
//				{
//					IM::BaseDefine::UserDetails *pAddInfo = cUser.add_add_info_list();
//					pAddInfo->set_details_uuid(it->details_uuid());
//					pAddInfo->set_details_type(it->details_type());
//					pAddInfo->set_details_title(it->details_title());
//					pAddInfo->set_details_content(it->details_content());
//				}
//
//				//get black list
//				list<IM::BaseDefine::ShieldUser> lsShieldUser;
//				CUserModel::getInstance()->GetUserShieldStatus(unUserID, lsShieldUser);
//				//log("Black List for %d: %d", nId, lsShieldUser.size());
//				for (auto it = lsShieldUser.begin(); it != lsShieldUser.end(); it++)
//				{
//					IM::BaseDefine::ShieldUser* pShieldUser = cUser.add_black_list();
//					pShieldUser->set_user_id(it->user_id());
//					pShieldUser->set_shield_status(it->shield_status());
//					//log("peerid: %d, shield status: %d", it->user_id(), it->shield_status());
//				}
//
//                lsUsers.push_back(cUser);
//            }
//            delete pResultSet;
//        }
//        else
//        {
//            log(" no result set for sql:%s", strSql.c_str());
//        }
//        pDBManager->RelDBConn(pDBConn);
//    }
//    else
//    {
//        log("no db connection for teamtalk_slave");
//    }
//}

bool CUserModel::getUserInfo(uint32_t nUserId, string& strNickName, string& strAvatar)
{
    bool bRet = false;
    CDBManager* pDBManager = CDBManager::getInstance();
    CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
    if (pDBConn)
    {
		string strUserName = "";
        string strSql = "select username,name,avatar from IMUser where id="+int2string(nUserId);
		//log("SQL for getting user info: %s", strSql.c_str());
        CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
        if(pResultSet)
        {
            while (pResultSet->Next())
            {
				if (pResultSet->GetString("name") == NULL)
				{
					strNickName = "";
				}
				else
				{
					strNickName = pResultSet->GetString("name");
				}

				if (pResultSet->GetString("username") == NULL)
				{
					strUserName = "";
				}
				else
				{
					strUserName = pResultSet->GetString("username");
				}

				if (pResultSet->GetString("avatar") == NULL)
				{
					strAvatar = "";
				}
				else
				{
					strAvatar = pResultSet->GetString("avatar");
				}

                bRet = true;
            }
            delete pResultSet;
        }
        else
        {
            log("no result set for sql:%s", strSql.c_str());
        }
        pDBManager->RelDBConn(pDBConn);
    }
    else
    {
        log("no db connection for teamtalk_slave");
    }
    return bRet;
}

bool CUserModel::updateUser(DBUserInfo_t &cUser,uint32_t nNow)
{
    bool bRet = false;
    CDBManager* pDBManager = CDBManager::getInstance();
    CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
    if (pDBConn)
    {
        uint32_t nNow = (uint32_t)time(NULL);
        string strSql = "update IMUser set `sex`= '" + int2string(cUser.nSex) + "', `signature`='"
			+ cUser.strSignature + "', `phone`='" + cUser.strTel +
			"', `telephone`='" + cUser.strPhone + "', `faxnum`='" 
			+ cUser.strFaxNum + "', `postcode`='" 
			+ cUser.strPostCode + "', `address`='" 
			+ cUser.strAddress+ "', `birthday`='" 
			+ cUser.strBirthday + "', `avatar`='" 
			+ cUser.strAvatar + "', `updated`=" 
			+ int2string(nNow) + " where id="+int2string(cUser.nId);
        bRet = pDBConn->ExecuteUpdate(strSql.c_str());
        if(!bRet)
        {
            log("update failed:%s", strSql.c_str());
        }
        pDBManager->RelDBConn(pDBConn);
    }
    else
    {
        log("no db connection for teamtalk_master");
    }
    return bRet;
}


bool CUserModel::updateUser(IM::BaseDefine::UserInfo& cUser, uint32_t nNow)
{
	bool bRet = false;
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
	if (pDBConn)
	{
		uint32_t nNow = (uint32_t)time(NULL);
		string strSql = "update IMUser set `sex`= " + int2string(cUser.user_gender()) + ", `signature`='"	
			//+ cUser.signature() + "', `birthday`='"
			//+ cUser.birthday() + "', `avatar`='"
			//+ cUser.avatar_url() + "', `updated`="
			+ cUser.signature() + "', `birthday`="
			+ int2string(cUser.birthday()) + ", `avatar`='"
			+ cUser.avatar_url() + "', `updated`="
			+ int2string(nNow) + " where id=" + int2string(cUser.user_id());
		bRet = pDBConn->ExecuteUpdate(strSql.c_str());
		log("SQL for updating msg: %s", strSql.c_str());
		if (!bRet)
		{
			log("update failed:%s", strSql.c_str());
		}

		//update additional information
		int nCount = cUser.add_info_list_size();
		for (int i = 0; i < nCount; i++)
		{
			IM::BaseDefine::UserDetails  Details = cUser.add_info_list(i);
			bool bExist = false;
			strSql = "select useruuid from UserDetails where uuid='" + Details.details_uuid() + "'";
			CResultSet *pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
			if (pResultSet)
			{
				while (pResultSet->Next())
				{
					bExist = true;
				}
				delete pResultSet;
				pResultSet = NULL;
			}

			if (bExist)
			{
				strSql = "update UserDetails set content = '" + Details.details_content() + "',updated=unix_timestamp() where uuid='" + Details.details_uuid() + "'";
				log("SQL for updating userdetails: %s", strSql.c_str());
				bRet = pDBConn->ExecuteUpdate(strSql.c_str());
			}
			else
			{
				strSql = "insert into UserDetails(uuid,useruuid,type,title,content,created,updated) values(?,?,?,?,?,?,?)";
				CPrepareStatement* stmt = new CPrepareStatement();
				if (stmt->Init(pDBConn->GetMysql(), strSql))
				{
					uint32_t nNow = (uint32_t)time(NULL);
					uint32_t index = 0;
					uint32_t nType = Details.details_type();
					stmt->SetParam(index++, Details.details_uuid());
					stmt->SetParam(index++, cUser.user_uuid());
					stmt->SetParam(index++, nType);
					stmt->SetParam(index++, Details.details_title());
					stmt->SetParam(index++, Details.details_content());
					stmt->SetParam(index++, nNow);
					stmt->SetParam(index++, nNow);
					bRet = stmt->ExecuteUpdate();

					if (!bRet)
					{
						log("insert user failed: %s", strSql.c_str());
					}
				}
				delete stmt;
			}
		}
		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_master");
	}

	if (bRet)
	{
		string strAvatar = "";
		//SyncUserInfo(cUser.user_id(), strAvatar);
	}
	return bRet;
}


bool CUserModel::updateNagure(uint32_t nUserId,std::string strNagure,uint32_t nNow )
{
	bool bRet = false;
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
	if (pDBConn)
	{

		string strSql = "update IMUser set `signature`= '" + strNagure+ "', `updated`="+int2string(nNow) + " where id="+int2string(nUserId);
		bRet = pDBConn->ExecuteUpdate(strSql.c_str());
		if(!bRet)
		{
			log("update failed:%s", strSql.c_str());
		}
		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_master");
	}

	if (bRet)
	{
		string strAvatar = "";
		//SyncUserInfo(nUserId, strAvatar);
	}
	return bRet;
}


std::string CUserModel::getComanyName(uint32_t nCompanyId)
{
	std::string strCompanyName;
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		string strSql = "select companyName  from IMCompany where id="+int2string(nCompanyId);
		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if(pResultSet)
		{
			while (pResultSet->Next())
			{
				strCompanyName = pResultSet->GetString("companyName");
			}
			delete pResultSet;
		}
		else
		{
			log("no result set for sql:%s", strSql.c_str());
		}
		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_slave");
	}
	return strCompanyName;
}


bool CUserModel::GetCompanyId(uint32_t nUserId,uint32_t &nCompanyId)
{
	bool bRet = false;
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		//string strSql = "select companyid from IMUser a, IMUserPosition b where a.id="+int2string(nUserId) + " and a.useruuid=b.useruuid";
		string strSql = "select companyid from IMUser  where id=" + int2string(nUserId);
		log("SQL for getting company id: %s", strSql.c_str());
		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		
		if(pResultSet)
		{
			while (pResultSet->Next())
			{
				nCompanyId = pResultSet->GetInt("companyid");
			}
			delete pResultSet;
		}
		else
		{
			log("no result set for sql:%s", strSql.c_str());
		}
		pDBManager->RelDBConn(pDBConn);


	/*	string strSql = "select departId from IMUser where id=" + int2string(nUserId);
		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		uint32_t nDepartId;

		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				nDepartId = pResultSet->GetInt("departId");
			}
			delete pResultSet;
			strSql = "select companyid from IMDepart where id=" + int2string(nDepartId);
			pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
			if (pResultSet)
			{
				while (pResultSet->Next())
				{
					nCompanyId = pResultSet->GetInt("companyid");
					bRet = true;
				}
				delete pResultSet;
			}
			else
			{
				log("no result set for sql:%s", strSql.c_str());
			}
		}
		else
		{
			log("no result set for sql:%s", strSql.c_str());
		}
		pDBManager->RelDBConn(pDBConn);*/
	}
	else
	{
		log("no db connection for teamtalk_slave");
	}

	return bRet;
}


bool CUserModel::GetCompanyId(string& strUserID, uint32_t &nCompanyId)
{
	bool bRet = false;
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		//string strSql = "select companyid from IMUserPosition where useruuid='" + strUserID + "'";
		string strSql = "select companyid from IMUser where useruuid='" + strUserID + "'";
		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		//log("SQL for getting company; %s", strSql.c_str());

		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				nCompanyId = pResultSet->GetInt("companyid");
			}
			delete pResultSet;
		}
		else
		{
			log("no result set for sql:%s", strSql.c_str());
		}
		pDBManager->RelDBConn(pDBConn);


		/*	string strSql = "select departId from IMUser where id=" + int2string(nUserId);
		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		uint32_t nDepartId;

		if (pResultSet)
		{
		while (pResultSet->Next())
		{
		nDepartId = pResultSet->GetInt("departId");
		}
		delete pResultSet;
		strSql = "select companyid from IMDepart where id=" + int2string(nDepartId);
		pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
		while (pResultSet->Next())
		{
		nCompanyId = pResultSet->GetInt("companyid");
		bRet = true;
		}
		delete pResultSet;
		}
		else
		{
		log("no result set for sql:%s", strSql.c_str());
		}
		}
		else
		{
		log("no result set for sql:%s", strSql.c_str());
		}
		pDBManager->RelDBConn(pDBConn);*/
	}
	else
	{
		log("no db connection for teamtalk_slave");
	}

	return bRet;
}


//std::string CUserModel::getDepartName(uint32_t nDepartId,uint32_t &nCompanyId)
std::string CUserModel::getDepartName(string& strDepartId, uint32_t &nCompanyId)
{
	std::string strDepartName;
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		string strSql = "select companyid ,departName  from IMDepart where uuid='"+ strDepartId + "'";
		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if(pResultSet)
		{
			while (pResultSet->Next())
			{
				strDepartName = pResultSet->GetString("departName");
				nCompanyId = pResultSet->GetInt("companyid");
			}
			delete pResultSet;
		}
		else
		{
			log("no result set for sql:%s", strSql.c_str());
		}
		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_slave");
	}
	return strDepartName;
}


bool CUserModel::insertUser(DBUserInfo_t &cUser)
{
    bool bRet = false;
    CDBManager* pDBManager = CDBManager::getInstance();
    CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
    if (pDBConn)
    {
        string strSql = "insert into IMUser(`id`,`sex`,`nick`,`domain`,`name`,`phone`,`email`,`avatar`,`departId`,`status`,`created`,`updated`) values(?,?,?,?,?,?,?,?,?,?,?,?)";
        CPrepareStatement* stmt = new CPrepareStatement();
        if (stmt->Init(pDBConn->GetMysql(), strSql))
        {
            uint32_t nNow = (uint32_t) time(NULL);
            uint32_t index = 0;
            uint32_t nGender = cUser.nSex;
            uint32_t nStatus = cUser.nStatus;
            stmt->SetParam(index++, cUser.nId);
            stmt->SetParam(index++, nGender);
            stmt->SetParam(index++, cUser.strNick);
            stmt->SetParam(index++, cUser.strDomain);
            stmt->SetParam(index++, cUser.strName);
            stmt->SetParam(index++, cUser.strTel);
            stmt->SetParam(index++, cUser.strEmail);
            stmt->SetParam(index++, cUser.strAvatar);
            stmt->SetParam(index++, cUser.nDeptId);
            stmt->SetParam(index++, nStatus);
            stmt->SetParam(index++, nNow);
            stmt->SetParam(index++, nNow);
            bRet = stmt->ExecuteUpdate();
            
            if (!bRet)
            {
                log("insert user failed: %s", strSql.c_str());
            }
        }
        delete stmt;
        pDBManager->RelDBConn(pDBConn);
    }
    else
    {
        log("no db connection for teamtalk_master");
    }
    return bRet;
}


void CUserModel::clearUserCounter(uint32_t nUserId, uint32_t nPeerId, IM::BaseDefine::SessionType nSessionType)
{
    if(IM::BaseDefine::SessionType_IsValid(nSessionType))
    {
        CacheManager* pCacheManager = CacheManager::getInstance();
        CacheConn* pCacheConn = pCacheManager->GetCacheConn("unread");
        if (pCacheConn)
        {
            // Clear P2P msg Counter
            if(nSessionType == IM::BaseDefine::SESSION_TYPE_SINGLE)
            {
                int nRet = pCacheConn->hdel("unread_" + int2string(nUserId), int2string(nPeerId));
				log("clear unread message count for user %d ---->%d", nPeerId, nUserId);
                if(!nRet)
                {
                    log("hdel failed %d->%d", nPeerId, nUserId);
                }
            }
            // Clear Group msg Counter
            else if(nSessionType == IM::BaseDefine::SESSION_TYPE_GROUP)
            {
                string strGroupKey = int2string(nPeerId) + GROUP_TOTAL_MSG_COUNTER_REDIS_KEY_SUFFIX;
                map<string, string> mapGroupCount;
                bool bRet = pCacheConn->hgetAll(strGroupKey, mapGroupCount);

				for (auto it = mapGroupCount.begin(); it != mapGroupCount.end(); ++it)
				{
					log("CLEARKEY: %s, FIRST: %s, SECOND: %s", strGroupKey.c_str(), it->first.c_str(), it->second.c_str());
				}

                if(bRet)
                {
                    string strUserKey = int2string(nUserId) + "_" + int2string(nPeerId) + GROUP_USER_MSG_COUNTER_REDIS_KEY_SUFFIX;
                    string strReply = pCacheConn->hmset(strUserKey, mapGroupCount);
					log("clear unread messge count for user %d in group %d", nUserId, nPeerId);
                    if(strReply.empty()) {
                        log("hmset %s failed !", strUserKey.c_str());
                    }
					//reset lastatmsgid (user----peer(group id) frequently?
					CSessionModel *pSession = CSessionModel::getInstance();
					if (pSession)
					{
						pSession->ResetSessionLastAtMsgId(nUserId, nPeerId);
					}
                }
                else
                {
                    log("hgetall %s failed!", strGroupKey.c_str());
                }
                
            }
            pCacheManager->RelCacheConn(pCacheConn);
        }
        else
        {
            log("no cache connection for unread");
        }
    }
    else{
        log("invalid sessionType. userId=%u, fromId=%u, sessionType=%u", nUserId, nPeerId, nSessionType);
    }
}


bool CUserModel::updateUserAvatar(uint32_t nUserId, string& strNewAvatar)
{
	bool bRet = false;
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
	string strUserUuid = "";
	if (pDBConn)
	{
		string strSql = "update IMUser set `avatar`= '" + strNewAvatar + "' , updated=unix_timestamp() where id=" + int2string(nUserId);
		log("SQL for updating avatar: %s", strSql.c_str());
		bRet = pDBConn->ExecuteUpdate(strSql.c_str());
		if (!bRet)
		{
			log("update failed:%s", strSql.c_str());
		}

		//get useruuid
		strSql = "select useruuid from IMUser where id=" + int2string(nUserId);
		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				strUserUuid = pResultSet->GetString("useruuid");
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

	//do not sync user info when updating avatar,recomment on June 28th,2016
	if (bRet)
	{
		Json::Value Root;
		Root["userId"] = strUserUuid;
		Root["headPortrait"] = strNewAvatar;

		bRet = SyncInfoWithEc(Root);
	}
	return bRet;
}


void CUserModel::GetPosition(uint32_t nUserID, list<IM::BaseDefine::PositionInfo>& listPosition)
{
	;
	//std::string strDepartName;
	//CDBManager* pDBManager = CDBManager::getInstance();
	//CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	//if (pDBConn)
	//{
	//	//client do not display removed positions.updated by george on Oct 27th,2015
	//	string strSql = "select b.departid,departName,position,b.status from IMDepart a, IMUserPosition b where b.userid=" + int2string(nUserID) +  " and a.id = b.departid and b.status = 0";
	//	//log("SQL for getting position: %s", strSql.c_str()); commented by george on Dec 15th,2015
	//	CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
	//	if (pResultSet)
	//	{	
	//		while (pResultSet->Next())
	//		{
	//			IM::BaseDefine::PositionInfo  Position;
	//			Position.set_depart_id(pResultSet->GetInt("departid"));
	//			Position.set_depart_name(pResultSet->GetString("departName"));
	//			Position.set_position(pResultSet->GetString("position"));
	//			Position.set_status(pResultSet->GetInt("status"));
	//			listPosition.push_back(Position);
	//		}
	//		delete pResultSet;
	//	}
	//	else
	//	{
	//		log("no result set for sql:%s", strSql.c_str());
	//	}
	//	pDBManager->RelDBConn(pDBConn);
	//}
	//else
	//{
	//	log("no db connection for teamtalk_slave");
	//}
}


//bool CUserModel::IsValidUser(uint32_t nUserID)
//{
//	if (nUserID < 10001)
//	{
//		return true;
//	}
//
//	bool bReturn = false;
//	CacheManager* pCacheManager = CacheManager::getInstance();
//	CacheConn* pCacheConn = pCacheManager->GetCacheConn("user");
//	if (pCacheConn)
//	{
//		string strUserKey = "user_" + int2string(nUserID);
//		string strValue = pCacheConn->get(strUserKey);
//		if (!strValue.empty())
//		{
//			if (string2int(strValue) == 1)
//			{
//				bReturn = true;
//			}
//			else
//			{
//				log("%d is an invalid user:  %s", nUserID, strValue.c_str());
//			}
//		}
//		pCacheManager->RelCacheConn(pCacheConn);
//	}
//	return bReturn;
//}


int CUserModel::IsValidUser(uint32_t nUserID)
{
	if (nUserID < 10001)
	{
		return 1;
	}

	int nReturn = USER_MODEL_USER_NO_OPEN_IM; //unknow reason 
	CacheManager* pCacheManager = CacheManager::getInstance();
	CacheConn* pCacheConn = pCacheManager->GetCacheConn("user");
	if (pCacheConn)
	{
		string strUserKey = "user_" + int2string(nUserID);
		string strValue = pCacheConn->get(strUserKey);
		if (!strValue.empty())
		{
			nReturn = string2int(strValue);
			if (nReturn != 1)
			{
				log("%d is an invalid user:  %s", nUserID, strValue.c_str());
			}
		}
		pCacheManager->RelCacheConn(pCacheConn);
	}
	return nReturn;
}


bool CUserModel::setPush(uint32_t unUserId, uint32_t unPeerId, uint32_t unType, uint32_t unStatus)
{
	bool bRet = false;
	if (!IsValidUser(unPeerId))
	{
		log("user:%d is invalid user", unPeerId);
		return bRet;;
	}

	CacheManager* pCacheManager = CacheManager::getInstance();
	CacheConn* pCacheConn = pCacheManager->GetCacheConn("user_set");
	if (pCacheConn)
	{
		string strUserKey = "user_set_" + int2string(unUserId);
		string strField = int2string(unPeerId) + "_" + int2string(unType);
		int nRet = pCacheConn->hset(strUserKey, strField, int2string(unStatus));
		pCacheManager->RelCacheConn(pCacheConn);
		if (nRet != -1)
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


bool CUserModel::getPush(uint32_t unUserId, list<IM::BaseDefine::ShieldUser>& lsPush)
{
	bool bRet = false;
	CacheManager* pCacheManager = CacheManager::getInstance();
	CacheConn* pCacheConn = pCacheManager->GetCacheConn("user_set");
	if (pCacheConn)
	{
		string strUserKey = "user_set_" + int2string(unUserId);
		map<string, string> mapResult;
		bRet = pCacheConn->hgetAll(strUserKey, mapResult);
		pCacheManager->RelCacheConn(pCacheConn);
		if (bRet)
		{
			for (auto it = mapResult.begin(); it != mapResult.end(); ++it)
			{
				string strField = it->first;
				int nPos = strField.find("_");
				uint32_t unPeerID = 0;
				if (nPos != string::npos)
				{
					string strPeerID = strField.substr(0, nPos);
					unPeerID = string2int(strPeerID);
				}
	
				IM::BaseDefine::ShieldUser status;
				status.set_user_id(unPeerID);
				status.set_shield_status(string2int(it->second));
				lsPush.push_back(status);
			}
		}
		else
		{
			log("hgetall %s failed!", strUserKey.c_str());
		}
	}
	else
	{
		log("no cache connection for group_set");
	}
	return bRet;
}


bool CUserModel::SetShieldStatus(uint32_t unUserId, uint32_t unPeerId, uint32_t unStatus)
{
	bool bRet = false;
	if (!IsValidUser(unPeerId))
	{
		log("user:%d is an invalid user", unPeerId);
		return bRet;;
	}

	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		string strSql = "select shieldstatus from IMP2PTalk where userid=" + int2string(unUserId) + " and peerid=" + int2string(unPeerId);
		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		pDBManager->RelDBConn(pDBConn);

		log("SQL for getting shield user status: %s", strSql.c_str());
		if(pResultSet)  //the user ---> peer status already set
		{
			bool bFound = false;
			while (pResultSet->Next())
			{
				bFound = true;
				pDBConn = pDBManager->GetDBConn("teamtalk_master");
				if (pDBConn)
				{
					string strSql = "update IMP2PTalk set shieldstatus = " + int2string(unStatus) + ",updated=unix_timestamp() where userid = " + int2string(unUserId) + " and peerid= " + int2string(unPeerId); // +" and status = 0";
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
			}
			delete pResultSet;
			pResultSet = NULL;

			if (!bFound) //insert a new record
			{
				pDBConn = pDBManager->GetDBConn("teamtalk_master");
				if (pDBConn)
				{
					string strSql = "insert into IMP2PTalk(userid,peerid,shieldstatus,created,updated)values(?,?,?,?,?)";
					log("Sql for inserting user shield status: %s", strSql.c_str());
					CPrepareStatement* stmt = new CPrepareStatement();
					if (stmt->Init(pDBConn->GetMysql(), strSql))
					{
						uint32_t nNow = (uint32_t)time(NULL);
						uint32_t index = 0;
						stmt->SetParam(index++, unUserId);
						stmt->SetParam(index++, unPeerId);
						stmt->SetParam(index++, unStatus);
						stmt->SetParam(index++, nNow);
						stmt->SetParam(index++, nNow);
						bRet = stmt->ExecuteUpdate();

						if (!bRet)
						{
							log("insert user shield status failed: %s", strSql.c_str());
						}
					}

					delete stmt;
					stmt = NULL;
					pDBManager->RelDBConn(pDBConn);
				}
				else
				{
					log("no db connection for teamtalk_master");
				}
			}
		}
		else
		{
			log("no result set for sql:%s", strSql.c_str());
		}
	}
	else
	{
		log("no db connection for teamtalk_master");
	}

	return bRet;
}


void CUserModel::GetUserShieldStatus(uint32_t unUserId, list<IM::BaseDefine::ShieldUser>& listShieldStatus)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		string strSql = "select peerid,shieldstatus from IMP2PTalk where shieldstatus=1 and userid=" + int2string(unUserId);
		//log("SQL for getting shield user status: %s", strSql.c_str()); commented by george on Dec 15th,2015
		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)  //the user ---> peer status already set
		{
			while (pResultSet->Next())
			{
				IM::BaseDefine::ShieldUser UserStatus;
				UserStatus.set_user_id(pResultSet->GetInt("peerid"));
				UserStatus.set_shield_status(pResultSet->GetInt("shieldstatus"));
				listShieldStatus.push_back(UserStatus);
			}

			delete pResultSet;
			pResultSet = NULL;
		}

		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_slave");
	}
}


void CUserModel::GetDuty(string& strUserUuid, list<IM::BaseDefine::DutyInfo>& listDuty, map<string, list<string>>& mapPosition, uint32_t unCompanyId)
{
	//string strDepartUuid = "", strPrevDepartUuid = "";
	string strDepartUuid = "";
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		////client do not display removed positions.updated by george on Oct 27th,2015
		//string strSql = "select a.uuid as uuid1, b.uuid as uuid2, jobgrade from IMDepart a, IMPosition b, IMUserPosition c where a.uuid = c.departuuid and b.uuid = c.posuuid ";		
		//strSql += " and c.useruuid='" + strUserUuid + "' and c.companyid=" + int2string(unCompanyId) + " and c.status = 0 order by c.departuuid";
		//log("SQL for getting duty: %s", strSql.c_str()); //commented by george on Jan 15th,2016

		//get department and jobgrade
		string strSql = "select departuuid, jobgrade,posuuid from IMUserPosition where useruuid='" + strUserUuid + "'and companyid=" + int2string(unCompanyId) + " and status=0 order by departuuid";
		//strSql += " and posuuid= '' and status = 0";
		log("SQL for getting depart and position: %s", strSql.c_str()); 
		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				strDepartUuid = pResultSet->GetString("departuuid"); //get previous depart uuid
				string strPosUuid = "";
				if (pResultSet->GetString("posuuid") != NULL)
				{
					strPosUuid = pResultSet->GetString("posuuid");
				}

				if (strPosUuid == "00000000000000000000000000000000") //no position
				{
					string strJobGrade = "";
					if (pResultSet->GetString("jobgrade") != NULL)
					{
						strJobGrade = pResultSet->GetString("jobgrade");
					}
					uint32_t unJobGrade = GetJobGrade(strJobGrade);

					IM::BaseDefine::DutyInfo Duty;
					Duty.set_depart_uuid(strDepartUuid);
					Duty.set_job_grade((IM::BaseDefine::JobGradeType)unJobGrade);
					listDuty.push_back(Duty);
				}
				else  //position found
				{
					auto it = mapPosition.find(strDepartUuid);
					if (it != mapPosition.end())
					{
						it->second.push_back(strPosUuid);
					}
					else
					{
						list<string> lsPosition;
						lsPosition.push_back(strPosUuid);
						mapPosition.insert(make_pair(strDepartUuid, lsPosition));
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

		pDBManager->RelDBConn(pDBConn);

		//if (pResultSet)
		//{
		//	while (pResultSet->Next())
		//	{
		//		strPrevDepartUuid = pResultSet->GetString("uuid1"); //get previous depart uuid
		//		if (strDepartUuid != strPrevDepartUuid)
		//		{
		//			strDepartUuid = strPrevDepartUuid;
		//			IM::BaseDefine::DutyInfo Duty;
		//			Duty.set_depart_uuid(strDepartUuid);
		//			string strJobGrade = "";
		//			if (pResultSet->GetString("jobgrade") != NULL)
		//			{
		//				strJobGrade = pResultSet->GetString("jobgrade");
		//			}
		//			uint32_t unJobGrade = GetJobGrade(strJobGrade);
		//			Duty.set_job_grade((IM::BaseDefine::JobGradeType)unJobGrade);
		//			listDuty.push_back(Duty);
		//		}

		//		string strPosUuid = "";
		//		if (pResultSet->GetString("uuid2") != NULL)
		//		{
		//			strPosUuid = pResultSet->GetString("uuid2");
		//		}

		//		auto it = mapPosition.find(strPrevDepartUuid);
		//		if (it != mapPosition.end())
		//		{
		//			it->second.push_back(strPosUuid);
		//		}
		//		else
		//		{
		//			list<string> lsPosition;
		//			lsPosition.push_back(strPosUuid);
		//			mapPosition.insert(make_pair(strPrevDepartUuid, lsPosition));
		//		}
		//	}

		//	delete pResultSet;
		//	pResultSet = NULL;
		//}
		//else
		//{
		//	log("no result set for sql:%s", strSql.c_str());
		//}
		//pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_slave");
	}
}


void CUserModel::GetUserAdditionalInfo(string& strUserUUID, list<IM::BaseDefine::UserDetails>& listAddiInfo, uint32_t nType)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		//get additional information for a user
		string strSql = "";
		if (nType == 1)
		{
			strSql = "select * from UserDetails where useruuid='" + strUserUUID + "'";
		}
		else if (nType == 2)
		{
			strSql = "select * from StaffDetails where staffuuid ='" + strUserUUID + "'";
		}
	
		//log("SQL for getting userdetails: %s", strSql.c_str()); 
		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				IM::BaseDefine::UserDetails AddiInfo;
				AddiInfo.set_details_uuid(pResultSet->GetString("uuid"));
				AddiInfo.set_details_type(pResultSet->GetInt("type"));
				AddiInfo.set_details_title(pResultSet->GetString("title"));
				AddiInfo.set_details_content(pResultSet->GetString("content"));
				listAddiInfo.push_back(AddiInfo);
			}
			delete pResultSet;
		}
		else
		{
			log("no result set for sql:%s", strSql.c_str());
		}
		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_slave");
	}
}


//bool CUserModel::SyncInfoWithEc(Json::Value& root)
//{
//	if (m_strHost == "")
//	{
//		//read config from server
//		CConfigFileReader configFile("serverconfig.conf");
//		char* pHttpHost = configFile.GetConfigName("HttpHost");
//		if (pHttpHost)
//		{
//			m_strHost = pHttpHost;
//		}
//	}
//
//	if (m_strHost != "")
//	{
//		string strPostData = "";
//		strPostData.append(root.toStyledString());
//
//		CHttpClient HttpClient;
//		string strAddiHeader = "";
//		string strToken = "";
//
//		HttpClient.GetAdditionalInfo(strToken, strAddiHeader);
//		string strActualPath = m_strHost + "/rs/s/ec/userService/modifyUserInfo&rs_token=" + strToken;
//		//string strActualPath = "http://ioa." + strDomainID + "/rs/s/ec/userService/modifyUserInfo?userId=" + strUserID + "&rs_token=" + strToken;
//		string strResponse;
//
//		HttpClient.Post(strActualPath, strPostData, strResponse, strAddiHeader);
//		log("URL: %s, PostData: %s, Response: %s", strActualPath.c_str(), strPostData.c_str(), strResponse.c_str());
//		if (strResponse == "0000")
//		{
//			return true;
//		}
//		else
//		{
//			return false;
//		}
//	}
//	else
//	{
//		log("unknow http host");
//	}
//}




	//Json::Value Root;
	//bool bResult = false;

	//CDBManager* pDBManager = CDBManager::getInstance();
	//CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
	//if (pDBConn)
	//{
	//	string strUserID = "";
	//	string strDomainID = "";
	//	string strSql = "select useruuid, substring_index(username,'@',-1) as mailsuffix, avatar,birthday,sex,signature,nick,phone from IMUser where id=" + int2string(nUserID);
	//	CResultSet *pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
	//	if (pResultSet)
	//	{
	//		while (pResultSet->Next())
	//		{
	//			if (pResultSet->GetString("useruuid") != NULL)
	//			{
	//				strUserID = pResultSet->GetString("useruuid");
	//			}
	//			else
	//			{
	//				strUserID = "";
	//			}

	//			Root["userId"] = strUserID;

	//			//do not pass avatar when updating user information,recomment on June 28th,2016
	//			if (pResultSet->GetString("avatar") != NULL)
	//			{
	//				Root["headPortrait"] = pResultSet->GetString("avatar");
	//			}
	//			else
	//			{
	//				Root["headPortraitd"] = "";
	//			}

	//			if (pResultSet->GetString("signature") != NULL)
	//			{
	//				Root["signature"] = pResultSet->GetString("signature");
	//			}
	//			else
	//			{
	//				Root["signature"] = "";
	//			}

	//			int nBirthday = pResultSet->GetInt("birthday");
	//			double llBirthday = (double)nBirthday * 1000;  //milliseconds for EC
	//			Root["birthday"] = llBirthday;

	//			if (pResultSet->GetString("mailsuffix") != NULL)
	//			{
	//				strDomainID = pResultSet->GetString("mailsuffix");
	//			}

	//			Root["sex"] = pResultSet->GetInt("sex");

	//			if (pResultSet->GetString("nick") != NULL)
	//			{
	//				Root["nickname"] = pResultSet->GetString("nick");
	//			}
	//			else
	//			{
	//				Root["nickname"] = "";
	//			}

	//			if (pResultSet->GetString("phone") != NULL)
	//			{
	//				Root["phone"] = pResultSet->GetString("phone");
	//			}
	//			else
	//			{
	//				Root["phone"] = "";
	//			}
	//		}
	//		delete pResultSet;
	//		pResultSet = NULL;
	//	}

	//	//LIST_USER_DETAILS listUserDetails;
	//	strSql = "select uuid,useruuid,type,title,content from UserDetails where useruuid='" + strUserID + "'";
	//	log("SQL for getting user details: %s", strSql.c_str());

	//	pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
	//	if (pResultSet)
	//	{
	//		Json::Value UserInfoList;

	//		while (pResultSet->Next())
	//		{
	//			Json::Value UserInfo;
	//		
	//			if (pResultSet->GetString("uuid") != NULL)
	//			{
	//				UserInfo["userDetailsId"] =  pResultSet->GetString("uuid");
	//			}
	//			else
	//			{
	//				UserInfo["userDetailsId"] =  "";
	//			}

	//			if (pResultSet->GetString("useruuid") != NULL)
	//			{
	//				UserInfo["userId"] = pResultSet->GetString("useruuid");
	//			}
	//			else
	//			{
	//				UserInfo["userId"]  = "";
	//			}

	//			UserInfo["type"] = pResultSet->GetInt("type");

	//			if (pResultSet->GetString("title") != NULL)
	//			{
	//				UserInfo["title"] = pResultSet->GetString("title");
	//			}
	//			else
	//			{
	//				UserInfo["title"] = "";
	//			}

	//			if (pResultSet->GetString("content") != NULL)
	//			{
	//				UserInfo["content"] = pResultSet->GetString("content");
	//			}
	//			else
	//			{
	//				UserInfo["content"] = "";
	//			}

	//			UserInfoList.append(UserInfo);
	//			/*USER_DETAILS UserDetails;
	//			if (pResultSet->GetString("uuid") != NULL)
	//			{
	//				UserDetails.m_strUUID = pResultSet->GetString("uuid");
	//			}
	//			else
	//			{
	//				UserDetails.m_strUUID = "";
	//			}

	//			if (pResultSet->GetString("useruuid") != NULL)
	//			{
	//				UserDetails.m_strUserUUID = pResultSet->GetString("useruuid");
	//			}
	//			else
	//			{
	//				UserDetails.m_strUserUUID = "";
	//			}

	//			UserDetails.m_nType = pResultSet->GetInt("type");

	//			if (pResultSet->GetString("title") != NULL)
	//			{
	//				UserDetails.m_strTitle = pResultSet->GetString("title");
	//			}
	//			else
	//			{
	//				UserDetails.m_strTitle = "";
	//			}

	//			if (pResultSet->GetString("content") == NULL)
	//			{
	//				UserDetails.m_strContent = pResultSet->GetString("content");
	//			}
	//			else
	//			{
	//				UserDetails.m_strContent = "";
	//			}
	//			listUserDetails.push_back(UserDetails);*/
	//		}
	//		delete pResultSet;
	//		pResultSet = NULL;
	//		Root["userDetails"] = UserInfoList;
	//	}
	//	

	//	string strPostData = "";
	//	strPostData.append(Root.toStyledString());

	//	CHttpClient HttpClient;

	//	string strAddiHeader = "";
	//	string strToken = "";

	//	HttpClient.GetAdditionalInfo(strToken, strAddiHeader);
	//	
	//	string strActualPath = "http://ioa." + strDomainID + "/rs/s/ec/userService/modifyUserInfo?userId=" + strUserID + "&rs_token=" + strToken;	
	//	string strResponse;

	//	HttpClient.Post(strActualPath, strPostData, strResponse, strAddiHeader);
	//	log("URL: %s, PostData: %s, Response: %s", strActualPath.c_str(), strPostData.c_str(), strResponse.c_str());

	//	//comment on June 27th,2016,recomment on June 28th,2016
	//	if (strAvatar != "") //update avatar
	//	{
	//		if (strResponse != "")
	//		{
	//			Json::Reader Reader;
	//			Json::Value Root;
	//			if (Reader.parse(strResponse, Root))
	//			{
	//				if (!Root["headPortrait"].isNull() && Root["headPortrait"].isString())
	//				{
	//					strAvatar = Root["headPortrait"].asString();
	//					strSql = "update IMUser set avatar = '" + strAvatar + ", updated = unix_timestamp() where useruuid='" + strUserID + "' and status = 1";
	//					pDBConn->ExecuteUpdate(strSql.c_str());
	//				}
	//			}
	//		}
	//	}

	//	bResult = true;
	//	pDBManager->RelDBConn(pDBConn);
	//}
//}


//void CUserModel::getUsers(uint32_t unCompanyId, uint32_t& unLastTime, list<IM::BaseDefine::UserInfo>& lsUsers, uint32_t& unUsers)
//{
//	CDBManager* pDBManager = CDBManager::getInstance();
//	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
//	if (pDBConn)
//	{
//
//		string strSql = "select count(*) as usercount from IMUser where companyid=" + int2string(unCompanyId) + " and status=1";
//		log("SQL for getting users: %s", strSql.c_str());
//		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
//		if(pResultSet)
//		{
//			while (pResultSet->Next())
//			{
//				unUsers = pResultSet->GetInt("usercount");
//			}
//			delete pResultSet;
//			pResultSet = NULL;
//		}
//	
//		if (unLastTime == 0)
//		{
//			strSql = "select a.*, max(a.updated) as maxupdated1,max(b.updated) as maxupdated2 from IMUser a left join IMUserPosition b on a.useruuid=b.useruuid where (a.companyid = " + int2string(unCompanyId) + " and a.status <> -1) or a.companyid=0 group by a.id";
//		}
//		else
//		{
//			strSql = "select a.*, max(a.updated) as maxupdated1,max(b.updated) as maxupdated2 from IMUser a left join IMUserPosition b on a.useruuid=b.useruuid where  (a.companyid = " + int2string(unCompanyId) + " or a.companyid = 0) and (a.updated>=" + int2string(unLastTime) + " or b.updated >= " + int2string(unLastTime) + ") group by a.id";
//		}
//
//		log("SQL for getting changed users: %s", strSql.c_str());
//
//		pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
//		if (pResultSet)
//		{
//			while (pResultSet->Next())
//			{
//				IM::BaseDefine::UserInfo cUser;
//
//				int32_t nStatus = -1;
//				uint32_t unUserID = 0, unGender = 2, unContactStatus = 0;//(unknown);
//				string strUserUUID, strNickName, strAvatar, strRealName, strPhone, strUserName, strSignature, strJobNumber, strBirthday, strBuyProducts, strKeyword;
//	
//				uint32_t nUpdated1 = pResultSet->GetInt("maxupdated1");
//				uint32_t nUpdated2 = 0;
//				if (pResultSet->GetString("maxupdated2") != NULL)
//				{
//					nUpdated2 = pResultSet->GetInt("maxupdated2");
//				}
//
//				if (unLastTime < nUpdated1)
//				{
//					unLastTime = nUpdated1;
//				}
//
//				if (unLastTime < nUpdated2)
//				{
//					unLastTime = nUpdated2;
//				}
//
//				if (pResultSet->GetString("jobnumber") == NULL)
//				{
//					strJobNumber = "";
//				}
//				else
//				{
//					strJobNumber = pResultSet->GetString("jobnumber");
//				}
//
//				unUserID = pResultSet->GetInt("id");
//
//				if (pResultSet->GetString("useruuid") == NULL)
//				{
//					strUserUUID = "";
//				}
//				else
//				{
//					strUserUUID = pResultSet->GetString("useruuid");
//				}
//
//				if (pResultSet->GetString("signature") == NULL)
//				{
//					strSignature = "";
//				}
//				else
//				{
//					strSignature = pResultSet->GetString("signature");
//				}
//
//				unGender = pResultSet->GetInt("sex");
//
//				if (pResultSet->GetString("name") == NULL)
//				{
//					strRealName = "";
//				}
//				else
//				{
//					strRealName = pResultSet->GetString("name");
//				}
//
//				if (pResultSet->GetString("username") == NULL)
//				{
//					strUserName = "";
//				}
//				else
//				{
//					strUserName = pResultSet->GetString("username");
//				}
//
//				if (pResultSet->GetString("nick") == NULL)
//				{
//					strNickName = "";
//				}
//				else
//				{
//					strNickName = pResultSet->GetString("nick");
//				}
//
//				if (strNickName == "")
//				{
//					strNickName = strRealName;
//				}
//
//				if (pResultSet->GetString("keyword") == NULL)
//				{
//					strKeyword = "";
//				}
//				else
//				{
//					strKeyword = pResultSet->GetString("keyword");
//				}
//
//				if (pResultSet->GetString("phone") == NULL)
//				{
//					strPhone = "";
//				}
//				else
//				{
//					strPhone = pResultSet->GetString("phone");
//				}
//
//				int32_t nBirthday = pResultSet->GetInt("birthday");
//
//				if (pResultSet->GetString("avatar") == NULL)
//				{
//					strAvatar = "";
//				}
//				else
//				{
//					strAvatar = pResultSet->GetString("avatar");
//				}
//
//				//appened by george on Nov 12th,2015
//				if (pResultSet->GetString("buyproduct") == NULL)
//				{
//					strBuyProducts = "";
//				}
//				else
//				{
//					strBuyProducts = pResultSet->GetString("buyproduct");
//				}
//
//				nStatus = pResultSet->GetInt("status");
//				unContactStatus = pResultSet->GetInt("contactstatus");
//
//				//get company name
//				string strCompanyName = CUserModel::getInstance()->getComanyName(unCompanyId);
//
//				cUser.set_user_id(unUserID);
//				cUser.set_user_uuid(strUserUUID); 
//				cUser.set_job_number(strJobNumber);
//				cUser.set_user_name(strUserName);
//				cUser.set_user_real_name(strRealName);
//				cUser.set_avatar_url(strAvatar);
//				cUser.set_user_gender(unGender);
//				cUser.set_birthday(nBirthday);
//				cUser.set_mobile_phone(strPhone);
//				cUser.set_contact_status(unContactStatus);
//				cUser.set_status(nStatus);
//				cUser.set_keyword(strKeyword);
//				cUser.set_user_nick_name(strNickName);
//				cUser.set_signature(strSignature);
//				cUser.set_companyname(strCompanyName);
//				cUser.set_buyproduct(strBuyProducts);
//
//				list<IM::BaseDefine::DutyInfo> listDutyInfo;
//				map<string, list<IM::BaseDefine::PositionInfo>> mapPositionInfo;
//				GetDuty(strUserUUID, listDutyInfo, mapPositionInfo);
//
//				for (auto it = listDutyInfo.begin(); it != listDutyInfo.end(); it++) //get departments for a user
//				{
//					IM::BaseDefine::DutyInfo *pDuty = cUser.add_duty_list();
//					pDuty->set_depart_uuid(it->depart_uuid());
//					pDuty->set_depart_name(it->depart_name());
//					pDuty->set_status(it->status());
//
//					auto it1 = mapPositionInfo.find(it->depart_uuid());
//					if (it1 != mapPositionInfo.end())
//					{
//						for (auto it2 = it1->second.begin(); it2 != it1->second.end(); it2++) //get position for a department
//						{
//							IM::BaseDefine::PositionInfo *pPosition = pDuty->add_position_list();
//							pPosition->set_position_uuid(it2->position_uuid());
//							pPosition->set_position_type(it2->position_type());
//							pPosition->set_position_name(it2->position_name());
//							pPosition->set_status(it2->status());
//							pPosition->set_sort(it2->sort());
//						}
//					}
//				}
//
//				//get additional information for a user
//				list<IM::BaseDefine::UserDetails> listUserAddInfo;
//				CUserModel::getInstance()->GetUserAdditionalInfo(strUserUUID, listUserAddInfo);
//				for (auto it = listUserAddInfo.begin(); it != listUserAddInfo.end(); it++)
//				{
//					IM::BaseDefine::UserDetails *pAddInfo = cUser.add_add_info_list();
//					pAddInfo->set_details_uuid(it->details_uuid());
//					pAddInfo->set_details_type(it->details_type());
//					pAddInfo->set_details_title(it->details_title());
//					pAddInfo->set_details_content(it->details_content());
//				}
//
//				//get black list
//				list<IM::BaseDefine::ShieldUser> lsShieldUser;
//				CUserModel::getInstance()->GetUserShieldStatus(unUserID, lsShieldUser);
//				//log("Black List for %d: %d", nId, lsShieldUser.size());
//				for (auto it = lsShieldUser.begin(); it != lsShieldUser.end(); it++)
//				{
//					IM::BaseDefine::ShieldUser* pShieldUser = cUser.add_black_list();
//					pShieldUser->set_user_id(it->user_id());
//					pShieldUser->set_shield_status(it->shield_status());
//					//log("peerid: %d, shield status: %d", it->user_id(), it->shield_status());
//				}
//
//				lsUsers.push_back(cUser);
//			}
//			delete pResultSet;
//			pResultSet = NULL;
//		}
//		else
//		{
//			log(" no result set for sql:%s", strSql.c_str());
//		}
//
//		pDBManager->RelDBConn(pDBConn);
//	}
//	else
//	{
//		log("no db connection for teamtalk_slave");
//	}
//}


void CUserModel::GetJoinedCompany(uint32_t unUserId, list<IM::BaseDefine::CompanyInfo>& listCompany)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		string strSql = "select a.* from IMCompany a, IMStaff b where a.id=b.companyid and b.userid= " + int2string(unUserId) + " and a.status=1 and b.status=1";
		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		log("SQL for getting company; %s", strSql.c_str());

		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				IM::BaseDefine::CompanyInfo  CompanyInfo;
				string strCompanyUuid = "";
				if (pResultSet->GetString("companyuuid") != NULL)
				{
					strCompanyUuid = pResultSet->GetString("companyuuid");
				}

				string strCompanyNo = "";
				if (pResultSet->GetString("companyno") != NULL)
				{
					strCompanyNo = pResultSet->GetString("companyno");
				}

				string strCompanyName = "";
				if (pResultSet->GetString("companyname") != NULL)
				{
					strCompanyName = pResultSet->GetString("companyname");
				}

				string strCompanyFullName = "";
				if (pResultSet->GetString("companyfullname") != NULL)
				{
					strCompanyFullName = pResultSet->GetString("companyfullname");
				}

				string strIndustryType = "";
				if (pResultSet->GetString("industrytype") != NULL)
				{
					strIndustryType = pResultSet->GetString("industrytype");
				}

				CompanyInfo.set_company_id(pResultSet->GetInt("id"));
				CompanyInfo.set_company_uuid(strCompanyUuid);
				CompanyInfo.set_company_no(strCompanyNo);
				CompanyInfo.set_company_name(strCompanyName);
				CompanyInfo.set_company_full_name(strCompanyFullName);
				CompanyInfo.set_industry_type(strIndustryType);
				CompanyInfo.set_company_is_joined(pResultSet->GetInt("isjoined"));
				CompanyInfo.set_company_is_invite(pResultSet->GetInt("isinvite"));

				listCompany.push_back(CompanyInfo);
			}

			delete pResultSet;
			pResultSet = NULL;
		}
		else
		{
			log("no result set for sql:%s", strSql.c_str());
		}

		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_slave");
	}
}


void CUserModel::GetCompanyStaff(uint32_t unUserId, uint32_t unCompanyId, uint32_t& unLastTime, list<IM::BaseDefine::StaffInfo>& lsStaff, uint32_t& unStaff)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		string strSql = "";

		/*string strSql1 = "select a.*, max(a.updated) as maxupdated1,max(b.updated) as maxupdated2,max(c.updated) as maxupdated3 from IMStaff a  ";
		strSql1 += " left join StaffDetails b on a.uuid = b.staffuuid ";
		strSql1 += " left join IMUserPosition c on a.uuid = c.useruuid ";*/

		string strSql1 = "select * from IMStaff ";

		string strSql2 = "";
		CResultSet* pResultSet = NULL;
		if (unCompanyId != 0)
		{
			string strSql = "select count(*) as usercount from IMStaff where companyid=" + int2string(unCompanyId) + " and status=1";
			log("SQL for getting staff: %s", strSql.c_str());
			pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
			if (pResultSet)
			{
				while (pResultSet->Next())
				{
					unStaff = pResultSet->GetInt("usercount");
				}
				delete pResultSet;
				pResultSet = NULL;
			}

			//strSql = "select a.*,a.updated as maxupdated1,maxupdated2,maxupdated3 from ";
			//strSql += "(select * from IMStaff where companyid=" + int2string(unCompanyId) + ") as a ";
			//strSql += " left join (select staffuuid, max(updated) as maxupdated2 from StaffDetails group by staffuuid) as b on a.uuid = b.staffuuid ";
			//strSql += " left join (select useruuid, max(updated) as maxupdated3 from IMUserPosition where companyid= " + int2string(unCompanyId) + " and status=0 group by useruuid) as c on a.useruuid=c.useruuid where ";

			strSql2 = " where companyid = " + int2string(unCompanyId) + " and ";
		}
		else
		{
			strSql = "select count(*) as usercount from IMStaff a where exists(select 1 from (select companyid from IMStaff where userid = " + int2string(unUserId) + ") as b where a.companyid = b.companyid) and a.status=1";
			log("SQL for getting staff: %s", strSql.c_str());
			CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
			if (pResultSet)
			{
				while (pResultSet->Next())
				{
					unStaff = pResultSet->GetInt("usercount");
				}
				delete pResultSet;
				pResultSet = NULL;
			}

			//strSql = "select a.*,a.updated as maxupdated1,maxupdated2,maxupdated3 from ";
			//strSql += " (select * from IMStaff d where exists(select 1 from (select companyid from IMStaff where userid = " + int2string(unUserId) + ") as e where d.companyid = e.companyid)) as a ";
			//strSql += " left join (select staffuuid, max(updated) as maxupdated2 from StaffDetails group by staffuuid) as b on a.uuid = b.staffuuid ";
			//strSql += " left join (select useruuid, max(updated) as maxupdated3 from IMUserPosition f  where companyid= " + int2string(unCompanyId) + " and status=0 group by useruuid) as c on a.useruuid=c.useruuid ";

			strSql2 = "a where exists(select 1 from (select companyid from IMStaff where userid=" + int2string(unUserId) + ") as b where a.companyid=b.companyid) and ";
		}

		strSql = strSql1;
		strSql += strSql2;

		if (unLastTime == 0)
		{
			strSql += "  status <> -1";
		}
		else
		{
			strSql += " updated >=" + int2string(unLastTime);
		}

		//strSql += " group by a.uuid";
		log("SQL for getting changed staff: %s", strSql.c_str());

		pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				IM::BaseDefine::StaffInfo CurrentStaff;
				string strStaffUuid = "", strUserUuid = "", strJobNumber = "", strUserRealName = "", strPhone = "", strKeyword = "", strBuyProduct = "";
				if (pResultSet->GetString("uuid") != NULL)
				{
					strStaffUuid = pResultSet->GetString("uuid");
				}

				if (pResultSet->GetString("jobnumber") != NULL)
				{
					strJobNumber = pResultSet->GetString("jobnumber");
				}

				if (pResultSet->GetString("staffname") != NULL)
				{
					strUserRealName = pResultSet->GetString("staffname");
				}

				if (pResultSet->GetString("mobilephone") != NULL)
				{
					strPhone = pResultSet->GetString("mobilephone");
				}

				if (pResultSet->GetString("keyword") != NULL)
				{
					strKeyword = pResultSet->GetString("keyword");
				}

				if (pResultSet->GetString("useruuid") != NULL)
				{
					strUserUuid = pResultSet->GetString("useruuid");
				}

				string strAccount = "", strUserName = "", strEmail = "", strDefaultEmail = "";
				if (pResultSet->GetString("account") != NULL)
				{
					strAccount = pResultSet->GetString("account");
				}

				if (pResultSet->GetString("username") != NULL)
				{
					strUserName = pResultSet->GetString("username");
				}

				if (pResultSet->GetString("email") != NULL)
				{
					strEmail = pResultSet->GetString("email");
				}

				if (pResultSet->GetString("defaultemail") != NULL)
				{
					strDefaultEmail = pResultSet->GetString("defaultemail");
				}

				CurrentStaff.set_staff_uuid(strStaffUuid);
				CurrentStaff.set_job_number(strJobNumber);
				CurrentStaff.set_staff_name(strUserRealName);
				CurrentStaff.set_mobile_phone(strPhone);
				CurrentStaff.set_keyword(strKeyword);
				CurrentStaff.set_user_uuid(strUserUuid);
				CurrentStaff.set_user_id(pResultSet->GetInt("userid"));
				CurrentStaff.set_company_id(pResultSet->GetInt("companyid"));
				CurrentStaff.set_account(strAccount);
				CurrentStaff.set_user_name(strUserName);
				CurrentStaff.set_email(strEmail);
				CurrentStaff.set_default_email(strDefaultEmail);
			
				int32_t nIsAdmin = pResultSet->GetInt("isadmin");
				int32_t nAdminStatus = pResultSet->GetInt("adminstatus");
				int32_t nIsSuperAdmin = pResultSet->GetInt("issuperadmin");
				int32_t nStatus = pResultSet->GetInt("status");
				int32_t nIsActived = pResultSet->GetInt("isactived");
				int32_t nContactStatus = pResultSet->GetInt("contactstatus");

				//set flag
				uint32_t unFlag = GetUserFlags(nIsAdmin, nAdminStatus, nIsSuperAdmin, nStatus, nIsActived, nContactStatus);
				CurrentStaff.set_flags(unFlag);

				uint32_t unUpdated = pResultSet->GetInt("updated");
				if (unLastTime < unUpdated)
				{
					unLastTime = unUpdated;
				}

			/*	uint32_t unUpdated1 = pResultSet->GetInt("maxupdated1");
				uint32_t unUpdated2 = 0;
				if (pResultSet->GetString("maxupdated2") != NULL)
				{
					unUpdated2 = pResultSet->GetInt("maxupdated2");
				}
				uint32_t unUpdated3 = 0;
				if (pResultSet->GetString("maxupdated3") != NULL)
				{
					unUpdated3 = pResultSet->GetInt("maxupdated3");
				}

				if (unLastTime < unUpdated1)
				{
					unLastTime = unUpdated1;
				}

				if (unLastTime < unUpdated2)
				{
					unLastTime = unUpdated2;
				}

				if (unLastTime < unUpdated3)
				{
					unLastTime = unUpdated3;
				}*/
				
				lsStaff.push_back(CurrentStaff);
			}

			delete pResultSet;
			pResultSet = NULL;
		}
		else
		{
			log(" no result set for sql:%s", strSql.c_str());
		}

		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_slave");
	}
}


void CUserModel::GetCompanyStaff(uint32_t unUserId, uint32_t unCompanyId, uint32_t& unLastTime, list<IM::BaseDefine::StaffInfoLite>& lsStaff)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
	/*	string strSql = "select a.*,d.id,name,avatar,sex,d.status as userstatus,max(a.updated) as maxupdated1,max(b.updated) as maxupdated2,max(c.updated) as maxupdated3, max(d.updated) as maxupdated4, max(e.updated) as maxupdated5 from ";
		strSql += " IMStaff a ";
		strSql += " left join StaffDetails b on a.uuid=b.staffuuid ";
		strSql += " left join IMUserPosition c on a.uuid=c.useruuid ";
		strSql += " left join IMUser d on a.userid = d.id ";
		strSql += " left join UserDetails e on d.useruuid=e.useruuid ";
		strSql += " where a.companyid=" + int2string(unCompanyId) + " and ";*/

		string strSql = "select a.*,a.updated as maxupdated1,b.id,name,avatar,sex,b.status as userstatus,b.updated as maxupdated2 from IMStaff a, IMUser b where a.userid=b.id and a.companyid=" + int2string(unCompanyId) + " and ";

		if (unLastTime == 0)
		{
			strSql += "a.status <> -1 and b.status <> -1";
		}
		else
		{
			strSql += "(a.updated >=" + int2string(unLastTime) + " or b.updated >= " + int2string(unLastTime) +  ")";
		}

		//strSql += " group by a.uuid";
		log("SQL for getting changed staff: %s", strSql.c_str());

		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				IM::BaseDefine::StaffInfoLite CurrentStaff;
				string strStaffUuid = "", strUserUuid = "", strStaffName = "", strUserRealName = "", strPhone = "", strAvatar = "";
				if (pResultSet->GetString("uuid") != NULL)
				{
					strStaffUuid = pResultSet->GetString("uuid");
				}

				if (pResultSet->GetString("name") != NULL)
				{
					strUserRealName = pResultSet->GetString("name");
				}

				if (pResultSet->GetString("staffname") != NULL)
				{
					strStaffName = pResultSet->GetString("staffname");
				}

				if (pResultSet->GetString("mobilephone") != NULL)
				{
					strPhone = pResultSet->GetString("mobilephone");
				}

				if (pResultSet->GetString("avatar") != NULL)
				{
					strAvatar = pResultSet->GetString("avatar");
				}

				if (pResultSet->GetString("useruuid") != NULL)
				{
					strUserUuid = pResultSet->GetString("useruuid");
				}

				string strAccount = "", strUserName = "", strEmail = "", strDefaultEmail = "";
				if (pResultSet->GetString("account") != NULL)
				{
					strAccount = pResultSet->GetString("account");
				}

				if (pResultSet->GetString("username") != NULL)
				{
					strUserName = pResultSet->GetString("username");
				}

				if (pResultSet->GetString("email") != NULL)
				{
					strEmail = pResultSet->GetString("email");
				}

				if (pResultSet->GetString("defaultemail") != NULL)
				{
					strDefaultEmail = pResultSet->GetString("defaultemail");
				}

				CurrentStaff.set_staff_uuid(strStaffUuid);
				CurrentStaff.set_staff_real_name(strStaffName);
				CurrentStaff.set_user_real_name(strUserRealName);
				CurrentStaff.set_mobile_phone(strPhone);
				CurrentStaff.set_avatar_url(strAvatar);
				CurrentStaff.set_user_uuid(strUserUuid);
				CurrentStaff.set_user_id(pResultSet->GetInt("userid"));
			/*	CurrentStaff.set_account(strAccount);
				CurrentStaff.set_user_name(strUserName);
				CurrentStaff.set_email(strEmail);
				CurrentStaff.set_default_email(strDefaultEmail);*/


				int32_t nIsAdmin = pResultSet->GetInt("isadmin");
				int32_t nAdminStatus = pResultSet->GetInt("adminstatus");
				int32_t nIsSuperAdmin = pResultSet->GetInt("issuperadmin");
				int32_t nStaffStatus = pResultSet->GetInt("status");
				int32_t nIsActived = pResultSet->GetInt("isactived");
				int32_t nContactStatus = pResultSet->GetInt("contactstatus");
				int32_t nSex = pResultSet->GetInt("sex");
				int32_t nUserStatus = pResultSet->GetInt("userstatus");

				//set flag
				uint32_t unFlag = GetUserFlags(nSex, nUserStatus, nIsAdmin, nAdminStatus, nIsSuperAdmin, nStaffStatus, nIsActived, nContactStatus);
				CurrentStaff.set_flags(unFlag);

				uint32_t unUpdated1 = pResultSet->GetInt("maxupdated1");
				uint32_t unUpdated2 = pResultSet->GetInt("maxupdated2"); 
			/*	if (pResultSet->GetString("maxupdated2") != NULL)
				{
					unUpdated2 = pResultSet->GetInt("maxupdated2");
				}
				uint32_t unUpdated3 = 0;
				if (pResultSet->GetString("maxupdated3") != NULL)
				{
					unUpdated3 = pResultSet->GetInt("maxupdated3");
				}

				uint32_t unUpdated4 = 0;
				if (pResultSet->GetString("maxupdated4") != NULL)
				{
					unUpdated4 = pResultSet->GetInt("maxupdated4");
				}
				uint32_t unUpdated5 = 0;
				if (pResultSet->GetString("maxupdated5") != NULL)
				{
					unUpdated5 = pResultSet->GetInt("maxupdated5");
				}*/

				//uint32_t unUpdated6 = 0;
				//if (pResultSet->GetString("maxupdated6") != NULL)
				//{
				//	unUpdated6 = pResultSet->GetInt("maxupdated6");
				//}

				if (unLastTime < unUpdated1)
				{
					unLastTime = unUpdated1;
				}

				if (unLastTime < unUpdated2)
				{
					unLastTime = unUpdated2;
				}

				//if (unLastTime < unUpdated3)
				//{
				//	unLastTime = unUpdated3;
				//}

				//if (unLastTime < unUpdated4)
				//{
				//	unLastTime = unUpdated4;
				//}

				//if (unLastTime < unUpdated5)
				//{
				//	unLastTime = unUpdated5;
				//}

				lsStaff.push_back(CurrentStaff);
			}

			delete pResultSet;
			pResultSet = NULL;
		}
		else
		{
			log(" no result set for sql:%s", strSql.c_str());
		}

		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_slave");
	}
}


bool CUserModel::UpdateUser(string& strUserUuid, uint32_t unSex, int32_t nBirthday, string& strSignature, string& strUserRealName, list<IM::BaseDefine::UserDetails>& lsUserDetails, uint32_t unNow)
{
	bool bRet = false;
	Json::Value Root;
	Root["userId"] = strUserUuid;
	double llBirthday = (double)nBirthday * 1000;  //milliseconds for EC
	Root["birthday"] = llBirthday;
	Root["signature"] = strSignature;
	Root["sex"] = unSex;
	Root["fullName"] = strUserRealName;

	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
	if (pDBConn)
	{
		string strSql = "update IMUser set `sex`= " + int2string(unSex) + ", `signature`='"
			+ strSignature + "', `birthday`="
			+ int2string(nBirthday) + ",name='" + strUserRealName + "',`updated`="
			+ int2string(unNow) + " where useruuid='" + strUserUuid + "'";
		bRet = pDBConn->ExecuteUpdate(strSql.c_str());
		log("SQL for updating msg: %s", strSql.c_str());
		if (!bRet)
		{
			log("update failed:%s", strSql.c_str());
		}

		//additonal information
		//delete all additional information first
		strSql = "delete from UserDetails where useruuid='" + strUserUuid + "'";
		pDBConn->ExecuteUpdate(strSql.c_str());
	
		Json::Value UserInfoList;
		
		strSql = "insert into UserDetails(uuid,useruuid,type,title,content,created,updated) values(?,?,?,?,?,?,?)";
		CPrepareStatement* stmt = new CPrepareStatement();
		if (stmt->Init(pDBConn->GetMysql(), strSql))
		{
			for (auto it = lsUserDetails.begin(); it != lsUserDetails.end(); it++)
			{
				IM::BaseDefine::UserDetails  Details = *it;
				Json::Value UserInfo;
				
				//UserInfo["userDetailsId"] = Details.details_uuid();
				UserInfo["userId"] = strUserUuid;
				UserInfo["type"] = Details.details_type();
				UserInfo["title"] = Details.details_title();
				UserInfo["content"] = Details.details_content();
				UserInfoList.append(UserInfo);


				uint32_t nNow = (uint32_t)time(NULL);
				uint32_t index = 0;
				uint32_t nType = Details.details_type();
				stmt->SetParam(index++, Details.details_uuid());
				stmt->SetParam(index++, strUserUuid);
				stmt->SetParam(index++, nType);
				stmt->SetParam(index++, Details.details_title());
				stmt->SetParam(index++, Details.details_content());
				stmt->SetParam(index++, unNow);
				stmt->SetParam(index++, unNow);
				bRet = stmt->ExecuteUpdate();

				if (!bRet)
				{
					log("insert user failed: %s", strSql.c_str());
				}
			}
		}
		delete stmt;

		if (lsUserDetails.size() != 0)
		{
			/*Json::Value UserInfo;
			UserInfoList.append(UserInfo);*/
			Root["userDetails"] = UserInfoList;
		}
		
		
		
		////delete all additional information first
		//strSql = "delete from UserDetails where useruuid='" + strUserUuid + "'";
		//pDBConn->ExecuteUpdate(strSql.c_str());

		////update additional information
		//for (auto it = lsUserDetails.begin(); it != lsUserDetails.end(); it++)
		//{
		//	IM::BaseDefine::UserDetails  Details = *it;

		//	Json::Value UserInfo;
		//	//UserInfo["userDetailsId"] = Details.details_uuid();
		//	UserInfo["userId"] = strUserUuid;
		//	UserInfo["type"] = Details.details_type();
		//	UserInfo["title"] = Details.details_title();
		//	UserInfo["content"] = Details.details_content();
		//	UserInfoList.append(UserInfo);

		//	strSql = "insert into UserDetails(uuid,useruuid,type,title,content,created,updated) values(?,?,?,?,?,?,?)";
		//	CPrepareStatement* stmt = new CPrepareStatement();
		//	if (stmt->Init(pDBConn->GetMysql(), strSql))
		//	{
		//		uint32_t nNow = (uint32_t)time(NULL);
		//		uint32_t index = 0;
		//		uint32_t nType = Details.details_type();
		//		stmt->SetParam(index++, Details.details_uuid());
		//		stmt->SetParam(index++, strUserUuid);
		//		stmt->SetParam(index++, nType);
		//		stmt->SetParam(index++, Details.details_title());
		//		stmt->SetParam(index++, Details.details_content());
		//		stmt->SetParam(index++, unNow);
		//		stmt->SetParam(index++, unNow);
		//		bRet = stmt->ExecuteUpdate();

		//		if (!bRet)
		//		{
		//			log("insert user failed: %s", strSql.c_str());
		//		}
		//	}

		//	delete stmt;
		//}

		pDBManager->RelDBConn(pDBConn);

		//Root["userDetails"] = UserInfoList;
	}
	else
	{
		log("no db connection for teamtalk_master");
	}
	
	if (bRet)
	{
		//bRet = SyncUserInfo(cUser.user_id(), strAvatar);
		bRet = SyncInfoWithEc(Root);
	}
	return bRet;

}


bool CUserModel::UpdateStaff(string& strStaffUuid, list<IM::BaseDefine::UserDetails>& lsStaffDetails, uint32_t unNow)
{
	Json::Value Root;
	bool bRet = false;
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
	if (pDBConn)
	{
		//delete current staff information
		string strSql = "delete from StaffDetails where staffuuid='" + strStaffUuid + "'";
		pDBConn->ExecuteUpdate(strSql.c_str());
	
		Json::Value StaffInfoList;
			
		strSql = "insert into StaffDetails(uuid,staffuuid,type,title,content,created,updated) values(?,?,?,?,?,?,?)";
		CPrepareStatement* stmt = new CPrepareStatement();
		if (stmt->Init(pDBConn->GetMysql(), strSql))
		{
			for (auto it = lsStaffDetails.begin(); it != lsStaffDetails.end(); it++)
			{
				IM::BaseDefine::UserDetails  Details = *it;

				Json::Value StaffInfo;
				StaffInfo["staffId"] = strStaffUuid;
				StaffInfo["type"] = Details.details_type();
				StaffInfo["title"] = Details.details_title();
				StaffInfo["content"] = Details.details_content();
				StaffInfoList.append(StaffInfo);

				uint32_t index = 0;
				uint32_t nType = Details.details_type();
				stmt->SetParam(index++, Details.details_uuid());
				stmt->SetParam(index++, strStaffUuid);
				stmt->SetParam(index++, nType);
				stmt->SetParam(index++, Details.details_title());
				stmt->SetParam(index++, Details.details_content());
				stmt->SetParam(index++, unNow);
				stmt->SetParam(index++, unNow);
				bRet = stmt->ExecuteUpdate();

				if (!bRet)
				{
					log("insert user failed: %s", strSql.c_str());
				}
			}
		}

		delete stmt;
		pDBManager->RelDBConn(pDBConn);

		if (bRet && lsStaffDetails.size() != 0)
		{
			/*Json::Value StaffInfo;
			StaffInfoList.append(StaffInfo);*/
			Root["staffDetails"] = StaffInfoList;
			Root["staffId"] = strStaffUuid;
			bRet = SyncInfoWithEc(Root, 2);
		}
	}
	else
	{
		log("no db connection for teamtalk_master");
	}

	return bRet;

	
	//bool bRet = false;
	//CDBManager* pDBManager = CDBManager::getInstance();
	//CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
	//if (pDBConn)
	//{
	//	//update additional information
	//	string strSql = "delete from StaffDetails where staffuuid='" + strStaffUuid + "'";
	//	pDBConn->ExecuteUpdate(strSql.c_str());
	//	
	//	for (auto it = lsStaffDetails.begin(); it != lsStaffDetails.end(); it++)
	//	{
	//		IM::BaseDefine::UserDetails  Details = *it;
	//		{
	//			Json::Value UserInfo;
	//			UserInfo["userDetailsId"] = Details.details_uuid();
	//			UserInfo["userId"] = strUserUuid;
	//			UserInfo["type"] = Details.details_type();
	//			UserInfo["title"] = Details.details_title();
	//			UserInfo["content"] = Details.details_content();
	//			UserInfoList.append(UserInfo);

	//	/*	bool bExist = false;
	//		string strSql = "select staffuuid from StaffDetails where uuid='" + Details.details_uuid() + "'";
	//		CResultSet *pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
	//		if (pResultSet)
	//		{
	//			while (pResultSet->Next())
	//			{
	//				bExist = true;
	//			}
	//			delete pResultSet;
	//			pResultSet = NULL;
	//		}

	//		if (bExist)
	//		{
	//			strSql = "update StaffDetails set title ='" + Details.details_title() + "', type=" + int2string(Details.details_type()) + ",content = '" + Details.details_content() + "',updated=unix_timestamp() where uuid='" + Details.details_uuid() + "'";
	//			log("SQL for updating staffdetails: %s", strSql.c_str());
	//			bRet = pDBConn->ExecuteUpdate(strSql.c_str());
	//		}
	//		else
	//		{*/
	//		
	//			strSql = "insert into StaffDetails(uuid,staffuuid,type,title,content,created,updated) values(?,?,?,?,?,?,?)";
	//			CPrepareStatement* stmt = new CPrepareStatement();
	//			if (stmt->Init(pDBConn->GetMysql(), strSql))
	//			{
	//				//uint32_t nNow = (uint32_t)time(NULL);
	//				uint32_t index = 0;
	//				uint32_t nType = Details.details_type();
	//				stmt->SetParam(index++, Details.details_uuid());
	//				stmt->SetParam(index++, strStaffUuid);
	//				stmt->SetParam(index++, nType);
	//				stmt->SetParam(index++, Details.details_title());
	//				stmt->SetParam(index++, Details.details_content());
	//				stmt->SetParam(index++, unNow);
	//				stmt->SetParam(index++, unNow);
	//				bRet = stmt->ExecuteUpdate();

	//				if (!bRet)
	//				{
	//					log("insert user failed: %s", strSql.c_str());
	//				}
	//			}

	//			delete stmt;
	//		}
	//	}

	//	pDBManager->RelDBConn(pDBConn);
	//}
	//else
	//{
	//	log("no db connection for teamtalk_master");
	//}

	//if (bRet)
	//{
	//	//sync staff info
	//	;
	//	//SyncUserInfo(cUser.user_id(), strAvatar);
	//}
	//return bRet;

}


void CUserModel::GetUsers(uint32_t unUserId, uint32_t& unLastTime, list<IM::BaseDefine::UserInfo>& lsUsers, uint32_t& unUsers)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{		
		//get normal users
	/*	string strSql = "select count(*) as usercount from IMUser c where status = 1 and exists(select 1 from IMStaff a ,(select companyid from IMStaff where userid=" + int2string(unUserId) + " and status = 1) as b ";
		strSql += " where a.companyid=b.companyid and c.id=a.userid and a.status = 1)";
		log("SQL for getting normal users: %s", strSql.c_str());

		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				unUsers = pResultSet->GetInt("usercount");
			}
			delete pResultSet;
			pResultSet = NULL;
		}*/

		
		string strSql = "select a.* from IMUser a left join IMStaff b on a.id=b.userid where ";
		strSql += " (exists(select 1 from (select companyid from IMStaff where userid = " + int2string(unUserId) + " and status <> -1) as c where b.companyid = c.companyid and b.status <> -1) or a.id < 10001) ";

		//string strSql = "select a.*,max(a.updated) as maxupdated1, max(b.updated) as maxupdated2 from IMUser a left join UserDetails b on a.useruuid=b.useruuid, IMStaff c ";
		//strSql += " where a.id < 10001 or (a.id=c.userid and exists(select 1 from (select companyid from IMStaff where userid=" + int2string(unUserId) + ") as d where c.companyid=d.companyid) ";

	/*	string strSql = "select a.*,a.updated as maxupdated1, e.maxupdated2 from IMUser a left join (select useruuid, max(updated) as maxupdated2 from  UserDetails group by useruuid) as e on a.useruuid=e.useruuid, ";
		strSql += "(select userid from IMStaff b where exists(select 1 from (select companyid from IMStaff where userid=" + int2string(unUserId) + " and status = 1) as c where b.companyid=c.companyid) group by userid) as d "
			+ " where (a.id <= 10000) or (a.id=d.userid  ";*/
	/*	log("SQL for getting users: %s", strSql.c_str());
		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				unUsers = pResultSet->GetInt("usercount");
			}
			delete pResultSet;
			pResultSet = NULL;
		}*/

		if (unLastTime == 0)
		{
			//strSql = "select a.*, max(a.updated) as maxupdated1,max(b.updated) as maxupdated2 from IMUser a left join IMUserPosition b on a.useruuid=b.useruuid where (a.companyid = " + int2string(unCompanyId) + " and a.status <> -1) or a.companyid=0 group by a.id";
			strSql += " and (a.status <> -1)";
		}
		else
		{
			//strSql = "select a.*, max(a.updated) as maxupdated1,max(b.updated) as maxupdated2 from IMUser a left join IMUserPosition b on a.useruuid=b.useruuid where  (a.companyid = " + int2string(unCompanyId) + " or a.companyid = 0) and (a.updated>=" + int2string(unLastTime) + " or b.updated >= " + int2string(unLastTime) + ") group by a.id";
			strSql += " and (a.updated >=" + int2string(unLastTime) + ")";
		}

		strSql += " group by id";
		log("SQL for getting changed users: %s", strSql.c_str());

		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		//pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				IM::BaseDefine::UserInfo CurrentUser;

				string strUserUuid = "", strNickName = "", strAvatar = "", strRealName = "", strPhone = "", strUserName = "", strKeyword = "",strSignature = "", strEmail = "";

				uint32_t unUpdate = pResultSet->GetInt("updated");
				if (unLastTime < unUpdate)
				{
					unLastTime = unUpdate;
				}

				/*uint32_t nUpdated1 = pResultSet->GetInt("maxupdated1");
				uint32_t nUpdated2 = 0;
				if (pResultSet->GetString("maxupdated2") != NULL)
				{
					nUpdated2 = pResultSet->GetInt("maxupdated2");
				}

				if (unLastTime < nUpdated1)
				{
					unLastTime = nUpdated1;
				}

				if (unLastTime < nUpdated2)
				{
					unLastTime = nUpdated2;
				}*/

				CurrentUser.set_user_id(pResultSet->GetInt("id"));

				if (pResultSet->GetString("useruuid") != NULL)
				{
					strUserUuid = pResultSet->GetString("useruuid");
				}

				if (pResultSet->GetString("signature") != NULL)
				{
					strSignature = pResultSet->GetString("signature");
				}

				if (pResultSet->GetString("name") != NULL)
				{
					strRealName = pResultSet->GetString("name");
				}

				if (pResultSet->GetString("username") != NULL)
				{
					strUserName = pResultSet->GetString("username");
				}

				if (pResultSet->GetString("nick") != NULL)
				{
					strNickName = pResultSet->GetString("nick");
				}

				if (strNickName == "")
				{
					strNickName = strRealName;
				}

				if (pResultSet->GetString("keyword") != NULL)
				{
					strKeyword = pResultSet->GetString("keyword");
				}

				if (pResultSet->GetString("phone") != NULL)
				{
					strPhone = pResultSet->GetString("phone");
				}

				int32_t nBirthday = pResultSet->GetInt("birthday");

				if (pResultSet->GetString("avatar") != NULL)
				{
					strAvatar = pResultSet->GetString("avatar");
				}

				if (pResultSet->GetString("email") != NULL)
				{
					strEmail = pResultSet->GetString("email");
				}

				string strAccount = "";
				if (pResultSet->GetString("account") != NULL)
				{
					strAccount = pResultSet->GetString("account");
				}

				CurrentUser.set_user_uuid(strUserUuid);
				CurrentUser.set_user_name(strUserName);
				CurrentUser.set_user_real_name(strRealName);
				CurrentUser.set_avatar_url(strAvatar);
				CurrentUser.set_user_gender(pResultSet->GetInt("sex"));
				CurrentUser.set_birthday(pResultSet->GetInt("birthday"));
				CurrentUser.set_mobile_phone(strPhone);
				CurrentUser.set_status(pResultSet->GetInt("status"));
				CurrentUser.set_keyword(strKeyword);
				CurrentUser.set_user_nick_name(strNickName);
				CurrentUser.set_signature(strSignature);
				CurrentUser.set_email(strEmail);
				CurrentUser.set_account(strAccount);

				lsUsers.push_back(CurrentUser);
			}

			delete pResultSet;
			pResultSet = NULL;
		}
		else
		{
			log(" no result set for sql:%s", strSql.c_str());
		}

		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_slave");
	}
}


void CUserModel::GetStaffInfo(uint32_t unUserId, uint32_t unStaffId, list<IM::BaseDefine::StaffInfo>& lsStaffInfo)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		string strSql = "select * from IMStaff a where a.userid=" + int2string(unStaffId) + " and ";
		strSql += " exists(select 1 from(select companyid from IMStaff where userid = " + int2string(unUserId) + " and status = 1) as b where a.companyid=b.companyid and a.status=1)";
		log("SQL for getting staff info: %s", strSql.c_str());
		
		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				IM::BaseDefine::StaffInfo CurrentStaff;
				string strUserUuid= "", strStaffUuid = "", strJobNumber = "", strStaffName = "", strPhone = "", strKeyword = "";

				if (pResultSet->GetString("uuid") != NULL)
				{
					strStaffUuid = pResultSet->GetString("uuid");
				}

				if (pResultSet->GetString("jobnumber") != NULL)
				{
					strJobNumber = pResultSet->GetString("jobnumber");
				}

				if (pResultSet->GetString("staffname") != NULL)
				{
					strStaffName = pResultSet->GetString("staffname");
				}

				if (pResultSet->GetString("mobilephone") != NULL)
				{
					strPhone = pResultSet->GetString("mobilephone");
				}

				if (pResultSet->GetString("keyword") != NULL)
				{
					strKeyword = pResultSet->GetString("keyword");
				}

				if (pResultSet->GetString("useruuid") != NULL)
				{
					strUserUuid = pResultSet->GetString("useruuid");
				}

				string strAccount = "", strUserName = "", strEmail = "", strDefaultEmail = "";
				if (pResultSet->GetString("account") != NULL)
				{
					strAccount = pResultSet->GetString("account");
				}

				if (pResultSet->GetString("username") != NULL)
				{
					strUserName = pResultSet->GetString("username");
				}

				if (pResultSet->GetString("email") != NULL)
				{
					strEmail = pResultSet->GetString("email");
				}

				if (pResultSet->GetString("defaultemail") != NULL)
				{
					strDefaultEmail = pResultSet->GetString("defaultemail");
				}

				CurrentStaff.set_staff_uuid(strStaffUuid);
				CurrentStaff.set_job_number(strJobNumber);
				CurrentStaff.set_staff_name(strStaffName);
				CurrentStaff.set_mobile_phone(strPhone);
				CurrentStaff.set_company_id(pResultSet->GetInt("companyid"));
				CurrentStaff.set_keyword(strKeyword);
				CurrentStaff.set_user_id(pResultSet->GetInt("userid"));
				CurrentStaff.set_user_uuid(strUserUuid);
				CurrentStaff.set_flags(0); //flag is 0 by default when query details

				CurrentStaff.set_account(strAccount);
				CurrentStaff.set_user_name(strUserName);
				CurrentStaff.set_email(strEmail);
				CurrentStaff.set_default_email(strDefaultEmail);

				lsStaffInfo.push_back(CurrentStaff);
			}

			delete pResultSet;
			pResultSet = NULL;
		}
		else
		{
			log(" no result set for sql:%s", strSql.c_str());
		}

		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_slave");
	}
}


void CUserModel::GetVip(uint32_t unUserId, list<uint32_t>& lsPerson)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		string strSql = "select vipid from IMImportantPerson where userid=" + int2string(unUserId) + " and status=0";
		log("SQL for getting vip: %s", strSql.c_str());

		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				uint32_t unVipId = pResultSet->GetInt("vipid");
				lsPerson.push_back(unVipId);
			}

			delete pResultSet;
			pResultSet = NULL;
		}
		else
		{
			log(" no result set for sql:%s", strSql.c_str());
		}

		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_slave");
	}
}


void CUserModel::GetUserInfo(uint32_t unUserId, IM::BaseDefine::UserInfo& UserInfo)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		string strSql = "select * from IMUser where id=" + int2string(unUserId);
		log("SQL for getting user info: %s", strSql.c_str());

		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				string strUserUuid = "", strNickName = "", strAvatar = "", strRealName = "", strPhone = "", strUserName = "", strKeyword = "", strSignature = "", strEmail = "";

				UserInfo.set_user_id(pResultSet->GetInt("id"));

				if (pResultSet->GetString("useruuid") != NULL)
				{
					strUserUuid = pResultSet->GetString("useruuid");
				}

				if (pResultSet->GetString("signature") != NULL)
				{
					strSignature = pResultSet->GetString("signature");
				}

				if (pResultSet->GetString("name") != NULL)
				{
					strRealName = pResultSet->GetString("name");
				}

				if (pResultSet->GetString("username") != NULL)
				{
					strUserName = pResultSet->GetString("username");
				}

				if (pResultSet->GetString("nick") != NULL)
				{
					strNickName = pResultSet->GetString("nick");
				}

				if (strNickName == "")
				{
					strNickName = strRealName;
				}

				if (pResultSet->GetString("keyword") != NULL)
				{
					strKeyword = pResultSet->GetString("keyword");
				}

				if (pResultSet->GetString("phone") != NULL)
				{
					strPhone = pResultSet->GetString("phone");
				}

				int32_t nBirthday = pResultSet->GetInt("birthday");

				if (pResultSet->GetString("avatar") != NULL)
				{
					strAvatar = pResultSet->GetString("avatar");
				}

				if (pResultSet->GetString("email") != NULL)
				{
					strEmail = pResultSet->GetString("email");
				}

				string strAccount = "";
				if (pResultSet->GetString("account") != NULL)
				{
					strAccount = pResultSet->GetString("account");
				}

				UserInfo.set_user_uuid(strUserUuid);
				UserInfo.set_user_name(strUserName);
				UserInfo.set_user_real_name(strRealName);
				UserInfo.set_avatar_url(strAvatar);
				UserInfo.set_user_gender(pResultSet->GetInt("sex"));
				UserInfo.set_birthday(pResultSet->GetInt("birthday"));
				UserInfo.set_mobile_phone(strPhone);
				UserInfo.set_status(pResultSet->GetInt("status"));
				UserInfo.set_keyword(strKeyword);
				UserInfo.set_user_nick_name(strNickName);
				UserInfo.set_signature(strSignature);
				UserInfo.set_email(strEmail);
				UserInfo.set_account(strAccount);
			}

			delete pResultSet;
			pResultSet = NULL;
		}
		else
		{
			log(" no result set for sql:%s", strSql.c_str());
		}

		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_slave");
	}
}


uint32_t CUserModel::GetJobGrade(string& strType)
{
	uint32_t unJobGrade = IM::BaseDefine::NONE;

	if (strType == "chiefleader")
	{
		unJobGrade = IM::BaseDefine::CHIEF_LEADER;
	}
	else if (strType == "leader")
	{
		unJobGrade = IM::BaseDefine::LEADER;
	}
	else if (strType == "staff")
	{
		unJobGrade = IM::BaseDefine::STAFF;
	}
	else if (strType == "other")
	{
		unJobGrade = IM::BaseDefine::OTHER;
	}

	return unJobGrade;
}


void CUserModel::GetStaffInfo(uint32_t unUserId, uint32_t unCompanyId, IM::BaseDefine::StaffInfo& CurrentStaff)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		string strSql = "select * from IMStaff where userid=" + int2string(unUserId) + " and companyid=" + int2string(unCompanyId) + " and isactived=1 and status=1";
		log("SQL for getting staff info: %s", strSql.c_str());

		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				string strUserUuid = "", strStaffUuid = "", strJobNumber = "", strStaffName = "", strPhone = "", strKeyword = "", strBuyProduct = "";

				if (pResultSet->GetString("uuid") != NULL)
				{
					strStaffUuid = pResultSet->GetString("uuid");
				}

				if (pResultSet->GetString("jobnumber") != NULL)
				{
					strJobNumber = pResultSet->GetString("jobnumber");
				}

				if (pResultSet->GetString("staffname") != NULL)
				{
					strStaffName = pResultSet->GetString("staffname");
				}

				if (pResultSet->GetString("mobilephone") != NULL)
				{
					strPhone = pResultSet->GetString("mobilephone");
				}

				if (pResultSet->GetString("keyword") != NULL)
				{
					strKeyword = pResultSet->GetString("keyword");
				}

				if (pResultSet->GetString("useruuid") != NULL)
				{
					strUserUuid = pResultSet->GetString("useruuid");
				}

				if (pResultSet->GetString("buyproduct") != NULL)
				{
					strBuyProduct = pResultSet->GetString("buyproduct");
				}

				string strAccount = "", strUserName = "", strEmail = "", strDefaultEmail = "";
				if (pResultSet->GetString("account") != NULL)
				{
					strAccount = pResultSet->GetString("account");
				}

				if (pResultSet->GetString("username") != NULL)
				{
					strUserName = pResultSet->GetString("username");
				}

				if (pResultSet->GetString("email") != NULL)
				{
					strEmail = pResultSet->GetString("email");
				}

				if (pResultSet->GetString("defaultemail") != NULL)
				{
					strDefaultEmail = pResultSet->GetString("defaultemail");
				}

				int32_t nIsAdmin = pResultSet->GetInt("isadmin");
				int32_t nAdminStatus = pResultSet->GetInt("adminstatus");
				int32_t nIsSuperAdmin = pResultSet->GetInt("issuperadmin");
				int32_t nStatus = pResultSet->GetInt("status");
				int32_t nIsActived = pResultSet->GetInt("isactived");
				int32_t nContactStatus = pResultSet->GetInt("contactstatus");

				//set flag
				uint32_t unFlag = GetUserFlags(nIsAdmin, nAdminStatus, nIsSuperAdmin, nStatus, nIsActived, nContactStatus);
				CurrentStaff.set_flags(unFlag);
				CurrentStaff.set_staff_uuid(strStaffUuid);
				CurrentStaff.set_job_number(strJobNumber);
				CurrentStaff.set_staff_name(strStaffName);
				CurrentStaff.set_mobile_phone(strPhone);
				CurrentStaff.set_company_id(pResultSet->GetInt("companyid"));
				CurrentStaff.set_keyword(strKeyword);
				CurrentStaff.set_user_id(pResultSet->GetInt("userid"));
				CurrentStaff.set_user_uuid(strUserUuid);
				CurrentStaff.set_account(strAccount);
				CurrentStaff.set_user_name(strUserName);
				CurrentStaff.set_email(strEmail);
				CurrentStaff.set_default_email(strDefaultEmail);
				
				uint32_t unBuyProduct = GetProduct(strBuyProduct);
				CurrentStaff.set_buy_product(unBuyProduct);
			}

			delete pResultSet;
			pResultSet = NULL;
		}
		else
		{
			log(" no result set for sql:%s", strSql.c_str());
		}

		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_slave");
	}
}


bool CUserModel::ChangeVip(uint32_t unUserId, uint32_t unVipId, uint32_t unOperationType, uint32_t& unNow)
{
	bool bRet = false;
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");

	if (pDBConn)
	{	
		string strSql = "";
		if (IM::BaseDefine::VIP_MODIFY_TYPE_DEL == unOperationType)
		{
			strSql = "update IMImportantPerson set status=1,updated=unix_timestamp() where userid=" + int2string(unUserId) + " and vipid=" + int2string(unVipId);
			log("SQL for updating vip: %s", strSql.c_str());
			bRet = pDBConn->ExecuteUpdate(strSql.c_str());
		}
		else
		{
			strSql = "insert into IMImportantPerson(userid,vipid,created,updated) values(?,?,?,?)";
			CPrepareStatement* stmt = new CPrepareStatement();
			if (stmt->Init(pDBConn->GetMysql(), strSql))
			{
				unNow = (uint32_t)time(NULL);
				uint32_t index = 0;
				stmt->SetParam(index++, unUserId);
				stmt->SetParam(index++, unVipId);
				//stmt->SetParam(index++, 0);
				stmt->SetParam(index++, unNow);
				stmt->SetParam(index++, unNow);
				bRet = stmt->ExecuteUpdate();
				if (!bRet)
				{
					log("insert vip failed: %s", strSql.c_str());
				}
			}

			delete stmt;
		}

		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_master");
	}

	if (bRet)
	{
		;
		//SyncUserInfo(cUser.user_id(), strAvatar);
	}
	return bRet;
}


void CUserModel::GetAssistantInfo(uint32_t& unLastTime,list<IM::BaseDefine::UserInfo>& lsUserInfo)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		string strSql = "select id,useruuid,name,username,avatar,keyword,updated from IMUser where id < 10001";
		if (unLastTime != 0)
		{
			strSql += " and updated >= " + int2string(unLastTime);
		}
		log("SQL for getting assistant info: %s", strSql.c_str());

		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				string strUserUuid = "", strNickName = "", strAvatar = "", strRealName = "",strUserName = "", strKeyword = "";

				IM::BaseDefine::UserInfo UserInfo;

				UserInfo.set_user_id(pResultSet->GetInt("id"));

				if (pResultSet->GetString("useruuid") != NULL)
				{
					strUserUuid = pResultSet->GetString("useruuid");
				}

				if (pResultSet->GetString("name") != NULL)
				{
					strRealName = pResultSet->GetString("name");
				}

				if (pResultSet->GetString("username") != NULL)
				{
					strUserName = pResultSet->GetString("username");
				}

				if (pResultSet->GetString("nick") != NULL)
				{
					strNickName = pResultSet->GetString("nick");
				}

				if (strNickName == "")
				{
					strNickName = strRealName;
				}

				if (pResultSet->GetString("keyword") != NULL)
				{
					strKeyword = pResultSet->GetString("keyword");
				}

				if (pResultSet->GetString("avatar") != NULL)
				{
					strAvatar = pResultSet->GetString("avatar");
				}

				uint32_t unUpdated = pResultSet->GetInt("updated");
				if (unLastTime < unUpdated)
				{
					unLastTime = unUpdated;
				}

				UserInfo.set_user_uuid(strUserUuid);
				UserInfo.set_user_name(strUserName);
				UserInfo.set_user_real_name(strRealName);
				UserInfo.set_avatar_url(strAvatar);
				UserInfo.set_user_gender(2);
				UserInfo.set_birthday(0);
				UserInfo.set_mobile_phone("");
				UserInfo.set_status(1);
				UserInfo.set_keyword(strKeyword);
				UserInfo.set_user_nick_name(strNickName);
				UserInfo.set_signature("");
				UserInfo.set_email("");

				lsUserInfo.push_back(UserInfo);
			}

			delete pResultSet;
			pResultSet = NULL;
		}
		else
		{
			log(" no result set for sql:%s", strSql.c_str());
		}

		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_slave");
	}
}


bool CUserModel::SyncInfoWithEc(Json::Value& root, uint32_t unSyncType)
{
	bool bRet = false;
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
		string strPostData = "";
		strPostData.append(root.toStyledString());

		CHttpClient HttpClient;
		string strAddiHeader = "";
		string strToken = "";

		HttpClient.GetAdditionalInfo(strToken, strAddiHeader);
		string strPath = "";
		if (unSyncType == 1)
		{
			strPath = "/rs/s/ec/userService/modifyUserInfo";
		}
		else if (unSyncType)
		{
			strPath = "/rs/s/ec/staffService/modifyStaffInfo";
		}
		string strActualPath = m_strHost + strPath + "?rs_token=" + strToken;
		//string strActualPath = "http://ioa." + strDomainID + "/rs/s/ec/userService/modifyUserInfo?userId=" + strUserID + "&rs_token=" + strToken;
		string strResponse;

		HttpClient.Post(strActualPath, strPostData, strResponse, strAddiHeader);
		log("URL: %s, PostData: %s, Response: %s", strActualPath.c_str(), strPostData.c_str(), strResponse.c_str());
		if (strResponse != "")
		{
			try
			{
				Json::Reader Reader;
				Json::Value Root;
				if (Reader.parse(strResponse, Root))
				{
					if (!Root["resultCode"].isNull() && Root["resultCode"].isString())
					{
						string strCode = Root["resultCode"].asString();
						if (strCode == "0000")
						{
							bRet = true;
							log("sync successfully");
						}
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

	return bRet;
}


void CUserModel::GetUserInfo(string& strUserUuid, IM::BaseDefine::UserInfo& UserInfo)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		string strSql = "select * from IMUser where useruuid='" + strUserUuid + "'";
		log("SQL for getting user info: %s", strSql.c_str());

		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				string strUserUuid = "", strNickName = "", strAvatar = "", strRealName = "", strPhone = "", strUserName = "", strKeyword = "", strSignature = "", strEmail = "";

				UserInfo.set_user_id(pResultSet->GetInt("id"));

				if (pResultSet->GetString("useruuid") != NULL)
				{
					strUserUuid = pResultSet->GetString("useruuid");
				}

				if (pResultSet->GetString("signature") != NULL)
				{
					strSignature = pResultSet->GetString("signature");
				}

				if (pResultSet->GetString("name") != NULL)
				{
					strRealName = pResultSet->GetString("name");
				}

				if (pResultSet->GetString("username") != NULL)
				{
					strUserName = pResultSet->GetString("username");
				}

				if (pResultSet->GetString("nick") != NULL)
				{
					strNickName = pResultSet->GetString("nick");
				}

				if (strNickName == "")
				{
					strNickName = strRealName;
				}

				if (pResultSet->GetString("keyword") != NULL)
				{
					strKeyword = pResultSet->GetString("keyword");
				}

				if (pResultSet->GetString("phone") != NULL)
				{
					strPhone = pResultSet->GetString("phone");
				}

				int32_t nBirthday = pResultSet->GetInt("birthday");

				if (pResultSet->GetString("avatar") != NULL)
				{
					strAvatar = pResultSet->GetString("avatar");
				}

				if (pResultSet->GetString("email") != NULL)
				{
					strEmail = pResultSet->GetString("email");
				}

				string strAccount = "";
				if (pResultSet->GetString("account") != NULL)
				{
					strAccount = pResultSet->GetString("account");
				}

				UserInfo.set_user_uuid(strUserUuid);
				UserInfo.set_user_name(strUserName);
				UserInfo.set_user_real_name(strRealName);
				UserInfo.set_avatar_url(strAvatar);
				UserInfo.set_user_gender(pResultSet->GetInt("sex"));
				UserInfo.set_birthday(pResultSet->GetInt("birthday"));
				UserInfo.set_mobile_phone(strPhone);
				UserInfo.set_status(pResultSet->GetInt("status"));
				UserInfo.set_keyword(strKeyword);
				UserInfo.set_user_nick_name(strNickName);
				UserInfo.set_signature(strSignature);
				UserInfo.set_email(strEmail);
				UserInfo.set_account(strAccount);
			}

			delete pResultSet;
			pResultSet = NULL;
		}
		else
		{
			log(" no result set for sql:%s", strSql.c_str());
		}

		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_slave");
	}
}