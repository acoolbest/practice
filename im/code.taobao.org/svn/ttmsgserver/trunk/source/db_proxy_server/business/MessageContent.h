/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *
 *   文件名称：MessageContent.h
 *   创 建 者：Zhang Yuanhao
 *   邮    箱：bluefoxah@gmail.com
 *   创建日期：2014年12月15日
 *   描    述：
 *
 ================================================================*/

#ifndef MESSAGECOUTENT_H_
#define MESSAGECOUTENT_H_

#include "ImPduBase.h"

#define MESSAGE_CONTENT_INTERNAL_ERROR		0x0002
#define MESSAGE_CONTENT_SEND_TO_SELF_ERROR	0x0003
#define MESSAGE_CONTENT_SENDER_DEL_USER		0x0004
#define MESSAGE_CONTENT_SENDER_DISABLE_USER	0x0005
#define MESSAGE_CONTENT_SENDER_NONIM_USER	0x0006
#define MESSAGE_CONTENT_PEER_DEL_USER		0x0007
#define MESSAGE_CONTENT_PEER_DISABLE_USER	0x0008
#define MESSAGE_CONTENT_PEER_NONIM_USER		0x0009
#define MESSAGE_CONTENT_INVALID_GROUP_ID	0x000a
#define MESSAGE_CONTENT_INVALID_GROUP_USER	0x000b
#define MESSAGE_CONTENT_UNKNOWN_ERROR		0x000c
#define MESSAGE_CONTENT_FILE_IS_RECEIVED	0x000d
#define MESSAGE_CONTENT_INVALID_MSG_LEN		0x000e
#define MESSAGE_CONTENT_INVALID_MSG_TYPE	0x000f


namespace DB_PROXY {

    void getMessage(CImPdu* pPdu, uint32_t conn_uuid);

    void sendMessage(CImPdu* pPdu, uint32_t conn_uuid);
    
    void getMessageById(CImPdu* pPdu, uint32_t conn_uuid);
    
    void getLatestMsgId(CImPdu* pPdu, uint32_t conn_uuid);

	void getResendMessageById(CImPdu* pPdu, uint32_t conn_uuid);

	int GetResultCodeForSender(int nStatus);
	int GetResultCodeForPeer(int nStatus);
};

#endif /* MESSAGECOUTENT_H_ */
