#include "RouteServConn.h"
//#include "DBServConn.h"
//#include "HttpConn.h"
//#include "HttpPdu.h"
#include "IM.Server.pb.h"
#include "IM.Other.pb.h"
#include "ImPduBase.h"

pthread_mutex_t SYNC::CRouteServConn::m_unLock = PTHREAD_MUTEX_INITIALIZER;

namespace SYNC
{
	static ConnMap_t g_route_server_conn_map;

	static serv_info_t* g_route_server_list;
	static uint32_t g_route_server_count;
	static CRouteServConn* g_master_rs_conn = NULL;


	//void route_server_conn_timer_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
	void* route_server_conn_timer_callback(void* callback_data)
	{
		pthread_detach(pthread_self());

		bool bRunning = true;
		struct timespec TimeOut;
		while(bRunning)
		{
			//sleep(1);//added by george on April 17th,2016
			memset(&TimeOut, 0, sizeof(TimeOut));
			TimeOut.tv_sec = 1;
			nanosleep(&TimeOut, NULL);

			ConnMap_t::iterator it_old;
			CRouteServConn* pConn = NULL;
			uint64_t cur_time = get_tick_count();

			for (ConnMap_t::iterator it = g_route_server_conn_map.begin(); it != g_route_server_conn_map.end();)
			{
				it_old = it;
				it++;

				pConn = (CRouteServConn*)it_old->second;

				if(pConn)
				{
					pConn->Lock();
					pConn->OnTimer(cur_time);
					pConn->Unlock();
				}
			}

			// reconnect RouteServer
			serv_check_reconnect<CRouteServConn>(g_route_server_list, g_route_server_count);
			set_route_serv_conn();
		}
	}

	void init_route_serv_conn(serv_info_t* server_list, uint32_t server_count)
	{
		g_route_server_list = server_list;
		g_route_server_count = server_count;

		serv_init<CRouteServConn>(g_route_server_list, g_route_server_count);

		//netlib_register_timer(route_server_conn_timer_callback, NULL, 1000);

		//create a thread to send heart beat packets. Added by george on April 17th,2016
		pthread_t unThreadId = 0;
		pthread_create(&unThreadId,NULL, route_server_conn_timer_callback, NULL);
	}

	bool is_route_server_available()
	{
		CRouteServConn* pConn = NULL;

		for (uint32_t i = 0; i < g_route_server_count; i++) {
			pConn = (CRouteServConn*)g_route_server_list[i].serv_conn;
			if (pConn && pConn->IsOpen()) {
				return true;
			}
		}

		return false;
	}

	void send_to_all_route_server(CImPdu* pPdu)
	{
		CRouteServConn* pConn = NULL;

		for (uint32_t i = 0; i < g_route_server_count; i++) {
			pConn = (CRouteServConn*)g_route_server_list[i].serv_conn;
			if (pConn && pConn->IsOpen()) {
				pConn->SendPdu(pPdu);
			}
		}
	}

	// get the oldest route server connection
	CRouteServConn* get_route_serv_conn()
	{
		return g_master_rs_conn;
	}

	void update_master_route_serv_conn()
	{
		uint64_t oldest_connect_time = (uint64_t)-1;
		CRouteServConn* pOldestConn = NULL;

		CRouteServConn* pConn = NULL;

		for (uint32_t i = 0; i < g_route_server_count; i++) {
			pConn = (CRouteServConn*)g_route_server_list[i].serv_conn;
			if (pConn && pConn->IsOpen() && (pConn->GetConnectTime() < oldest_connect_time)){
				pOldestConn = pConn;
				oldest_connect_time = pConn->GetConnectTime();
			}
		}

		g_master_rs_conn = pOldestConn;

		if (g_master_rs_conn)
		{
			IM::Server::IMRoleSet msg;
			msg.set_master(1);
			CImPdu pdu;
			pdu.SetPBMsg(&msg);
			pdu.SetServiceId(IM::BaseDefine::SID_OTHER);
			pdu.SetCommandId(IM::BaseDefine::CID_OTHER_ROLE_SET);
			g_master_rs_conn->SendPdu(&pdu);
		}
	}

	//set route server connection
	void set_route_serv_conn(void)
	{
		if (g_master_rs_conn == NULL)
		{
			update_master_route_serv_conn();
		}
	}


	CRouteServConn::CRouteServConn()
	{
		m_bOpen = false;
		m_serv_idx = 0;
	}

	CRouteServConn::~CRouteServConn()
	{

	}

	void CRouteServConn::Connect(const char* server_ip, uint16_t server_port, uint32_t idx)
	{
		log("Connecting to RouteServer %s:%d ", server_ip, server_port);

		m_serv_idx = idx;
		m_handle = netlib_connect(server_ip, server_port, imconn_callback, (void*)&g_route_server_conn_map);

		if (m_handle != NETLIB_INVALID_HANDLE)
		{
			g_route_server_conn_map.insert(make_pair(m_handle, this));
			//george added the following on April 16th,2016
			m_bOpen = true;
			m_connect_time = get_tick_count();
			struct timeval TimeOut;
			memset(&TimeOut, 0, sizeof(TimeOut));
			TimeOut.tv_sec = SERVER_TIMEOUT / 1000;
			setsockopt(m_handle, SOL_SOCKET, SO_RCVTIMEO, (struct timeval*)&TimeOut, sizeof(struct timeval));
		}
	}

	void CRouteServConn::Close()
	{
		serv_reset<CRouteServConn>(g_route_server_list, g_route_server_count, m_serv_idx);

		m_bOpen = false;
		if (m_handle != NETLIB_INVALID_HANDLE) {
			netlib_close(m_handle);
			g_route_server_conn_map.erase(m_handle);
		}

		ReleaseRef();

		if (g_master_rs_conn == this) {
			update_master_route_serv_conn();
		}
	}

	void CRouteServConn::OnConfirm()
	{
		log("connect to route server success ");
		m_bOpen = true;
		m_connect_time = get_tick_count();
		g_route_server_list[m_serv_idx].reconnect_cnt = MIN_RECONNECT_CNT / 2;

		if (g_master_rs_conn == NULL) {
			update_master_route_serv_conn();
		}

	}

	void CRouteServConn::OnClose()
	{
		log("onclose from route server handle=%d ", m_handle);
		Close();
	}

	void CRouteServConn::OnTimer(uint64_t curr_tick)
	{
		int nBytes = 0;

		if (curr_tick > m_last_send_tick + SERVER_HEARTBEAT_INTERVAL)
		{
			IM::Other::IMHeartBeat msg;
			CImPdu pdu;
			pdu.SetPBMsg(&msg);
			pdu.SetServiceId(IM::BaseDefine::SID_OTHER);
			pdu.SetCommandId(IM::BaseDefine::CID_OTHER_HEARTBEAT);
			nBytes = SendPdu(&pdu);
			if (nBytes <= 0)
			{
				log("route server is closed......");
				Close();
			}
			//log("sending heart beat................");
		}

		if (nBytes > 0)
		{
			nBytes = Recv();
			if (nBytes < 0)
			{
				log("conn to route server timeout");
				Close();
			}
			else if (nBytes == 0)
			{
				log("route server is closed");
				Close();
			}
		}

	/*	if (curr_tick > m_last_recv_tick + SERVER_TIMEOUT)
		{
			log("conn to route server timeout ");
			Close();
		}*/
	}

	void CRouteServConn::HandlePdu(CImPdu* pPdu)
	{
	}

	int CRouteServConn::Send(void* data, int len)
	{
		m_last_send_tick = get_tick_count();
	//	++g_send_pkt_cnt;

		if (m_busy)
		{
			m_out_buf.Write(data, len);
			return len;
		}

		int offset = 0;
		int remain = len;
		int ret = -1;
		while (remain > 0)
		{
			int send_size = remain;
			if (send_size > NETLIB_MAX_SOCKET_BUF_SIZE)
			{
				send_size = NETLIB_MAX_SOCKET_BUF_SIZE;
			}

			ret = send(m_handle, (char*)data + offset , send_size, 0);
			if (ret <= 0)
			{
				if (ret < 0 && (errno == EAGAIN || errno == EINTR))
				{
					continue;
				}
				else if (ret < 0)
				{
					log("failed to send data to route server: %s", strerror(errno));
					break;
				}
				else
				{
					ret = 0;
					break;
				}
			}

			offset += ret;
			remain -= ret;
		}

		return ret;
	/*	if (remain > 0)
		{
			m_out_buf.Write((char*)data + offset, remain);
			m_busy = true;
			log("send busy, remain=%d ", m_out_buf.GetWriteOffset());
		}
		else
		{
			OnWriteCompelete();
		}*/

		//return len;
	}

	int CRouteServConn::Recv(void)
	{
		char szBuffer[18];
		memset(szBuffer, 0, sizeof(szBuffer));
		int nBytes = 0;
		bool bContinue = true;
		while (bContinue)
		{
			nBytes = recv(m_handle, szBuffer, sizeof(szBuffer), 0);
			m_last_recv_tick = get_tick_count();
			if (nBytes < 0 && (errno == EAGAIN || errno == EINTR))
			{
				continue;
			}
			else
			{
				break;
			}
		}
		return nBytes;
	}

	void CRouteServConn::Lock(void)
	{
		pthread_mutex_lock(&m_unLock);
	}

	void CRouteServConn::Unlock(void)
	{
		pthread_mutex_unlock(&m_unLock);
	}
};
