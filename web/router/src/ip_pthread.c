
#include "ip_pthread.h"
#include "arp_link.h"


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
10, 220, 4, 254,						//send_src_ip: 10.220.4.32
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	//target_mac: c8:9c:dc:ba:aa:ca
10, 220, 4, 31,						//target_ip: 10.220.4.31	
};



void ip_deal(char *buf, unsigned int ip)
{	
	sprintf(buf, "%d.%d.%d.%d", (ip>>8*0 & 0xff), (ip>>8*1 &0xff),\
		(ip>>8*2 & 0xff), (ip>>8*3 & 0xff));
}

void mac_deal(unsigned char *buf, unsigned char *mac)
{	
	sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x",\
		mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
}

void *deal_recv_ip(void *arg)
{
	//3-----数据初始化---------------------------------------------
	struct sockaddr_ll sll;					//barbarism sockfd addr
	struct ifreq ethreq;						//network interface addr	
	strncpy(ethreq.ifr_name, "eth0", IFNAMSIZ);//appointed name for network card		
	//4-----将网络接口赋值给原始套接字地址结构-------
	if(-1 == ioctl(sock_raw_fd, SIOCGIFINDEX, (char *)&ethreq))
	{
		perror("ioctl");
		close(sock_raw_fd);
		exit(-1);
	}	
	bzero(&sll, sizeof(sll));
	sll.sll_ifindex = ethreq.ifr_ifindex;

	
	unsigned char recv_msg[1024] = "";			//接收网络数据
	struct ether_header *ethernet_protocol;	//以太网头部
	unsigned short ethernet_type;				//协议类型
	
	struct iphdr *ip_header = NULL;
	unsigned char ip_dst[16] = "";
	LIST *pb;
	unsigned char det_mac_buf[6];
	unsigned char eth0_mac[];
	while(1)
	{
		int recv_len = recvfrom(sock_raw_fd, recv_msg, sizeof(recv_msg), 0, NULL, NULL);
		ethernet_protocol = (struct ether_header *)recv_msg;
		ethernet_type = ntohs(ethernet_protocol->ether_type);
		
		switch(ethernet_type)
		{
			case 0x0800:						//IP Protocol
				//过滤链表遍历、是否过滤
				ip_header = (struct iphdr *)(recv_msg + 14);
				ip_deal(ip_dst, ip_header->daddr);
				if(link_search_ip(s_ip_head, ip_dst) == NULL)
				{
					break;
				}
				if(link_search(head,ip_dst) == NULL)
				{
					
					
						sendto(sock_raw_fd, send_msg1, 42, 0, (struct sockaddr *)&sll, sizeof(sll));
						strncpy(ethreq.ifr_name, "eth1", IFNAMSIZ);//appointed name for network card
						if(-1 == ioctl(sock_raw_fd, SIOCGIFINDEX, (char *)&ethreq))
						{
							perror("ioctl");
							close(sock_raw_fd);
							exit(-1);
						}	
						bzero(&sll, sizeof(sll));
						sll.sll_ifindex = ethreq.ifr_ifindex;
						sendto(sock_raw_fd, send_msg2, 42, 0, (struct sockaddr *)&sll, sizeof(sll));
				}
				if(link_search(head,ip_dst) != NULL)
				{
					pb = link_search(head,ip_dst);
					printf("dst_mac == %s\n, pb->mac");
					
					
					sscanf(pb->mac, "%02x:%02x:%02x:%02x:%02x:%02x",\
						&det_mac_buf[0],&det_mac_buf[1],&det_mac_buf[2],\
						&det_mac_buf[3],&det_mac_buf[4],&det_mac_buf[5]);
					memcpy(ethernet_protocol->ether_dhost, det_mac_buf, 6);
					if((ip_header->daddr)&0xffffff00 == 0x0adc0400)//send to client	eth0
					{
						memcpy(ethernet_protocol->ether_shost, net_interface[1].mac, 6);
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
					if((ip_header->daddr)&0xffffff00 == 0x0adc0500)//send to A8	eth1
					{
						memcpy(ethernet_protocol->ether_shost, net_interface[2].mac, 6);
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
				}
				break;
			default:
				break;
		}
	}
	return NULL;
}
