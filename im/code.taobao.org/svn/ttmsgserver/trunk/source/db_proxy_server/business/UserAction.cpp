/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *
 *   文件名称：UserAction.cpp
 *   创 建 者：Zhang Yuanhao
 *   邮    箱：bluefoxah@gmail.com
 *   创建日期：2014年12月15日
 *   描    述：
 *
 ================================================================*/

#include <list>
#include <map>

#include "../ProxyConn.h"
#include "../DBPool.h"
#include "../SyncCenter.h"
#include "public_define.h"
#include "UserModel.h"
#include "IM.Buddy.pb.h"
#include "IM.BaseDefine.pb.h"
#include "IM.Server.pb.h"
#include "UserAction.h"


namespace DB_PROXY {

	void getUserInfo(CImPdu* pPdu, uint32_t conn_uuid)
	{
		IM::Buddy::IMUsersInfoReq msg;
		IM::Buddy::IMUsersInfoRsp msgResp;

		if (msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
		{
			CImPdu* pPduRes = new CImPdu;

			uint32_t from_user_id = msg.user_id();
			uint32_t userCount = msg.user_id_list_size();

			std::list<uint32_t> idList;
			for (uint32_t i = 0; i < userCount; ++i)
			{
				idList.push_back(msg.user_id_list(i));
			}

			msgResp.set_user_id(from_user_id);

			for (auto it = idList.begin(); it != idList.end(); it++)
			{
				IM::BaseDefine::UserInfo *pUserInfo = msgResp.add_user_info_list();
				IM::BaseDefine::UserInfo  CurrentUserInfo;
				CUserModel::getInstance()->GetUserInfo(*it, CurrentUserInfo);

				//get user basic information
				string strUserUuid = CurrentUserInfo.user_uuid();
				pUserInfo->set_user_id(CurrentUserInfo.user_id());
				pUserInfo->set_user_uuid(CurrentUserInfo.user_uuid());
				pUserInfo->set_user_name(CurrentUserInfo.user_name());
				pUserInfo->set_user_real_name(CurrentUserInfo.user_real_name());
				pUserInfo->set_avatar_url(CurrentUserInfo.avatar_url());
				pUserInfo->set_user_gender(CurrentUserInfo.user_gender());
				pUserInfo->set_birthday(CurrentUserInfo.birthday());
				pUserInfo->set_mobile_phone(CurrentUserInfo.mobile_phone());
				pUserInfo->set_status(CurrentUserInfo.status());
				pUserInfo->set_keyword(CurrentUserInfo.keyword());
				pUserInfo->set_user_nick_name(CurrentUserInfo.user_nick_name());
				pUserInfo->set_signature(CurrentUserInfo.signature());
				pUserInfo->set_email(CurrentUserInfo.email());
				pUserInfo->set_account(CurrentUserInfo.account());

				//get additional information for a user
				list<IM::BaseDefine::UserDetails> lsAddiInfo;
				CUserModel::getInstance()->GetUserAdditionalInfo(strUserUuid, lsAddiInfo);
				for (auto it1 = lsAddiInfo.begin(); it1 != lsAddiInfo.end(); it1++)
				{
					IM::BaseDefine::UserDetails *pAddInfo = pUserInfo->add_add_info_list();
					pAddInfo->CopyFrom(*it1);
					//pAddInfo->set_details_uuid(it1->details_uuid());
					//pAddInfo->set_details_type(it1->details_type());
					//pAddInfo->set_details_title(it1->details_title());
					//pAddInfo->set_details_content(it1->details_content());
				}

				log("User Details: %d", pUserInfo->add_info_list_size());
			}

			msgResp.set_attach_data(msg.attach_data());
			pPduRes->SetPBMsg(&msgResp);
			pPduRes->SetSeqNum(pPdu->GetSeqNum());
			pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
			pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_LIST_USER_INFO_RESPONSE);
			CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
			log("userId=%u, userCnt=%u", from_user_id, userCount);
		}
		else
		{
			log("parse pb failed");
		}
	}

	void getChangedUser(CImPdu* pPdu, uint32_t conn_uuid)
	{
		IM::Buddy::IMAllUserReq msg;
		IM::Buddy::IMAllUserRsp msgResp;
		if (msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
		{
			uint32_t nReqId = msg.user_id();
			uint32_t unLastTime = msg.latest_update_time();

			list<IM::BaseDefine::UserInfo> lsUsers;
			uint32_t unTotalUsers = 0;
			CUserModel *pInstance = CUserModel::getInstance();
			pInstance->GetUsers(nReqId, unLastTime, lsUsers, unTotalUsers);

			uint32_t unTotalCount = 0;
			uint32_t unCount = lsUsers.size();
			msgResp.set_user_id(nReqId);
			msgResp.set_latest_update_time(0);
			msgResp.set_user_count(unTotalUsers);
			msgResp.set_attach_data(msg.attach_data());


			for (list<IM::BaseDefine::UserInfo>::iterator it = lsUsers.begin(); it != lsUsers.end(); ++it)
			{

				IM::BaseDefine::UserInfo* pUser = msgResp.add_user_list();

				string strUserUuid = it->user_uuid();
				pUser->set_user_id(it->user_id());
				pUser->set_user_uuid(it->user_uuid());
				pUser->set_user_name(it->user_name());
				pUser->set_user_real_name(it->user_real_name());
				pUser->set_avatar_url(it->avatar_url());
				pUser->set_user_gender(it->user_gender());
				pUser->set_birthday(it->birthday());
				pUser->set_mobile_phone(it->mobile_phone());
				pUser->set_status(it->status());
				pUser->set_keyword(it->keyword());
				pUser->set_user_nick_name(it->user_nick_name());
				pUser->set_signature(it->signature());
				pUser->set_email(it->email());
				pUser->set_account(it->account());

				//get additional information for a user
				list<IM::BaseDefine::UserDetails> lsAddiInfo;
				pInstance->GetUserAdditionalInfo(strUserUuid, lsAddiInfo);
				for (auto it = lsAddiInfo.begin(); it != lsAddiInfo.end(); it++)
				{
					IM::BaseDefine::UserDetails *pAddInfo = pUser->add_add_info_list();
					pAddInfo->set_details_uuid(it->details_uuid());
					pAddInfo->set_details_type(it->details_type());
					pAddInfo->set_details_title(it->details_title());
					pAddInfo->set_details_content(it->details_content());
				}
				//log("Additional Informations: %d", pUser->add_info_list_size());

				if ((++unTotalCount) % MAX_USER_COUNT_EACH_PACKET == 0) //added by george on April 11th,2016
				{
					//unTotalCount += unMaxCount;
					if (unTotalCount == unCount)
					{
						msgResp.set_latest_update_time(unLastTime); //set actual latest time to indicate completely send all users
					}

					CImPdu* pPduRes = new CImPdu;
					//msgResp.set_attach_data(msg.attach_data());
					pPduRes->SetPBMsg(&msgResp);
					pPduRes->SetSeqNum(pPdu->GetSeqNum());//change sequence?(TBD)
					pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
					pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_LIST_ALL_USER_RESPONSE);
					CProxyConn::AddResponsePdu(conn_uuid, pPduRes);

					log("userId=%u, userCnt=%u, totalcount: %d", nReqId, unCount, unTotalCount);

					//reset data
					msgResp.clear_user_list();
				}
			}

			if (unTotalCount % MAX_USER_COUNT_EACH_PACKET != 0 || unCount == 0)
			{
				msgResp.set_latest_update_time(unLastTime); //set actual latest time to indicate completely send all users
				//msgResp.set_attach_data(msg.attach_data());
				CImPdu* pPduRes = new CImPdu;
				pPduRes->SetPBMsg(&msgResp);
				pPduRes->SetSeqNum(pPdu->GetSeqNum());
				pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
				pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_LIST_ALL_USER_RESPONSE);
				CProxyConn::AddResponsePdu(conn_uuid, pPduRes);

				//unTotalCount += unMaxCount;
				log("userId=%u, last_time=%u, userCnt=%u, totalcount: %d", nReqId, unLastTime, unCount, unTotalCount);
			}


			//	list<uint32_t> lsIds;
			//	uint32_t nCompanyId = 0;
			//	uint32_t unTotalUsers = 0;
			//	CUserModel::getInstance()->GetCompanyId(nReqId, nCompanyId);
			//CUserModel::getInstance()->getUsers(nCompanyId, nLastTime, lsUsers, unTotalUsers);
			///*	CUserModel::getInstance()->getChangedId(nLastTime, lsIds, nCompanyId);
			//	CUserModel::getInstance()->getUsers(lsIds, lsUsers);*/

			//	uint32_t unCount = lsUsers.size();
			//	msgResp.set_user_id(nReqId);
			//	msgResp.set_latest_update_time(0);
			//	msgResp.set_user_count(unTotalUsers);
			//	msgResp.set_attach_data(msg.attach_data());

			//	log("Last time: %u, Total normal users: %d", nLastTime, unTotalUsers);
			//	//set maximum users to send to client each time(currently 50 users each time due to uncertained packet size)
			//	//unsigned int unMaxCount = 0; //added by george on April 11th,2016
			//	uint32_t unTotalCount = 0;

			//	for (list<IM::BaseDefine::UserInfo>::iterator it = lsUsers.begin(); it != lsUsers.end(); ++it)
			//	{

			//		IM::BaseDefine::UserInfo* pUser = msgResp.add_user_list();

			//		pUser->set_user_id(it->user_id());
			//		pUser->set_user_uuid(it->user_uuid());
			//		pUser->set_job_number(it->job_number());
			//		pUser->set_user_name(it->user_name());
			//		pUser->set_user_real_name(it->user_real_name());
			//		pUser->set_avatar_url(it->avatar_url());
			//		pUser->set_user_gender(it->user_gender());
			//		pUser->set_birthday(it->birthday());
			//		pUser->set_mobile_phone(it->mobile_phone());
			//		pUser->set_contact_status(it->contact_status());
			//		pUser->set_status(it->status());
			//		pUser->set_keyword(it->keyword());
			//		pUser->set_user_nick_name(it->user_nick_name());
			//		pUser->set_signature(it->signature());
			//		pUser->set_companyname(it->companyname());
			//		pUser->set_buyproduct(it->buyproduct());

			//		//get departs and positions
			//		int nTotals = it->duty_list_size();
			//		for (int i = 0; i < nTotals; i++)
			//		{
			//			IM::BaseDefine::DutyInfo *pDuty = pUser->add_duty_list();
			//			IM::BaseDefine::DutyInfo Duty = it->duty_list(i);

			//			pDuty->set_depart_uuid(Duty.depart_uuid());
			//			pDuty->set_depart_name(Duty.depart_name());
			//			pDuty->set_status(Duty.status());

			//			int nCount = Duty.position_list_size();
			//			for (int j = 0; j < nCount; j++) //get positions for a depart
			//			{
			//				IM::BaseDefine::PositionInfo CurrPosition = Duty.position_list(j);
			//				IM::BaseDefine::PositionInfo *pPosition = pDuty->add_position_list();
			//				pPosition->set_position_uuid(CurrPosition.position_uuid());
			//				pPosition->set_position_type(CurrPosition.position_type());
			//				pPosition->set_position_name(CurrPosition.position_name());
			//				pPosition->set_status(CurrPosition.status());
			//				pPosition->set_sort(CurrPosition.sort());
			//			}
			//			//log("Position List Size: %d for Depart %d", pDuty->position_list_size(), i);
			//		}
			//		//log("Duty List Size: %d", pUser->duty_list_size());

			//		//get shield users 
			//		nTotals = it->black_list_size();
			//		for (int i = 0; i < nTotals; i++)
			//		{
			//			IM::BaseDefine::ShieldUser ShieldUser = it->black_list(i);
			//			IM::BaseDefine::ShieldUser* pShieldUser = pUser->add_black_list();
			//			pShieldUser->set_user_id(ShieldUser.user_id());
			//			pShieldUser->set_shield_status(ShieldUser.shield_status());
			//		}
			//		//log("Black List Size: %d", pUser->black_list_size());

			//		//get additional information
			//		nTotals = it->add_info_list_size();
			//		for (int i = 0; i < nTotals; i++)
			//		{
			//			//get additional information for a user
			//			IM::BaseDefine::UserDetails *pAddInfo = pUser->add_add_info_list();
			//			IM::BaseDefine::UserDetails AddInfo = it->add_info_list(i);
			//			pAddInfo->set_details_uuid(AddInfo.details_uuid());
			//			pAddInfo->set_details_type(AddInfo.details_type());
			//			pAddInfo->set_details_title(AddInfo.details_title());
			//			pAddInfo->set_details_content(AddInfo.details_content());
			//		}
			//		//log("Additional Informations: %d", pUser->add_info_list_size());

			//		//if (++unMaxCount == MAX_USER_COUNT_EACH_PACKET) //added by george on April 11th,2016
			//		if ((++unTotalCount) % MAX_USER_COUNT_EACH_PACKET == 0) //added by george on April 11th,2016
			//		{
			//			//unTotalCount += unMaxCount;
			//			if (unTotalCount == unCount)
			//			{
			//				msgResp.set_latest_update_time(nLastTime); //set actual latest time to indicate completely send all users
			//			}

			//			CImPdu* pPduRes = new CImPdu;
			//			//msgResp.set_attach_data(msg.attach_data());
			//			pPduRes->SetPBMsg(&msgResp);
			//			pPduRes->SetSeqNum(pPdu->GetSeqNum());//change sequence?(TBD)
			//			pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
			//			pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_LIST_ALL_USER_RESPONSE);
			//			CProxyConn::AddResponsePdu(conn_uuid, pPduRes);

			//			log("userId=%u, userCnt=%u, totalcount: %d", nReqId, unCount, unTotalCount);

			//			//reset data
			//			//unMaxCount = 0;
			//			msgResp.clear_user_list();
			//		}
			//	}

			//	//if (unMaxCount > 0 || unCount == 0)
			//	if (unTotalCount % MAX_USER_COUNT_EACH_PACKET != 0 || unCount == 0)
			//	{
			//		msgResp.set_latest_update_time(nLastTime); //set actual latest time to indicate completely send all users
			//		//msgResp.set_attach_data(msg.attach_data());
			//		CImPdu* pPduRes = new CImPdu;
			//		pPduRes->SetPBMsg(&msgResp);
			//		pPduRes->SetSeqNum(pPdu->GetSeqNum());
			//		pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
			//		pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_LIST_ALL_USER_RESPONSE);
			//		CProxyConn::AddResponsePdu(conn_uuid, pPduRes);

			//		//unTotalCount += unMaxCount;
			//		log("userId=%u, last_time=%u, userCnt=%u, totalcount: %d", nReqId, nLastTime, unCount, unTotalCount);
			//	}
		}
		else
		{
			log("parse pb failed");
		}
	}


	void UpdateUserNagure(CImPdu* pPdu, uint32_t conn_uuid)
	{
		IM::Buddy::IMUpdateUserNagureReq msg;
		IM::Buddy::IMUpdateUserNagureRsp msgResp;
		if (msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
		{
			CImPdu* pPduRes = new CImPdu;
			uint32_t nReqId = msg.user_id();
			std::string strNagure = msg.nagure();
			uint32_t nNow = (uint32_t)time(NULL);
			bool bRet = CUserModel::getInstance()->updateNagure(nReqId, strNagure, nNow);
			/*if (bRet)
			{*/
			msgResp.set_user_id(nReqId);
			msgResp.set_nagure(strNagure);
			msgResp.set_latest_update_time(bRet ? nNow : 0);
			msgResp.set_attach_data(msg.attach_data());
			pPduRes->SetPBMsg(&msgResp);
			pPduRes->SetSeqNum(pPdu->GetSeqNum());
			pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
			pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_LIST_USER_UPDATE_NAGURE_RESPONSE);
			CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
			/*}*/

			return;
		}
		else
		{
			log("parse pb failed");
		}
	}


	//void UpdateUser(CImPdu* pPdu, uint32_t conn_uuid)
	//{
	//	IM::Buddy::IMUsersInfoRsp msg;
	//	IM::Buddy::IMUpdateUserRsp msgResp;
	//	if(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
	//	{
	//		CImPdu* pPduRes = new CImPdu;
	//		uint32_t nReqId = msg.user_id();

	//		//to be modified
	//	//	IM::BaseDefine::UserInfo tUserInfo = msg.user_info_list(0);
	//	//	DBUserInfo_t tDbUserInfo;
	//	//	tDbUserInfo.nSex = tUserInfo.user_gender();
	//	//	tDbUserInfo.strSignature = tUserInfo.signature();
	//	//	tDbUserInfo.strPhone = tUserInfo.mobile_phone();

	//	//	tDbUserInfo.strBirthday = tUserInfo.birthday();
	//	//	tDbUserInfo.strAvatar = tUserInfo.avatar_url();
	//	//	tDbUserInfo.nId = nReqId;
	//	//	uint32_t nNow = (uint32_t)time(NULL);
	//	//	bool bRet = CUserModel::getInstance()->updateUser(tDbUserInfo,nNow);
	//	///*	if (bRet)
	//	//	{*/
	//	//		msgResp.set_user_id(nReqId);
	//	//		msgResp.set_strsignature(tDbUserInfo.strSignature);
	//	//		msgResp.set_nsex(tDbUserInfo.nSex);
	//	//		msgResp.set_strphone(tDbUserInfo.strPhone);
	//	//		msgResp.set_strtelephone(tDbUserInfo.strTel);
	//	//		msgResp.set_strfaxnum(tDbUserInfo.strFaxNum);
	//	//		msgResp.set_strpostcode(tDbUserInfo.strPostCode);
	//	//		msgResp.set_straddress(tDbUserInfo.strAddress);
	//	//		msgResp.set_strbirthday(tDbUserInfo.strBirthday);
	//	//		msgResp.set_stravatar(tDbUserInfo.strAvatar);
	//	//		msgResp.set_latest_update_time(bRet ? nNow: 0);
	//	//		msgResp.set_attach_data(msg.attach_data());


	//			pPduRes->SetPBMsg(&msgResp);
	//			pPduRes->SetSeqNum(pPdu->GetSeqNum());
	//			pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
	//			pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_LIST_USER_UPDATE_INFO_RESPONSE);
	//			CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
	//		/*}*/
	//		return;
	//	}
	//	else
	//	{
	//		log("parse pb failed");
	//	}
	//}


	void UpdateUser(CImPdu* pPdu, uint32_t conn_uuid)
	{
		IM::Buddy::IMUpdateUserReq msg;
		IM::Buddy::IMUpdateUserRsp msgResp;
		if (msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
		{
			CImPdu* pPduRes = new CImPdu;
			uint32_t nReqId = msg.user_id();
			uint32_t nNow = (uint32_t)time(NULL);
			bool bRet = false;

			string strUserUuid = msg.user_uuid();
			uint32_t unSex = msg.sex();
			int32_t nBirthday = msg.birthday();
			string strSignature = msg.signature();
			string strRealName = msg.user_real_name();
			list<IM::BaseDefine::UserDetails> lsDetails;
			int nCount = msg.user_details_list_size();
			for (int i = 0; i < nCount; i++)
			{
				IM::BaseDefine::UserDetails UserDetails = msg.user_details_list(i);
				lsDetails.push_back(UserDetails);
			}

			log("User Details Size: %d", nCount);
			bRet = CUserModel::getInstance()->UpdateUser(strUserUuid, unSex, nBirthday, strSignature, strRealName, lsDetails, nNow);

			msgResp.set_user_id(nReqId);
			msgResp.set_user_uuid(strUserUuid);
			msgResp.set_signature(strSignature);
			msgResp.set_sex(unSex);
			msgResp.set_birthday(nBirthday);
			msgResp.set_user_real_name(strRealName);

			for (auto it = lsDetails.begin(); it != lsDetails.end(); it++)
			{
				IM::BaseDefine::UserDetails *pUserDetails = msgResp.add_user_details_list();
				pUserDetails->set_details_uuid(it->details_uuid());
				pUserDetails->set_details_type(it->details_type());
				pUserDetails->set_details_title(it->details_title());
				pUserDetails->set_details_content(it->details_content());
			}

			msgResp.set_latest_update_time(bRet ? nNow : 0); //0 meas failed to update
			msgResp.set_attach_data(msg.attach_data());

			pPduRes->SetPBMsg(&msgResp);
			pPduRes->SetSeqNum(pPdu->GetSeqNum());
			pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
			pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_LIST_USER_UPDATE_INFO_RESPONSE);
			CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
			return;
		}
		else
		{
			log("parse pb failed");
		}
	}

	void updateUserAvatar(CImPdu* pPdu, uint32_t conn_uuid)
	{
		IM::Buddy::IMChangeAvatarReq msg;
		IM::Buddy::IMChangeAvatarAndNotifyRsp msgResp;
		//IM::Buddy::IMChangeAvatarRsp msgResp;
		if (msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
		{
			CImPdu* pPduRes = new CImPdu;
			uint32_t nUserId = msg.user_id();
			string strAvatarUrl = msg.avatar_url();
			log("Userid=%u,Avatar Url:%s", nUserId, strAvatarUrl.c_str());
			bool bRet = CUserModel::getInstance()->updateUserAvatar(nUserId, strAvatarUrl);
			msgResp.set_user_id(nUserId);
			msgResp.set_result_code(bRet ? 0 : 1);
			msgResp.set_avatar_url(strAvatarUrl);
			msgResp.set_attach_data(msg.attach_data());
			pPduRes->SetPBMsg(&msgResp);
			pPduRes->SetSeqNum(pPdu->GetSeqNum());
			pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
			pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_LIST_CHANGE_AVATAR_RESPONSE);
			CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
			return;
		}
		else
		{
			log("parse pb failed");
		}
	}


	void setUserPush(CImPdu* pPdu, uint32_t conn_uuid)
	{
		IM::Buddy::IMUserShieldReq msg;
		IM::Buddy::IMUserShieldRsp msgResp;
		int nResult = 0;
		if (msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
		{
			uint32_t nUserId = msg.user_id();
			uint32_t nPeerId = msg.peer_id();
			uint32_t nStatus = msg.shield_status();

			CImPdu* pPduRes = new CImPdu;

			if (CUserModel::getInstance()->IsValidUser(nUserId))
			{
				bool bRet = CUserModel::getInstance()->setPush(nUserId, nPeerId, IM_USER_SETTING_PUSH, nStatus);
				bRet = CUserModel::getInstance()->SetShieldStatus(nUserId, nPeerId, nStatus);
				nResult = bRet ? 0 : 1;

				/* msgResp.set_user_id(nUserId);
				msgResp.set_group_id(nGroupId);
				msgResp.set_result_code(nResult);*/

				log("userId=%u, PeerId=%u, result=%u", nUserId, nPeerId, nResult);

				/*   msgResp.set_attach_data(msg.attach_data());
				pPduRes->SetPBMsg(&msgResp);
				pPduRes->SetSeqNum(pPdu->GetSeqNum());
				pPduRes->SetServiceId(IM::BaseDefine::SID_GROUP);
				pPduRes->SetCommandId(IM::BaseDefine::CID_GROUP_SHIELD_GROUP_RESPONSE);
				CProxyConn::AddResponsePdu(conn_uuid, pPduRes);*/
			}
			else
			{
				log("Invalid userId=%u", nPeerId);
				nResult = 0x2000;
			}

			msgResp.set_user_id(nUserId);
			msgResp.set_peer_id(nPeerId);
			msgResp.set_result_code(nResult);
			msgResp.set_attach_data(msg.attach_data());
			pPduRes->SetPBMsg(&msgResp);
			pPduRes->SetSeqNum(pPdu->GetSeqNum());
			pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
			pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_SHIELD_USER_RESPONSE);
			CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
		}
		else
		{
			log("parse pb failed");
		}
	}

	/**
	*  获取个人用户的推送设置
	*
	*  @param pPdu      收到的packet包指针
	*  @param conn_uuid 该包过来的socket 描述符
	*/
	void getUserPush(CImPdu* pPdu, uint32_t conn_uuid)
	{
		IM::Server::IMUserGetShieldReq msg;
		IM::Server::IMUserGetShieldRsp msgResp;
		if (msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
		{
			uint32_t nToID = msg.to_user_id();
			uint32_t nFromID = msg.from_user_id();

			if (CUserModel::getInstance()->IsValidUser(nToID))
			{
				CImPdu* pPduRes = new CImPdu;

				list<IM::BaseDefine::ShieldUser> lsPush;
				CUserModel::getInstance()->getPush(nToID, lsPush);

				list<IM::BaseDefine::ShieldUser>::iterator it = lsPush.begin();
				for (; it != lsPush.end(); ++it)
				{
					if (it->user_id() == nFromID)
					{
						break;
					}
				}

				msgResp.set_to_user_id(nToID);
				IM::BaseDefine::ShieldUser* pStatus = msgResp.mutable_shield_status();
				if (it == lsPush.end())
				{
					pStatus->set_user_id(nFromID);
					pStatus->set_shield_status(0);
				}
				else
				{
					pStatus->set_user_id(it->user_id());
					pStatus->set_shield_status(it->shield_status());
				}

				log("to_userId=%u, from_usrId: %d, shield status: %d", nToID, pStatus->user_id(), pStatus->shield_status());

				msgResp.set_attach_data(msg.attach_data());
				pPduRes->SetPBMsg(&msgResp);
				pPduRes->SetSeqNum(pPdu->GetSeqNum());
				pPduRes->SetServiceId(IM::BaseDefine::SID_OTHER);
				pPduRes->SetCommandId(IM::BaseDefine::CID_OTHER_GET_SHIELD_USER_RSP);
				CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
			}
			else
			{
				log("Invalid userId. USER ID=%u", nToID);
			}
		}
		else
		{
			log("parse pb failed");
		}
	}


	void getChangedUserLite(CImPdu* pPdu, uint32_t conn_uuid)
	{
		//IM::Buddy::IMAllUserLiteReq msg;
		//IM::Buddy::IMAllUserLiteRsp msgResp;
		//if (msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
		//{
		//	uint32_t nReqId = msg.user_id();
		//	uint32_t nLastTime = msg.latest_update_time();

		//	list<IM::BaseDefine::UserInfo> lsUsers;

		//	list<uint32_t> lsIds;
		//	uint32_t nCompanyId = 0;
		//	uint32_t unTotalUsers = 0;
		//	CUserModel::getInstance()->GetCompanyId(nReqId, nCompanyId);
		//	CUserModel::getInstance()->getUsers(nCompanyId, nLastTime, lsUsers, unTotalUsers);
		//	//CUserModel::getInstance()->getChangedId(nLastTime, lsIds, nCompanyId);
		//	//CUserModel::getInstance()->getUsers(lsIds, lsUsers);

		//	uint32_t unCount = lsUsers.size();
		//	msgResp.set_user_id(nReqId);
		//	msgResp.set_latest_update_time(0);
		//	//msgResp.set_user_count(unTotalUsers);
		//	msgResp.set_attach_data(msg.attach_data());

		//	//set maximum users to send to client each time(currently 50 users each time due to uncertained packet size)
		//	//unsigned int unMaxCount = 0; //added by george on April 11th,2016
		//	uint32_t unTotalCount = 0; //for test
		//	log("Last time: %u", nLastTime);

		//	for (list<IM::BaseDefine::UserInfo>::iterator it = lsUsers.begin(); it != lsUsers.end(); ++it)
		//	{

		//		IM::BaseDefine::UserInfoBase* pUser = msgResp.add_user_list();

		//		//required uint32  flags = 4; //标志位(按从低位到高位顺序排列最后2位代表性别；第3位代表通讯录显示状态；第4和5位代表用户状态，6-8位保留位)
		//		////说明: 性别中00表示女性；01:男性；10:未知，通讯录显示状态中0表示不显示；1表示显示，用户状态中00表示禁用；01:表示启用；10表示删除
		//		//required uint32 buy_product = 5; //用户订购产品(16位中每一位代表一种产品。1表示开通该产品；0则表示未开通。就目前开发的产品来看从低位到高位的每一位分别代表IM、审批、邮箱、公告、云盘和通讯录)。

		//		pUser->set_user_id(it->user_id());
		//		pUser->set_user_uuid(it->user_uuid());
		//		pUser->set_user_real_name(it->user_real_name());
		//		pUser->set_avatar_url(it->avatar_url());
		//		
		//		//uint32_t unFlag = 0; //lower 2 bits for gender; lower 3 means contact status;lower 4 and 5 means user status
		//		uint32_t unFlag = GetUserFlags(it->user_gender(), it->contact_status(), it->status());
		//		pUser->set_flags(unFlag);


		//		string strBuyProduct = it->buyproduct();
		//		uint32_t unBuyProduct = GetProduct(strBuyProduct);
		//		pUser->set_buy_product(unBuyProduct);
		//		pUser->set_user_name(it->user_name());
		//		pUser->set_keyword(it->keyword());

		//		//get departs and positions
		//		int nTotals = it->duty_list_size();
		//		for (int i = 0; i < nTotals; i++)
		//		{
		//			IM::BaseDefine::DutyInfoLite *pDuty = pUser->add_duty_list();
		//			IM::BaseDefine::DutyInfo Duty = it->duty_list(i);

		//			pDuty->set_depart_uuid(Duty.depart_uuid());

		//			int nCount = Duty.position_list_size();
		//			for (int j = 0; j < nCount; j++) //get positions for a depart
		//			{
		//				IM::BaseDefine::PositionInfo CurrPosition = Duty.position_list(j);
		//				pDuty->add_position_uuid_list(CurrPosition.position_uuid());
		//			}
		//			//log("Position List Size: %d for Depart %d", pDuty->position_list_size(), i);
		//		}
		//		//log("Duty List Size: %d", pUser->duty_list_size());

		//		//get shield users 
		//		nTotals = it->black_list_size();
		//		for (int i = 0; i < nTotals; i++)
		//		{
		//			IM::BaseDefine::ShieldUser ShieldUser = it->black_list(i);
		//			IM::BaseDefine::ShieldUser* pShieldUser = pUser->add_black_list();
		//			pShieldUser->set_user_id(ShieldUser.user_id());
		//			pShieldUser->set_shield_status(ShieldUser.shield_status());
		//		}
		//		//log("Black List Size: %d", pUser->black_list_size());


		//		//if (++unMaxCount == MAX_USER_COUNT_EACH_PACKET_LITE) //added by george on April 11th,2016
		//		if ((++unTotalCount) % MAX_USER_COUNT_EACH_PACKET_LITE == 0) //added by george on April 11th,2016
		//		{
		//			if (unTotalCount == unCount)
		//			{
		//				msgResp.set_latest_update_time(nLastTime); //set actual latest time to indicate completely send all users
		//			}

		//			CImPdu* pPduRes = new CImPdu;
		//			//msgResp.set_attach_data(msg.attach_data());
		//			pPduRes->SetPBMsg(&msgResp);
		//			pPduRes->SetSeqNum(pPdu->GetSeqNum());//change sequence?(TBD)
		//			pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
		//			pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_LIST_ALL_USER_LITE_RESPONSE);
		//			CProxyConn::AddResponsePdu(conn_uuid, pPduRes);

		//			//unTotalCount += unMaxCount; //for test
		//			log("userId=%u, userCnt=%u, TotalCount: %d", nReqId, unCount, unTotalCount);

		//			//reset data
		//			//unMaxCount = 0;
		//			msgResp.clear_user_list();
		//		}
		//	}

		//	//if (unMaxCount > 0 || unCount == 0)
		//	if (unTotalCount % MAX_USER_COUNT_EACH_PACKET_LITE != 0 || unCount == 0)
		//	{
		//		msgResp.set_latest_update_time(nLastTime); //set actual latest time to indicate completely send all users
		//		//unTotalCount += unMaxCount;
		//		//msgResp.set_attach_data(msg.attach_data());
		//		CImPdu* pPduRes = new CImPdu;
		//		pPduRes->SetPBMsg(&msgResp);
		//		pPduRes->SetSeqNum(pPdu->GetSeqNum());
		//		pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
		//		pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_LIST_ALL_USER_LITE_RESPONSE);
		//		CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
		//		log("userId=%u, last_time=%u, userCnt=%u, TotalCount: %d", nReqId, nLastTime, unCount, unTotalCount);
		//	}
		//}
		//else
		//{
		//	log("parse pb failed");
		//}

	}


	void getUserInfoLite(CImPdu* pPdu, uint32_t conn_uuid)
	{
		//IM::Buddy::IMUserInfoLiteReq msg;
		//IM::Buddy::IMUserInfoLiteRsp msgResp;
		//if (msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
		//{
		//	CImPdu* pPduRes = new CImPdu;

		//	uint32_t from_user_id = msg.user_id();
		//	uint32_t userCount = msg.user_id_list_size();

		//	std::list<uint32_t> idList;
		//	for (uint32_t i = 0; i < userCount; ++i) {
		//		idList.push_back(msg.user_id_list(i));
		//	}
		//	std::list<IM::BaseDefine::UserInfo> lsUser;
		//	CUserModel::getInstance()->getUsers(idList, lsUser);
		//	msgResp.set_user_id(from_user_id);

		//	for (list<IM::BaseDefine::UserInfo>::iterator it = lsUser.begin();
		//		it != lsUser.end(); ++it)
		//	{
		//		IM::BaseDefine::UserInfoLite* pUser = msgResp.add_user_info_list();

		//		IM::BaseDefine::UserInfoBase *pUserInfo = pUser->mutable_base_info();
		//		pUserInfo->set_user_id(it->user_id());
		//		pUserInfo->set_user_uuid(it->user_uuid());
		//		pUserInfo->set_user_real_name(it->user_real_name());
		//		pUserInfo->set_avatar_url(it->avatar_url());

		//		uint32_t unFlag = GetUserFlags(it->user_gender(), it->contact_status(), it->status());
		//		pUserInfo->set_flags(unFlag);

		//		string strBuyProduct = it->buyproduct();
		//		uint32_t unBuyProduct = GetProduct(strBuyProduct);

		//		pUserInfo->set_buy_product(unBuyProduct);
		//		pUserInfo->set_user_name(it->user_name());
		//		pUserInfo->set_keyword(it->keyword());
		//		pUser->set_job_number(it->job_number());
		//		
		//		pUser->set_birthday(it->birthday());
		//		pUser->set_mobile_phone(it->mobile_phone());
		//		pUser->set_user_nick_name(it->user_nick_name());
		//		pUser->set_signature(it->signature());

		//		//get departs and positions
		//		int nTotals = it->duty_list_size();
		//		for (int i = 0; i < nTotals; i++)
		//		{
		//			IM::BaseDefine::DutyInfoLite *pDuty = pUserInfo->add_duty_list();
		//			IM::BaseDefine::DutyInfo Duty = it->duty_list(i);

		//			pDuty->set_depart_uuid(Duty.depart_uuid());

		//			int nCount = Duty.position_list_size();
		//			for (int j = 0; j < nCount; j++) //get positions for a depart
		//			{
		//				IM::BaseDefine::PositionInfo CurrPosition = Duty.position_list(j);
		//				pDuty->add_position_uuid_list(CurrPosition.position_uuid());

		//			}
		//			//log("Position List Size: %d for Depart %d", pDuty->position_uuid_list_size(), i);
		//		}
		//		log("Duty List Size: %d", pUserInfo->duty_list_size());

		//		//get shield users 
		//		nTotals = it->black_list_size();
		//		for (int i = 0; i < nTotals; i++)
		//		{
		//			IM::BaseDefine::ShieldUser ShieldUser = it->black_list(i);
		//			IM::BaseDefine::ShieldUser* pShieldUser = pUserInfo->add_black_list();
		//			pShieldUser->set_user_id(ShieldUser.user_id());
		//			pShieldUser->set_shield_status(ShieldUser.shield_status());
		//		}
		//		//log("Black List Size: %d", pUserInfo->black_list_size());

		//		//get additional information
		//		nTotals = it->add_info_list_size();
		//		for (int i = 0; i < nTotals; i++)
		//		{
		//			//get additional information for a user
		//			IM::BaseDefine::UserDetails *pAddInfo = pUser->add_add_info_list();
		//			IM::BaseDefine::UserDetails AddInfo = it->add_info_list(i);
		//			pAddInfo->set_details_uuid(AddInfo.details_uuid());
		//			pAddInfo->set_details_type(AddInfo.details_type());
		//			pAddInfo->set_details_title(AddInfo.details_title());
		//			pAddInfo->set_details_content(AddInfo.details_content());
		//		}
		//		//log("Additional Informations: %d", pUser->add_info_list_size());

		//	}

		//	msgResp.set_attach_data(msg.attach_data());
		//	pPduRes->SetPBMsg(&msgResp);
		//	pPduRes->SetSeqNum(pPdu->GetSeqNum());
		//	pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
		//	pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_LIST_USER_INFO_LITE_RESPONSE);
		//	CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
		//	log("userId=%u, userCnt=%u", from_user_id, userCount);
		//}
		//else
		//{
		//	log("parse pb failed");
		//}

	}

	void GetChangedStaff(CImPdu* pPdu, uint32_t conn_uuid)
	{
		IM::Buddy::IMAllStaffReq msg;
		IM::Buddy::IMAllStaffRsp msgResp;
		if (msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
		{
			uint32_t unReqId = msg.user_id();
			uint32_t unLastTime = msg.latest_update_time();
			uint32_t unCompanyId = msg.company_id();

			list<IM::BaseDefine::StaffInfo> lsStaffInfo;
			uint32_t unTotalUsers = 0;
			CUserModel *pInstance = CUserModel::getInstance();
			pInstance->GetCompanyStaff(unReqId, unCompanyId, unLastTime, lsStaffInfo, unTotalUsers);

			uint32_t unTotalCount = 0;
			uint32_t unCount = lsStaffInfo.size();
			msgResp.set_user_id(unReqId);
			msgResp.set_company_id(unCompanyId);
			msgResp.set_latest_update_time(0);
			msgResp.set_staff_count(unTotalUsers);
			msgResp.set_attach_data(msg.attach_data());


			for (auto it = lsStaffInfo.begin(); it != lsStaffInfo.end(); ++it)
			{

				IM::BaseDefine::StaffInfo* pStaffInfo = msgResp.add_staff_info_list();
				string strStaffUuid = it->staff_uuid();
				unCompanyId = it->company_id();
				pStaffInfo->set_staff_uuid(strStaffUuid);
				pStaffInfo->set_job_number(it->job_number());
				pStaffInfo->set_staff_name(it->staff_name());
				pStaffInfo->set_mobile_phone(it->mobile_phone());
				pStaffInfo->set_company_id(unCompanyId);
				pStaffInfo->set_keyword(it->keyword());
				pStaffInfo->set_user_id(it->user_id());
				pStaffInfo->set_user_uuid(it->user_uuid());
				pStaffInfo->set_flags(it->flags());
				pStaffInfo->set_account(it->account());
				pStaffInfo->set_user_name(it->user_name());
				pStaffInfo->set_email(it->email());
				pStaffInfo->set_default_email(it->default_email());
				//pStaffInfo->set_buy_product(it->buy_product());

				//get staff details;

				list<IM::BaseDefine::UserDetails> lsStaffDetails;
				pInstance->GetUserAdditionalInfo(strStaffUuid, lsStaffDetails, 2);
				for (auto it = lsStaffDetails.begin(); it != lsStaffDetails.end(); it++)
				{
					IM::BaseDefine::UserDetails *pStaffDetails = pStaffInfo->add_add_info_list();
					pStaffDetails->set_details_uuid(it->details_uuid());
					pStaffDetails->set_details_type(it->details_type());
					pStaffDetails->set_details_title(it->details_title());
					pStaffDetails->set_details_content(it->details_content());
				}
				log("Staff additional information: %d", lsStaffDetails.size());

				//get staff duty
				list<IM::BaseDefine::DutyInfo> lsDutyInfo;
				map<string, list<string>> mapPosition;
				pInstance->GetDuty(strStaffUuid, lsDutyInfo, mapPosition, unCompanyId);
				for (auto it = lsDutyInfo.begin(); it != lsDutyInfo.end(); it++) //get departments for a user
				{
					IM::BaseDefine::DutyInfo *pDutyInfo = pStaffInfo->add_duty_list();
					pDutyInfo->set_depart_uuid(it->depart_uuid());
					pDutyInfo->set_job_grade(it->job_grade());

					auto it1 = mapPosition.find(it->depart_uuid());
					if (it1 != mapPosition.end())
					{
						for (auto it2 = it1->second.begin(); it2 != it1->second.end(); it2++) //get position for a department
						{
							pDutyInfo->add_position_uuid_list(*it2);
						}
						log("Position List Size: %d for Depart: %s", pDutyInfo->position_uuid_list_size(), it->depart_uuid().c_str());
					}
				}
				log("Duty List Size: %d", pStaffInfo->duty_list_size());


				if ((++unTotalCount) % MAX_USER_COUNT_EACH_PACKET == 0) //added by george on April 11th,2016
				{
					//unTotalCount += unMaxCount;
					if (unTotalCount == unCount)
					{
						msgResp.set_latest_update_time(unLastTime); //set actual latest time to indicate completely send all users
					}

					CImPdu* pPduRes = new CImPdu;
					//msgResp.set_attach_data(msg.attach_data());
					pPduRes->SetPBMsg(&msgResp);
					pPduRes->SetSeqNum(pPdu->GetSeqNum());//change sequence?(TBD)
					pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
					pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_LIST_ALL_STAFF_RESPONSE);
					CProxyConn::AddResponsePdu(conn_uuid, pPduRes);

					log("userId=%u, userCnt=%u, totalcount: %d", unReqId, unCount, unTotalCount);

					//reset data
					msgResp.clear_staff_info_list();
				}
			}

			if (unTotalCount % MAX_USER_COUNT_EACH_PACKET != 0 || unCount == 0)
			{
				msgResp.set_latest_update_time(unLastTime); //set actual latest time to indicate completely send all users
				//msgResp.set_attach_data(msg.attach_data());
				CImPdu* pPduRes = new CImPdu;
				pPduRes->SetPBMsg(&msgResp);
				pPduRes->SetSeqNum(pPdu->GetSeqNum());
				pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
				pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_LIST_ALL_STAFF_RESPONSE);
				CProxyConn::AddResponsePdu(conn_uuid, pPduRes);

				//unTotalCount += unMaxCount;
				log("userId=%u, last_time=%u, userCnt=%u, totalcount: %d", unReqId, unLastTime, unCount, unTotalCount);
			}
		}
		else
		{
			log("parse pb failed");
		}
	}


	void ChangeVip(CImPdu* pPdu, uint32_t conn_uuid)
	{
		IM::Buddy::IMChangeVipRequest msg;
		IM::Buddy::IMChangeVipResponse msgResp;
		if (msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
		{
			CImPdu* pPduRes = new CImPdu;
			uint32_t unReqId = msg.user_id();
			uint32_t unVipId = msg.vip_id();
			uint32_t unOperationType = msg.operation_type();
			uint32_t nNow = 0;
			bool bRet = false;

			bRet = CUserModel::getInstance()->ChangeVip(unReqId, unVipId, unOperationType, nNow);

			msgResp.set_user_id(unReqId);
			msgResp.set_vip_id(unVipId);
			msgResp.set_operation_type((IM::BaseDefine::VipModifyType)unOperationType);
			msgResp.set_latest_update_time(bRet ? nNow : 0); //0 meas failed to update
			msgResp.set_attach_data(msg.attach_data());

			pPduRes->SetPBMsg(&msgResp);
			pPduRes->SetSeqNum(pPdu->GetSeqNum());
			pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
			pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_LIST_CHANGE_VIP_RESPONSE);
			CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
			return;
		}
		else
		{
			log("parse pb failed");
		}
	}

	void GetStaffInfo(CImPdu* pPdu, uint32_t conn_uuid)
	{
		IM::Buddy::IMStaffInfoReq msg;
		IM::Buddy::IMStaffInfoRsp msgResp;
		if (msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
		{
			CImPdu* pPduRes = new CImPdu;

			uint32_t from_user_id = msg.user_id();
			uint32_t userCount = msg.user_id_list_size();

			std::list<uint32_t> idList;
			for (uint32_t i = 0; i < userCount; ++i)
			{
				idList.push_back(msg.user_id_list(i));
			}

			msgResp.set_user_id(from_user_id); 

			for (auto it = idList.begin(); it != idList.end(); it++)
			{
				IM::BaseDefine::UserInfo *pUserInfo = msgResp.add_user_info_list();
				IM::BaseDefine::UserInfo  CurrentUserInfo;
				CUserModel::getInstance()->GetUserInfo(*it, CurrentUserInfo);

				//get user basic information
				string strUserUuid = CurrentUserInfo.user_uuid();
				pUserInfo->set_user_id(CurrentUserInfo.user_id());
				pUserInfo->set_user_uuid(CurrentUserInfo.user_uuid());
				pUserInfo->set_user_name(CurrentUserInfo.user_name());
				pUserInfo->set_user_real_name(CurrentUserInfo.user_real_name());
				pUserInfo->set_avatar_url(CurrentUserInfo.avatar_url());
				pUserInfo->set_user_gender(CurrentUserInfo.user_gender());
				pUserInfo->set_birthday(CurrentUserInfo.birthday());
				pUserInfo->set_mobile_phone(CurrentUserInfo.mobile_phone());
				pUserInfo->set_status(CurrentUserInfo.status());
				pUserInfo->set_keyword(CurrentUserInfo.keyword());
				pUserInfo->set_user_nick_name(CurrentUserInfo.user_nick_name());
				pUserInfo->set_signature(CurrentUserInfo.signature());
				pUserInfo->set_email(CurrentUserInfo.email());
				pUserInfo->set_account(CurrentUserInfo.account());

				//get additional information for a user
				list<IM::BaseDefine::UserDetails> lsAddiInfo;
				CUserModel::getInstance()->GetUserAdditionalInfo(strUserUuid, lsAddiInfo);
				for (auto it1 = lsAddiInfo.begin(); it1 != lsAddiInfo.end(); it1++)
				{
					IM::BaseDefine::UserDetails *pAddInfo = pUserInfo->add_add_info_list();
					pAddInfo->CopyFrom(*it1);
					//pAddInfo->set_details_uuid(it1->details_uuid());
					//pAddInfo->set_details_type(it1->details_type());
					//pAddInfo->set_details_title(it1->details_title());
					//pAddInfo->set_details_content(it1->details_content());
				}

				log("User Details: %d", pUserInfo->add_info_list_size());

				//get staff information for the user
				list<IM::BaseDefine::StaffInfo> lsStaff;
				CUserModel::getInstance()->GetStaffInfo(from_user_id, *it, lsStaff);
				for (auto it1 = lsStaff.begin(); it1 != lsStaff.end(); it1++)
				{
					IM::BaseDefine::StaffInfo* pStaffInfo = pUserInfo->add_staff_info_list();
					string strStaffUuid = it1->staff_uuid();
					pStaffInfo->set_staff_uuid(strStaffUuid);
					pStaffInfo->set_job_number(it1->job_number());
					pStaffInfo->set_staff_name(it1->staff_name());
					pStaffInfo->set_mobile_phone(it1->mobile_phone());
					pStaffInfo->set_company_id(it1->company_id());
					pStaffInfo->set_keyword(it1->keyword());
					pStaffInfo->set_user_id(it1->user_id());
					pStaffInfo->set_user_uuid(it1->user_uuid());
					pStaffInfo->set_flags(it1->flags());
					pStaffInfo->set_buy_product(0);
					pStaffInfo->set_account(it1->account());
					pStaffInfo->set_user_name(it1->user_name());
					pStaffInfo->set_email(it1->email());
					pStaffInfo->set_default_email(it1->default_email());

					//get staff details;

					list<IM::BaseDefine::UserDetails> lsStaffDetails;
					CUserModel::getInstance()->GetUserAdditionalInfo(strStaffUuid, lsStaffDetails, 2);
					for (auto it2 = lsStaffDetails.begin(); it2 != lsStaffDetails.end(); it2++)
					{
						IM::BaseDefine::UserDetails *pStaffDetails = pStaffInfo->add_add_info_list();
						pStaffDetails->CopyFrom(*it2);
					/*	pStaffDetails->set_details_uuid(it2->details_uuid());
						pStaffDetails->set_details_type(it2->details_type());
						pStaffDetails->set_details_title(it2->details_title());
						pStaffDetails->set_details_content(it2->details_content());*/
					}


					//get staff duty
					list<IM::BaseDefine::DutyInfo> lsDutyInfo;
					map<string, list<string>> mapPosition;
					CUserModel::getInstance()->GetDuty(strStaffUuid, lsDutyInfo, mapPosition,it1->company_id());
					for (auto it = lsDutyInfo.begin(); it != lsDutyInfo.end(); it++) //get departments for a user
					{
						IM::BaseDefine::DutyInfo *pDutyInfo = pStaffInfo->add_duty_list();
						pDutyInfo->set_depart_uuid(it->depart_uuid());
						pDutyInfo->set_job_grade(it->job_grade());

						auto it1 = mapPosition.find(it->depart_uuid());
						if (it1 != mapPosition.end())
						{
							for (auto it2 = it1->second.begin(); it2 != it1->second.end(); it2++) //get position for a department
							{
								pDutyInfo->add_position_uuid_list(*it2);
							}
							log("Position List Size: %d for Depart: %s", pDutyInfo->position_uuid_list_size(), it->depart_uuid().c_str());
						}
					}
					log("Duty List Size: %d", pStaffInfo->duty_list_size());

				}
				log("Staff list size: %d ", pUserInfo->staff_info_list_size());
			}

			msgResp.set_attach_data(msg.attach_data());
			pPduRes->SetPBMsg(&msgResp);
			pPduRes->SetSeqNum(pPdu->GetSeqNum());
			pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
			pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_LIST_STAFF_INFO_RESPONSE);
			CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
			log("userId=%u, userCnt=%u", from_user_id, userCount);
		}
		else
		{
			log("parse pb failed");
		}
	}


	void GetChangedStaffLite(CImPdu* pPdu, uint32_t conn_uuid)
	{
		IM::Buddy::IMAllStaffLiteReq msg;
		IM::Buddy::IMAllStaffLiteRsp msgResp;
		if (msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
		{
			uint32_t unReqId = msg.user_id();
			uint32_t unLastTime = msg.latest_update_time();
			uint32_t unCompanyId = msg.company_id();

			list<IM::BaseDefine::StaffInfoLite> lsStaffInfo;
			uint32_t unTotalUsers = 0;
			CUserModel *pInstance = CUserModel::getInstance();
			pInstance->GetCompanyStaff(unReqId, unCompanyId, unLastTime, lsStaffInfo);

			uint32_t unTotalCount = 0;
			uint32_t unCount = lsStaffInfo.size();
			msgResp.set_user_id(unReqId);
			msgResp.set_company_id(unCompanyId);
			msgResp.set_latest_update_time(0);
			//msgResp.set_staff_count(unTotalUsers);
			msgResp.set_attach_data(msg.attach_data());


			for (auto it = lsStaffInfo.begin(); it != lsStaffInfo.end(); ++it)
			{

				IM::BaseDefine::StaffInfoLite* pStaffInfo = msgResp.add_staff_info_list();

				string strStaffUuid = it->staff_uuid();

				pStaffInfo->set_user_id(it->user_id());
				pStaffInfo->set_user_uuid(it->user_uuid());
				pStaffInfo->set_avatar_url(it->avatar_url());
				pStaffInfo->set_flags(it->flags());
				pStaffInfo->set_staff_uuid(strStaffUuid);
				pStaffInfo->set_user_real_name(it->user_real_name());
				pStaffInfo->set_staff_real_name(it->staff_real_name());
				pStaffInfo->set_mobile_phone(it->mobile_phone());
				
				//get staff duty
				list<IM::BaseDefine::DutyInfo> lsDutyInfo;
				map<string, list<string>> mapPosition;
				pInstance->GetDuty(strStaffUuid, lsDutyInfo, mapPosition, unCompanyId);
				for (auto it = lsDutyInfo.begin(); it != lsDutyInfo.end(); it++) //get departments for a user
				{
					IM::BaseDefine::DutyInfo *pDutyInfo = pStaffInfo->add_duty_list();
					pDutyInfo->set_depart_uuid(it->depart_uuid());
					pDutyInfo->set_job_grade(it->job_grade());

					auto it1 = mapPosition.find(it->depart_uuid());
					if (it1 != mapPosition.end())
					{
						for (auto it2 = it1->second.begin(); it2 != it1->second.end(); it2++) //get position for a department
						{
							pDutyInfo->add_position_uuid_list(*it2);
						}
						log("Position List Size: %d for Depart: %s", pDutyInfo->position_uuid_list_size(), it->depart_uuid().c_str());
					}
				}
				log("Duty List Size: %d", pStaffInfo->duty_list_size());


				if ((++unTotalCount) % MAX_USER_COUNT_EACH_PACKET_LITE == 0) //added by george on April 11th,2016
				{
					//unTotalCount += unMaxCount;
					if (unTotalCount == unCount)
					{
						msgResp.set_latest_update_time(unLastTime); //set actual latest time to indicate completely send all users
					}

					CImPdu* pPduRes = new CImPdu;
					//msgResp.set_attach_data(msg.attach_data());
					pPduRes->SetPBMsg(&msgResp);
					pPduRes->SetSeqNum(pPdu->GetSeqNum());//change sequence?(TBD)
					pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
					pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_LIST_ALL_STAFF_LITE_RESPONSE);
					CProxyConn::AddResponsePdu(conn_uuid, pPduRes);

					log("userId=%u, userCnt=%u, totalcount: %d", unReqId, unCount, unTotalCount);

					//reset data
					msgResp.clear_staff_info_list();
				}
			}

			if (unTotalCount % MAX_USER_COUNT_EACH_PACKET_LITE != 0 || unCount == 0)
			{
				msgResp.set_latest_update_time(unLastTime); //set actual latest time to indicate completely send all users
				//msgResp.set_attach_data(msg.attach_data());
				CImPdu* pPduRes = new CImPdu;
				pPduRes->SetPBMsg(&msgResp);
				pPduRes->SetSeqNum(pPdu->GetSeqNum());
				pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
				pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_LIST_ALL_STAFF_LITE_RESPONSE);
				CProxyConn::AddResponsePdu(conn_uuid, pPduRes);

				//unTotalCount += unMaxCount;
				log("userId=%u, last_time=%u, userCnt=%u, totalcount: %d", unReqId, unLastTime, unCount, unTotalCount);
			}
		}
		else
		{
			log("parse pb failed");
		}

	}


	void UpdateStaff(CImPdu* pPdu, uint32_t conn_uuid)
	{
		IM::Buddy::IMUpdateStaffReq msg;
		IM::Buddy::IMUpdateStaffRsp msgResp;
		if (msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
		{
			CImPdu* pPduRes = new CImPdu;
			uint32_t unReqId = msg.user_id();
			string strStaffUuid = msg.staff_uuid();
			uint32_t unNow = (uint32_t)time(NULL);
			bool bRet = false;

			list<IM::BaseDefine::UserDetails> lsDetails;
			int nCount = msg.staff_details_list_size();
			for (int i = 0; i < nCount; i++)
			{
				IM::BaseDefine::UserDetails UserDetails = msg.staff_details_list(i);
				lsDetails.push_back(UserDetails);
			}
			log("Staff Details Size: %d", nCount);

			bRet = CUserModel::getInstance()->UpdateStaff(strStaffUuid, lsDetails, unNow);

			msgResp.set_user_id(unReqId);
			msgResp.set_staff_uuid(strStaffUuid);
		
			for (auto it = lsDetails.begin(); it != lsDetails.end(); it++)
			{
				IM::BaseDefine::UserDetails UserDetails = *it;
				IM::BaseDefine::UserDetails *pUserDetails = msgResp.add_staff_details_list();
				pUserDetails->CopyFrom(UserDetails);

				/*pUserDetails->set_details_uuid(it->details_uuid());
				pUserDetails->set_details_type(it->details_type());
				pUserDetails->set_details_title(it->details_title());
				pUserDetails->set_details_content(it->details_content());*/
			}

			msgResp.set_latest_update_time(bRet ? unNow : 0); //0 meas failed to update
			msgResp.set_attach_data(msg.attach_data());

			pPduRes->SetPBMsg(&msgResp);
			pPduRes->SetSeqNum(pPdu->GetSeqNum());
			pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
			pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_LIST_STAFF_UPDATE_INFO_RESPONSE);
			CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
			return;
		}
		else
		{
			log("parse pb failed");
		}
	}

	void GetAssistantInfo(CImPdu* pPdu, uint32_t conn_uuid)
	{
		IM::Buddy::IMAssistantInfoReq msg;
		IM::Buddy::IMAssistantInfoRsp msgResp;
		if (msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
		{
			uint32_t unReqId = msg.user_id();
			uint32_t unLastTime = msg.latest_update_time();

			list<IM::BaseDefine::UserInfo> lsUsers;
			CUserModel::getInstance()->GetAssistantInfo(unLastTime, lsUsers);

			msgResp.set_user_id(unReqId);
			msgResp.set_latest_update_time(unLastTime);
			msgResp.set_attach_data(msg.attach_data());

			for (auto it = lsUsers.begin(); it != lsUsers.end(); ++it)
			{

				IM::BaseDefine::UserInfo* pUser = msgResp.add_asst_info_list();

				pUser->set_user_id(it->user_id());
				pUser->set_user_uuid(it->user_uuid());
				pUser->set_user_name(it->user_name());
				pUser->set_user_real_name(it->user_real_name());
				pUser->set_avatar_url(it->avatar_url());
				pUser->set_user_gender(it->user_gender());
				pUser->set_birthday(it->birthday());
				pUser->set_mobile_phone(it->mobile_phone());
				pUser->set_status(it->status());
				pUser->set_keyword(it->keyword());
				pUser->set_user_nick_name(it->user_nick_name());
				pUser->set_signature(it->signature());
				pUser->set_email(it->email());
				pUser->set_account(it->account());
			}

			CImPdu* pPduRes = new CImPdu;
			pPduRes->SetPBMsg(&msgResp);
			pPduRes->SetSeqNum(pPdu->GetSeqNum());
			pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
			pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_LIST_ASST_INFO_RESPONSE);
			CProxyConn::AddResponsePdu(conn_uuid, pPduRes);

			log("userId=%u, last_time=%u, assistants: %d", unReqId, unLastTime, lsUsers.size());
		}
		else
		{
			log("parse pb failed");
		}
	}


	void QueryUserInfo(CImPdu* pPdu, uint32_t conn_uuid)
	{
		IM::Buddy::IMQueryUserInfoReq msg;
		IM::Buddy::IMQueryUserInfoRsp msgResp;

		if (msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
		{
			CImPdu* pPduRes = new CImPdu;

			uint32_t from_user_id = msg.user_id();
			string strUserUuid = msg.user_uuid();

			msgResp.set_user_id(from_user_id);
		
			IM::BaseDefine::UserInfo *pUserInfo = msgResp.mutable_user_info();
			IM::BaseDefine::UserInfo  CurrentUserInfo;
			CUserModel::getInstance()->GetUserInfo(strUserUuid, CurrentUserInfo);

			//get user basic information
			//string strUserUuid = CurrentUserInfo.user_uuid();
			pUserInfo->set_user_id(CurrentUserInfo.user_id());
			pUserInfo->set_user_uuid(CurrentUserInfo.user_uuid());
			pUserInfo->set_user_name(CurrentUserInfo.user_name());
			pUserInfo->set_user_real_name(CurrentUserInfo.user_real_name());
			pUserInfo->set_avatar_url(CurrentUserInfo.avatar_url());
			pUserInfo->set_user_gender(CurrentUserInfo.user_gender());
			pUserInfo->set_birthday(CurrentUserInfo.birthday());
			pUserInfo->set_mobile_phone(CurrentUserInfo.mobile_phone());
			pUserInfo->set_status(CurrentUserInfo.status());
			pUserInfo->set_keyword(CurrentUserInfo.keyword());
			pUserInfo->set_user_nick_name(CurrentUserInfo.user_nick_name());
			pUserInfo->set_signature(CurrentUserInfo.signature());
			pUserInfo->set_email(CurrentUserInfo.email());
			pUserInfo->set_account(CurrentUserInfo.account());

			//get additional information for a user
			list<IM::BaseDefine::UserDetails> lsAddiInfo;
			CUserModel::getInstance()->GetUserAdditionalInfo(strUserUuid, lsAddiInfo);
			for (auto it1 = lsAddiInfo.begin(); it1 != lsAddiInfo.end(); it1++)
			{
				IM::BaseDefine::UserDetails *pAddInfo = pUserInfo->add_add_info_list();
				pAddInfo->CopyFrom(*it1);
				//pAddInfo->set_details_uuid(it1->details_uuid());
				//pAddInfo->set_details_type(it1->details_type());
				//pAddInfo->set_details_title(it1->details_title());
				//pAddInfo->set_details_content(it1->details_content());
			}

			log("User Details: %d", pUserInfo->add_info_list_size());

			msgResp.set_attach_data(msg.attach_data());
			pPduRes->SetPBMsg(&msgResp);
			pPduRes->SetSeqNum(pPdu->GetSeqNum());
			pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
			pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_LIST_QUERY_USER_INFO_RESPONSE);
			CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
			//log("userId=%u, userCnt=%u", from_user_id, userCount);
		}
		else
		{
			log("parse pb failed");
		}

	}
};


