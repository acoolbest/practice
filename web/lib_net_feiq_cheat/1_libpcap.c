/* ************************************************************************
 *       Filename:  1_libpcap.c
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
struct ether_header
{
	unsigned char ether_dhost[6];
	unsigned char ether_shost[6];
	unsigned short ether_type;
};

#define BUFSIZE 1514
int main(int argc, char *argv[])
{
	pcap_t *pcap_handle = NULL;
	char error_content[100] = "";
	unsigned char *p_packet_content = NULL;
	unsigned char *p_mac_string = NULL;
	unsigned short ethernet_type = 0;
	char *p_net_interface_name = NULL;
	struct pcap_pkthdr protocol_header;
	struct ether_header *ethernet_protocol;

	p_net_interface_name = pcap_lookupdev(error_content);
	if(NULL == p_net_interface_name)
	{
		perror("pcap_lookupdev");
		exit(-1);
	}
	
	pcap_handle = pcap_open_live(p_net_interface_name, BUFSIZE, 1, 0, error_content);
	p_packet_content = pcap_next(pcap_handle, &protocol_header);
	
	printf("----------------------------------------------------\n");
	printf("Capture a Packet from p_net_interface_name :%s\n", p_net_interface_name);
	printf("Capture Time is :%s\n", ctime((const time_t *)&protocol_header.ts.tv_sec));
	printf("Packet Length is :%d\n", protocol_header.len);
	
	ethernet_protocol = (struct ether_header *)p_packet_content;
	p_mac_string = (unsigned char *)ethernet_protocol->ether_shost;
	printf("Mac Source Address is %02x:%02x:%02x:%02x:%02x:%02x\n",\
			*(p_mac_string+0), *(p_mac_string+1), *(p_mac_string+2),\
			*(p_mac_string+3), *(p_mac_string+4), *(p_mac_string+5));
	p_mac_string = (unsigned char *)ethernet_protocol->ether_dhost;
	printf("Mac Destination Address is %02x:%02x:%02x:%02x:%02x:%02x\n",\
			*(p_mac_string+0), *(p_mac_string+1), *(p_mac_string+2),\
			*(p_mac_string+3), *(p_mac_string+4), *(p_mac_string+5));
	
	ethernet_type = ntohs(ethernet_protocol->ether_type);
	printf("Ethernet type is :%04x\t", ethernet_type);
	switch(ethernet_type)
	{
		case 0x0800:printf("The network layer is IP protocol\n");break;
		case 0x0806:printf("The network layer is ARP protocol\n");break;
		case 0x8035:printf("The network layer is RARP protocol\n");break;
		default:printf("The network layer unknow!\n");break;
	}

	pcap_close(pcap_handle);
	return 0;
}

