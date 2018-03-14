/* ************************************************************************
 *       Filename:  feiq_cheat.c
 *    Description:  
 *        Version:  1.0
 *        Created:  2013��12��19�� 11ʱ24��46��
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/


#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ether.h>
#include <netpacket/packet.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <stdlib.h>
#include <strings.h>

/***********************************************************************************
˵����
1.����
	��һ������ΪҪ����У�����ݵ���ʼ��ַ
	
	�ڶ�������Ϊ����У�����ݵĸ���������
	һ��Ҫע������ط�Ҫ ����2������Ҫ����У������ݵĳ���Ϊ42,��ô�������Ӧ��Ϊ42/2
	
	return ΪС�ˣ��ڷ������ݱ���������Ҫת��Ϊ���
*************************************************************************************/
unsigned short checksum(unsigned short *buf, int nword)
{
	unsigned long sum;
	for(sum = 0; nword > 0; nword--)
	{
		sum += htons(*buf);
		buf++;
	}
	sum = (sum>>16) + (sum&0xffff);
	sum += (sum>>16);
	return ~sum;
}

int main(int argc, char *argv[])
{
	int sock_raw_fd = 0;
	sock_raw_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if(sock_raw_fd < 0)
	{
		perror("socket");
		exit(-1);
	}
	//IP: 10.220.4.13 ~ MAC: 10:78:d2:ce:5c:75
	unsigned char send_msg[1024] = {//IP: 10.220.4.156 ~ MAC: 74:27:ea:55:1f:d2
		//---combination mac---- 0 ~ 13 ----------
	//	0x10, 0x78, 0xd2, 0xce, 0x5c, 0x75,	//meng	13
		0xc8, 0x9c, 0xdc, 0xba, 0xaa, 0xca,	//det_mac 
		0x00, 0x0c, 0x29, 0x4d, 0x0a, 0x02,	//src_mac
		0x08, 0x00,							//type: 0x0800 IP
		
		//---combination ip---- 14 ~ 33 ----------
		0x45, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		128, 17, 0x00, 0x00,
		10, 220, 4, 32,
		10, 220, 4, 31,

		//---combination udp---- 34 ~ 41 ----------
		0x1f, 0x40, 0x09, 0x79,
		0x00, 0x00, 0x00, 0x00

	};
//	unsigned char buf_edition[] = "1_lbt6_31#128#1078D2CE5C75#0#0#0#4000#9";
							//	1_lbt6_68#128#C89CDCBAAACA#3373#59767#0#4000#9
	//��feiq���ݰ�
	unsigned char msg[1024] = "";
	int lens = sprintf(msg, "1_lbt6_31#128#1078D2CE5C75#0#0#0#4000#9:%d:%s:%s:%d:%s",\
						123, "�Ǻ�", "�Ǻ�", 32, "�Ǻ�");
	if((lens % 2) != 0)	//���ݲ���ż�����ֽڲ�0
	{
		lens = lens + 1;
		msg[lens] = 0;
	}
	printf("msg = %s\n", msg);
	printf("lens = %d\n", lens);
	memcpy(send_msg + 42, msg, lens);
	printf("send_msg + 42 = %s\n", send_msg + 42);
	
	//IP�ܳ��� = 33 - 14 + 1 + 8 + lens)	(�ײ��������ݵ��ܳ��ȣ���λΪ�ֽ�)
	*((unsigned short*)(send_msg + 16)) = htons(20 + 8 +lens);
	printf("%02x:%02x\n", send_msg[16], send_msg[17]);
	
	//UDP���� = (8 + lens)	(UDP�û����ݱ��ĳ��ȣ���Сֵ��8 <�����ײ�>)
	*((unsigned short*)(send_msg + 38)) = htons(8 + lens);
	printf("UDP_lens == %02x:%02x\n", send_msg[38], send_msg[39]);
	
	//ip_head_check_sum --- unsigned short checksum(unsigned short *buf, int nword)
	int nword = 20/2;//(send_msg[14]&0x0f)/2;
	unsigned short ip_head_check_sum = checksum((unsigned short *)&send_msg[14], nword);
	*((unsigned short*)(send_msg + 24)) = htons(ip_head_check_sum);
	printf("ip_head_check_sum == %02x:%02x\n", send_msg[24], send_msg[25]);
	
	//UDPαͷ��
	unsigned char udp_fake_head[1024] = {
		10, 220, 4, 32,
		10, 220, 4, 31,
		0, 17, 0, 0
	};
	*((unsigned short*)(udp_fake_head + 10)) = htons(8 + lens);
	memcpy(udp_fake_head + 12, send_msg + 34, 8 + lens);
	nword = (20 + lens)/2;
	unsigned short udp_head_check_sum = checksum((unsigned short *)udp_fake_head, nword);
	*((unsigned short*)(send_msg + 40)) = htons(udp_head_check_sum);
	
//ָ���������ݵı�������ӿ�-----------------------------
	struct ifreq ethreq;
	strncpy(ethreq.ifr_name, "eth0", IFNAMSIZ);
	if(-1 == ioctl(sock_raw_fd, SIOCGIFINDEX, &ethreq))
	{
		perror("ioctl");
		close(sock_raw_fd);
		exit(-1);
	}
	struct sockaddr_ll sll;
	bzero(&sll, sizeof(sll));
	sll.sll_ifindex = ethreq.ifr_ifindex;
	while(1)
	{	
		sendto(sock_raw_fd, send_msg, 42 + lens, 0, (struct sockaddr *)&sll, sizeof(sll));
		sleep(2);
	}
	int i = 0;
	for(i = 0; i < 42+lens; i++)
	{
		printf("%02x ", send_msg[i]);
	}
	printf("\n");
	printf("send success!\n");
	return 0;
}


