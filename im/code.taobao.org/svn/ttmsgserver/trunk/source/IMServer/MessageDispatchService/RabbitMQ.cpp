#include "RabbitMQ.h"

CRabbitMQ::CRabbitMQ()
{
	m_pSocket = NULL;
	m_nConnState = 0;
	m_nChannel = 0;
}


CRabbitMQ::~CRabbitMQ()
{
	if (m_pSocket)
	{
		//delete m_pSocket;
		m_pSocket = NULL;
	}
	m_nConnState = 0;
	m_nChannel = 0;
}


bool CRabbitMQ::Init(const char* pszHostIP, int nPort)
{
	m_nConnState = amqp_new_connection();
	m_pSocket = amqp_tcp_socket_new(m_nConnState);
	if (!m_pSocket)
	{
		die("failed to creating a TCP socket");
	}

	int status = amqp_socket_open(m_pSocket, pszHostIP, nPort);
	if (status)
	{
		die("failed to opening a TCP socket");
	}

	return true;
}


bool CRabbitMQ::Login(const char* pszVirtualHost, int nFrameSize, const char* pszName, const char* pszPasswd)
{
	die_on_amqp_error(amqp_login(m_nConnState, pszVirtualHost, 0, nFrameSize, 0, AMQP_SASL_METHOD_PLAIN, pszName, pszPasswd), "Logging in");
	return true;
}


bool CRabbitMQ::Preparing(int nInChannel, const char* pszInExchangeName, const char* pszInExchangeType, const char* pszInQueueName, const char* pszInRoutingKey)
{
	m_nChannel = nInChannel;
	amqp_channel_open(m_nConnState, nInChannel);
	die_on_amqp_error(amqp_get_rpc_reply(m_nConnState), "Opening channel");

	amqp_exchange_declare(m_nConnState, nInChannel, amqp_cstring_bytes(pszInExchangeName), amqp_cstring_bytes(pszInExchangeType), 0, 1, 0, 0, amqp_empty_table);
	die_on_amqp_error(amqp_get_rpc_reply(m_nConnState), "Declaring exchange");

	amqp_queue_declare_ok_t *r = amqp_queue_declare(m_nConnState, nInChannel, amqp_cstring_bytes(pszInQueueName), 0, 1, 0, 0, amqp_empty_table);
	die_on_amqp_error(amqp_get_rpc_reply(m_nConnState), "Declaring queue");

	if (strcmp(pszInRoutingKey, "None") != 0)
	{
		amqp_queue_bind(m_nConnState, nInChannel, amqp_cstring_bytes(pszInQueueName), amqp_cstring_bytes(pszInExchangeName), amqp_cstring_bytes(pszInRoutingKey), amqp_empty_table);
	}
	else
	{
		amqp_queue_bind(m_nConnState, nInChannel, amqp_cstring_bytes(pszInQueueName), amqp_cstring_bytes(pszInExchangeName), amqp_empty_bytes, amqp_empty_table);
	}
	die_on_amqp_error(amqp_get_rpc_reply(m_nConnState), "Binding queue");

	//amqp_basic_consume(m_nConnState, nInChannel, amqp_cstring_bytes(pszInQueueName), amqp_empty_bytes, 0, 1, 0, amqp_empty_table);
	amqp_basic_consume(m_nConnState, nInChannel, amqp_cstring_bytes(pszInQueueName), amqp_empty_bytes, 0, 0, 0, amqp_empty_table);
	die_on_amqp_error(amqp_get_rpc_reply(m_nConnState), "Consuming");
	return true;
}


//int CRabbitMQ::RecvMessage(string& strBody)
//{
//	amqp_frame_t frame;
//	amqp_rpc_reply_t ret;
//	amqp_envelope_t envelope;
//	int nBodyLen = 0;
//
//	amqp_maybe_release_buffers(m_nConnState);
//	ret = amqp_consume_message(m_nConnState, &envelope, NULL, 0);
//
//	if (AMQP_RESPONSE_NORMAL != ret.reply_type)
//	{
//		if (AMQP_RESPONSE_LIBRARY_EXCEPTION == ret.reply_type && AMQP_STATUS_UNEXPECTED_STATE == ret.library_error)
//		{
//			if (AMQP_STATUS_OK != amqp_simple_wait_frame(m_nConnState, &frame))
//			{
//				log("failed to wait frame");
//				return -1;
//			}
//
//			if (AMQP_FRAME_METHOD == frame.frame_type)
//			{
//				switch (frame.payload.method.id)
//				{
//					case AMQP_BASIC_ACK_METHOD:
//						/* if we've turned publisher confirms on, and we've published a message
//						* here is a message being confirmed
//						*/
//						log("Basic ack method");
//						return -1;
//						break;
//					case AMQP_BASIC_RETURN_METHOD:
//						/* if a published message couldn't be routed and the mandatory flag was set
//						* this is what would be returned. The message then needs to be read.
//						*/
//						{
//							amqp_message_t message;
//							ret = amqp_read_message(m_nConnState, frame.channel, &message, 0);
//							if (AMQP_RESPONSE_NORMAL != ret.reply_type)
//							{
//								log("failed to read message");
//								
//							}
//							amqp_destroy_message(&message);
//							return -1;
//						}
//						break;
//
//					case AMQP_CHANNEL_CLOSE_METHOD:
//						/* a channel.close method happens when a channel exception occurs, this
//						* can happen by publishing to an exchange that doesn't exist for example
//						*
//						* In this case you would need to open another channel redeclare any queues
//						* that were declared auto-delete, and restart any consumers that were attached
//						* to the previous channel
//						*/
//						log("channel is closed..........");
//						return -1;
//						break;
//
//					case AMQP_CONNECTION_CLOSE_METHOD:
//						/* a connection.close method happens when a connection exception occurs,
//						* this can happen by trying to use a channel that isn't open for example.
//						*
//						* In this case the whole connection must be restarted.
//						*/
//						log("connection is closed...........");
//						return -1;
//						break;
//
//					default:
//						log("An unexpected method was received %d\n", frame.payload.method.id);
//						return -1;
//				}
//			}
//		}
//	}
//	else
//	{
//		nBodyLen = envelope.message.body.len;
//		char *messageBody = new char[nBodyLen + 1];
//		memset(messageBody, 0, nBodyLen + 1);
//		memcpy(messageBody, envelope.message.body.bytes, nBodyLen);
//		/*if (amqp_basic_ack(m_nConnState, m_nChannel, envelope.delivery_tag, false) > 0)
//		{
//			log("failing to send the ack to the broker\n");
//		}*/
//
//		strBody.append(messageBody);
//		free(messageBody);
//		amqp_destroy_envelope(&envelope);
//	}
//
//	return nBodyLen;
//}


int CRabbitMQ::RecvMessage(string& strBody)
{
	amqp_frame_t frame;
	amqp_rpc_reply_t ret;
	amqp_envelope_t envelope;
	int nBodyLen = -1;

	amqp_maybe_release_buffers(m_nConnState);
	ret = amqp_consume_message(m_nConnState, &envelope, NULL, 0);

	if (AMQP_RESPONSE_NORMAL != ret.reply_type)
	{
		//amqp_destroy_envelope(&envelope);
		log("failed to consume message:%d", ret.reply_type);
	}
	else
	{
		nBodyLen = envelope.message.body.len;
		if (nBodyLen > 0)
		{
			char *messageBody = new char[nBodyLen + 1];
			memset(messageBody, 0, nBodyLen + 1);
			memcpy(messageBody, envelope.message.body.bytes, nBodyLen);
			if (amqp_basic_ack(m_nConnState, m_nChannel, envelope.delivery_tag, false) > 0)
			{
				log("failing to send the ack to the broker\n");
			}

			strBody.append(messageBody);
			free(messageBody);
			amqp_destroy_envelope(&envelope);
		}
	}

	return nBodyLen;
}



void CRabbitMQ::Close(void)
{
	log("ready to close channel...");
	//die_on_amqp_error(amqp_channel_close(m_nConnState, m_nChannel, AMQP_REPLY_SUCCESS), "Closing channel");
	amqp_channel_close(m_nConnState, m_nChannel, AMQP_REPLY_SUCCESS);
	log("ready to close connection...");
	//die_on_amqp_error(amqp_connection_close(m_nConnState, AMQP_REPLY_SUCCESS), "Closing connection");
	amqp_connection_close(m_nConnState, AMQP_REPLY_SUCCESS);
	log("ready to destroy connection.......");
	//die_on_error(amqp_destroy_connection(m_nConnState), "Ending connection");
	amqp_destroy_connection(m_nConnState);
	if (m_pSocket)
	{
		//delete m_pSocket;
		m_pSocket = NULL;
	}
	m_nConnState = 0;
	m_nChannel = 0;
	log("close the mq");
}
