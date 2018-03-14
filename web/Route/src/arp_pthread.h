

#ifndef _ARP_PTHREAD_H_
#define _ARP_PTHREAD_H_
#include "ip_pthread.h"
#include "main.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "arp_link.h"
#include <string.h>
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

void *deal_recv_arp(void *arg);

#endif



