


#include <pthread.h>

void *deal_recv_arp(void *arg)
{	
	unsigned char recv_msg[1024] = "";			//������������
	struct ether_header *ethernet_protocol; //��̫��ͷ��
	unsigned short ethernet_type;				//Э������
	struct arphdr_2 *arph;
	LIST *p_new=NULL;
	while(1)
	{
		recvfrom(sock_raw_fd, recv_msg, sizeof(recv_msg), 0, NULL, NULL);
		ethernet_protocol = (struct ether_header *)recv_msg;
		ethernet_type = ntohs(ethernet_protocol->ether_type);
		
		switch(ethernet_type)
		{
			case 0x0806:						//ARP Protocol
				//ARP	����ά��
				arph = (struct arphdr_2 *)(recv_msg + 14);
				p_new=(LIST*)malloc(sizeof(LIST));
				mac_deal(p_new->mac, arph->ar_sha);
				ip_deal(p_new->ip, arph->ar_sip);
				link_creat_end(&head,p_new);
				break;
			default:
				break;
		}
	}
	return NULL;
}


