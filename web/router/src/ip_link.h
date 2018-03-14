

#ifndef _IP_LINK_H_
#define _IP_LINK_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "main.h"
#include <string.h>

/******************************************************************
��	��:	��ȡ���������ĵ���Ϣ������
*******************************************************************/
void read_ipconfig(void);

/******************************************************************
��	��:	������������
*******************************************************************/
void link_creat_ip(S_IP **p_head,S_IP *p_new);
void link_creat_mac(S_MAC **p_head,S_MAC*p_new);
void link_creat_tcp(S_TCP **p_head,S_TCP *p_new);
void link_creat_udp(S_UDP **p_head,S_UDP *p_new);
void link_creat_key(S_KEY **p_head,S_KEY *p_new);
/******************************************************************
��	��:	�������ļ���ȡ����
*******************************************************************/
char * read_src_file(unsigned long int *file_length,char *src_file_name);

/******************************************************************
��	��:	��\r\n	�и�����
*******************************************************************/
int ipconfig_strtok(char *msg_src, char *msg_done[]);




#endif


