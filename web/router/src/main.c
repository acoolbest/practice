
#include "get_interface.h"
#include "main.h"
#include "ip_link.h"
#include "key_pthread.h"
#include "ip_pthread.h"

LIST *head;
S_IP *s_ip_head;
S_MAC *s_mac_head;
S_TCP *s_tcp_head;
S_UDP *s_udp_head;
S_KEY *s_key_head;


int main(int argc, char *argv[])
{	
	//��ȡ���������ĵ���Ϣ������
	read_ipconfig();
	
	//��ȡ�ӿ���Ϣ	

	getinterface();
	

	//�߳�1		��ȡ��������	
	pthread_t tid_recv;
	pthread_create(&tid_recv, NULL, deal_recv, NULL);
	pthread_detach(tid_recv);

	//�߳�2		��ȡ��������	
	pthread_t tid_key;
	pthread_create(&tid_key, NULL, deal_key, NULL);
	pthread_join(tid_key, NULL);

	//�ȴ��߳̽���	
	



	return 0;
}

