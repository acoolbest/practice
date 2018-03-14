/* ************************************************************************
 *       Filename:  arp.c
 *    Description:  
 *        Version:  1.0
 *        Created:  2013年12月18日 19时22分40秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 g++ arp.cpp -o arp -lpthread -std=c++11
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
#include <pthread.h>

#include <stdint.h>

#include <iostream>
#include <map>
#include <fstream>


using namespace std;

#define SUCCESS			(0)
#define	FAIL			(1)

static map<string, string> 									g_ip_mac_map;
int 														sock_raw_fd = 0;
static pthread_mutex_t										g_ip_mac_mutex;

static uint32_t init_ip_mac_lock()
{
	return pthread_mutex_init(&g_ip_mac_mutex, NULL) == 0 ? SUCCESS : FAIL;
}

static uint32_t destory_ip_mac_lock()
{
	return pthread_mutex_destroy(&g_ip_mac_mutex) == 0 ? SUCCESS : FAIL;
}

static void enter_ip_mac_lock()
{
	pthread_mutex_lock(&g_ip_mac_mutex);
}

static void leave_ip_mac_lock()
{
	pthread_mutex_unlock(&g_ip_mac_mutex);
}

void update_ip_mac_map(string ip, string mac)
{
	enter_ip_mac_lock();
	g_ip_mac_map[ip] = mac;
	leave_ip_mac_lock();
}

void print_ip_mac_map()
{
	static int count = 1; // 当count写为cout时，会出现cout重载，若此时要用"cout"输出时就需要增加"std::"
	cout << "print ip mac map start!" << endl;
	enter_ip_mac_lock();
	printf("%d time print by map\n",count++);
	for(auto iter = g_ip_mac_map.begin(); iter != g_ip_mac_map.end(); iter++)
	{
		cout << "IP: " << iter->first << " --- MAC: " << iter->second << endl;
	}

	leave_ip_mac_lock();
}

uint32_t save_ip_mac_map_into_local_file()
{
	uint32_t ret = FAIL;
	cout << "save ip mac map into local file start!" << endl;
	enter_ip_mac_lock();
	ofstream out;
	/**
	ios::app：　　　以追加的方式打开文件
　　ios::ate：　　　文件打开后定位到文件尾，ios:app就包含有此属性
　　ios::binary：　以二进制方式打开文件，缺省的方式是文本方式。两种方式的区别见前文
　　ios::in：　　　 文件以输入方式打开(文件数据输入到内存)
　　ios::out：　　　文件以输出方式打开(内存数据输出到文件)
　　ios::nocreate： 不建立文件，所以文件不存在时打开失败
　　ios::noreplace：不覆盖文件，所以打开文件时如果文件存在失败
　　ios::trunc：　　如果文件存在，把文件长度设为0
	**/
	out.open("ip_mac_list_fstream.txt", ios::in|ios::out|ios::trunc);
	if (!out.is_open()) // if(!out.good())
	{
		cout << "Error opening file"; 
	}
	else
	{
		for(auto iter = g_ip_mac_map.begin(); iter != g_ip_mac_map.end(); iter++)
		{
			cout << "IP: " << iter->first << " --- MAC: " << iter->second <<endl;
			out << "IP: " << iter->first << " --- MAC: " << iter->second << "\n";
		}
		out.close();
		ret = SUCCESS;
	}

	leave_ip_mac_lock();
	return ret;
}

void * deal_recv_data(void * arg)
{
	// 5-----接受对方的ARP应答--------------------------------------
	uint8_t recv_msg[1024] = "";
	char ip[16] = "";
	char mac[18] = "";
	
	while(1)
	{
		memset(recv_msg, 0, sizeof(recv_msg));
		memset(ip, 0, sizeof(ip));
		memset(mac, 0, sizeof(mac));
		recvfrom(sock_raw_fd, recv_msg, sizeof(recv_msg), 0, NULL, NULL);
		if(recv_msg[21] == 2)					//ARP_answer
		{
			sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x",\
					recv_msg[22], recv_msg[23], recv_msg[24], recv_msg[25], recv_msg[26], recv_msg[27]);
			sprintf(ip, "%d.%d.%d.%d", recv_msg[28], recv_msg[29], recv_msg[30], recv_msg[31]);
			update_ip_mac_map(ip, mac);
		}
	}
	
	return NULL;
}

int main(int argc, char *argv[])
{
	// 1-----创建通信用典原始套接字------------------------------
	sock_raw_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if(sock_raw_fd < 0)
	{
		perror("socket");
		exit(-1);
	}
	
	// 2-----根据各种协议首部格式构建发送数据报-------------------
	unsigned char send_msg[1024] = {
		//-------------combination mac------ 0 ~ 13 -----------
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff,	//det_mac: ff:ff:ff:ff:ff:ff
		0x00, 0x50, 0xc2, 0x30, 0xe6, 0x84,	//src_mac: 00:0c:29:4d:0a:02
		0x08, 0x06,							//type: 0x0806 ARP argeement
		
		//-------------combination ARP------ 14 ~ 41 ----------
		0x00, 0x01, 0x08, 0x00,				//hardware_type: 1, agreement_type: 0x0800(IP)
		0x06, 0x04, 0x00, 0x01,				//hardware_length: 6, agreement_length: 4, option:(1:ARP_ask, 2:ARP_answer, 3:RARP_ask, 4:RARP_answer)
		0x00, 0x50, 0xc2, 0x30, 0xe6, 0x84,	//send_src_mac: 00:50:c2:30:e6:84
		192, 168, 0, 163,					//send_src_ip: 192.168.0.163
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	//target_mac: c8:9c:dc:ba:aa:ca
		192, 168, 0, 1,						//target_ip: 192.168.0.1
	};
	
	// 3-----数据初始化---------------------------------------------
	struct sockaddr_ll sll;					//barbarism sockfd addr
	struct ifreq ethreq;					//network interface addr
	strncpy(ethreq.ifr_name, "enp4s0", IFNAMSIZ);//appointed name for network card
	
	// 4-----将网络接口赋值给原始套接字地址结构---------------------
	if(-1 == ioctl(sock_raw_fd, SIOCGIFINDEX, (char *)&ethreq)) // 获得网卡序号存储于ethreq.ifr_ifindex中
	{
		perror("ioctl");
		close(sock_raw_fd);
		exit(-1);
	}
	bzero(&sll, sizeof(sll));
	sll.sll_ifindex = ethreq.ifr_ifindex;
	
	init_ip_mac_lock();
	pthread_t tid;
	pthread_create(&tid, NULL, deal_recv_data, NULL);
	pthread_detach(tid);
	
	int iTmp;
	for(iTmp = 1; iTmp < 255; iTmp++)
	{
		send_msg[41] = iTmp;
		sendto(sock_raw_fd, send_msg, 42, 0, (struct sockaddr *)&sll, sizeof(sll));
//		usleep(50*1000);
	}
	sleep(3);
	pthread_cancel(tid);
	
	save_ip_mac_map_into_local_file();
	print_ip_mac_map();
	destory_ip_mac_lock();
	return 0;
}


