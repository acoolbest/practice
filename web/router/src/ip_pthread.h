

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

struct arphdr_2{
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
void ip_deal(char *buf, unsigned int ip);
void mac_deal(unsigned char *buf, unsigned char *mac);
void *deal_recv(void *arg);







#endif



