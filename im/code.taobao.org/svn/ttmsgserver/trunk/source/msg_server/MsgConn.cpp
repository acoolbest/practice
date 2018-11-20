/*
 * MsgConn.cpp
 *
 *  Created on: 2013-7-5
 *      Author: ziteng@mogujie.com
 */

#include "MsgConn.h"
#include "DBServConn.h"
#include "LoginServConn.h"
#include "RouteServConn.h"
#include "FileHandler.h"
#include "GroupChat.h"
#include "ImUser.h"
#include "AttachData.h"
#include "IM.Buddy.pb.h"
#include "IM.Message.pb.h"
#include "IM.Login.pb.h"
#include "IM.Other.pb.h"
#include "IM.Group.pb.h"
#include "IM.Server.pb.h"
#include "IM.SwitchService.pb.h"
#include "public_define.h"
#include "ImPduBase.h"
using namespace IM::BaseDefine;

#define TIMEOUT_WATI_LOGIN_RESPONSE		15000	// 15 seconds
#define TIMEOUT_WAITING_MSG_DATA_ACK	15000	// 15 seconds
#define TIMEOUT_WAITING_RESEND_MSG_DATA	90000	// 1.5 minutes		
#define LOG_MSG_STAT_INTERVAL			300000	// log message miss status in every 5 minutes;
#define MAX_MSG_CNT_PER_SECOND			20	// user can not send more than 20 msg in one second
#define MAX_MSG_DATA_LENTH				10000 //maximum message data length
static ConnMap_t g_msg_conn_map;
static UserMap_t g_msg_conn_user_map;

static uint64_t	g_last_stat_tick;	// 上次显示丢包率信息的时间
static uint32_t g_up_msg_total_cnt = 0;		// 上行消息包总数
static uint32_t g_up_msg_miss_cnt = 0;		// 上行消息包丢数
static uint32_t g_down_msg_total_cnt = 0;	// 下行消息包总数
static uint32_t g_down_msg_miss_cnt = 0;	// 下行消息丢包数

static bool g_log_msg_toggle = true;	// 是否把收到的MsgData写入Log的开关，通过kill -SIGUSR2 pid 打开/关闭

//static CFileHandler* s_file_handler = NULL;
static CGroupChat* s_group_chat = NULL;

void msg_conn_timer_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	ConnMap_t::iterator it_old;
	CMsgConn* pConn = NULL;
	uint64_t cur_time = get_tick_count();

	for (ConnMap_t::iterator it = g_msg_conn_map.begin(); it != g_msg_conn_map.end(); ) {
		it_old = it;
		it++;

		pConn = (CMsgConn*)it_old->second;
		pConn->OnTimer(cur_time);
	}

	if (cur_time > g_last_stat_tick + LOG_MSG_STAT_INTERVAL) {
		g_last_stat_tick = cur_time;
		log("up_msg_cnt=%u, up_msg_miss_cnt=%u, down_msg_cnt=%u, down_msg_miss_cnt=%u ",
			g_up_msg_total_cnt, g_up_msg_miss_cnt, g_down_msg_total_cnt, g_down_msg_miss_cnt);
	}
}

static void signal_handler_usr1(int sig_no)
{
	if (sig_no == SIGUSR1) {
		log("receive SIGUSR1 ");
		g_up_msg_total_cnt = 0;
		g_up_msg_miss_cnt = 0;
		g_down_msg_total_cnt = 0;
		g_down_msg_miss_cnt = 0;
	}
}

static void signal_handler_usr2(int sig_no)
{
	if (sig_no == SIGUSR2) {
		log("receive SIGUSR2 ");
		g_log_msg_toggle = !g_log_msg_toggle;
	}
}

static void signal_handler_hup(int sig_no)
{
	if (sig_no == SIGHUP) {
		log("receive SIGHUP exit... ");
		exit(0);
	}
}

void init_msg_conn()
{
	g_last_stat_tick = get_tick_count();
	signal(SIGUSR1, signal_handler_usr1);
	signal(SIGUSR2, signal_handler_usr2);
	signal(SIGHUP, signal_handler_hup);
	netlib_register_timer(msg_conn_timer_callback, NULL, 1000);
	//s_file_handler = CFileHandler::getInstance();
	s_group_chat = CGroupChat::GetInstance();
}

////////////////////////////
CMsgConn::CMsgConn()
{
    m_user_id = 0;
    m_bOpen = false;
    m_bKickOff = false;
    m_last_seq_no = 0;
    m_msg_cnt_per_sec = 0;
    m_send_msg_list.clear();
    m_online_status = IM::BaseDefine::USER_STATUS_OFFLINE;
	m_bSent = true;
	m_recv_msg_list.clear();
}

CMsgConn::~CMsgConn()
{
	m_send_msg_list.clear();
	m_recv_msg_list.clear();
}

void CMsgConn::SendUserStatusUpdate(uint32_t user_status)
{
    if (!m_bOpen) {
		return;
	}
    
    CImUser* pImUser = CImUserManager::GetInstance()->GetImUserById(GetUserId());
    if (!pImUser) {
        return;
    }
    
    // 只有上下线通知才通知LoginServer
    if (user_status == ::IM::BaseDefine::USER_STATUS_ONLINE) {
        IM::Server::IMUserCntUpdate msg;
        msg.set_user_action(USER_CNT_INC);
        msg.set_user_id(pImUser->GetUserId());
        CImPdu pdu;
        pdu.SetPBMsg(&msg);
        pdu.SetServiceId(SID_OTHER);
        pdu.SetCommandId(CID_OTHER_USER_CNT_UPDATE);
        send_to_all_login_server(&pdu);
        
        IM::Server::IMUserStatusUpdate msg2;
        msg2.set_user_status(::IM::BaseDefine::USER_STATUS_ONLINE);
        msg2.set_user_id(pImUser->GetUserId());
        msg2.set_client_type((::IM::BaseDefine::ClientType)m_client_type);
        CImPdu pdu2;
        pdu2.SetPBMsg(&msg2);
        pdu2.SetServiceId(SID_OTHER);
        pdu2.SetCommandId(CID_OTHER_USER_STATUS_UPDATE);
        
        send_to_all_route_server(&pdu2);
    } else if (user_status == ::IM::BaseDefine::USER_STATUS_OFFLINE) {
        IM::Server::IMUserCntUpdate msg;
        msg.set_user_action(USER_CNT_DEC);
        msg.set_user_id(pImUser->GetUserId());
        CImPdu pdu;
        pdu.SetPBMsg(&msg);
        pdu.SetServiceId(SID_OTHER);
        pdu.SetCommandId(CID_OTHER_USER_CNT_UPDATE);
        send_to_all_login_server(&pdu);
        
        IM::Server::IMUserStatusUpdate msg2;
        msg2.set_user_status(::IM::BaseDefine::USER_STATUS_OFFLINE);
        msg2.set_user_id(pImUser->GetUserId());
        msg2.set_client_type((::IM::BaseDefine::ClientType)m_client_type);
        CImPdu pdu2;
        pdu2.SetPBMsg(&msg2);
        pdu2.SetServiceId(SID_OTHER);
        pdu2.SetCommandId(CID_OTHER_USER_STATUS_UPDATE);
        send_to_all_route_server(&pdu2);
    }
}

void CMsgConn::Close(bool kick_user)
{
   /* log("Close client, handle=%d, user_id=%u ", m_handle, GetUserId());
    if (m_handle != NETLIB_INVALID_HANDLE) {
        netlib_close(m_handle);
        g_msg_conn_map.erase(m_handle);
    }*/
    
	log("Delete connection for user: %d", m_user_id);
    CImUser *pImUser = CImUserManager::GetInstance()->GetImUserById(GetUserId());
    if (pImUser)
	{
        pImUser->DelMsgConn(GetHandle());
        pImUser->DelUnValidateMsgConn(this);
        
        SendUserStatusUpdate(::IM::BaseDefine::USER_STATUS_OFFLINE);
        if (pImUser->IsMsgConnEmpty()) {
            CImUserManager::GetInstance()->RemoveImUser(pImUser);
        }
    }
    
    pImUser = CImUserManager::GetInstance()->GetImUserByLoginName(GetLoginName());
    if (pImUser) {
        pImUser->DelUnValidateMsgConn(this);
        if (pImUser->IsMsgConnEmpty()) {
            CImUserManager::GetInstance()->RemoveImUser(pImUser);
        }
    }

	//reset user id
	m_user_id = 0;

	log("Close client, handle=%d, user_id=%u ", m_handle, GetUserId());
	if (m_handle != NETLIB_INVALID_HANDLE)
	{
		netlib_close(m_handle);
		g_msg_conn_map.erase(m_handle);
	}

	/*log("ready to reset token, user_id=%d, client_type=%u. ", GetUserId(), GetClientType());
	if (CHECK_CLIENT_TYPE_MOBILE(GetClientType()))
	{
		CDBServConn* pDBConn = get_db_serv_conn();
		if (pDBConn)
		{
			IM::Login::IMDeviceTokenReq msg;
			msg.set_user_id(GetUserId());
			msg.set_device_token("");
			CImPdu pdu;
			pdu.SetPBMsg(&msg);
			pdu.SetServiceId(SID_LOGIN);
			pdu.SetCommandId(CID_LOGIN_REQ_DEVICETOKEN);
			pdu.SetSeqNum(1);
			pDBConn->SendPdu(&pdu);
			log("force reset token request...............");
		}
	}*/
    
    ReleaseRef();
}

void CMsgConn::OnConnect(net_handle_t handle)
{
	m_handle = handle;
	m_login_time = get_tick_count();

	g_msg_conn_map.insert(make_pair(handle, this));

	netlib_option(handle, NETLIB_OPT_SET_CALLBACK, (void*)imconn_callback);
	netlib_option(handle, NETLIB_OPT_SET_CALLBACK_DATA, (void*)&g_msg_conn_map);
	netlib_option(handle, NETLIB_OPT_GET_REMOTE_IP, (void*)&m_peer_ip);
	netlib_option(handle, NETLIB_OPT_GET_REMOTE_PORT, (void*)&m_peer_port);
}

void CMsgConn::OnClose()
{
    log("Warning: peer closed. ");
	Close();
}

void CMsgConn::OnTimer(uint64_t curr_tick)
{
	m_msg_cnt_per_sec = 0;

    if (CHECK_CLIENT_TYPE_MOBILE(GetClientType()))
    {
        if (curr_tick > m_last_recv_tick + MOBILE_CLIENT_TIMEOUT) {
            log("mobile client timeout, handle=%d, uid=%u ", m_handle, GetUserId());
            Close();
            return;
        }
    }
    else
    {
        if (curr_tick > m_last_recv_tick + CLIENT_TIMEOUT) {
            log("client timeout, handle=%d, uid=%u ", m_handle, GetUserId());
            Close();
            return;
        }
    }
    

	if (!IsOpen()) {
		if (curr_tick > m_login_time + TIMEOUT_WATI_LOGIN_RESPONSE) {
			log("login timeout, handle=%d, uid=%u ", m_handle, GetUserId());
			Close();
			return;
		}
	}

	list<msg_ack_t>::iterator it = m_send_msg_list.begin();
	if(it != m_send_msg_list.end())
	{
		if (curr_tick >= it->timestamp + TIMEOUT_WAITING_MSG_DATA_ACK)
		{
			log("send time: %lu, current time: %lu, !!!a msg missed, msg_id=%u, %u->%u,resend ", it->timestamp, curr_tick, it->msg_id, it->from_id, GetUserId());
			g_down_msg_miss_cnt++;
			//m_send_msg_list.erase(it_old);
			//msg.timestamp = get_tick_count();//reset message sent time
			SetSentStatus(true);
			log("time out and resend a message");
		}
		ResendMessageFromSendList(it);
		//ResendMessageFromSendList();
	}

	DelFromRecvList(curr_tick);  //added by george on Jan 6th,2015

	//george commented and changed the method to the above. Dec 31st,2015
	/*list<msg_ack_t>::iterator it_old;
	for (list<msg_ack_t>::iterator it = m_send_msg_list.begin(); it != m_send_msg_list.end(); )
	{
		msg_ack_t msg = *it;
		it_old = it;
		it++;
		if (curr_tick >= msg.timestamp + TIMEOUT_WAITING_MSG_DATA_ACK)
		{
			log("!!!a msg missed, msg_id=%u, %u->%u,resend ", msg.msg_id, msg.from_id, GetUserId());
			g_down_msg_miss_cnt++;
			m_send_msg_list.erase(it_old);
		}
		else 
		{
			break;
		}
	}*/
}

void CMsgConn::HandlePdu(CImPdu* pPdu)
{
	// request authorization check
	if (pPdu->GetCommandId() != CID_LOGIN_REQ_USERLOGIN && !IsOpen() && IsKickOff()) {
        log("HandlePdu, wrong msg. ");
        throw CPduException(pPdu->GetServiceId(), pPdu->GetCommandId(), ERROR_CODE_WRONG_SERVICE_ID, "HandlePdu error, user not login. ");
		return;
    }
    
	switch (pPdu->GetCommandId()) {
        case CID_OTHER_HEARTBEAT:
            _HandleHeartBeat(pPdu);
			//log("Heartbeat from client: %s..........................", CHECK_CLIENT_TYPE_MOBILE(GetClientType()) ? "Mobile" : "PC");
            break;
        case CID_LOGIN_REQ_USERLOGIN:
            _HandleLoginRequest(pPdu );
            break;
        case CID_LOGIN_REQ_LOGINOUT:
            _HandleLoginOutRequest(pPdu);
            break;
        case CID_LOGIN_REQ_DEVICETOKEN:
            _HandleClientDeviceToken(pPdu);
            break;
        case CID_LOGIN_REQ_KICKPCCLIENT:
            _HandleKickPCClient(pPdu);
            break;
        case CID_MSG_DATA:
            _HandleClientMsgData(pPdu);
            break;
        case CID_MSG_DATA_ACK:
            _HandleClientMsgDataAck(pPdu);
            break;
        case CID_MSG_TIME_REQUEST:
            _HandleClientTimeRequest(pPdu);
            break;
        case CID_MSG_LIST_REQUEST:
            _HandleClientGetMsgListRequest(pPdu);
            break;
        case CID_MSG_GET_BY_MSG_ID_REQ:
            _HandleClientGetMsgByMsgIdRequest(pPdu);
            break;
        case CID_MSG_UNREAD_CNT_REQUEST:
            _HandleClientUnreadMsgCntRequest(pPdu );
            break;
        case CID_MSG_READ_ACK:
            _HandleClientMsgReadAck(pPdu);
            break;

        case CID_MSG_GET_LATEST_MSG_ID_REQ:
            _HandleClientGetLatestMsgIDReq(pPdu);
            break;
        case CID_SWITCH_P2P_CMD:
            _HandleClientP2PCmdMsg(pPdu );
            break;
        case CID_BUDDY_LIST_RECENT_CONTACT_SESSION_REQUEST:
            _HandleClientRecentContactSessionRequest(pPdu);
            break;
        case CID_BUDDY_LIST_USER_INFO_REQUEST:
            _HandleClientUserInfoRequest( pPdu );
            break;
        case CID_BUDDY_LIST_REMOVE_SESSION_REQ:
            _HandleClientRemoveSessionRequest( pPdu );
            break;
        case CID_BUDDY_LIST_ALL_USER_REQUEST:
            _HandleClientAllUserRequest(pPdu );
            break;
		case CID_BUDDY_LIST_USER_UPDATE_NAGURE_REQUEST:
			_HandleClientUpdateUserNagure(pPdu);
			break;

		case CID_BUDDY_LIST_USER_UPDATE_INFO_REQUEST:
			_HandleClientUpdateUser(pPdu);
			break;

        case CID_BUDDY_LIST_CHANGE_AVATAR_REQUEST:
            _HandleChangeAvatarRequest(pPdu);
            break;
            
        case CID_BUDDY_LIST_USERS_STATUS_REQUEST:
            _HandleClientUsersStatusRequest(pPdu);
            break;
        case CID_BUDDY_LIST_DEPARTMENT_REQUEST:
            _HandleClientDepartmentRequest(pPdu);
			break;
		case CID_BUDDY_LIST_COMPANY_REQUEST:
			_HandleClientCompanyRequest(pPdu);
            break;
		case CID_BUDDY_LIST_ONLINE_USER_REQUEST:
			_HandleOnlineUsersRequest(pPdu);
			break;
		case CID_BUDDY_SHIELD_USER_REQUEST:
			_HandleClientShieldUserRequest(pPdu); //insrted by george jiang on Nov 17th,2015
			break;

		case CID_BUDDY_LIST_POSITION_REQUEST:
			_HandleClientPositionRequest(pPdu);
			break;

	/*	case CID_BUDDY_LIST_ALL_USER_LITE_REQUEST:
			_HandleClientAllUserLiteRequest(pPdu);
			break;

		case CID_BUDDY_LIST_USER_INFO_LITE_REQUEST:
			_HandleClientUserInfoLiteRequest(pPdu);
			break;*/

        // for group process
        case CID_GROUP_NORMAL_LIST_REQUEST:
            s_group_chat->HandleClientGroupNormalRequest(pPdu, this);
            break;
        case CID_GROUP_INFO_REQUEST:
            s_group_chat->HandleClientGroupInfoRequest(pPdu, this);
            break;
        case CID_GROUP_CREATE_REQUEST:
            s_group_chat->HandleClientGroupCreateRequest(pPdu, this);
            break;
        case CID_GROUP_CHANGE_MEMBER_REQUEST:
            s_group_chat->HandleClientGroupChangeMemberRequest(pPdu, this);
            break;
        case CID_GROUP_SHIELD_GROUP_REQUEST:
            s_group_chat->HandleClientGroupShieldGroupRequest(pPdu, this);
            break;
		case CID_GROUP_CHANGE_GRPNAME_REQUEST:
			s_group_chat->HandleClientGroupChangeNameRequest(pPdu,this);
			break;
		case CID_GROUP_TRANSFER_LEADER_REQUEST:
			s_group_chat->HandleClientGroupTransferLeaderRequest(pPdu,this);
			break;
            
        /*case CID_FILE_REQUEST:
            s_file_handler->HandleClientFileRequest(this, pPdu);
            break;
        case CID_FILE_HAS_OFFLINE_REQ:
            s_file_handler->HandleClientFileHasOfflineReq(this, pPdu);
            break;
        case CID_FILE_ADD_OFFLINE_REQ:
            s_file_handler->HandleClientFileAddOfflineReq(this, pPdu);
            break;
        case CID_FILE_DEL_OFFLINE_REQ:
            s_file_handler->HandleClientFileDelOfflineReq(this, pPdu);
            break;*/
		case CID_BUDDY_LIST_ALL_STAFF_REQUEST:
			_HandleClientAllStaffRequest(pPdu);
			break;

		case CID_BUDDY_LIST_ALL_STAFF_LITE_REQUEST:
			_HandleClientAllStaffLiteRequest(pPdu);
			break;

		case CID_BUDDY_LIST_STAFF_INFO_REQUEST:
			_HandleClientStaffInfoRequest(pPdu);
			break;

		case CID_BUDDY_LIST_STAFF_UPDATE_INFO_REQUEST:
			_HandleClientUpdateStaffRequest(pPdu);
			break;

		case CID_BUDDY_LIST_CHANGE_VIP_REQUEST:
			_HandleClientChangeVipRequest(pPdu);
			break;

		case CID_BUDDY_LIST_ASST_INFO_REQUEST:
			_HandleClientAssistantInfoRequest(pPdu);
			break;

		case CID_BUDDY_LIST_QUERY_USER_INFO_REQUEST:
			_HandleClientQueryUserInfoRequest(pPdu);
			break;

        default:
            log("wrong msg, cmd id=%d, user id=%u. ", pPdu->GetCommandId(), GetUserId());
            break;
	}
}

void CMsgConn::_HandleHeartBeat(CImPdu *pPdu)
{
    //响应
    SendPdu(pPdu);
	//log("sending heartbeat response to client......................");
}

// process: send validate request to db server
void CMsgConn::_HandleLoginRequest(CImPdu* pPdu)
{
    // refuse second validate request
    if (m_login_name.length() != 0) {
        log("duplicate LoginRequest in the same conn ");
        return;
    }
    
    // check if all server connection are OK
    uint32_t result = 0;
    string result_string = "";
    CDBServConn* pDbConn = get_db_serv_conn_for_login();
    if (!pDbConn) {
        result = IM::BaseDefine::REFUSE_REASON_NO_DB_SERVER;
		result_string = "登录错误，请联系管理员处理";
		log("no db server(%d)", REFUSE_REASON_NO_DB_SERVER);
		//result_string = "服务端异常(" + int2string(REFUSE_REASON_NO_DB_SERVER) + ")";
    }
    else if (!is_login_server_available()) {
        result = IM::BaseDefine::REFUSE_REASON_NO_LOGIN_SERVER;
		result_string = "登录错误，请联系管理员处理";
		log("no login server(%d)", REFUSE_REASON_NO_LOGIN_SERVER);
    }
    else if (!is_route_server_available()) {
        result = IM::BaseDefine::REFUSE_REASON_NO_ROUTE_SERVER;
		result_string = "登录错误，请联系管理员处理";
		log("no route server(%d)", REFUSE_REASON_NO_ROUTE_SERVER);
    }
	

	IM::Login::IMLoginReq msg;
	string password = "";
	uint32_t unActivatedStatus = 0;
	if (!result)
	{
		m_bKickOff = false; //george added on Jan 12th,2016

		CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
		//假如是汉字，则转成拼音
		m_login_name = msg.user_name();
		password = msg.password();
		unActivatedStatus = msg.is_activated();

		uint32_t online_status = msg.online_status();
		if (online_status < IM::BaseDefine::USER_STATUS_ONLINE || online_status > IM::BaseDefine::USER_STATUS_LEAVE) {
			log("HandleLoginReq, online status wrong: %u ", online_status);
			online_status = IM::BaseDefine::USER_STATUS_ONLINE;
		}

		m_client_version = msg.client_version();
		m_client_type = msg.client_type();

		if (!IsValidClientVersion())
		{
			result = IM::BaseDefine::REFUSE_REASON_LOWER_CLIENT_VERSION;
			result_string = "对不起，客户端版本号太低,请下载最新版本！";
		}

		m_online_status = online_status;
		log("HandleLoginReq, user_name=%s, status=%u, client_type=%u, client=%s, ",
			m_login_name.c_str(), online_status, m_client_type, m_client_version.c_str());
	}


    if (result) {
        IM::Login::IMLoginRes msg;
        msg.set_server_time(time(NULL));
        msg.set_result_code((IM::BaseDefine::ResultType)result);
        msg.set_result_string(result_string);
        CImPdu pdu;
        pdu.SetPBMsg(&msg);
        pdu.SetServiceId(SID_LOGIN);
        pdu.SetCommandId(CID_LOGIN_RES_USERLOGIN);
        pdu.SetSeqNum(pPdu->GetSeqNum());
        SendPdu(&pdu);
        Close();
        return;
    }
    
	//m_bKickOff = false; //george added on Jan 12th,2016

 //   IM::Login::IMLoginReq msg;
 //   CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
 //   //假如是汉字，则转成拼音
 //   m_login_name = msg.user_name();
 //   string password = msg.password();
 //   uint32_t online_status = msg.online_status();
 //   if (online_status < IM::BaseDefine::USER_STATUS_ONLINE || online_status > IM::BaseDefine::USER_STATUS_LEAVE) {
 //       log("HandleLoginReq, online status wrong: %u ", online_status);
 //       online_status = IM::BaseDefine::USER_STATUS_ONLINE;
 //   }
 //   m_client_version = msg.client_version();
 //   m_client_type = msg.client_type();
 //   m_online_status = online_status;
 //   log("HandleLoginReq, user_name=%s, status=%u, client_type=%u, client=%s, ",
 //       m_login_name.c_str(), online_status, m_client_type, m_client_version.c_str());
    
    CImUser* pImUser = CImUserManager::GetInstance()->GetImUserByLoginName(GetLoginName());
    if (!pImUser) {
        pImUser = new CImUser(GetLoginName());
        CImUserManager::GetInstance()->AddImUserByLoginName(GetLoginName(), pImUser);
    }
    pImUser->AddUnValidateMsgConn(this);
    
    CDbAttachData attach_data(ATTACH_TYPE_HANDLE, m_handle, 0);
    // continue to validate if the user is OK
    
    IM::Server::IMValidateReq msg2;
    msg2.set_user_name(msg.user_name());
    msg2.set_password(password);
	msg2.set_client_ip(m_peer_ip);
	msg2.set_is_activated(unActivatedStatus);
    msg2.set_attach_data(attach_data.GetBuffer(), attach_data.GetLength());
    CImPdu pdu;
    pdu.SetPBMsg(&msg2);
    pdu.SetServiceId(SID_OTHER);
    pdu.SetCommandId(CID_OTHER_VALIDATE_REQ);
    pdu.SetSeqNum(pPdu->GetSeqNum());
    pDbConn->SendPdu(&pdu);
}

void CMsgConn::_HandleLoginOutRequest(CImPdu *pPdu)
{
    log("HandleLoginOutRequest, user_id=%d, client_type=%u. ", GetUserId(), GetClientType());
    CDBServConn* pDBConn = get_db_serv_conn();
	if (pDBConn) {
        IM::Login::IMDeviceTokenReq msg;
        msg.set_user_id(GetUserId());
        msg.set_device_token("");
        CImPdu pdu;
        pdu.SetPBMsg(&msg);
        pdu.SetServiceId(SID_LOGIN);
        pdu.SetCommandId(CID_LOGIN_REQ_DEVICETOKEN);
        pdu.SetSeqNum(pPdu->GetSeqNum());
		pDBConn->SendPdu(&pdu);
	}
    
    IM::Login::IMLogoutRsp msg2;
    msg2.set_result_code(0);
    CImPdu pdu2;
    pdu2.SetPBMsg(&msg2);
    pdu2.SetServiceId(SID_LOGIN);
    pdu2.SetCommandId(CID_LOGIN_RES_LOGINOUT);
    pdu2.SetSeqNum(pPdu->GetSeqNum());
    SendPdu(&pdu2);
    Close();
}

void CMsgConn::_HandleKickPCClient(CImPdu *pPdu)
{
    IM::Login::IMKickPCClientReq msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
    uint32_t user_id = GetUserId();
    if (!CHECK_CLIENT_TYPE_MOBILE(GetClientType()))
    {
        log("HandleKickPCClient, user_id = %u, cmd must come from mobile client. ", user_id);
        return;
    }
    log("HandleKickPCClient, user_id = %u. ", user_id);
    
    CImUser* pImUser = CImUserManager::GetInstance()->GetImUserById(user_id);
    if (pImUser)
    {
        pImUser->KickOutSameClientType(CLIENT_TYPE_MAC, IM::BaseDefine::KICK_REASON_MOBILE_KICK,this);
    }
    
    CRouteServConn* pRouteConn = get_route_serv_conn();
    if (pRouteConn) {
        IM::Server::IMServerKickUser msg2;
        msg2.set_user_id(user_id);
        msg2.set_client_type(::IM::BaseDefine::CLIENT_TYPE_MAC);
        msg2.set_reason(IM::BaseDefine::KICK_REASON_MOBILE_KICK);
        CImPdu pdu;
        pdu.SetPBMsg(&msg2);
        pdu.SetServiceId(SID_OTHER);
        pdu.SetCommandId(CID_OTHER_SERVER_KICK_USER);
        pRouteConn->SendPdu(&pdu);
    }
    
    IM::Login::IMKickPCClientRsp msg2;
    msg2.set_user_id(user_id);
    msg2.set_result_code(0);
    CImPdu pdu;
    pdu.SetPBMsg(&msg2);
    pdu.SetServiceId(SID_LOGIN);
    pdu.SetCommandId(CID_LOGIN_RES_KICKPCCLIENT);
    pdu.SetSeqNum(pPdu->GetSeqNum());
    SendPdu(&pdu);
}

void CMsgConn::_HandleClientRecentContactSessionRequest(CImPdu *pPdu)
{
    CDBServConn* pConn = get_db_serv_conn_for_login();
    if (!pConn) {
        return;
    }
    
    IM::Buddy::IMRecentContactSessionReq msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
    log("HandleClientRecentContactSessionRequest, user_id=%u, latest_update_time=%u. ", GetUserId(), msg.latest_update_time());

    msg.set_user_id(GetUserId());
    // 请求最近联系会话列表
    CDbAttachData attach_data(ATTACH_TYPE_HANDLE, m_handle, 0);
    msg.set_attach_data(attach_data.GetBuffer(), attach_data.GetLength());
    pPdu->SetPBMsg(&msg);
    pConn->SendPdu(pPdu);
}

void CMsgConn::_HandleClientMsgData(CImPdu* pPdu)
{
    IM::Message::IMMsgData msg;
	
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
	/*if (msg.msg_data().length() == 0) {
		log("discard an empty message, uid=%u ", GetUserId());
		return;
	}

	if (m_msg_cnt_per_sec >= MAX_MSG_CNT_PER_SECOND) {
		log("!!!too much msg cnt in one second, uid=%u ", GetUserId());
		return;
	}
    
    if (msg.from_user_id() == msg.to_session_id() && CHECK_MSG_TYPE_SINGLE(msg.msg_type()))
    {
        log("!!!from_user_id == to_user_id. ");
        return;
    }*/
	//time_t nCurrTime = time(NULL);
	int nResult = 0;
	if (msg.msg_data().length() == 0)
	{
		nResult = ::IM::BaseDefine::MESSAGE_SEND_ERROR_NO_DATA;
		log("discard an empty message, uid=%u ", GetUserId());
	}
	/*else if (msg.msg_data().length() > MAX_MSG_DATA_LENTH && (msg.msg_type() != IM::BaseDefine::MSG_TYPE_GROUP_AUDIO && msg.msg_type() != IM::BaseDefine::MSG_TYPE_SINGLE_AUDIO))
	{
		nResult = ::IM::BaseDefine::MESSAGE_SEND_ERROR_TOO_MUCH_DATA;
		log("too much data, uid=%u ", GetUserId());
	}*/
	else if (m_msg_cnt_per_sec >= MAX_MSG_CNT_PER_SECOND)
	{
		log("!!!too much msg cnt in one second, uid=%u ", GetUserId());
		nResult = ::IM::BaseDefine::MESSAGE_SEND_ERROR_TOO_MUCH_MSG_CNT_PER_SECOND;
	}
	else if ((msg.from_user_id() <= 10000) || (msg.to_session_id() <= 10000 && CHECK_MSG_TYPE_SINGLE(msg.msg_type())) || msg.from_user_id() != GetUserId() || IsKickOff())  //user id should be greater than 10000. appended by george on Nov 12th,2015
	{
		log("user id should be greater than 10000, %d(passed) is an invalid user: %d(set)m to_session: %d", msg.from_user_id(),GetUserId(), msg.to_session_id());
		nResult = ::IM::BaseDefine::MESSAGE_SEND_ERROR_INVALID_USER;
	}
	else if (msg.from_user_id() == msg.to_session_id() && CHECK_MSG_TYPE_SINGLE(msg.msg_type()))
	{
		log("!!!from_user_id == to_user_id. ");
		nResult = ::IM::BaseDefine::MESSAGE_SEND_ERROR_SAME_USER;
	} 
	
	//else if (msg.create_time() == 0) //time deficit is more than 5 seconds 
	//{
	//	log("Current Time: %d, Created Time: %d", nCurrTime, msg.create_time());
	//	nResult = ::IM::BaseDefine::MESSAGE_SEND_ERROR_INVALID_TIMESTAMP;
	//}

	uint32_t to_session_id = msg.to_session_id();
	uint32_t msg_id = msg.msg_id();
	uint8_t msg_type = msg.msg_type();


	if (nResult)
	{
		IM::Message::IMMsgDataAck msg2;
		msg2.set_user_id(GetUserId());
		msg2.set_msg_id(msg_id);
		msg2.set_session_id(to_session_id);
		if (CHECK_MSG_TYPE_GROUP(msg_type))
		{
			msg2.set_session_type(::IM::BaseDefine::SESSION_TYPE_GROUP);
		}
		else
		{
			msg2.set_session_type(::IM::BaseDefine::SESSION_TYPE_SINGLE);
		}
		msg2.set_result_code(nResult);
		CImPdu pdu;
		pdu.SetPBMsg(&msg2);
		pdu.SetServiceId(SID_MSG);
		pdu.SetCommandId(CID_MSG_DATA_ACK);
		pdu.SetSeqNum(pPdu->GetSeqNum());
		SendPdu(&pdu);
		return;
	}

	//if (ResendMessageFromRecvList(pPdu->GetSeqNum())) //sequence found?
	//{
	//	log("message has already inserted into database,just resend MSG DATA ACK to the clien: %d", m_handle);
	//	return;
	//}

	uint32_t session_type = 0;
	if (CHECK_MSG_TYPE_GROUP(msg_type))
	{
		session_type = ::IM::BaseDefine::SESSION_TYPE_GROUP;
	}
	else
	{
		session_type = ::IM::BaseDefine::SESSION_TYPE_SINGLE;
	}

	//if (!AddToRecvList(msg_id, GetUserId(), to_session_id, pPdu->GetSeqNum(), session_type))
	if (!AddToRecvList(0, GetUserId(), to_session_id, pPdu->GetSeqNum(), session_type))
	{
		return;
	}

	m_msg_cnt_per_sec++;

	/*uint32_t to_session_id = msg.to_session_id();
    uint32_t msg_id = msg.msg_id();
	uint8_t msg_type = msg.msg_type();*/
    string msg_data = msg.msg_data();

	if (g_log_msg_toggle) {
		log("HandleClientMsgData, %d->%d, msg_type=%u, msg_id=%u. ", GetUserId(), to_session_id, msg_type, msg_id);
	}

	uint32_t cur_time = time(NULL); //set create time by msg server side.recomment by george on Dec 24th,2015
    CDbAttachData attach_data(ATTACH_TYPE_HANDLE, m_handle, 0);
    msg.set_from_user_id(GetUserId());
	msg.set_create_time(cur_time);
    msg.set_attach_data(attach_data.GetBuffer(), attach_data.GetLength());
    pPdu->SetPBMsg(&msg);
	// send to DB storage server
	CDBServConn* pDbConn = get_db_serv_conn();
	if (pDbConn) {
		pDbConn->SendPdu(pPdu);
	}
}

void CMsgConn::_HandleClientMsgDataAck(CImPdu* pPdu)
{
    IM::Message::IMMsgDataAck msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
    
    IM::BaseDefine::SessionType session_type = msg.session_type();
	uint32_t msg_id = msg.msg_id();
	uint32_t session_id = msg.session_id();
	uint32_t from_id = msg.user_id();
	log("Handle Client Msg Data ACK: from id: %u, session id: %u, msg id: %u", from_id,session_id, msg_id);
	DelFromSendList(msg_id, from_id, session_id, pPdu->GetSeqNum(),session_type);

   /* if (session_type == IM::BaseDefine::SESSION_TYPE_SINGLE)
    {
        uint32_t msg_id = msg.msg_id();
        uint32_t session_id = msg.session_id();
        DelFromSendList(msg_id, session_id);
    }*/
}

void CMsgConn::_HandleClientTimeRequest(CImPdu* pPdu)
{
    IM::Message::IMClientTimeRsp msg;
    msg.set_server_time((uint32_t)time(NULL));
    CImPdu pdu;
    pdu.SetPBMsg(&msg);
    pdu.SetServiceId(SID_MSG);
    pdu.SetCommandId(CID_MSG_TIME_RESPONSE);
    pdu.SetSeqNum(pPdu->GetSeqNum());
	SendPdu(&pdu);
}

void CMsgConn::_HandleClientGetMsgListRequest(CImPdu *pPdu)
{
    IM::Message::IMGetMsgListReq msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
    uint32_t session_id = msg.session_id();
    uint32_t msg_id_begin = msg.msg_id_begin();
    uint32_t msg_cnt = msg.msg_cnt();
    uint32_t session_type = msg.session_type();
    log("HandleClientGetMsgListRequest, req_id=%u, session_type=%u, session_id=%u, msg_id_begin=%u, msg_cnt=%u. ",
        GetUserId(), session_type, session_id, msg_id_begin, msg_cnt);
    CDBServConn* pDBConn = get_db_serv_conn_for_login();
    if (pDBConn) {
        CDbAttachData attach(ATTACH_TYPE_HANDLE, m_handle, 0);
        msg.set_user_id(GetUserId());
        msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
        pPdu->SetPBMsg(&msg);
        pDBConn->SendPdu(pPdu);
    }
}

void CMsgConn::_HandleClientGetMsgByMsgIdRequest(CImPdu *pPdu)
{
    IM::Message::IMGetMsgByIdReq msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
    uint32_t session_id = msg.session_id();
    uint32_t session_type = msg.session_type();
    uint32_t msg_cnt = msg.msg_id_list_size();
    log("_HandleClientGetMsgByMsgIdRequest, req_id=%u, session_type=%u, session_id=%u, msg_cnt=%u.",
        GetUserId(), session_type, session_id, msg_cnt);
    CDBServConn* pDBConn = get_db_serv_conn_for_login();
    if (pDBConn) {
        CDbAttachData attach(ATTACH_TYPE_HANDLE, m_handle, 0);
        msg.set_user_id(GetUserId());
        msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
        pPdu->SetPBMsg(&msg);
        pDBConn->SendPdu(pPdu);
    }
}

void CMsgConn::_HandleClientUnreadMsgCntRequest(CImPdu* pPdu)
{
	log("HandleClientUnreadMsgCntReq, from_id=%u ", GetUserId());
    IM::Message::IMUnreadMsgCntReq msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
    
	CDBServConn* pDBConn = get_db_serv_conn_for_login();
	if (pDBConn) {
		CDbAttachData attach(ATTACH_TYPE_HANDLE, m_handle, 0);
        msg.set_user_id(GetUserId());
        msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
        pPdu->SetPBMsg(&msg);
        pDBConn->SendPdu(pPdu);
	}
}

void CMsgConn::_HandleClientMsgReadAck(CImPdu* pPdu)
{
    IM::Message::IMMsgDataReadAck msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
    uint32_t session_type = msg.session_type();
    uint32_t session_id = msg.session_id();
    uint32_t msg_id = msg.msg_id();
	uint32_t from_id = msg.user_id();
    log("HandleClientMsgReadAck, user_id=%u, from_id:%u, session_id=%u, msg_id=%u, session_type=%u. ", GetUserId(),from_id,session_id, msg_id, session_type);
    
	CDBServConn* pDBConn = get_db_serv_conn();
	if (pDBConn) {
        msg.set_user_id(GetUserId());
        pPdu->SetPBMsg(&msg);
		pDBConn->SendPdu(pPdu);
	}
    IM::Message::IMMsgDataReadNotify msg2;
    msg2.set_user_id(GetUserId());
    msg2.set_session_id(session_id);
    msg2.set_msg_id(msg_id);
    msg2.set_session_type((IM::BaseDefine::SessionType)session_type);
    CImPdu pdu;
    pdu.SetPBMsg(&msg2);
    pdu.SetServiceId(SID_MSG);
    pdu.SetCommandId(CID_MSG_READ_NOTIFY);
    CImUser* pUser = CImUserManager::GetInstance()->GetImUserById(GetUserId());
    if (pUser)
    {
        pUser->BroadcastPdu(&pdu, this);
    }
    CRouteServConn* pRouteConn = get_route_serv_conn();
    if (pRouteConn) {
        pRouteConn->SendPdu(&pdu);
    }
    
   /* if (session_type == IM::BaseDefine::SESSION_TYPE_SINGLE)
    {
        DelFromSendList(msg_id, session_id);
    }*/
	DelFromSendList(msg_id, from_id,session_id, pPdu->GetSeqNum(),session_type);
}

void CMsgConn::_HandleClientGetLatestMsgIDReq(CImPdu *pPdu)
{
    IM::Message::IMGetLatestMsgIdReq msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
    uint32_t session_type = msg.session_type();
    uint32_t session_id = msg.session_id();
    log("HandleClientGetMsgListRequest, user_id=%u, session_id=%u, session_type=%u. ", GetUserId(),session_id, session_type);
    
    CDBServConn* pDBConn = get_db_serv_conn();
    if (pDBConn) {
        CDbAttachData attach(ATTACH_TYPE_HANDLE, m_handle, 0);
        msg.set_user_id(GetUserId());
        msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
        pPdu->SetPBMsg(&msg);
        pDBConn->SendPdu(pPdu);
    }
}


void CMsgConn::_HandleClientP2PCmdMsg(CImPdu* pPdu)
{
    IM::SwitchService::IMP2PCmdMsg msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
	string cmd_msg = msg.cmd_msg_data();
	uint32_t from_user_id = msg.from_user_id();
	uint32_t to_user_id = msg.to_user_id();

	log("HandleClientP2PCmdMsg, %u->%u, cmd_msg: %s ", from_user_id, to_user_id, cmd_msg.c_str());

    CImUser* pFromImUser = CImUserManager::GetInstance()->GetImUserById(GetUserId());
	CImUser* pToImUser = CImUserManager::GetInstance()->GetImUserById(to_user_id);
    
	if (pFromImUser) {
		pFromImUser->BroadcastPdu(pPdu, this);
	}
    
	if (pToImUser) {
		pToImUser->BroadcastPdu(pPdu, NULL);
	}
    
	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn) {
		pRouteConn->SendPdu(pPdu);
	}
}

void CMsgConn::_HandleClientUserInfoRequest(CImPdu* pPdu)
{
    IM::Buddy::IMUsersInfoReq msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
    uint32_t user_cnt = msg.user_id_list_size();
	log("HandleClientUserInfoReq, req_id=%u, user_cnt=%u ", GetUserId(), user_cnt);
	CDBServConn* pDBConn = get_db_serv_conn_for_login();
	if (pDBConn) {
		CDbAttachData attach(ATTACH_TYPE_HANDLE, m_handle, 0);
        msg.set_user_id(GetUserId());
        msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
        pPdu->SetPBMsg(&msg);
		pDBConn->SendPdu(pPdu);
	}
}

void CMsgConn::_HandleClientRemoveSessionRequest(CImPdu* pPdu)
{
    IM::Buddy::IMRemoveSessionReq msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
    uint32_t session_type = msg.session_type();
    uint32_t session_id = msg.session_id();
    log("HandleClientRemoveSessionReq, user_id=%u, session_id=%u, type=%u ", GetUserId(), session_id, session_type);
    
    CDBServConn* pConn = get_db_serv_conn();
    if (pConn) {
        CDbAttachData attach(ATTACH_TYPE_HANDLE, m_handle, 0);
        msg.set_user_id(GetUserId());
        msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
        pPdu->SetPBMsg(&msg);
        pConn->SendPdu(pPdu);
    }
    
	//george commented the bracket on Sep 29th,2015
  /*  if (session_type == IM::BaseDefine::SESSION_TYPE_SINGLE)
    {
        IM::Buddy::IMRemoveSessionNotify msg2;
        msg2.set_user_id(GetUserId());
        msg2.set_session_id(session_id);
        msg2.set_session_type((IM::BaseDefine::SessionType)session_type);
        CImPdu pdu;
        pdu.SetPBMsg(&msg2);
        pdu.SetServiceId(SID_BUDDY_LIST);
        pdu.SetCommandId(CID_BUDDY_LIST_REMOVE_SESSION_NOTIFY);
        CImUser* pImUser = CImUserManager::GetInstance()->GetImUserById(GetUserId());
        if (pImUser) {
            pImUser->BroadcastPdu(&pdu, this);
        }
        CRouteServConn* pRouteConn = get_route_serv_conn();
        if (pRouteConn) {
            pRouteConn->SendPdu(&pdu);
        }
    }*/
}

void CMsgConn::_HandleClientAllUserRequest(CImPdu* pPdu)
{
    IM::Buddy::IMAllUserReq msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
    uint32_t latest_update_time = msg.latest_update_time();
    log("HandleClientAllUserReq, user_id=%u, latest_update_time=%u. ", GetUserId(), latest_update_time);
    
    CDBServConn* pConn = get_db_serv_conn();
    if (pConn) {
        CDbAttachData attach(ATTACH_TYPE_HANDLE, m_handle, 0);
        msg.set_user_id(GetUserId());
        msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
        pPdu->SetPBMsg(&msg);
        pConn->SendPdu(pPdu);
    }
}

void CMsgConn::_HandleClientUpdateUserNagure(CImPdu* pPdu)
{
	IM::Buddy::IMUpdateUserNagureReq msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
	log("_HandleClientUpdateUserNagure, user_id=%u ", GetUserId());

	CDBServConn* pConn = get_db_serv_conn();
	if (pConn) {
		CDbAttachData attach(ATTACH_TYPE_HANDLE, m_handle, 0);
		msg.set_user_id(GetUserId());
		msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
		pPdu->SetPBMsg(&msg);
		pConn->SendPdu(pPdu);
	}
}
void CMsgConn::_HandleClientUpdateUser(CImPdu* pPdu)
{
	IM::Buddy::IMUpdateUserReq msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
	log("_HandleClientUpdateUser, user_id=%u ", GetUserId());

	CDBServConn* pConn = get_db_serv_conn();
	if (pConn)
	{
		CDbAttachData attach(ATTACH_TYPE_HANDLE, m_handle, 0);
		msg.set_user_id(GetUserId());
		msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
		pPdu->SetPBMsg(&msg);
		pConn->SendPdu(pPdu);
	}
}
void CMsgConn::_HandleClientUsersStatusRequest(CImPdu* pPdu)
{
    IM::Buddy::IMUsersStatReq msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
	uint32_t user_count = msg.user_id_list_size();
	log("HandleClientUsersStatusReq, user_id=%u, query_count=%u.", GetUserId(), user_count);
    
    CRouteServConn* pRouteConn = get_route_serv_conn();
    if(pRouteConn)
    {
        msg.set_user_id(GetUserId());
        CPduAttachData attach(ATTACH_TYPE_HANDLE, m_handle,0, NULL);
        msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
        pPdu->SetPBMsg(&msg);
        pRouteConn->SendPdu(pPdu);
    }
}
void CMsgConn::_HandleChangeAvatarRequest(CImPdu* pPdu)
{
	IM::Buddy::IMChangeAvatarReq msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
	log("HandleChangeAvatarRequest, user_id=%u, Avatar: %s ", GetUserId(),msg.avatar_url().c_str());
	CDBServConn* pDBConn = get_db_serv_conn();
	if (pDBConn) {
		CDbAttachData attach(ATTACH_TYPE_HANDLE, m_handle, 0);
		msg.set_user_id(GetUserId());
		msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
		pPdu->SetPBMsg(&msg);
		pDBConn->SendPdu(pPdu);
	}
}
void CMsgConn::_HandleClientDepartmentRequest(CImPdu *pPdu)
{
    IM::Buddy::IMDepartmentReq msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
    log("HandleClientDepartmentRequest, user_id=%u, company_id: %u, latest_update_time=%u.", GetUserId(), msg.company_id(), msg.latest_update_time());
    CDBServConn* pDBConn = get_db_serv_conn();
    if (pDBConn) {
        CDbAttachData attach(ATTACH_TYPE_HANDLE, m_handle, 0);
        msg.set_user_id(GetUserId());
		msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
        pPdu->SetPBMsg(&msg);
        pDBConn->SendPdu(pPdu);
    }
}
void CMsgConn::_HandleClientCompanyRequest(CImPdu* pPdu)
{
	IM::Buddy::IMCompanyReq msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
	log("HandleClientDepartmentRequest, user_id=%u, latest_update_time=%u.", GetUserId(), msg.latest_update_time());
	CDBServConn* pDBConn = get_db_serv_conn();
	if (pDBConn) {
		CDbAttachData attach(ATTACH_TYPE_HANDLE, m_handle, 0);
		msg.set_user_id(GetUserId());
		msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
		pPdu->SetPBMsg(&msg);
		pDBConn->SendPdu(pPdu);
	}
}
void CMsgConn::_HandleClientDeviceToken(CImPdu *pPdu)
{
    if (!CHECK_CLIENT_TYPE_MOBILE(GetClientType()) || GetUserId() == 0)
    {
        log("HandleClientDeviceToken, user_id=%u, not mobile client.", GetUserId());
        return;
    }
    IM::Login::IMDeviceTokenReq msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
    string device_token = msg.device_token();
    log("HandleClientDeviceToken, user_id=%u, device_token=%s ", GetUserId(), device_token.c_str());
    
    IM::Login::IMDeviceTokenRsp msg2;
    msg.set_user_id(GetUserId());
    msg.set_client_type((::IM::BaseDefine::ClientType)GetClientType());
    CImPdu pdu;
    pdu.SetPBMsg(&msg2);
    pdu.SetServiceId(SID_LOGIN);
    pdu.SetCommandId(CID_LOGIN_RES_DEVICETOKEN);
    pdu.SetSeqNum(pPdu->GetSeqNum());
    SendPdu(&pdu);
    
    CDBServConn* pDBConn = get_db_serv_conn();
	if (pDBConn) {
        msg.set_user_id(GetUserId());
        pPdu->SetPBMsg(&msg);
		pDBConn->SendPdu(pPdu);
	}
}



void CMsgConn::AddToSendList(uint32_t msg_id, uint32_t from_id, uint32_t to_id, uint32_t sequence_no, uint32_t session_type)
{
	log("AddSendMsg, seq_no=%u, from_id=%u, to_id:%u, current user:%u, msg_id=%u, session_type:%u ", sequence_no, from_id, to_id, m_user_id, msg_id, session_type);
	msg_ack_t msg;
	msg.msg_id = msg_id;
	msg.from_id = from_id;
	msg.to_id = to_id;
	msg.timestamp = get_tick_count();
	msg.session_type = session_type;
	msg.sequence_no = sequence_no;
	m_send_msg_list.push_back(msg);
	log("%d messages after appending a message, handler: %d", m_send_msg_list.size(), m_handle);
	g_down_msg_total_cnt++;
}

void CMsgConn::DelFromSendList(uint32_t msg_id, uint32_t from_id, uint32_t to_id, uint32_t sequence_no, uint32_t session_type)
{
	//log("DelSendMsg, seq_no=%u, from_id=%u, msg_id=%u, session_type=%u", sequence_no, from_id, msg_id,session_type);
	for (list<msg_ack_t>::iterator it = m_send_msg_list.begin(); it != m_send_msg_list.end(); it++)
	{
		msg_ack_t msg = *it;
		if ( (msg.msg_id == msg_id) && (msg.from_id == from_id) && (msg.to_id == to_id) && (msg.session_type == session_type))
		{
			m_send_msg_list.erase(it);
			m_bSent = true;
			log("DelSendMsg, seq_no=%u, from_id=%u, to_id=%u, msg_id=%u, session_type=%u", sequence_no, from_id, to_id, msg_id, session_type);
			log("%d messages remaining after deleting a message, handler: %d", m_send_msg_list.size(), m_handle);
			break;
		}
	}
}

uint32_t CMsgConn::GetClientTypeFlag()
{
    uint32_t client_type_flag = 0x00;
    if (CHECK_CLIENT_TYPE_PC(GetClientType()))
    {
        client_type_flag = CLIENT_TYPE_FLAG_PC;
    }
    else if (CHECK_CLIENT_TYPE_MOBILE(GetClientType()))
    {
        client_type_flag = CLIENT_TYPE_FLAG_MOBILE;
    }
    return client_type_flag;
}


void CMsgConn::_HandleOnlineUsersRequest(CImPdu* pPdu)
{
	IM::Buddy::IMOnlineUsersReq msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
	uint32_t user_id = msg.user_id();
	log("HandleClientOnlineUsersReq, user_id=%u", GetUserId());

	CRouteServConn* pRouteConn = get_route_serv_conn();
	if (pRouteConn)
	{
		msg.set_user_id(GetUserId());
		CPduAttachData attach(ATTACH_TYPE_HANDLE, m_handle, 0, NULL);
		msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
		pPdu->SetPBMsg(&msg);
		pRouteConn->SendPdu(pPdu);
	}
}


void CMsgConn::_HandleClientShieldUserRequest(CImPdu* pPdu)
{
	IM::Buddy::IMUserShieldReq msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

	uint32_t shield_status = msg.shield_status();
	uint32_t peer_id = msg.peer_id();
	uint32_t user_id = GetUserId();
	log("HandleClientShieldUserRequest, user_id: %u, peer_id: %u, shield_status: %u. ",
		user_id, peer_id, shield_status);

	CDBServConn* pDbConn = get_db_serv_conn();
	if (pDbConn)
	{
		CDbAttachData attach_data(ATTACH_TYPE_HANDLE, m_handle, 0);
		msg.set_user_id(user_id);
		msg.set_attach_data(attach_data.GetBuffer(), attach_data.GetLength());
		pPdu->SetPBMsg(&msg);
		pDbConn->SendPdu(pPdu);

	}
	else 
	{
		log("no DB connection ");
		IM::Buddy::IMUserShieldRsp msg2;
		msg2.set_user_id(user_id);
		msg2.set_result_code(1);
		msg2.set_peer_id(peer_id);
		CImPdu pdu;
		pdu.SetPBMsg(&msg2);
		pdu.SetServiceId(SID_BUDDY_LIST);
		pdu.SetCommandId(CID_BUDDY_SHIELD_USER_RESPONSE);
		pdu.SetSeqNum(pPdu->GetSeqNum());
		SendPdu(&pdu);
	}
}


int CMsgConn::GetSendList(void)
{
	return m_send_msg_list.size();
}


void CMsgConn::ResendMessageFromSendList(void)
{
	//log("%d messages to be sent for handler(connection): %d", m_send_msg_list.size(), m_handle);
	if (GetSentStatus())
	{
		list<msg_ack_t>::iterator it = m_send_msg_list.begin();
		if (it != m_send_msg_list.end())
		{
			//msg_ack_t msg = *it;
			it->timestamp = get_tick_count();//reset message sent time
			IM::Message::IMGetResendMsgByIdReq msg;
			msg.set_resend_user_id(m_user_id);
			msg.set_user_id(it->from_id);
			msg.set_session_id(it->to_id);
			msg.set_session_type((IM::BaseDefine::SessionType)it->session_type);
			msg.add_msg_id_list(it->msg_id);//one message each time

			log("ResendMessageFromSendList, user_id=%u, from_id=%d, session_id=%u, msgid=%d, session_type=%u. ", m_user_id, it->from_id, it->to_id, it->msg_id, it->session_type);

			CDBServConn* pDBConn = get_db_serv_conn();
			if (pDBConn)
			{
				CDbAttachData attach(ATTACH_TYPE_HANDLE, m_handle, 0);
				msg.set_attach_data(attach.GetBuffer(), attach.GetLength());

				CImPdu pdu;
				pdu.SetPBMsg(&msg);
				pdu.SetServiceId(SID_MSG);
				pdu.SetCommandId(CID_MSG_RESEND_MSG_REQ);
				pdu.SetSeqNum(it->sequence_no);
				pDBConn->SendPdu(&pdu);
				log("Sending a message for handler:%d from the send list.........",m_handle);
				SetSentStatus(false);
				//m_send_msg_list.erase(it);
			}
		}
	}
}


void CMsgConn::SetSentStatus(bool bSentStatus)
{
	m_bSent = bSentStatus;
}


bool CMsgConn::GetSentStatus(void)
{
	return m_bSent;
}


void CMsgConn::ResendMessageFromSendList(list<msg_ack_t>::iterator &it)
{
	//log("%d messages to be sent for handler(connection): %d", m_send_msg_list.size(), m_handle);
	if (GetSentStatus())
	{
		it->timestamp = get_tick_count();//reset message sent time
		IM::Message::IMGetResendMsgByIdReq msg;
		msg.set_resend_user_id(m_user_id);
		msg.set_user_id(it->from_id);
		msg.set_session_id(it->to_id);
		msg.set_session_type((IM::BaseDefine::SessionType)it->session_type);
		msg.add_msg_id_list(it->msg_id);//one message each time

		log("ResendMessageFromSendList, user_id=%u, from_id=%u, session_id=%u, msgid=%d, session_type=%u. ", m_user_id, it->from_id, it->to_id,it->msg_id, it->session_type);

		CDBServConn* pDBConn = get_db_serv_conn();
		if (pDBConn)
		{
			CDbAttachData attach(ATTACH_TYPE_HANDLE, m_handle, 0);
			msg.set_attach_data(attach.GetBuffer(), attach.GetLength());

			CImPdu pdu;
			pdu.SetPBMsg(&msg);
			pdu.SetServiceId(SID_MSG);
			pdu.SetCommandId(CID_MSG_RESEND_MSG_REQ);
			pdu.SetSeqNum(it->sequence_no);
			pDBConn->SendPdu(&pdu);
			log("Sending a message for handler:%d from the send list.........", m_handle);
			SetSentStatus(false);
			//m_send_msg_list.erase(it);
		}
	}
}


bool  CMsgConn::AddToRecvList(uint32_t msg_id, uint32_t from_id, uint32_t to_id, uint32_t sequence_no, uint32_t session_type)
{
	bool bResult = true;
	log("AddRecvMsg, seq_no=%u, from_id=%u, to_id:%u, current user:%u, msg_id=%u, session_type:%u ", sequence_no, from_id, to_id, m_user_id, msg_id, session_type);
	msg_ack_t msg;
	msg.msg_id = msg_id;
	msg.from_id = from_id;
	msg.to_id = to_id;
	msg.timestamp = get_tick_count();
	msg.session_type = session_type;
	msg.sequence_no = sequence_no;
	list<msg_ack_t>::iterator it = m_recv_msg_list.begin();
	for (; it != m_recv_msg_list.end(); it++)
	{
		if (it->sequence_no == sequence_no)
		{
			if (it->msg_id == 0)
			{
				if (msg_id != 0)
				{
					it->msg_id = msg_id;
					it->timestamp = get_tick_count();
					log("message successfully inserted into db and msg id is:%d ,handler: %d ", msg_id, m_handle);
				}
				else
				{
					log("resend message from handler: %d, previous packet sequence %d is now waiting for db's response,and just directly send MSG_DATA_ACK packet ", m_handle, sequence_no);
					ResendMessageFromRecvList(it);
					bResult = false;
				}
			}
			else
			{
				log("message for handler: %d 's sequence %d is alreay exists,discard it and just directly send MSG_DATA_ACK packet", m_handle, sequence_no);
				ResendMessageFromRecvList(it);
				bResult = false;
			}

		/*	if (msg_id != 0)
			{
				it->msg_id = msg_id;
				it->timestamp = get_tick_count();
				log("msg successfully inserted into db and msg id is:%d ,handler: %d ", msg_id, m_handle);
			}
			else
			{
				log("message for handler: %d 's sequence %d is alreay exists,discard it", m_handle, sequence);
			}*/
			break;
		}
	}

	if (it == m_recv_msg_list.end())
	{
		m_recv_msg_list.push_back(msg);
		log("totally receiving %d messages after appending a new one handler: %d", m_recv_msg_list.size(), m_handle);
	}

	return bResult;
}


void CMsgConn::DelFromRecvList(uint64_t ullCurrTime)
{
	//log("DelRecvMsg, seq_no=%u, from_id=%u, msg_id=%u, session_type=%u", sequence_no, from_id, msg_id,session_type);
	for (list<msg_ack_t>::iterator it = m_recv_msg_list.begin(); it != m_recv_msg_list.end(); )
	{
		list<msg_ack_t>::iterator oldit = it;
		msg_ack_t msg = *oldit;
		++it;

		if (ullCurrTime - msg.timestamp >= TIMEOUT_WAITING_RESEND_MSG_DATA)
		{
			log("DelRecvMsg, seq_no=%u, from_id=%u, to_id=%u, msg_id=%u, session_type=%u", oldit->sequence_no, oldit->from_id, oldit->to_id, oldit->msg_id, oldit->session_type);
			log("%d messages remaining after deleting a message from receiving list, handler: %d", m_recv_msg_list.size(), m_handle);
			m_recv_msg_list.erase(oldit);
		}
		else
		{
			break;
		}
	}

}


bool CMsgConn::ResendMessageFromRecvList(uint32_t sequence_no)
{
	bool bResult = false;
	for (list<msg_ack_t>::iterator it = m_recv_msg_list.begin(); it != m_recv_msg_list.end(); ++it)
	{
		if (it->sequence_no == sequence_no)
		{
			IM::Message::IMMsgDataAck msg2;
			//msg2.set_user_id(GetUserId());
			msg2.set_user_id(it->from_id);
			msg2.set_msg_id(it->msg_id);
			msg2.set_session_id(it->to_id);
			msg2.set_session_type((::IM::BaseDefine::SessionType)it->session_type);
			msg2.set_result_code(0);

			CImPdu pdu;
			pdu.SetPBMsg(&msg2);
			pdu.SetServiceId(SID_MSG);
			pdu.SetCommandId(CID_MSG_DATA_ACK);
			pdu.SetSeqNum(sequence_no);
			SendPdu(&pdu);

			log("SEQ: %d, FROM: %d, TO: %d, SESSION TYPE: %d, MSG ID: %d", it->sequence_no, it->from_id, it->to_id, it->session_type, it->msg_id);
			bResult = true;
			break;
		}
	}
	return bResult;
}


void CMsgConn::ResendMessageFromRecvList(list<msg_ack_t>::iterator &it)
{
	IM::Message::IMMsgDataAck msg2;
	//msg2.set_user_id(GetUserId());
	msg2.set_user_id(it->from_id);
	msg2.set_msg_id(it->msg_id);
	msg2.set_session_id(it->to_id);
	msg2.set_session_type((::IM::BaseDefine::SessionType)it->session_type);
	msg2.set_result_code(0);

	CImPdu pdu;
	pdu.SetPBMsg(&msg2);
	pdu.SetServiceId(SID_MSG);
	pdu.SetCommandId(CID_MSG_DATA_ACK);
	pdu.SetSeqNum(it->sequence_no);
	SendPdu(&pdu);
	log("SEQ: %d, FROM: %d, TO: %d, SESSION TYPE: %d, MSG ID: %d", it->sequence_no, it->from_id, it->to_id, it->session_type, it->msg_id);
}


bool CMsgConn::IsValidClientVersion()
{
	CConfigFileReader config_file("clientversion.conf");
	char* pszClientVersion = NULL;

	if (m_client_type == IM::BaseDefine::CLIENT_TYPE_WINDOWS)
	{
		pszClientVersion = config_file.GetConfigName("Windows");
	}
	else if (m_client_type == IM::BaseDefine::CLIENT_TYPE_IOS)
	{
		pszClientVersion = config_file.GetConfigName("IOS");
	}
	else if (m_client_type == IM::BaseDefine::CLIENT_TYPE_ANDROID)
	{
		pszClientVersion = config_file.GetConfigName("Android");
	}
	else if (m_client_type == IM::BaseDefine::CLIENT_TYPE_MAC)
	{
		pszClientVersion = config_file.GetConfigName("Mac");
	}

	if (IsAllowdedClientVersion(pszClientVersion))
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool CMsgConn::IsAllowdedClientVersion(const char* pszCurrVersion)
{
	//bool bResult = false;
	if (pszCurrVersion != NULL)
	{
		int nCurrVersion[4], nClientVersion[4];
		memset(nCurrVersion, 0, sizeof(nCurrVersion));
		memset(nClientVersion, 0, sizeof(nClientVersion));
		int nCount1 = sscanf(pszCurrVersion, "%d.%d.%d.%d", nCurrVersion, nCurrVersion + 1, nCurrVersion + 2, nCurrVersion + 3);
		int nCount2 = sscanf(m_client_version.c_str(), "%d.%d.%d.%d", nClientVersion, nClientVersion + 1, nClientVersion + 2, nClientVersion + 3);
		if (nCount2 != 4)
		{
			char szVersionType[32];
			memset(szVersionType, 0, sizeof(szVersionType));
			nCount2 = sscanf(m_client_version.c_str(), "%d.%d.%d.%d.%s", nClientVersion, nClientVersion + 1, nClientVersion + 2, nClientVersion + 3, szVersionType);
			return false;
		}

		if ((nCount2 >= nCount1) && nCount1 == 4)
		{
			if (nClientVersion[0] < nCurrVersion[0])
			{
				return false;
			}

			if (nClientVersion[0] > nCurrVersion[0])
			{
				return true;
			}

			if (nClientVersion[1] < nCurrVersion[1])
			{
				return false;
			}

			if (nClientVersion[1] > nCurrVersion[1])
			{
				return true;
			}

			if (nClientVersion[2] < nCurrVersion[2])
			{
				return false;
			}

			if (nClientVersion[2] >= nCurrVersion[2])
			{
				return true;
			}

			////if (nClientVersion[0] >= nCurrVersion[0] && nClientVersion[1] >= nCurrVersion[1] && nClientVersion[2] >= nCurrVersion[2] && nClientVersion[3] >= nCurrVersion[3])
			//if (nClientVersion[0] > nCurrVersion[0] && nClientVersion[1] >= nCurrVersion[1] && nClientVersion[2] >= nCurrVersion[2])
			//{
			//	bResult = true;
			//}
		}
	}

	return false;
}


void CMsgConn::_HandleClientPositionRequest(CImPdu* pPdu)
{
	IM::Buddy::IMPositionReq msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
	log("HandleClientPositionRequest, user_id=%u, company_id=%u, latest_update_time=%u.", GetUserId(), msg.company_id(), msg.latest_update_time());
	CDBServConn* pDBConn = get_db_serv_conn();
	if (pDBConn)
	{
		CDbAttachData attach(ATTACH_TYPE_HANDLE, m_handle, 0);
		msg.set_user_id(GetUserId());
		msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
		pPdu->SetPBMsg(&msg);
		pDBConn->SendPdu(pPdu);
	}
}


void CMsgConn::_HandleClientAllUserLiteRequest(CImPdu *pPdu)
{
	/*IM::Buddy::IMAllUserLiteReq msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
	uint32_t latest_update_time = msg.latest_update_time();
	log("HandleClientAllUserLiteReq, user_id=%u, latest_update_time=%u. ", GetUserId(), latest_update_time);

	CDBServConn* pConn = get_db_serv_conn();
	if (pConn)
	{
		CDbAttachData attach(ATTACH_TYPE_HANDLE, m_handle, 0);
		msg.set_user_id(GetUserId());
		msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
		pPdu->SetPBMsg(&msg);
		pConn->SendPdu(pPdu);
	}*/
}


void CMsgConn::_HandleClientUserInfoLiteRequest(CImPdu *pPdu)
{
	/*IM::Buddy::IMUserInfoLiteReq msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
	uint32_t user_cnt = msg.user_id_list_size();
	log("HandleClientUserInfoLiteReq, req_id=%u, user_cnt=%u ", GetUserId(), user_cnt);
	CDBServConn* pDBConn = get_db_serv_conn_for_login();
	if (pDBConn) 
	{
		CDbAttachData attach(ATTACH_TYPE_HANDLE, m_handle, 0);
		msg.set_user_id(GetUserId());
		msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
		pPdu->SetPBMsg(&msg);
		pDBConn->SendPdu(pPdu);
	}*/
}


void CMsgConn::_HandleClientAllStaffRequest(CImPdu* pPdu)
{
	IM::Buddy::IMAllStaffReq msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
	uint32_t latest_update_time = msg.latest_update_time();
	uint32_t unCompanyId = msg.company_id();
	log("HandleClientAllStaffReq, user_id=%u, company_id= %u, latest_update_time=%u. ", GetUserId(), unCompanyId, latest_update_time);

	CDBServConn* pConn = get_db_serv_conn();
	if (pConn)
	{
		CDbAttachData attach(ATTACH_TYPE_HANDLE, m_handle, 0);
		msg.set_user_id(GetUserId());
		msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
		pPdu->SetPBMsg(&msg);
		pConn->SendPdu(pPdu);
	}
}


void CMsgConn::_HandleClientAllStaffLiteRequest(CImPdu* pPdu)
{
	IM::Buddy::IMAllStaffLiteReq msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
	uint32_t latest_update_time = msg.latest_update_time();
	uint32_t unCompanyId = msg.company_id();
	log("HandleClientAllStaffLiteReq, user_id=%u, company_id= %u, latest_update_time=%u. ", GetUserId(), unCompanyId, latest_update_time);

	CDBServConn* pConn = get_db_serv_conn();
	if (pConn)
	{
		CDbAttachData attach(ATTACH_TYPE_HANDLE, m_handle, 0);
		msg.set_user_id(GetUserId());
		msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
		pPdu->SetPBMsg(&msg);
		pConn->SendPdu(pPdu);
	}
}


void CMsgConn::_HandleClientStaffInfoRequest(CImPdu* pPdu)
{
	IM::Buddy::IMStaffInfoReq msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
	uint32_t user_cnt = msg.user_id_list_size();
	log("HandleClientStaffInfoReq, req_id=%u, user_cnt=%u ", GetUserId(), user_cnt);
	CDBServConn* pDBConn = get_db_serv_conn();
	if (pDBConn)
	{
		CDbAttachData attach(ATTACH_TYPE_HANDLE, m_handle, 0);
		msg.set_user_id(GetUserId());
		msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
		pPdu->SetPBMsg(&msg);
		pDBConn->SendPdu(pPdu);
	}
}


void CMsgConn::_HandleClientUpdateStaffRequest(CImPdu* pPdu)
{
	IM::Buddy::IMUpdateStaffReq msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
	log("HandleClientUpdateStaffReq, user_id=%u ",GetUserId());

	CDBServConn* pConn = get_db_serv_conn();
	if (pConn)
	{
		CDbAttachData attach(ATTACH_TYPE_HANDLE, m_handle, 0);
		msg.set_user_id(GetUserId());
		msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
		pPdu->SetPBMsg(&msg);
		pConn->SendPdu(pPdu);
	}
}


void CMsgConn::_HandleClientChangeVipRequest(CImPdu* pPdu)
{
	IM::Buddy::IMChangeVipRequest msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
	log("HandleClientChangeVipReq, user_id=%u ", GetUserId());

	CDBServConn* pConn = get_db_serv_conn();
	if (pConn)
	{
		CDbAttachData attach(ATTACH_TYPE_HANDLE, m_handle, 0);
		msg.set_user_id(GetUserId());
		msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
		pPdu->SetPBMsg(&msg);
		pConn->SendPdu(pPdu);
	}
}


void CMsgConn::_HandleClientAssistantInfoRequest(CImPdu* pPdu)
{
	IM::Buddy::IMAssistantInfoReq msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
	uint32_t latest_update_time = msg.latest_update_time();
	log("HandleClientAssistantInfoReq, req_id=%u, last_update_time=%u ", GetUserId(), latest_update_time);
	CDBServConn* pDBConn = get_db_serv_conn();
	if (pDBConn)
	{
		CDbAttachData attach(ATTACH_TYPE_HANDLE, m_handle, 0);
		msg.set_user_id(GetUserId());
		msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
		pPdu->SetPBMsg(&msg);
		pDBConn->SendPdu(pPdu);
	}
}


void CMsgConn::_HandleClientQueryUserInfoRequest(CImPdu* pPdu)
{
	IM::Buddy::IMQueryUserInfoReq msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
	string strUserUuid = msg.user_uuid();
	log("HandleClientQueryUserInfoReq, req_id=%u,user_uuid=%u ", GetUserId(), strUserUuid.c_str());
	CDBServConn* pDBConn = get_db_serv_conn();
	if (pDBConn)
	{
		CDbAttachData attach(ATTACH_TYPE_HANDLE, m_handle, 0);
		msg.set_user_id(GetUserId());
		msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
		pPdu->SetPBMsg(&msg);
		pDBConn->SendPdu(pPdu);
	}
}


uint64_t CMsgConn::GetLoginTime(void)
{
	return m_login_time;
}