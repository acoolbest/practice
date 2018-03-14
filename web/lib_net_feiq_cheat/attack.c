/* ************************************************************************
 *       Filename:  3_libpcap.c
 *    Description:  
 *        Version:  1.0
 *        Created:  2013年12月19日 23时50分21秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/
#include <pcap.h>
#include <libnet.h>

#include <arpa/inet.h>
#include <linux/if_ether.h>

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>

#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/ether.h>

#include <sys/ioctl.h>
#include <sys/socket.h>

#include <net/ethernet.h>
#include <netpacket/packet.h>	//sockaddr_ll sll


/*
struct ether_header
{
	unsigned char ether_dhost[6];
	unsigned char ether_shost[6];
	unsigned short ether_type;
};
*/
struct arphdr_2
{
	unsigned short int ar_hrd;
	unsigned short int ar_pro;
	unsigned char ar_hln;
	unsigned char ar_pln;
	unsigned short int ar_op;
	
	unsigned char ar_sha[6];
	unsigned short int ar_sip;
	unsigned char ar_tha[6];
	unsigned short int ar_tip;
};
char *p_net_interface_name = NULL;
#define BUFSIZE 1024
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
void ethernet_protocol_callback(unsigned char *argument,\
		const struct pcap_pkthdr *packet_header, const unsigned char *packet_content)
{
	unsigned char *p_mac_string;
	struct ether_header *ethernet_protocol;
	unsigned short ethernet_type;

	printf("----------------------------------------------------\n");
	printf("Capture a Packet from p_net_interface_name :%s\n", p_net_interface_name);
	printf("Capture Time is :%s", ctime((time_t *)&(packet_header->ts.tv_sec)));
	printf("Packet Length is :%d\n\n", packet_header->len);

	ethernet_protocol = (struct ether_header *)packet_content;
	p_mac_string = (unsigned char *)ethernet_protocol->ether_shost;
	printf("Mac Source Address is %02x:%02x:%02x:%02x:%02x:%02x\n",\
			*(p_mac_string+0), *(p_mac_string+1), *(p_mac_string+2),\
			*(p_mac_string+3), *(p_mac_string+4), *(p_mac_string+5));
/*	p_mac_string = (unsigned char *)ethernet_protocol->ether_dhost;
	printf("Mac Destination Address is %02x:%02x:%02x:%02x:%02x:%02x\n",\
			*(p_mac_string+0), *(p_mac_string+1), *(p_mac_string+2),\
			*(p_mac_string+3), *(p_mac_string+4), *(p_mac_string+5));
*/	
	unsigned char mac_buf[18] = "";
	mac_deal(mac_buf, ethernet_protocol->ether_dhost);
	printf("Mac Destination Address is %s\n", mac_buf);

	ethernet_type = ntohs(ethernet_protocol->ether_type);
	printf("Ethernet type is :%04x\n", ethernet_type);
	switch(ethernet_type)
	{
		case 0x0800:
			printf("The network layer is IP protocol\n");
			struct iphdr *ip_header = NULL;
			ip_header = (struct iphdr *)(ethernet_protocol + 1);
//			ip_header = (struct iphdr *)(packet_content + 14);
			unsigned char ip_src[16] = "";
			unsigned char ip_dst[16] = "";
			ip_deal(ip_src, ip_header->saddr);
			ip_deal(ip_dst, ip_header->daddr);
			printf("Src IP :%s --- Dst IP :%s\n", ip_src, ip_dst);
			unsigned char ip_type = ip_header->protocol;
			if(ip_type == 17)//UDP
			{
				struct udphdr *udph = NULL;
				udph = (struct udphdr*)(packet_content + 14 + ((ip_header->ihl)*4));
				printf("UDP Src PORT:%d --- Dst PORT:%d\n",\
						ntohs(udph->source), ntohs(udph->dest));
				if(udph->len != 8)
				{
					printf("message:%s\n",\
							packet_content + 14 + ((ip_header->ihl)*4) + 8);
				}
			}
			else if(ip_type == 6)
			{
				struct tcphdr *tcph = NULL;
				tcph = (struct tcphdr *)(packet_content + 14 +((ip_header->ihl)*4));
				printf("TCP Src PORT:%d --- Dst PORT:%d\n",\
						ntohs(tcph->source), ntohs(tcph->dest));
				if((tcph->doff)*4 == 20)
				{
					printf("message:%s\n",\
							packet_content + 14 +(ip_header->ihl)*4 + (tcph->doff)*4);
				}
			}
			else if(ip_type == 1)
			{
				printf("ICMP\n");
				struct icmphdr *icmph = NULL;
				icmph = (struct icmphdr *)(packet_content + 14 + (ip_header->ihl)*4);
				printf("ICMP_type:%d\n",icmph->type);
				if(icmph->type == 0)
				{
					printf("___%s___\n", packet_content + 14 + 20 + 4);
					printf("___%s___\n", packet_content + 14 + 20 + 8);
				}
				else if(icmph->type == 8)
				{
					printf("___%s___\n", packet_content + 14 + 20 + 4);
					printf("___%s___\n", packet_content + 14 + 20 + 8);
				}
			}
			break;
		case 0x0806:
			printf("The network layer is ARP protocol\n");
			struct arphdr_2 *arph = NULL;
			arph = (struct arphdr_2 *)(packet_content + 14);
			if(ntohs(arph->ar_op) == 1)
			{
				printf("ARP Ask!\n");
			}
			if(ntohs(arph->ar_op) == 2)
			{
				printf("ARP Answer!\n");
			}
			unsigned char src_ip[16] = "";
			unsigned char mac_src[18] = "";
			unsigned char dst_ip[16] = "";
			unsigned char mac_dst[18] = "";
			mac_deal(mac_src, arph->ar_sha);
			ip_deal(src_ip, arph->ar_sip);
			mac_deal(mac_dst, arph->ar_tha);
			ip_deal(dst_ip, arph->ar_tip);
			printf("Src MAC :%s --- Src IP :%s\n", mac_src, src_ip);
			printf("Dst MAC :%s --- Dst IP :%s\n", mac_dst, dst_ip);
			break;
		case 0x8035:
			printf("The network layer is RARP protocol\n");
			break;
		default:printf("The network layer unknow!\n");break;
	}
	
}
void *deal_send(void *arg)
{
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

	char *src_ip_str = "10.220.4.19";	//伪装者ip
	char *dst_ip_str = "10.220.4.31";	//目标ip
//	char *dst_ip_str = "10.220.4.156";
//	char *dst_ip_str = "10.220.4.255";
//	char *dst_ip_str = "10.220.4.19";
//	char *dst_ip_str = "10.220.4.101";
//	char *dst_ip_str = "10.220.4.13";
	unsigned long src_ip = 0, dst_ip = 0;
//1_lbt6_11#128#7427EA54CCE5#0#0#0#4000#9	吴险
//1_lbt6_31#128#1078D2CE5C75#0#0#0#4000#9	王猛

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

	lib_t = libnet_build_udp(2425, 2425, 8 + lens, 0, send_msg, lens, lib_net, 0);

	lib_t = libnet_build_ipv4(20 + 8 + lens, 0, 500, 0, 128, 17, 0,\
							src_ip, dst_ip, NULL, 0, lib_net, 0);

	lib_t = libnet_build_ethernet((u_int8_t *)dst_mac, (u_int8_t *)src_mac,\
								ETHERTYPE_IP, NULL, 0, lib_net, 0);
	int count = 0;							
	while(1)
	{
		libnet_write(lib_net);
		count++;
		printf("\n*****send %d times yet*****\n\n", count);
		sleep(3);
	}
	libnet_destroy(lib_net);

	return NULL;
}
void recieve(void)
{
	pcap_t *pcap_handle = NULL;
	char error_content[512] = "";
	unsigned char *p_packet_content = NULL;
	unsigned char *p_mac_string = NULL;
	unsigned short ethernet_type = 0;
//	char *p_net_interface_name = NULL;
	struct pcap_pkthdr protocol_header;
	struct ether_header *ethernet_protocol = NULL;
	struct bpf_program bpf_filter;
	bpf_u_int32 netp = 0, maskp = 0;
	char *bpf_filter_string = "arp or ip";

	p_net_interface_name = pcap_lookupdev(error_content);
	if(NULL == p_net_interface_name)
	{
		perror("pcap_lookupdev");
		exit(-1);
	}

	int ret = pcap_lookupnet(p_net_interface_name, &netp, &maskp, error_content);
	if(ret < 0)
	{
		perror("pcap_lookupnet");
		exit(-1);
	}

	pcap_handle = pcap_open_live(p_net_interface_name, BUFSIZE, 1, 0, error_content);
	
	if(pcap_compile(pcap_handle, &bpf_filter, bpf_filter_string, 0, maskp) < 0)
		perror("pcap_compile");	
	if(pcap_setfilter(pcap_handle, &bpf_filter) < 0)
		perror("pcap_setfilter");
	if(pcap_loop(pcap_handle, -1, ethernet_protocol_callback, NULL) < 0)
		perror("pcap_loop");
	pcap_close(pcap_handle);


}

char *help_print = "******************************************\n\
***				\n\
***				\n\
***				\n\
***				\n\
***				\n\
***				\n\
***				\n\
***				\n\
******************************************\n";
int main(int argc, char *argv[])
{
	printf("Please input your target ip, for example:10.220.4.31\n");
	char ip_buf[16] = "";
	fgets(ip_buf, sizeof(ip_buf), stdin);
	pthread_t tid;
	pthread_create(&tid, NULL, deal_send, NULL);
	pthread_detach(tid);
	
	recieve();

	return 0;
}

