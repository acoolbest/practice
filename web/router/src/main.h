
#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

typedef struct ip_mac_list{
	unsigned char ip[16];	
	unsigned char mac[18];	
	struct ip_mac_list *next;		//Ö¸ÕëÓò
}LIST;
LIST *head;

typedef struct shield_ip{
	unsigned char ip[16];
	struct shield_ip *next;		//Ö¸ÕëÓò
}S_IP;

typedef struct shield_mac{
	unsigned char mac[18];
	struct shield_mac *next;		//Ö¸ÕëÓò
}S_MAC;

typedef struct shield_tcp{
	unsigned short tcp_port;
	struct shield_tcp *next;		//Ö¸ÕëÓò
}S_TCP;

typedef struct shield_udp{
	unsigned short udp_port;
	struct shield_udp *next;		//Ö¸ÕëÓò
}S_UDP;

typedef struct shield_key{
	char keyword[1024];
	struct shield_key *next;		//Ö¸ÕëÓò
}S_KEY;

S_IP *s_ip_head;
S_MAC *s_mac_head;
S_TCP *s_tcp_head;
S_UDP *s_udp_head;
S_KEY *s_key_head;




#endif

