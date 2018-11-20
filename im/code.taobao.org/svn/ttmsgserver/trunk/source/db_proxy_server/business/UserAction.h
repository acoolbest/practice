/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *
 *   文件名称：UserAction.h
 *   创 建 者：Zhang Yuanhao
 *   邮    箱：bluefoxah@gmail.com
 *   创建日期：2014年12月15日
 *   描    述：
 *
 ================================================================*/

#ifndef __USER_ACTION_H__
#define __USER_ACTION_H__

#include "ImPduBase.h"

#define MAX_USER_COUNT_EACH_PACKET	10
#define MAX_USER_COUNT_EACH_PACKET_LITE	10

namespace DB_PROXY
{
    void getUserInfo(CImPdu* pPdu, uint32_t conn_uuid);
    void getChangedUser(CImPdu* pPdu, uint32_t conn_uuid);
	void UpdateUserNagure(CImPdu* pPdu, uint32_t conn_uuid);
	void UpdateUser(CImPdu* pPdu, uint32_t conn_uuid);
	void updateUserAvatar(CImPdu* pPdu, uint32_t conn_uuid);
	void setUserPush(CImPdu* pPdu, uint32_t conn_uuid);
	void getUserPush(CImPdu* pPdu, uint32_t conn_uuid);
	void getChangedUserLite(CImPdu* pPdu, uint32_t conn_uuid);
	void getUserInfoLite(CImPdu* pPdu, uint32_t conn_uuid);
	
	void GetChangedStaff(CImPdu* pPdu, uint32_t conn_uuid);
	void GetStaffInfo(CImPdu* pPdu, uint32_t conn_uuid);
	void GetChangedStaffLite(CImPdu* pPdu, uint32_t conn_uuid);
	void UpdateStaff(CImPdu* pPdu, uint32_t conn_uuid);
	void ChangeVip(CImPdu* pPdu, uint32_t conn_uuid);
	void GetAssistantInfo(CImPdu* pPdu, uint32_t conn_uuid);
	void QueryUserInfo(CImPdu* pPdu, uint32_t conn_uuid);


	//void getProduct(string& strProduct, uint32_t& unProduct);
};


#endif /* __USER_ACTION_H__ */
