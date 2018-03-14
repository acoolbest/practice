


#include "arp_pthread.h"

void *deal_recv_arp(void *arg)
{	
//	printf("-------------deal_recv_arp-------------\n");
	unsigned char recv_msg[1024*1024] = "";
	memcpy(recv_msg, (unsigned char *)arg, 42);			//接收网络数据
	LIST *p_new=NULL;
	char ip_addr[16] = "";
	char mac_addr[18] = "";
	char ip_src[16] = "";
	char mac_src[18] = "";
/*	int i = 0;
	printf("in deal_recv_arp\n");
	for(i = 0; i < 42; i++)
	{
		printf("%02x ", recv_msg[i]);
	}
	printf("\n");
*/
	sprintf(ip_addr, "%d.%d.%d.%d",\
		recv_msg[28], recv_msg[29], recv_msg[30], recv_msg[31]);
	sprintf(mac_addr, "%02x:%02x:%02x:%02x:%02x:%02x",\
		recv_msg[22], recv_msg[23], recv_msg[24],recv_msg[25], recv_msg[26], recv_msg[27]);
	
//	printf("ARP_ip:%s---ARP_mac:%s\n", ip_addr, mac_addr);

	sprintf(ip_src, "%d.%d.%d.%d",\
		recv_msg[38], recv_msg[39], recv_msg[40], recv_msg[41]);
	sprintf(mac_src, "%02x:%02x:%02x:%02x:%02x:%02x",\
		recv_msg[32], recv_msg[33], recv_msg[34],recv_msg[35], recv_msg[36], recv_msg[37]);
	if(link_search(head, ip_addr) == NULL)
	{
		p_new=(LIST*)malloc(sizeof(LIST));
		memcpy(p_new->ip, ip_addr, 16);
		memcpy(p_new->mac, mac_addr, 18);
		link_creat_end(&head, p_new);
	}
	if(link_search(head, ip_src) == NULL)
	{
		p_new=(LIST*)malloc(sizeof(LIST));
		memcpy(p_new->ip, ip_src, 16);
		memcpy(p_new->mac, mac_src, 18);
		link_creat_end(&head, p_new);
	}
	return NULL;
}


