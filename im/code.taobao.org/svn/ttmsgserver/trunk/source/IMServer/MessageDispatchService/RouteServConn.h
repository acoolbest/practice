#ifndef ROUTESERVCONN_H_
#define ROUTESERVCONN_H_

#include "imconn.h"
#include "ServInfo.h"
#include <pthread.h>

namespace SYNC
{

	class CRouteServConn : public CImConn
	{
		public:
			CRouteServConn();
			virtual ~CRouteServConn();

			bool IsOpen() { return m_bOpen; }
			uint64_t GetConnectTime() { return m_connect_time; }

			void Connect(const char* server_ip, uint16_t server_port, uint32_t serv_idx);
			virtual void Close();

			virtual void OnConfirm();
			virtual void OnClose();
			virtual void OnTimer(uint64_t curr_tick);

			virtual void HandlePdu(CImPdu* pPdu);
			int SendPdu(CImPdu* pPdu) { return Send(pPdu->GetBuffer(), pPdu->GetLength()); }
			void Lock(void);
			void Unlock(void);
			int Recv(void);
	
		private:
			bool 		m_bOpen;
			uint32_t	m_serv_idx;
			uint64_t	m_connect_time;
			int Send(void* data, int len);
			static pthread_mutex_t m_unLock;
	};

	void init_route_serv_conn(serv_info_t* server_list, uint32_t server_count);
	bool is_route_server_available();
	void send_to_all_route_server(CImPdu* pPdu);
	CRouteServConn* get_route_serv_conn();
	void set_route_serv_conn(void);
};

#endif /* ROUTESERVCONN_H_ */
