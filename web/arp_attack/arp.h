#ifndef _ARP_H_
#define _ARP_H_

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

typedef struct ip_mac_list
{
	char ip[16];
	char mac[18];
	struct ip_mac_list *next;		//÷∏’Î”Ú
}LIST;
typedef struct{
	char target_ip[16];
	char pretend_ip[16];
}TARGET;
TARGET tar_ip_buf;
LIST *head;

void link_creat_head(LIST **p_head,LIST *p_new);
void link_creat_end(LIST **p_head,LIST *p_new);
void link_print(LIST *head);
void link_save(LIST *head);
void *deal(void *arg);
void send_arp_ask(void);
LIST *link_search(LIST *head,char *ip);

#endif
