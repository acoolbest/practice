/*
 * DBServConn.cpp
 *
 *  Created on: 2013-7-8
 *      Author: ziteng@mogujie.com
 */

#include "EncDec.h"
#include "DBServConn.h"
#include "MsgConn.h"
#include "RouteServConn.h"
#include "GroupChat.h"
#include "FileHandler.h"
#include "PushServConn.h"
#include "ImUser.h"
#include "security.h"
#include "AttachData.h"
#include "jsonxx.h"
#include "IM.Other.pb.h"
#include "IM.Buddy.pb.h"
#include "IM.Login.pb.h"
#include "IM.Group.pb.h"
#include "IM.Message.pb.h"
#include "IM.Server.pb.h"
#include "ImPduBase.h"
#include "public_define.h"
#include "ConfigFileReader.h"
#include "util.h"


using namespace IM::BaseDefine;

static ConnMap_t g_db_server_conn_map;

static serv_info_t* g_db_server_list = NULL;
static uint32_t		g_db_server_count = 0;			// 到DBServer的总连接数
static uint32_t		g_db_server_login_count = 0;	// 到进行登录处理的DBServer的总连接数
static CGroupChat*	s_group_chat = NULL;
//static CFileHandler* s_file_handler = NULL;


extern CAes *pAes;

static void db_server_conn_timer_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	ConnMap_t::iterator it_old;
	CDBServConn* pConn = NULL;
	uint64_t cur_time = get_tick_count();

	for (ConnMap_t::iterator it = g_db_server_conn_map.begin(); it != g_db_server_conn_map.end(); ) {
		it_old = it;
		it++;

		pConn = (CDBServConn*)it_old->second;
		if (pConn->IsOpen()) {
			pConn->OnTimer(cur_time);
		}
	}

	// reconnect DB Storage Server
	// will reconnect in 4s, 8s, 16s, 32s, 64s, 4s 8s ...
	serv_check_reconnect<CDBServConn>(g_db_server_list, g_db_server_count);
}

void init_db_serv_conn(serv_info_t* server_list, uint32_t server_count, uint32_t concur_conn_cnt)
{
	g_db_server_list = server_list;
	g_db_server_count = server_count;

	uint32_t total_db_instance = server_count / concur_conn_cnt;
	g_db_server_login_count = (total_db_instance / 2) * concur_conn_cnt;
	log("DB server connection index for login business: [0, %u), for other business: [%u, %u) ",
			g_db_server_login_count, g_db_server_login_count, g_db_server_count);

	serv_init<CDBServConn>(g_db_server_list, g_db_server_count);

	netlib_register_timer(db_server_conn_timer_callback, NULL, 1000);
	s_group_chat = CGroupChat::GetInstance();
	//s_file_handler = CFileHandler::getInstance();
}

// get a random db server connection in the range [start_pos, stop_pos)
static CDBServConn* get_db_server_conn_in_range(uint32_t start_pos, uint32_t stop_pos)
{
	uint32_t i = 0;
	CDBServConn* pDbConn = NULL;

	// determine if there is a valid DB server connection
	for (i = start_pos; i < stop_pos; i++) {
		pDbConn = (CDBServConn*)g_db_server_list[i].serv_conn;
		if (pDbConn && pDbConn->IsOpen()) {
			break;
		}
	}

	// no valid DB server connection
	if (i == stop_pos) {
		return NULL;
	}

	// return a random valid DB server connection
	while (true) {
		int i = rand() % (stop_pos - start_pos) + start_pos;
		pDbConn = (CDBServConn*)g_db_server_list[i].serv_conn;
		if (pDbConn && pDbConn->IsOpen()) {
			break;
		}
	}

	return pDbConn;
}

CDBServConn* get_db_serv_conn_for_login()
{
	// 先获取login业务的实例，没有就去获取其他业务流程的实例
	CDBServConn* pDBConn = get_db_server_conn_in_range(0, g_db_server_login_count);
	if (!pDBConn) {
		pDBConn = get_db_server_conn_in_range(g_db_server_login_count, g_db_server_count);
	}

	return pDBConn;
}

CDBServConn* get_db_serv_conn()
{
	// 先获取其他业务流程的实例，没有就去获取login业务的实例
	CDBServConn* pDBConn = get_db_server_conn_in_range(g_db_server_login_count, g_db_server_count);
	if (!pDBConn) {
		pDBConn = get_db_server_conn_in_range(0, g_db_server_login_count);
	}

	return pDBConn;
}


CDBServConn::CDBServConn()
{
	m_bOpen = false;
}

CDBServConn::~CDBServConn()
{

}

void CDBServConn::Connect(const char* server_ip, uint16_t server_port, uint32_t serv_idx)
{
	log("Connecting to DB Storage Server %s:%d ", server_ip, server_port);

	m_serv_idx = serv_idx;
	m_handle = netlib_connect(server_ip, server_port, imconn_callback, (void*)&g_db_server_conn_map);

	if (m_handle != NETLIB_INVALID_HANDLE) {
		g_db_server_conn_map.insert(make_pair(m_handle, this));
	}
}

void CDBServConn::Close()
{
	// reset server information for the next connect
	serv_reset<CDBServConn>(g_db_server_list, g_db_server_count, m_serv_idx);

	if (m_handle != NETLIB_INVALID_HANDLE) {
		netlib_close(m_handle);
		g_db_server_conn_map.erase(m_handle);
	}

	ReleaseRef();
}

void CDBServConn::OnConfirm()
{
	log("connect to db server success");
	m_bOpen = true;
	g_db_server_list[m_serv_idx].reconnect_cnt = MIN_RECONNECT_CNT / 2;
}

void CDBServConn::OnClose()
{
	log("onclose from db server handle=%d", m_handle);
	Close();
}

void CDBServConn::OnTimer(uint64_t curr_tick)
{
	if (curr_tick > m_last_send_tick + SERVER_HEARTBEAT_INTERVAL) {
        IM::Other::IMHeartBeat msg;
        CImPdu pdu;
        pdu.SetPBMsg(&msg);
        pdu.SetServiceId(SID_OTHER);
        pdu.SetCommandId(CID_OTHER_HEARTBEAT);
		SendPdu(&pdu);
	}

	if (curr_tick > m_last_recv_tick + SERVER_TIMEOUT) {
		log("conn to db server timeout");
		Close();
	}
}

void CDBServConn::HandlePdu(CImPdu* pPdu)
{
	switch (pPdu->GetCommandId()) {
        case CID_OTHER_HEARTBEAT:
            break;
        case CID_OTHER_VALIDATE_RSP:
            _HandleValidateResponse(pPdu );
            break;
        case CID_LOGIN_RES_DEVICETOKEN:
            _HandleSetDeviceTokenResponse(pPdu);
            break;
        case CID_MSG_UNREAD_CNT_RESPONSE:
            _HandleUnreadMsgCountResponse( pPdu );
            break;
        case CID_MSG_LIST_RESPONSE:
            _HandleGetMsgListResponse(pPdu);
            break;
        case CID_MSG_GET_BY_MSG_ID_RES:
            _HandleGetMsgByIdResponse(pPdu);
            break;
			//added by george on Dec 30th,2015
		case CID_MSG_RESEND_MSG_RES:
			_HandleResendMessageResponse(pPdu);
			break;
        case CID_MSG_DATA:
            _HandleMsgData(pPdu);
            break;
        case CID_MSG_GET_LATEST_MSG_ID_RSP:
            _HandleGetLatestMsgIDRsp(pPdu);
            break;

        case CID_BUDDY_LIST_RECENT_CONTACT_SESSION_RESPONSE:
            _HandleRecentSessionResponse(pPdu);
            break;
        case CID_BUDDY_LIST_ALL_USER_RESPONSE:
            _HandleAllUserResponse(pPdu);
            break;
		case CID_BUDDY_LIST_USER_UPDATE_NAGURE_RESPONSE:
			_HandleUpdateUserNagureResponse(pPdu);
			break;
		case CID_BUDDY_LIST_USER_UPDATE_INFO_RESPONSE:
			_HandleUpdateUserResponse(pPdu);
			break;
        case CID_BUDDY_LIST_USER_INFO_RESPONSE:
            _HandleUsersInfoResponse(pPdu );
            break;
        case CID_BUDDY_LIST_REMOVE_SESSION_RES:
            _HandleRemoveSessionResponse(pPdu );
            break;
        case CID_BUDDY_LIST_CHANGE_AVATAR_RESPONSE:
            _HandleChangeAvatarResponse(pPdu);
            break;
        case CID_BUDDY_LIST_DEPARTMENT_RESPONSE:
            _HandleDepartmentResponse(pPdu);
            break;
		case CID_BUDDY_LIST_COMPANY_RESPONSE:
			_HandleCompanyResponse(pPdu);
			break;
		case CID_BUDDY_SHIELD_USER_RESPONSE:
			_HandleUserShieldUserResponse(pPdu);
			break;

		case CID_BUDDY_LIST_POSITION_RESPONSE:
			_HandlePositionResponse(pPdu);
			break;
	
		case CID_BUDDY_LIST_ALL_USER_LITE_RESPONSE:
			_HandleAllUserLiteResponse(pPdu);
			break;

		case CID_BUDDY_LIST_USER_INFO_LITE_RESPONSE:
			_HandleUserInfoLiteResponse(pPdu);
			break;

		case CID_OTHER_GET_SHIELD_USER_RSP:
			_HandleUserGetShieldByUserResponse(pPdu);
			break;
        case CID_OTHER_GET_DEVICE_TOKEN_RSP:
            _HandleGetDeviceTokenResponse(pPdu);
            break;
        case CID_OTHER_GET_SHIELD_RSP:
            s_group_chat->HandleGroupGetShieldByGroupResponse(pPdu);
            break;
        case CID_OTHER_STOP_RECV_PACKET:
            _HandleStopReceivePacket(pPdu);
            break;
        //group
        case CID_GROUP_NORMAL_LIST_RESPONSE:
            s_group_chat->HandleGroupNormalResponse( pPdu );
            break;
        case CID_GROUP_INFO_RESPONSE:
            s_group_chat->HandleGroupInfoResponse(pPdu);
            break;
        case CID_GROUP_CREATE_RESPONSE:
            s_group_chat->HandleGroupCreateResponse(pPdu);
            break;
        case CID_GROUP_CHANGE_MEMBER_RESPONSE:
            s_group_chat->HandleGroupChangeMemberResponse(pPdu);
            break;
        case CID_GROUP_SHIELD_GROUP_RESPONSE:
            s_group_chat->HandleGroupShieldGroupResponse(pPdu);
            break;

		case CID_GROUP_CHANGE_GRPNAME_RESPONSE:
			s_group_chat->HandleGroupChangeNameResponse(pPdu);
			break;

		case CID_GROUP_TRANSFER_LEADER_RESPONSE:
			s_group_chat->HandleGroupTransferLeaderResponse(pPdu);
			break;
        
      /*  case CID_FILE_HAS_OFFLINE_RES:
            s_file_handler->HandleFileHasOfflineRes(pPdu);
            break;

		case CID_FILE_ADD_OFFLINE_RES:
			s_file_handler->HandleClientFileAddOfflineRes(pPdu);
			break;*/
		case CID_BUDDY_LIST_ALL_STAFF_RESPONSE:
			_HandleAllStaffResponse(pPdu);
			break;

		case CID_BUDDY_LIST_ALL_STAFF_LITE_RESPONSE:
			_HandleAllStaffLiteResponse(pPdu);
			break;

		case CID_BUDDY_LIST_STAFF_INFO_RESPONSE:
			_HandleStaffInfoResponse(pPdu);
			break;

		case CID_BUDDY_LIST_STAFF_UPDATE_INFO_RESPONSE:
			_HandleUpdateStaffResponse(pPdu);
			break;

		case CID_BUDDY_LIST_CHANGE_VIP_RESPONSE:
			_HandleClientChangeVipResponse(pPdu);
			break;

		case CID_BUDDY_LIST_ASST_INFO_RESPONSE:
			_HandleClientAsstistantInfoResponse(pPdu);
			break;

		case CID_BUDDY_LIST_QUERY_USER_INFO_RESPONSE:
			_HandleClientQueryUserInfoResponse(pPdu);
			break;

        default:
            log("db server, wrong cmd id=%d ", pPdu->GetCommandId());
	}
}

void CDBServConn::_HandleValidateResponse(CImPdu* pPdu)
{
    IM::Server::IMValidateRsp msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
    string login_name = msg.user_name();
    uint32_t result = msg.result_code();
    string result_string = msg.result_string();
    CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
    log("HandleValidateResp, user_name=%s, result=%d", login_name.c_str(), result);
    
    CImUser* pImUser = CImUserManager::GetInstance()->GetImUserByLoginName(login_name);
    CMsgConn* pMsgConn = NULL;
    if (!pImUser) {
        log("ImUser for user_name=%s not exist", login_name.c_str());
        return;
    } else {
        pMsgConn = pImUser->GetUnValidateMsgConn(attach_data.GetHandle());
        if (!pMsgConn || pMsgConn->IsOpen()) {
            log("no such conn is validated, user_name=%s", login_name.c_str());
            return;
        }
    }
    
	//george commented the following brackets due to new invalid type
   /* if (result != 0)
	{
        result = IM::BaseDefine::REFUSE_REASON_DB_VALIDATE_FAILED;
    }*/
    
    if (result == 0)
    {
        IM::BaseDefine::UserInfo user_info = msg.user_info();
        uint32_t user_id = user_info.user_id();
        CImUser* pUser = CImUserManager::GetInstance()->GetImUserById(user_id);
        if (pUser)
        {
            pUser->AddUnValidateMsgConn(pMsgConn);
            pImUser->DelUnValidateMsgConn(pMsgConn);
            if (pImUser->IsMsgConnEmpty())
            {
                CImUserManager::GetInstance()->RemoveImUserByLoginName(login_name);
                delete pImUser;
            }
        }
        else
        {
            pUser = pImUser;
        }
        
        pUser->SetUserId(user_id);
        pUser->SetNickName(user_info.user_nick_name());
        pUser->SetValidated();
        CImUserManager::GetInstance()->AddImUserById(user_id, pUser);
        
		if (pUser->KickOutSameClientType(pMsgConn->GetClientType(), IM::BaseDefine::KICK_REASON_DUPLICATE_USER, pMsgConn))
		{
			if (CHECK_CLIENT_TYPE_MOBILE(pMsgConn->GetClientType()))
			{
				CDBServConn* pDBConn = get_db_serv_conn();
				if (pDBConn)
				{
					log("reset device token for user %d", user_id);
					IM::Login::IMDeviceTokenReq msg;
					msg.set_user_id(user_id);
					msg.set_device_token("");
					msg.set_client_type((::IM::BaseDefine::ClientType)pMsgConn->GetClientType());
					CImPdu pdu;
					pdu.SetPBMsg(&msg);
					pdu.SetServiceId(SID_LOGIN);
					pdu.SetCommandId(CID_LOGIN_REQ_DEVICETOKEN);
					pdu.SetSeqNum(pPdu->GetSeqNum());
					pDBConn->SendPdu(&pdu);
				}
			}
		}
        
        CRouteServConn* pRouteConn = get_route_serv_conn();
        if (pRouteConn)
		{
            IM::Server::IMServerKickUser msg2;
            msg2.set_user_id(user_id);
            msg2.set_client_type((::IM::BaseDefine::ClientType)pMsgConn->GetClientType());
            msg2.set_reason(1);
            CImPdu pdu;
            pdu.SetPBMsg(&msg2);
            pdu.SetServiceId(SID_OTHER);
            pdu.SetCommandId(CID_OTHER_SERVER_KICK_USER);
            pRouteConn->SendPdu(&pdu);
        }
        
        log("user_name: %s, uid: %d", login_name.c_str(), user_id);
        pMsgConn->SetUserId(user_id);
        pMsgConn->SetOpen();
        pMsgConn->SendUserStatusUpdate(IM::BaseDefine::USER_STATUS_ONLINE);
        pUser->ValidateMsgConn(pMsgConn->GetHandle(), pMsgConn);
        
        IM::Login::IMLoginRes msg3;
        msg3.set_server_time(time(NULL));
        msg3.set_result_code(IM::BaseDefine::REFUSE_REASON_NONE);
        msg3.set_result_string(result_string);
        msg3.set_online_status((IM::BaseDefine::UserStatType)pMsgConn->GetOnlineStatus());


        IM::BaseDefine::UserInfo* pUserInfo = msg3.mutable_user_info();
		pUserInfo->set_user_id(user_info.user_id());
		pUserInfo->set_user_uuid(user_info.user_uuid());
		pUserInfo->set_user_name(user_info.user_name());
		pUserInfo->set_user_real_name(user_info.user_real_name());
		pUserInfo->set_avatar_url(user_info.avatar_url());
		pUserInfo->set_user_gender(user_info.user_gender());
		pUserInfo->set_birthday(user_info.birthday());
		pUserInfo->set_mobile_phone(user_info.mobile_phone());
		pUserInfo->set_status(user_info.status());
		pUserInfo->set_keyword(user_info.keyword());
		pUserInfo->set_user_nick_name(user_info.user_nick_name());
		pUserInfo->set_signature(user_info.signature());
		pUserInfo->set_email(user_info.email());
		pUserInfo->set_account(user_info.account());

		int nTotals = user_info.black_list_size();
		for (int i = 0; i < nTotals; i++)
		{
			IM::BaseDefine::ShieldUser ShieldUser = user_info.black_list(i);
			IM::BaseDefine::ShieldUser* pShieldUser = pUserInfo->add_black_list();
			pShieldUser->set_user_id(ShieldUser.user_id());
			pShieldUser->set_shield_status(ShieldUser.shield_status());
		}
		log("Black List Size: %d", pUserInfo->black_list_size());
	

		//get company list
		nTotals = user_info.company_info_list_size();
		for (int i = 0; i < nTotals; i++)
		{
			IM::BaseDefine::CompanyInfo CompanyInfo = user_info.company_info_list(i);
			IM::BaseDefine::CompanyInfo *pCompanyInfo = pUserInfo->add_company_info_list();
			
			pCompanyInfo->set_company_id(CompanyInfo.company_id());
			pCompanyInfo->set_company_uuid(CompanyInfo.company_uuid());
			pCompanyInfo->set_company_no(CompanyInfo.company_no());
			pCompanyInfo->set_company_name(CompanyInfo.company_name());
			pCompanyInfo->set_company_full_name(CompanyInfo.company_full_name());
			pCompanyInfo->set_industry_type(CompanyInfo.industry_type());
			pCompanyInfo->set_company_is_joined(CompanyInfo.company_is_joined());
			pCompanyInfo->set_company_is_invite(CompanyInfo.company_is_invite());
		}
		log("Joined company count: %d", pUserInfo->company_info_list_size());


		//get vip list
		nTotals = user_info.vip_id_list_size();
		for (int i = 0; i < nTotals; i++)
		{
			uint32_t unVipId = user_info.vip_id_list(i);
			pUserInfo->add_vip_id_list(unVipId);
		}
		log("VIP count: %d", pUserInfo->vip_id_list_size());


		//get user details
		nTotals = user_info.add_info_list_size();
		for (int i = 0; i < nTotals; i++)
		{
			IM::BaseDefine::UserDetails AddInfo = user_info.add_info_list(i);
			IM::BaseDefine::UserDetails *pAddInfo = pUserInfo->add_add_info_list();
			pAddInfo->set_details_uuid(AddInfo.details_uuid());
			pAddInfo->set_details_type(AddInfo.details_type());
			pAddInfo->set_details_title(AddInfo.details_title());
			pAddInfo->set_details_content(AddInfo.details_content());
		}
		log("User Additional Informations: %d", pUserInfo->add_info_list_size());

		//get staff information for each company
		nTotals = user_info.staff_info_list_size();
		for (int i = 0; i < nTotals; i++)
		{
			IM::BaseDefine::StaffInfo StaffInfo = user_info.staff_info_list(i);
			IM::BaseDefine::StaffInfo* pStaffInfo = pUserInfo->add_staff_info_list();
			pStaffInfo->set_staff_uuid(StaffInfo.staff_uuid());
			pStaffInfo->set_job_number(StaffInfo.job_number());
			pStaffInfo->set_staff_name(StaffInfo.staff_name());
			pStaffInfo->set_mobile_phone(StaffInfo.mobile_phone());
			pStaffInfo->set_company_id(StaffInfo.company_id());
			pStaffInfo->set_keyword(StaffInfo.keyword());
			pStaffInfo->set_user_id(StaffInfo.user_id());
			pStaffInfo->set_user_uuid(StaffInfo.user_uuid());
			pStaffInfo->set_flags(StaffInfo.flags());
			pStaffInfo->set_buy_product(StaffInfo.buy_product());
			pStaffInfo->set_account(StaffInfo.account());
			pStaffInfo->set_user_name(StaffInfo.user_name());
			pStaffInfo->set_email(StaffInfo.email());
			pStaffInfo->set_default_email(StaffInfo.default_email());

			//get staff details;
			int nCount = StaffInfo.add_info_list_size();
			for (int i = 0; i < nCount; i++)
			{
				IM::BaseDefine::UserDetails *pStaffDetails = pStaffInfo->add_add_info_list();
				IM::BaseDefine::UserDetails StaffDetails = StaffInfo.add_info_list(i);
				pStaffDetails->set_details_uuid(StaffDetails.details_uuid());
				pStaffDetails->set_details_type(StaffDetails.details_type());
				pStaffDetails->set_details_title(StaffDetails.details_title());
				pStaffDetails->set_details_content(StaffDetails.details_content());
			}
			log("Staff additional information: %d", pStaffInfo->add_info_list_size());

			//get staff duty
			nCount = StaffInfo.duty_list_size();
			for (int i = 0; i < nCount; i++) //get departments for a user
			{
				IM::BaseDefine::DutyInfo *pDutyInfo = pStaffInfo->add_duty_list();
				IM::BaseDefine::DutyInfo DutyInfo = StaffInfo.duty_list(i);

				pDutyInfo->set_depart_uuid(DutyInfo.depart_uuid());
				pDutyInfo->set_job_grade(DutyInfo.job_grade());

				int nSize = DutyInfo.position_uuid_list_size();
				for (int i = 0; i < nSize; i++)
				{
					string strPositionUuid = DutyInfo.position_uuid_list(i);
					pDutyInfo->add_position_uuid_list(strPositionUuid);
				}
				log("Position List Size: %d for Depart: %s", pDutyInfo->position_uuid_list_size(), pDutyInfo->depart_uuid().c_str());
			}
			log("Duty List Size: %d", pStaffInfo->duty_list_size());
		}



//		//user_info_tmp->set_job_number(user_info.job_number());
//		//user_info_tmp->set_mobile_phone(user_info.mobile_phone());
//		//user_info_tmp->set_birthday(user_info.birthday());
//		//user_info_tmp->set_user_nick_name(user_info.user_nick_name());
//		//user_info_tmp->set_signature(user_info.signature());
//
//
//		//IM::BaseDefine::UserInfoBase* pBaseInfo = user_info_tmp->mutable_base_info();
//
//		//pBaseInfo->set_user_id(user_info.user_id());
//		//pBaseInfo->set_user_uuid(user_info.user_uuid());
//		//pBaseInfo->set_user_name(user_info.user_name());
//		//pBaseInfo->set_user_real_name(user_info.user_real_name());
//		//pBaseInfo->set_avatar_url(user_info.avatar_url());
//		//pBaseInfo->set_keyword(user_info.keyword());
//
//		//uint32_t unFlag = 0; //lower 2 bits for gender; lower 3 means contact status;lower 4 and 5 means user status
//		////if (user_info.user_gender() == 1) //male
//		////{
//		////	unFlag |= 0x01;
//		////}
//		////else if (user_info.user_gender() == 2) //unknown
//		////{
//		////	unFlag |= 0x02;
//		////}
//
//		////if (user_info.contact_status() == 1)
//		////{
//		////	unFlag |= 0x04;
//		////}
//
//		////if (user_info.status() == 1)  //normal
//		////{
//		////	unFlag |= 0x08;
//		////}
//		////else if (user_info.status() == -1) //deleted
//		////{
//		////	unFlag |= 0x10;
//		////}
//
//		//unFlag = GetUserFlags(user_info.user_gender(), user_info.contact_status(), user_info.status());
//		//pBaseInfo->set_flags(unFlag);
//
//		//string strBuyProducts = user_info.buyproduct();
//		//uint32_t unBuyProducts = GetProduct(strBuyProducts);
//		//pBaseInfo->set_buy_product(unBuyProducts);
//
//		////log("AVTAR URL: %s", user_info.avatar_url().c_str());
//
//		//get departs and positions
//		int nTotals = user_info.duty_list_size();
//		for (int i = 0; i < nTotals; i++)
//		{
//			IM::BaseDefine::DutyInfoLite *pDuty = pBaseInfo->add_duty_list();
//			IM::BaseDefine::DutyInfo Duty = user_info.duty_list(i);
//
//			pDuty->set_depart_uuid(Duty.depart_uuid());
//			/*pDuty->set_depart_name(Duty.depart_name());
//			pDuty->set_status(Duty.status());
//*/
//			int nCount = Duty.position_list_size();
//			for (int j = 0; j < nCount; j++) //get positions for a depart
//			{
//				IM::BaseDefine::PositionInfo CurrPosition = Duty.position_list(j);
//				//IM::BaseDefine::PositionInfo *pPosition = pDuty->add_position_list();
//				pDuty->add_position_uuid_list(CurrPosition.position_uuid());
//
//			/*	pPosition->set_position_uuid(CurrPosition.position_uuid());
//				pPosition->set_position_type(CurrPosition.position_type());
//				pPosition->set_position_name(CurrPosition.position_name());
//				pPosition->set_status(CurrPosition.status());
//				pPosition->set_sort(CurrPosition.sort());*/
//			}
//			log("Position List Size: %d for Depart %d", pDuty->position_uuid_list_size(), i);
//		}
//		log("Duty List Size: %d", pBaseInfo->duty_list_size());
//
//		//get shield users 
//		nTotals = user_info.black_list_size();
//		for (int i = 0; i < nTotals; i++)
//		{
//			IM::BaseDefine::ShieldUser ShieldUser = user_info.black_list(i);
//			IM::BaseDefine::ShieldUser* pShieldUser = pBaseInfo->add_black_list();
//			pShieldUser->set_user_id(ShieldUser.user_id());
//			pShieldUser->set_shield_status(ShieldUser.shield_status());
//		}
//		log("Black List Size: %d",pBaseInfo->black_list_size());
//
//		//get additional information
//		nTotals = user_info.add_info_list_size();
//		for (int i = 0; i < nTotals; i++)
//		{
//			//get additional information for a user
//			IM::BaseDefine::UserDetails *pAddInfo = user_info_tmp->add_add_info_list();
//			IM::BaseDefine::UserDetails AddInfo = user_info.add_info_list(i);
//			pAddInfo->set_details_uuid(AddInfo.details_uuid());
//			pAddInfo->set_details_type(AddInfo.details_type());
//			pAddInfo->set_details_title(AddInfo.details_title());
//			pAddInfo->set_details_content(AddInfo.details_content());
//		}
//		log("Additional Informations: %d", user_info_tmp->add_info_list_size());


		//get configure items
		CConfigFileReader ClientConfig("clientconfig.conf", false);
		map<string, string>* pmapConfigItems = ClientConfig.GetConfigItems();
		for (map<string, string>::iterator it = pmapConfigItems->begin(); it != pmapConfigItems->end(); it++)
		{
			IM::BaseDefine::ConfigPara* pConfigItems = msg3.add_config_item_list();
			pConfigItems->set_key(it->first);
			/*
			//AvatarUrl1=http://ioa.chinac.com/fs/file/previewFilePublic?fileId=
			//jichunlai@chinac.com, test@ht1.huayunyou.com
			if (key == "AvatarUrl1"){
				set_value("http://ioa.%s/fs/file/previewFilePublic?fileId=", domain);
			}
			*/
			pConfigItems->set_value(it->second);
		}
		pmapConfigItems = NULL;
		//end of adding 

        CImPdu pdu2;
        pdu2.SetPBMsg(&msg3);
        pdu2.SetServiceId(SID_LOGIN);
        pdu2.SetCommandId(CID_LOGIN_RES_USERLOGIN);
        pdu2.SetSeqNum(pPdu->GetSeqNum());
        pMsgConn->SendPdu(&pdu2);
    }
    else
    {
        IM::Login::IMLoginRes msg4;
        msg4.set_server_time(time(NULL));
        msg4.set_result_code((IM::BaseDefine::ResultType)result);
		if (result != IM::BaseDefine::REFUSE_REASON_USER_NOT_EXIST && result != IM::BaseDefine::REFUSE_REASON_INVALID_USER_PASSWORD)
		{
			//get configure items
			CConfigFileReader ClientConfig("clientconfig.conf", false);
			map<string, string>* pmapConfigItems = ClientConfig.GetConfigItems();
			for (map<string, string>::iterator it = pmapConfigItems->begin(); it != pmapConfigItems->end(); it++)
			{
				IM::BaseDefine::ConfigPara* pConfigItems = msg4.add_config_item_list();
				pConfigItems->set_key(it->first);
				pConfigItems->set_value(it->second);
			}
			pmapConfigItems = NULL;
		}

		log("ResultCode1: %d, ResultCode2:%d", result, msg.result_code());
        msg4.set_result_string(result_string);
        CImPdu pdu3;
        pdu3.SetPBMsg(&msg4);
        pdu3.SetServiceId(SID_LOGIN);
        pdu3.SetCommandId(CID_LOGIN_RES_USERLOGIN);
        pdu3.SetSeqNum(pPdu->GetSeqNum());
        pMsgConn->SendPdu(&pdu3);

		if (result != IM::BaseDefine::REFUSE_REASON_INVALID_ALIAS_PASSWORD)
		{
			pMsgConn->Close();
		}
    }
}

void CDBServConn::_HandleRecentSessionResponse(CImPdu *pPdu)
{
    IM::Buddy::IMRecentContactSessionRsp msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
    uint32_t user_id = msg.user_id();
    uint32_t session_cnt = msg.contact_session_list_size();
    CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
    uint32_t handle = attach_data.GetHandle();
    
    log("HandleRecentSessionResponse, userId=%u, session_cnt=%u", user_id, session_cnt);
    
    CMsgConn* pMsgConn = CImUserManager::GetInstance()->GetMsgConnByHandle(user_id, handle);
    
    if (pMsgConn && pMsgConn->IsOpen())
    {
        msg.clear_attach_data();
        pPdu->SetPBMsg(&msg);
        pMsgConn->SendPdu(pPdu);
    }
}

void CDBServConn::_HandleAllUserResponse(CImPdu *pPdu)
{
    IM::Buddy::IMAllUserRsp msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
    
    uint32_t user_id = msg.user_id();
    uint32_t latest_update_time = msg.latest_update_time();
    uint32_t user_cnt = msg.user_list_size();
	uint32_t user_total_cnt = msg.user_count();
    CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
    uint32_t handle = attach_data.GetHandle();
    
    log("HandleAllUserResponse, userId=%u, latest_update_time=%u, user_cnt=%u,total normal count: %d", user_id, latest_update_time,user_cnt,user_total_cnt);
    
    CMsgConn* pMsgConn = CImUserManager::GetInstance()->GetMsgConnByHandle(user_id, handle);
    
    if (pMsgConn && pMsgConn->IsOpen())
    {
        msg.clear_attach_data();
		pPdu->SetPBMsg(&msg);
		int nBytes = pMsgConn->SendPdu(pPdu);
		log("Send %d bytes to the client when handling all user response", nBytes);
    }
}

void CDBServConn::_HandleUpdateUserNagureResponse(CImPdu *pPdu)
{
	IM::Buddy::IMUpdateUserNagureRsp msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

	uint32_t user_id = msg.user_id();
	uint32_t latest_update_time = msg.latest_update_time();
	log("_HandleUpdateUserNagureResponse, user_id=%u, latest_update_time=%u", user_id, latest_update_time);

	CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
	uint32_t handle = attach_data.GetHandle();
	CMsgConn* pConn = CImUserManager::GetInstance()->GetMsgConnByHandle(user_id, handle);
	if (pConn && pConn->IsOpen()) 
	{
		msg.clear_attach_data();
		pPdu->SetPBMsg(&msg);
		pConn->SendPdu(pPdu);
	}

	if (latest_update_time) //update nagure successfully
	{
		IM::Buddy::IMUpdateUserNagureNotify msgNotify;
		msgNotify.set_user_id(user_id);
		msgNotify.set_nagure(msg.nagure());

		CImPdu pdu;
		pdu.SetPBMsg(&msgNotify);
		pdu.SetServiceId(SID_BUDDY_LIST);
		pdu.SetCommandId(CID_BUDDY_LIST_UPDATE_NAGURE_NOTIFY);

	/*	CRouteServConn* pRouteConn = get_route_serv_conn();
		if (pRouteConn) {
			pRouteConn->SendPdu(&pdu);
		}*/

		CImUserManager::GetInstance()->BroadcastPdu(&pdu, CLIENT_TYPE_FLAG_BOTH);
	}
}
void CDBServConn::_HandleUpdateUserResponse(CImPdu *pPdu)
{
	IM::Buddy::IMUpdateUserRsp msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

	uint32_t user_id = msg.user_id();
	uint32_t latest_update_time = msg.latest_update_time();
	log("_HandleUpdateUserResponse, user_id=%u, latest_update_time=%u", user_id, latest_update_time);

	CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
	uint32_t handle = attach_data.GetHandle();
	CMsgConn* pConn = CImUserManager::GetInstance()->GetMsgConnByHandle(user_id, handle);
	if (pConn && pConn->IsOpen()) 
	{
		msg.clear_attach_data();
		pPdu->SetPBMsg(&msg);
		pConn->SendPdu(pPdu);
	}

	if (latest_update_time) //updated user information successfully
	{
		IM::Buddy::IMUpdateUserNotify msgNotify;
		msgNotify.set_user_id(msg.user_id()); 
		msgNotify.set_signature(msg.signature());
		msgNotify.set_sex(msg.sex());
		msgNotify.set_birthday(msg.birthday());
		msgNotify.set_user_real_name(msg.user_real_name());
		msgNotify.set_user_uuid(msg.user_uuid());

		int nCount = msg.user_details_list_size();
		for (int i = 0; i < nCount; i++)
		{
			IM::BaseDefine::UserDetails *pUserDetails = msgNotify.add_user_details_list();
			IM::BaseDefine::UserDetails  Details = msg.user_details_list(i);
			pUserDetails->set_details_uuid(Details.details_uuid());
			pUserDetails->set_details_type(Details.details_type());
			pUserDetails->set_details_title(Details.details_title());
			pUserDetails->set_details_content(Details.details_content());
		}

		CImPdu pdu;
		pdu.SetPBMsg(&msgNotify);
		pdu.SetServiceId(SID_BUDDY_LIST);
		pdu.SetCommandId(CID_BUDDY_LIST_UPDATE_INFO_NOTIFY);

		/*	CRouteServConn* pRouteConn = get_route_serv_conn();
		if (pRouteConn) {
		pRouteConn->SendPdu(&pdu);
		}*/

		CImUserManager::GetInstance()->BroadcastPdu(&pdu, CLIENT_TYPE_FLAG_BOTH);
	}
}
void CDBServConn::_HandleGetMsgListResponse(CImPdu *pPdu)
{
    IM::Message::IMGetMsgListRsp msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

    uint32_t user_id = msg.user_id();
    uint32_t session_type = msg.session_type();
    uint32_t session_id = msg.session_id();
    uint32_t msg_cnt = msg.msg_list_size();
    uint32_t msg_id_begin = msg.msg_id_begin();
    CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
    uint32_t handle = attach_data.GetHandle();
    
    log("HandleGetMsgListResponse, userId=%u, session_type=%u, opposite_user_id=%u, msg_id_begin=%u, cnt=%u.", user_id, session_type, session_id, msg_id_begin, msg_cnt);
    
    CMsgConn* pMsgConn = CImUserManager::GetInstance()->GetMsgConnByHandle(user_id, handle);
    if (pMsgConn && pMsgConn->IsOpen()) {
        msg.clear_attach_data();
        pPdu->SetPBMsg(&msg);
        pMsgConn->SendPdu(pPdu);
    }
}

void CDBServConn::_HandleGetMsgByIdResponse(CImPdu *pPdu)
{
    IM::Message::IMGetMsgByIdRsp msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
    
    uint32_t user_id = msg.user_id();
    uint32_t session_type = msg.session_type();
    uint32_t session_id = msg.session_id();
    uint32_t msg_cnt = msg.msg_list_size();
    CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
    uint32_t handle = attach_data.GetHandle();
    
    log("HandleGetMsgByIdResponse, userId=%u, session_type=%u, opposite_user_id=%u, cnt=%u.", user_id, session_type, session_id, msg_cnt);
    
    CMsgConn* pMsgConn = CImUserManager::GetInstance()->GetMsgConnByHandle(user_id, handle);
    if (pMsgConn && pMsgConn->IsOpen()) {
        msg.clear_attach_data();
        pPdu->SetPBMsg(&msg);
        pMsgConn->SendPdu(pPdu);
    }
}

void CDBServConn::_HandleMsgData(CImPdu *pPdu)
{
    IM::Message::IMMsgData msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
    if (CHECK_MSG_TYPE_GROUP(msg.msg_type())) {
		log("handle message data from database proxy server for a group");
        s_group_chat->HandleGroupMessage(pPdu);
        return;
    }
    
    uint32_t from_user_id = msg.from_user_id();
    uint32_t to_user_id = msg.to_session_id();
    uint32_t msg_id = msg.msg_id();
	uint8_t msg_type = msg.msg_type();

   /* if (msg_id == 0) {
        log("HandleMsgData, write db failed, %u->%u.", from_user_id, to_user_id);
        return;
    }
    
    uint8_t msg_type = msg.msg_type();
    CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
    uint32_t handle = attach_data.GetHandle();
    
    log("HandleMsgData, from_user_id=%u, to_user_id=%u, msg_id=%u.", from_user_id, to_user_id, msg_id);*/

	uint32_t nResult = 0;
	if (msg_id == 0)
	{
		//log("HandleMsgData, write db failed, %u->%u.", from_user_id, to_user_id);
		nResult = (uint32_t)(atoi(msg.msg_data().c_str()));
		//nResult = 0x0307; //change pb later
		log("HandleMsgData, error code: %u, %u->%u.", nResult , from_user_id, to_user_id);
		//nResult = ::IM::BaseDefine::MESSAGE_SEND_ERROR_WRITE_DB_ERROR;
	}

	CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
	uint32_t handle = attach_data.GetHandle();

	log("HandleMsgData, from_user_id=%u, to_user_id=%u, msg_id=%u.", from_user_id, to_user_id, msg_id);

	CMsgConn* pMsgConn = CImUserManager::GetInstance()->GetMsgConnByHandle(from_user_id, attach_data.GetHandle());
	if (pMsgConn)
	{

		pMsgConn->AddToRecvList(msg_id, from_user_id, to_user_id, pPdu->GetSeqNum(), IM::BaseDefine::SESSION_TYPE_SINGLE); //george added on Jan 6th,2016

		IM::Message::IMMsgDataAck msg2;
		msg2.set_user_id(from_user_id);
		msg2.set_msg_id(msg_id);
		msg2.set_session_id(to_user_id);
		msg2.set_session_type(::IM::BaseDefine::SESSION_TYPE_SINGLE);
		msg2.set_result_code(nResult);
		CImPdu pdu;
		pdu.SetPBMsg(&msg2);
		pdu.SetServiceId(SID_MSG);
		pdu.SetCommandId(CID_MSG_DATA_ACK);
		pdu.SetSeqNum(pPdu->GetSeqNum());
		pMsgConn->SendPdu(&pdu);
		log("HandleMsgDataACK, msg server sends a MSG_DATA_ACK packet to the sender: %d", pMsgConn->GetHandle());
	}
    
	if (!nResult)
	{
		CRouteServConn* pRouteConn = get_route_serv_conn();
		if (pRouteConn) {
			pRouteConn->SendPdu(pPdu);
		}

		msg.clear_attach_data();
		pPdu->SetPBMsg(&msg);
		CImUser* pFromImUser = CImUserManager::GetInstance()->GetImUserById(from_user_id);
		CImUser* pToImUser = CImUserManager::GetInstance()->GetImUserById(to_user_id);
		//pPdu->SetSeqNum(0);
		if (pFromImUser) {
			pFromImUser->BroadcastClientMsgData(pPdu, msg_id, to_user_id,pPdu->GetSeqNum(), pMsgConn, from_user_id);
		}

		if (pToImUser) {
			pToImUser->BroadcastClientMsgData(pPdu, msg_id, to_user_id,pPdu->GetSeqNum(), NULL, from_user_id);
			log("msg server sends a  MSG_DATA packet to the receiver: %d", pToImUser->GetUserId());
		}


		//Push相关
		IM::Server::IMUserGetShieldReq msg3;
		msg3.set_to_user_id(to_user_id);
		msg3.set_from_user_id(from_user_id);
		msg3.set_attach_data(pPdu->GetBodyData(), pPdu->GetBodyLength());
		
		CImPdu pdu2;
		pdu2.SetPBMsg(&msg3);
		pdu2.SetServiceId(SID_OTHER);
		pdu2.SetCommandId(CID_OTHER_GET_SHIELD_USER_REQ);
		CDBServConn* pDbConn = get_db_serv_conn();
		if (pDbConn)
		{
			pDbConn->SendPdu(&pdu2);
		}


		/*IM::Server::IMGetDeviceTokenReq msg3;
		msg3.add_user_id(to_user_id);
		msg3.set_attach_data(pPdu->GetBodyData(), pPdu->GetBodyLength());
		CImPdu pdu2;
		pdu2.SetPBMsg(&msg3);
		pdu2.SetServiceId(SID_OTHER);
		pdu2.SetCommandId(CID_OTHER_GET_DEVICE_TOKEN_REQ);
		SendPdu(&pdu2);*/
	}
}

void CDBServConn::_HandleGetLatestMsgIDRsp(CImPdu *pPdu)
{
    IM::Message::IMGetLatestMsgIdRsp msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

    uint32_t user_id = msg.user_id();
    uint32_t session_id = msg.session_id();
    uint32_t session_type = msg.session_type();
    uint32_t latest_msg_id = msg.latest_msg_id();
    CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
    uint32_t handle = attach_data.GetHandle();
    
    log("HandleUnreadMsgCntResp, userId=%u, session_id=%u, session_type=%u, latest_msg_id=%u.",
        user_id, session_id, session_type, latest_msg_id);
    
    CMsgConn* pMsgConn = CImUserManager::GetInstance()->GetMsgConnByHandle(user_id, handle);
    if (pMsgConn && pMsgConn->IsOpen()) {
        msg.clear_attach_data();
        pPdu->SetPBMsg(&msg);
        pMsgConn->SendPdu(pPdu);
    }
}

void CDBServConn::_HandleUnreadMsgCountResponse(CImPdu* pPdu)
{
    IM::Message::IMUnreadMsgCntRsp msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

	uint32_t user_id = msg.user_id();
    uint32_t total_cnt = msg.total_cnt();
	uint32_t user_unread_cnt = msg.unreadinfo_list_size();
    CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
	uint32_t handle = attach_data.GetHandle();
	
	log("HandleUnreadMsgCntResp, userId=%u, total_cnt=%u, user_unread_cnt=%u.", user_id,
        total_cnt, user_unread_cnt);

    CMsgConn* pMsgConn = CImUserManager::GetInstance()->GetMsgConnByHandle(user_id, handle);

	if (pMsgConn && pMsgConn->IsOpen()) {
        msg.clear_attach_data();
        pPdu->SetPBMsg(&msg);
        pMsgConn->SendPdu(pPdu);
	}
}

void CDBServConn::_HandleUsersInfoResponse(CImPdu* pPdu)
{
    IM::Buddy::IMUsersInfoRsp msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

    uint32_t user_id = msg.user_id();
    uint32_t user_cnt = msg.user_info_list_size();
    CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
	uint32_t handle = attach_data.GetHandle();
    
    log("HandleUsersInfoResp, user_id=%u, user_cnt=%u.", user_id, user_cnt);
    
    CMsgConn* pMsgConn = CImUserManager::GetInstance()->GetMsgConnByHandle(user_id, handle);
    if (pMsgConn && pMsgConn->IsOpen())
	{
        msg.clear_attach_data();
        pPdu->SetPBMsg(&msg);
        pMsgConn->SendPdu(pPdu);
    }
}

void CDBServConn::_HandleStopReceivePacket(CImPdu* pPdu)
{
	log("HandleStopReceivePacket, from %s:%d.",
			g_db_server_list[m_serv_idx].server_ip.c_str(), g_db_server_list[m_serv_idx].server_port);

	m_bOpen = false;
}

void CDBServConn::_HandleRemoveSessionResponse(CImPdu* pPdu)
{
    IM::Buddy::IMRemoveSessionRsp msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

	uint32_t user_id = msg.user_id();
	uint32_t result = msg.result_code();
	uint32_t session_type = msg.session_type();
	uint32_t session_id = msg.session_id();
	log("HandleRemoveSessionResp, req_id=%u, result=%u, session_id=%u, type=%u.",
			user_id, result, session_id, session_type);

    CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
    uint32_t handle = attach_data.GetHandle();
    CMsgConn* pConn = CImUserManager::GetInstance()->GetMsgConnByHandle(user_id, handle);
	if (pConn && pConn->IsOpen()) {
        msg.clear_attach_data();
        pPdu->SetPBMsg(&msg);
        pConn->SendPdu(pPdu);
	}


	if(!result) //successfully removed the session
 	{
		IM::Buddy::IMRemoveSessionNotify msg2;
		msg2.set_user_id(user_id);
		msg2.set_session_id(session_id);
		msg2.set_session_type((IM::BaseDefine::SessionType)session_type);

		CImPdu pdu;
		pdu.SetPBMsg(&msg2);
		pdu.SetServiceId(SID_BUDDY_LIST);
		pdu.SetCommandId(CID_BUDDY_LIST_REMOVE_SESSION_NOTIFY);

		CImUser* pImUser = CImUserManager::GetInstance()->GetImUserById(user_id);
		if (pImUser)
		{
			//CImUserManager::GetInstance()->BroadcastPdu(&pdu, CLIENT_TYPE_FLAG_BOTH);
			pImUser->BroadcastPdu(&pdu);	
		}

		CRouteServConn* pRouteConn = get_route_serv_conn();
		if (pRouteConn)
		{
			pRouteConn->SendPdu(&pdu);
		}
	}
}

void CDBServConn::_HandleChangeAvatarResponse(CImPdu* pPdu)
{
    IM::Buddy::IMChangeAvatarAndNotifyRsp msg;
	IM::Buddy::IMChangeAvatarRsp msg2;
	
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

    uint32_t user_id = msg.user_id();
    uint32_t result = msg.result_code();
	string strAvatar = msg.avatar_url();

	//message body of response for changing avatar
	msg2.set_user_id(user_id);
	msg2.set_result_code(result);
	msg2.set_attach_data(msg.attach_data());
  
	log("HandleChangeAvatarResp, user_id=%u, result=%u, avatar_url: %s.", user_id, result, strAvatar.c_str());

	CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
	uint32_t handle = attach_data.GetHandle();
	CMsgConn* pConn = CImUserManager::GetInstance()->GetMsgConnByHandle(user_id, handle);
	if (pConn && pConn->IsOpen())
	{
		msg.clear_attach_data();
		pPdu->SetPBMsg(&msg);
		pConn->SendPdu(pPdu);
	}
    
    //CImUser* pUser = CImUserManager::GetInstance()->GetImUserById(user_id);
    //if (NULL != pUser) {
    //    //msg.clear_attach_data();
    //    pPdu->SetPBMsg(&msg2);
    //    pUser->BroadcastPdu(pPdu);
    //}

	if (!result)
	{
		IM::Buddy::IMChangeAvatarNotify msgNotify;
		msgNotify.set_user_id(user_id);
		msgNotify.set_avatar_url(strAvatar);

		CImPdu pdu;
		pdu.SetPBMsg(&msgNotify);
		pdu.SetServiceId(SID_BUDDY_LIST);
		pdu.SetCommandId(CID_BUDDY_LIST_CHANGE_AVATAR_NOTIFY);

		CRouteServConn* pRouteConn = get_route_serv_conn();
		if (pRouteConn)
		{
			pRouteConn->SendPdu(&pdu);
		}

		CImUserManager::GetInstance()->BroadcastPdu(&pdu, CLIENT_TYPE_FLAG_BOTH, pConn);
	}
}

void CDBServConn::_HandleDepartmentResponse(CImPdu *pPdu)
{
    IM::Buddy::IMDepartmentRsp msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
    
    uint32_t user_id = msg.user_id();
    uint32_t latest_update_time = msg.latest_update_time();
    uint32_t dept_cnt = msg.dept_list_size();
	uint32_t unCompanyId = msg.company_id();
    log("HandleDepartmentResponse, user_id=%u, company_id=%u, latest_update_time=%u, dept_cnt=%u.", user_id, unCompanyId, latest_update_time, dept_cnt);
    
    CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
    uint32_t handle = attach_data.GetHandle();
    CMsgConn* pConn = CImUserManager::GetInstance()->GetMsgConnByHandle(user_id, handle);
    if (pConn && pConn->IsOpen())
	{
        pPdu->SetPBMsg(&msg);
        int nBytes = pConn->SendPdu(pPdu);
		msg.clear_attach_data();
		log("Send %d bytes to the client when handling department response", nBytes);
    }
}
void CDBServConn::_HandleCompanyResponse(CImPdu *pPdu)
{
	IM::Buddy::IMCompanyRsp msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

	uint32_t user_id = msg.user_id();
	uint32_t latest_update_time = msg.latest_update_time();
	log("_HandleCompanyResponse, user_id=%u, latest_update_time=%u", user_id, latest_update_time);

	CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
	uint32_t handle = attach_data.GetHandle();
	CMsgConn* pConn = CImUserManager::GetInstance()->GetMsgConnByHandle(user_id, handle);
	if (pConn && pConn->IsOpen()) 
	{
		msg.clear_attach_data();
		pPdu->SetPBMsg(&msg);
		pConn->SendPdu(pPdu);
	}
}
void CDBServConn::_HandleSetDeviceTokenResponse(CImPdu *pPdu)
{
    IM::Login::IMDeviceTokenRsp msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

    uint32_t user_id = msg.user_id();
    log("HandleSetDeviceTokenResponse, user_id = %u.", user_id);
}

void CDBServConn::_HandleGetDeviceTokenResponse(CImPdu *pPdu)
{
    IM::Server::IMGetDeviceTokenRsp msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
    
    IM::Message::IMMsgData msg2;
    CHECK_PB_PARSE_MSG(msg2.ParseFromArray(msg.attach_data().c_str(), msg.attach_data().length()));
    string msg_data = msg2.msg_data();
    uint32_t msg_type = msg2.msg_type();
    uint32_t from_id = msg2.from_user_id();
    uint32_t to_id = msg2.to_session_id();
	
    if (msg_type == IM::BaseDefine::MSG_TYPE_SINGLE_TEXT || msg_type == IM::BaseDefine::MSG_TYPE_GROUP_TEXT)
    {
        //msg_data =
        char* msg_out = NULL;
        uint32_t msg_out_len = 0;
        if (pAes->Decrypt(msg_data.c_str(), msg_data.length(), &msg_out, msg_out_len) == 0)
        {
            msg_data = string(msg_out, msg_out_len);
        }
        else
        {
            log("HandleGetDeviceTokenResponse, decrypt msg failed, from_id: %u, to_id: %u, msg_type: %u.", from_id, to_id, msg_type);
            return;
        }
        pAes->Free(msg_out);
    }
    
    build_ios_push_flash(msg_data, msg2.msg_type(), from_id);

    //{
    //    "msg_type": 1,
    //    "from_id": "1345232",
    //    "group_type": "12353",
    //}
    jsonxx::Object json_obj;
    json_obj << "msg_type" << (uint32_t)msg2.msg_type();
    json_obj << "from_id" << from_id;
	json_obj << "msg_id" << msg2.msg_id();
    if (CHECK_MSG_TYPE_GROUP(msg2.msg_type()))
	{
        json_obj << "group_id" << to_id;
		log("*********************Push message for group: %d*************************", to_id);
    }
    
    uint32_t user_token_cnt = msg.user_token_info_size();
    log("HandleGetDeviceTokenResponse, user_token_cnt = %u.", user_token_cnt);
    
	//get at user list
	uint32_t nAtUsers = msg2.at_user_list_size();
	list<uint32_t> lsAtUser;
	bool bAtAll = false;

	//set at message data
	string strAtMsgData = "";
	if (nAtUsers > 0)
	{
		CImUser* pImUser = CImUserManager::GetInstance()->GetImUserById(from_id);
		if (pImUser)
		{
			strAtMsgData = pImUser->GetNickName();
			strAtMsgData.append("在群聊中@你");
		}

		if (nAtUsers == 1 && msg2.at_user_list(0) == 9999)
		{
			log("At all...................");
			bAtAll = true;
		}	

		for (int i = 0; i < nAtUsers; i++)
		{
			lsAtUser.push_back(msg2.at_user_list(i));
		}
	}

    IM::Server::IMPushToUserReq msg3;
	IM::Server::IMPushToUserReq msg5; //push at message

    for (uint32_t i = 0; i < user_token_cnt; i++)
    {
        IM::BaseDefine::UserTokenInfo user_token = msg.user_token_info(i);
        uint32_t user_id = user_token.user_id();
        string device_token = user_token.token();
        uint32_t push_cnt = user_token.push_count();
        uint32_t client_type = user_token.user_type();
        //自己发得消息不给自己发推送
        if (from_id == user_id) 
		{
            continue;
        } 

        log("HandleGetDeviceTokenResponse, user_id = %u, device_token = %s, push_cnt = %u, client_type = %u.",
            user_id, device_token.c_str(), push_cnt, client_type);
        
        CImUser* pUser = CImUserManager::GetInstance()->GetImUserById(user_id);
        if (pUser)
        {
			IM::BaseDefine::UserTokenInfo* user_token_tmp = NULL;
			if (nAtUsers > 0)  //just push to at users
			{
				if (bAtAll)
				{
					user_token_tmp = msg5.add_user_token_list();
				}
				else
				{
					list<uint32_t>::iterator it = find(lsAtUser.begin(), lsAtUser.end(), user_id);
					if (it != lsAtUser.end())
					{
						user_token_tmp = msg5.add_user_token_list();
						log("At users to be pushed to %d", user_id);
					}
					else
					{
						user_token_tmp = msg3.add_user_token_list();
						log("normal message to be pushed to %d", user_id);
					}
				}				
			}
			else
			{
				user_token_tmp = msg3.add_user_token_list();
			}
			
			
            /*IM::BaseDefine::UserTokenInfo* user_token_tmp = msg3.add_user_token_list();*/
            user_token_tmp->set_user_id(user_id);
            user_token_tmp->set_user_type((IM::BaseDefine::ClientType)client_type);
            user_token_tmp->set_token(device_token);
            user_token_tmp->set_push_count(push_cnt);

            //////pc client登录，则为勿打扰式推送
            //if (user_info_tmp->GetPCLoginStatus() == IM_PC_LOGIN_STATUS_ON)
            //{
            //    user_token_tmp->set_push_type(IM_PUSH_TYPE_SILENT);
            //    log("HandleGetDeviceTokenResponse, user id: %d, push type: silent.", user_id);
            //}
            //else
            {
				//always set normal push
                user_token_tmp->set_push_type(IM_PUSH_TYPE_NORMAL);
                log("HandleGetDeviceTokenResponse, user id: %d, push type: normal.", user_id);
            }
        }
        else
        {
            IM::Server::IMPushToUserReq msg4;
            msg4.set_flash(msg_data);
            msg4.set_data(json_obj.json());
            IM::BaseDefine::UserTokenInfo* user_token_tmp = msg4.add_user_token_list();
            user_token_tmp->set_user_id(user_id);
            user_token_tmp->set_user_type((IM::BaseDefine::ClientType)client_type);
            user_token_tmp->set_token(device_token);
            user_token_tmp->set_push_count(push_cnt);
            user_token_tmp->set_push_type(IM_PUSH_TYPE_NORMAL);
            CImPdu pdu;
            pdu.SetPBMsg(&msg4);
            pdu.SetServiceId(SID_OTHER);
            pdu.SetCommandId(CID_OTHER_PUSH_TO_USER_REQ);
            
            CPduAttachData attach_data(ATTACH_TYPE_PDU_FOR_PUSH, 0, pdu.GetBodyLength(), pdu.GetBodyData());
            IM::Buddy::IMUsersStatReq msg5;
            msg5.set_user_id(0);
            msg5.add_user_id_list(user_id);
            msg5.set_attach_data(attach_data.GetBuffer(), attach_data.GetLength());
            CImPdu pdu2;
            pdu2.SetPBMsg(&msg5);
            pdu2.SetServiceId(SID_BUDDY_LIST);
            pdu2.SetCommandId(CID_BUDDY_LIST_USERS_STATUS_REQUEST);
            CRouteServConn* route_conn = get_route_serv_conn();
            if (route_conn)
            {
                route_conn->SendPdu(&pdu2);
            }
        }
    }
    
	string strJsonValue = json_obj.json();

    if (msg3.user_token_list_size() > 0) //push normal message
    {
	/*	msg3.set_flash(msg_data);
		msg3.set_data(json_obj.json());*/
		SendPacketToPushServer(msg_data, strJsonValue, &msg3);
		log("sending normal message to push server for %d users ..................", msg3.user_token_list_size());
      /*  pdu3.SetServiceId(SID_OTHER);
        pdu3.SetCommandId(CID_OTHER_PUSH_TO_USER_REQ);
        
        CPushServConn* PushConn = get_push_serv_conn();
        if (PushConn)
		{
            PushConn->SendPdu(&pdu3);
			log("sending normal message to push server for %d users ..................", msg3.user_token_list_size());
        }*/
    }

	if (msg5.user_token_list_size() > 0)   //push at message
	{
		/*msg5.set_flash(strAtMsgData);
		msg5.set_data(json_obj.json());*/
		SendPacketToPushServer(strAtMsgData, strJsonValue, &msg5);
		log("sending at message to push server for %d users ..................", msg5.user_token_list_size());
		/*pdu3.SetServiceId(SID_OTHER);
		pdu3.SetCommandId(CID_OTHER_PUSH_TO_USER_REQ);

		CPushServConn* PushConn = get_push_serv_conn();
		if (PushConn)
		{
			PushConn->SendPdu(&pdu3);
			log("sending at message to push server for %d users ..................", msg5.user_token_list_size());
		}*/
	}

}


void CDBServConn::_HandleUserShieldUserResponse(CImPdu* pPdu)
{
	IM::Buddy::IMUserShieldRsp msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

	uint32_t result = msg.result_code();
	uint32_t user_id = msg.user_id();
	uint32_t peer_id = msg.peer_id();
	log("HandleUserShieldUserResponse, result: %u, user_id: %u, peer_id: %u. ", result,
		user_id, peer_id);

	CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
	CMsgConn* pMsgConn = CImUserManager::GetInstance()->GetMsgConnByHandle(user_id,
		attach_data.GetHandle());
	if (pMsgConn)
	{
		msg.clear_attach_data();
		pPdu->SetPBMsg(&msg);
		pMsgConn->SendPdu(pPdu);
	}
}


void CDBServConn::_HandleUserGetShieldByUserResponse(CImPdu *pPdu)
{
	IM::Server::IMUserGetShieldRsp msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

	uint32_t to_user_id = msg.to_user_id();
	IM::BaseDefine::ShieldUser ShieldUser = msg.shield_status();
	
	log("HandleUserGetShieldByUserResponse: %d--->%d: %d,",to_user_id, ShieldUser.user_id(), ShieldUser.shield_status());

	if (ShieldUser.shield_status() == 0)
	{
		IM::Server::IMGetDeviceTokenReq msg2;
		msg2.set_attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
		msg2.add_user_id(to_user_id);
		CImPdu pdu;
		pdu.SetPBMsg(&msg2);
		pdu.SetServiceId(SID_OTHER);
		pdu.SetCommandId(CID_OTHER_GET_DEVICE_TOKEN_REQ);
		CDBServConn* pDbConn = get_db_serv_conn();
		if (pDbConn)
		{
			pDbConn->SendPdu(&pdu);
		}
	}
}


void CDBServConn::_HandleResendMessageResponse(CImPdu* pPdu)
{
	IM::Message::IMGetResendMsgByIdRsp msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
	//uint32_t user_id = msg.user_id();
	//uint32_t session_type = msg.session_type();
	//uint32_t session_id = msg.session_id();
	uint32_t msg_cnt = msg.msg_list_size();
	if (msg_cnt > 0)
	{
		IM::Message::IMMsgData msg2;
	
		IM::BaseDefine::MsgInfo MsgDetails = msg.msg_list(0);
		uint32_t user_id = msg.user_id();
		msg2.set_from_user_id(user_id);
		msg2.set_to_session_id(msg.session_id());
		msg2.set_msg_id(MsgDetails.msg_id());
		msg2.set_create_time(MsgDetails.create_time());
		msg2.set_msg_type(MsgDetails.msg_type());
		msg2.set_msg_data(MsgDetails.msg_data());
		uint32_t resend_user_id = msg.resend_user_id();
			
		CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
		uint32_t handle = attach_data.GetHandle();
		//msg2.set_attach_data(attach_data);

		log("HandleResendMessageResponse, resend_user_id=%u, from_id=%u, message_type=%u, to_user_id=%u, cnt=%u.", resend_user_id, user_id, MsgDetails.msg_type(), msg.session_id(), msg_cnt);

		CMsgConn* pMsgConn = CImUserManager::GetInstance()->GetMsgConnByHandle(resend_user_id, handle);
		if (pMsgConn && !pMsgConn->IsKickOff())
		{
			log("User: %d,Handle: %d", resend_user_id, handle);
			CImPdu pdu;
			pdu.SetPBMsg(&msg2);
			pdu.SetServiceId(SID_MSG);
			pdu.SetCommandId(CID_MSG_DATA);
			pdu.SetSeqNum(pPdu->GetSeqNum());
			pMsgConn->SendPdu(&pdu);
		}
	}
}


void CDBServConn::_HandlePositionResponse(CImPdu* pPdu)
{
	IM::Buddy::IMPositionRsp msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

	uint32_t user_id = msg.user_id();
	uint32_t latest_update_time = msg.latest_update_time();
	uint32_t position_cnt = msg.position_list_size();
	uint32_t unCompanyId = msg.company_id();

	log("HandlePositionResponse, user_id=%u, company_id=%u, latest_update_time=%u, dept_cnt=%u.", user_id, unCompanyId, latest_update_time, position_cnt);

	CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
	uint32_t handle = attach_data.GetHandle();
	CMsgConn* pConn = CImUserManager::GetInstance()->GetMsgConnByHandle(user_id, handle);
	if (pConn && pConn->IsOpen())
	{
		pPdu->SetPBMsg(&msg);
		pConn->SendPdu(pPdu);
		msg.clear_attach_data();
	}
}


void CDBServConn::_HandleAllUserLiteResponse(CImPdu* pPdu)
{
	/*IM::Buddy::IMAllUserLiteRsp msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

	uint32_t user_id = msg.user_id();
	uint32_t latest_update_time = msg.latest_update_time();
	uint32_t user_cnt = msg.user_list_size();
	CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
	uint32_t handle = attach_data.GetHandle();

	log("HandleAllUserResponse, userId=%u, latest_update_time=%u, user_cnt=%u", user_id, latest_update_time, user_cnt);

	CMsgConn* pMsgConn = CImUserManager::GetInstance()->GetMsgConnByHandle(user_id, handle);

	if (pMsgConn && pMsgConn->IsOpen())
	{
		msg.clear_attach_data();
		pPdu->SetPBMsg(&msg);
		int nBytes = pMsgConn->SendPdu(pPdu);
		log("send %d bytes to the client", nBytes);
	}*/
}


void CDBServConn::_HandleUserInfoLiteResponse(CImPdu* pPdu)
{
	/*IM::Buddy::IMUserInfoLiteRsp msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

	uint32_t user_id = msg.user_id();
	uint32_t user_cnt = msg.user_info_list_size();
	CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
	uint32_t handle = attach_data.GetHandle();

	log("HandleUsersInfoLiteResp, user_id=%u, user_cnt=%u.", user_id, user_cnt);

	CMsgConn* pMsgConn = CImUserManager::GetInstance()->GetMsgConnByHandle(user_id, handle);
	if (pMsgConn && pMsgConn->IsOpen())
	{
		msg.clear_attach_data();
		pPdu->SetPBMsg(&msg);
		pMsgConn->SendPdu(pPdu);
	}*/
}


void CDBServConn::SendPacketToPushServer(string& strData, string& strJson, void* pMessage)
{
	IM::Server::IMPushToUserReq *pMsg = (IM::Server::IMPushToUserReq*)pMessage;
	pMsg->set_flash(strData);
	pMsg->set_data(strJson);

	CImPdu pdu;
	pdu.SetPBMsg(pMsg);
	pdu.SetServiceId(SID_OTHER);
	pdu.SetCommandId(CID_OTHER_PUSH_TO_USER_REQ);

	CPushServConn* PushConn = get_push_serv_conn();
	if (PushConn)
	{
		PushConn->SendPdu(&pdu);
		//log("sending at message to push server for %d users ..................", msg5.user_token_list_size());
	}

}
//void CDBServConn::getProduct(string& strProduct, uint32_t& unProduct)
//{
//	// //用户订购产品(16位中每一位代表一种产品。1表示开通该产品；0则表示未开通。就目前开发的产品来看从低位到高位的每一位分别代表IM、审批、邮箱、公告、云盘和通讯录)
//	unProduct = 0;
//	if (strProduct.find("IM") != string::npos)
//	{
//		unProduct = 0x01;
//	}
//
//	if (strProduct.find("WORKFLOW") != string::npos)
//	{
//		unProduct |= 0x02;
//	}
//
//	if (strProduct.find("MAIL") != string::npos)
//	{
//		unProduct |= 0x04;
//	}
//
//	if (strProduct.find("BULLETIN") != string::npos)
//	{
//		unProduct |= 0x08;
//	}
//
//	if (strProduct.find("CLOUDDISK") != string::npos)
//	{
//		unProduct |= 0x10;
//	}
//
//	if (strProduct.find("CONTACTS") != string::npos)
//	{
//		unProduct |= 0x30;
//	}
//}


void CDBServConn::_HandleAllStaffResponse(CImPdu* pPdu)
{
	IM::Buddy::IMAllStaffRsp msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

	uint32_t user_id = msg.user_id();
	uint32_t latest_update_time = msg.latest_update_time();
	uint32_t staff_cnt = msg.staff_info_list_size();
	uint32_t unCompanyId = msg.company_id();

	CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
	uint32_t handle = attach_data.GetHandle();

	log("HandleAllStaffResponse, userId=%u, companyid=%u, latest_update_time=%u, staff_cnt=%u", user_id, unCompanyId, latest_update_time, staff_cnt);

	CMsgConn* pMsgConn = CImUserManager::GetInstance()->GetMsgConnByHandle(user_id, handle);

	if (pMsgConn && pMsgConn->IsOpen())
	{
		msg.clear_attach_data();
		pPdu->SetPBMsg(&msg);
		int nBytes = pMsgConn->SendPdu(pPdu);
		log("send %d bytes to the client", nBytes);
	}
}


void CDBServConn::_HandleAllStaffLiteResponse(CImPdu* pPdu)
{
	IM::Buddy::IMAllStaffLiteRsp msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

	uint32_t user_id = msg.user_id();
	uint32_t latest_update_time = msg.latest_update_time();
	uint32_t staff_cnt = msg.staff_info_list_size();
	uint32_t unCompanyId = msg.company_id();

	CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
	uint32_t handle = attach_data.GetHandle();

	log("HandleAllStaffLiteResponse, userId=%u, companyid=%u, latest_update_time=%u, staff_cnt=%u", user_id, unCompanyId, latest_update_time, staff_cnt);

	CMsgConn* pMsgConn = CImUserManager::GetInstance()->GetMsgConnByHandle(user_id, handle);

	if (pMsgConn && pMsgConn->IsOpen())
	{
		msg.clear_attach_data();
		pPdu->SetPBMsg(&msg);
		int nBytes = pMsgConn->SendPdu(pPdu);
		log("send %d bytes to the client", nBytes);
	}
}


void CDBServConn::_HandleStaffInfoResponse(CImPdu* pPdu)
{
	IM::Buddy::IMStaffInfoRsp msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

	uint32_t user_id = msg.user_id();
	uint32_t staff_cnt = msg.user_info_list_size();
	CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
	uint32_t handle = attach_data.GetHandle();

	log("HandleStaffInfoResponse, userId=%u, staff_cnt=%u", user_id, staff_cnt);

	CMsgConn* pMsgConn = CImUserManager::GetInstance()->GetMsgConnByHandle(user_id, handle);

	if (pMsgConn && pMsgConn->IsOpen())
	{
		msg.clear_attach_data();
		pPdu->SetPBMsg(&msg);
		int nBytes = pMsgConn->SendPdu(pPdu);
		log("send %d bytes to the client", nBytes);
	}
}

void CDBServConn::_HandleUpdateStaffResponse(CImPdu* pPdu)
{
	IM::Buddy::IMUpdateStaffRsp msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

	uint32_t user_id = msg.user_id();
	uint32_t latest_update_time = msg.latest_update_time();
	CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
	uint32_t handle = attach_data.GetHandle();

	log("HandleUpdateStaffResponse, userId=%u, latest_update_time=%u,", user_id, latest_update_time);

	CMsgConn* pMsgConn = CImUserManager::GetInstance()->GetMsgConnByHandle(user_id, handle);

	if (pMsgConn && pMsgConn->IsOpen())
	{
		msg.clear_attach_data();
		pPdu->SetPBMsg(&msg);
		int nBytes = pMsgConn->SendPdu(pPdu);
		log("send %d bytes to the client", nBytes);
	}

	if (latest_update_time) //updated staff information successfully
	{
		IM::Buddy::IMUpdateStaffNotify msgNotify;
		msgNotify.set_user_id(msg.user_id());
		msgNotify.set_staff_uuid(msg.staff_uuid());

		int nCount = msg.staff_details_list_size();
		for (int i = 0; i < nCount; i++)
		{
			IM::BaseDefine::UserDetails *pUserDetails = msgNotify.add_staff_details_list();
			IM::BaseDefine::UserDetails  Details = msg.staff_details_list(i);
			pUserDetails->set_details_uuid(Details.details_uuid());
			pUserDetails->set_details_type(Details.details_type());
			pUserDetails->set_details_title(Details.details_title());
			pUserDetails->set_details_content(Details.details_content());
		}

		CImPdu pdu;
		pdu.SetPBMsg(&msgNotify);
		pdu.SetServiceId(SID_BUDDY_LIST);
		pdu.SetCommandId(CID_BUDDY_LIST_STAFF_UPDATE_INFO_NOTIFY);

		/*	CRouteServConn* pRouteConn = get_route_serv_conn();
		if (pRouteConn) {
		pRouteConn->SendPdu(&pdu);
		}*/

		CImUserManager::GetInstance()->BroadcastPdu(&pdu, CLIENT_TYPE_FLAG_BOTH, pMsgConn);
	}
}


void CDBServConn::_HandleClientChangeVipResponse(CImPdu* pPdu)
{
	IM::Buddy::IMChangeVipResponse msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

	uint32_t user_id = msg.user_id();
	CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
	uint32_t handle = attach_data.GetHandle();

	log("HandleChangeVipResponse, userId=%u", user_id);

	CMsgConn* pMsgConn = CImUserManager::GetInstance()->GetMsgConnByHandle(user_id, handle);

	if (pMsgConn && pMsgConn->IsOpen())
	{
		msg.clear_attach_data();
		pPdu->SetPBMsg(&msg);
		int nBytes = pMsgConn->SendPdu(pPdu);
		log("send %d bytes to the client", nBytes);
	}

	if (msg.latest_update_time()) //change vip successfully
	{
		IM::Buddy::IMChangeVipNotify msgNotify;
		msgNotify.set_user_id(msg.user_id());
		msgNotify.set_vip_id(msg.vip_id());
		msgNotify.set_operation_type((IM::BaseDefine::VipModifyType)msg.operation_type());

		CImPdu pdu;
		pdu.SetPBMsg(&msgNotify);
		pdu.SetServiceId(SID_BUDDY_LIST);
		pdu.SetCommandId(CID_BUDDY_LIST_CHANGE_VIP_NOTIFY);

		/*	CRouteServConn* pRouteConn = get_route_serv_conn();
		if (pRouteConn) {
		pRouteConn->SendPdu(&pdu);
		}*/

		//CImUserManager::GetInstance()->BroadcastPdu(&pdu, CLIENT_TYPE_FLAG_BOTH, pMsgConn);
		CImUser* pUser = CImUserManager::GetInstance()->GetImUserById(msg.user_id());
		if (pUser)
		{
			pUser->BroadcastPdu(&pdu, pMsgConn);
		}
	}
}


void CDBServConn::_HandleClientAsstistantInfoResponse(CImPdu* pPdu)
{
	IM::Buddy::IMAssistantInfoRsp msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

	uint32_t user_id = msg.user_id();
	uint32_t asst_cnt = msg.asst_info_list_size();
	CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
	uint32_t handle = attach_data.GetHandle();

	log("HandleAsstInfoResponse, userId=%u, asst_cnt=%u", user_id, asst_cnt);

	CMsgConn* pMsgConn = CImUserManager::GetInstance()->GetMsgConnByHandle(user_id, handle);

	if (pMsgConn && pMsgConn->IsOpen())
	{
		msg.clear_attach_data();
		pPdu->SetPBMsg(&msg);
		int nBytes = pMsgConn->SendPdu(pPdu);
		log("send %d bytes to the client", nBytes);
	}
}


void CDBServConn::_HandleClientQueryUserInfoResponse(CImPdu* pPdu)
{
	IM::Buddy::IMQueryUserInfoRsp msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

	uint32_t user_id = msg.user_id();;
	CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
	uint32_t handle = attach_data.GetHandle();

	log("HandleQueryUserInfoResp, user_id=%u", user_id);

	CMsgConn* pMsgConn = CImUserManager::GetInstance()->GetMsgConnByHandle(user_id, handle);
	if (pMsgConn && pMsgConn->IsOpen())
	{
		msg.clear_attach_data();
		pPdu->SetPBMsg(&msg);
		pMsgConn->SendPdu(pPdu);
	}
}