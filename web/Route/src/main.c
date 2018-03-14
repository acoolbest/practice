
#include "get_interface.h"
#include "main.h"
#include "ip_link.h"
#include "key_pthread.h"
#include "ip_pthread.h"
#include "arp_pthread.h"

LIST *head;
S_IP *s_ip_head;
S_MAC *s_mac_head;
S_TCP *s_tcp_head;
S_UDP *s_udp_head;
S_KEY *s_key_head;
S_TYPE *s_type_head;

int sock_raw_fd;
pthread_mutex_t mutex;
pool_t pool;
pthread_t tid_recv;
pthread_t tid_key;

int flag = 0;

int main(int argc, char *argv[])
{	
	while(1)
	{
		
		pool_init(&pool, 100);
		sock_raw_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
		pthread_mutex_init(&mutex,NULL);
		//读取过滤配置文档信息入链表
		read_ipconfig();		
		//获取接口信息	
		getinterface();
		//线程1		读取网络数据
		
		pthread_create(&tid_recv, NULL, deal_recv, NULL);
		pthread_detach(tid_recv);
		//线程2		读取键盘输入			
		flag = 0;
		pthread_create(&tid_key, NULL, deal_key, NULL);
		pthread_join(tid_key, NULL);		
		//等待线程结束	
		sleep(3);
	}
	return 0;
}

