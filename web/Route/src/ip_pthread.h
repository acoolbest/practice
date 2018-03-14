

#ifndef _IP_PTHREAD_H_
#define _IP_PTHREAD_H_
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <net/ethernet.h>
#include <netpacket/packet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include "get_interface.h"
#include <strings.h>
#include "main.h"
#include <netinet/udp.h>
#include <netinet/tcp.h>



/*
void ip_deal(unsigned char *buf, unsigned int ip);
void mac_deal(unsigned char *buf, unsigned char *mac);
*/
void *deal_send(void *arg);

void *deal_recv_ip(void *arg);
void *deal_recv(void *arg);

struct ip_pthread{
	int recv_len;
	unsigned char recv_msg[1024*2];
};






#endif



