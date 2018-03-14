
#include "ip_pthread.h"
#include "arp_link.h"
#include "ip_link.h"
#include "arp_pthread.h"

static unsigned char send_msg2[1024] = {
//-------------combination mac------ 0 ~ 13 -----------
0xff, 0xff, 0xff, 0xff, 0xff, 0xff,	//det_mac: ff:ff:ff:ff:ff:ff
0x00, 0x0c, 0x29, 0x4d, 0x0a, 0x0c,	//src_mac: 00:0c:29:4d:0a:02
0x08, 0x06,							//type: 0x0806 ARP argeement
//-------------combination ARP------ 14 ~ 41 ----------		
0x00, 0x01, 0x08, 0x00,				//hardware_type: 1, agreement_type: 0x0800(IP)
0x06, 0x04, 0x00, 0x01,				//hardware_length: 6, agreement_length: 4, option:(1:ARP_ask, 2:ARP_answer, 3:RARP_ask, 4:RARP_answer)
0x00, 0x0c, 0x29, 0x4d, 0x0a, 0x0c,	//send_src_mac: 00:0c:29:4d:0a:02
10, 220, 5, 254,						//send_src_ip: 10.220.4.32
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	//target_mac: c8:9c:dc:ba:aa:ca
10, 220, 5, 37,						//target_ip: 10.220.4.31	
};
static unsigned char send_msg1[1024] = {
//-------------combination mac------ 0 ~ 13 -----------
0xff, 0xff, 0xff, 0xff, 0xff, 0xff,	//det_mac: ff:ff:ff:ff:ff:ff
0x00, 0x0c, 0x29, 0x4d, 0x0a, 0x02,	//src_mac: 00:0c:29:4d:0a:02
0x08, 0x06,							//type: 0x0806 ARP argeement
//-------------combination ARP------ 14 ~ 41 ----------		
0x00, 0x01, 0x08, 0x00,				//hardware_type: 1, agreement_type: 0x0800(IP)
0x06, 0x04, 0x00, 0x01,				//hardware_length: 6, agreement_length: 4, option:(1:ARP_ask, 2:ARP_answer, 3:RARP_ask, 4:RARP_answer)
0x00, 0x0c, 0x29, 0x4d, 0x0a, 0x02,	//send_src_mac: 00:0c:29:4d:0a:02
10, 220, 4, 254,					//send_src_ip: 10.220.4.32
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	//target_mac: c8:9c:dc:ba:aa:ca
10, 220, 4, 31,						//target_ip: 10.220.4.31	
};

/*
void ip_deal(unsigned char *buf, unsigned int ip)
{	
	sprintf(buf, "%d.%d.%d.%d", (ip>>8*0 & 0xff), (ip>>8*1 &0xff),\
		(ip>>8*2 & 0xff), (ip>>8*3 & 0xff));
}

void mac_deal(unsigned char *buf, unsigned char *mac)
{	
	sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x",\
		mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
}
*/
void *deal_send(void *arg)
{
	int sock_raw_fd0 = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	int sock_raw_fd1 = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	char ip_dst[16] = "";
	memcpy(ip_dst, (char *)arg, 16);
//	printf("link_search(head,ip_dst) == NULL\tip_dst == %s\n", ip_dst);
	
	struct sockaddr_ll sll0, sll1;						//barbarism sockfd addr
	struct ifreq ethreq0, ethreq1;						//network interface addr	

	strncpy(ethreq0.ifr_name, "eth0", IFNAMSIZ);	//appointed name for network card
	if(-1 == ioctl(sock_raw_fd0, SIOCGIFINDEX, (char *)&ethreq0))
	{
		perror("ioctl");
		close(sock_raw_fd0);
		exit(-1);
	}
	bzero(&sll0, sizeof(sll0));
	sll0.sll_ifindex = ethreq0.ifr_ifindex;

	strncpy(ethreq1.ifr_name, "eth1", IFNAMSIZ);//appointed name for network card
	if(-1 == ioctl(sock_raw_fd1, SIOCGIFINDEX, (char *)&ethreq1))
	{
		perror("ioctl");
		close(sock_raw_fd1);
		exit(-1);
	}
	bzero(&sll1, sizeof(sll1));
	sll1.sll_ifindex = ethreq1.ifr_ifindex;
	
	int iTmp;
	for(iTmp = 1; iTmp < 255; iTmp++)
	{		
		send_msg1[41] = iTmp;
		send_msg2[41] = iTmp;
		int ret = sendto(sock_raw_fd0, send_msg1, 42, 0, (struct sockaddr *)&sll0, sizeof(sll0));
		if(ret < 0)
		{
			perror("sendto_eth0");
		}
		ret = sendto(sock_raw_fd1, send_msg2, 42, 0, (struct sockaddr *)&sll1, sizeof(sll1));
		if(ret < 0)
		{
			perror("sendto_eth1");
		}
	}
	
/*	int ret = sendto(sock_raw_fd, send_msg1, 42, 0, (struct sockaddr *)&sll, sizeof(sll));
	if(ret < 0)
	{
		perror("sendto");
	}
	*/
	
/*	for(iTmp = 1; iTmp < 255; iTmp++)
	{		
		send_msg2[41] = iTmp;
		int ret = sendto(sock_raw_fd1, send_msg2, 42, 0, (struct sockaddr *)&sll1, sizeof(sll1));
		if(ret < 0)
		{
			perror("sendto");
		}
	}
*/	
/*	ret = sendto(sock_raw_fd, send_msg2, 42, 0, (struct sockaddr *)&sll, sizeof(sll));
	if(ret < 0)
	{
		perror("sendto");
	}
	*/
	return NULL;
}
void *deal_recv_ip(void *arg)
{
	struct ip_pthread *ip_pth = (struct ip_pthread *)arg;
	unsigned char recv_msg[1024*1024] = "";
	int recv_len = 0;
	recv_len = ip_pth->recv_len;
//	printf("recv_len == %d\n", recv_len);
	bzero(recv_msg, sizeof(recv_msg));
	memcpy(recv_msg, ip_pth->recv_msg, recv_len);
	
	struct iphdr *ip_header = NULL;
	ip_header = (struct iphdr *)(recv_msg + 14);
	unsigned char ip_type = ip_header->protocol;
	int type = (int)ip_type;
//	printf("type == %d\n", type);
	if(NULL != link_search_type(s_type_head, type))
	{
		printf("tcp/udp shielded!\n");
		return NULL;
	}
	if(ip_type == 17)//UDP
	{				
		struct udphdr *udph = NULL;
		udph = (struct udphdr*)(recv_msg + 14 + ((ip_header->ihl)*4));
//		printf("UDP Src PORT:%d --- Dst PORT:%d\n", ntohs(udph->source), ntohs(udph->dest));
		if(NULL != link_search_udp(s_udp_head, ntohs(udph->dest)))//过滤目的udp_port
		{
			printf("udp_port shielded!\n");
			return NULL;
		}
		if(udph->len != 8)				
		{				
//			printf("message:%s\n", recv_msg + 14 + ((ip_header->ihl)*4) + 8);
			if(link_search_key(s_key_head, recv_msg + 14 + ((ip_header->ihl)*4) + 8))
			{
				printf("keyword shielded!\n");//过滤udp关键字
				return NULL;
			}

		}			
	}			
	if(ip_type == 6)
	{	
		struct tcphdr *tcph = NULL;
		tcph = (struct tcphdr *)(recv_msg + 14 +((ip_header->ihl)*4));
//		printf("TCP Src PORT:%d --- Dst PORT:%d\n", ntohs(tcph->source), ntohs(tcph->dest));
		if(NULL != link_search_tcp(s_tcp_head, ntohs(tcph->source)))//过滤源tcp_port
		{
			printf("tcp_port shielded!\n");
			return NULL;
		}
		if((tcph->doff)*4 == 20)
		{	
//			printf("message:%s\n",recv_msg + 14 +(ip_header->ihl)*4 + (tcph->doff)*4);
			if(link_search_key(s_key_head, recv_msg + 14 +(ip_header->ihl)*4 + (tcph->doff)*4))
			{
				printf("keyword shielded!\n");//过滤tcp关键字
				return NULL;
			}

		}		
	}

	
	LIST *pb;
	unsigned int det_mac_buf[6];
//	unsigned int src_mac_buf[6];
	int i = 0;
	struct sockaddr_ll sll;						//barbarism sockfd addr
	struct ifreq ethreq;						//network interface addr	
	
	char ip_dst[16] = "";
	char ip_src[16] = "";
	sprintf(ip_dst, "%d.%d.%d.%d",\
		recv_msg[30], recv_msg[31], recv_msg[32], recv_msg[33]);
	sprintf(ip_src, "%d.%d.%d.%d",\
		recv_msg[26], recv_msg[27], recv_msg[28], recv_msg[29]);
//	printf("\n------------------------------\nip_dst:%s----ip_src:%s\n", ip_dst, ip_src);
	if(link_search_ip(s_ip_head, ip_src) != NULL)//过滤源ip
	{
		printf("IP shielded!\n");
		return NULL;
	}
	if(head == NULL)
	{	
		pthread_t tid;
		pthread_create(&tid, NULL, deal_send, (void*)ip_dst);
		pthread_join(tid, NULL);
//		printf("-----------------arp_link_start----------------------\n");
//		link_print(head);
//		printf("------------------arp_link_end----------------------\n");
	}
/*	if(0 == strcmp(ip_dst, "10.220.4.255"))
		return NULL;
	if(0 == strcmp(ip_dst, "10.220.5.255"))
		return NULL;
	if(0 == strcmp(ip_dst, "10.220.0.11"))
		return NULL;
	
*/	
	

	if((0 != strcmp(ip_dst, "10.220.4.31"))&&(0 != strcmp(ip_dst, "10.220.5.37"))\
		&&(0 != strcmp(ip_dst, "10.220.4.254"))&&(0 != strcmp(ip_dst, "10.220.5.254")))
		return NULL;

	pb = link_search(head,ip_dst);
	if(link_search_mac(s_mac_head,pb->mac))//过滤目的mac
	{
		printf("MAC shielded!\n");
		return NULL;
	}
//	printf("dst_pb->mac == %s\n", pb->mac);
	sscanf(pb->mac, "%02x:%02x:%02x:%02x:%02x:%02x",\
		&det_mac_buf[0],&det_mac_buf[1],&det_mac_buf[2],\
		&det_mac_buf[3],&det_mac_buf[4],&det_mac_buf[5]);
	
	if(((*(int*)(recv_msg+30))&(0x00ffffff)) == 0x0004dc0a)//send to client	eth0
//		if(((ip_header->daddr)&(0x00ffffff)) == 0x0004dc0a)//send to client	eth0
	{
//		printf("send to client\n");
/*			pb = link_search(head,"10.220.4.254");
		printf("10.220.4.254->mac == %s\n", pb->mac);
		bzero(src_mac_buf, sizeof(src_mac_buf));
		sscanf(pb->mac, "%02x:%02x:%02x:%02x:%02x:%02x",\
		&src_mac_buf[0],&src_mac_buf[1],&src_mac_buf[2],\
		&src_mac_buf[3],&src_mac_buf[4],&src_mac_buf[5]);
*/			
		unsigned char src_mac_buf0[6] = {0x00, 0x0c, 0x29, 0x4d, 0x0a, 0x02};
		for(i = 0; i < 6; i++)
		{
			recv_msg[i] = det_mac_buf[i];
			recv_msg[i+6] = src_mac_buf0[i];
		}
/*		printf("change---dst_mac:%02x:%02x:%02x:%02x:%02x:%02x\tsrc_mac:%02x:%02x:%02x:%02x:%02x:%02x\n",\
			recv_msg[0], recv_msg[1], recv_msg[2], recv_msg[3], recv_msg[4], recv_msg[5],\
			recv_msg[6], recv_msg[7], recv_msg[8], recv_msg[9], recv_msg[10], recv_msg[11]);
*/
		strncpy(ethreq.ifr_name, "eth0", IFNAMSIZ);//appointed name for network card
		if(-1 == ioctl(sock_raw_fd, SIOCGIFINDEX, (char *)&ethreq))
		{
			perror("ioctl");
			close(sock_raw_fd);
			exit(-1);
		}	
		bzero(&sll, sizeof(sll));
		sll.sll_ifindex = ethreq.ifr_ifindex;
		sendto(sock_raw_fd, recv_msg, recv_len, 0, (struct sockaddr *)&sll, sizeof(sll));
	}
	if(((*(int*)(recv_msg+30))&(0x00ffffff)) == 0x0005dc0a)//send to A8	eth1
//		if(((ip_header->daddr)&(0x00ffffff)) == 0x0005dc0a)//send to A8	eth1
	{
//		printf("send to A8\n");
/*		pb = link_search(head,"10.220.5.254");
		bzero(src_mac_buf, sizeof(src_mac_buf));
		sscanf(pb->mac, "%02x:%02x:%02x:%02x:%02x:%02x",\
		&src_mac_buf[0],&src_mac_buf[1],&src_mac_buf[2],\
		&src_mac_buf[3],&src_mac_buf[4],&src_mac_buf[5]);
*/			
		unsigned char src_mac_buf1[6] = {0x00, 0x0c, 0x29, 0x4d, 0x0a, 0x0c};
		for(i = 0; i < 6; i++)
		{
			recv_msg[i] = det_mac_buf[i];
			recv_msg[i+6] = src_mac_buf1[i];
		}
/*		printf("change---dst_mac:%02x:%02x:%02x:%02x:%02x:%02x\tsrc_mac:%02x:%02x:%02x:%02x:%02x:%02x\n",\
			recv_msg[0], recv_msg[1], recv_msg[2], recv_msg[3], recv_msg[4], recv_msg[5],\
			recv_msg[6], recv_msg[7], recv_msg[8], recv_msg[9], recv_msg[10], recv_msg[11]);
*/
		strncpy(ethreq.ifr_name, "eth1", IFNAMSIZ);//appointed name for network card
		if(-1 == ioctl(sock_raw_fd, SIOCGIFINDEX, (char *)&ethreq))
		{
			perror("ioctl");
			close(sock_raw_fd);
			exit(-1);
		}	
		bzero(&sll, sizeof(sll));
		sll.sll_ifindex = ethreq.ifr_ifindex;
		sendto(sock_raw_fd, recv_msg, recv_len, 0, (struct sockaddr *)&sll, sizeof(sll));
	}

	return NULL;
}
void *deal_recv(void *arg)
{

	static unsigned char recv_msg[1024*1024] = "";			//接收网络数据
	int recv_len = 0;
	unsigned short ethernet_type;				//协议类型
	struct ip_pthread ip_pth;
	
//	pthread_t tid_ip;
//	pthread_t tid_recv_arp;
	while(1)
	{
		bzero(recv_msg, sizeof(recv_msg));
		recv_len = recvfrom(sock_raw_fd, recv_msg, sizeof(recv_msg), 0, NULL, NULL);
		if(recv_len < 0)
		{
			perror("recvfrom");
		}
		ethernet_type = ntohs(*(unsigned short *)(recv_msg+12));
		bzero(&ip_pth, sizeof(ip_pth));
		ip_pth.recv_len = recv_len;
		memcpy(ip_pth.recv_msg, recv_msg, recv_len);
/*		
		int i = 0;
		printf("in deal_recv\n");
		for(i = 0; i < recv_len; i++)
		{
			printf("%02x ", recv_msg[i]);
		}
		printf("\n");
*/
//		printf("ethernet_type == %04x\n", ethernet_type);
		switch(ethernet_type)
		{
			
			case 0x0800:						//IP Protocol
		//		printf("ethernet_type == %04x\n", ethernet_type);
				pool_add_task(&pool, deal_recv_ip, (void*)&ip_pth);
				//pthread_create(&tid_ip, NULL, deal_recv_ip, (void *)&ip_pth);
				//pthread_detach(tid_ip);
			
				break;
			case 0x0806:	
		//		printf("ethernet_type == %04x\n", ethernet_type);
				if(recv_msg[21] == 2)
				{

					unsigned char buff[1024*2] = "";
					memcpy(buff, recv_msg, 42);
					pool_add_task(&pool, deal_recv_arp, (void *)buff);
				//	pthread_create(&tid_recv_arp, NULL, deal_recv_arp, (void *)buff);
				//	pthread_detach(tid_recv_arp);
				}
				break;
			default:
				break;
		}
	}
	return NULL;
}
