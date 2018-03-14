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



#include <arpa/inet.h>
#include <time.h>
#include <pcap.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <linux/if_ether.h>
#include <net/if_arp.h>
#include <net/ethernet.h>
/*
struct ether_header
{
	unsigned char ether_dhost[6];
	unsigned char ether_shost[6];
	unsigned short ether_type;
};
*/
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
			struct arphdr *arph = NULL;
			arph = (struct arphdr *)(packet_content + 14);
			if(ntohs(arph->ar_op) == 1)
			{
				printf("ARP Ask!\n");
			}
			if(ntohs(arph->ar_op) == 2)
			{
				printf("ARP Answer!\n");
			}
		//	unsigned char ip_src[16] = "";
		//	unsigned char ip_dst[16] = "";
		//	ip_deal(ip_src, ip_header->saddr);
		//	ip_deal(ip_dst, ip_header->daddr);
		//	printf("Src IP :%s --- Dst IP :%s\n", ip_src, ip_dst);

			break;
		case 0x8035:
			printf("The network layer is RARP protocol\n");
			break;
		default:printf("The network layer unknow!\n");break;
	}
	
}
int main(int argc, char *argv[])
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
	return 0;
}

