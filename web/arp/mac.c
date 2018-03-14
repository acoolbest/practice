/* ************************************************************************
 *       Filename:  mac.c
 *    Description:  
 *        Version:  1.0
 *        Created:  2013年12月18日 10时44分10秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 gcc mac.c -o mac
 * ************************************************************************/


#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ether.h>
int lens = 0;
void IP_fun(unsigned char *buf)
{
	printf("PRO == %d\n", buf[23]);
	unsigned char src_ip[36] = "";
	unsigned char des_ip[36] = "";

	if(buf[23] == 1)
	{
		printf("agreement_type == ICMP\n");
	}
	if(buf[23] == 2)
	{
		printf("agreement_type == IGMP\n");
	}
	if(buf[23] == 6)
	{
		printf("agreement_type == TCP\n");
	}
	if(buf[23] == 17)
	{
		printf("agreement_type == UDP\n");
	}
	sprintf(src_ip, "%d.%d.%d.%d", buf[26], buf[27], buf[28], buf[29]);
	sprintf(des_ip, "%d.%d.%d.%d", buf[30], buf[31], buf[32], buf[33]);
	printf("src_ip == %s\ndes_ip == %s\n", src_ip, des_ip);
	printf("user_data == ");
	fflush(stdout);
	int i = 0;
	for(i = 32*6 - 1; i < lens; i++)
	{
		printf("%02x ", buf[i]);
	}
	printf("\n");
	int head_len = buf[14]&0x0f;
	unsigned char src_port[5] = "";
	unsigned char des_port[5] = "";
	unsigned char udp_len[5] = "";
	unsigned char udp_check_sum[5] = "";

	if(head_len == 5)
	{
		if(buf[23] == 17)
		{
			unsigned short port_src = ntohs(*((unsigned short *)(buf + 34)));
			printf("===port_src==%d===\n", port_src);
			sprintf(src_port, "%02x%02x", buf[34], buf[35]);
			printf("src_port == %s\n", src_port);
			sprintf(des_port, "%02x%02x", buf[36], buf[37]);
			printf("des_port == %s\n", des_port);
			sprintf(udp_len, "%02x%02x", buf[38], buf[39]);
			printf("udp_len == %s\n", udp_len);
			sprintf(udp_check_sum, "%02x%02x", buf[40], buf[41]);
			printf("udp_check_sum == %s\n", udp_check_sum);
			printf("data == %s\n", buf + 42);
		}
		if(buf[23] == 6)
		{
			sprintf(src_port, "%02x%02x", buf[34], buf[35]);
			printf("src_port == %s\n", src_port);
			sprintf(des_port, "%02x%02x", buf[36], buf[37]);
			printf("des_port == %s\n", des_port);
			int tcp_head_len = buf[46]&0xf0;
			if(tcp_head_len == 0x50)
			{
				printf("data == %s\n", buf + 54);
			}

		}
	}
	else
	{
		if(buf[23] == 17)
		{
			sprintf(src_port, "%02x%02x", buf[38], buf[39]);
			printf("src_port == %s\n", src_port);
			sprintf(des_port, "%02x%02x", buf[40], buf[41]);
			printf("des_port == %s\n", des_port);
			sprintf(udp_len, "%02x%02x", buf[42], buf[43]);
			printf("udp_len == %s\n", udp_len);
			sprintf(udp_check_sum, "%02x%02x", buf[44], buf[45]);
			printf("udp_check_sum == %s\n", udp_check_sum);
			printf("data == %s\n", buf + 46);
		}
		if(buf[23] == 6)
		{
			sprintf(src_port, "%02x%02x", buf[38], buf[39]);
			printf("src_port == %s\n", src_port);
			sprintf(des_port, "%02x%02x", buf[40], buf[41]);
			printf("des_port == %s\n", des_port);
			int tcp_head_len = buf[46]&0xf0;
			if(tcp_head_len == 0x50)
			{
				printf("data == %s\n", buf + 58);
			}
			else
			{
				printf("data == %s\n", buf + 62);
			}

		}
	}

}

void ARP_fun(unsigned char *buf)
{
	printf("PRO == %d\n", buf[23]);
	printf("OPCODE == %d\n", buf[21]);
	if(buf[21] == 1)
	{
		printf("arp request\n");
	}
	if(buf[21] == 2)
	{
		printf("arp response\n");
	}
	if(buf[21] == 3)
	{
		printf("rarp request\n");
	}
	if(buf[21] == 4)
	{
		printf("rarp response\n");
	}
	unsigned char src_ethernet_mac[18] = "";
	unsigned char src_ip[36] = "";
	unsigned char target_ethernet_mac[18] = "";
	unsigned char target_ip[36] ="";
	sprintf(src_ethernet_mac, "%02x:%02x:%02x:%02x:%02x:%02x",\
				buf[22], buf[23], buf[24], buf[25], buf[26], buf[27]);
	sprintf(target_ethernet_mac, "%02x:%02x:%02x:%02x:%02x:%02x",\
				buf[32], buf[33], buf[34], buf[35], buf[36], buf[37]);
	sprintf(src_ip, "%d.%d.%d.%d", buf[28], buf[29], buf[30], buf[31]);
	sprintf(target_ip, "%d.%d.%d.%d", buf[38], buf[39], buf[40], buf[41]);
	printf("src_ethernet_mac == %s\n\
target_ethernet_mac == %s\n\
src_ip == %s\n\
target_ip == %s\n",\
			src_ethernet_mac,\
			target_ethernet_mac,\
			src_ip,\
			target_ip);
}

void RARP_fun(unsigned char *buf)
{

}
typedef struct
{
	unsigned char type[5];
	void (*fun)(unsigned char *buf);
}FUN;
FUN fun_list[] = {
		{"0800", IP_fun},
		{"0806", ARP_fun},
		{"8035", RARP_fun}
};
void select_type(unsigned char *type, unsigned char *buf)
{
	int i = 0;
	for(i = 0; i < sizeof(fun_list)/sizeof(FUN); i++)
	{
		if(!strcmp(type, fun_list[i].type))
			fun_list[i].fun(buf);
	}

}
int main(int argc, char *argv[])
{
	unsigned char buf[1024] = "";
	int sock_raw_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	while(1)
	{
		unsigned char src_mac[18] = "";
		unsigned char des_mac[18] = "";
		lens = recvfrom(sock_raw_fd, buf, sizeof(buf), 0, NULL, NULL);
		sprintf(des_mac, "%02x:%02x:%02x:%02x:%02x:%02x",\
				buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
		sprintf(src_mac, "%02x:%02x:%02x:%02x:%02x:%02x",\
				buf[6], buf[7], buf[8], buf[9], buf[10], buf[11]);
		printf("MAC:%s >> %s\n", src_mac, des_mac);
		unsigned char type[5] = "";
		sprintf(type, "%02x%02x", buf[12], buf[13]);
		printf("type == %s\n", type);

		select_type(type, buf);
		printf("\n--------------------------------------------------------\n\n");
	}
	return 0;
}


