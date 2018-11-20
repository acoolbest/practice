/*
 * DBServConn.h
 *
 *  Created on: 2013-7-8
 *      Author: ziteng@mogujie.com
 */

#ifndef DBSERVCONN_H_
#define DBSERVCONN_H_

#include "imconn.h"
#include "ServInfo.h"
#include "RouteServConn.h"

class CDBServConn : public CImConn
{
public:
	CDBServConn();
	virtual ~CDBServConn();

	bool IsOpen() { return m_bOpen; }

	void Connect(const char* server_ip, uint16_t server_port, uint32_t serv_idx);
	virtual void Close();

	virtual void OnConfirm();
	virtual void OnClose();
	virtual void OnTimer(uint64_t curr_tick);

	virtual void HandlePdu(CImPdu* pPdu);
private:
	void _HandleValidateResponse(CImPdu* pPdu);
    void _HandleRecentSessionResponse(CImPdu* pPdu);
    void _HandleAllUserResponse(CImPdu* pPdu);
    void _HandleGetMsgListResponse(CImPdu* pPdu);
	void _HandleUpdateUserNagureResponse(CImPdu *pPdu);
	void _HandleUpdateUserResponse(CImPdu *pPdu);
    void _HandleGetMsgByIdResponse(CImPdu* pPdu);
    void _HandleMsgData(CImPdu* pPdu);
	void _HandleUnreadMsgCountResponse(CImPdu* pPdu);
    void _HandleGetLatestMsgIDRsp(CImPdu* pPdu);
	void _HandleDBWriteResponse(CImPdu* pPdu);
	void _HandleUsersInfoResponse(CImPdu* pPdu);
	void _HandleStopReceivePacket(CImPdu* pPdu);
	void _HandleRemoveSessionResponse(CImPdu* pPdu);
	void _HandleChangeAvatarResponse(CImPdu* pPdu);
    void _HandleSetDeviceTokenResponse(CImPdu* pPdu);
    void _HandleGetDeviceTokenResponse(CImPdu* pPdu);
    void _HandleDepartmentResponse(CImPdu* pPdu);
	void _HandleCompanyResponse(CImPdu* pPdu);
	void _HandleUserShieldUserResponse(CImPdu* pPdu);
	void _HandleUserGetShieldByUserResponse(CImPdu* pPdu);
	void _HandleResendMessageResponse(CImPdu* pPdu);
	void _HandlePositionResponse(CImPdu* pPdu);
	void _HandleAllUserLiteResponse(CImPdu* pPdu);
	void _HandleUserInfoLiteResponse(CImPdu* pPdu);
	void _HandleAllStaffResponse(CImPdu* pPdu);
	void _HandleAllStaffLiteResponse(CImPdu* pPdu);
	void _HandleStaffInfoResponse(CImPdu* pPdu);
	void _HandleUpdateStaffResponse(CImPdu* pPdu);
	void _HandleClientChangeVipResponse(CImPdu* pPdu);
	void _HandleClientAsstistantInfoResponse(CImPdu* pPdu);
	void _HandleClientQueryUserInfoResponse(CImPdu* pPdu);
	//void getProduct(string& strProduct, uint32_t& unProduct);
private:
	bool 		m_bOpen;
	uint32_t	m_serv_idx;
	void SendPacketToPushServer(string& strData, string& strJson, void* pMessage);
};

void init_db_serv_conn(serv_info_t* server_list, uint32_t server_count, uint32_t concur_conn_cnt);
CDBServConn* get_db_serv_conn_for_login();
CDBServConn* get_db_serv_conn();

#endif /* DBSERVCONN_H_ */
