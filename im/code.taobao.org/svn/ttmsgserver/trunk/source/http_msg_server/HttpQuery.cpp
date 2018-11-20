/*
 * HttpQuery.cpp
 *
 *  Created on: 2013-10-22
 *      Author: ziteng@mogujie.com
 */
#include <sstream>

#include "public_define.h"
#include "HttpQuery.h"
#include "RouteServConn.h"
#include "DBServConn.h"
#include "HttpPdu.h"
#include "public_define.h"
#include "AttachData.h"
#include "security.h"
#include "IM.Message.pb.h"
#include "IM.Buddy.pb.h"
#include "IM.SwitchService.pb.h"
#include "IM.Group.pb.h"

static uint32_t g_total_query = 0;
static uint32_t g_last_year = 0;
static uint32_t g_last_month = 0;
static uint32_t g_last_mday = 0;

CHttpQuery* CHttpQuery::m_query_instance = NULL;

hash_map<string, auth_struct*> g_hm_http_auth;

void http_query_timer_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	struct tm* tm;
	time_t currTime;

	time(&currTime);
	tm = localtime(&currTime);

	uint32_t year = tm->tm_year + 1900;
	uint32_t mon = tm->tm_mon + 1;
	uint32_t mday = tm->tm_mday;
	if (year != g_last_year || mon != g_last_month || mday != g_last_mday) {
		// a new day begin, clear the count
		log("a new day begin, g_total_query=%u ", g_total_query);
		g_total_query = 0;
		g_last_year = year;
		g_last_month = mon;
		g_last_mday = mday;
	}
}

CHttpQuery* CHttpQuery::GetInstance()
{
	if (!m_query_instance) {
		m_query_instance = new CHttpQuery();
		netlib_register_timer(http_query_timer_callback, NULL, 1000);
	}

	return m_query_instance;
}

void CHttpQuery::DispatchQuery(std::string& url, std::string& post_data, CHttpConn* pHttpConn)
{
	++g_total_query;
    
    /* 在用
     /query/SendMessageByXiaoT
     /query/GroupP2PMessage
     
     */
	log("DispatchQuery, url=%s, content=%s ", url.c_str(), post_data.c_str());

    Json::Reader reader;
    Json::Value value;
    Json::Value root;

	if ( !reader.parse(post_data, value) ) {
		log("json parse failed, post_data=%s ", post_data.c_str());
		pHttpConn->Close();
		return;
	}
    
    string strErrorMsg;
    string strAppKey;
    HTTP_ERROR_CODE nRet = HTTP_ERROR_SUCCESS;
    try
    {
        string strInterface(url.c_str() + strlen("/query/"));
        strAppKey = value["app_key"].asString();
        string strIp = pHttpConn->GetPeerIP();
        uint32_t nUserId = value["req_user_id"].asUInt();
        nRet = _CheckAuth(strAppKey, nUserId, strInterface, strIp);
    }
    catch ( std::runtime_error msg)
    {
        nRet = HTTP_ERROR_INTERFACE;
    }
    
    if(HTTP_ERROR_SUCCESS != nRet)
    {
        if(nRet < HTTP_ERROR_MAX)
        {
            root["error_code"] = nRet;
            root["error_msg"] = HTTP_ERROR_MSG[nRet];
        }
        else
        {
            root["error_code"] = -1;
            root["error_msg"] = "未知错误";
        }
        string strResponse = root.toStyledString();
        pHttpConn->Send((void*)strResponse.c_str(), strResponse.length());
        return;
    }
    
    // process post request with post content
    if (strcmp(url.c_str(), "/query/CreateGroup") == 0)
    {
        _QueryCreateGroup(strAppKey, value, pHttpConn);
    }
    else if (strcmp(url.c_str(), "/query/ChangeMembers") == 0)
    {
        _QueryChangeMember(strAppKey, value, pHttpConn);
    }
	else if (strcmp(url.c_str(), "/query/SendMessage") == 0)
	{
		_QuerySendMessage(strAppKey, value, pHttpConn);
	}
    else {
        log("url not support ");
        pHttpConn->Close();
        return;
    }
}

void CHttpQuery::_QueryCreateGroup(const string& strAppKey, Json::Value &post_json_obj, CHttpConn *pHttpConn)
{
    HTTP::CDBServConn *pConn = HTTP::get_db_serv_conn();
    if (!pConn) {
        log("no connection to DBProxy ");
        char* response_buf = PackSendResult(HTTP_ERROR_SERVER_EXCEPTION , HTTP_ERROR_MSG[9].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
        return;
    }
    
    if ( post_json_obj["req_user_id"].isNull()) {
        log("no user id ");
        char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
        return;

    }
    
    if (post_json_obj["group_name"].isNull()) {
        log("no group name ");
        char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
        return;
    }
    
    if (post_json_obj["group_type"].isNull()) {
        log("no group type ");
        char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
        return;
    }
    
    if (post_json_obj["group_avatar"].isNull()) {
        log("no group avatar ");
        char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
        return;
    }
    
    if (post_json_obj["user_id_list"].isNull()) {
        log("no user list ");
        char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
        return;
    }
    
    try
    {
        uint32_t user_id = post_json_obj["req_user_id"].asUInt();
        string group_name = post_json_obj["group_name"].asString();
        uint32_t group_type = post_json_obj["group_type"].asUInt();
        string group_avatar = post_json_obj["group_avatar"].asString();
        uint32_t user_cnt = post_json_obj["user_id_list"].size();
        log("QueryCreateGroup, user_id: %u, group_name: %s, group_type: %u, user_cnt: %u. ",
            user_id, group_name.c_str(), group_type, user_cnt);
        if (!IM::BaseDefine::GroupType_IsValid(group_type)) {
            log("QueryCreateGroup, unvalid group_type");
            char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
            pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
            pHttpConn->Close();
            return;
        }
        
        CDbAttachData attach_data(ATTACH_TYPE_HANDLE, pHttpConn->GetConnHandle());
        IM::Group::IMGroupCreateReq msg;
        msg.set_user_id(0);
        msg.set_group_name(group_name);
        msg.set_group_avatar(group_avatar);
        msg.set_group_type((::IM::BaseDefine::GroupType)group_type);
        for (uint32_t i = 0; i < user_cnt; i++) {
            uint32_t member_id = post_json_obj["user_id_list"][i].asUInt();
            msg.add_member_id_list(member_id);
        }
        msg.set_attach_data(attach_data.GetBuffer(), attach_data.GetLength());
        
        CImPdu pdu;
        pdu.SetPBMsg(&msg);
        pdu.SetServiceId(IM::BaseDefine::SID_GROUP);
        pdu.SetCommandId(IM::BaseDefine::CID_GROUP_CREATE_REQUEST);
        pConn->SendPdu(&pdu);

    }
    catch (std::runtime_error msg)
    {
        log("parse json data failed.");
        char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
    }
    
}

void CHttpQuery::_QueryChangeMember(const string& strAppKey, Json::Value &post_json_obj, CHttpConn *pHttpConn)
{
    HTTP::CDBServConn *pConn = HTTP::get_db_serv_conn();
    if (!pConn) {
        log("no connection to RouteServConn ");
        char* response_buf = PackSendResult(HTTP_ERROR_SERVER_EXCEPTION, HTTP_ERROR_MSG[9].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
        return;
    }

    if ( post_json_obj["req_user_id"].isNull()) {
        log("no user id ");
        char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
        return;
    }
    
    if ( post_json_obj["group_id"].isNull() ) {
        log("no group id ");
        char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
        return;
    }
    
    if ( post_json_obj["modify_type"].isNull() ) {
        log("no modify_type ");
        char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
        return;
    }
    
    if (post_json_obj["user_id_list"].isNull()) {
        log("no user list ");
        char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
        return;
    }
    
    try
    {
        uint32_t user_id = post_json_obj["req_user_id"].asUInt();
        uint32_t group_id = post_json_obj["group_id"].asUInt();
        uint32_t modify_type = post_json_obj["modify_type"].asUInt();
        uint32_t user_cnt =  post_json_obj["user_id_list"].size();
        log("QueryChangeMember, user_id: %u, group_id: %u, modify type: %u, user_cnt: %u. ",
            user_id, group_id, modify_type, user_cnt);
        if (!IM::BaseDefine::GroupModifyType_IsValid(modify_type)) {
            log("QueryChangeMember, unvalid modify_type");
            char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
            pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
            pHttpConn->Close();
            return;
        }
        CDbAttachData attach_data(ATTACH_TYPE_HANDLE, pHttpConn->GetConnHandle());
        IM::Group::IMGroupChangeMemberReq msg;
        msg.set_user_id(0);
        msg.set_change_type((::IM::BaseDefine::GroupModifyType)modify_type);
        msg.set_group_id(group_id);
        for (uint32_t i = 0; i < user_cnt; i++) {
            uint32_t member_id = post_json_obj["user_id_list"][i].asUInt();
            msg.add_member_id_list(member_id);
        }
        msg.set_attach_data(attach_data.GetBuffer(), attach_data.GetLength());
        CImPdu pdu;
        pdu.SetPBMsg(&msg);
        pdu.SetServiceId(IM::BaseDefine::SID_GROUP);
        pdu.SetCommandId(IM::BaseDefine::CID_GROUP_CHANGE_MEMBER_REQUEST);
        pConn->SendPdu(&pdu);
    }
    catch (std::runtime_error msg)
    {
        log("parse json data failed.");
        char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
    }
}

HTTP_ERROR_CODE CHttpQuery::_CheckAuth(const string& strAppKey, const uint32_t userId, const string& strInterface, const string& strIp)
{
    return HTTP_ERROR_SUCCESS;
}

HTTP_ERROR_CODE CHttpQuery::_CheckPermission(const string& strAppKey, uint8_t nType, const list<uint32_t>& lsToId, string strMsg)
{
    strMsg.clear();
    return HTTP_ERROR_SUCCESS;
}


void CHttpQuery::_QuerySendMessage(const string& strAppKey, Json::Value& post_json_obj, CHttpConn* pHttpConn)
{
	HTTP::CDBServConn *pConn = HTTP::get_db_serv_conn();
	if (!pConn) {
		log("no connection to RouteServConn ");
		char* response_buf = PackSendResult(HTTP_ERROR_SERVER_EXCEPTION, HTTP_ERROR_MSG[9].c_str());
		pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
		pHttpConn->Close();
		return;
	}

	//
	if (post_json_obj["to_user_id"].isNull() || !post_json_obj["to_user_id"].isUInt())
	{
		log("no user id ");
		char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
		pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
		pHttpConn->Close();
		return;
	}

	if (post_json_obj["product_id"].isNull() || !post_json_obj["product_id"].isUInt())
	{
		log("no product id ");
		char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
		pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
		pHttpConn->Close();
		return;
	}

	if (post_json_obj["detailed_msg"].isNull() || !post_json_obj["detailed_msg"].isString())
	{
		log("no detaild msg ");
		char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
		pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
		pHttpConn->Close();
		return;
	}

	/*if (post_json_obj["receiver_type"].isNull() || !post_json_obj["receiver_type"].isUInt())
	{
		log("no receiver_type ");
		char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
		pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
		pHttpConn->Close();
		return;
	}*/

	try
	{
		uint32_t user_id = post_json_obj["to_user_id"].asUInt();
		//uint32_t unType = post_json_obj["receiver_type"].asUInt();
		uint32_t product_id = post_json_obj["product_id"].asUInt();
		string strMsgData = post_json_obj["detailed_msg"].asString();
		uint32_t unMessageType = 0;
		
		if (product_id == HUAYUN_IM_PRODUCT_WORKFLOW)
		{
			unMessageType = IM::BaseDefine::MSG_TYPE_SINGLE_WORKFLOW;
		}
		else if (product_id == HUAYUN_IM_PRODUCT_EMAIL)
		{
			unMessageType = IM::BaseDefine::MSG_TYPE_SINGLE_EMAIL;
		}
		else if (product_id == HUAYUN_IM_PRODUCT_BULLETIN)
		{
			unMessageType = IM::BaseDefine::MSG_TYPE_SINGLE_BULLETIN;
		}
		else if (product_id == HUAYUN_IM_ASST_NEW_FRIEND)
		{
			unMessageType = IM::BaseDefine::MSG_TYPE_SINGLE_NEW_FRIEND;
		}
		else if (product_id == HUAYUN_IM_ASST_NEW_STAFF)
		{
			unMessageType = IM::BaseDefine::MSG_TYPE_SINGLE_NEW_STAFF;
		}
		else if (product_id == HUAYUN_IM_ASST_ATTENDANCE)
		{
			unMessageType = IM::BaseDefine::MSG_TYPE_SINGLE_ATTENDANCE;
		}
	/*	else if (product_id == HUAYUN_IM_ASST_MOMENT)
		{
			unMessageType = IM::BaseDefine::MSG_TYPE_SINGLE_MOMENT;
		}*/

		log("QuerySendMessage, to_user_id: %u,  message_type: %u, details: %s ",
			user_id, unMessageType, strMsgData.c_str());

		IM::Message::IMMsgData msg;
		msg.set_from_user_id(product_id);
		msg.set_to_session_id(user_id);
		msg.set_msg_type((IM::BaseDefine::MsgType)unMessageType);
		msg.set_msg_id(0);
		
		char *pszData = NULL;
		uint32_t nDataLen = 0;
		int nResult = EncryptMsg(strMsgData.c_str(), strMsgData.length(), &pszData, nDataLen);
		log("JSON DATA: %s,LEN: %d, OUTDATALEN:%d", strMsgData.c_str(), strMsgData.length(), nDataLen);
		if (!nResult)
		{
			msg.set_msg_data(pszData);
			log("successfully encrypted data when changing user........");
			Free(pszData);
			pszData = NULL;
		}
		else
		{
			msg.set_msg_data(strMsgData.c_str());
		}

		msg.set_create_time(time(NULL));
		CDbAttachData attach_data(ATTACH_TYPE_HANDLE, pHttpConn->GetConnHandle());
		msg.set_attach_data(attach_data.GetBuffer(), attach_data.GetLength());

		CImPdu pdu;
		pdu.SetPBMsg(&msg);
		pdu.SetServiceId(IM::BaseDefine::SID_MSG);
		pdu.SetCommandId(IM::BaseDefine::CID_MSG_DATA);
		//pdu.SetSeqNum(random() % 1000);

		/*if (product_id == HUAYUN_IM_ASST_NEW_FRIEND)
		{*/
			//log("user: %s no  found, retry again after 5 seconds", strName.c_str());
			struct timespec TimeOut;
			memset(&TimeOut, 0, sizeof(TimeOut));
			TimeOut.tv_sec = 0;
			TimeOut.tv_nsec = 100000000;  //100 millseconds
			nanosleep(&TimeOut, NULL);
	/*	}*/

		pConn->SendPdu(&pdu);
	}
	catch (std::runtime_error msg)
	{
		log("parse json data failed.");
		char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
		pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
		pHttpConn->Close();
	}
}