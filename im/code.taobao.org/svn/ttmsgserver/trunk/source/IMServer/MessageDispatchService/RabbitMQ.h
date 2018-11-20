#ifndef HUAYUN_MESSAGE_DISPATCH_H
#define HUAYUN_MESSAGE_DISPATCH_H

#include "HttpClient.h"
#include "json/json.h"
#include "DBPool.h"
#include "ConfigFileReader.h"
#include "util.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <amqp.h>
#include <amqp_tcp_socket.h>
#include <amqp_framing.h>
#include "utils.h"

class CRabbitMQ
{
	public:
		CRabbitMQ();
		~CRabbitMQ();
		bool Init(const char* pszHostIP, int nPort);
		bool Login(const char* pszVirtualHost, int nFrameSize, const char* pszName, const char* pszPasswd);
		bool Preparing(int nInChannel, const char* pszInExchangeName, const char* pszInExchangeType, const char* pszInQueueName, const char* pszInRoutingKey);
		int RecvMessage(string& strBody);
		void Close(void);

	protected:
		amqp_socket_t *m_pSocket;
		amqp_connection_state_t m_nConnState;
		amqp_channel_t m_nChannel;
};

#endif